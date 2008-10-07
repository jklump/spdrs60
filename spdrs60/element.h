/***************************************************************************
                           element.h
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-10-07 17:34:43 $
                           $Revision: 1.102 $
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

// symbol names
// signals
static const char SYM_HS[]  = "signal_hs"; // Hauptsignal
static const char SYM_HSS[] = "signal_hss";// Hauptsperrsignal
static const char SYM_SS[]  = "signal_ss"; // Schutzhaltsignal
static const char SYM_SSH[] = "signal_ssh";// Schutzhaltsignal Falschfahrt
static const char SYM_SSS[] = "signal_sss";// Schutzhaltsignal zwei Gleistasten
static const char SYM_WS[]  = "signal_ws"; // Rangierhaltsignal (Ra 11)
static const char SYM_VS[]  = "signal_vs"; // Vorsignal
static const char SYM_ZP[]  = "signal_zp";
static const char SYM_NRB[] = "signal_nrb";// not really signals but rails
static const char SYM_SRB[] = "signal_srb";// with a routing button

// turnouts
static const char SYM_WEL[] = "weiche_links";//turnout left
static const char SYM_WER[] = "weiche_rechts";//turnout right
static const char SYM_DWL[] = "weiche_diag_links";//turnoutdiagonalleft
static const char SYM_DWR[] = "weiche_diag_rechts";//turnoutdiagonalright
static const char SYM_WEY[] = "weiche_y";
static const char SYM_DRW[] = "dreier_weiche";//3-way turnout
static const char SYM_EKL[] = "ekw_links"; // single-slip switch left
static const char SYM_EKR[] = "ekw_rechts";// single-slip switch right
static const char SYM_DKL[] = "dkw_links"; // double-slip switch left
static const char SYM_DKR[] = "dkw_rechts";// double-slip switch right

// straight tracks
static const char SYM_GER[] = "gerade";// straight track horizontal
static const char SYM_TRV[] = "trackvertical";
static const char SYM_DIL[] = "diagonale_links";
static const char SYM_DIR[] = "diagonale_rechts";
static const char SYM_TDR[] = "richtung_1";//track direction right
static const char SYM_TDB[] = "richtung_2";//track both directions
static const char SYM_KRH[] = "kreuzung_hose";  //crossing
static const char SYM_KRR[] = "kreuzung_rechts";//crossing right
static const char SYM_KRL[] = "kreuzung_links"; //crossing left
static const char SYM_GET[] = "gerade_tl"; // tunnel straight left/right (tug)
static const char SYM_TUL[] = "diagonale_links_tl";//tunnelbottomleft /top left
static const char SYM_TUR[] = "diagonale_rechts_tl";// right

// curved tracks
static const char SYM_KUR[] = "kurve_rechts";//curved track right bottom (KUR)
static const char SYM_KUL[] = "kurve_links"; //left top (KUL)
static const char SYM_TTL[] = "turn_topvert_left";
static const char SYM_TTR[] = "turn_topvert_right";
static const char SYM_TBL[] = "turn_botvert_left";
static const char SYM_TBR[] = "turn_botvert_right";

// miscellaneous
static const char SYM_ENK[] = "entkoppler";// decoupler (dco)
static const char SYM_BLD[] = "blind";     // blind item, switchable
static const char SYM_ADR[] = "adresse";   // trackaddressindicator
static const char SYM_BUE[] = "uebergang"; // level crossing (lcr)
static const char SYM_REL[] = "relais";
static const char SYM_MDC[] = "motor_dc";
static const char SYM_DRE[] = "drehscheibe";// turntable (tnt)
static const char SYM_SBN[] = "schiebebuehne";// transfer table (trt)

// decorative items
static const char SYM_PRE[] = "prellbock"; // buffer stop, bumper
static const char SYM_LEE[] = "leer";// (txt)
static const char SYM_BUC[] = "haus_1";
static const char SYM_BUL[] = "haus_2";
static const char SYM_SHO[] = "schuppen_o";// loco shed
static const char SYM_LSR[] = "schuppen_m";
static const char SYM_SHU[] = "schuppen_u";

// external group buttons
static const char SYM_TAF[] = "taste_fht";
static const char SYM_TAU[] = "taste_ufgt";// combination with MGT
static const char SYM_TAW[] = "taste_wgt";
static const char SYM_TAS[] = "taste_sgt"; // combination with HaGT

static const char SYM_FEG[] = "panel_green";// route group
static const char SYM_FEB[] = "panel_blue"; // turnout group
static const char SYM_FER[] = "panel_red";  // signal group
static const char SYM_FEY[] = "panel_yellow";// level crossing group
static const char SYM_FEE[] = "panel_grey"; // power supply
static const char SYM_FEN[] = "panel_brown";// axle counter



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
    ksmFoundEl
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
static const char GF_NAME[]      = "icon";
static const char GF_ROTATE[]    = "rotate";
static const char GF_INVERSTO[]  = "invers turnout";
static const char GF_DECODER[]   = "decoder";
static const char GF_PROTOCOL[]  = "protocol";
static const char GF_ADDRESS1[]  = "address_1";
static const char GF_ADDRESS2[]  = "address_2";
static const char GF_XCHCONN1[]  = "change conn 1";
static const char GF_XCHCONN2[]  = "change conn 2";
static const char GF_DIRECTION[] = "direction";
static const char GF_SUBTYPE[]   = "subtype";
static const char GF_TEXT[]      = "text";
static const char GF_ACTTIME[]   = "active time";
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
        siciSt1 = 100, siciSt2, siciSt3, siciSt4,
        siciCr1 = 120, siciCr2, siciCr3, siciCr4,
        siciCl1 = 130, siciCl2, siciCl3, siciCl4,
        siciKrh = 150, siciKr1, siciKr2, siciKl1, siciKl2, //Kr2, Kl2 not used
        siciTuh = 160, siciTuv, siciTul, siciTur,
        siciTdr = 170, siciTdl, siciTdb,
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
        siciTr1 = 500, siciTr2, siciTr3, siciTr4,
        siciTl1 = 510, siciTl2, siciTl3, siciTl4,
        siciIr1 = 520, siciIr2, siciIr3, siciIr4, //2, 4 not used
        siciIl1 = 530, siciIl2, siciIl3, siciIl4, //2, 4 not used
        siciSy1 = 550, siciSy2, siciSy3, siciSy4, //2, 4 not used
        siciSr1 = 600, siciSr2, siciSr3, siciSr4,
        siciSl1 = 610, siciSl2, siciSl3, siciSl4,
        siciDr1 = 620, siciDr2, siciDl1, siciDl2, //2 not used
        siciTw1 = 650, siciTw2, siciTw3, siciTw4, //2, 4 not used
        siciDre = 700, siciSbn,
        siciRel = 710,
        siciMdc = 720,
        siciBs1 = 800, siciBs2, siciBs3, siciBs4,
        siciLs1 = 820, siciLs2, siciLs3, siciLs4, //2, 4 not used
        siciLt1 = 830, siciLt2, siciLt3, siciLt4, //2, 4 not used
        siciLb1 = 840, siciLb2, siciLb3, siciLb4, //2, 4 not used
        siciBuc = 850, siciBul, siciBur,
        siciAdr = 900,
        siciEnk = 930,
        siciBld = 940,
        siciBue = 950,
        siciTxt = 1000,
        siciFeg = 1100, siciTaf, siciTau,
        siciFeb = 1200, siciTaw,
        siciFer = 1300, siciTas,
        siciFey = 1400,
        siciFen = 1500,
        siciFee = 1600, siciTal};
    
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

private:
    elementCommander*   turntableProperties;
    turntableCommander* ttComm;

    elemSelectionMode selectionMode;
    elemVisualMode visualMode;
    SpdrItemClassId classid;
    unsigned int iSoldIndex;
    unsigned int iFBBusNo;
    int iFBContact;
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
    int      activetime;
    int      state;
    int      iSoldInvert;
    int      trackindicatoroff;
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
    SrcpMessage::Protocol protocol;
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
    /*temporary functions to convert old file types*/
    element::SpdrItemClassId translateItem(QString&, bool);
    element::SpdrItemClassId translateRotatedItem(QString&);
    element::SpdrItemClassId translateNotRotatedItem(QString&);

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
    void slotUpdateTurntableData(QPoint&);
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
