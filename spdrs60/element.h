/***************************************************************************
                           element.h
                           version 0.5.2 $Revision: 1.72 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-09-22 14:25:35 $
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
#include <qwmatrix.h>

#include "elementdialog.h"
#include "elementcommander.h"
#include "srcpmessage.h"
#include "turntablecommander.h"

// symbol names
// signals
#define SYM_HS   "signal_hs"
#define SYM_HSS  "signal_hss"
#define SYM_SS   "signal_ss"
#define SYM_SSH  "signal_ssh" 
#define SYM_SSS  "signal_sss" 
#define SYM_WS   "signal_ws"
#define SYM_VS   "signal_vs"
#define SYM_ZP   "signal_zp"
#define SYM_NRB  "signal_nrb" // not really signals but rails
#define SYM_SRB  "signal_srb" // with a routing button

// turnouts
#define SYM_WEL  "weiche_links" //turnout left
#define SYM_WER  "weiche_rechts" //turnout right
#define SYM_DWL  "weiche_diag_links" //turnoutdiagonalleft
#define SYM_DWR  "weiche_diag_rechts" //turnoutdiagonalright
#define SYM_WEY  "weiche_y"
#define SYM_DRW  "dreier_weiche" //3-way turnout
#define SYM_EKL  "ekw_links"  // single-slip switch left
#define SYM_EKR  "ekw_rechts" // single-slip switch right
#define SYM_DKL  "dkw_links"  // double-slip switch left
#define SYM_DKR  "dkw_rechts" // double-slip switch right

// straight tracks
#define SYM_GER  "gerade" // straight track horizontal
#define SYM_TRV  "trackvertical"
#define SYM_DIL  "diagonale_links"
#define SYM_DIR  "diagonale_rechts"
#define SYM_RI1  "richtung_1" //trackonedirection
#define SYM_RI2  "richtung_2" //tracktwodirections
#define SYM_KRH  "kreuzung_hose"   //crossing
#define SYM_KRR  "kreuzung_rechts" //crossing right
#define SYM_KRL  "kreuzung_links"  //crossing left

// curved tracks
#define SYM_KUR  "kurve_rechts" //curved track right
#define SYM_KUL  "kurve_links"
#define SYM_TTL  "turn_topvert_left"
#define SYM_TTR  "turn_topvert_right"
#define SYM_TBL  "turn_botvert_left"
#define SYM_TBR  "turn_botvert_right"

// miscellaneous
#define SYM_ENK  "entkoppler" // decoupler
#define SYM_BLD  "blind"      // blind item, switchable
#define SYM_ADR  "adresse"    //trackaddressindicator
#define SYM_BUE  "uebergang"  // level crossing
#define SYM_REL  "relais"
#define SYM_MDC  "motor_dc"
#define SYM_DRE  "drehscheibe" // turntable
#define SYM_SBN  "schiebebuehne" // transfer table

// decorative items
#define SYM_PRE  "prellbock"  // buffer stop, bumper
#define SYM_GET  "gerade_tl"  //tunnel straight left/right
#define SYM_DLT  "diagonale_links_tl" //tunnelbottomleft /top left
#define SYM_DRT  "diagonale_rechts_tl" // right
#define SYM_LEE  "leer"
#define SYM_HS1  "haus_1"
#define SYM_HS2  "haus_2"
#define SYM_SHO  "schuppen_o" // loco shed
#define SYM_SHM  "schuppen_m"
#define SYM_SHU  "schuppen_u"

// external group buttons
#define SYM_TAF  "taste_fht"
#define SYM_TAU  "taste_ufgt" // combination with MGT
#define SYM_TAW  "taste_wgt"
#define SYM_TAS  "taste_sgt"  // combination with HaGT

#define SYM_FEG  "panel_green" // route group
#define SYM_FEB  "panel_blue"  // turnout group
#define SYM_FER  "panel_red"   // signal group
#define SYM_FEY  "panel_yellow"// level crossing group
#define SYM_FEE  "panel_grey"  // power supply
#define SYM_FEN  "panel_brown" // axle counter



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
    ksmDisplay,
    ksmSwitchEl,
    ksmFoundEl
};

/*element visual modes, shown as colored right and bottom line*/
enum elemVisualMode {
    kvmNormal = 0,
    kvmEditLayout,
    kvmEditRoute
};

