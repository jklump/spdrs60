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
#include <qtooltip.h>

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

/*element selection modes, shown as an inner rectangle*/
enum elemSelectionMode {
    ksmNormal = 0,
    ksmSelected,
    ksmStopSig,
    ksmStartSig,
    ksmDisplay,
    ksmSwitchEl,
    ksmFoundEl,
    ksmDropTarget
};

/*element visual modes, shown as colored right and bottom line*/
enum elemVisualMode {
    kvmNormal = 0,
    kvmEditLayout,
    kvmEditRoute,
    kvmEditClearance
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
static const char GF_INDEX[]     = "index";
static const char GF_CLASSID[]   = "classid";
static const char GF_INVERSTO[]  = "invers turnout";
static const char GF_DECODER[]   = "decoder";
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

static const char DS[]  = ";";   // data separator in spdrs60 files
static const char IDS[] = ":";   // data separator in imported files

// TODO: adjust width to 55 (56 has no center)
enum {
    EL_WIDTH  = 56, // width of an element in pixels (orig: 54 mm)
    EL_HEIGHT = 35  // height of an element in pixels (orig: 34 mm)
};                  // 8 * H = 5 * W = 280
                    // diagonal: 65.513 pixels (63.812)
                    // alpha: 31.264° (32.196°)
                    // beta: 58.736°  (57.804°)

// forward declaration
class element;


struct stateElement {
    unsigned int bus, address, state;
    element* elemPtr;
    QString name;
};

class element: public QWidget
{
    Q_OBJECT

public:
    enum SpdrItemClassId {
        siciNone = 0,
        siciSt1 = 100, siciSt2, siciSt3, siciSt4, // straight track
        siciCr1 = 120, siciCr2, siciCr3, siciCr4, // right curves
        siciCl1 = 130, siciCl2, siciCl3, siciCl4, // left curves
        siciKrh = 150, siciKr1, siciKr2, siciKl1, siciKl2, //Kr2, Kl2 not used
        siciTuh = 160, siciTuv, siciTul, siciTur, // bridges
        siciTdr = 170, siciTdl, siciTdb,          // arrows
        siciZt1 = 200, siciZt2, siciZt3, siciZt4, //2, 4 not used
        siciRt1 = 210, siciRt2, siciRt3, siciRt4, //2, 4 not used
        siciZp1 = 250, siciZp2, siciZp3, siciZp4, //2, 4 not used
        siciHs1 = 300, siciHs2, siciHs3, siciHs4, //2, 4 not used
        siciHv1 = 310, siciHv2, siciHv3, siciHv4, //Hs + Vs not used
        siciHss1 = 320, siciHss2, siciHss3, siciHss4,//2 + 4 not used
        siciHvs1 = 330, siciHvs2, siciHvs3, siciHvs4,//Hss + Vs ext. not used
        siciSs1 = 340, siciSs2, siciSs3, siciSs4, //2, 4 not used
        siciSh1 = 350, siciSh2, siciSh3, siciSh4, //2, 4 not used
        siciSd1 = 360, siciSd2, siciSd3, siciSd4, //2, 4 not used
        siciWs1 = 370, siciWs2, siciWs3, siciWs4, //2, 4 not used
        siciVs1 = 380, siciVs2, siciVs3, siciVs4, //2, 4 not used
        siciVx1 = 390, siciVx2, siciVx3, siciVx4, // Vs extension for Hss n. u.
        siciSb1 = 400, siciSb2, siciSb3, siciSb4, // Selbstblocksignal
        siciZb1 = 410, siciZb2, siciZb3, siciZb4, // Zentralblocksignal
        siciZv1 = 420, siciZv2, siciZv3, siciZv4, // Zentralblocksignal + Vs
        siciTr1 = 500, siciTr2, siciTr3, siciTr4, // right turnouts
        siciTl1 = 510, siciTl2, siciTl3, siciTl4, // left turnouts
        siciIr1 = 520, siciIr2, siciIr3, siciIr4, //2, 4 not used
        siciIl1 = 530, siciIl2, siciIl3, siciIl4, //2, 4 not used
        siciSy1 = 550, siciSy2, siciSy3, siciSy4, //2, 4 not used
        siciSr1 = 600, siciSr2, siciSr3, siciSr4,
        siciSl1 = 610, siciSl2, siciSl3, siciSl4,
        siciDr1 = 620, siciDr2, siciDl1, siciDl2, //2 not used
        siciTw1 = 650, siciTw2, siciTw3, siciTw4, //2, 4 not used
        siciDre = 700, siciSbn, // turntable, shiftbridge
        siciRel = 710, //relais
        siciMdc = 720, //DC motor
        siciBs1 = 800, siciBs2, siciBs3, siciBs4,
        siciLs1 = 820, siciLs2, siciLs3, siciLs4, //2, 4 not used
        siciLt1 = 830, siciLt2, siciLt3, siciLt4, //2, 4 not used
        siciLb1 = 840, siciLb2, siciLb3, siciLb4, //2, 4 not used
        siciBuc = 850, siciBul, siciBur,
        siciAdr = 900, // address indicator
        siciEnk = 930, // decoupler
        siciBld = 940, // blind element
        siciBue = 950, // level crossing
        siciTxt = 1000, // text only
        siciFeg = 1100, siciTaf, siciTau, // Green, FHT, UfGT
        siciFeb = 1200, siciTaw, siciTwh, // Blue, WGT, WHT
        siciFer = 1300, siciTas, // Red, SGT
        siciFey = 1400, // Yellow
        siciFen = 1500, // Brown
        siciFee = 1600, siciTal // Grey, Ein
    };
    
