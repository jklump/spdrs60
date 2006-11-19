/***************************************************************************
                           element.cpp
                           version 0.5.0 $Revision: 1.98 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2006 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2006-11-19 10:37:21 $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
   this file sets up the outfit of one element only and handles the sending
   of the switch command to the erddcd daemon
 ***************************************************************************/

#include <unistd.h>             // for usleep()
#include <stdio.h>              // for sprintf()

#include "element.h"
#include "preferences.h"
#include "resources.h"

/* popup menu icons */
#include "pixmaps/ctx_rota.xpm"
#include "pixmaps/ctx_straight.xpm"
#include "pixmaps/ctx_clear.xpm"
#include "pixmaps/ctx_l_curve.xpm"
#include "pixmaps/ctx_l_diag.xpm"
#include "pixmaps/ctx_l_turn.xpm"
#include "pixmaps/ctx_r_curve.xpm"
#include "pixmaps/ctx_r_diag.xpm"
#include "pixmaps/ctx_r_turn.xpm"

/* track buttons */
#include "pixmaps/button-red.xpm"
#include "pixmaps/button-gray.xpm"
#include "pixmaps/button-black.xpm"
#include "pixmaps/button-yellow.xpm"

/* labels for external group buttons */
#include "pixmaps/label-fht.xpm"
#include "pixmaps/label-hagt.xpm"
#include "pixmaps/label-mgt.xpm"
#include "pixmaps/label-sgt.xpm"
#include "pixmaps/label-ufgt.xpm"
#include "pixmaps/label-wgt.xpm"
#include "pixmaps/transfertable.xpm"
#include "pixmaps/signal-w.xpm"
#include "pixmaps/signal-wr.xpm"

static const char* leer_xpm[]={
"56 35 1 1",
". c None",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................",
"........................................................"};

// Element directions
#define   DIR_HP0          0
#define   DIR_HP1          1
#define   DIR_HP2          2
#define   DIR_SH1          3

#define   DIR_Sh0          0  // for shunt signals, main signal variants
#define   DIR_Sh1          1  // are not applicable

#define   DIR_ENK_DW       0  // decoupler off
#define   DIR_ENK_UP       1  // decoupler on

#define   DIR_REL0         0  // relay off
#define   DIR_REL1         1  // relay on

#define   DIR_0            0
#define   DIR_1            1

// constants for context menu
#define   CTX_ID_REP       901
#define   CTX_ID_TOGGLE    902
#define   CTX_ID_CLEAR     903
#define   CTX_ID_ROTATE    904

#define   LED_OFF          0   // LED states of an element = off
#define   LED_YEL          1   // route selected
#define   LED_RED          2   // occupied

// delay for edit mode after element locating
#define   LOCATE_TIMER     5000

// serd: Viessman Signale often need several attempts for reaching
// their correct position
#define   cNumRepeatCommands 3

#define SANGLE 31.264         // small angle
#define WANGLE (180.0 - SANGLE) // wide angle


element::element(QWidget* parent): QWidget(parent)
{
    initVariables();

    iSoldIndex = 0;
    sSoldIcon = SYM_LEE;
    iSoldRotate = -1;
    iSoldInvert = -1;
    sSoldDecoder = "-1";
    protocol = SrcpMessage::proNone;
    iSoldAddress_1 = -1;
    iSoldAddress_2 = -1;
    iSoldChangeConn[0] = -1;
    iSoldChangeConn[1] = -1;
    iSoldDirection = -1;
    iSoldSubType = -1;
    sSoldText = "-1"; // for test cases: "test"
    iSoldActiveTime = -1;
    iFBContact = 0;
    iSoldLEDoff = 1;

    createPopupMenus();
    updateProperties();
    updateContextMenus();
    setupElementIcon();
}
    

element::element(QTextStream& ats, QWidget* parent, bool isNewFormat)
: QWidget(parent)
{
    initVariables();

    if (isNewFormat)
        readFileTextFromStream(ats);
    else
        readOldFileTextFromStream(ats);

    createPopupMenus();
    updateProperties();
    updateContextMenus();
    setupElementIcon();
}


/*set all variables which are not read from file*/
void element::initVariables()
{
    editsAddress = 0;
    countervalue = 0;
    ffmactive = false;
    ffm = false;
    occupied = false;
    routable = false;
    routed = false;
    signal = false;
    state2dkw = false;
    switchable = false;
    switched = false;
    simplega = false;
    turnout = false;
    lightson = true;
    lastdir = 0;
    newdir = 0;
    iGA1BusNo = iGA2BusNo = iFBBusNo = 1;
    port1 = 1;
    port2 = 1;

    // this is only used for crossings to choose the routed track
    routedtrack = 0;

    setMaximumSize(sizeHint());
    setMinimumSize(sizeHint());
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed,
                false));
    selectionMode = ksmNormal;
    visualMode = kvmNormal;

    sRepeatIcon = SYM_LEE;
    lockCounter = 0;
    blinkcounter = 0;
    iSoldLEDstate = LED_OFF;

    elementPropertyDlg = NULL;
    turntableProperties = NULL;
    ttComm = NULL;
    ctxNorm = NULL;
    ctxEdit = NULL;
}


void element::readFileTextFromStream(QTextStream& ats)
{
    QString s, key, value;

    while (!ats.eof()) {
        s = ats.readLine();
        if (!s.startsWith("#")) {
            key = s.section(DS, 0, 0);
            value = s.section(DS, 1, 1).stripWhiteSpace();
            /* key/value pairs are read sequence independent */
            if (key.compare(GF_INDEX) == 0){
                  iSoldIndex = value.stripWhiteSpace().toUInt();
                  //fprintf(stderr, "New idx: %d  ", iSoldIndex);
            }
            else if (key.compare(GF_NAME) == 0){
                  sSoldIcon = value.stripWhiteSpace();
                  //fprintf(stderr, "New-Icon: %s\n", sSoldIcon.data());
            }
            else if (key.compare(GF_ROTATE) == 0){
                iSoldRotate = value.toInt();
            }
            else if (key.compare(GF_INVERSTO) == 0){
                iSoldInvert = value.toInt();
            }
            else if (key.compare(GF_DECODER) == 0){
                sSoldDecoder = value;
            }
            else if (key.compare(GF_PROTOCOL) == 0){
                if (value == "M")
                    protocol = SrcpMessage::proMM;
                else if (value == "N")
                    protocol = SrcpMessage::proDCC;
                else if (value == "P")
                    protocol = SrcpMessage::proServer;
                else if (value == "S")
                    protocol = SrcpMessage::proSelectrix;
                else
                    protocol = SrcpMessage::proNone;
            }
            else if (key.compare(GF_ADDRESS1) == 0){
                iGA1BusNo = value.toInt();
                value = s.section(DS, 2, 2).stripWhiteSpace();
                iSoldAddress_1 = value.toInt();
                value = s.section(DS, 3, 3).stripWhiteSpace();
                port1 = value.toUInt();
            }
            else if (key.compare(GF_ADDRESS2) == 0){
                iGA2BusNo = value.toInt();
                value = s.section(DS, 2, 2).stripWhiteSpace();
                iSoldAddress_2 = value.toInt();
                value = s.section(DS, 3, 3).stripWhiteSpace();
                port2 = value.toUInt();
            }
            else if (key.compare(GF_XCHCONN1) == 0){
                iSoldChangeConn[0] = value.toInt();
            }
            else if (key.compare(GF_XCHCONN2) == 0){
                iSoldChangeConn[1] = value.toInt();
            }
            else if (key.compare(GF_DIRECTION) == 0){
                iSoldDirection = value.toInt();
            }
            else if (key.compare(GF_SUBTYPE) == 0){
                iSoldSubType = value.toInt();
            }
            else if (key.compare(GF_TEXT) == 0){
                //sSoldText = value;
                sSoldText = s.section(DS, 1).stripWhiteSpace();
            }
            else if (key.compare(GF_ACTTIME) == 0){
                iSoldActiveTime = value.toInt();
            }
            else if (key.compare(GF_FBPORT) == 0){
                iFBBusNo = value.toUInt();
                value = s.section(DS, 2, 2).stripWhiteSpace();
                iFBContact = value.toUInt();
            }
            else if (key.compare(GF_HIDELEDS) == 0){
                iSoldLEDoff = value.toInt();
                /*this is the last parameter, now exit while loop*/
                break;
            }
        }
    }
}

/* code for old file format up to spdrs60 0.4.7 */
void element::readOldFileTextFromStream(QTextStream& ats)
{
    QString s, key, value;

    while (!ats.eof()) {
        s = ats.readLine();
        if (!s.startsWith("#")) {
            key = s.section(IDS, 0, 0);
            value = s.section(IDS, 1, 1).stripWhiteSpace();
            /* key/value pairs are read sequence independent */
            if (key.compare(GF_NAME) == 0){
                  sSoldIcon = value.stripWhiteSpace();
                  //fprintf(stderr, "Old-Icon: %s\n", sSoldIcon.data());
            }
            else if (key.compare(GF_ROTATE) == 0){
                iSoldRotate = value.toInt();
            }
            else if (key.compare(GF_INVERSTO) == 0){
                iSoldInvert = value.toInt();
            }
            else if (key.compare(GF_DECODER) == 0){
                sSoldDecoder = value;
            }
            else if (key.compare(GF_PROTOCOL) == 0){
                if (value == "M")
                    protocol = SrcpMessage::proMM;
                else if (value == "N")
                    protocol = SrcpMessage::proDCC;
                else if (value == "P")
                    protocol = SrcpMessage::proServer;
                else if (value == "S")
                    protocol = SrcpMessage::proSelectrix;
                else
                    protocol = SrcpMessage::proNone;
            }
            else if (key.compare(GF_ADDRESS1) == 0){
                iSoldAddress_1 = value.toInt();
                iGA1BusNo = 1;
                port1 = 1;
            }
            else if (key.compare(GF_ADDRESS2) == 0){
                iSoldAddress_2 = value.toInt();
                iGA2BusNo = 1;
                port2 = 1;
            }
            else if (key.compare(GF_XCHCONN1) == 0){
                iSoldChangeConn[0] = value.toInt();
            }
            else if (key.compare(GF_XCHCONN2) == 0){
                iSoldChangeConn[1] = value.toInt();
            }
            else if (key.compare(GF_DIRECTION) == 0){
                iSoldDirection = value.toInt();
            }
            else if (key.compare(GF_SUBTYPE) == 0){
                iSoldSubType = value.toInt();
            }
            else if (key.compare(GF_TEXT) == 0){
                //sSoldText = value;
                sSoldText = s.section(IDS, 1, 1).stripWhiteSpace();
            }
            else if (key.compare(GF_ACTTIME) == 0){
                iSoldActiveTime = value.toInt();
            }
            else if (key.compare(GF_FBPORT) == 0){
                int tempport = value.toInt();
                if (tempport <= -1) {
                    iFBContact = 0;
                    iFBBusNo = 1;
                }
                else {
                    iFBContact = tempport % 496 + 1;
                    iFBBusNo = tempport / 496 + 1;
                }
            }
            else if (key.compare(GF_HIDELEDS) == 0){
                iSoldLEDoff = value.toInt();
                /*this is the last parameter, now exit while loop*/
                break;
            }
        }
    }
}

/**
 * update element type dependend property values
 */
void element::updateProperties()
{
    /* initialize standard properties of this special symbol to avoid
     * recalculation in several procedures by expensive string
     * comparations*/
    // init signals as they were saved in layout file or with red state
    if ((sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS ||
         sSoldIcon == SYM_SS || sSoldIcon == SYM_SSH ||
         sSoldIcon == SYM_SSS || sSoldIcon == SYM_REL ||
         sSoldIcon == SYM_WS || sSoldIcon == SYM_ZP ||
         sSoldIcon == SYM_BLD || sSoldIcon == SYM_VS) &&
         (pref.initsignalsred))
        iSoldDirection = 0;

    if (sSoldIcon == SYM_ENK)   // couplers get the non-active direction
        iSoldDirection = 0;     // on setup

    signal = sSoldIcon.startsWith("signal");

    if (signal)
        ffm = (sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS ||
                sSoldIcon == SYM_SSH);

    else if (sSoldIcon.startsWith("weiche") ||
                sSoldIcon.startsWith("dreier") ||
                sSoldIcon.startsWith("ekw") ||
                sSoldIcon.startsWith("dkw"))
        turnout = true;

    else if (sSoldIcon == SYM_BLD ||sSoldIcon == SYM_ENK ||
            sSoldIcon == SYM_REL || sSoldIcon == SYM_MDC)
        simplega = true;

    /*element can be a part of a route*/
    routable = signal || turnout ||
        sSoldIcon.startsWith("diagonale") ||
        sSoldIcon.startsWith("kreuzung") ||
        sSoldIcon.startsWith("kurve") ||
        sSoldIcon.startsWith("richtung") ||
        sSoldIcon.startsWith("gerade") || sSoldIcon == SYM_BUE ||
        sSoldIcon == SYM_ADR || sSoldIcon == SYM_BLD ||
        sSoldIcon == SYM_ENK;
    
    /*element has decoder connected*/
    switchable = (signal || turnout || simplega) &&
        sSoldIcon != SYM_NRB && sSoldIcon != SYM_SRB;

    state2dkw = (sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR)
        && iSoldSubType == 0;

    isright = sSoldIcon.contains("links", 1) ? 0 : 1;
}


bool element::is2StateDKW()
{
    return state2dkw;
}

/* check if this element contains information to save*/
bool element::isEmpty()
{
    return (sSoldIcon == SYM_LEE) && (iSoldInvert != 1) &&
        (sSoldText == "-1" || sSoldText.isEmpty());
}


bool element::isSignal()
{
    return signal;
}


bool element::isRoutable()
{
    return routable;
}


bool element::isSwitchable()
{
    return switchable;
}


bool element::isSimpleGA()
{
    return simplega;
}


bool element::isTurnout()
{
    return turnout;
}

/**
 * create context popup menus
 *
 * Every single element gets its own toggle and edit popupmenu!
 * TODO: Move this to gbsarea and use only one popup for all elements
 */
void element::createPopupMenus()
{
    // context menu with "toggle" for normal mode
    ctxNorm = new QPopupMenu(this, "ctxNormPM");
    ctxNorm->insertItem(tr("&Toggle"), this, SLOT(slotToggle()),
                        0, CTX_ID_TOGGLE);

    QPixmap p;
    // context menu with entries for edit mode
    ctxEdit = new QPopupMenu(this, "ctxEditPM");
    ctxEdit->insertItem(tr("&Repeat"), CTX_ID_REP);
    ctxEdit->setItemEnabled(CTX_ID_REP, false);
    ctxEdit->insertSeparator();

    p = QPixmap(ctx_rota_xpm);
    ctxEdit->insertItem(p, tr("R&otate"), CTX_ID_ROTATE);
    p = QPixmap(ctx_clear_xpm);
    ctxEdit->insertItem(p, tr("&Clear"), CTX_ID_CLEAR);
    ctxEdit->insertSeparator();

    p = QPixmap(ctx_straight_xpm);
    ctxEdit->insertItem(p, SYM_GER, 5);
    p = QPixmap(ctx_l_curve_xpm);
    ctxEdit->insertItem(p, SYM_KUL, 6);
    p = QPixmap(ctx_r_curve_xpm);
    ctxEdit->insertItem(p, SYM_KUR, 7);
    p = QPixmap(ctx_l_diag_xpm);
    ctxEdit->insertItem(p, SYM_DIL, 8);
    p = QPixmap(ctx_r_diag_xpm);
    ctxEdit->insertItem(p, SYM_DIR, 9);
    p = QPixmap(ctx_l_turn_xpm);
    ctxEdit->insertItem(p, SYM_WEL, 10);
    p = QPixmap(ctx_r_turn_xpm);
    ctxEdit->insertItem(p, SYM_WER, 11);

    connect(ctxEdit, SIGNAL(activated(int)), this, SLOT(slotCtxEdit(int)));
}

/**
 * switch element to new direction
 */
void element::switchToDir(int newdir)
{
    // quit if element contains no solenoid
    if (!switchable)
        return;

    if (turnout)
        switchToDirBlinking(newdir);
    else {
        // Repainting the element is only done when new direction differs
        // from the old one. Momentary couplers are exceptional: they
        // only use one connector e.g. one direction which is activated
        // or deactivated, these are treated the same
        // TODO: option to repaint only when INFO messages came back from
        // srcp server
        if (newdir != iSoldDirection || sSoldIcon == SYM_ENK) {
            iSoldDirection = newdir;
            setupElementIcon();
            sendSrcpState();
        }

        // if the new direction equals the old one just setup the element
        // to ensure that the contextmenu and the lock variable are correct set
        // exception: 2-state-DKW, they would show a momentary false LED state
        //else if (!((sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR)
        //           && iSoldSubType == 0))
        else {
            //if (!is2StateDKW()) 
            //updateCtxNorm();

            // Switch command is send to SRCP server if forced.
            if (pref.sendstate)
                sendSrcpState();
        }
    }
}

/**
 * react to mouse press events
 */
void element::mousePressEvent(QMouseEvent* e)
{
    /*normal mode*/
    if (visualMode == kvmNormal) {
        if (e->button() == LeftButton) {
            GbsButtonState ctrlButton = kNoneClicked;
            QPoint CursorPos = mapFromGlobal(QCursor::pos());

            /*TODO: move this to gbsarea*/
            if (sSoldIcon == SYM_DRE) {
                ttComm = new turntableCommander(this, iSoldSubType, sSoldText);
                connect(ttComm, SIGNAL(applyPressed(QPoint)),
                        this, SLOT(slotUpdateTurntableData(QPoint)));
                connect(ttComm, SIGNAL(sendAvailTracks(const QString&)),
                        this, SLOT(slotCopyAvailTracks(const QString&)));

                ttComm->exec();     // parent window NOT usable
                ttComm->move(QCursor::pos());
            }

            /*TODO: move this to gbsarea*/
            else if (sSoldIcon == SYM_SBN || sSoldIcon == SYM_MDC) {
                turntableProperties = new elementCommander(this, sSoldIcon);
                connect(turntableProperties, SIGNAL(applyPressed(QPoint)),
                        this, SLOT(slotUpdateTurntableData(QPoint)));

                turntableProperties->exec();        // parent window NOT usable
                turntableProperties->move(QCursor::pos());
            }

            /* determine what type of button was pressed an send the
             * correspondig value to GBSArea to change cursor shape etc.*/
            // if element contains a solenoid or is a external button
            else if (iSoldAddress_1 != -1){

                if (sSoldIcon == SYM_HS || sSoldIcon == SYM_NRB)
                    ctrlButton = kZfsClicked;

                else if (sSoldIcon == SYM_SS || sSoldIcon == SYM_SSS ||
                        sSoldIcon == SYM_WS || sSoldIcon == SYM_SRB)
                    ctrlButton = kRfsClicked;

                else if (sSoldIcon == SYM_HSS){
                    /* two different buttons on this panel */
                    if (CursorPos.x() > (EL_WIDTH >> 1) ^ (bool)iSoldRotate)
                        ctrlButton = kZfsClicked; 
                    else
                        ctrlButton = kRfsClicked;
                }

                else if (sSoldIcon == SYM_SSH)
                    ctrlButton = kZhsClicked;

                else
                    ctrlButton = kTurnoutClicked;

                emit elementClicked(this, ctrlButton);
            }

            /*add here new external button functions*/
            /*TODO: change to switch SYM_ID_ */
            else if (sSoldIcon.startsWith("taste")){
                if (sSoldIcon == SYM_TAW)
                    ctrlButton = kWgtClicked;

                else if (sSoldIcon == SYM_TAF) {
                    ctrlButton = kFhtClicked;

                    /*
                     * increment counter and repaint symbol, if value
                     * has more than four digits, reset to zero
                     */
                    ++countervalue;
                    if (countervalue == 10000)
                        countervalue = 0;
                    setupElementIcon();
                }

                else if (sSoldIcon == SYM_TAU) {
                    if (CursorPos.x() < (EL_WIDTH >> 1))
                        ctrlButton = kUfgtClicked; 
                    else
                        ctrlButton = kMgtClicked;
                }

                else if (sSoldIcon == SYM_TAS) {
                    if (CursorPos.x() < (EL_WIDTH >> 1))
                        ctrlButton = kSgtClicked; 
                    else
                        ctrlButton = kHagtClicked;
                }
                emit elementClicked(this, ctrlButton);
            }
            e->accept();
        }
    }
    /*layout edit mode*/
    else if (visualMode == kvmEditLayout) {
        if (e->button() == LeftButton) {
            /*TODO: select element*/
            e->accept();
        }
    }
    /*route edit mode*/
    else if (visualMode == kvmEditRoute) {
        // do nothing
        e->accept();
    }
}

