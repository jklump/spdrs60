/***************************************************************************
                           element.h
                           version 0.4.8 $Revision: 1.30 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-12-23 17:48:05 $
****************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
   this is the header file to element.cpp
 ***************************************************************************/

#ifndef ELEMENT_H
#define ELEMENT_H

#include <qfont.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qpopupmenu.h>
#include <qrect.h>
#include <qstring.h>
#include <qtooltip.h>
#include <qwmatrix.h>

#include "resources.h"
#include "elementdialog.h"
#include "elementcommander.h"
#include "turntablecommander.h"


/* Click states of layout internal buttons (group key block, signals,
   turnouts) for every new key, add here a corresponding name */
enum GbsButtonState {
    kNoneClicked = 0,
    kTurnoutClicked,
    kZfsClicked,
    kZhsClicked,
    kRfsClicked,
    kFhtClicked,
    kWgtClicked,
    kUfgtClicked,
    kMgtClicked,
    kSgtClicked,
    kHagtClicked,
    kFrtClicked
};

/*element selection modes, shown as an inner rectangle*/
enum elemSelectionMode {
    ksmNormal = 0,
    ksmSelected,
    ksmStopSig,
    ksmStartSig,
    ksmSwitchEl,
    ksmFoundEl
};

/*element visual modes, shown as colored right and bottom line*/
enum elemVisualMode {
    kvmNormal = 0,
    kvmEditLayout,
    kvmEditRoute
};

/*element recording types for start/stop signals and normal elements*/
enum elemRecordType {
    krecNormal,
    krecStartStop,
    krecClear
};

/*some magic strings for reading and writing layout files*/
#define GF_INDEX      "index"
#define GF_NAME       "icon"
#define GF_ROTATE     "rotate"
#define GF_INVERSTO   "invers turnout"
#define GF_DECODER    "decoder"
#define GF_PROTOCOL   "protocol"
#define GF_ADDRESS1   "address_1"
#define GF_ADDRESS2   "address_2"
#define GF_XCHCONN1   "change conn 1"
#define GF_XCHCONN2   "change conn 2"
#define GF_DIRECTION  "direction"
#define GF_SUBTYPE    "subtype"
#define GF_TEXT       "text"
#define GF_ACTTIME    "active time"
#define GF_FBPORT     "feedback port"
#define GF_HIDELEDS   "hide LEDs"

#define DS            ";"     // data separator in spdrs60 files
#define IDS           ":"     // data separator in imported files
#define EL_WIDTH      56      // width of an element in pixels
#define EL_HEIGHT     35      // height of an element in pixels
#define XPM_SUFFIX    ".xpm"  // file appendix for bitmap files

// forward declaration
class element;


struct stateElement {
    unsigned int bus, address, state;
    element* elemPtr;
    element* elemPtr2;
    /* may be there should be an element list if gbs contains more than
     * one element with same address*/
    QString name;
};


class element: public QWidget
{
    Q_OBJECT

public:
    element(QWidget* parent=0);
    element(QTextStream&, QWidget* parent=0, bool isNewFormat = false);

    /*this variables should also be private*/
    QString  sSoldIcon;
    QString  sSoldText;
    int      iSoldAddress_2;
    int      iSoldRotate;
    int      iSoldSubType;

    void activateFfM(bool);
    void readFileTextFromStream(QTextStream&);
    void readOldFileTextFromStream(QTextStream&);
    void writeFileTextToStream(QTextStream&);
    QSize sizeHint() const;
    int  routeElement(int, bool, int);
    void sendState();
    void locateMe();
    QString getName() const;
    bool hasSameAddress(int);
    bool hasShuntingRouteButtonOnly();
    bool hasDifferentDirection(int);
    bool hasFfMLock();
    bool hasLEDsOn();
    bool isEmpty();
    bool isLocked();
    bool isOccupied();
    bool isSignal();
    bool isTurnout();
    bool isRoutable();
    bool isSwitchable();
    bool is2StateDKW();
    bool isSimpleGA();
    void showElementState(int, elemSelectionMode);
    void setIndexNo(unsigned int);
    void setLocked(bool);
    unsigned int getIndexNo();
    void getStateData(stateElement& se);
    elemSelectionMode getSelectionMode();
    int getAddressCount();

private:
    elementDialog*      elementPropertyDlg;
    elementCommander*   turntableProperties;
    turntableCommander* ttComm;

    QPopupMenu* ctxNorm;
    QPopupMenu* ctxEdit;
    elemSelectionMode selectionMode;
    elemVisualMode visualMode;
    unsigned int iSoldIndex;
    unsigned int iFBBusNo;
    unsigned int iFBContact;
    unsigned int editsAddress;
    int      iGA1BusNo;
    int      iGA2BusNo;
    int      iSoldActiveTime;
    int      iSoldAddress_1;
    int      iSoldChangeConn[2];
    int      iSoldDirection;
    int      iSoldInvert;
    int      iSoldLEDoff;
    int      iSoldLEDstate;
    int      lockCounter;
    bool     ffm;
    bool     ffmactive;
    bool     occupied;
    bool     routable;
    bool     routed;
    bool     signal;
    bool     state2dkw;
    bool     switchable;
    bool     turnout;
    bool     simplega;
    QString  sRepeatIcon;
    QString  sSoldDecoder;
    QString  sSoldProtocol;
    QString  sSaveReplaceIcon;
    QTimer*  locateTimer;

    void addTooltip();
    void clear();
    void createPopupMenus();
    void makeCommand();
    void showPropertyDlg();
    void rotate();
    void setupElementIcon(int, QString);
    void updateProperties();
    void updateCtxNorm();
    void updateLEDState();
    void setOccupied(bool);
    void setRouted(bool);
    void updateEDiTSAddress(unsigned int, bool);

public slots:
    void slotSwitchIt(int, int);
    void slotToggle();
    void slotOccupyElement(unsigned int, unsigned int, bool);
    void switchSelectionMode(elemSelectionMode);
    void switchVisualMode(elemVisualMode);
    void slotRepeatIcon(const QString&);
    void slotShowElement(int, int, elemSelectionMode);
    void slotRepaintLayout();
    void repaintTimeOutEnk();

private slots:
    void slotLocateTimerTimeout();
    void slotUpdateTurntableData(QPoint);
    void slotCopyAvailTracks(const QString&);
    void slotCtxEdit(int);
    void processInfoPortMessage(QString prot, int addr, int port,
                    int state);

signals:
    void cmdToDebug(const QString&);
    void elementClicked(int, GbsButtonState);
    void elementClicked(element*, GbsButtonState);
    void sendCommand(const QString&);
    void setRepeatIcon(const QString&);
    void sigElementClickedRecord(int, int);
    void sigShowFBmodules();
    void recordElement(element*, elemRecordType);

protected:
    virtual void fontChange(const QFont&);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);
    virtual void paintEvent(QPaintEvent*);
};


#endif  //ELEMENT_H