/* element recording types for start/stop signals, train number display
 * and normal elements*/
enum elemRecordType {
    krecNormal,
    krecStartStop,
    krecDisplay,
    krecClear
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
#define GF_INDEX      "index"
#define GF_CLASSID    "classid"
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
// TODO: adjust width to 55 (56 has no center)
#define EL_WIDTH      56     // width of an element in pixels (orig: 54 mm)
#define EL_HEIGHT     35     // height of an element in pixels (orig: 34 mm)
                             // 8 * H = 5 * W = 280
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
        siciGer = 100, siciTrv, siciDir, siciDil,
        siciKur = 120, siciKul,
        siciTtl = 125, siciTtr, siciTbl, siciTbr,
        siciKrh = 150, siciKrr, siciKrl,
        siciRi1 = 170,
        siciRi2 = 172,
        siciNrb = 200,
        siciSrb = 202,
        siciZp = 250,
        siciEnk = 252,
        siciBld = 255,
        siciHs = 300,
        siciHss = 310,
        siciSs = 320,
        siciSsh = 330,
        siciSss = 340,
        siciWs = 350,
        siciVs = 360,
        siciAdr = 400,
        siciBue = 420,
        siciWel = 500,
        siciWer = 502,
        siciDwl = 510,
        siciDwr = 512,
        siciWey = 550,
        siciEkl = 600,
        siciEkr = 602,
        siciDkl = 610,
        siciDkr = 612,
        siciDrw = 650,
        siciDre = 700, siciSbn,
        siciRel = 710,
        siciMdc = 720,
        siciPre = 800,
        siciShm = 820,
        siciSho = 822,
        siciShu = 824,
        siciDlt = 825,
        siciGet = 826,
        siciDrt = 827,
        siciHs1 = 830,
        siciHs2 = 831,
        siciLee = 900,
        siciFeg = 1100, siciTaf, siciTau,
        siciFeb = 1200, siciTaw,
        siciFer = 1300, siciTas,
        siciFey = 1400,
        siciFen = 1500,
        siciFee = 1600};
    
    element(QWidget* parent = NULL, SpdrItemClassId ci = siciLee,
            const char * si = SYM_LEE, elemVisualMode vm = kvmNormal);
    element(QTextStream&, QWidget* parent=0);


    /*this variables should also be private*/
    QString  sSoldIcon;
    QString  sSoldText;
    int      iSoldAddress_2;
    int      iSoldRotate;
    int      iSoldSubType;
    unsigned int routedtrack;

    void activateFfM(bool);
    void readFileTextFromStream(QTextStream&);
    void writeFileTextToStream(QTextStream&);
    QSize sizeHint() const;
    unsigned int routeElement(unsigned int, bool);
    void locateMe();
    QString getLabelText() const;
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
    bool isTrainNumberDisplay();
    bool hasThreeStates();
    void showElementState(int, elemSelectionMode);
    void showPropertyDlg();
    void sendSrcpState();
    bool sendSRCP08InitGA(unsigned int gano = 1);
    void setIndexNo(unsigned int);
    void setElementName(const QString&);
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
    bool isRotatable();
    bool ctxCanSwitch();
    void updateTrainNumber(unsigned int);
    int classId();
    void setClassId(SpdrItemClassId);

private:
    ElementDialog*      elementPropertyDlg;
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
    int      lastdir;
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
    bool     isright;
    QString  sSoldDecoder;
    SrcpMessage::Protocol protocol;
    QTimer*  locateTimer;

    void addTooltip();
    void clear();
    void initVariables();
    void setupElementIcon();
    void setLightsOn(bool);
    void switchToDirBlinking(int);
    void updateProperties();
    void updateLEDState();
    void setOccupied(bool);
    void setRouted(bool);
    void updateEDiTSAddress(unsigned int, bool);
    void updateFeedbackState();
    void switchAddress(bool);

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
    void slotUpdateTurntableData(QPoint);
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
