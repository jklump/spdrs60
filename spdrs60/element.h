/***************************************************************************
                           element.h
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2008 by Guido Scholz
    e-mail               : guido.scholz@bayernline.de
    last modified        : $Date: 2009-10-27 21:24:52 $
                           $Revision: 1.111 $
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
#include <qrect.h>
#include <qstring.h>

#if QT_VERSION >= 0x040000
#include <qmatrix.h>
#include <QMouseEvent>
#include <QPaintEvent>
#include <Q3TextStream>
#else
#include <qwmatrix.h>
#endif

#include "gbsbuttonstate.h"
#include "elementcommander.h"
#include "srcpmessage.h"
#include "graypanel.h"
#include "turntablecommander.h"




/* element recording types for start/stop signals, train number display
 * and normal elements*/
enum elemRecordType {
    krecNormal,
    krecStartStop,
    krecDisplay,
    krecClear,
    krecTrackIndicator
};

/*element route directions for 2D-routing*/
const unsigned int rdCenter = 0u;
const unsigned int rdN = 2u;
const unsigned int rdS = 4u;
const unsigned int rdW = 8u;
const unsigned int rdE = 16u;
const unsigned int rdNW = rdN | rdW;
const unsigned int rdNE = rdN | rdE;
const unsigned int rdSW = rdS | rdW;
const unsigned int rdSE = rdS | rdE;


// forward declaration
class element;


struct stateElement {
    unsigned int bus, address, state;
    element* elemPtr;
    QString name;
};

class element: public GrayPanel
{
    Q_OBJECT

public:
    element(QWidget* parent = NULL, SpdrItemClassId ci = siciTxt);
    element(QTextStream&, QWidget* parent=0, SpdrItemClassId ci = siciTxt);

    /*this variables should also be private*/
    QString  sSoldText;
    int      iSoldSubType;
    unsigned int routedtrack;

    void activateFfM(bool);
    void readFileTextFromStream(QTextStream&);
    void writeFileTextToStream(QTextStream&);
    unsigned int routeElement(unsigned int, bool);
    void locateMe();
    QString getLabelText() const;
    bool hasSameAddress(int, int);
    bool hasShuntingRouteButtonOnly();
    bool hasDifferentState(int);
    bool hasFfMLock();
    bool hasLEDsOn();
    bool is2StateDKW();
    bool isLocked();
    bool isLockable();
    bool isOccupied();
    bool isRoutable();
    bool isRouteMark();
    bool isSignal();
    bool isSimpleGA();
    bool isSwitchable();
    bool isSwitched();
    bool isTurnout();
    bool isTrainNumberDisplay();
    bool hasThreeStates();
    void showElementState(int, elemSelectionMode);
    bool showsStop();
    void sendSrcpState();
    bool sendSRCP08InitGA(unsigned int gano = 1);
    void setLocked(bool);
    void setSwitched(bool);
    void getStateData(stateElement& se);
    int getAddressCount();
    int getAddress1();
    int getAddress2();
    int getFBBusNo();
    int getGA1BusNo();
    int getGA2BusNo();
    void rotate();
    void toggle();
    bool ctxCanSwitch();
    void updateTrainNumber(unsigned int);
    unsigned int entryDir();
    void setTableLight(bool);
    void setDroppedFbContact(QByteArray&);
    bool canReceiveFbcDrop();
    bool hasLabel();
    bool hasTrackIndicator();
    bool hasVirtualAddress();
    bool hasVariants();
    bool hasFeedbackTrigger();
    int driveCount();
    int buttonCount();
    bool showLabelDialog();
    bool showTrackIndicatorDialog();
    bool showDriveDialog();
    bool showDualDriveDialog();
    bool showVirtualAddressDialog();
    bool showVariantDialog();
    bool showFeedbackTriggerDialog(const QPoint&);
    bool showTriggerHaltDialog();
    bool showTriggerClearDialog();

private:
    elementCommander* commander;

    unsigned int iFBBusNo;
    int iFBContact;
    /*button 1 trigger*/
    bool     enable1fbtrigger;
    unsigned int button1fbbus;
    unsigned int button1fbcontact;
    /*button 2 trigger*/
    bool     enable2fbtrigger;
    unsigned int button2fbbus;
    unsigned int button2fbcontact;
    /*halt aspect trigger*/
    bool     octriggerhalt;
    bool     retriggerclear;
    unsigned int fbtriggerhaltbus;
    unsigned int fbtriggerhaltcontact;
    /*clear aspect trigger*/
    bool     octriggerclear;
    bool     retriggerhalt;
    unsigned int fbtriggerclearbus;
    unsigned int fbtriggerclearcontact;

    unsigned int editsAddress;
    unsigned int countervalue;
    int      address1;
    int      address2;
    int      bus1;
    int      bus2;
    int      port1;
    int      port2;
    int      xchangeport1;
    int      xchangeport2;
    int      activetime1;
    int      activetime2;
    int      state;
    int      iSoldInvert;
    bool     trackindicatoroff;
    int      lockCounter;
    int      blinkcounter;
    int      lastdir;
    int      newdir;
    bool     ffm;
    bool     ffmactive;
    bool     occupied;
    bool     routable;
    bool     routed;
    bool     routemark;
    bool     signal;
    bool     simplega;
    bool     state2dkw;
    bool     switchable;
    bool     switched;
    bool     turnout;
    bool     lightson;
    bool     tablelight;
    SrcpMessage::Protocol protocol1;
    SrcpMessage::Protocol protocol2;
    QTimer*  locateTimer;

    void initVariables();
    void setLightsOn(bool);
    void switchToDirBlinking(int);
    void updateProperties();
    void setOccupied(bool);
    void setRouted(bool);
    void updateEDiTSAddress(unsigned int, bool);
    void updateFeedbackState();
    void switchAddress(bool);
    void switch2AddressItem(unsigned int, unsigned int);
    void runPropertyMenue(const QPoint&);

protected:  // virtual inherited methods
    void setupElementIcon();
    void addTooltip();
    void fontChange(const QFont&);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

public slots:
    void runTurnoutBlinkTimer();
    void repaintTimeOutEnk();
    void switchToDir(int);
    void slotOccupyElement(unsigned int, unsigned int, bool);
    void slotOccupyElement(unsigned int, unsigned int, bool,
            unsigned int);
    void slotShowElement(int, int, elemSelectionMode);

private slots:
    void slotLocateTimerTimeout();
    void slotUpdateCommanderData(int, int);
    void processInfoPortMessage(unsigned int bus,
            unsigned int addr, unsigned int port, unsigned int value);

signals:
    void cmdToDebug(const QString&);
    void elementClicked(int, GbsButtonState);
    void elementClicked(element*, GbsButtonState);
    void sendSrcpMessage(SrcpMessage*);
    void sigElementClickedRecord(int, int);
    void sigShowFBmodules();
    void turnoutIsSwitched();
    void recordElement(element*, elemRecordType);
};


#endif  //ELEMENT_H