    element(QWidget* parent = NULL, SpdrItemClassId ci = siciTxt,
            elemVisualMode vm = kvmNormal);
    element(QTextStream&, QWidget* parent=0);


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
    bool showPropertyDlg();
    bool showsStop();
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
    void rotate();
    void toggle();
    bool ctxCanSwitch();
    void updateTrainNumber(unsigned int);
    element::SpdrItemClassId classId();
    unsigned int entryDir();
    void setTableLight(bool);
    void setDropTargetView(bool);
    void setDroppedFbContact(QByteArray&);
    bool canReceiveFbcDrop();
    bool hasLabel();
    bool hasTrackIndicator();
    bool hasVirtualAddress();
    bool hasVariants();
    int driveCount();
    int buttonCount();
    bool showLabelDialog();
    bool showTrackIndicatorDialog();
    bool showDriveDialog();
    bool showDualDriveDialog();
    bool showVirtualAddressDialog();
    bool showVariantDialog();
    bool showButtonDialog();
    bool showDualButtonDialog();

private:
    elementCommander*   turntableProperties;
    turntableCommander* ttComm;

    QPixmap background;
    elemSelectionMode selectionMode;
    elemVisualMode visualMode;
    SpdrItemClassId classid;
    unsigned int iSoldIndex;
    unsigned int iFBBusNo;
    int iFBContact;
    bool     enablefbtrigger;
    unsigned int button1fbbus;
    unsigned int button1fbcontact;
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
    QString  sSoldDecoder;
    SrcpMessage::Protocol protocol1;
    SrcpMessage::Protocol protocol2;
    QTimer*  locateTimer;

    void addTooltip();
    void initVariables();
    void setupElementIcon();
    void setLightsOn(bool);
    void switchToDirBlinking(int);
    void updateProperties();
    void setOccupied(bool);
    void setRouted(bool);
    void updateEDiTSAddress(unsigned int, bool);
    void updateFeedbackState();
    void switchAddress(bool);
    void switch2AddressItem(unsigned int, unsigned int);

public slots:
    void runTurnoutBlinkTimer();
    void repaintTimeOutEnk();
    void switchToDir(int);
    void slotOccupyElement(unsigned int, unsigned int, bool);
    void switchSelectionMode(elemSelectionMode);
    void switchVisualMode(elemVisualMode);
    void slotShowElement(int, int, elemSelectionMode);
    void slotRepaintLayout();

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
    void paintEvent(QPaintEvent*);
};


#endif  //ELEMENT_H