/**
 * react to mouse release events
 */
void element::mouseReleaseEvent(QMouseEvent* e)
{
    /*normal mode*/
    if (visualMode == kvmNormal) {
        if (e->button() == RightButton){
            if (isSwitchable())
                ctxNorm->exec(QCursor::pos());
            e->accept();
        }
    }
    /*layout edit mode*/
    else if (visualMode == kvmEditLayout) {
        if (e->button() == LeftButton) {
            /*TODO: handle drop action*/
            e->accept();
        }
        else if (e->button() == MidButton) {
            ctxEdit->exec(QCursor::pos());
            e->accept();
        }
        else if (e->button() == RightButton){
            showPropertyDlg();
            e->accept();
        }
    }
    /*route edit mode*/
    else if (visualMode == kvmEditRoute) {
        if (e->button() == LeftButton) {
            /*select/unselect start or stop signal*/
            if ((sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS ||
                 sSoldIcon == SYM_SS || 
                 sSoldIcon == SYM_SSH || sSoldIcon == SYM_SSS ||
                 sSoldIcon == SYM_NRB || sSoldIcon == SYM_SRB)){
                /*send record signal to router*/
                if (ksmNormal == selectionMode)
                    emit recordElement(this, krecStartStop);
                else
                    emit recordElement(this, krecClear);
                e->accept();
            }
        }
        else if (e->button() == MidButton) {
            /*select/unselect switchable element*/
            if (sSoldIcon == SYM_WEL || sSoldIcon == SYM_WER ||
                sSoldIcon == SYM_DWL || sSoldIcon == SYM_DWR || 
                sSoldIcon == SYM_WEY || sSoldIcon == SYM_DKR || 
                sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR || 
                sSoldIcon == SYM_DKL || sSoldIcon == SYM_DRW || 
                sSoldIcon == SYM_REL || sSoldIcon == SYM_ZP  || 
                sSoldIcon == SYM_HS  || sSoldIcon == SYM_HSS ||
                sSoldIcon == SYM_SS  || sSoldIcon == SYM_SSS ||
                sSoldIcon == SYM_SSH || sSoldIcon == SYM_BLD ||
                sSoldIcon == SYM_VS  || sSoldIcon == SYM_WS  ||
                sSoldIcon == SYM_MDC) {
                /*send record signal to router*/
                if (ksmNormal == selectionMode)
                    emit recordElement(this, krecNormal);
                else
                    emit recordElement(this, krecClear);
                e->accept();
            }
        }
        else if (e->button() == RightButton){
            /*show context menu to switch element only without selection*/
            if (ksmNormal == selectionMode && isSwitchable())
                ctxNorm->exec(QCursor::pos());
            e->accept();
        }
    }
}


void element::slotShowElement(int address, int state,
                              elemSelectionMode sm)
{
    if (address == iSoldAddress_1) {
        selectionMode = sm;
        if (state != -1)
            iSoldDirection = state;
        setupElementIcon();
    }
}


void element::showElementState(int state, elemSelectionMode sm)
{
    selectionMode = sm;
    if (state != -1)
        iSoldDirection = state;
    setupElementIcon();
}


void element::locateMe()
{
    // activate edit mode for LOCATE_TIMER secs
    // if this is the searched element, show element in found mode
    selectionMode = ksmFoundEl;
    update();

    locateTimer = new QTimer();
    locateTimer->start(LOCATE_TIMER, true);
    connect(locateTimer, SIGNAL(timeout()),
            this, SLOT(slotLocateTimerTimeout()));
}


void element::slotLocateTimerTimeout()
{
    delete locateTimer;
    // show element in normal mode
    selectionMode = ksmNormal;
    update();
}


void element::switchSelectionMode(elemSelectionMode sm)
{
    if (selectionMode != sm) {
        selectionMode = sm;

        if (isSwitchable())
            updateCtxNorm();

        update();
    }
}


void element::switchVisualMode(elemVisualMode vm)
{
    visualMode = vm;
    /* send element state when visual mode is switched to normal mode
     * and selection mode is not normal; typicaly after view route mode
     */
    if (selectionMode != ksmNormal)
        selectionMode = ksmNormal;

    if (isSwitchable())
        updateCtxNorm();

    update();
}


void element::sendSrcpState()
{
    /* do not send anything for rail buttons without signals */
    if (sSoldIcon == SYM_SRB || sSoldIcon == SYM_NRB)
        return;

    bool bSwitchSecondAddress = false;
        
    // default copy of direction + address
    int iRealDirection = iSoldDirection;
    int iRealAddress = iSoldAddress_1;
    int iRealBus = iGA1BusNo;


  DO_AGAIN:;
    int sendRepeatCounter = 1;

    // element contains a momentarily activated coupler
    if (sSoldIcon == SYM_ENK && iSoldSubType != -1)
        iRealDirection = iSoldSubType;  // copy subtype as realDirection

    // element contains a main signal with direction >= 2
    else if ((sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS ||
              sSoldIcon == SYM_VS) && iSoldDirection >= 2) {

        /*
         * serd: Viessman Formsignale often need several attempts
         * wait only if command for signal is repeated
         */
        if (sSoldDecoder.startsWith("Vi"))
            sendRepeatCounter = cNumRepeatCommands;

        // Hp0+Hp1 not considered, is done by default copy
        switch (iSoldSubType) {
            case 6:            // Hp0+Hp2,     --> iSoldDirection = 2
            case 7:            // Hp0+Hp2+Sh1, --> iSoldDirection = 2 or 3
                iRealDirection = 1;
                // Sh1, case 7, HSS
                if (iSoldDirection == 3) {
                    iRealAddress = iSoldAddress_2;
                    iRealBus = iGA2BusNo;
                }
                break;
            case 4:            // Hp0+Hp1+Hp2, --> iSoldDirection = 2
                iRealDirection = 0;
                iRealAddress = iSoldAddress_2;
                iRealBus = iGA2BusNo;
                break;
            case 1:            // Hp0+Hp1+Sh1, --> iSoldDirection = 3
            case 5:            // Hp0+Hp1+Hp2+Sh1, --> iSoldDirection = 2 or 3
                iRealDirection = iSoldDirection - 2;
                iRealAddress = iSoldAddress_2;
                iRealBus = iGA2BusNo;
                break;
        }
    }

    // element contains a 3-way-turnout
    else if (sSoldIcon == SYM_DRW) {
        switch (bSwitchSecondAddress) {
            case false:            // send first address data
                iRealDirection = iSoldDirection % 2;
                break;
            case true:             // send second address data
                iRealDirection = (iSoldDirection > 1);
                iRealAddress = iSoldAddress_2;
                iRealBus = iGA2BusNo;
                break;
        }
    }

    // element contains a 4-state-DKW or EKW
    else if ((sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR) ||
             ((sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR) &&
              iSoldSubType == 1)) {
        switch (bSwitchSecondAddress) {
            case false:            // send first address data
                iRealDirection = (iSoldDirection >= 2);
                break;
            case true:             // send second address data
                iRealDirection = (iSoldDirection == 1 || iSoldDirection == 2);
                iRealAddress = iSoldAddress_2;
                iRealBus = iGA2BusNo;
                break;
        }
    }

    if (protocol != SrcpMessage::proNone) {

        /*
         * The calculated real direction to be sent is modified
         * again if you electronically changed your decoder
         * outputs of one address.
         */
        if (sSoldIcon != SYM_DRE) {
            if (iRealAddress == iSoldAddress_1)
                iRealDirection = iRealDirection ^ iSoldChangeConn[0];
            if (iRealAddress == iSoldAddress_2)
                iRealDirection = iRealDirection ^ iSoldChangeConn[1];
        }

        int port = 0;
        int value = 0;
            
        switch (protocol) {
            case SrcpMessage::proMM:
                port = iRealDirection;
                value = 1;
                break; 
            case SrcpMessage::proDCC: 
                port = !iRealDirection;
                value = 1;
                break;
            default:
                //TODO: translate Selectrix address to flat address
                if (iRealAddress == iSoldAddress_1)
                    port = port1;
                else
                    port = port2;

                value = iRealDirection;
                break;
        }

        SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgGaSet);
        if (sm == NULL)
            return;

        sm->setGaData(protocol, iRealBus, iRealAddress, port,
                value, iSoldActiveTime);
        
        while (sendRepeatCounter > 0) {
            emit sendSrcpMessage(sm);
            --sendRepeatCounter;
            // TODO: timer controlled repeat, this delay also suspends
            // redrawing of element icons
            if (sendRepeatCounter > 0)
                usleep(iSoldActiveTime * 1000);
        }
        delete sm;

        // return to copy direction and address for second switch
        if ((sSoldIcon == SYM_DRW || sSoldIcon == SYM_EKL ||
             sSoldIcon == SYM_EKR || ((sSoldIcon == SYM_DKL ||
             sSoldIcon == SYM_DKR) && iSoldSubType == 1)) &&
             !bSwitchSecondAddress) {
            bSwitchSecondAddress = true;
            goto DO_AGAIN;
        }
        /*
         * if momentary coupler: activate it, wait for a short time
         * and deactivate it graphically
         */
        else if (sSoldIcon == SYM_ENK && iSoldSubType != -1)
            QTimer::singleShot(iSoldActiveTime, this,
                    SLOT(repaintTimeOutEnk()));
    }
}


void element::repaintTimeOutEnk()
{
    iSoldDirection = !iSoldDirection;
    setupElementIcon();
}


/**
 * this is the reverse case of "sendSrcpState()"
 */
void element::processInfoPortMessage(unsigned int bus,
        unsigned int addr, unsigned int port, unsigned int value)
{
    //TODO: respect "value"
    if (!switchable)
        return;

    if (blinkcounter != 0)
        return;
    
    if (sSoldIcon == SYM_ENK && iSoldSubType != -1)
        return;
    
    if (!(bus == iGA1BusNo && addr == iSoldAddress_1) ||
       (bus == iGA2BusNo && addr == iSoldAddress_2))
        return;
    
    /*TODO: add elements with two addresses*/
    if (iSoldAddress_2 != -1 || iSoldDirection > 2)
        return;

    //FIXME: only MM and DCC is accepted
    if (protocol != SrcpMessage::proDCC && protocol != SrcpMessage::proMM)
        return;

    int realDir = iSoldDirection;
    if (realDir == 2) //Hp0-Hp2-Type (iSoldSubType == 6)
        realDir = 1;
    
    bool isDCC = (protocol == SrcpMessage::proDCC);
    if (isDCC)
        realDir = !realDir;

    /*invert direction if connectors are exchanged*/
    realDir = realDir ^ iSoldChangeConn[0];
        
    if (port != realDir) {
        realDir = port;

        /*again invert direction if connectors are exchanged*/
        realDir = realDir ^ iSoldChangeConn[0];
        
        if (isDCC)
            realDir = !realDir;

        //Hp0-Hp2-Type (iSoldSubType == 6)
        if (iSoldSubType == 6 && realDir == 1)
            realDir = 2;

        iSoldDirection = realDir;
        setupElementIcon();
        // TODO: show warning message when element is locked
    }
}

/**
 * show element property dialog
 */
void element::showPropertyDlg()
{
    /* when dialog is allready open just bring it to front
       else create new dialog */
    if (elementPropertyDlg != NULL) {
        elementPropertyDlg->setActiveWindow();
        elementPropertyDlg->raise();
    }
    else {
        elementPropertyDlg = new elementDialog(this, iSoldIndex);
        if (elementPropertyDlg == NULL)
            return;
        
        elementPropertyDlg->setSymbolText(sSoldText);
        elementPropertyDlg->setRotated(iSoldRotate);
        elementPropertyDlg->setInverted(iSoldInvert);
        elementPropertyDlg->setLEDsAreOff(iSoldLEDoff);
        elementPropertyDlg->setGASubType(iSoldSubType);
        elementPropertyDlg->setProtocol((int) protocol);
        elementPropertyDlg->setDecoder(sSoldDecoder);
        elementPropertyDlg->setSRCPBus1(iGA1BusNo);
        elementPropertyDlg->setAddress1(iSoldAddress_1);
        elementPropertyDlg->setXChangeConn1(iSoldChangeConn[0]);
        elementPropertyDlg->setSRCPBus2(iGA2BusNo);
        elementPropertyDlg->setAddress2(iSoldAddress_2);
        elementPropertyDlg->setXChangeConn2(iSoldChangeConn[1]);
        elementPropertyDlg->setDirection(iSoldDirection);
        elementPropertyDlg->setActiveTime(iSoldActiveTime);
        elementPropertyDlg->setFBBus(iFBBusNo);
        elementPropertyDlg->setFBContact(iFBContact);
        // this must be the last one, because it tiggers enabling and
        // disabling of all element dependend widgets
        elementPropertyDlg->setSymbolName(sSoldIcon);
        //FIXME: minvalues and maxvalues of port spinboxes are set to late
        elementPropertyDlg->setPort1(port1);
        elementPropertyDlg->setPort2(port2);

        connect(elementPropertyDlg, SIGNAL(sigShowFBmodules()),
                this, SIGNAL(sigShowFBmodules()));
        
        if (elementPropertyDlg->exec() == QDialog::Accepted){

            sSoldIcon = elementPropertyDlg->getSymbolName();
            sSoldText = elementPropertyDlg->getSymbolText();
            iSoldRotate = elementPropertyDlg->getRotated();
            iSoldInvert = elementPropertyDlg->getInverted();
            iSoldLEDoff = elementPropertyDlg->getLEDsAreOff();
            iSoldSubType = elementPropertyDlg->getGASubType();
            protocol =
                (SrcpMessage::Protocol) elementPropertyDlg->getProtocol();
            sSoldDecoder = elementPropertyDlg->getDecoder();
            iGA1BusNo = elementPropertyDlg->getSRCPBus1();
            iSoldAddress_1 = elementPropertyDlg->getAddress1();
            iSoldChangeConn[0] = elementPropertyDlg->getXChangeConn1();
            port1 = elementPropertyDlg->getPort1();
            iGA2BusNo = elementPropertyDlg->getSRCPBus2();
            iSoldAddress_2 = elementPropertyDlg->getAddress2();
            iSoldChangeConn[1] = elementPropertyDlg->getXChangeConn2();
            port2 = elementPropertyDlg->getPort2();
            iSoldDirection = elementPropertyDlg->getDirection();
            iSoldActiveTime = elementPropertyDlg->getActiveTime();
            iFBBusNo = elementPropertyDlg->getFBBus();
            iFBContact = elementPropertyDlg->getFBContact();
            
            //set new repeat icon and send new name to all other elements
            slotRepeatIcon(sSoldIcon);
            emit setRepeatIcon(sRepeatIcon);

            updateProperties();
            // TODO: ask server for current occupation state
            //iSoldLEDstate = bFBport[iFBContact] << 1;
            updateLEDState();
            updateContextMenus();
            setupElementIcon();
        }
        disconnect(elementPropertyDlg, SIGNAL(sigShowFBmodules()),
                this, SIGNAL(sigShowFBmodules()));
        
        delete elementPropertyDlg;
        elementPropertyDlg = NULL;
    }
}

/**
 * set repeaticon to new value and update edit mode context menu
 **/
void element::slotRepeatIcon(const QString& ri)
{
    if (ctxEdit == NULL)
        return;

    if (sRepeatIcon != ri) {
        sRepeatIcon = ri;
        if (sRepeatIcon.isEmpty())
            ctxEdit->setItemEnabled(CTX_ID_REP, false);
        else
            ctxEdit->setItemEnabled(CTX_ID_REP, true);
        QString s = QString(tr("&Repeat: %1")).arg(sRepeatIcon);
        ctxEdit->changeItem(s, CTX_ID_REP);
    }
}

/**
 * toggle through element direction states
 */
void element::slotToggle()
{
    // toggles cyclic for 3-state-solenoids
    if (sSoldIcon == SYM_DRW ||
        sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR) {
        if (iSoldDirection < 2)
            switchToDir(iSoldDirection + 1);
        else
            switchToDir(0);
    }

    else if (sSoldIcon == SYM_HS || sSoldIcon == SYM_VS) {
        switch (iSoldDirection) {
            case 0:
                if (iSoldSubType != 6)
                    switchToDir(1);
                else 
                    switchToDir(2);
                break;
            case 1:
                if (iSoldSubType == 0)
                    switchToDir(0);
                else
                    switchToDir(2);
                break;
            case 2:
                switchToDir(0);
                break;
        } 
    }

    else if (sSoldIcon == SYM_HSS) {
        switch (iSoldDirection) {
            case 0:
                if (iSoldSubType < 6)
                    switchToDir(1);
                else 
                    switchToDir(2);
                break;
            case 1:
                if (iSoldSubType == 1)
                    switchToDir(3);
                else
                    switchToDir(2);
                break;
            case 2:
                switchToDir(3);
                break;
            case 3:
                switchToDir(0);
                break;
        } 
    }

    // toggles cyclic for 4-state-solenoids
    else if ((sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR)
             && iSoldSubType == 1) {
        if (iSoldDirection < 3)
            switchToDir(iSoldDirection + 1);
        else
            switchToDir(0);
    }

    // toggles cyclic for 2-state-solenoids
    else
        switchToDir(!iSoldDirection);
}

/**
 * react to actions of the edit context menu
 * 
 * this method has some inherent errors due to missing property
 * settings, especially when sRepeatIcon is used
 */
void element::slotCtxEdit(int ctxID)
{
    switch (ctxID) {
        case CTX_ID_REP:
            clear();
            sSoldIcon = sRepeatIcon;
            iSoldRotate = 0;        // all symbols are rotatable
            iSoldLEDoff = 0;
            iFBContact = 0;
            updateProperties();
            break;
        case CTX_ID_ROTATE:
            rotate();
            break;
        case CTX_ID_CLEAR:
            clear();
            break;
        case 5:
            clear();
            sSoldIcon = SYM_GER;    // straight
            iSoldRotate = 0;        // all symbols are rotatable
            iSoldLEDoff = 0;
            iFBContact = 0;
            updateProperties();
            break;
        case 6:
            clear();
            sSoldIcon = SYM_KUL;    // left curve
            iSoldRotate = 0;        // all symbols are rotatable
            iSoldLEDoff = 0;
            iFBContact = 0;
            updateProperties();
            break;
        case 7:
            clear();
            sSoldIcon = SYM_KUR;    // right "
            iSoldRotate = 0;        // all symbols are rotatable
            iSoldLEDoff = 0;
            iFBContact = 0;
            updateProperties();
            break;
        case 8:
            clear();
            sSoldIcon = SYM_DIL;    // left diagonal
            iSoldLEDoff = 0;
            iFBContact = 0;
            updateProperties();
            break;
        case 9:
            clear();
            sSoldIcon = SYM_DIR;    // right "
            iSoldLEDoff = 0;
            iFBContact = 0;
            updateProperties();
            break;
        case 10:
            clear();
            sSoldIcon = SYM_WEL;    // left turnout
            iSoldRotate = 0;        // all symbols are rotatable
            iSoldLEDoff = 0;
            iFBContact = 0;
            updateProperties();
            break;
        case 11:
            clear();
            sSoldIcon = SYM_WER;    // right "
            iSoldRotate = 0;        // all symbols are rotatable
            iSoldLEDoff = 0;
            iFBContact = 0;
            updateProperties();
            break;
    }
    //TODO: get current occupation state if information is relevant for
    //this element
    //iSoldLEDstate = bFBport[iFBContact] << 1;
    //send new icon name to all other elements
    slotRepeatIcon(sSoldIcon);
    emit setRepeatIcon(sSoldIcon);
    updateLEDState();
    updateContextMenus();
    setupElementIcon();
}

