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

#include "elementcommander.h"
#include "srcpmessage.h"
#include "spdrpanel.h"
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
    kWhtClicked,
    kUfgtClicked,
    kMgtClicked,
    kSgtClicked,
    kHagtClicked,
    kFrtClicked,
    kEinClicked,
    kAusClicked
};

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

/*some magic strings for reading and writing layout files*/
static const char GF_INVERSTO[]  = "invers turnout";
static const char GF_PROTOCOL[]  = "protocol";
static const char GF_PROTOCOL1[]  = "protocol1";
static const char GF_PROTOCOL2[]  = "protocol2";
static const char GF_ADDRESS1[]  = "address_1";
static const char GF_ADDRESS2[]  = "address_2";
static const char GF_XCHCONN1[]  = "change conn 1";
static const char GF_XCHCONN2[]  = "change conn 2";
static const char GF_DIRECTION[] = "direction";
static const char GF_SUBTYPE[]   = "subtype";
static const char GF_TEXT[]      = "text";
static const char GF_ACTTIME[]   = "active time";
static const char GF_ACTTIME1[]   = "active time1";
static const char GF_ACTTIME2[]   = "active time2";
static const char GF_FBPORT[]    = "feedback port";
static const char GF_HIDELEDS[]  = "hide LEDs";
static const char GF_BUTTON1FB[]  = "button1fb";
static const char GF_BUTTON2FB[]  = "button2fb";



// forward declaration
class element;


struct stateElement {
    unsigned int bus, address, state;
    element* elemPtr;
    QString name;
};

class element: public SpdrPanel
{
    Q_OBJECT

public:    
    element(QWidget* parent = NULL, SpdrItemClassId ci = siciTxt);
    element(QTextStream&, QWidget* parent=0);
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
    bool isModified();

private:
    elementCommander*   turntableProperties;
    turntableCommander* ttComm;

    unsigned int iFBBusNo;
    int iFBContact;
    bool     enable1fbtrigger;
    unsigned int button1fbbus;
    unsigned int button1fbcontact;
    bool     enable2fbtrigger;
    unsigned int button2fbbus;
    unsigned int button2fbcontact;
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
    bool     modified;
    SrcpMessage::Protocol protocol1;
    SrcpMessage::Protocol protocol2;
    QTimer*  locateTimer;

    void addTooltip();
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

public slots:
    void runTurnoutBlinkTimer();
    void repaintTimeOutEnk();
    void switchToDir(int);
    void slotOccupyElement(unsigned int, unsigned int, bool);
    void slotShowElement(int, int, elemSelectionMode);

private slots:
    void slotLocateTimerTimeout();
    void slotUpdateTurntableData(int, int);
    void slotCopyAvailTracks(const QString&);
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

protected:  // virtual inherited methods
    void fontChange(const QFont&);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void setupElementIcon();
};


#endif  //ELEMENT_H
