/***************************************************************************
                           element.h
                           version 0.5.0 $Revision: 1.47 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2006 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2006-10-31 18:20:41 $
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
#include "srcpmessage.h"
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

#define DS            ";"    // data separator in spdrs60 files
#define IDS           ":"    // data separator in imported files
#define XPM_SUFFIX    ".xpm" // file appendix for bitmap files
#define EL_WIDTH      56     // width of an element in pixels (orig: 54 mm)
#define EL_HEIGHT     35     // height of an element in pixels (orig: 34 mm)
                             // diagonale: 65.513 pixels (63.812)
                             // alpha: 31.264° (32.196°)
                             // beta: 58.736°  (57.804°)

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
    void locateMe();
    QString getName() const;
    bool hasSameAddress(int, int);
    bool hasShuntingRouteButtonOnly();
    bool hasDifferentDirection(int);
    bool hasFfMLock();
    bool hasLEDsOn();
    bool is2StateDKW();
    bool isEmpty();
    bool isLocked();
    bool isOccupied();
    bool isRoutable();
    bool isSignal();
    bool isSimpleGA();
    bool isSwitchable();
    bool isSwitched();
    bool isTurnout();
    bool hasThreeStates();
    void showElementState(int, elemSelectionMode);
    void sendSrcpState();
    bool sendSRCP08InitGA(unsigned int gano = 1);
    void setIndexNo(unsigned int);
    void setLocked(bool);
    void setSwitched(bool);
    unsigned int getIndexNo();
    void getStateData(stateElement& se);
    elemSelectionMode getSelectionMode();
    int getAddressCount();
    int getAddress1();
    int getAddress2();
    int getFBBusNo();
    int getGA1BusNo();
    int getGA2BusNo();

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
    unsigned int countervalue;
    int      iGA1BusNo;
    int      iGA2BusNo;
    int      port1;
    int      port2;
    int      iSoldActiveTime;
    int      iSoldAddress_1;
    int      iSoldChangeConn[2];
    int      iSoldDirection;
    int      iSoldInvert;
    int      iSoldLEDoff;
    int      iSoldLEDstate;
    int      lockCounter;
    int      blinkcounter;
    int      newdir;
    bool     ffm;
    bool     ffmactive;
    bool     occupied;
    bool     routable;
    bool     routed;
    bool     signal;
    bool     simplega;
    bool     state2dkw;
    bool     switchable;
    bool     switched;
    bool     turnout;
    bool     lightson;
    QString  sRepeatIcon;
    QString  sSoldDecoder;
    SrcpMessage::Protocol protocol;
    QString  sSaveReplaceIcon;
    QTimer*  locateTimer;

    void addTooltip();
    void clear();
    void createPopupMenus();
    void showPropertyDlg();
    void rotate();
    void setupElementIcon(int, QString);
    void setLightsOn(bool);
    void switchToDirBlinking(int);
    void updateProperties();
    void updateCtxNorm();
    void updateLEDState();
    void setOccupied(bool);
    void setRouted(bool);
    void updateEDiTSAddress(unsigned int, bool);
    void updateFeedbackState();

public slots:
    void runTurnoutBlinkTimer();
    void repaintTimeOutEnk();
    void switchToDir(int);
    void slotToggle();
    void slotOccupyElement(unsigned int, unsigned int, bool);
    void switchSelectionMode(elemSelectionMode);
    void switchVisualMode(elemVisualMode);
    void slotRepeatIcon(const QString&);
    void slotShowElement(int, int, elemSelectionMode);
    void slotRepaintLayout();

private slots:
    void slotLocateTimerTimeout();
    void slotUpdateTurntableData(QPoint);
    void slotCopyAvailTracks(const QString&);
    void slotCtxEdit(int);
    void processInfoPortMessage(unsigned int bus,
            unsigned int addr, unsigned int port, unsigned int value);

signals:
    void cmdToDebug(const QString&);
    void elementClicked(int, GbsButtonState);
    void elementClicked(element*, GbsButtonState);
    void sendSrcpMessage(SrcpMessage*);
    void setRepeatIcon(const QString&);
    void sigElementClickedRecord(int, int);
    void sigShowFBmodules();
    void turnoutIsSwitched();
    void recordElement(element*, elemRecordType);

protected:
    virtual void fontChange(const QFont&);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);
    virtual void paintEvent(QPaintEvent*);
};


#endif  //ELEMENT_H