/*
 * rotate element
 */
void element::rotate()
{
    iSoldRotate = !iSoldRotate;
}

/**
 * reset all element data to an empty element
 */
void element::clear()
{
    iSoldActiveTime = -1;
    iSoldChangeConn[0] = -1;
    iSoldChangeConn[1] = -1;
    iSoldDirection = -1;
    iFBContact = 0;
    iSoldInvert = -1;
    iSoldLEDoff = 1;
    iSoldLEDstate = LED_OFF;
    lockCounter = 0;
    iSoldRotate = -1;
    iSoldSubType = -1;
    iSoldAddress_1 = -1;
    iSoldAddress_2 = -1;
    sSoldDecoder = "-1";
    sSoldIcon = SYM_LEE;
    protocol = SrcpMessage::proNone;
    sSoldText = "-1";
    updateProperties();
}

/**
 * update context menu for normal visual mode
 */
void element::updateCtxNorm()
{
    bool enableCtxN = (!isLocked()) &&
        (!isOccupied() || sSoldIcon == SYM_ENK || signal) &&
        (visualMode == kvmNormal || visualMode == kvmEditRoute);

    if (ctxNorm != NULL)
        ctxNorm->setItemEnabled(CTX_ID_TOGGLE, enableCtxN);
}

/**
 * update the context menus for edit mode and normal mode,
 * used after element name has changed
 */
void element::updateContextMenus()
{
    if (ctxEdit != NULL) {
        ctxEdit->setItemEnabled(CTX_ID_CLEAR, !isEmpty());
        ctxEdit->setItemEnabled(CTX_ID_ROTATE, iSoldRotate != -1);
    }
    if (isSwitchable())
        updateCtxNorm();
}

/**
 * paint element icon
 * TODO: this should completely be rewritten due to performance flaws
 */

// element::pixmap();
// static element::getPixmap(const QString& name);
// setPaletteBackgroundPixmap(pixmap());
//
void element::setupElementIcon()
{
    // empty symbol
    if (sSoldIcon == SYM_LEE) {
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
            
        // paint panel
        if (iSoldInvert == 1) {
            p.fillRect(0, 0, pm.width() - 1, pm.height() - 1,
                    QBrush(darkGray));
        }

        // paint text
        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 1);
            p.setFont(f);

            p.drawText(pm.rect(), Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, sSoldText);
        }
        p.end();
        setPaletteBackgroundPixmap(pm);
    }
   
    // blue panel
    else if (sSoldIcon == SYM_FEB) {
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
            
        // paint panel
        p.fillRect(0, 0, pm.width() - 1, pm.height() - 1,
                QBrush(QColor(0, 0, 192)));

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
   
    // blue panel with wgt button
    else if (sSoldIcon == SYM_TAW) {
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
            
        // paint panel
        p.fillRect(0, 0, pm.width() - 1, pm.height() - 1,
                QBrush(QColor(0, 0, 192)));

        // paint red light
        p.setBrush(red);
        p.drawEllipse(pm.width() / 2 - 2, pm.height() / 4 - 3, 5, 5);
        
        // paint button
        p.setBrush(darkGray);
        p.drawEllipse(pm.width() / 2 - 4, pm.height() / 2 - 4, 9, 9);
        
        // paint button label
        p.drawPixmap(18, 24, QPixmap(label_wgt_xpm));

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
   
    // green panel
    else if (sSoldIcon == SYM_FEG) {
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
            
        // paint panel
        p.fillRect(0, 0, pm.width() - 1, pm.height() - 1,
                QBrush(QColor(0, 160, 0)));

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
   
    // green panel with FHT button and counter
    else if (sSoldIcon == SYM_TAF) {
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
            
        // paint panel
        p.fillRect(0, 0, pm.width() - 1, pm.height() - 1,
                QBrush(QColor(0, 160, 0)));
        
        // paint button
        p.setBrush(darkGray);
        p.drawEllipse(7, pm.height() / 2 - 4, 9, 9);
        
        // paint button label
        p.drawPixmap(2, 24, QPixmap(label_fht_xpm));

        // paint counter
        p.setBrush(white);
        p.drawRect(pm.width() / 2 , pm.height() / 2 - 5, 24, 11);
        sSoldText.sprintf("%04d", countervalue);
        QFont f("Helvetica");
        f.setPointSize(7);
        p.setFont(f);
        QRect br = p.fontMetrics().boundingRect(sSoldText);
        br.moveTopRight(QPoint(pm.width() / 2 + 21, pm.height() / 2 - 3));
        p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                Qt::DontClip, sSoldText);

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
   
    // green panel with ufgt and mgt buttons
    else if (sSoldIcon == SYM_TAU) {
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
            
        // paint panel
        p.fillRect(0, 0, pm.width() - 1, pm.height() - 1,
                QBrush(QColor(0, 160, 0)));
        
        // paint buttons
        p.setBrush(darkGray);
        p.drawEllipse(7, pm.height() / 2 - 4, 9, 9);
        p.drawEllipse(pm.width() - 16, pm.height() / 2 - 4, 9, 9);
        
        // paint button labels
        p.drawPixmap(2, 24, QPixmap(label_ufgt_xpm));
        p.drawPixmap(34, 24, QPixmap(label_mgt_xpm));

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
   
    // red panel
    else if (sSoldIcon == SYM_FER) {
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
            
        // paint panel
        p.fillRect(0, 0, pm.width() - 1, pm.height() - 1,
                QBrush(QColor(221, 0, 0)));

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
   
    // red panel with sgt and hagt buttons
    else if (sSoldIcon == SYM_TAS) {
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
            
        // paint panel
        p.fillRect(0, 0, pm.width() - 1, pm.height() - 1,
                QBrush(QColor(221, 0, 0)));

        // paint buttons
        p.setBrush(darkGray);
        p.drawEllipse(7, pm.height() / 2 - 4, 9, 9);
        p.drawEllipse(pm.width() - 16, pm.height() / 2 - 4, 9, 9);
        
        // paint button labels
        p.drawPixmap(2, 24, QPixmap(label_sgt_xpm));
        p.drawPixmap(31, 24, QPixmap(label_hagt_xpm));

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
   
    // yellow panel
    else if (sSoldIcon == SYM_FEY) {
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
            
        // paint panel
        p.fillRect(0, 0, pm.width() - 1, pm.height() - 1,
                QBrush(QColor(224, 224, 0)));

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
   
    // brown panel
    else if (sSoldIcon == SYM_FEN) {
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
            
        // paint panel
        p.fillRect(0, 0, pm.width() - 1, pm.height() - 1,
                QBrush(QColor(112, 48, 0)));

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
   
    // grey panel
    else if (sSoldIcon == SYM_FEE) {
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
            
        // paint panel
        p.fillRect(0, 0, pm.width() - 1, pm.height() - 1,
                QBrush(QColor(128, 128, 128)));

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
   
    // buffer stop (prellbock)
    else if (sSoldIcon == SYM_PRE) {
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
            
        // paint panel
        if (iSoldRotate == 1)
            p.fillRect(0, pm.height() / 2 - 6, 5, 13,
                    QBrush(black));
        else
            p.fillRect(pm.width() - 5, pm.height() / 2 - 6, pm.width(),
                    13, QBrush(black));

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
   
    // direction arrows
    else if (sSoldIcon == SYM_RI1 || sSoldIcon == SYM_RI2){
        bool isri2 = (sSoldIcon == SYM_RI2);
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w - 1, 7, QBrush(black));

        // paint arrows
        /**
         * isri2  rotated  rightarrow  leftarrow
         * -------------------------------------
         *  0       0         1            0
         *  0       1         0            1
         *  1       0         1            1
         *  1       1         1            1
         * -------------------------------------
         **/

        p.setBrush(QBrush(black));

        if (isri2 || iSoldRotate == 1) {
            QPointArray leftarrow = QPointArray(4);
            leftarrow.putPoints(0, 4, w * 3 /4, h / 2,
                    w - 1, 1, w / 2 - 6, h / 2, w - 1, h - 1);
            p.drawPolygon(leftarrow);
        }
        if (isri2 || iSoldRotate != 1) {
            QPointArray rightarrow = QPointArray(4);
            rightarrow.putPoints(0, 4, w / 4, h / 2, 1, 1,
                    w / 2 + 6, h / 2, 1, h - 1);
            p.drawPolygon(rightarrow);
        }
        
        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(w / 3, h / 2, 2 * w / 3, h / 2);
            p.setPen(QPen(black));
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QRect br = fm.boundingRect(sSoldText);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            if (iSoldRotate == 1)
                br.moveBottomRight(QPoint(w / 2 + br.width() / 2,
                            h - 3));
            else
                br.moveTopLeft(QPoint(w / 2 - br.width() / 2, 2));
            
            p.fillRect(br, QBrush(white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, sSoldText);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // address
    else if (sSoldIcon == SYM_ADR){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        // paint track
        p.fillRect(0, pm.height() / 2 - 3, 3, 7,
                QBrush(black));
        p.fillRect(pm.width() - 4, pm.height() / 2 - 3, 3, 7,
                QBrush(black));

        // paint address field
        p.setPen(QPen(darkGray, 2));
        
        if (iSoldRotate == 1)
            p.setBrush(QColor(black));
        else
            p.setBrush(QColor(220, 220, 220));
        
        p.drawRect(4, 7, pm.width() - 8, pm.height() - 14);

        // paint address value
        if (iSoldRotate == 1)
            p.setPen(QPen(red));
        else
            p.setPen(QPen(black));

        QFont f("Helvetica");
        f.setPointSize(QApplication::font().pointSize() + 2);
        f.setWeight(QFont::DemiBold);
        p.setFont(f);
        sSoldText.sprintf("%05d", editsAddress);
        QFontMetrics fm(f);
        QRect br = fm.boundingRect(sSoldText);
        br.moveTopLeft(QPoint(pm.width() / 2 - br.width() / 2 - 1,
                    pm.height() / 2 - br.height() / 2));
	p.drawText(br, Qt::AlignCenter | Qt::SingleLine | Qt::DontClip,
                sSoldText);
        
        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // level crossing
    else if (sSoldIcon == SYM_BUE){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        // paint road
        p.fillRect(pm.width() / 2 - 7, 0, 15, pm.height(),
                QBrush(darkGray));

        // paint track
        p.fillRect(0, pm.height() / 2 - 3, pm.width() - 1, 7,
                QBrush(black));

        // paint track button
        p.drawPixmap(pm.width() / 2  - 4, pm.height() / 2 - 3,
                QPixmap(button_yellow_xpm));
        
        // paint lights
        p.setBrush(darkGray);
        p.drawEllipse(6, 4, 6, 6);
        p.drawEllipse(pm.width() - 12, 4, 6, 6);
        //p.drawRect(pm.width()/2 - 3, 4, 6, 6);

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // relay
    else if (sSoldIcon == SYM_REL){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);

        int w = pm.width();
        int h = pm.height();
        
        // paint symbol
        p.drawLine(1, h / 8, w / 6 + 3, h / 8);
        p.drawLine(w / 6 + 3, h / 8, w / 6 + 3, h / 4);
        
        p.drawLine(1, h * 7 / 8, w / 6 + 3, h * 7 / 8);
        p.drawLine(w / 6 + 3, h * 7 / 8, w / 6 + 3, h * 3 / 4);

        p.drawRect(w / 6, h / 4 + 1, w / 8, h / 2);
        p.drawLine(w / 6, h / 4 + 1, w * 7 / 24 - 1, h * 3 / 4 - 1);
        
        p.setPen(QPen(Qt::DotLine));
        p.drawLine(w / 4 + 3, h / 2, w * 3 / 4, h / 2);
        p.setPen(QPen(Qt::SolidLine));
        
        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(w - 3, h - 3));
            
            p.fillRect(br, QBrush(white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        // paint signalization (black or yellow filled circle)
        if (iSoldDirection == 1)
            p.setBrush(QColor(yellow));
        else
            p.setBrush(QColor(black));

        p.drawEllipse(w * 3 / 4 - 3, h / 2 - 5, 11, 11);

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // motor
    else if (sSoldIcon == SYM_MDC){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);

        int w = pm.width();
        int h = pm.height();
        
        // paint signalization (black or yellow filled circle)
        p.setPen(QPen(black, 2));

        if (iSoldDirection == 1)
            p.setBrush(QColor(yellow));

        p.drawEllipse(w / 2 - 9, h - 18 - 1, 17, 17);

        // paint symbol
        // red connector left
        p.setPen(red);
        p.drawLine(3, h - 9 - 2, w / 2 - 10, h - 9 - 2);
        p.drawLine(w / 2 - 11, h - 9 - 3, w / 2 - 10, h - 9 - 3);
        p.drawLine(w / 2 - 11, h - 9 - 1, w / 2 - 10, h - 9 - 1);
        // plus sign
        p.drawLine(3, h - 9 - 6, 7, h - 9 - 6);
        p.drawLine(5, h - 9 - 8, 5, h - 9 - 4);
        
        // blue connector right
        p.setPen(blue);
        p.drawLine(w - 5, h - 9 - 2, w / 2 + 8, h - 9 - 2);
        p.drawLine(w / 2 + 9, h - 9 - 3, w / 2 + 8, h - 9 - 3);
        p.drawLine(w / 2 + 9, h - 9 - 1, w / 2 + 8, h - 9 - 1);
        // minus sign
        p.drawLine(w - 5, h - 9 - 6, w - 9, h - 9 - 6);

        // motor labels
        p.setPen(black);
        p.drawLine(w / 2 - 3, h - 9, w / 2 + 1, h - 9);
        p.drawPoint(w / 2 - 3, h - 7);
        p.drawPoint(w / 2 - 1, h - 7);
        p.drawPoint(w / 2 + 1, h - 7);

        QPointArray motor = QPointArray(5);
        motor.putPoints(0, 5, 0, 3, 0, 0, 2, 2, 4, 0, 4, 3);
        motor.translate(w / 2 - 3, h - 14);
        p.drawPolyline(motor);
        
        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(iSoldAddress_2);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopLeft(QPoint(w / 2 - br.width() / 2, 2));
            
            p.fillRect(br, QBrush(white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // decoupler
    else if (sSoldIcon == SYM_ENK){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        // paint track
        p.fillRect(0, pm.height() / 2 - 3, pm.width() - 1, 7,
                QBrush(black));
        
        // paint symbol
        p.fillRect(21, 10, 14, 3, QBrush(black));
        
        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, pm.height() / 2 - 2, 5, 5,
                        QBrush(lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(pm.width() / 3, pm.height() / 2,
                    2 * pm.width() / 3, pm.height() / 2);
            p.setPen(black);
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(pm.width()/2 + br.width()/2,
                        pm.height() - 3));
            
            p.fillRect(br, QBrush(white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        // paint signalization (white triagle)
        if (iSoldDirection == 1) {
            p.setPen(white);
            p.setBrush(QColor(white));
            QPointArray triangle = QPointArray(4);
            triangle.putPoints(0, 4, 21, 9, 27, 3, 28, 3, 34, 9);
            p.drawPolygon(triangle);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // blind element
    else if (sSoldIcon == SYM_BLD){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w - 1, 7, QBrush(black));
        
        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(w / 3, h / 2, 2 * w / 3, h / 2);
            p.setPen(black);
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            if (iSoldRotate == 1)
                br.moveTopLeft(QPoint(w/2 - br.width()/2, 2));
            else
                br.moveBottomRight(QPoint(w/2 + br.width()/2, h - 3));
            
            p.fillRect(br, QBrush(white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        // paint signalization
        if (iSoldDirection == 1)
            p.setPen(green);
        else
            p.setPen(red);
        
        p.drawLine(19, 12, 19 + 17, 12);
        p.drawLine(18, 13, 18 + 19, 13);
        p.drawLine(18, 21, 18 + 19, 21);
        p.drawLine(19, 22, 19 + 17, 22);

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // shunt wait signal
    else if (sSoldIcon == SYM_WS){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w - 1, 7, QBrush(black));
        
        // paint track button
        if (iSoldRotate == 1)
            p.drawPixmap(w / 6  - 4, h / 2 - 3,
                    QPixmap(button_gray_xpm));
        else
            p.drawPixmap(5 * w / 6 - 4 , h / 2 - 3, 
                    QPixmap(button_gray_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        if (iSoldRotate == 1)
            p.drawEllipse(w / 2 - 1, 5, 5, 5);
        else
            p.drawEllipse(w / 2 - 5, h - 10, 5, 5);

        // paint signal icon
        if (iSoldRotate == 1) {
            p.drawPixmap(6, 2, QPixmap(signal_wr_xpm));
            p.fillRect(22, 5, 2, 5, QBrush(black));
            p.drawLine(13, 7, 21, 7);
        }
        else {
            p.drawPixmap(w - 16 , h / 2 + 5, QPixmap(signal_w_xpm));
            p.fillRect(w - 25, 25, 2, 5, QBrush(black));
            p.drawLine(w - 15, 27, w - 23, 27);
        }
 
        // paint signal light
        if (iSoldDirection == 1) {
            p.setPen(QPen(white));
            p.setBrush(white);
        }
        else
            p.setBrush(darkGray);

        if (iSoldRotate == 1) {
            p.drawRect(4, 2, 3, 3);
            p.drawRect(14, 9, 3, 3);
        }
        else {
            p.drawRect(w - 18, h / 2 + 6, 3, 3);
            p.drawRect(w - 8, h / 2 + 13, 3, 3);
        }

        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
            p.setPen(QPen(black));
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            if (iSoldRotate == 1)
                br.moveBottomRight(QPoint(w/2 + br.width()/2,
                            h - 3));
            else
                br.moveTopLeft(QPoint(w/2 - br.width()/2, 2));
            
            p.fillRect(br, QBrush(white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // signal HSS
    else if (sSoldIcon == SYM_HSS){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w - 1, 7, QBrush(black));
        
        // paint track button
        if (iSoldRotate == 1) {
            p.drawPixmap(w / 6  - 4, h / 2 - 3,
                    QPixmap(button_red_xpm));
            p.drawPixmap(5 * w / 6  - 4, h / 2 - 3,
                    QPixmap(button_gray_xpm));
        }
        else {
            p.drawPixmap(5 * w / 6 - 4 , h / 2 - 3, 
                    QPixmap(button_red_xpm));
            p.drawPixmap(w / 6  - 4, h / 2 - 3,
                    QPixmap(button_gray_xpm));
        }

        // paint signal icon
        p.setBrush(black);
        if (iSoldRotate == 1) {
            p.drawEllipse(31, 4, 7, 7);
            p.fillRect(9, 4, 24, 7, QBrush(black));
            p.fillRect(41, 5, 2, 5, QBrush(black));
            p.drawLine(38, 7, 40, 7);
        }
        else {
            p.drawEllipse(17, h - 11, 7, 7);
            p.fillRect(22, h - 11, 24, 7, QBrush(black));
            p.fillRect(12, 25, 2, 5, QBrush(black));
            p.drawLine(14, 27, 16, 27);
        }

        /**
         * direction subtype bottom-left bottom-right top-left top-right
         * -------------------------------------------------------------
         *     0       0          y           y
         *     1       0                                 g        g
         *     0       6          y           y
         *     2       6          y                               g
         *     0       4          y           y
         *     1       4                                 g        g
         *     2       4          y                               g
         * -------------------------------------------------------------
         **/
 
        // fprintf(stderr, "dir: %d type: %d\n", iSoldDirection, iSoldSubType);

        // paint signal light
        // fix potential wrong direction value
        if (iSoldSubType == 6 && iSoldDirection == DIR_HP1)
            iSoldDirection = DIR_HP2;

        switch (iSoldDirection) {
            case DIR_HP0:              // HP0 => 0
                p.setBrush(red);
                if (iSoldRotate == 1) {
                    // bottom
                    p.drawEllipse(14, 4, 7, 7);
                    // top
                    p.setBrush(black);
                    p.drawEllipse(4, 4, 7, 7);
                }
                else {
                    // bottom
                    p.drawEllipse(w - 22, h - 11, 7, 7);
                    // top
                    p.setBrush(black);
                    p.drawEllipse(w - 12, h - 11, 7, 7);
                }
                break;
            case DIR_HP2:              // HP2 => 2
                if (pref.hp2) {
                    p.setBrush(yellow);
                    if (iSoldRotate == 1) {
                        // top
                        p.drawEllipse(4, 4, 7, 7);
                        // bottom
                        p.setBrush(black);
                        p.drawEllipse(14, 4, 7, 7);
                    }
                    else {
                        // top
                        p.drawEllipse(w - 12, h - 11, 7, 7);
                        // bottom
                        p.setBrush(black);
                        p.drawEllipse(w - 22, h - 11, 7, 7);
                    }
                    break;
                }
            case DIR_HP1:              // HP1 => 1
                p.setBrush(green);
                if (iSoldRotate == 1) {
                    // top
                    p.drawEllipse(4, 4, 7, 7);
                    // bottom
                    p.setBrush(black);
                    p.drawEllipse(14, 4, 7, 7);
                }
                else {
                    // top
                    p.drawEllipse(w - 12, h - 11, 7, 7);
                    // bottom
                    p.setBrush(black);
                    p.drawEllipse(w - 22, h - 11, 7, 7);
                }
                break;
            case DIR_SH1:              // SH1 => 3
                p.setBrush(black);
                if (iSoldRotate == 1) {
                    // top light position
                    p.drawEllipse(4, 4, 7, 7);
                    // shunt light
                    int startx = w - 22;
                    p.setPen(yellow);
                    p.drawLine(startx, 9, startx - 4, 5);
                    p.drawLine(startx + 1, 9, startx - 3, 5);
                }
                else {
                    // top light position
                    p.drawEllipse(w - 12, h - 11, 7, 7);
                    // shunt light
                    int startx = 19;
                    p.setPen(yellow);
                    p.drawLine(startx, h - 10, startx + 4, h - 6);
                    p.drawLine(startx + 1, h - 10, startx + 5, h - 6);
                }
                break;
        }

        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
            p.setPen(QPen(black));
        }

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        if (iSoldRotate == 1)
            p.drawEllipse(w - 11, 5, 5, 5);
        else
            p.drawEllipse(5, h - 10, 5, 5);

        // paint FfM
        if (ffm) {
            p.setBrush(ffmactive ? yellow : darkGray);
            if (iSoldRotate == 1)
                p.drawRect(5, h - 10, 6, 6);
            else
                p.drawRect(w - 11, 4, 6, 6);
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            if (iSoldRotate == 1)
                br.moveBottomRight(QPoint(w/2 + br.width()/2,
                            h - 3));
            else
                br.moveTopLeft(QPoint(w/2 - br.width()/2, 2));
            
            p.fillRect(br, QBrush(white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // signal HS
    else if (sSoldIcon == SYM_HS){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w - 1, 7, QBrush(black));
        
        // paint track button
        if (iSoldRotate == 1)
            p.drawPixmap(w / 6  - 4, h / 2 - 3,
                    QPixmap(button_red_xpm));
        else
            p.drawPixmap(5 * w / 6 - 4 , h / 2 - 3, 
                    QPixmap(button_red_xpm));

        // paint signal icon

        if (iSoldRotate == 1) {
            p.fillRect(7, 4, 10, 7, QBrush(black));
            p.fillRect(24, 5, 2, 5, QBrush(black));
            p.drawLine(21, 7, 24, 7);
        }
        else {
            p.fillRect(w - 20, h - 11, 10, 7, QBrush(black));
            p.fillRect(w - 27, 25, 2, 5, QBrush(black));
            p.drawLine(w - 23, 27, w - 25, 27);
        }

        /**
         * direction subtype bottom-left bottom-right top-left top-right
         * -------------------------------------------------------------
         *     0       0          y           y
         *     1       0                                 g        g
         *     0       6          y           y
         *     2       6          y                               g
         *     0       4          y           y
         *     1       4                                 g        g
         *     2       4          y                               g
         * -------------------------------------------------------------
         **/
 
        // fprintf(stderr, "dir: %d type: %d\n", iSoldDirection, iSoldSubType);

        // paint signal light
        // fix potential wrong direction value
        if (iSoldSubType == 6 && iSoldDirection == DIR_HP1)
            iSoldDirection = DIR_HP2;

        switch (iSoldDirection) {
            case DIR_HP0:              // HP0 => 0
                p.setBrush(red);
                if (iSoldRotate == 1) {
                    // bottom
                    p.drawEllipse(14, 4, 7, 7);
                    // top
                    p.setBrush(black);
                    p.drawEllipse(4, 4, 7, 7);
                }
                else {
                    // bottom
                    p.drawEllipse(w - 22, h - 11, 7, 7);
                    // top
                    p.setBrush(black);
                    p.drawEllipse(w - 12, h - 11, 7, 7);
                }
                break;
            case DIR_HP2:              // HP2 => 2
                if (pref.hp2) {
                    p.setBrush(yellow);
                    if (iSoldRotate == 1) {
                        // top
                        p.drawEllipse(4, 4, 7, 7);
                        // bottom
                        p.setBrush(black);
                        p.drawEllipse(14, 4, 7, 7);
                    }
                    else {
                        // top
                        p.drawEllipse(w - 12, h - 11, 7, 7);
                        // bottom
                        p.setBrush(black);
                        p.drawEllipse(w - 22, h - 11, 7, 7);
                    }
                    break;
                }
            case DIR_HP1:              // HP1 => 1
                p.setBrush(green);
                if (iSoldRotate == 1) {
                    // top
                    p.drawEllipse(4, 4, 7, 7);
                    // bottom
                    p.setBrush(black);
                    p.drawEllipse(14, 4, 7, 7);
                }
                else {
                    // top
                    p.drawEllipse(w - 12, h - 11, 7, 7);
                    // bottom
                    p.setBrush(black);
                    p.drawEllipse(w - 22, h - 11, 7, 7);
                }
                break;
        }

        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
            p.setPen(QPen(black));
        }

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        if (iSoldRotate == 1)
            p.drawEllipse(w / 2, 5, 5, 5);
        else
            p.drawEllipse(w / 2 - 6, h - 10, 5, 5);

        // paint FfM
        if (ffm) {
            p.setBrush(ffmactive ? yellow : darkGray);
            if (iSoldRotate == 1)
                p.drawRect(5, h - 10, 6, 6);
            else
                p.drawRect(w - 11, 4, 6, 6);
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            if (iSoldRotate == 1)
                br.moveBottomRight(QPoint(w/2 + br.width()/2,
                            h - 3));
            else
                br.moveTopLeft(QPoint(w/2 - br.width()/2, 2));
            
            p.fillRect(br, QBrush(white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // signal VS
    else if (sSoldIcon == SYM_VS){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w - 1, 7, QBrush(black));
        
        // paint signal icon
        QPointArray icon = QPointArray(6);
        icon.putPoints(0, 6, 0, 0, 0, 3, 3, 6, 12, 6, 12, 3, 9, 0);
        p.setBrush(black);

        if (iSoldRotate == 1) {
            icon.translate(4, 4);
            p.drawPolygon(icon);
            p.fillRect(20, 5, 2, 5, QBrush(black));
            p.drawLine(17, 7, 20, 7);
        }
        else {
            icon.translate(w - 18, h - 11);
            p.drawPolygon(icon);
            p.fillRect(w - 23, 25, 2, 5, QBrush(black));
            p.drawLine(w - 18, 27, w - 21, 27);
        }

        /**
         * direction subtype bottom-left bottom-right top-left top-right
         * -------------------------------------------------------------
         *     0       0          y           y
         *     1       0                                 g        g
         *     0       6          y           y
         *     2       6          y                               g
         *     0       4          y           y
         *     1       4                                 g        g
         *     2       4          y                               g
         * -------------------------------------------------------------
         **/
 
        // fprintf(stderr, "dir: %d type: %d\n", iSoldDirection, iSoldSubType);

        // paint signal light
        // fix potential wrong direction value
        if (iSoldSubType == 6 && iSoldDirection == DIR_HP1)
            iSoldDirection = DIR_HP2;

        switch (iSoldDirection) {
            case DIR_HP0:              // VS0 => 0
                if (iSoldRotate == 1) {
                    // bottom left
                    p.fillRect(14, 8, 2, 2, QBrush(yellow));
                    // bottom right
                    p.fillRect(12, 5, 2, 2, QBrush(yellow));
                }
                else {
                    // bottom left
                    p.fillRect(w - 17, h - 10, 2, 2, QBrush(yellow));
                    // bottom right
                    p.fillRect(w - 15, h - 7, 2, 2, QBrush(yellow));
                }
                break;
            case DIR_HP2:              // VS2 => 2
                if (pref.hp2) {
                    if (iSoldRotate == 1) {
                        // bottom left
                        p.fillRect(14, 8, 2, 2, QBrush(yellow));
                        //top right
                        p.fillRect(5, 5, 2, 2, QBrush(green));
                    }
                    else {
                        // bottom left
                        p.fillRect(w - 17, h - 10, 2, 2, QBrush(yellow));
                        //top right
                        p.fillRect(w - 8, h - 7, 2, 2, QBrush(green));
                    }
                    break;
                }
            case DIR_HP1:              // VS1 => 1
                if (iSoldRotate == 1) {
                    //top left
                    p.fillRect(7, 8, 2, 2, QBrush(green));
                    //top right
                    p.fillRect(5, 5, 2, 2, QBrush(green));
                }
                else {
                    //top left
                    p.fillRect(w - 10, h - 10, 2, 2, QBrush(green));
                    //top right
                    p.fillRect(w - 8, h - 7, 2, 2, QBrush(green));
                }
                break;
        }

        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
            p.setPen(QPen(black));
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            if (iSoldRotate == 1)
                br.moveBottomRight(QPoint(w/2 + br.width()/2,
                            h - 3));
            else
                br.moveTopLeft(QPoint(w/2 - br.width()/2, 2));
            
            p.fillRect(br, QBrush(white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // signal ZP
    else if (sSoldIcon == SYM_ZP){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w - 1, 7, QBrush(black));
        
        // paint lock light
        if (lockCounter == 0)
            p.setBrush(darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        if (iSoldRotate == 1)
            p.drawEllipse(w / 2 - 2, 5, 5, 5);
        else
            p.drawEllipse(w / 2 - 3, h - 10, 5, 5);

        // paint signal icon
        if (iSoldRotate == 1) {
            p.fillRect(21, 5, 2, 5, QBrush(black));
            p.fillRect(5, 2, 11, 11, QBrush(black));
            p.drawLine(16, 7, 20, 7);
        }
        else {
            p.fillRect(w - 23, 25, 2, 5, QBrush(black));
            p.fillRect(w - 16, 22, 11, 11, QBrush(black));
            p.drawLine(w - 17, 27, w - 21, 27);
        }
 
        // paint signal light
        QPointArray lights = QPointArray(12);
        lights.putPoints(0, 12, 3, 0, 5, 0, 1, 1, 7, 1, 0, 3, 8, 3,
                0, 5, 8, 5, 1, 7, 7, 7, 3, 8, 5, 8);

        if (iSoldRotate == 1)
            lights.translate(6, 3);
        else
            lights.translate(w - 15, 23);

        if (iSoldDirection == 1)
            p.setPen(QPen(green));
        else
            p.setPen(QPen(darkGray));

        p.drawPoints(lights);
        
        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
            p.setPen(QPen(black));
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            if (iSoldRotate == 1)
                br.moveBottomRight(QPoint(w/2 + br.width()/2,
                            h - 3));
            else
                br.moveTopLeft(QPoint(w/2 - br.width()/2, 2));
            
            p.fillRect(br, QBrush(white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // straight track
    else if (sSoldIcon == SYM_GER){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w - 1, 7, QBrush(black));
        
        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(w / 3, h / 2, 2 * w / 3, h / 2);
            p.setPen(QPen(black));
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 1);
            p.setFont(f);

            QRect br = pm.rect();
            br.setHeight(h / 2 - 7);
            if (iSoldRotate == 1)
                br.moveBottom(h - 4);
            else
                br.moveTop(1);
            
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, sSoldText);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // track turn right and left
    else if (sSoldIcon == SYM_KUR || sSoldIcon == SYM_KUL){
        bool left = (sSoldIcon == SYM_KUL);

        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        if (iSoldRotate == 1)
            p.fillRect(w / 2 - 1, h / 2 - 3, w - 1, 7, QBrush(black));
        else
            p.fillRect(0, h / 2 - 3, w / 2 + 2, 7, QBrush(black));

        p.save();
        p.translate(w / 2, h / 2);

        if (iSoldRotate == 1)
            if (left)
                p.rotate(WANGLE);
            else
                p.rotate(-WANGLE);
        else
            if (left)
                p.rotate(-SANGLE);
            else
                p.rotate(SANGLE);

        p.setPen(QPen(black, 7));
        p.drawLine(0, 0, w / 2 + 5, 0);
        p.restore();

        // paint track lights
        if (iSoldLEDoff == 1) {
            if (iSoldRotate == 1) {
                for (int i = 4; i < 7; ++i)
                    p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                            QBrush(lightGray));
                
                p.save();
                p.translate(w / 2, h / 2);

                if (left)
                    p.rotate(WANGLE);
                else
                    p.rotate(-WANGLE);

                for (int i = 0; i < 3; ++i)
                    p.fillRect(5 + 7 * i, -2, 5, 5, QBrush(lightGray));
                p.restore();
            }
            else {
                for (int i = 0; i < 3; ++i)
                    p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                            QBrush(lightGray));
                p.save();
                p.translate(w / 2, h / 2);

                if (left)
                    p.rotate(-SANGLE);
                else
                    p.rotate(SANGLE);

                for (int i = 0; i < 3; ++i)
                    p.fillRect(5 + 7 * i, -2, 5, 5, QBrush(lightGray));
                p.restore();
            }
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            if (iSoldRotate == 1) {
                p.fillRect(w / 2, h / 2 - 1, w / 5 , 3, QBrush(c));
                p.save();
                p.translate(w / 2, h / 2);

                if (left)
                    p.rotate(WANGLE);
                else
                    p.rotate(-WANGLE);

                p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
                p.drawLine(0, 0, w / 5, 0);
                p.restore();
            }
            else {
                p.fillRect(w / 3, h / 2 - 1, w / 5 , 3, QBrush(c));
                p.save();
                p.translate(w / 2, h / 2);

                if (left)
                    p.rotate(-SANGLE);
                else
                    p.rotate(SANGLE);

                p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
                p.drawLine(0, 0, w / 5, 0);
                p.restore();
            }
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }

    // diagonal track right and left
    else if (sSoldIcon == SYM_DIR || sSoldIcon == SYM_DIL){
        bool left = (sSoldIcon == SYM_DIL);

        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.setPen(QPen(black, 7));
        if (left)
            p.drawLine(0, h - 1, w - 1, 0);
        else
            p.drawLine(0, 0, w - 1, h - 1);

        // paint track lights
        if (iSoldLEDoff == 1) {
            p.save();
            p.translate(w / 2, h / 2);

            if (left)
                p.rotate(-SANGLE);
            else
                p.rotate(SANGLE);

            for (int i = -3; i < 4; ++i)
                p.fillRect(-3 + 7 * i, -2, 5, 5, QBrush(lightGray));

            p.restore();
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            p.save();
            p.translate(w / 2, h / 2);

            if (left)
                p.rotate(-SANGLE);
            else
                p.rotate(SANGLE);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(-w / 6, 0, w / 6, 0);

            p.restore();
        }
        p.end();
        setPaletteBackgroundPixmap(pm);
    }

    // diagonal crossing (hosentraeger)
    else if (sSoldIcon == SYM_KRH){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.setPen(QPen(black, 7));
        p.drawLine(0, 0, w - 1, h - 1);
        p.drawLine(0, h - 1, w - 1, 0);
        
        // paint track lights
        if (iSoldLEDoff == 1) {
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(lightGray));
            p.rotate(2 * SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(lightGray));
            p.restore();
        }

        /**
         * routedtrack routed occupied track1 track2
         * -----------------------------------------
         *      0        0       0       g      g
         *      1        1       0       y      g 
         *      2        1       0       g      y
         *      0        0       1       r      r
         *      1        1       1       r      g
         *      2        1       1       g      r
         * -----------------------------------------
         **/
        else {
            QColor c1, c2;
            if (occupied)
                if (routedtrack == 1) {
                    c1 = QColor(red);
                    c2 = QColor(darkGray);
                }
                else if (routedtrack == 2) {
                    c1 = QColor(darkGray);
                    c2 = QColor(red);
                }
                else {
                    c1 = QColor(red);
                    c2 = QColor(red);
                }
            else {
                if (routed)
                    if (routedtrack == 1) {
                        c1 = QColor(255, 225, 0);
                        c2 = QColor(darkGray);
                    }
                    else {
                        c1 = QColor(darkGray);
                        c2 = QColor(255, 225, 0);
                    }
                else {
                    c1 = QColor(darkGray);
                    c2 = QColor(darkGray);
                }
            }
            int startx = w / 5 - 2;
            int stopx = 2 * w / 5 - 4;
            int starty = h / 5 - 2;
            int stopy = 2 * h / 5 - 3;

            // track 1
            p.setPen(QPen(c1, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(startx, h - starty - 1, stopx, h - stopy - 1);
            p.drawLine(w - startx - 1, starty, w - stopx - 1, stopy);

            // track 2
            p.setPen(QPen(c2, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(startx, starty, stopx, stopy);
            p.drawLine(w - startx - 1, h - starty - 1, w - stopx - 1,
                    h - stopy - 1);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // left crossing
    else if (sSoldIcon == SYM_KRL){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.setPen(QPen(black, 7));
        p.drawLine(0, h / 2, w - 1, h / 2);
        p.drawLine(0, h - 1, w - 1, 0);
        
        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(lightGray));
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(lightGray));
            p.restore();
        }

        /**
         * routedtrack routed occupied track1 track2
         * -----------------------------------------
         *      0        0       0       g      g
         *      1        1       0       y      g 
         *      2        1       0       g      y
         *      0        0       1       r      r
         *      1        1       1       r      g
         *      2        1       1       g      r
         * -----------------------------------------
         **/
        else {
            QColor c1, c2;
            if (occupied)
                if (routedtrack == 1) {
                    c1 = QColor(red);
                    c2 = QColor(darkGray);
                }
                else if (routedtrack == 2) {
                    c1 = QColor(darkGray);
                    c2 = QColor(red);
                }
                else {
                    c1 = QColor(red);
                    c2 = QColor(red);
                }
            else {
                if (routed)
                    if (routedtrack == 1) {
                        c1 = QColor(255, 225, 0);
                        c2 = QColor(darkGray);
                    }
                    else {
                        c1 = QColor(darkGray);
                        c2 = QColor(255, 225, 0);
                    }
                else {
                    c1 = QColor(darkGray);
                    c2 = QColor(darkGray);
                }
            }
            int startx = w / 5 - 2;
            int stopx = 2 * w / 5 - 4;
            int starty = h / 5 - 2;
            int stopy = 2 * h / 5 - 3;

            // diagonal track (1)
            p.setPen(QPen(c1, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(startx, h - starty - 1, stopx, h - stopy - 1);
            p.drawLine(w - startx - 1, starty, w - stopx - 1, stopy);

            // straight track (2)
            p.setPen(QPen(c2, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(startx, h /2, stopx, h / 2);
            p.drawLine(w - startx - 1, h / 2, w - stopx - 1, h / 2);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // right crossing
    else if (sSoldIcon == SYM_KRR){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.setPen(QPen(black, 7));
        p.drawLine(0, h / 2, w - 1, h / 2);
        p.drawLine(0, 0, w - 1, h - 1);
        
        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(lightGray));
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(lightGray));
            p.restore();
        }

        /**
         * routedtrack routed occupied track1 track2
         * -----------------------------------------
         *      0        0       0       g      g
         *      1        1       0       y      g 
         *      2        1       0       g      y
         *      0        0       1       r      r
         *      1        1       1       r      g
         *      2        1       1       g      r
         * -----------------------------------------
         **/
        else {
            QColor c1, c2;
            if (occupied)
                if (routedtrack == 1) {
                    c1 = QColor(red);
                    c2 = QColor(darkGray);
                }
                else if (routedtrack == 2) {
                    c1 = QColor(darkGray);
                    c2 = QColor(red);
                }
                else {
                    c1 = QColor(red);
                    c2 = QColor(red);
                }
            else {
                if (routed)
                    if (routedtrack == 1) {
                        c1 = QColor(255, 225, 0);
                        c2 = QColor(darkGray);
                    }
                    else {
                        c1 = QColor(darkGray);
                        c2 = QColor(255, 225, 0);
                    }
                else {
                    c1 = QColor(darkGray);
                    c2 = QColor(darkGray);
                }
            }
            int startx = w / 5 - 2;
            int stopx = 2 * w / 5 - 4;
            int starty = h / 5 - 2;
            int stopy = 2 * h / 5 - 3;

            // diagonal track (1)
            p.setPen(QPen(c1, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(startx, starty, stopx, stopy);
            p.drawLine(w - startx - 1, h - starty - 1, w - stopx - 1,
                    h - stopy - 1);

            // straight track (2)
            p.setPen(QPen(c2, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(startx, h / 2, stopx, h / 2);
            p.drawLine(w - startx - 1, h / 2, w - stopx - 1, h / 2);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // single slip switch left
    else if (sSoldIcon == SYM_EKL){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.setPen(QPen(black, 7));
        p.drawLine(0, h / 2, w - 1, h / 2);
        p.drawLine(0, h - 1, w - 1, 0);
        
        // paint drive symbol
        p.setPen(QPen(black, 1));
        if (iSoldRotate == 1) {
            p.drawLine(w / 2 + 1, h / 2 + 5, w / 2 + 7, h / 2 + 5);
            p.drawLine(w / 2 + 2, h / 2 + 5, w / 2 - 3, h / 2 + 7);
        }
        else {
            p.drawLine(w / 2 - 7, h / 2 - 5, w / 2 - 2, h / 2 - 5);
            p.drawLine(w / 2 - 2, h / 2 - 5, w / 2 + 3, h / 2 - 8);
        }

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        if (iSoldRotate == 1)
            p.drawEllipse(3 * w / 4 - 2, h - 11, 5, 5);
        else
            p.drawEllipse(w / 4, 6, 5, 5);

        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(lightGray));
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(lightGray));
            p.restore();
        }

        /**
         * lastdir  currentdir  newdir  c1  c2  c3  c4
         * -------------------------------------------
         *     0       0/0        0     -   -   -   -
         *     1       1/1        1     -   -   -   -
         *     2       2/2        2     -   -   -   -
         *                   
         *     0       0/1        1     -   1   -   2 
         *     1       1/0        0     -   2   -   1
         *     
         *     1       1/2        2     1   -   2   -
         *     2       2/1        1     2   -   1   -
         *                   
         *     2       2/0        0     2   2   1   1
         *     0       0/2        2     1   1   2   2
         * -------------------------------------------
         **/
        else {
            QColor c1, c2, c3, c4;
            if (occupied) {
                if (iSoldDirection == 0) {
                    c1 = QColor(red);
                    c2 = QColor(red);
                    c3 = QColor(darkGray);
                    c4 = QColor(darkGray);
                }
                else if (iSoldDirection == 1) {
                    c1 = QColor(red);
                    c2 = QColor(darkGray);
                    c3 = QColor(darkGray);
                    c4 = QColor(red);
                }
                else if (iSoldDirection == 2) {
                    c1 = QColor(darkGray);
                    c2 = QColor(darkGray);
                    c3 = QColor(red);
                    c4 = QColor(red);
                }
                // error indication
                else {
                    c1 = QColor(red);
                    c2 = QColor(red);
                    c3 = QColor(red);
                    c4 = QColor(red);
                }
            }
            else {
                if (iSoldDirection == 0) {
                    if (!lightson && ((lastdir == 0 && newdir == 2) ||
                            (lastdir == 2 && newdir == 0)))
                        c1 = QColor(darkGray);
                    else
                        c1 = QColor(255, 225, 0);
                    
                    if (!lightson && ((lastdir != 0 && newdir == 0) ||
                            (lastdir == 0 && newdir != 0)))
                        c2 = QColor(darkGray);
                    else
                        c2 = QColor(255, 225, 0);
                    
                    c3 = QColor(darkGray);
                    c4 = QColor(darkGray);
                }
                else if (iSoldDirection == 1) {
                    if (!lightson && ((lastdir == 1 && newdir == 2) ||
                        (lastdir == 2 && newdir == 1)))
                        c1 = QColor(darkGray);
                    else
                        c1 = QColor(255, 225, 0);
                    
                    c2 = QColor(darkGray);
                    c3 = QColor(darkGray);
                    
                    if (!lightson && ((lastdir == 1 && newdir == 0) ||
                                (lastdir == 0 && newdir == 1)))
                        c4 = QColor(darkGray);
                    else
                        c4 = QColor(255, 225, 0);
                }
                else if (iSoldDirection == 2) {
                    c1 = QColor(darkGray);
                    c2 = QColor(darkGray);
                    
                    if (!lightson && ((lastdir == 2 && newdir != 2) ||
                                (lastdir != 2 && newdir == 2)))
                        c3 = QColor(darkGray);
                    else
                        c3 = QColor(255, 225, 0);
                    
                    if (!lightson && ((lastdir == 2 && newdir == 0) ||
                                (lastdir == 0 && newdir == 2)))
                        c4 = QColor(darkGray);
                    else
                        c4 = QColor(255, 225, 0);
                }
                // error indication
                else {
                    c1 = QColor(255, 225, 0);
                    c2 = QColor(255, 225, 0);
                    c3 = QColor(255, 225, 0);
                    c4 = QColor(255, 225, 0);
                }
            }
            int startx = w / 5 - 2;
            int stopx = 2 * w / 5 - 4;
            int starty = h / 5 - 2;
            int stopy = 2 * h / 5 - 3;

            // swap track indicator assignment if element is rotated
            if (iSoldRotate == 1) {
                // straight track (1)
                p.setPen(QPen(c1, 3, Qt::SolidLine, Qt::RoundCap,
                            Qt::MiterJoin));
                p.drawLine(w - startx - 1, h / 2, w - stopx - 1, h / 2);
                // straight track (2)
                p.setPen(QPen(c2, 3, Qt::SolidLine, Qt::RoundCap,
                            Qt::MiterJoin));
                p.drawLine(startx, h /2, stopx, h / 2);
                // diagonal track (3)
                p.setPen(QPen(c3, 3, Qt::SolidLine, Qt::RoundCap,
                            Qt::MiterJoin));
                p.drawLine(w - startx - 1, starty, w - stopx - 1, stopy);
                // diagonal track (4)
                p.setPen(QPen(c4, 3, Qt::SolidLine, Qt::RoundCap,
                            Qt::MiterJoin));
                p.drawLine(startx, h - starty - 1, stopx, h - stopy - 1);
            }
            else {
                // straight track (1)
                p.setPen(QPen(c1, 3, Qt::SolidLine, Qt::RoundCap,
                            Qt::MiterJoin));
                p.drawLine(startx, h /2, stopx, h / 2);
                // straight track (2)
                p.setPen(QPen(c2, 3, Qt::SolidLine, Qt::RoundCap,
                            Qt::MiterJoin));
                p.drawLine(w - startx - 1, h / 2, w - stopx - 1, h / 2);
                // diagonal track (3)
                p.setPen(QPen(c3, 3, Qt::SolidLine, Qt::RoundCap,
                            Qt::MiterJoin));
                p.drawLine(startx, h - starty - 1, stopx, h - stopy - 1);
                // diagonal track (4)
                p.setPen(QPen(c4, 3, Qt::SolidLine, Qt::RoundCap,
                            Qt::MiterJoin));
                p.drawLine(w - startx - 1, starty, w - stopx - 1, stopy);
            }

            p.setPen(QPen(black, 1));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));
        
        // paint text label
        p.setPen(black);
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            if (iSoldRotate == 1)
                br.moveTopLeft(QPoint(2, 2));
            else
                br.moveBottomRight(QPoint(w - 3, h - 3));

            p.fillRect(br, QBrush(white));
            p.setBrush(white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // single slip switch right
    else if (sSoldIcon == SYM_EKR){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.setPen(QPen(black, 7));
        p.drawLine(0, h / 2, w - 1, h / 2);
        p.drawLine(0, 0, w - 1, h - 1);
        
        // paint drive symbol
        p.setPen(QPen(black, 1));
        if (iSoldRotate == 1) {
            p.drawLine(w / 2 - 1, h / 2 + 5, w / 2 - 7, h / 2 + 5);
            p.drawLine(w / 2 - 3, h / 2 + 5, w / 2 + 2, h / 2 + 7);
        }
        else {
            p.drawLine(w / 2 + 7, h / 2 - 5, w / 2 + 2, h / 2 - 5);
            p.drawLine(w / 2 + 2, h / 2 - 5, w / 2 - 3, h / 2 - 8);
        }

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        if (iSoldRotate == 1)
            p.drawEllipse(w / 4, h - 11, 5, 5);
        else
            p.drawEllipse(3 * w/ 4 - 2 - 2  , 6, 5, 5);

        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(lightGray));
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(lightGray));
            p.restore();
        }

        /**
         * lastdir  currentdir  newdir  c1  c2  c3  c4
         * -------------------------------------------
         *     0       0/0        0     -   -   -   -
         *     1       1/1        1     -   -   -   -
         *     2       2/2        2     -   -   -   -
         *                   
         *     0       0/1        1     -   1   -   2 
         *     1       1/0        0     -   2   -   1
         *     
         *     1       1/2        2     1   -   2   -
         *     2       2/1        1     2   -   1   -
         *                   
         *     2       2/0        0     2   2   1   1
         *     0       0/2        2     1   1   2   2
         * -------------------------------------------
         **/
        else {
            QColor c1, c2, c3, c4;
            if (occupied) {
                if (iSoldDirection == 0) {
                    c1 = QColor(red);
                    c2 = QColor(red);
                    c3 = QColor(darkGray);
                    c4 = QColor(darkGray);
                }
                else if (iSoldDirection == 1) {
                    c1 = QColor(red);
                    c2 = QColor(darkGray);
                    c3 = QColor(darkGray);
                    c4 = QColor(red);
                }
                else if (iSoldDirection == 2) {
                    c1 = QColor(darkGray);
                    c2 = QColor(darkGray);
                    c3 = QColor(red);
                    c4 = QColor(red);
                }
                // error indication
                else {
                    c1 = QColor(red);
                    c2 = QColor(red);
                    c3 = QColor(red);
                    c4 = QColor(red);
                }
            }
            else {
                if (iSoldDirection == 0) {
                    if (!lightson && ((lastdir == 0 && newdir == 2) ||
                            (lastdir == 2 && newdir == 0)))
                        c1 = QColor(darkGray);
                    else
                        c1 = QColor(255, 225, 0);
                    
                    if (!lightson && ((lastdir == 0 && newdir != 0) ||
                            (lastdir != 0 && newdir == 0)))
                        c2 = QColor(darkGray);
                    else
                        c2 = QColor(255, 225, 0);
                    
                    c3 = QColor(darkGray);
                    c4 = QColor(darkGray);
                }
                else if (iSoldDirection == 1) {
                    if (!lightson && ((lastdir == 1 && newdir == 2) ||
                        (lastdir == 2 && newdir == 1)))
                        c1 = QColor(darkGray);
                    else
                        c1 = QColor(255, 225, 0);
                    
                    c2 = QColor(darkGray);
                    c3 = QColor(darkGray);
                    
                    if (!lightson && ((lastdir == 1 && newdir == 0) ||
                                (lastdir == 0 && newdir == 1)))
                        c4 = QColor(darkGray);
                    else
                        c4 = QColor(255, 225, 0);
                }
                else if (iSoldDirection == 2) {
                    c1 = QColor(darkGray);
                    c2 = QColor(darkGray);
                    
                    if (!lightson && ((lastdir == 2 && newdir != 2) ||
                                (lastdir != 2 && newdir == 2)))
                        c3 = QColor(darkGray);
                    else
                        c3 = QColor(255, 225, 0);
                    
                    if (!lightson && ((lastdir == 2 && newdir == 0) ||
                                (lastdir == 0 && newdir == 2)))
                        c4 = QColor(darkGray);
                    else
                        c4 = QColor(255, 225, 0);
                }
                // error indication
                else {
                    c1 = QColor(255, 225, 0);
                    c2 = QColor(255, 225, 0);
                    c3 = QColor(255, 225, 0);
                    c4 = QColor(255, 225, 0);
                }
            }
            int startx = w / 5 - 2;
            int stopx = 2 * w / 5 - 4;
            int starty = h / 5 - 2;
            int stopy = 2 * h / 5 - 3;

            // swap track indicator assignment if element is rotated
            if (iSoldRotate == 1) {
                // straight track (1)
                p.setPen(QPen(c1, 3, Qt::SolidLine, Qt::RoundCap,
                            Qt::MiterJoin));
                p.drawLine(startx, h /2, stopx, h / 2);
                // straight track (2)
                p.setPen(QPen(c2, 3, Qt::SolidLine, Qt::RoundCap,
                            Qt::MiterJoin));
                p.drawLine(w - startx - 1, h / 2, w - stopx - 1, h / 2);
                // diagonal track (3)
                p.setPen(QPen(c3, 3, Qt::SolidLine, Qt::RoundCap,
                            Qt::MiterJoin));
                p.drawLine(startx, starty, stopx, stopy);
                // diagonal track (4)
                p.setPen(QPen(c4, 3, Qt::SolidLine, Qt::RoundCap,
                            Qt::MiterJoin));
                p.drawLine(w - startx - 1, h - starty - 1, w - stopx - 1,
                        h - stopy - 1);
            }
            else {
                // straight track (1)
                p.setPen(QPen(c1, 3, Qt::SolidLine, Qt::RoundCap,
                            Qt::MiterJoin));
                p.drawLine(w - startx - 1, h / 2, w - stopx - 1, h / 2);
                // straight track (2)
                p.setPen(QPen(c2, 3, Qt::SolidLine, Qt::RoundCap,
                            Qt::MiterJoin));
                p.drawLine(startx, h /2, stopx, h / 2);
                // diagonal track (3)
                p.setPen(QPen(c3, 3, Qt::SolidLine, Qt::RoundCap,
                            Qt::MiterJoin));
                p.drawLine(w - startx - 1, h - starty - 1, w - stopx - 1,
                        h - stopy - 1);
                // diagonal track (4)
                p.setPen(QPen(c4, 3, Qt::SolidLine, Qt::RoundCap,
                            Qt::MiterJoin));
                p.drawLine(startx, starty, stopx, stopy);
            }

            p.setPen(QPen(black, 1));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));
        
        // paint text label
        p.setPen(black);
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            if (iSoldRotate == 1)
                br.moveTopRight(QPoint(w - 3, 2));
            else
                br.moveBottomLeft(QPoint(2, h - 3));

            p.fillRect(br, QBrush(white));
            p.setBrush(white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // double slip switch left
    else if (sSoldIcon == SYM_DKL){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.setPen(QPen(black, 7));
        p.drawLine(0, h / 2, w - 1, h / 2);
        p.drawLine(0, h - 1, w - 1, 0);
        
        // paint drive symbol
        p.setPen(QPen(black, 1));
        p.drawLine(w / 2 + 1, h / 2 + 5, w / 2 + 7, h / 2 + 5);
        p.drawLine(w / 2 + 2, h / 2 + 5, w / 2 - 3, h / 2 + 7);
        p.drawLine(w / 2 - 7, h / 2 - 5, w / 2 - 2, h / 2 - 5);
        p.drawLine(w / 2 - 2, h / 2 - 5, w / 2 + 3, h / 2 - 8);

        // paint lock lights TODO: lock only one
        if (lockCounter == 0)
            p.setBrush(darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 + 4, h - 11, 5, 5);
        p.drawEllipse(w / 4, 6, 5, 5);

        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(lightGray));
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(lightGray));
            p.restore();
        }

        //TODO: optimize, color selection is the same for DKL and DKR
        /**
         * lastdir  currentdir  newdir  c1  c2  c3  c4
         * -------------------------------------------
         *     0       0/0        0     -   -   -   -
         *     1       1/1        1     -   -   -   -
         *     2       2/2        2     -   -   -   -
         *     3       3/3        3     -   -   -   -
         *                   
         *     0       0/1        1     -   1   -   2 
         *     1       1/0        0     -   2   -   1
         *                   
         *     0       0/2        2     1   1   2   2
         *     2       2/0        0     2   2   1   1
         *                   
         *     0       0/3        3     1   -   2   -
         *     3       3/0        0     2   -   1   -
         *     
         *     1       1/2        2     1   -   2   -
         *     2       2/1        1     2   -   1   -
         *     
         *     1       1/3        3     1   2   2   1
         *     3       3/1        1     2   1   1   2
         *                   
         *     2       2/3        3     -   2   -   1
         *     3       3/2        2     -   1   -   2
         * -------------------------------------------
         **/
        else {
            QColor c1, c2, c3, c4;
            if (occupied) {
                if (iSoldDirection == 0) {
                    c1 = QColor(red);
                    c2 = QColor(red);
                    c3 = QColor(darkGray);
                    c4 = QColor(darkGray);
                }
                else if (iSoldDirection == 1) {
                    c1 = QColor(red);
                    c2 = QColor(darkGray);
                    c3 = QColor(darkGray);
                    c4 = QColor(red);
                }
                else if (iSoldDirection == 2) {
                    c1 = QColor(darkGray);
                    c2 = QColor(darkGray);
                    c3 = QColor(red);
                    c4 = QColor(red);
                }
                else if (iSoldDirection == 3) {
                    c1 = QColor(darkGray);
                    c2 = QColor(red);
                    c3 = QColor(red);
                    c4 = QColor(darkGray);
                }
                // error indication
                else {
                    c1 = QColor(red);
                    c2 = QColor(red);
                    c3 = QColor(red);
                    c4 = QColor(red);
                }
            }
            else {
                if (iSoldDirection == 0) {
                    if (!lightson && ((lastdir == 0 && newdir >= 2) ||
                            (lastdir >= 2 && newdir == 0)))
                        c1 = QColor(darkGray);
                    else
                        c1 = QColor(255, 225, 0);
                    
                    if (!lightson && (((lastdir == 1 || lastdir == 2) &&
                                    newdir == 0) ||
                            (lastdir == 0 && (newdir == 1 || newdir == 2))))
                        c2 = QColor(darkGray);
                    else
                        c2 = QColor(255, 225, 0);
                    
                    c3 = QColor(darkGray);
                    c4 = QColor(darkGray);
                }
                else if (iSoldDirection == 1) {
                    if (!lightson && ((lastdir == 1 &&
                                    (newdir == 2) || newdir == 3) ||
                                ((lastdir == 2 || newdir == 3) &&
                                 newdir == 1)))
                        c1 = QColor(darkGray);
                    else
                        c1 = QColor(255, 225, 0);
                    
                    c2 = QColor(darkGray);
                    c3 = QColor(darkGray);
                    
                    if (!lightson && ((lastdir == 1 &&
                                    (newdir == 0) || newdir == 3) ||
                                ((lastdir == 0 || lastdir == 3) &&
                                 newdir == 1)))
                        c4 = QColor(darkGray);
                    else
                        c4 = QColor(255, 225, 0);
                }
                else if (iSoldDirection == 2) {
                    c1 = QColor(darkGray);
                    c2 = QColor(darkGray);
                    
                    if (!lightson && ((lastdir == 2 &&
                                    (newdir == 0 || newdir == 1)) ||
                                ((lastdir == 0 || lastdir == 1) &&
                                 newdir == 2)))
                        c3 = QColor(darkGray);
                    else
                        c3 = QColor(255, 225, 0);
                    
                    if (!lightson && ((lastdir == 2 &&
                                    (newdir == 0 || newdir == 3)) ||
                                ((lastdir == 0 || lastdir == 3) &&
                                 newdir == 2)))
                        c4 = QColor(darkGray);
                    else
                        c4 = QColor(255, 225, 0);
                }
                else if (iSoldDirection == 3) {
                    c1 = QColor(darkGray);
                    
                    if (!lightson && ((lastdir == 3 &&
                                    (newdir == 1 || newdir == 2)) ||
                                ((lastdir == 1 || lastdir == 2) &&
                                 newdir == 3)))
                        c2 = QColor(darkGray);
                    else
                        c2 = QColor(255, 225, 0);
                    
                    if (!lightson && ((lastdir == 3 &&
                                    (newdir == 0 || newdir == 1)) ||
                                ((lastdir == 0 || lastdir == 1) &&
                                 newdir == 3)))
                        c3 = QColor(darkGray);
                    else
                        c3 = QColor(255, 225, 0);

                    c4 = QColor(darkGray);
                }
                // error indication
                else {
                    c1 = QColor(255, 225, 0);
                    c2 = QColor(255, 225, 0);
                    c3 = QColor(255, 225, 0);
                    c4 = QColor(255, 225, 0);
                }
            }

            int startx = w / 5 - 2;
            int stopx = 2 * w / 5 - 4;
            int starty = h / 5 - 2;
            int stopy = 2 * h / 5 - 3;

            // straight track (1)
            p.setPen(QPen(c1, 3, Qt::SolidLine, Qt::RoundCap,
                        Qt::MiterJoin));
            p.drawLine(startx, h /2, stopx, h / 2);
            // straight track (2)
            p.setPen(QPen(c2, 3, Qt::SolidLine, Qt::RoundCap,
                        Qt::MiterJoin));
            p.drawLine(w - startx - 1, h / 2, w - stopx - 1, h / 2);
            // diagonal track (3)
            p.setPen(QPen(c3, 3, Qt::SolidLine, Qt::RoundCap,
                        Qt::MiterJoin));
            p.drawLine(startx, h - starty - 1, stopx, h - stopy - 1);
            // diagonal track (4)
            p.setPen(QPen(c4, 3, Qt::SolidLine, Qt::RoundCap,
                        Qt::MiterJoin));
            p.drawLine(w - startx - 1, starty, w - stopx - 1, stopy);

            p.setPen(QPen(black, 1));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));
        
        // paint text label
        p.setPen(black);
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);
            br.moveBottomRight(QPoint(w - 3, h - 3));
            p.fillRect(br, QBrush(white));
            p.setBrush(white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // double slip switch right
    else if (sSoldIcon == SYM_DKR){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.setPen(QPen(black, 7));
        p.drawLine(0, h / 2, w - 1, h / 2);
        p.drawLine(0, 0, w - 1, h - 1);
        
        // paint drive symbol
        p.setPen(QPen(black, 1));
        p.drawLine(w / 2 - 1, h / 2 + 5, w / 2 - 7, h / 2 + 5);
        p.drawLine(w / 2 - 3, h / 2 + 5, w / 2 + 2, h / 2 + 7);
        p.drawLine(w / 2 + 7, h / 2 - 5, w / 2 + 2, h / 2 - 5);
        p.drawLine(w / 2 + 2, h / 2 - 5, w / 2 - 3, h / 2 - 8);

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 8, h - 11, 5, 5);
        p.drawEllipse(3 * w/ 4 - 2 - 2  , 6, 5, 5);

        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(lightGray));
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(lightGray));
            p.restore();
        }

        /**
         * lastdir  currentdir  newdir  c1  c2  c3  c4
         * -------------------------------------------
         *     0       0/0        0     -   -   -   -
         *     1       1/1        1     -   -   -   -
         *     2       2/2        2     -   -   -   -
         *     3       3/3        3     -   -   -   -
         *                   
         *     0       0/1        1     -   1   -   2 
         *     1       1/0        0     -   2   -   1
         *                   
         *     0       0/2        2     1   1   2   2
         *     2       2/0        0     2   2   1   1
         *                   
         *     0       0/3        3     1   -   2   -
         *     3       3/0        0     2   -   1   -
         *     
         *     1       1/2        2     1   -   2   -
         *     2       2/1        1     2   -   1   -
         *     
         *     1       1/3        3     1   2   2   1
         *     3       3/1        1     2   1   1   2
         *                   
         *     2       2/3        3     -   2   -   1
         *     3       3/2        2     -   1   -   2
         * -------------------------------------------
         **/
        else {
            QColor c1, c2, c3, c4;
            if (occupied) {
                if (iSoldDirection == 0) {
                    c1 = QColor(red);
                    c2 = QColor(red);
                    c3 = QColor(darkGray);
                    c4 = QColor(darkGray);
                }
                else if (iSoldDirection == 1) {
                    c1 = QColor(red);
                    c2 = QColor(darkGray);
                    c3 = QColor(darkGray);
                    c4 = QColor(red);
                }
                else if (iSoldDirection == 2) {
                    c1 = QColor(darkGray);
                    c2 = QColor(darkGray);
                    c3 = QColor(red);
                    c4 = QColor(red);
                }
                else if (iSoldDirection == 3) {
                    c1 = QColor(darkGray);
                    c2 = QColor(red);
                    c3 = QColor(red);
                    c4 = QColor(darkGray);
                }
                // error indication
                else {
                    c1 = QColor(red);
                    c2 = QColor(red);
                    c3 = QColor(red);
                    c4 = QColor(red);
                }
            }
            else {
                if (iSoldDirection == 0) {
                    if (!lightson && ((lastdir == 0 && newdir >= 2) ||
                            (lastdir >= 2 && newdir == 0)))
                        c1 = QColor(darkGray);
                    else
                        c1 = QColor(255, 225, 0);
                    
                    if (!lightson && (((lastdir == 1 || lastdir == 2) &&
                                    newdir == 0) ||
                            (lastdir == 0 && (newdir == 1 || newdir == 2))))
                        c2 = QColor(darkGray);
                    else
                        c2 = QColor(255, 225, 0);
                    
                    c3 = QColor(darkGray);
                    c4 = QColor(darkGray);
                }
                else if (iSoldDirection == 1) {
                    if (!lightson && ((lastdir == 1 &&
                                    (newdir == 2) || newdir == 3) ||
                                ((lastdir == 2 || newdir == 3) &&
                                 newdir == 1)))
                        c1 = QColor(darkGray);
                    else
                        c1 = QColor(255, 225, 0);
                    
                    c2 = QColor(darkGray);
                    c3 = QColor(darkGray);
                    
                    if (!lightson && ((lastdir == 1 &&
                                    (newdir == 0) || newdir == 3) ||
                                ((lastdir == 0 || lastdir == 3) &&
                                 newdir == 1)))
                        c4 = QColor(darkGray);
                    else
                        c4 = QColor(255, 225, 0);
                }
                else if (iSoldDirection == 2) {
                    c1 = QColor(darkGray);
                    c2 = QColor(darkGray);
                    
                    if (!lightson && ((lastdir == 2 &&
                                    (newdir == 0 || newdir == 1)) ||
                                ((lastdir == 0 || lastdir == 1) &&
                                 newdir == 2)))
                        c3 = QColor(darkGray);
                    else
                        c3 = QColor(255, 225, 0);
                    
                    if (!lightson && ((lastdir == 2 &&
                                    (newdir == 0 || newdir == 3)) ||
                                ((lastdir == 0 || lastdir == 3) &&
                                 newdir == 2)))
                        c4 = QColor(darkGray);
                    else
                        c4 = QColor(255, 225, 0);
                }
                else if (iSoldDirection == 3) {
                    c1 = QColor(darkGray);
                    
                    if (!lightson && ((lastdir == 3 &&
                                    (newdir == 1 || newdir == 2)) ||
                                ((lastdir == 1 || lastdir == 2) &&
                                 newdir == 3)))
                        c2 = QColor(darkGray);
                    else
                        c2 = QColor(255, 225, 0);
                    
                    if (!lightson && ((lastdir == 3 &&
                                    (newdir == 0 || newdir == 1)) ||
                                ((lastdir == 0 || lastdir == 1) &&
                                 newdir == 3)))
                        c3 = QColor(darkGray);
                    else
                        c3 = QColor(255, 225, 0);

                    c4 = QColor(darkGray);
                }
                // error indication
                else {
                    c1 = QColor(255, 225, 0);
                    c2 = QColor(255, 225, 0);
                    c3 = QColor(255, 225, 0);
                    c4 = QColor(255, 225, 0);
                }
            }

            int startx = w / 5 - 2;
            int stopx = 2 * w / 5 - 4;
            int starty = h / 5 - 2;
            int stopy = 2 * h / 5 - 3;

            // straight track (1)
            p.setPen(QPen(c1, 3, Qt::SolidLine, Qt::RoundCap,
                        Qt::MiterJoin));
            p.drawLine(w - startx - 1, h / 2, w - stopx - 1, h / 2);
            // straight track (2)
            p.setPen(QPen(c2, 3, Qt::SolidLine, Qt::RoundCap,
                        Qt::MiterJoin));
            p.drawLine(startx, h /2, stopx, h / 2);
            // diagonal track (3)
            p.setPen(QPen(c3, 3, Qt::SolidLine, Qt::RoundCap,
                        Qt::MiterJoin));
            p.drawLine(w - startx - 1, h - starty - 1, w - stopx - 1,
                    h - stopy - 1);
            // diagonal track (4)
            p.setPen(QPen(c4, 3, Qt::SolidLine, Qt::RoundCap,
                        Qt::MiterJoin));
            p.drawLine(startx, starty, stopx, stopy);

            p.setPen(QPen(black, 1));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));
        
        // paint text label
        p.setPen(black);
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);
            br.moveBottomLeft(QPoint(2, h - 3));
            p.fillRect(br, QBrush(white));
            p.setBrush(white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // track with normal route button
    else if (sSoldIcon == SYM_NRB || sSoldIcon == SYM_SRB){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.fillRect(0, h/2 - 3, w - 1, 7, QBrush(black));
        
        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            p.fillRect(w / 3, h / 2 - 1, w / 3, 3, QBrush(c));
        }

        // paint track button
        if (iSoldRotate == 1) {
            if (sSoldIcon == SYM_NRB)
                p.drawPixmap(w / 6  - 4, h / 2 - 3,
                        QPixmap(button_red_xpm));
            else
                p.drawPixmap(w / 6  - 4, h / 2 - 3,
                        QPixmap(button_gray_xpm));
        }
        else {
            if (sSoldIcon == SYM_NRB)
                p.drawPixmap(5 * w / 6 - 4 , h / 2 - 3, 
                        QPixmap(button_red_xpm));
            else
                p.drawPixmap(5 * w / 6 - 4 , h / 2 - 3, 
                        QPixmap(button_gray_xpm));
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            if (iSoldRotate == 1)
                br.moveTopLeft(QPoint(w/2 - br.width()/2, 2));
            else
                br.moveBottomRight(QPoint(w/2 + br.width()/2,
                            h - 3));

            p.fillRect(br, QBrush(white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }

    // shunting signals SS, SSH, SSS
    else if (sSoldIcon == SYM_SS || sSoldIcon == SYM_SSH ||
            sSoldIcon == SYM_SSS){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.fillRect(0, h/2 - 3, w - 1, 7, QBrush(black));
        
        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h/2 - 2, 5, 5, QBrush(lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
        }

        // paint track button
        int xpos1 = w / 6  - 4;
        int xpos2 = 5 * w / 6 - 4;

        if (sSoldIcon == SYM_SSS) {
            p.drawPixmap(xpos1, h / 2 - 3, QPixmap(button_gray_xpm));
            p.drawPixmap(xpos2, h / 2 - 3, QPixmap(button_gray_xpm));
        }

        if (iSoldRotate != 1)
        xpos1 = xpos2;

        if (sSoldIcon == SYM_SSH)
            p.drawPixmap(xpos1, h / 2 - 3, QPixmap(button_red_xpm));

        else if (sSoldIcon == SYM_SS) {
            p.drawPixmap(xpos1, h / 2 - 3, QPixmap(button_gray_xpm));
        }

        // paint signal icon
        p.setBrush(black);

        if (iSoldRotate != 1) {
            p.save();
            p.translate(w - 1, h - 1);
            p.rotate(180.0);
        }
        p.drawRect(5, 5, 15, 5);
        p.drawLine(6, 4, 18, 4);
        p.drawLine(6, 10, 18, 10);
        p.drawLine(5 + 15, 7, 6 + 16, 7);
        p.drawLine(7 + 16, 5, 7 + 16, 9);
        p.drawLine(7 + 17, 5, 7 + 17, 9);

        // paint signal light
        // SH0
        if (iSoldDirection == 0) {
            p.setPen(QPen(red));
            p.drawLine(7, 5, 7, 9);
            p.drawLine(8, 5, 8, 9);
        }
        // SH1
        else {
            p.setPen(yellow);
            p.drawLine(14, 5, 18, 9);
            p.drawLine(13, 5, 17, 9);
        }
        p.setPen(QPen(black));
        
        if (iSoldRotate != 1) 
            p.restore();

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        if (iSoldRotate == 1)
            p.drawEllipse(w / 2 - 1, 5, 5, 5);
        else
            p.drawEllipse(w / 2 - 4, h - 10, 5, 5);

        // paint FfM
        if (ffm) {
            p.setBrush(ffmactive ? yellow : darkGray);
            if (iSoldRotate == 1)
                p.drawRect(5, h - 10, 6, 6);
            else
                p.drawRect(w - 11, 4, 6, 6);
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            if (iSoldRotate == 1)
                br.moveBottomRight(QPoint(w/2 + br.width()/2, h - 3));
            else
                br.moveTopLeft(QPoint(w/2 - br.width()/2, 2));

            p.fillRect(br, QBrush(white));
            p.setBrush(white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);
            
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        p.end();
        setPaletteBackgroundPixmap(pm);
    }

    // turnout left or turnout right
    else if (sSoldIcon == SYM_WEL || sSoldIcon == SYM_WER){
        bool left = sSoldIcon == SYM_WEL;
        
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.fillRect(0, h/2 - 3, w - 1, 7, QBrush(black));
        
        p.save();
        p.translate(w / 2, h / 2);

        if (iSoldRotate == 1)
            if (left)
                p.rotate(WANGLE);
            else
                p.rotate(-WANGLE);
        else
            if (left)
                p.rotate(-SANGLE);
            else
                p.rotate(SANGLE);

        p.setPen(QPen(black, 7));
        p.drawLine(0, 0, w / 2 + 5, 0);
        p.restore();

        // paint track lights
        if (iSoldLEDoff == 1) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h/2 - 2, 5, 5, QBrush(lightGray));
            // short track
            p.save();
            p.translate(w / 2, h / 2);

            if (iSoldRotate == 1)
                if (left)
                    p.rotate(WANGLE);
                else
                    p.rotate(-WANGLE);
            else
                if (left)
                    p.rotate(-SANGLE);
                else
                    p.rotate(SANGLE);

            for (int i = 0; i < 3; ++i)
                p.fillRect(6 + 7 * i, -2, 5, 5, QBrush(lightGray));
            p.restore();

        }
        else {
            // first light
            QColor c;
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            if (iSoldRotate == 1)
                p.drawLine(w - 5, h / 2 , w - 19,  h / 2);
            else
                p.drawLine(5, h / 2 , 19,  h / 2);

            // second light
            if (iSoldDirection == 0 && lightson)
                if (occupied)
                    c = QColor(red);
                else
                    c = QColor(255, 225, 0);
            else
                c = QColor(darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            if (iSoldRotate == 1)
                p.drawLine(5, h / 2 , 19,  h / 2);
            else
                p.drawLine(w - 5, h / 2 , w - 19,  h / 2);

            // third light
            if (iSoldDirection == 1 && lightson)
                if (occupied)
                    c = QColor(red);
                else
                    c = QColor(255, 225, 0);
            else
                c = QColor(darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);

            if (iSoldRotate == 1)
                if (left)
                    p.rotate(WANGLE);
                else
                    p.rotate(-WANGLE);
            else
                if (left)
                    p.rotate(-SANGLE);
                else
                    p.rotate(SANGLE);

            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            p.setPen(QPen(black));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        if (iSoldRotate == 1 && left || iSoldRotate == 0 && !left)
            p.drawEllipse(w / 2 - 2, 6, 5, 5);
        else
            p.drawEllipse(w / 2 - 2, h - 11, 5, 5);

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            if (iSoldRotate == 1)
                if (left)
                    br.moveTopRight(QPoint(w - 3, 2));
                else
                    br.moveBottomRight(QPoint(w - 3, h - 3));
            else
                if (left)
                    br.moveBottomLeft(QPoint(2, h - 3));
                else
                    br.moveTopLeft(QPoint(2, 2));

            p.fillRect(br, QBrush(white));
            p.setBrush(white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }
        p.end();
        setPaletteBackgroundPixmap(pm);
    }

    // diagonal turnout left or right
    else if (sSoldIcon == SYM_DWL || sSoldIcon == SYM_DWR){
        bool left = sSoldIcon == SYM_DWL;
        
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        if (iSoldRotate == 1)
            p.fillRect(0, h / 2 - 3, w / 2, 7, QBrush(black));
        else
            p.fillRect(w / 2, h / 2 - 3, w - 1, 7, QBrush(black));
 
        p.save();
        p.translate(w / 2, h / 2);

        if (iSoldRotate == 1)
            if (left)
                p.rotate(-WANGLE);
            else
                p.rotate(WANGLE);
        else
            if (left)
                p.rotate(SANGLE);
            else
                p.rotate(-SANGLE);

        p.setPen(QPen(black, 7));
        p.drawLine(-w / 2 - 5, 0, w / 2 + 5, 0);
        p.restore();

        // paint track lights
        if (iSoldLEDoff == 1) {
            // short track
            if (iSoldRotate == 1)
                for (int i = 0; i < 3; ++i)
                    p.fillRect(4 + 7 * i, h/2-2, 5, 5, QBrush(lightGray));
            else
                for (int i = 0; i < 3; ++i)
                    p.fillRect(w / 2 + 5 + 7 * i, h / 2 - 2, 5, 5,
                            QBrush(lightGray));

            // long track
            p.save();
            p.translate(w / 2, h / 2);

            if (iSoldRotate == 1)
                if (left)
                    p.rotate(-WANGLE);
                else
                    p.rotate(WANGLE);
            else
                if (left)
                    p.rotate(SANGLE);
                else
                    p.rotate(-SANGLE);

            for (int i = 0; i < 3; ++i)
                p.fillRect(7 + 7 * i, - 2, 5, 5, QBrush(lightGray));

            for (int i = 0; i < 3; ++i)
                p.fillRect(-11 - 7 * i, - 2, 5, 5, QBrush(lightGray));

            p.restore();

        }
        else {
            QColor c;
            p.save();
            p.translate(w / 2, h / 2);

            if (left)
                p.rotate(SANGLE);
            else
                p.rotate(-SANGLE);

            // first light
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            if (iSoldRotate == 1)
                p.drawLine(10, 0, 24, 0);
            else
                p.drawLine(-10, 0, -24, 0);

            // second light
            if (iSoldDirection == 0 && lightson)
                if (occupied)
                    c = QColor(red);
                else
                    c = QColor(255, 225, 0);
            else
                c = QColor(darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            if (iSoldRotate == 1)
                p.drawLine(-10, 0, -24, 0);
            else
                p.drawLine(10, 0, 24, 0);

            p.restore();
                
            // third light
            if (iSoldDirection == 1 && lightson)
                if (occupied)
                    c = QColor(red);
                else
                    c = QColor(255, 225, 0);
            else
                c = QColor(darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            if (iSoldRotate == 1)
                p.drawLine(5, h / 2, 5 + 14, h / 2);
            else
                p.drawLine(w - 5, h / 2, w - 5 - 14, h / 2);

            p.setPen(QPen(black));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        if (iSoldRotate == 1)
            if (left)
                p.drawEllipse(w / 2 + 5, 8, 5, 5);
            else
                p.drawEllipse(w / 2 + 5, h - 14, 5, 5);
        else
            if (left)
                p.drawEllipse(w / 2 - 8, h - 14, 5, 5);
            else
                p.drawEllipse(w / 2 - 8, 8, 5, 5);

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            if (iSoldRotate == 1)
                if (left)
                    br.moveTopRight(QPoint(w - 3, 2));
                else
                    br.moveBottomRight(QPoint(w - 3, h - 3));
            else
                if (left)
                    br.moveBottomLeft(QPoint(2, h - 3));
                else
                    br.moveTopLeft(QPoint(2, 2));

            p.fillRect(br, QBrush(white));
            p.setBrush(white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }
        p.end();
        setPaletteBackgroundPixmap(pm);
    }

    // y-turnout
    else if (sSoldIcon == SYM_WEY){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.save();
        p.translate(w / 2, h / 2);
        p.setPen(QPen(black, 7));


        if (iSoldRotate == 1) {
            p.drawLine(0, 0, w / 2, 0);
            p.rotate(WANGLE);
            p.drawLine(0, 0, w / 2 + 5, 0);
            p.rotate(-WANGLE * 2);
            p.drawLine(0, 0, w / 2 + 5, 0);
        }
        else {
            p.drawLine(-w / 2, 0, 0, 0);
            p.rotate(-SANGLE);
            p.drawLine(0, 0, w / 2 + 5, 0);
            p.rotate(SANGLE * 2);
            p.drawLine(0, 0, w / 2 + 5, 0);
        }

        p.restore();

        // paint track lights
        if (iSoldLEDoff == 1) {
            // short track
            p.save();
            p.translate(w / 2, h / 2);

            if (iSoldRotate == 1) {
                for (int i = 0; i < 3; ++i)
                    p.fillRect(6 + 7 * i, -2, 5, 5, QBrush(lightGray));
                p.rotate(WANGLE);
                for (int i = 0; i < 3; ++i)
                    p.fillRect(7 + 7 * i, -2, 5, 5, QBrush(lightGray));
                p.rotate(-WANGLE * 2);
                for (int i = 0; i < 3; ++i)
                    p.fillRect(7 + 7 * i, -2, 5, 5, QBrush(lightGray));
            }
            else {
                for (int i = 0; i < 3; ++i)
                    p.fillRect(-11 - 7 * i, -2, 5, 5, QBrush(lightGray));
                p.rotate(-SANGLE);
                for (int i = 0; i < 3; ++i)
                    p.fillRect(7 + 7 * i, -2, 5, 5, QBrush(lightGray));
                p.rotate(SANGLE * 2);
                for (int i = 0; i < 3; ++i)
                    p.fillRect(7 + 7 * i, -2, 5, 5, QBrush(lightGray));
            }
            p.restore();

        }
        else {
            // first light
            QColor c;
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            if (iSoldRotate == 1)
                p.drawLine(w - 5, h / 2 , w - 19,  h / 2);
            else
                p.drawLine(5, h / 2 , 19,  h / 2);

            // second light
            if (iSoldDirection == 0 && lightson)
                if (occupied)
                    c = QColor(red);
                else
                    c = QColor(255, 225, 0);
            else
                c = QColor(darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);
            
            if (iSoldRotate == 1)
                p.rotate(-WANGLE);
            else
                p.rotate(SANGLE);

            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            // third light
            if (iSoldDirection == 1 && lightson)
                if (occupied)
                    c = QColor(red);
                else
                    c = QColor(255, 225, 0);
            else
                c = QColor(darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);

            if (iSoldRotate == 1)
                p.rotate(WANGLE);
            else
                p.rotate(-SANGLE);

            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            p.setPen(QPen(black));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        if (iSoldRotate == 1)
            p.drawEllipse(w / 2, 6, 5, 5);
        else
            p.drawEllipse(w / 2 - 4, h - 11, 5, 5);

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            if (iSoldRotate == 1)
                    br.moveBottomRight(QPoint(w - 3, h - 3));
            else
                    br.moveTopLeft(QPoint(2, 2));

            p.fillRect(br, QBrush(white));
            p.setBrush(white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }
        p.end();
        setPaletteBackgroundPixmap(pm);
    }

    // 3-way turnout
    else if (sSoldIcon == SYM_DRW){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        int w = pm.width();
        int h = pm.height();
        
        // paint track
        p.fillRect(0, h/2 - 3, w - 1, 7, QBrush(black));
        
        p.save();
        p.translate(w / 2, h / 2);
        p.setPen(QPen(black, 7));

        if (iSoldRotate == 1) {
            p.rotate(WANGLE);
            p.drawLine(0, 0, w / 2 + 5, 0);
            p.rotate(-WANGLE * 2);
            p.drawLine(0, 0, w / 2 + 5, 0);
        }
        else {
            p.rotate(-SANGLE);
            p.drawLine(0, 0, w / 2 + 5, 0);
            p.rotate(SANGLE * 2);
            p.drawLine(0, 0, w / 2 + 5, 0);
        }

        p.restore();

        // paint track lights (track indicator)
        if (iSoldLEDoff == 1) {
            p.save();
            p.translate(w / 2, h / 2);

            if (iSoldRotate == 1) {
                for (int i = 0; i < 3; ++i)
                    p.fillRect(-11 - 7 * i, -2, 5, 5, QBrush(lightGray));
                for (int i = 0; i < 3; ++i)
                    p.fillRect(6 + 7 * i, -2, 5, 5, QBrush(lightGray));
                p.rotate(WANGLE);
                for (int i = 0; i < 3; ++i)
                    p.fillRect(7 + 7 * i, -2, 5, 5, QBrush(lightGray));
                p.rotate(-WANGLE * 2);
                for (int i = 0; i < 3; ++i)
                    p.fillRect(7 + 7 * i, -2, 5, 5, QBrush(lightGray));
            }
            else {
                for (int i = 0; i < 3; ++i)
                    p.fillRect(-11 - 7 * i, -2, 5, 5, QBrush(lightGray));
                for (int i = 0; i < 3; ++i)
                    p.fillRect(6 + 7 * i, -2, 5, 5, QBrush(lightGray));
                p.rotate(-SANGLE);
                for (int i = 0; i < 3; ++i)
                    p.fillRect(7 + 7 * i, -2, 5, 5, QBrush(lightGray));
                p.rotate(SANGLE * 2);
                for (int i = 0; i < 3; ++i)
                    p.fillRect(7 + 7 * i, -2, 5, 5, QBrush(lightGray));
            }

            p.restore();

        }
        else {
            // first light
            QColor c;
            if (occupied)
                c = QColor(red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            if (iSoldRotate == 1)
                p.drawLine(w - 5, h / 2 , w - 19,  h / 2);
            else
                p.drawLine(5, h / 2 , 19,  h / 2);

            // second light
            if (iSoldDirection == 0 && lightson)
                if (occupied)
                    c = QColor(red);
                else
                    c = QColor(255, 225, 0);
            else
                c = QColor(darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            if (iSoldRotate == 1)
                p.drawLine(5, h / 2 , 19,  h / 2);
            else
                p.drawLine(w - 5, h / 2 , w - 19,  h / 2);

            // third light
            if (iSoldDirection == 1 && lightson)
                if (occupied)
                    c = QColor(red);
                else
                    c = QColor(255, 225, 0);
            else
                c = QColor(darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);

            if (iSoldRotate == 1)
                p.rotate(WANGLE);
            else
                p.rotate(-SANGLE);

            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            // fourth light
            if (iSoldDirection == 2 && lightson)
                if (occupied)
                    c = QColor(red);
                else
                    c = QColor(255, 225, 0);
            else
                c = QColor(darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);

            if (iSoldRotate == 1)
                p.rotate(-WANGLE);
            else
                p.rotate(SANGLE);

            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            p.setPen(QPen(black));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        if (iSoldRotate == 1)
            p.drawEllipse(w / 2, 6, 5, 5);
        else
            p.drawEllipse(w / 2 - 4, h - 11, 5, 5);

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f("Helvetica");
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(iSoldAddress_1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            if (iSoldRotate == 1)
                br.moveBottomRight(QPoint(w - 3, h - 3));
            else
                br.moveTopLeft(QPoint(2, 2));

            p.fillRect(br, QBrush(white));
            p.setBrush(white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }
        p.end();
        setPaletteBackgroundPixmap(pm);
    }

    // house 1 (train station middle section)
    else if (sSoldIcon == SYM_HS1) {
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);

        // paint house
        p.setBrush(QColor(192, 0 ,0));
        p.drawRect(0, 9, pm.width() - 1, 17);
        p.drawLine(0, pm.height() / 2, pm.width(), pm.height() / 2);

        p.end();
        setPaletteBackgroundPixmap(pm);
    }

    // house 2 (train station side section)
    else if (sSoldIcon == SYM_HS2) {
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);

        // paint house
        p.setBrush(QColor(192, 0 ,0));
        if (iSoldRotate == 1) {
            p.drawRect(4, 5, 25, 25);
            p.drawLine(4, 5, 28, 29);
            p.drawLine(4, 29, 28, 5);
            p.drawRect(0, 9, 5, 17);
        }
        else {
            p.drawRect(26, 5, 25, 25);
            p.drawLine(26, 5, 50, 29);
            p.drawLine(26, 29, 50, 5);
            p.drawRect(50, 9, 5, 17);
        }
        
        p.end();
        setPaletteBackgroundPixmap(pm);
    }
   
    // straight track with tunnel
    else if (sSoldIcon == SYM_GET || sSoldIcon == SYM_DLT ||
            sSoldIcon == SYM_DRT) {

        bool isleft = (sSoldIcon == SYM_DLT);
        bool isright = (sSoldIcon == SYM_DRT);
        
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);

        // translate origin to center of pixmap
        p.translate(pm.width()/2, pm.height()/2);

        int tracklen = pm.width() / 2 - 5;

        if (isleft) {
            tracklen += 5;
            if (iSoldRotate == 1) 
                p.rotate(WANGLE);
            else 
                p.rotate(-SANGLE);
        }
        else if (isright) {
            tracklen += 5;
            if (iSoldRotate == 1) 
                p.rotate(SANGLE);
            else 
                p.rotate(-WANGLE);
        }
        else {
            if (iSoldRotate == 1)
                p.rotate(180.0);
        }
        
        // paint track
        p.fillRect(-5, -3, -tracklen, 7, QBrush(black));
        
        // paint tunnel entry
        QPointArray tunnel = QPointArray(4);
        tunnel.putPoints(0, 4, -8, -14, 0, -6, 0, 6, -8, 14);
        p.setPen(QPen(darkGray, 2));
        p.drawPolyline(tunnel);

        p.end();
        setPaletteBackgroundPixmap(pm);
    }

    // track with loco shed (lokschuppen)
    else if (sSoldIcon == SYM_SHO || sSoldIcon == SYM_SHM ||
            sSoldIcon == SYM_SHU) {

        bool istop = (sSoldIcon == SYM_SHO);
        bool isbottom = (sSoldIcon == SYM_SHU);
        
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);

        // translate origin to center of pixmap
        p.translate(pm.width()/2, pm.height()/2);

        int tracklen = pm.width() / 2;
        int shedwidth = pm.height();
        int shedxoffset = 0;
        int shedyoffset = 0;

        if (istop) {
            tracklen += 5;
            shedwidth += 8;
            shedxoffset += 9;
            shedyoffset += 8;
            if (iSoldRotate == 1) 
                p.rotate(WANGLE);
            else 
                p.rotate(-SANGLE);
        }
        else if (isbottom) {
            tracklen += 5;
            shedwidth += 8;
            shedxoffset += 9;
            shedyoffset -= 8;
            if (iSoldRotate == 1) 
                p.rotate(-WANGLE);
            else 
                p.rotate(SANGLE);
        }
        else {
            if (iSoldRotate == 1)
                p.rotate(180.0);
        }
        
        // paint track
        p.fillRect(0, -3, -tracklen, 7, QBrush(black));
        
        // paint schuppen
        p.setBrush(QColor(192, 0 ,0));
        p.drawRect(0 - shedxoffset, -shedwidth/2 + shedyoffset,
                pm.width()/2, shedwidth);
        p.drawLine(pm.width() / 4 - shedxoffset, -shedwidth / 2 + shedyoffset,
                pm.width() / 4 - shedxoffset, shedwidth / 2 + shedyoffset);

        p.end();
        setPaletteBackgroundPixmap(pm);
    }

    // transfer table
    else if (sSoldIcon == SYM_SBN){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        // paint track
        p.fillRect(0, pm.height() / 2 - 3, 4, 7, QBrush(black));
        p.fillRect(pm.width() - 4, pm.height() / 2 - 3, 3, 7,
                QBrush(black));

        // paint table icon
        p.drawPixmap(4, 4, QPixmap(transfertable_xpm));
        
        //paint label
        QFont f("Helvetica");
        f.setPointSize(QApplication::font().pointSize() - 3);
        p.setFont(f);
        QFontMetrics fm(f);
        if (iSoldAddress_2 - iSoldAddress_1 == 0)
            sSoldText.setNum(iSoldDirection);
        QRect br = fm.boundingRect(sSoldText);
        br.setWidth(br.width() + 4);
        br.setHeight(br.height() + 2);
        br.moveTopLeft(QPoint(pm.width()/2 - br.width()/2,
                    pm.height()/2 - br.height()/2));
        p.fillRect(br, QBrush(white));
        p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                Qt::DontClip, sSoldText);

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    // turntable
    else if (sSoldIcon == SYM_DRE){
        QPixmap pm = QPixmap(leer_xpm);
        QPainter p;
        p.begin(&pm);
        
        // translate origin to center of pixmap
        p.translate(pm.width()/2, pm.height()/2);

        // paint track s
        int tracklen = pm.width() / 4;
        int startx = -pm.height() / 2;
        p.fillRect(startx, -3, -tracklen, 7, QBrush(black));
        p.rotate(SANGLE);
        p.fillRect(startx, -3, -tracklen - 1, 7, QBrush(black));
        p.rotate(-2 * SANGLE);
        p.fillRect(startx, -3, -tracklen - 1, 7, QBrush(black));
        p.rotate(180.0);
        p.fillRect(startx, -3, -tracklen - 1, 7, QBrush(black));
        p.rotate(SANGLE);
        p.fillRect(startx, -3, -tracklen, 7, QBrush(black));
        p.rotate(SANGLE);
        p.fillRect(startx, -3, -tracklen - 1, 7, QBrush(black));

        // paint icon
        p.setBrush(darkGray);
        p.setPen(QPen(QColor(128, 0, 0), 2));
        p.drawEllipse(-pm.height() / 2 + 1, -pm.height() / 2 + 1,
                pm.height() - 2, pm.height() - 2);

        // turning track, may be animated later
        p.setBrush(white);
        p.rotate(-180.0 -SANGLE/2);
        p.drawRect(-pm.height() / 2 + 2, -3, pm.height() - 4, 7);
        p.drawLine(-pm.height() / 2 + 2, 0, pm.height() / 2 - 2, 0);
        p.drawRect(6, -6, 6, 3);

        //paint label
        p.rotate(-SANGLE/2);
        p.setPen(QPen(black));
        QFont f("Helvetica");
        f.setPointSize(QApplication::font().pointSize() - 3);
        p.setFont(f);
        QFontMetrics fm(f);
        QString s;
        s.setNum(iSoldSubType);
        QRect br = fm.boundingRect(s);
        br.setWidth(br.width() + 4);
        br.setHeight(br.height() + 2);
        br.moveTopLeft(QPoint(-br.width()/2, 5));
        p.fillRect(br, QBrush(white));
        p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                Qt::DontClip, s);

        p.end();
        setPaletteBackgroundPixmap(pm);
    }
    
    addTooltip();
}


/*this draws only foreground lines on background pixmap*/
void element::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    QColor c;

    /* 1) paint visual mode lines*/
    switch (visualMode) {
        case kvmNormal:
            // normal mode: grey
            c = QColor(gray);
            break;
        case kvmEditLayout:
            // edit mode: red
            c = QColor(red);
            break;
        case kvmEditRoute:
            // show route mode: blue
            c = QColor(blue);
            break;
        default:
            // normal mode: grey
            c = QColor(gray);
            break;
    }
    
    p.setPen(c);
    int h = height();
    int w = width();
    p.drawLine(0, h - 1, w - 1, h - 1);
    p.drawLine(w - 1, h - 1, w - 1, 0);

    /* 2) paint optional selection rectangle*/
    if (selectionMode != ksmNormal) {
        switch (selectionMode) {
            case ksmStopSig:
                // red if in show route mode, stop signal
                c = QColor(red);
                break;
            case ksmStartSig:
                // green if in show route mode, start signal
                c = QColor(green);
                break;
            case ksmSwitchEl:
                // yellow if clicked element in record route mode
                c = QColor(251, 251, 0);
                break;
            case ksmFoundEl:
                // found: orange
                c = QColor("DarkOrange");
                break;
            default:
                c = QColor(black);
                break;
        }

        p.setPen(QPen(c, 2, SolidLine));
        p.drawLine(0, h - 2, w - 1, h - 2);
        p.drawLine(w - 2, h - 2, w - 2, 0);
        p.drawLine(w - 2, 1, 0, 0);
        p.drawLine(1, 1, 1, h - 2);
    }
}


/**
 * remove every tooltip and if configured add new one
 * with current element data
 */
void element::addTooltip()
{
    if (!pref.datatooltips)
        return;

    QToolTip::remove(this);
    QString a1 = QString::number(iSoldAddress_1);
    QString a2 = QString::number(iSoldAddress_2);

    QString tip1, tip2;

    tip1.sprintf("ELEMENT  # %03d\n"
            "icon     : %s\n"
            "rotate   : %s (=%1d)\n"
            "hide LEDs: %s (=%1d)\n"
            "invers   : %s (=%1d)\n"
            "decoder  : %s\n"
            "protocol : %s\n"
            "adress 1 : %s\n",
            iSoldIndex,
            sSoldIcon.data(),
            iSoldRotate == -1 ? "N/A" : (iSoldRotate ==
                0 ? "No" : "Yes"),
            iSoldRotate,
            iSoldLEDoff == -1 ? "N/A" : (iSoldLEDoff ==
                0 ? "No" : "Yes"),
            iSoldLEDoff,
            iSoldInvert == -1 ? "N/A" : (iSoldInvert ==
                0 ? "No" : "Yes"),
            iSoldInvert,
            sSoldDecoder == "-1" ?  "N/A (=-1)" 
            : sSoldDecoder.data(),
            protocol == SrcpMessage::proNone ? "N/A (=-1)"
                : (protocol == SrcpMessage::proMM ? "Motorola"
                        : (protocol == SrcpMessage::proDCC ? "NMRA/DCC"
                            : (protocol == SrcpMessage::proSelectrix ?
                                "Selectrix"
                                : "Server"))),
            iSoldAddress_1 == -1 ?  "N/A (=-1)" : a1.data());

    tip2.sprintf("adress 2 : %s\n"
            "c conn 1 : %s (=%1d)\n"
            "c conn 2 : %s (=%1d)\n"
            "direction: %d\n"
            "subtype  : %d\n"
            "text     : %s\n"
            "lock     : %s (=%1d)\n"
            "time (ms): %d\n"
            "FB contact: %d\n",
            iSoldAddress_2 == -1 ? "N/A (=-1)" : a2.data(),
            iSoldChangeConn[0] ==
            -1 ? "N/A" : (iSoldChangeConn[0] == 0 ? "No" : "Yes"),
            iSoldChangeConn[0],
            iSoldChangeConn[1] ==
            -1 ? "N/A" : (iSoldChangeConn[1] == 0 ? "No" : "Yes"),
            iSoldChangeConn[1], iSoldDirection, iSoldSubType,
            sSoldText == "-1" ? "N/A (=-1)" : sSoldText.data(),
            lockCounter == -1 ? "N/A" : (isLocked() ? "No" : "Yes"),
            lockCounter, iSoldActiveTime, iFBContact);

    tip1.append(tip2);

    QToolTip::setFont((QFont) "Courier");   //serd
    QToolTip::add(this, tip1);
}


/**
 * paint yellow track and return vertical correction value
 * to_r = routing direction to right layout side
 * C = correction for index
 */

// FIXME: routing should stop if track is not continued

/* TODO: two dimensional routing:
 *       entry -> exit (rdC, rdN, rdS, rdW, rdE, rdNW, rdNE, ...)
 *
 *                                 vertcorr
 *  rdNW | rdN  | rdNE      !to_r |  +1    | to_r
 * ------+------+------    -------+--------+-------
 *  rdW  | rdC  | rdE             |   0    |
 * ------+------+------    -------+--------+-------
 *  rdSW | rdS  | rdSE            |  -1    |
 *
 */
//int element::getRouteExit(int entrypos, bool setroute)

int element::routeElement(bool to_r, bool setroute, int vertcorr)
{
    int D = iSoldDirection;
    bool rotate = iSoldRotate == 1;
    int returnvalue = vertcorr;

    // immediate return if track is straightforward
    if (sSoldIcon == SYM_GER || sSoldIcon == SYM_ENK || sSoldIcon == SYM_HS
        || sSoldIcon == SYM_NRB || sSoldIcon == SYM_SRB
        || sSoldIcon == SYM_HSS || sSoldIcon == SYM_SS
        || sSoldIcon == SYM_SSH || sSoldIcon == SYM_SSS
        || sSoldIcon == SYM_BUE || sSoldIcon == SYM_RI1
        || sSoldIcon == SYM_RI2 || sSoldIcon == SYM_WS
        || sSoldIcon == SYM_ZP || sSoldIcon == SYM_BLD
        || sSoldIcon == SYM_ADR || sSoldIcon == SYM_VS) {
        returnvalue = 0;
    }

    else if (sSoldIcon == SYM_WEL || sSoldIcon == SYM_WER) {
        if (D == 0 || !(rotate ^ to_r))
            returnvalue = 0;
        if (D == 1
            && (rotate && !to_r && isright || !rotate && to_r && !isright))
            returnvalue = -1;
        if (D == 1
            && (rotate && !to_r && !isright || !rotate && to_r && isright))
            returnvalue = +1;
    }

    else if (sSoldIcon == SYM_DIL || sSoldIcon == SYM_DIR) {
        if (to_r ^ isright)
            returnvalue = -1;
        if (!(to_r ^ isright))
            returnvalue = +1;
    }

    else if (sSoldIcon == SYM_KUL || sSoldIcon == SYM_KUR) {
        if (!(rotate ^ to_r))
            returnvalue = 0;
        if (rotate && !to_r && isright || !rotate && to_r && !isright)
            returnvalue = -1;
        if (rotate && !to_r && !isright || !rotate && to_r && isright)
            returnvalue = +1;
    }

    else if (sSoldIcon == SYM_DRW) {
        if (!(rotate ^ to_r) || D == 0)
            returnvalue = 0;
        if (!rotate && to_r && D == 1 || rotate && !to_r && D == 2)
            returnvalue = -1;
        if (!rotate && to_r && D == 2 || rotate && !to_r && D == 1)
            returnvalue = +1;
    }

    else if (sSoldIcon == SYM_WEY) {
        if (!(rotate ^ to_r))
            returnvalue = 0;
        if (!rotate && to_r && D == 1 || rotate && !to_r && D == 0)
            returnvalue = -1;
        if (!rotate && to_r && D == 0 || rotate && !to_r && D == 1)
            returnvalue = +1;
    }

    else if (sSoldIcon == SYM_DWL || sSoldIcon == SYM_DWR) {
        if ((rotate ^ to_r) && D == 1)
            returnvalue = 0;
        if (!to_r && !isright && (!rotate || D == 0) || to_r && isright
            && (rotate || D == 0))
            returnvalue = -1;
        if (!to_r && isright && (!rotate || D == 0) || to_r && !isright
            && (rotate || D == 0))
            returnvalue = +1;
    }

    else if (sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR) {
        if (D == 0 || D == 1 &&
                (!(rotate ^ isright) && !to_r || (rotate ^ isright) && to_r))
            returnvalue = 0;
        if ((to_r ^ isright)
            && (!rotate && D == 1 || !rotate && D == 2 || rotate && D == 2))
            returnvalue = -1;
        if (!(to_r ^ isright)
            && (rotate && D == 1 || !rotate && D == 2 || rotate && D == 2))
            returnvalue = +1;

        // shortcut to avoid double painting
        setupElementIcon();
        return returnvalue;
    }

    // 4-state-DKWs
    else if ((sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR) &&
             iSoldSubType == 1) {
        if (D == 0 || !to_r && (D == 1 && !isright || D == 3 && isright) ||
            to_r && (D == 3 && !isright || D == 1 && isright))
            returnvalue = 0;
        if ((to_r ^ isright) && (D == 1 || D == 2))
            returnvalue = -1;
        if (!(to_r ^ isright) && (D == 2 || D == 3))
            returnvalue = +1;

        // shortcut to avoid double painting
        setupElementIcon();
        return returnvalue;
    }

    // 2-state-DKWs
    else if ((sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR) &&
             iSoldSubType == 0) {

        if (D == 1 && !isright
            && (vertcorr == +1 && !to_r || vertcorr == 0 && to_r)) {
            if (vertcorr != 0)
                returnvalue = 0;
            else
                returnvalue = -1;
        }

        else if (D == 1 && isright
            && (vertcorr == -1 && !to_r || vertcorr == 0 && to_r)) {
            if (vertcorr != 0)
                returnvalue = 0;
            else
                returnvalue = +1;
        }

        else if (D == 1 && !isright
            && (vertcorr == -1 && to_r || vertcorr == 0 && !to_r)) {
            if (vertcorr != 0)
                returnvalue = 0;
            else
                returnvalue = +1;
        }
        else if (D == 1 && isright
            && (vertcorr == +1 && to_r || vertcorr == 0 && !to_r)) {
            if (vertcorr != 0)
                returnvalue = 0;
            else
                returnvalue = -1;
        }

        // shortcut to avoid double painting
        setupElementIcon();
        return returnvalue;
    }

    // rail crossings
    else if (sSoldIcon == SYM_KRH) {
        if (setroute) {
            if (!to_r && vertcorr == +1 || to_r && vertcorr == -1)
                routedtrack = 1;
            else if (!to_r && vertcorr == -1 || to_r && vertcorr == +1)
                routedtrack = 2;
        }
        else
            routedtrack = 0;
    }

    else if (sSoldIcon == SYM_KRL) {
        if (setroute) {
            if (vertcorr == 0)
                routedtrack = 2;
            else
                routedtrack = 1;
        }
        else
            routedtrack = 0;
    }
    
    else if (sSoldIcon == SYM_KRR) {
        if (setroute) {
            if (vertcorr == 0)
                routedtrack = 2;
            else
                routedtrack = 1;
        }
        else
            routedtrack = 0;
    }

    // repaint element according to new routing state
    setRouted(setroute);
    return returnvalue;
}


/**
 * This slot is always called if a feedback port toggles.
 */
void element::slotOccupyElement(unsigned int bus, unsigned int contact,
        bool state)
{
    if (bus == iFBBusNo) {
        if (sSoldIcon == SYM_ADR) {
            unsigned int targetmod = (contact - 1) / 8 + 1;
            unsigned int selfmod = (iFBContact - 1) / 8 + 1;

            if (targetmod == selfmod)
                updateEDiTSAddress(contact, state);
        }
        else if (contact == iFBContact)
            setOccupied(state);
    }
}


/**
 * calculate the EDiTS address by bit field manipulation
 */
void element::updateEDiTSAddress(unsigned int contact, bool state)
{
    /*
     * address range for contact is 1..8, 9..16, 17..24, etc.
     * first mask three lower address bits (7), then evaluate bit value
     */
    unsigned int address = (contact - 1) & 7u;
    unsigned int bit = 1u << address;
   
    if (state)
        editsAddress = editsAddress | bit;
    else
        editsAddress = editsAddress & ~bit;

    /*
     * Most probably this is done eight times, due to eight feedback
     * address bits. If this eight feedback states are send in a fixed
     * sequence, it would be possible to check for the last arriving
     * bit and update the icon only once:
     * if (address == 7)
     */
    setupElementIcon();
}


void element::setOccupied(bool ostate)
{
    if (occupied != ostate) {
        occupied = ostate;
        updateLEDState();
    }
}


void element::setRouted(bool rstate)
{
    if (routed != rstate || routedtrack != 0) {
        routed = rstate;
        updateLEDState();
    }
}


void element::updateLEDState()
{
    /*
     * routed occupied iSoldLEDstate
     * -----------------------------
     *   0       0         LED_OFF
     *   1       0         LED_YEL
     *   0       1         LED_RED
     *   1       1         LED_RED
     * -----------------------------
     */
    int newLEDState;

    if (occupied)
        newLEDState = LED_RED;
    else {
        if (routed)
            newLEDState = LED_YEL;
        else
            newLEDState = LED_OFF;
    }

    if (iSoldLEDstate != newLEDState || routedtrack != 0) {
        iSoldLEDstate = newLEDState;
        setupElementIcon();
    }
}


void element::slotUpdateTurntableData(QPoint newCmd_)
{
    iSoldAddress_1 = iSoldAddress_2 + newCmd_.x() - 1;
    iSoldDirection = newCmd_.y();

    // save track# in subtype if a track key was pressed
    if (sSoldIcon == SYM_DRE && newCmd_.x() >= 4)
        iSoldSubType = newCmd_.x() * 2 - 9 + newCmd_.y();

    setupElementIcon();
    sendSrcpState();
}

/*
 * copy all available tracks at turntable into element's text
 * field, update tooltip
 */
void element::slotCopyAvailTracks(const QString& trackstr)
{
    sSoldText = trackstr;
    addTooltip();
}


void element::slotRepaintLayout()
{
    // show element now with opposite of text/address labels
    setupElementIcon();
}


QSize element::sizeHint() const
{
    return QSize(EL_WIDTH, EL_HEIGHT);
}


void element::writeFileTextToStream(QTextStream& ts)
{
    ts << GF_INDEX     << DS << iSoldIndex<< endl;
    ts << GF_NAME      << DS << sSoldIcon << endl;
    ts << GF_ROTATE    << DS << iSoldRotate << endl;
    ts << GF_INVERSTO  << DS << iSoldInvert << endl;
    ts << GF_DECODER   << DS << sSoldDecoder << endl;
    ts << GF_PROTOCOL  << DS << 
        ((protocol == SrcpMessage::proMM) ? "M" :
         (protocol == SrcpMessage::proDCC) ? "N" :
         (protocol == SrcpMessage::proServer) ? "P" :
         (protocol == SrcpMessage::proSelectrix) ? "S" : "-1"
         ) << endl;
    ts << GF_ADDRESS1  << DS << iGA1BusNo << DS << iSoldAddress_1 <<
        DS << port1 << endl;
    ts << GF_ADDRESS2  << DS << iGA2BusNo << DS << iSoldAddress_2 <<
        DS << port2 << endl;
    ts << GF_XCHCONN1  << DS << iSoldChangeConn[0] << endl;
    ts << GF_XCHCONN2  << DS << iSoldChangeConn[1] << endl;
    ts << GF_DIRECTION << DS << iSoldDirection << endl;
    ts << GF_SUBTYPE   << DS << iSoldSubType << endl;
    ts << GF_TEXT      << DS << sSoldText << endl;
    ts << GF_ACTTIME   << DS << iSoldActiveTime << endl;
    ts << GF_FBPORT    << DS << iFBBusNo << DS << iFBContact << endl;
    ts << GF_HIDELEDS  << DS << iSoldLEDoff << endl;
}


QString element::getName() const
{
    return sSoldText;
}


bool element::hasSameAddress(int bus, int address)
{
    return (bus == iGA1BusNo && address == iSoldAddress_1);
}


bool element::isLocked()
{
    return (lockCounter > 0);
}


bool element::isOccupied()
{
    return occupied;
}

/*increase or decrease lock state and repaint element if necessary*/
void element::setLocked(bool lock)
{
    if (lock) {
        ++lockCounter;
        if (lockCounter == 1)
            setupElementIcon();
    }
    else if (lockCounter > 0) {
        --lockCounter;
        if (lockCounter == 0)
            setupElementIcon();
    }
}


/**
 * switch "Fahrstrassenfestlegemelder" on or off;
 */
void element::activateFfM(bool active)
{
    if (ffm)
        if (ffmactive != active) {
            ffmactive = active;
            setupElementIcon();
        }
}


/**
 * test if "Fahrstrassenfestlegemelder" is switched on
 */
bool element::hasFfMLock()
{
    return ffmactive;
}


void element::setIndexNo(unsigned int idx)
{
    iSoldIndex = idx;
}


unsigned int element::getIndexNo()
{
    return iSoldIndex;
}


void element::getStateData(stateElement& se)
{
    se.name = sSoldText;
    se.bus = iGA1BusNo;
    se.address = iSoldAddress_1;
    se.state = iSoldDirection;
    se.elemPtr = this;
    se.elemPtr2 = NULL;
}


elemSelectionMode element::getSelectionMode()
{
    return selectionMode;
}


bool element::hasDifferentDirection(int dir)
{
    return iSoldDirection != dir;
}


bool element::hasShuntingRouteButtonOnly()
{
    return (sSoldIcon == SYM_SS || sSoldIcon == SYM_SSS || sSoldIcon ==
            SYM_SRB || sSoldIcon == SYM_WS);
}


bool element::hasLEDsOn()
{
    return (iSoldLEDoff != 1);
}


int element::getAddressCount()
{
    int returnvalue = 0;
    if (iSoldAddress_1 != -1) {
        ++returnvalue;
        if (iSoldAddress_2 != -1) 
            ++returnvalue;
    }
    return returnvalue;
}


void element::updateFeedbackState()
{
    // get current feedback status from server to update LEDstate
    if ((iSoldLEDoff != 1) && (iFBContact >= 0)) {
        
        SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgFbGet);
        if (sm == NULL)
            return;

        sm->setFbData(iFBBusNo,
                (SrcpMessage::Feedback) pref.fbmoduletype, iFBContact);
        
        emit sendSrcpMessage(sm);

        delete sm;
    }
}


/*
 * update element if system font is changed e.g. by qtconfig
 * do nothing for empty element with no text
 */
void element::fontChange(const QFont& oldFont)
{
    if (sSoldIcon == SYM_LEE && (sSoldText.isEmpty() || sSoldText == "-1"))
        return;
    else
        setupElementIcon();
}


bool element::sendSRCP08InitGA(unsigned int gano)
{
    bool returnvalue = false;

    if (isSwitchable()){

        SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgGaInit);
        if (sm == NULL)
            return returnvalue;
        
        if (gano == 1)
            sm->setGaData(protocol, iGA1BusNo, iSoldAddress_1, 0, 0, 0);
        else
            sm->setGaData(protocol, iGA2BusNo, iSoldAddress_2, 0, 0, 0);

        emit sendSrcpMessage(sm);
        delete sm;

        returnvalue = true;
    }
    return returnvalue;
}


int element::getAddress1()
{               
    return iSoldAddress_1;
}   
            
            
int element::getAddress2()
{
    return iSoldAddress_2;
}   

                        
int element::getGA1BusNo()
{               
    return iGA1BusNo;
}   
            
            
int element::getGA2BusNo()
{
    return iGA2BusNo;
}   

                        
int element::getFBBusNo()
{
    return iFBBusNo;
}   
                

bool element::hasThreeStates()
{
    return (sSoldIcon == SYM_HS || sSoldIcon == SYM_VS) &&
        iSoldSubType == 6;
}


bool element::isSwitched()
{
    return switched;
}


void element::setSwitched(bool sw)
{
    switched = sw;
}


/*
 * Blinking in original is one time in old direction and five times in new
 * direction resulting in 14 switch cycles.
 *
 * dir old ->| |<- new
 * on  --+ +-+ +-+ +-+ +-+ +-+ +-+ +--
 *      0| |1| |2| |3| |4| |5| |6| |7
 * off   +-+ +-+ +-+ +-+ +-+ +-+ +-+
 *       ^     ^
 *       |     |
 *       |     change visible direction
 *       send SRCP command
*/
void element::switchToDirBlinking(int ndir)
{
    // save last state for single-slip and double-slip switches
    lastdir = iSoldDirection;

    if (blinkcounter != 0) {
        blinkcounter = 1;
        newdir = ndir;
    }
    else {
        blinkcounter = 1;
        newdir = ndir;
        runTurnoutBlinkTimer();
    }
}


void element::runTurnoutBlinkTimer()
{
    if (blinkcounter == 1) {
         int olddir = iSoldDirection;
         iSoldDirection = newdir;
         sendSrcpState();
         iSoldDirection = olddir;
    }
    if (blinkcounter % 2 == 1)
        setLightsOn(false);
    else {
        if (blinkcounter == 4)
            iSoldDirection = newdir;
        setLightsOn(true);
    }
    ++blinkcounter;   
    if (blinkcounter < 13)
        QTimer::singleShot(500, this, SLOT(runTurnoutBlinkTimer()));
    else {
        blinkcounter = 0;
        emit turnoutIsSwitched();
    }
} 


void element::setLightsOn(bool ison)
{
    if (ison != lightson) {
        lightson = ison;
        setupElementIcon();
        repaint();
    }
}
