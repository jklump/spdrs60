/***************************************************************************
                           element.cpp
                           -------------------------------
    Copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2010 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2009-11-01 20:40:38 $
                           $Revision: 1.198 $
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

#include <qapplication.h>
#include <qaction.h>
#include <qpixmap.h>
#include <qpopupmenu.h>

#include "drivedialog.h"
#include "dualdrivedialog.h"
#include "element.h"
#include "elementlabeldialog.h"
#include "feedbacktriggerdialog.h"
#include "preferences.h"
#include "resources.h"
#include "variantdialog.h"
#include "virtualaddressdialog.h"

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
#include "pixmaps/label-wht.xpm"
#include "pixmaps/label-ein-on.xpm"
#include "pixmaps/label-ein-off.xpm"
#include "pixmaps/label-aus-on.xpm"
#include "pixmaps/label-aus-off.xpm"
#include "pixmaps/transfertable.xpm"
#include "pixmaps/signal-w.xpm"
#include "pixmaps/signal-wr.xpm"

/*pixmaps for variant dialog*/
#include "pixmaps/signal_hss_st1.xpm"
#include "pixmaps/signal_hss_st2.xpm"
#include "pixmaps/signal_hss_st3.xpm"
#include "pixmaps/signal_hs_st1.xpm"
#include "pixmaps/signal_hs_st2.xpm"
#include "pixmaps/signal_hs_st3.xpm"
#include "pixmaps/signal_vs_st1.xpm"
#include "pixmaps/signal_vs_st2.xpm"
#include "pixmaps/signal_vs_st3.xpm"
#include "pixmaps/entkoppler_st1.xpm"
#include "pixmaps/entkoppler_st2.xpm"
#include "pixmaps/entkoppler_st3.xpm"
#include "pixmaps/dkw_links_st2.xpm"
#include "pixmaps/dkw_links_st3.xpm"
#include "pixmaps/dkw_rechts_st2.xpm"
#include "pixmaps/dkw_rechts_st3.xpm"


// Element directions
enum {
   DIR_HP0 = 0,
   DIR_HP1,
   DIR_HP2,
   DIR_SH1
};

// Maerklin turntable addresses
enum {
   MMTT14 = 209,
   MMTT15 = 225
};

// signal light size parameter
// delay for edit mode after element locating
enum {
    LIGHTSIZE = 7,
    LOCATE_TIMER = 5000
};

static const float SANGLE = 31.264f;           // small angle
static const float WANGLE = (180.0f - SANGLE); // wide angle

#if QT_VERSION >= 0x040000
using namespace Qt;
#endif


element::element(QWidget* parent, SpdrItemClassId ci)
    : SpdrPanel(parent)
{
    initVariables();
    classid = ci;
    updateProperties();
    setupElementIcon();
}
    

element::element(QTextStream& ts, QWidget* parent)
    : SpdrPanel(parent)
{
    initVariables();
    visualMode = kvmNormal;
    classid = siciNone;
    readFileTextFromStream(ts);
    updateProperties();
    setupElementIcon();
}


element::element(QTextStream& ts, QWidget* parent, SpdrItemClassId ci)
    : SpdrPanel(parent)
{
    initVariables();
    visualMode = kvmNormal;
    classid = ci;
    readFileTextFromStream(ts);
    updateProperties();
    setupElementIcon();
}


/*set all variables which are not read from file*/
void element::initVariables()
{
#if QT_VERSION >= 0x040000
    setAutoFillBackground(true);
#endif
    iSoldInvert = -1;
    protocol1 = SrcpMessage::proNone;
    protocol2 = SrcpMessage::proNone;
    address1 = -1;
    address2 = -1;
    xchangeport1 = -1;
    xchangeport2 = -1;
    state = -1;
    iSoldSubType = -1;
    sSoldText = "";
    activetime1 = -1;
    activetime2 = -1;
    iFBContact = 1;
    trackindicatoroff = true;
    enable1fbtrigger = false;
    button1fbbus = 1;
    button1fbcontact = 1;
    enable2fbtrigger = false;
    button2fbbus = 1;
    button2fbcontact = 1;

    editsAddress = 0;
    countervalue = 0;
    ffmactive = false;
    ffm = false;
    occupied = false;
    routable = false;
    routed = false;
    routemark = false;
    signal = false;
    state2dkw = false;
    switchable = false;
    switched = false;
    simplega = false;
    turnout = false;
    lightson = true;
    lastdir = 0;
    newdir = 0;
    bus1 = bus2 = iFBBusNo = 1;
    port1 = 1;
    port2 = 1;

    // this is only used for crossings to choose the routed track
    routedtrack = 0;

    background = QPixmap(size());
    background.fill(Qt::lightGray);

    lockCounter = 0;
    blinkcounter = 0;

    turntableProperties = NULL;
    ttComm = NULL;
    tablelight = true;
}

/* Read the layout element data from stream, old data style containing
 * an icon name is translated to new style using class ids.
 * Lines starting with # are recognized as comments, lines starting with
 * % are recognized as end of dataset marker (new style file format)*/
void element::readFileTextFromStream(QTextStream& ats)
{
    QString s, key, value, oldname;

    while (!ats.eof()) {
        s = ats.readLine();
        if (!s.startsWith("#")) {
            key = s.section(DS, 0, 0);
            value = s.section(DS, 1, 1).stripWhiteSpace();

            /* key/value pairs are read sequence independent */
            if (key.compare(GF_CLASSID) == 0) {
                  classid = (SpdrItemClassId)value.toInt();
            }
            else if (key.compare(GF_INDEX) == 0) {
                  iSoldIndex = value.stripWhiteSpace().toUInt();
            }
            else if (key.startsWith("%")) {
                /*end of dataset, exit while loop*/
                  break;
            }
            else if (key.compare(GF_INVERSTO) == 0) {
                iSoldInvert = value.toInt();
            }
            else if (key.compare(GF_PROTOCOL1) == 0) {
                if (value == "M")
                    protocol1 = SrcpMessage::proMM;
                else if (value == "N")
                    protocol1 = SrcpMessage::proDCC;
                else if (value == "P")
                    protocol1 = SrcpMessage::proServer;
                else if (value == "S")
                    protocol1 = SrcpMessage::proSelectrix;
                else
                    protocol1 = SrcpMessage::proNone;
            }
            else if (key.compare(GF_PROTOCOL2) == 0) {
                if (value == "M")
                    protocol2 = SrcpMessage::proMM;
                else if (value == "N")
                    protocol2 = SrcpMessage::proDCC;
                else if (value == "P")
                    protocol2 = SrcpMessage::proServer;
                else if (value == "S")
                    protocol2 = SrcpMessage::proSelectrix;
                else
                    protocol2 = SrcpMessage::proNone;
            }
            /*old style compatibility*/
            else if (key.compare(GF_PROTOCOL) == 0) {
                if (value == "M")
                    protocol1 = SrcpMessage::proMM;
                else if (value == "N")
                    protocol1 = SrcpMessage::proDCC;
                else if (value == "P")
                    protocol1 = SrcpMessage::proServer;
                else if (value == "S")
                    protocol1 = SrcpMessage::proSelectrix;
                else
                    protocol1 = SrcpMessage::proNone;
                protocol2 = protocol1;
            }
            else if (key.compare(GF_ADDRESS1) == 0) {
                bus1 = value.toInt();
                value = s.section(DS, 2, 2).stripWhiteSpace();
                address1 = value.toInt();
                value = s.section(DS, 3, 3).stripWhiteSpace();
                port1 = value.toUInt();
            }
            else if (key.compare(GF_ADDRESS2) == 0) {
                bus2 = value.toInt();
                value = s.section(DS, 2, 2).stripWhiteSpace();
                address2 = value.toInt();
                value = s.section(DS, 3, 3).stripWhiteSpace();
                port2 = value.toUInt();
            }
            else if (key.compare(GF_XCHCONN1) == 0) {
                xchangeport1 = value.toInt();
            }
            else if (key.compare(GF_XCHCONN2) == 0) {
                xchangeport2 = value.toInt();
            }
            else if (key.compare(GF_DIRECTION) == 0) {
                state = value.toInt();
            }
            else if (key.compare(GF_SUBTYPE) == 0) {
                iSoldSubType = value.toInt();
            }
            else if (key.compare(GF_TEXT) == 0) {
                sSoldText = s.section(DS, 1).stripWhiteSpace();
                // filter old flag for "no label text"
                if (sSoldText == "-1")
                    sSoldText = "";
            }
            else if (key.compare(GF_ACTTIME1) == 0) {
                activetime1 = value.toInt();
            }
            else if (key.compare(GF_ACTTIME2) == 0) {
                activetime2 = value.toInt();
            }
            /*old style compatibility*/
            else if (key.compare(GF_ACTTIME) == 0) {
                activetime1 = value.toInt();
                activetime2 = activetime1;
            }
            else if (key.compare(GF_FBPORT) == 0) {
                iFBBusNo = value.toUInt();
                value = s.section(DS, 2, 2).stripWhiteSpace();
                iFBContact = value.toInt();
                if (iFBContact <= 0)
                    iFBContact = 1;
            }
            else if (key.compare(GF_BUTTON1FB) == 0) {
                enable1fbtrigger = value.toUInt();
                value = s.section(DS, 2, 2).stripWhiteSpace();
                button1fbbus = value.toUInt();
                value = s.section(DS, 3, 3).stripWhiteSpace();
                button1fbcontact = value.toUInt();
            }
            else if (key.compare(GF_BUTTON2FB) == 0) {
                enable2fbtrigger = value.toUInt();
                value = s.section(DS, 2, 2).stripWhiteSpace();
                button2fbbus = value.toUInt();
                value = s.section(DS, 3, 3).stripWhiteSpace();
                button2fbcontact = value.toUInt();
            }
            else if (key.compare(GF_HIDELEDS) == 0) {
                trackindicatoroff = (value.toInt() == 1);
                /*this is the last parameter for old style format;
                 * now exit while loop*/
                break;
            }
        }
    }
}

/**
 * update element type dependent property values
 *
 * initialize standard properties of this special symbol to avoid
 * recalculation in several procedures by expensive string
 * comparisons
 * */
void element::updateProperties()
{
    // couplers and crossings get the non-active direction on setup
    if (classid == siciEnk || classid == siciKrh ||
        classid == siciKr1 || classid == siciKl1)
        state = 0; 

    // route marks are entry or exit points of routes (limits)
    routemark = classid == siciHs1 || classid == siciHs3
        || classid == siciHss1 || classid == siciHss3
        || classid == siciSs1 || classid == siciSs3
        || classid == siciSd1 || classid == siciSd3
        || classid == siciSh1 || classid == siciSh3
        || classid == siciWs1 || classid == siciWs3
        || classid == siciZt1 || classid == siciZt3
        || classid == siciRt1 || classid == siciRt3;

    // signals include also not route mark type signals
    signal = routemark
        || classid == siciVs1 || classid == siciVs3
        || classid == siciZp1 || classid == siciZp3;


    // init signals as they were saved in layout file or with red state
    if (pref.initsignalsred) {
        if (signal || classid == siciRel || classid == siciBld)
            state = 0;
    }

    if (signal)
        ffm = (classid == siciHs1 || classid == siciHs3
                || classid == siciHss1 || classid == siciHss3
                || classid == siciSh1 || classid == siciSh3);

    else if (classid == siciTl1 || classid == siciTl3 ||
            classid == siciTr1 || classid == siciTr3 ||
            classid == siciIl1 || classid == siciIr3 ||
            classid == siciIl3 || classid == siciIr1 ||
            classid == siciSy1 || classid == siciSy3 ||
            classid == siciTw3 || classid == siciTw1 ||
            classid == siciSl1 || classid == siciSl3 ||
            classid == siciSr1 || classid == siciSr3 ||
            classid == siciDl1 || classid == siciDr1)
        turnout = true;

    else if (classid == siciBld || classid == siciEnk ||
            classid == siciRel || classid == siciMdc)
        simplega = true;

    /*element can be a part of a route*/
    routable = signal || turnout ||
        classid == siciSt1 || classid == siciSt2 ||
        classid == siciSt3 || classid == siciSt4 ||
        classid == siciCr1 || classid == siciCr2 ||
        classid == siciCr3 || classid == siciCr4 ||
        classid == siciCl1 || classid == siciCl2 ||
        classid == siciCl3 || classid == siciCl4 ||
        classid == siciTdr || classid == siciTdl ||
        classid == siciTul || classid == siciTur ||
        classid == siciTuh || classid == siciTuv ||
        classid == siciTdb || classid == siciKrh ||
        classid == siciKr1 || classid == siciKl1 ||
        classid == siciBue || classid == siciAdr ||
        classid == siciBld || classid == siciEnk;
    
    /*element has decoder connected*/
    switchable = (signal || turnout || simplega) &&
        classid != siciZt3 && classid != siciZt1 &&
        classid != siciRt3 && classid != siciRt1;

    state2dkw = (classid == siciDl1 || classid == siciDr1)
        && iSoldSubType == 0;
}


bool element::is2StateDKW()
{
    return state2dkw;
}


bool element::isRouteMark()
{
    return routemark;
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
        if (newdir != state || classid == siciEnk) {
            state = newdir;
            setupElementIcon();
            sendSrcpState();
        }
        else {
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
        if (e->button() == Qt::LeftButton) {
            GbsButtonState ctrlButton = kNoneClicked;
            QPoint CursorPos = mapFromGlobal(QCursor::pos());

            /*TODO: move this to gbsarea*/
            if (classid == siciDre) {
                ttComm = new turntableCommander(sSoldText, this, iSoldSubType);
                connect(ttComm, SIGNAL(sendTtCommand(int, int)),
                        this, SLOT(slotUpdateTurntableData(int, int)));
                connect(ttComm, SIGNAL(trackPositionsChanged(const QString&)),
                        this, SLOT(slotCopyAvailTracks(const QString&)));

                ttComm->exec();
            }

            /*TODO: move this to gbsarea*/
            else if (classid == siciSbn || classid == siciMdc) {
                turntableProperties = new elementCommander(this, classid);
                connect(turntableProperties, SIGNAL(sendTtCommand(int, int)),
                        this, SLOT(slotUpdateTurntableData(int, int)));

                turntableProperties->exec();
            }

            /* determine what type of button was pressed an send the
             * corresponding value to GBSArea to change cursor shape etc.*/
            // if element contains a solenoid or is a external button
            // TODO: if (hasButton())
            else if ((address1 != -1) && (classid != siciAdr)) {

                if (classid == siciHs1 || classid == siciHs3
                        || classid == siciZt1 || classid == siciZt3)
                    ctrlButton = kZfsClicked;

                else if (classid == siciSs1 || classid == siciSs3 ||
                        classid == siciSd1 || classid == siciSd3 ||
                        classid == siciWs1 || classid == siciWs3 ||
                        classid == siciRt1 || classid == siciRt3)
                    ctrlButton = kRfsClicked;

                else if (classid == siciHss1) {
                    /* two different buttons on this panel */
                    if (CursorPos.x() > (width() >> 1))
                        ctrlButton = kZfsClicked;
                    else
                        ctrlButton = kRfsClicked;
                }

                else if (classid == siciHss3) {
                    /* two different buttons on this panel */
                    if (CursorPos.x() > (width() >> 1))
                        ctrlButton = kRfsClicked;
                    else
                        ctrlButton = kZfsClicked; 
                }

                else if (classid == siciSh1 || classid == siciSh3)
                    ctrlButton = kZhsClicked;

                else
                    ctrlButton = kTurnoutClicked;

                emit elementClicked(this, ctrlButton);
            }

            /*add here new external button functions*/
            /*WGT button*/
            else if (classid == siciTaw) {
                ctrlButton = kWgtClicked;
                emit elementClicked(this, ctrlButton);
            }

            /*WHT button*/
            else if (classid == siciTwh) {
                ctrlButton = kWhtClicked;

                /*
                 * increment counter and repaint symbol, if value
                 * has more than four digits, reset to zero
                 */
                ++countervalue;
                if (countervalue == 10000)
                    countervalue = 0;
                setupElementIcon();
                emit elementClicked(this, ctrlButton);
            }

            /*FHT button*/
            else if (classid == siciTaf) {
                ctrlButton = kFhtClicked;

                /*
                 * increment counter and repaint symbol, if value
                 * has more than four digits, reset to zero
                 */
                ++countervalue;
                if (countervalue == 10000)
                    countervalue = 0;
                setupElementIcon();
                emit elementClicked(this, ctrlButton);
            }

            /*UfGT/MGT button*/
            else if (classid == siciTau) {
                if (CursorPos.x() < (width() >> 1))
                    ctrlButton = kUfgtClicked; 
                else
                    ctrlButton = kMgtClicked;
                emit elementClicked(this, ctrlButton);
            }

            /*SGT/HaGT button*/
            else if (classid == siciTas) {
                if (CursorPos.x() < (width() >> 1))
                    ctrlButton = kSgtClicked; 
                else
                    ctrlButton = kHagtClicked;
                emit elementClicked(this, ctrlButton);
            }

            /*Ein/Aus button*/
            else if (classid == siciTal) {
                if (CursorPos.x() < (width() >> 1))
                    ctrlButton = kEinClicked; 
                else
                    ctrlButton = kAusClicked;
                emit elementClicked(this, ctrlButton);
            }

            e->accept();
        }
    }
    /*layout edit mode*/
    else if (visualMode == kvmEditLayout) {
        if (e->button() == Qt::LeftButton) {
            /*handled by gbsarea*/
            e->ignore();
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
        if (e->button() == Qt::RightButton) {
            if (isSwitchable()) {
                QAction* toggleAction = new QAction(tr("&Toggle"), 0,
                        this, "toggleaction");
                toggleAction->setEnabled(ctxCanSwitch());
                QPopupMenu menu;
                toggleAction->addTo(&menu);
                if (menu.exec(QCursor::pos()) != -1)
                    toggle();
            }
            e->accept();
        }
    }

    /*layout edit mode*/
    else if (visualMode == kvmEditLayout) {
        if (e->button() == Qt::RightButton) {
            runPropertyMenue(e->pos());
            e->accept();
        }
        else
            e->ignore();
    }

    /*route edit mode*/
    else if (visualMode == kvmEditRoute) {
        if (e->button() == Qt::LeftButton) {
            /*select/deselect start or stop signal*/
            if (routemark) {
                /*send record signal to router*/
                if (ksmNormal == selectionMode)
                    emit recordElement(this, krecStartStop);
                else
                    emit recordElement(this, krecClear);
                e->accept();
            }
        }
        else if (e->button() == Qt::MidButton) {

            /*select/deselect switchable element*/
            /*and send record signal to router*/
            if (turnout || classid == siciMdc ||
                classid == siciRel || classid == siciBld ||
                classid == siciZp1 || classid == siciZp3 || 
                classid == siciHs1 || classid == siciHs3 ||
                classid == siciHss1 || classid == siciHss3 ||
                classid == siciSs1 || classid == siciSs3 ||
                classid == siciSd1 || classid == siciSd3 ||
                classid == siciSh1 || classid == siciSh3 ||
                classid == siciVs1 || classid == siciVs3 ||
                classid == siciWs1 || classid == siciWs3 ||
                classid == siciKr1 || classid == siciKl1 ||
                classid == siciKrh) {
                if (ksmNormal == selectionMode)
                    emit recordElement(this, krecNormal);
                else
                    emit recordElement(this, krecClear);
                e->accept();
            }

            /*select/deselect train number display*/
            /*and send record signal to router*/
            else if (classid == siciAdr) {
                if (ksmNormal == selectionMode)
                    emit recordElement(this, krecDisplay);
                else
                    emit recordElement(this, krecClear);
                e->accept();
            }
        }
        /*FIXME: same as in normal mode*/
        else if (e->button() == Qt::RightButton) {
            if (isSwitchable()) {
                QAction* toggleAction = new QAction(tr("&Toggle"), 0,
                        this, "toggleaction");
                toggleAction->setEnabled(ctxCanSwitch());
                QPopupMenu menu;
                toggleAction->addTo(&menu);
                if (menu.exec(QCursor::pos()) != -1)
                    toggle();
            }
            e->accept();
        }
    }

    /*track clear detection / track occupancy detection*/
    else if (visualMode == kvmEditClearance) {
        if (e->button() == Qt::LeftButton) {

            /*select/deselect track indicator elements*/
            /*and send record signal to element recorder*/
            if (routable) {
                if (ksmNormal == selectionMode)
                    emit recordElement(this, krecTrackIndicator);
                else
                    emit recordElement(this, krecClear);
            }
            e->accept();
        }
    }
    else
        e->ignore();
}


void element::slotShowElement(int address, int ns, elemSelectionMode sm)
{
    if (address == address1) {
        selectionMode = sm;
        if (ns != -1)
            state = ns;
        setupElementIcon();
    }
}


void element::showElementState(int ns, elemSelectionMode sm)
{
    selectionMode = sm;
    if (ns != -1)
        state = ns;
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


void element::switchAddress(bool secondone)
{
    // default copy of direction + address
    int realstate = state;
    SrcpMessage::Protocol realprotocol = protocol1;
    int realactivetime = activetime1;
    int iRealBus = bus1;
    int iRealAddress = address1;


    // element contains a momentarily activated coupler
    if (classid == siciEnk && iSoldSubType != -1)
        realstate = iSoldSubType;  // copy subtype as realDirection

    // element contains a main signal with direction >= 2
    else if ((classid == siciHs1 || classid == siciHs3 ||
                classid == siciHss1 || classid == siciHss3 ||
              classid == siciVs1 || classid == siciVs3)
            && state >= 2) {

        // Hp0+Hp1 not considered, is done by default copy
        switch (iSoldSubType) {
            case 6:            // Hp0+Hp2,     --> state = 2
            case 7:            // Hp0+Hp2+Sh1, --> state = 2 or 3
                realstate = 1;
                // Sh1, case 7, HSS
                if (state == 3) {
                    iRealAddress = address2;
                    iRealBus = bus2;
                }
                break;
            case 4:            // Hp0+Hp1+Hp2, --> state = 2
                realstate = 0;
                iRealAddress = address2;
                iRealBus = bus2;
                break;
            case 1:            // Hp0+Hp1+Sh1, --> state = 3
            case 5:            // Hp0+Hp1+Hp2+Sh1, --> state = 2 or 3
                realstate = state - 2;
                iRealAddress = address2;
                iRealBus = bus2;
                break;
        }
    }

    // element contains a 3-way-turnout
    else if (classid == siciTw1 || classid == siciTw3) {

        // send second address data
        if (secondone) {
            realstate = (state > 1);
            realprotocol = protocol2;
            realactivetime = activetime2;
            iRealAddress = address2;
            iRealBus = bus2;
        }
        // send first address data
        else {
            realstate = state % 2;
        }
    }

    // element contains a 4-state-DKW or EKW
    else if ((classid == siciSl1 || classid == siciSl3 ||
                classid == siciSr1 || classid == siciSr3) ||
            ((classid == siciDl1 || classid == siciDr1) &&
             iSoldSubType == 1)) {

        // send second address data
        if (secondone) {
            realstate = (state == 1 || state == 2);
            realprotocol = protocol2;
            realactivetime = activetime2;
            iRealAddress = address2;
            iRealBus = bus2;
        }
        // send first address data
        else {
            realstate = (state >= 2);
        }
    }

    if (realprotocol == SrcpMessage::proNone)
        return;

    /*
     * The calculated real direction to be sent is modified
     * again if you electronically changed your decoder
     * outputs of one address.
     */
    if (classid != siciDre) {
        if (iRealAddress == address1)
            realstate = realstate ^ xchangeport1;
        if (iRealAddress == address2)
            realstate = realstate ^ xchangeport2;
    }

    int port = 0;

    switch (realprotocol) {

        // only two ports for MM
        case SrcpMessage::proMM:
            port = realstate;
            break; 

        // only two ports for DCC
        case SrcpMessage::proDCC: 
            port = !realstate;
            break;

        // multiple (paired) ports for Selectrix
        case SrcpMessage::proSelectrix:
            if (iRealAddress == address1)
                port = port1 + realstate;
            else
                port = port2 + realstate;
            break;

        // multiple (paired) ports for Server
        case SrcpMessage::proServer:
            if (iRealAddress == address1)
                port = port1 + realstate;
            else
                port = port2 + realstate;
            break;

        default:
            break;
    }

    SrcpMessage sm = SrcpMessage(SrcpMessage::msgGaSet);
    sm.setGaData(realprotocol, iRealBus, iRealAddress, port, 1,
            realactivetime);
    emit sendSrcpMessage(&sm);

    /*qWarning("Class: %d, Stype: %d, Dir: %d, A1: %d, A2: %d, RA: %d, P: %d",
            classid, iSoldSubType, state, address1,
            address2, iRealAddress, port);*/
    qApp->processEvents();
}


void element::sendSrcpState()
{
    /* do not send anything for rail buttons without signals */
    if (classid == siciRt1 || classid == siciRt3 ||
            classid == siciZt1 || classid == siciZt3)
        return;

    // switch only first address
    switchAddress(false);

    // switch also second address if there is one
    if ((classid == siciTw1 || classid == siciTw3 ||
                classid == siciSl1 || classid == siciSl3 ||
                classid == siciSr1 || classid == siciSr3 ||
                ((classid == siciDl1 || classid == siciDr1) &&
                 iSoldSubType == 1))) {
        switchAddress(true);
    }

    /*
     * if momentary coupler: activate it, wait for a short time
     * and deactivate it graphically
     */
    else if (classid == siciEnk && iSoldSubType != -1)
        QTimer::singleShot(activetime1, this,
                SLOT(repaintTimeOutEnk()));
}


void element::repaintTimeOutEnk()
{
    state = !state;
    setupElementIcon();
}

/*handle info messages for items with two decoders*/
void element::switch2AddressItem(unsigned int addr, unsigned int port)
{
    int realport;

    if (addr == (unsigned int)address1) {
        realport = port ^ xchangeport1;
        if (protocol1 == SrcpMessage::proDCC)
            realport = realport == 0 ? 1 : 0;

        if (classid == siciHss1 || classid == siciHss3) {
            // Subtype 7
            if (iSoldSubType == 7) {
                if (realport == 0 && state != 0) {
                    state = 0;
                    setupElementIcon();
                }
                else if (realport == 1 && state == 0) {
                    state = 2;
                    setupElementIcon();
                }
            }
            // Subtype 1 + 5
            else if (state != realport) {
                state = realport;
                setupElementIcon();
            }
        }

        else if (classid == siciHs1 || classid == siciHs3 ||
                classid == siciVs1 || classid == siciVs3 || 
                classid == siciTw1 || classid == siciTw3) {
            if (state != realport) {
                state = realport;
                setupElementIcon();
            }
        }

        // Crossing Subtype 1, decoder 1
        else if (classid == siciDl1 || classid == siciDr1) {
            if (state == 0 && realport == 1) {
                state = 3;
                setupElementIcon();
            }
            else if (state == 1 && realport == 1) {
                state = 2;
                setupElementIcon();
            }
            else if (state == 2 && realport == 0) {
                state = 1;
                setupElementIcon();
            }
            else if (state == 3 && realport == 0) {
                state = 0;
                setupElementIcon();
            }
        }

        // simple crossings, decoder 2
        else if (classid == siciSl1 || classid == siciSl3 ||
                classid == siciSr1 || classid == siciSr3) {
            if (state == 1 && realport == 1) {
                state = 2;
                setupElementIcon();
            }
            else if (state == 2 && realport == 0) {
                state = 1;
                setupElementIcon();
            }
        }

        //qWarning("A1, State: %d, RealPort: %d\n", state, realport);

    }
    else if (addr == (unsigned int)address2) {
        realport = port ^ xchangeport2;
        if (protocol2 == SrcpMessage::proDCC)
            realport = realport == 0 ? 1 : 0;

        if (classid == siciHss1 || classid == siciHss3) {
            // Subtype 1 + 7
            if (iSoldSubType == 1 || iSoldSubType == 7) {
                // TODO: Check, realport should be compared to 0!
                if (realport == 1 && state != 3) {
                    state = 3;
                    setupElementIcon();
                }
            }
            // Subtype 5
            else {
                if (realport == 0 && state != 2) {
                    state = 2;
                    setupElementIcon();
                }
                else if (realport == 1 && state != 3) {
                    state = 3;
                    setupElementIcon();
                }
            }
        }
        // Subtype 6
        else if (classid == siciHs1 || classid == siciHs3 ||
                classid == siciVs1 || classid == siciVs3) {
            if (state != 2 && realport == 0) {
                state = 2;
                setupElementIcon();
            }
        }

        // Crossing Subtype 1, decoder 2
        else if (classid == siciDl1 || classid == siciDr1) {
            if (state == 0 && realport == 1) {
                state = 1;
                setupElementIcon();
            }
            else if (state == 1 && realport == 0) {
                state = 0;
                setupElementIcon();
            }
            else if (state == 2 && realport == 0) {
                state = 3;
                setupElementIcon();
            }
            else if (state == 3 && realport == 1) {
                state = 2;
                setupElementIcon();
            }
        }

        // Tree way turnout, decoder 2
        else if (classid == siciTw1 || classid == siciTw3) {
            if (state != 2 && realport == 1) {
                state = 2;
                setupElementIcon();
            }
        }

        // simple crossings, decoder 1
        else if (classid == siciSl1 || classid == siciSl3 ||
                classid == siciSr1 || classid == siciSr3) {
            if (state == 0 && realport == 1) {
                state = 1;
                setupElementIcon();
            }
            else if (state == 1 && realport == 0) {
                state = 0;
                setupElementIcon();
            }
        }
        //qWarning("A2, State: %d, RealPort: %d\n", state, realport);
    }
    else
        return;
}

/**
 * this is the reverse case of "sendSrcpState()"
 */
void element::processInfoPortMessage(unsigned int bus,
        unsigned int addr, unsigned int port, unsigned int value)
{
    // ignore port reset messages
    if (value == 0)
        return;

    if (!switchable)
        return;

    if (blinkcounter != 0)
        return;
    
    if (classid == siciEnk && iSoldSubType != -1)
        return;
    
    if ((bus == (unsigned int)bus1 &&
                addr == (unsigned int)address1) ||
            (bus == (unsigned int)bus2 &&
             addr == (unsigned int)address2)) {

        if (address2 != -1) {
            //qWarning("InfoPass, Bus: %d, Addr: %d, Port: %d, Value: %d",
            //    bus, addr, port, value);
            switch2AddressItem(addr, port);
            return;
        }

        int realstate = state;
        if (realstate == 2) //Hp0-Hp2-Type (iSoldSubType == 6)
            realstate = 1;

        bool isDCC = (protocol1 == SrcpMessage::proDCC);
        if (isDCC)
            realstate = !realstate;

        /*invert direction if connectors are exchanged*/
        realstate = realstate ^ xchangeport1;

        /* modulo 2 if there are more than 2 ports per address*/
        if ((port % 2) != (unsigned int)realstate) {
            realstate = port % 2;

            /*again invert direction if connectors are exchanged*/
            realstate = realstate ^ xchangeport1;

            if (isDCC)
                realstate = !realstate;

            //Hp0-Hp2-Type (iSoldSubType == 6)
            if (iSoldSubType == 6 && realstate == 1)
                realstate = 2;

            state = realstate;
            setupElementIcon();
            // TODO: show warning message when element is locked
        }
    }
}

/**
 * toggle through element direction states
 */
void element::toggle()
{
    // toggles direction for 3-state-solenoids
    // 0 -> 1 -> 2 -> 0
    if (classid == siciTw1 ||classid == siciTw3 ||
        classid == siciSl1 || classid == siciSl3 ||
        classid == siciSr1 || classid == siciSr3) {
        if (state < 2)
            switchToDir(state + 1);
        else
            switchToDir(0);
    }

    // 0: Hp0, 1: Hp1, 2: Hp2 (resp. Vr...)
    // SubType 0: 0 -> 1 -> 0
    // SubType 4: 0 -> 1 -> 2 -> 0
    // SubType 6: 0 -> 2 -> 0
    else if (classid == siciHs1 || classid == siciHs3 ||
            classid == siciVs1 || classid == siciVs3) {
        switch (state) {
            case 0:
                if (iSoldSubType == 6)
                    switchToDir(2);
                else 
                    switchToDir(1);
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

    // 0: Hp0, 1: Hp1, 2: Hp2, 3: Sh1
    // SubType 1: 0 -> 1 -> 3 -> 0
    // SubType 5: 0 -> 1 -> 2 -> 3 -> 0
    // SubType 7: 0 -> 2 -> 3 -> 0
    else if (classid == siciHss1 || classid == siciHss3) {
        switch (state) {
            case 0:
                if (iSoldSubType == 7)
                    switchToDir(2);
                else 
                    switchToDir(1);
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
    // 0 -> 1 -> 2 -> 3 -> 0
    else if ((classid == siciDl1 || classid == siciDr1)
             && iSoldSubType == 1) {
        if (state < 3)
            switchToDir(state + 1);
        else
            switchToDir(0);
    }

    // toggles cyclic for 2-state-solenoids
    else
        switchToDir(state == 1 ? 0 : 1);
}

/**
 * get conditions for context menu in normal visual mode and route edit
 * mode
 */
bool element::ctxCanSwitch()
{
    return (!isLocked()) &&
        (!isOccupied() || simplega || signal) &&
        (visualMode == kvmNormal ||
         (visualMode == kvmEditRoute && ksmNormal == selectionMode));
}

/**
 * paint element icon
 */
void element::setupElementIcon()
{
    // empty symbol
    if (classid == siciTxt) {
        QRect br;

        if (iSoldInvert == 1) 
            background.fill(QColor(Qt::darkGray));
            
        // paint text label
        QPainter p(&background);

        int w = background.width();
        int h = background.height();

        // text and white rectangle for background
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 1);
            p.setFont(f);
            QFontMetrics fm(f);
            br = fm.boundingRect(sSoldText);
            br.setWidth(br.width() + 6);
            br.setHeight(br.height() + 4);
#if QT_VERSION >= 0x030100
            br.moveLeft((w - br.width())/2);
            br.moveTop((h - br.height())/2);
#else
            br.moveTopLeft(QPoint((w - br.width())/2,
                        (h - br.height())/2));
#endif
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(background.rect(), Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, sSoldText);
        }
        // no text available -> show white rectangle only
        else {
            br = QRect(13, 11, 30, 13);
            p.fillRect(br, QBrush(Qt::white));
        }

        setPaletteBackgroundPixmap(background);
    }
   
    // blue panel
    else if (classid == siciFeb) {
        background.fill(QColor(0, 0, 192));
        setPaletteBackgroundPixmap(background);
    }
   
    // blue panel with wgt button
    else if (classid == siciTaw) {
        background.fill(QColor(0, 0, 192));
        QPainter p(&background);
            
        int w = background.width();
        int h = background.height();

        // paint red light
        p.setBrush(Qt::red);
        p.drawEllipse(w / 2 - 2, h / 4 - 3, 5, 5);
        
        // paint button
        p.setBrush(Qt::darkGray);
        p.drawEllipse(w / 2 - 4, h / 2 - 4, 9, 9);
        
        // paint button label
        p.drawPixmap(18, 24, QPixmap(label_wgt_xpm));

        setPaletteBackgroundPixmap(background);
    }
   
    // blue panel with WHT button and counter
    else if (classid == siciTwh) {
        background.fill(QColor(0, 0, 192));
        QPainter p(&background);
            
        int w = background.width();
        int h = background.height();

        // paint button
        p.setBrush(Qt::darkGray);
        p.drawEllipse(7, h / 2 - 4, 9, 9);
        
        // paint button label
        p.drawPixmap(2, 24, QPixmap(label_wht_xpm));

        // paint counter
        p.setBrush(Qt::white);
        p.drawRect(w / 2 , h / 2 - 5, 24, 11);
        sSoldText.sprintf("%04d", countervalue);
        QFont f(QApplication::font());
        f.setPointSize(7);
        p.setFont(f);
        QRect br = p.fontMetrics().boundingRect(sSoldText);
        br.moveTopRight(QPoint(w / 2 + 21, h / 2 - 3));
        p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                Qt::DontClip, sSoldText);

        setPaletteBackgroundPixmap(background);
    }
   
    // green panel
    else if (classid == siciFeg) {
        background.fill(QColor(0, 160, 0));
        setPaletteBackgroundPixmap(background);
    }
   
    // green panel with FHT button and counter
    else if (classid == siciTaf) {
        background.fill(QColor(0, 160, 0));
        QPainter p(&background);
            
        int w = background.width();
        int h = background.height();

        // paint button
        p.setBrush(Qt::darkGray);
        p.drawEllipse(7, h / 2 - 4, 9, 9);
        
        // paint button label
        p.drawPixmap(2, 24, QPixmap(label_fht_xpm));

        // paint counter
        p.setBrush(Qt::white);
        p.drawRect(w / 2 , h / 2 - 5, 24, 11);
        sSoldText.sprintf("%04d", countervalue);
        QFont f(QApplication::font());
        f.setPointSize(7);
        p.setFont(f);
        QRect br = p.fontMetrics().boundingRect(sSoldText);
        br.moveTopRight(QPoint(w / 2 + 21, h / 2 - 3));
        p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                Qt::DontClip, sSoldText);

        setPaletteBackgroundPixmap(background);
    }
   
    // green panel with ufgt and mgt buttons
    else if (classid == siciTau) {
        background.fill(QColor(0, 160, 0));
        QPainter p(&background);
            
        int w = background.width();
        int h = background.height();

        // paint buttons
        p.setBrush(Qt::darkGray);
        p.drawEllipse(7, h / 2 - 4, 9, 9);
        p.drawEllipse(w - 16, h / 2 - 4, 9, 9);
        
        // paint button labels
        p.drawPixmap(2, 24, QPixmap(label_ufgt_xpm));
        p.drawPixmap(34, 24, QPixmap(label_mgt_xpm));

        setPaletteBackgroundPixmap(background);
    }
   
    // red panel
    else if (classid == siciFer) {
        background.fill(QColor(221, 0, 0));
        setPaletteBackgroundPixmap(background);
    }
   
    // red panel with sgt and hagt buttons
    else if (classid == siciTas) {
        background.fill(QColor(221, 0, 0));
        QPainter p(&background);
            
        int w = background.width();
        int h = background.height();

        // paint buttons
        p.setBrush(Qt::darkGray);
        p.drawEllipse(7, h / 2 - 4, 9, 9);
        p.drawEllipse(w - 16, h / 2 - 4, 9, 9);
        
        // paint button labels
        p.drawPixmap(2, 24, QPixmap(label_sgt_xpm));
        p.drawPixmap(31, 24, QPixmap(label_hagt_xpm));

        setPaletteBackgroundPixmap(background);
    }
   
    // yellow panel
    else if (classid == siciFey) {
        background.fill(QColor(224, 224, 0));
        setPaletteBackgroundPixmap(background);
    }
   
    // brown panel
    else if (classid == siciFen) {
        background.fill(QColor(112, 48, 0));
        setPaletteBackgroundPixmap(background);
    }
   
    // gray panel
    else if (classid == siciFee) {
        background.fill(QColor(128, 128, 128));
        setPaletteBackgroundPixmap(background);
    }
   
    // grey panel with light Ein and Aus buttons
    else if (classid == siciTal) {
        background.fill(Qt::darkGray);
        QPainter p(&background);
            
        int w = background.width();
        int h = background.height();

        // paint buttons
        p.setBrush(Qt::darkGray);
        p.drawEllipse(7, h / 2 - 4, 9, 9);
        p.drawEllipse(w - 16, h / 2 - 4, 9, 9);
        
        // paint button labels
        if (tablelight) {
            p.drawPixmap(2, 24, QPixmap(label_ein_on_xpm));
            p.drawPixmap(34, 24, QPixmap(label_aus_off_xpm));
        }
        else {
            p.drawPixmap(2, 24, QPixmap(label_ein_off_xpm));
            p.drawPixmap(34, 24, QPixmap(label_aus_on_xpm));
        }

        setPaletteBackgroundPixmap(background);
    }
   
    // buffer stop right (prellbock)
    else if (classid == siciBs1) {
        QPainter p(&background);
            
        int w = background.width();
        int h = background.height();

        // paint panel
        p.fillRect(w - 5, h / 2 - 6, w,
                13, QBrush(Qt::black));

        setPaletteBackgroundPixmap(background);
    }
   
    // buffer stop top
    else if (classid == siciBs2) {
        QPainter p(&background);
            
        int w = background.width();

        // paint panel
        p.fillRect(w / 2 - 6, 0, 13, 5, QBrush(Qt::black));

        setPaletteBackgroundPixmap(background);
    }
   
    // buffer stop left
    else if (classid == siciBs3) {
        QPainter p(&background);
            
        int h = background.height();

        // paint panel
        p.fillRect(0, h / 2 - 6, 5, 13, QBrush(Qt::black));

        setPaletteBackgroundPixmap(background);
    }
   
    // buffer stop bottom
    else if (classid == siciBs4) {
        QPainter p(&background);
            
        // paint panel
        int w = background.width();
        int h = background.height();

        p.fillRect(w / 2 - 6, h - 5, 13, 5, QBrush(Qt::black));

        setPaletteBackgroundPixmap(background);
    }
   
    // direction arrows
    else if (classid == siciTdr || classid == siciTdb) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
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

        p.setBrush(QBrush(Qt::black));

        // paint left arrow
        if (classid == siciTdb) {
            QPointArray leftarrow = QPointArray(4);
            leftarrow.putPoints(0, 4, w * 3 /4, h / 2,
                    w - 1, 1, w / 2 - 6, h / 2, w - 1, h - 1);
            p.drawPolygon(leftarrow);
        }
        // paint right arrow
        QPointArray rightarrow = QPointArray(4);
        rightarrow.putPoints(0, 4, w / 4, h / 2, 1, 1,
                w / 2 + 6, h / 2, 1, h - 1);
        p.drawPolygon(rightarrow);
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(w / 3, h / 2, 2 * w / 3, h / 2);
            p.setPen(QPen(Qt::black));
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QRect br = fm.boundingRect(sSoldText);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopLeft(QPoint(w / 2 - br.width() / 2, 2));
            
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, sSoldText);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // left direction arrow
    else if (classid == siciTdl) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint arrows
        p.setBrush(QBrush(Qt::black));
        QPointArray leftarrow = QPointArray(4);
        leftarrow.putPoints(0, 4, w * 3 /4, h / 2,
                w - 1, 1, w / 2 - 6, h / 2, w - 1, h - 1);
        p.drawPolygon(leftarrow);
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(w / 3, h / 2, 2 * w / 3, h / 2);
            p.setPen(QPen(Qt::black));
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QRect br = fm.boundingRect(sSoldText);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopLeft(QPoint(w / 2 - br.width() / 2, 2));
            
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, sSoldText);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // address
    else if (classid == siciAdr) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, 2, 7,
                QBrush(Qt::black));
        p.fillRect(w - 2, h / 2 - 3, 2, 7,
                QBrush(Qt::black));

        // paint address field
        p.setPen(QPen(Qt::black));
        p.setBrush(QColor(Qt::black));

        p.drawRect(2, 8, w - 4, h - 16);

        // paint address value
        p.setPen(QPen(Qt::red));

        QFont f(QApplication::font());
        f.setPointSize(QApplication::font().pointSize() + 1);
        f.setWeight(QFont::DemiBold);
#if QT_VERSION >= 0x030200
        f.setStretch(90);
#else
        f.setPointSizeFloat(f.pointSizeFloat() * .9);
#endif
        p.setFont(f);
        sSoldText.sprintf("%06d", editsAddress);
        QFontMetrics fm(f);
        QRect br = fm.boundingRect(sSoldText);
        br.moveTopLeft(QPoint(w / 2 - br.width() / 2,
                    h / 2 - br.height() / 2));
	p.drawText(br, Qt::AlignCenter | Qt::SingleLine | Qt::DontClip,
                sSoldText);
        
        setPaletteBackgroundPixmap(background);
    }
    
    // level crossing
    else if (classid == siciBue) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint road
        p.fillRect(w / 2 - 7, 0, 15, h, QBrush(Qt::darkGray));

        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));

        // paint track button
        p.drawPixmap(w / 2  - 4, h / 2 - 3, QPixmap(button_yellow_xpm));
        
        // paint lights
        p.setBrush(Qt::darkGray);
        p.drawEllipse(6, 4, 6, 6);
        p.drawEllipse(w - 12, 4, 6, 6);
        //p.drawRect(w / 2 - 3, 4, 6, 6);

        setPaletteBackgroundPixmap(background);
    }
    
    // relay
    else if (classid == siciRel) {
        QPainter p(&background);

        int w = background.width();
        int h = background.height();
        
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
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(w - 3, h - 3));
            
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        // paint signalization (black or yellow filled circle)
        if (state == 1)
            p.setBrush(QColor(Qt::yellow));
        else
            p.setBrush(QColor(Qt::black));

        p.drawEllipse(w * 3 / 4 - 3, h / 2 - 5, 11, 11);

        setPaletteBackgroundPixmap(background);
    }
    
    // motor
    else if (classid == siciMdc) {
        QPainter p(&background);

        int w = background.width();
        int h = background.height();
        
        // paint signalization (black or yellow filled circle)
        p.setPen(QPen(Qt::black, 2));

        if (state == 1)
            p.setBrush(QColor(Qt::yellow));

        p.drawEllipse(w / 2 - 9, h - 18 - 1, 17, 17);

        // paint symbol
        // red connector left
        p.setPen(Qt::red);
        p.drawLine(3, h - 9 - 2, w / 2 - 10, h - 9 - 2);
        p.drawLine(w / 2 - 11, h - 9 - 3, w / 2 - 10, h - 9 - 3);
        p.drawLine(w / 2 - 11, h - 9 - 1, w / 2 - 10, h - 9 - 1);
        // plus sign
        p.drawLine(3, h - 9 - 6, 7, h - 9 - 6);
        p.drawLine(5, h - 9 - 8, 5, h - 9 - 4);
        
        // blue connector right
        p.setPen(Qt::blue);
        p.drawLine(w - 5, h - 9 - 2, w / 2 + 8, h - 9 - 2);
        p.drawLine(w / 2 + 9, h - 9 - 3, w / 2 + 8, h - 9 - 3);
        p.drawLine(w / 2 + 9, h - 9 - 1, w / 2 + 8, h - 9 - 1);
        // minus sign
        p.drawLine(w - 5, h - 9 - 6, w - 9, h - 9 - 6);

        // motor labels
        p.setPen(Qt::black);
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
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(address2);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopLeft(QPoint(w / 2 - br.width() / 2, 2));
            
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // decoupler
    else if (classid == siciEnk) {
        QPainter p(&background);

        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, background.height() / 2 - 3, w, 7,
                QBrush(Qt::black));
        
        // paint symbol
        p.fillRect(21, 10, 14, 3, QBrush(Qt::black));
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(w / 3, h / 2,
                    2 * w / 3, h / 2);
            p.setPen(Qt::black);
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(w / 2 + br.width()/2,
                        h - 3));
            
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        // paint signalization (white triangle)
        if (state == 1) {
            p.setPen(Qt::white);
            p.setBrush(QColor(Qt::white));
            QPointArray triangle = QPointArray(4);
            triangle.putPoints(0, 4, 21, 9, 27, 3, 28, 3, 34, 9);
            p.drawPolygon(triangle);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // blind element
    else if (classid == siciBld) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(w / 3, h / 2, 2 * w / 3, h / 2);
            p.setPen(Qt::black);
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(w / 2 + br.width()/2, h - 3));
            
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        // paint signalization
        if (state == 1)
            p.setPen(Qt::green);
        else
            p.setPen(Qt::red);
        
        p.drawLine(19, 12, 19 + 17, 12);
        p.drawLine(18, 13, 18 + 19, 13);
        p.drawLine(18, 21, 18 + 19, 21);
        p.drawLine(19, 22, 19 + 17, 22);

        setPaletteBackgroundPixmap(background);
    }
    
    // right shunt wait signal
    else if (classid == siciWs1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(5 * w / 6 - 4 , h / 2 - 3, 
                QPixmap(button_gray_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 5, h - 10, 5, 5);

        // paint signal icon
        p.drawPixmap(w - 16 , h / 2 + 5, QPixmap(signal_w_xpm));
        p.fillRect(w - 25, 25, 2, 5, QBrush(Qt::black));
        p.drawLine(w - 15, 27, w - 23, 27);

        // paint signal light
        if (state == 1) {
            p.setPen(QPen(Qt::white));
            p.setBrush(Qt::white);
        }
        else
            p.setBrush(Qt::darkGray);

        p.drawRect(w - 18, h / 2 + 6, 3, 3);
        p.drawRect(w - 8, h / 2 + 13, 3, 3);
        p.setPen(QPen(Qt::black));

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopLeft(QPoint(w / 2 - br.width()/2, 2));
            
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // left shunt wait signal
    else if (classid == siciWs3) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(w / 6  - 4, h / 2 - 3,
                QPixmap(button_gray_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 1, 5, 5, 5);

        // paint signal icon
        p.drawPixmap(6, 2, QPixmap(signal_wr_xpm));
        p.fillRect(22, 5, 2, 5, QBrush(Qt::black));
        p.drawLine(13, 7, 21, 7);
 
        // paint signal light
        if (state == 1) {
            p.setPen(QPen(Qt::white));
            p.setBrush(Qt::white);
        }
        else
            p.setBrush(Qt::darkGray);

        p.drawRect(4, 2, 3, 3);
        p.drawRect(14, 9, 3, 3);
        p.setPen(QPen(Qt::black));

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(w / 2 + br.width()/2, h - 3));

            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }

    // right signal HSS
    else if (classid == siciHss1) {
        QPainter p(&background);

        int w = background.width();
        int h = background.height();

        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(5 * w / 6 - 4 , h / 2 - 3, 
                QPixmap(button_red_xpm));
        p.drawPixmap(w / 6  - 4, h / 2 - 3,
                QPixmap(button_gray_xpm));

        // paint signal icon
        p.setBrush(Qt::black);
        p.drawEllipse(17, h - 11, LIGHTSIZE, LIGHTSIZE);
        p.fillRect(22, h - 11, 24, 7, QBrush(Qt::black));
        p.fillRect(12, 25, 2, 5, QBrush(Qt::black));
        p.drawLine(14, 27, 16, 27);

        /**
         *   state subtype bottom-left bottom-right top-left top-right
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
 
        // fprintf(stderr, "dir: %d type: %d\n", state, iSoldSubType);

        // paint signal light
        // fix potential wrong direction value
        if (iSoldSubType == 6 && state == DIR_HP1)
            state = DIR_HP2;

        switch (state) {
            case DIR_HP0:              // HP0 => 0
                p.setBrush(Qt::red);
                // bottom
                p.drawEllipse(w - 22, h - 11, LIGHTSIZE, LIGHTSIZE);
                // top
                p.setBrush(Qt::black);
                p.drawEllipse(w - 12, h - 11, LIGHTSIZE, LIGHTSIZE);
                break;
            case DIR_HP2:              // HP2 => 2
                if (pref.hp2) {
                    p.setBrush(Qt::yellow);
                    // top
                    p.drawEllipse(w - 12, h - 11, LIGHTSIZE, LIGHTSIZE);
                    // bottom
                    p.setBrush(Qt::black);
                    p.drawEllipse(w - 22, h - 11, LIGHTSIZE, LIGHTSIZE);
                    break;
                }
                // fall through
            case DIR_HP1:              // HP1 => 1
                p.setBrush(Qt::green);
                // top
                p.drawEllipse(w - 12, h - 11, LIGHTSIZE, LIGHTSIZE);
                // bottom
                p.setBrush(Qt::black);
                p.drawEllipse(w - 22, h - 11, LIGHTSIZE, LIGHTSIZE);
                break;
            case DIR_SH1:              // SH1 => 3
                {
                p.setPen(Qt::yellow);
                // paint only shunt light
                int startx = 19;
                p.drawLine(startx, h - 10, startx + 4, h - 6);
                p.drawLine(startx + 1, h - 10, startx + 5, h - 6);
                p.setPen(Qt::black);
                // top
                p.drawEllipse(w - 12, h - 11, LIGHTSIZE, LIGHTSIZE);
                break;
                }
        }

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(5, h - 10, 5, 5);

        // paint FfM
        if (ffm) {
            p.setBrush(ffmactive ? Qt::yellow : Qt::darkGray);
            p.drawRect(w - 11, 4, 6, 6);
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopLeft(QPoint(w / 2 - br.width()/2, 2));
            
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // left signal HSS
    else if (classid == siciHss3) {
        QPainter p(&background);

        int w = background.width();
        int h = background.height();

        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(w / 6  - 4, h / 2 - 3,
                QPixmap(button_red_xpm));
        p.drawPixmap(5 * w / 6  - 4, h / 2 - 3,
                QPixmap(button_gray_xpm));

        // paint signal icon
        p.setBrush(Qt::black);
        p.drawEllipse(31, 4, LIGHTSIZE, LIGHTSIZE);
        p.fillRect(9, 4, 24, 7, QBrush(Qt::black));
        p.fillRect(41, 5, 2, 5, QBrush(Qt::black));
        p.drawLine(38, 7, 40, 7);

        /**
         *   state  subtype bottom-left bottom-right top-left top-right
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
 
        // fprintf(stderr, "dir: %d type: %d\n", state, iSoldSubType);

        // paint signal light
        // fix potential wrong direction value
        if (iSoldSubType == 6 && state == DIR_HP1)
            state = DIR_HP2;

        switch (state) {
            case DIR_HP0:              // HP0 => 0
                p.setBrush(Qt::red);
                // bottom
                p.drawEllipse(14, 4, LIGHTSIZE, LIGHTSIZE);
                // top
                p.setBrush(Qt::black);
                p.drawEllipse(4, 4, LIGHTSIZE, LIGHTSIZE);
                break;
            case DIR_HP2:              // HP2 => 2
                if (pref.hp2) {
                    p.setBrush(Qt::yellow);
                    // top
                    p.drawEllipse(4, 4, LIGHTSIZE, LIGHTSIZE);
                    // bottom
                    p.setBrush(Qt::black);
                    p.drawEllipse(14, 4, LIGHTSIZE, LIGHTSIZE);
                    break;
                }
                // fall through
            case DIR_HP1:              // HP1 => 1
                p.setBrush(Qt::green);
                // top
                p.drawEllipse(4, 4, LIGHTSIZE, LIGHTSIZE);
                // bottom
                p.setBrush(Qt::black);
                p.drawEllipse(14, 4, LIGHTSIZE, LIGHTSIZE);
                break;
            case DIR_SH1:              // SH1 => 3
                {
                    p.setPen(Qt::yellow);
                    // paint only shunt light
                    int startx = w - 22;
                    p.drawLine(startx, 9, startx - 4, 5);
                    p.drawLine(startx + 1, 9, startx - 3, 5);
                    p.setPen(Qt::black);
                    // top
                    p.drawEllipse(4, 4, LIGHTSIZE, LIGHTSIZE);
                    break;
                }
        }

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w - 11, 5, 5, 5);

        // paint FfM
        if (ffm) {
            p.setBrush(ffmactive ? Qt::yellow : Qt::darkGray);
            p.drawRect(5, h - 10, 6, 6);
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(w / 2 + br.width()/2, h - 3));
            
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // signal right HS
    else if (classid == siciHs1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(5 * w / 6 - 4 , h / 2 - 3, 
                QPixmap(button_red_xpm));

        // paint signal icon
        p.fillRect(w - 20, h - 11, 10, 7, QBrush(Qt::black));
        p.fillRect(w - 27, 25, 2, 5, QBrush(Qt::black));
        p.drawLine(w - 23, 27, w - 25, 27);

        /**
         *   state subtype bottom-left bottom-right top-left top-right
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
 
        // fprintf(stderr, "dir: %d type: %d\n", state, iSoldSubType);

        // paint signal light
        // fix potential wrong direction value
        if (iSoldSubType == 6 && state == DIR_HP1)
            state = DIR_HP2;

        switch (state) {
            case DIR_HP0:              // HP0 => 0
                p.setBrush(Qt::red);
                // bottom
                p.drawEllipse(w - 22, h - 11, LIGHTSIZE, LIGHTSIZE);
                // top
                p.setBrush(Qt::black);
                p.drawEllipse(w - 12, h - 11, LIGHTSIZE, LIGHTSIZE);
                break;
            case DIR_HP2:              // HP2 => 2
                if (pref.hp2) {
                    p.setBrush(Qt::yellow);
                    // top
                    p.drawEllipse(w - 12, h - 11, LIGHTSIZE, LIGHTSIZE);
                    // bottom
                    p.setBrush(Qt::black);
                    p.drawEllipse(w - 22, h - 11, LIGHTSIZE, LIGHTSIZE);
                    break;
                }
                // fall through
            case DIR_HP1:              // HP1 => 1
                p.setBrush(Qt::green);
                // top
                p.drawEllipse(w - 12, h - 11, LIGHTSIZE, LIGHTSIZE);
                // bottom
                p.setBrush(Qt::black);
                p.drawEllipse(w - 22, h - 11, LIGHTSIZE, LIGHTSIZE);
                break;
        }

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 6, h - 10, 5, 5);

        // paint FfM
        if (ffm) {
            p.setBrush(ffmactive ? Qt::yellow : Qt::darkGray);
            p.drawRect(w - 11, 4, 6, 6);
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopLeft(QPoint(w / 2 - br.width()/2, 2));
            
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // signal left HS
    else if (classid == siciHs3) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(w / 6  - 4, h / 2 - 3,
                QPixmap(button_red_xpm));

        // paint signal icon
        p.fillRect(7, 4, 10, 7, QBrush(Qt::black));
        p.fillRect(24, 5, 2, 5, QBrush(Qt::black));
        p.drawLine(21, 7, 24, 7);

        /**
         *   state subtype bottom-left bottom-right top-left top-right
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
 
        // paint signal light
        // fix potential wrong direction value
        if (iSoldSubType == 6 && state == DIR_HP1)
            state = DIR_HP2;

        switch (state) {
            case DIR_HP0:              // HP0 => 0
                p.setBrush(Qt::red);
                // bottom
                p.drawEllipse(14, 4, LIGHTSIZE, LIGHTSIZE);
                // top
                p.setBrush(Qt::black);
                p.drawEllipse(4, 4, LIGHTSIZE, LIGHTSIZE);
                break;
            case DIR_HP2:              // HP2 => 2
                if (pref.hp2) {
                    p.setBrush(Qt::yellow);
                    // top
                    p.drawEllipse(4, 4, LIGHTSIZE, LIGHTSIZE);
                    // bottom
                    p.setBrush(Qt::black);
                    p.drawEllipse(14, 4, LIGHTSIZE, LIGHTSIZE);
                    break;
                }
                // fall through
            case DIR_HP1:              // HP1 => 1
                p.setBrush(Qt::green);
                // top
                p.drawEllipse(4, 4, LIGHTSIZE, LIGHTSIZE);
                // bottom
                p.setBrush(Qt::black);
                p.drawEllipse(14, 4, LIGHTSIZE, LIGHTSIZE);
                break;
        }

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2, 5, 5, 5);

        // paint FfM
        if (ffm) {
            p.setBrush(ffmactive ? Qt::yellow : Qt::darkGray);
            p.drawRect(5, h - 10, 6, 6);
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(w / 2 + br.width()/2, h - 3));
            
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // right signal VS
    else if (classid == siciVs1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        // paint signal icon
        QPointArray icon = QPointArray(6);
        icon.putPoints(0, 6, 0, 0, 0, 3, 3, 6, 12, 6, 12, 3, 9, 0);
        p.setBrush(Qt::black);

        icon.translate(w - 18, h - 11);
        p.drawPolygon(icon);
        p.fillRect(w - 23, 25, 2, 5, QBrush(Qt::black));
        p.drawLine(w - 18, 27, w - 21, 27);

        /**
         *   state subtype bottom-left bottom-right top-left top-right
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
 
        // fprintf(stderr, "dir: %d type: %d\n", state, iSoldSubType);

        // paint signal light
        // fix potential wrong direction value
        if (iSoldSubType == 6 && state == DIR_HP1)
            state = DIR_HP2;

        switch (state) {
            case DIR_HP0:              // VS0 => 0
                // bottom left
                p.fillRect(w - 17, h - 10, 2, 2, QBrush(Qt::yellow));
                // bottom right
                p.fillRect(w - 15, h - 7, 2, 2, QBrush(Qt::yellow));
                break;
            case DIR_HP2:              // VS2 => 2
                if (pref.hp2) {
                    // bottom left
                    p.fillRect(w - 17, h - 10, 2, 2, QBrush(Qt::yellow));
                    //top right
                    p.fillRect(w - 8, h - 7, 2, 2, QBrush(Qt::green));
                    break;
                }
            case DIR_HP1:              // VS1 => 1
                //top left
                p.fillRect(w - 10, h - 10, 2, 2, QBrush(Qt::green));
                //top right
                p.fillRect(w - 8, h - 7, 2, 2, QBrush(Qt::green));
                break;
        }

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
            p.setPen(QPen(Qt::black));
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopLeft(QPoint(w / 2 - br.width()/2, 2));
            
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // left signal VS
    else if (classid == siciVs3) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        // paint signal icon
        QPointArray icon = QPointArray(6);
        icon.putPoints(0, 6, 0, 0, 0, 3, 3, 6, 12, 6, 12, 3, 9, 0);
        p.setBrush(Qt::black);

        icon.translate(4, 4);
        p.drawPolygon(icon);
        p.fillRect(20, 5, 2, 5, QBrush(Qt::black));
        p.drawLine(17, 7, 20, 7);

        /**
         *   state subtype bottom-left bottom-right top-left top-right
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
 
        // fprintf(stderr, "dir: %d type: %d\n", state, iSoldSubType);

        // paint signal light
        // fix potential wrong direction value
        if (iSoldSubType == 6 && state == DIR_HP1)
            state = DIR_HP2;

        switch (state) {
            case DIR_HP0:              // VS0 => 0
                // bottom left
                p.fillRect(14, 8, 2, 2, QBrush(Qt::yellow));
                // bottom right
                p.fillRect(12, 5, 2, 2, QBrush(Qt::yellow));
                break;
            case DIR_HP2:              // VS2 => 2
                if (pref.hp2) {
                    // bottom left
                    p.fillRect(14, 8, 2, 2, QBrush(Qt::yellow));
                    //top right
                    p.fillRect(5, 5, 2, 2, QBrush(Qt::green));
                    break;
                }
            case DIR_HP1:              // VS1 => 1
                //top left
                p.fillRect(7, 8, 2, 2, QBrush(Qt::green));
                //top right
                p.fillRect(5, 5, 2, 2, QBrush(Qt::green));
                break;
        }

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
            p.setPen(QPen(Qt::black));
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(w / 2 + br.width()/2, h - 3));
            
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // right signal ZP
    else if (classid == siciZp1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 3, h - 10, 5, 5);

        // paint signal icon
        p.fillRect(w - 23, 25, 2, 5, QBrush(Qt::black));
        p.fillRect(w - 16, 22, 11, 11, QBrush(Qt::black));
        p.drawLine(w - 17, 27, w - 21, 27);
 
        // paint signal light
        QPointArray lights = QPointArray(12);
        lights.putPoints(0, 12, 3, 0, 5, 0, 1, 1, 7, 1, 0, 3, 8, 3,
                0, 5, 8, 5, 1, 7, 7, 7, 3, 8, 5, 8);

        lights.translate(w - 15, 23);

        if (state == 1)
            p.setPen(QPen(Qt::green));
        else
            p.setPen(QPen(Qt::darkGray));

        p.drawPoints(lights);
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
            p.setPen(QPen(Qt::black));
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopLeft(QPoint(w / 2 - br.width()/2, 2));
            
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // left signal ZP
    else if (classid == siciZp3) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 2, 5, 5, 5);

        // paint signal icon
        p.fillRect(21, 5, 2, 5, QBrush(Qt::black));
        p.fillRect(5, 2, 11, 11, QBrush(Qt::black));
        p.drawLine(16, 7, 20, 7);
 
        // paint signal light
        QPointArray lights = QPointArray(12);
        lights.putPoints(0, 12, 3, 0, 5, 0, 1, 1, 7, 1, 0, 3, 8, 3,
                0, 5, 8, 5, 1, 7, 7, 7, 3, 8, 5, 8);

        lights.translate(6, 3);

        if (state == 1)
            p.setPen(QPen(Qt::green));
        else
            p.setPen(QPen(Qt::darkGray));

        p.drawPoints(lights);
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
            p.setPen(QPen(Qt::black));
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;
            
            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(w / 2 + br.width()/2, h - 3));
            
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // straight track
    else if (classid == siciSt1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(w / 3, h / 2, 2 * w / 3, h / 2);
            p.setPen(QPen(Qt::black));
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 1);
            p.setFont(f);
            QFontMetrics fm(f);

            QRect br = fm.boundingRect(sSoldText);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);
            //QRect br = background.rect();
            //br.setHeight(h / 2 - 7);

#if QT_VERSION >= 0x030100
            br.moveTop(1);
#else
            br.moveTopLeft(QPoint(br.x(), 1));
#endif
            
            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, sSoldText);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // vertical track
    else if (classid == siciSt2) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(w / 2 - 3, 0, 7, h, QBrush(Qt::black));
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 4; ++i)
                p.fillRect(w / 2 - 2 , 4 + 7 * i, 5, 5,
                        QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(w / 2, h / 3, w / 2, 2 * h / 3);
            p.setPen(QPen(Qt::black));
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // vertical turn top left
    else if (classid == siciCl2) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(w / 2 - 3, h / 2 - 1, 7, h  - 1, QBrush(Qt::black));

        p.save();
        p.translate(w / 2, h / 2);
        p.rotate(-WANGLE);
        p.setPen(QPen(Qt::black, 7));
        p.drawLine(-1, 0, w / 2 + 5, 0);
        p.restore();
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 2; ++i)
                p.fillRect(w / 2 - 2 , h / 2 + 2 + 7 * i, 5, 5,
                        QBrush(Qt::lightGray));
            p.save();
            p.translate(w / 2, h / 2);
            p.rotate(-WANGLE);
            for (int i = 0; i < 3; ++i)
                p.fillRect(5 + 7 * i, -2, 5, 5,
                        QBrush(Qt::lightGray));
            p.restore();
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(w / 2, h / 2 + 2, w / 2, 2 * h / 3);
            p.save();
            p.translate(w / 2, h / 2);
            p.rotate(-WANGLE);
            p.drawLine(0, 0, w / 5, 0);
            p.restore();
            p.setPen(QPen(Qt::black));
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // vertical turn top right
    else if (classid == siciCr2) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(w / 2 - 3, h / 2 - 1, 7, h  - 1, QBrush(Qt::black));

        p.save();
        p.translate(w / 2, h / 2);
        p.rotate(-SANGLE);
        p.setPen(QPen(Qt::black, 7));
        p.drawLine(-1, 0, w / 2 + 5, 0);
        p.restore();
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 2; ++i)
                p.fillRect(w / 2 - 2 , h / 2 + 2 + 7 * i, 5, 5,
                        QBrush(Qt::lightGray));
            p.save();
            p.translate(w / 2, h / 2);
            p.rotate(-SANGLE);
            for (int i = 0; i < 3; ++i)
                p.fillRect(5 + 7 * i, -2, 5, 5,
                        QBrush(Qt::lightGray));
            p.restore();
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(w / 2, h / 2 + 2, w / 2, 2 * h / 3);
            p.save();
            p.translate(w / 2, h / 2);
            p.rotate(-SANGLE);
            p.drawLine(0, 0, w / 5, 0);
            p.restore();
            p.setPen(QPen(Qt::black));
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // vertical turn bottom left
    else if (classid == siciCl4) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(w / 2 - 3, 0, 7, h / 2 + 2, QBrush(Qt::black));

        p.save();
        p.translate(w / 2, h / 2);
        p.rotate(SANGLE);
        p.setPen(QPen(Qt::black, 7));
        p.drawLine(-1, 0, w / 2 + 5, 0);
        p.restore();
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 2; ++i)
                p.fillRect(w / 2 - 2 , 4 + 7 * i, 5, 5,
                        QBrush(Qt::lightGray));
            p.save();
            p.translate(w / 2, h / 2 - 1);
            p.rotate(SANGLE);
            for (int i = 0; i < 3; ++i)
                p.fillRect(5 + 7 * i, -2, 5, 5,
                        QBrush(Qt::lightGray));
            p.restore();
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(w / 2, h / 3, w / 2, h / 2 - 2);
            p.save();
            p.translate(w / 2, h / 2 - 1);
            p.rotate(SANGLE);
            p.drawLine(0, 0, w / 5, 0);
            p.restore();
            p.setPen(QPen(Qt::black));
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // vertical turn bottom right
    else if (classid == siciCr4) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(w / 2 - 3, 0, 7, h / 2 + 2, QBrush(Qt::black));

        p.save();
        p.translate(w / 2, h / 2);
        p.rotate(WANGLE);
        p.setPen(QPen(Qt::black, 7));
        p.drawLine(-1, 0, w / 2 + 5, 0);
        p.restore();
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 2; ++i)
                p.fillRect(w / 2 - 2 , 4 + 7 * i, 5, 5,
                        QBrush(Qt::lightGray));
            p.save();
            p.translate(w / 2, h / 2);
            p.rotate(WANGLE);
            for (int i = 0; i < 3; ++i)
                p.fillRect(5 + 7 * i, -2, 5, 5,
                        QBrush(Qt::lightGray));
            p.restore();
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(w / 2, h / 3, w / 2, h / 2 - 2);
            p.save();
            p.translate(w / 2, h / 2 - 1);
            p.rotate(WANGLE);
            p.drawLine(0, 0, w / 5, 0);
            p.restore();
            p.setPen(QPen(Qt::black));
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // track turn right and left
    else if (classid == siciCr1 || classid == siciCl1) {
        bool left = (classid == siciCl1);

        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w / 2 + 2, 7, QBrush(Qt::black));

        p.save();
        p.translate(w / 2, h / 2);

        if (left)
            p.rotate(-SANGLE);
        else
            p.rotate(SANGLE);

        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, 0, w / 2 + 5, 0);
        p.restore();

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 3; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
            p.save();
            p.translate(w / 2, h / 2);

            if (left)
                p.rotate(-SANGLE);
            else
                p.rotate(SANGLE);

            for (int i = 0; i < 3; ++i)
                p.fillRect(5 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.restore();
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
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

        setPaletteBackgroundPixmap(background);
    }

    // (rotated) track turn right and left
    else if (classid == siciCr3 || classid == siciCl3) {
        bool left = (classid == siciCl3);

        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(w / 2 - 1, h / 2 - 3, w, 7, QBrush(Qt::black));

        p.save();
        p.translate(w / 2, h / 2);

        if (left)
            p.rotate(WANGLE);
        else
            p.rotate(-WANGLE);

        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, 0, w / 2 + 5, 0);
        p.restore();

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 4; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));

            p.save();
            p.translate(w / 2, h / 2);

            if (left)
                p.rotate(WANGLE);
            else
                p.rotate(-WANGLE);

            for (int i = 0; i < 3; ++i)
                p.fillRect(5 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.restore();
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
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

        setPaletteBackgroundPixmap(background);
    }

    // diagonal track right and left
    else if (classid == siciSt3 || classid == siciSt4) {
        bool left = (classid == siciSt4);

        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.setPen(QPen(Qt::black, 7));
        if (left)
            p.drawLine(0, h - 1, w, 0);
        else
            p.drawLine(0, 0, w, h - 1);

        // paint track lights
        if (trackindicatoroff) {
            p.save();
            p.translate(w / 2, h / 2);

            if (left)
                p.rotate(-SANGLE);
            else
                p.rotate(SANGLE);

            for (int i = -3; i < 4; ++i)
                p.fillRect(-3 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));

            p.restore();
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
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
        setPaletteBackgroundPixmap(background);
    }

    // diagonal crossing (hosentraeger)
    else if (classid == siciKrh) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, 0, w, h - 1);
        p.drawLine(0, h - 1, w, 0);
        
        // paint track lights
        if (trackindicatoroff) {
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.rotate(2 * SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
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
                    c1 = QColor(Qt::red);
                    c2 = QColor(Qt::darkGray);
                }
                else if (routedtrack == 2) {
                    c1 = QColor(Qt::darkGray);
                    c2 = QColor(Qt::red);
                }
                else {
                    c1 = QColor(Qt::red);
                    c2 = QColor(Qt::red);
                }
            else {
                if (routed)
                    if (routedtrack == 1) {
                        c1 = QColor(255, 225, 0);
                        c2 = QColor(Qt::darkGray);
                    }
                    else {
                        c1 = QColor(Qt::darkGray);
                        c2 = QColor(255, 225, 0);
                    }
                else {
                    c1 = QColor(Qt::darkGray);
                    c2 = QColor(Qt::darkGray);
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

        // paint lock light
        p.setPen(QPen(Qt::black, 1));
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 4 - 3, h / 2 - 2, 5, 5);

        setPaletteBackgroundPixmap(background);
    }
    
    // left crossing
    else if (classid == siciKl1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, h / 2, w, h / 2);
        p.drawLine(0, h - 1, w, 0);
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
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
                    c1 = QColor(Qt::red);
                    c2 = QColor(Qt::darkGray);
                }
                else if (routedtrack == 2) {
                    c1 = QColor(Qt::darkGray);
                    c2 = QColor(Qt::red);
                }
                else {
                    c1 = QColor(Qt::red);
                    c2 = QColor(Qt::red);
                }
            else {
                if (routed)
                    if (routedtrack == 1) {
                        c1 = QColor(255, 225, 0);
                        c2 = QColor(Qt::darkGray);
                    }
                    else {
                        c1 = QColor(Qt::darkGray);
                        c2 = QColor(255, 225, 0);
                    }
                else {
                    c1 = QColor(Qt::darkGray);
                    c2 = QColor(Qt::darkGray);
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

        // paint lock light
        p.setPen(QPen(Qt::black, 1));
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 4, 6, 5, 5);

        setPaletteBackgroundPixmap(background);
    }
    
    // right crossing
    else if (classid == siciKr1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, h / 2, w, h / 2);
        p.drawLine(0, 0, w, h - 1);
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
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
                    c1 = QColor(Qt::red);
                    c2 = QColor(Qt::darkGray);
                }
                else if (routedtrack == 2) {
                    c1 = QColor(Qt::darkGray);
                    c2 = QColor(Qt::red);
                }
                else {
                    c1 = QColor(Qt::red);
                    c2 = QColor(Qt::red);
                }
            else {
                if (routed)
                    if (routedtrack == 1) {
                        c1 = QColor(255, 225, 0);
                        c2 = QColor(Qt::darkGray);
                    }
                    else {
                        c1 = QColor(Qt::darkGray);
                        c2 = QColor(255, 225, 0);
                    }
                else {
                    c1 = QColor(Qt::darkGray);
                    c2 = QColor(Qt::darkGray);
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

        // paint lock light
        p.setPen(QPen(Qt::black, 1));
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(3 * w/ 4 - 4, 6, 5, 5);

        setPaletteBackgroundPixmap(background);
    }
    
    // single slip switch left position 1
    else if (classid == siciSl1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, h / 2, w, h / 2);
        p.drawLine(0, h - 1, w, 0);
        
        // paint drive symbol
        p.setPen(QPen(Qt::black, 1));
        p.drawLine(w / 2 - 7, h / 2 - 5, w / 2 - 2, h / 2 - 5);
        p.drawLine(w / 2 - 2, h / 2 - 5, w / 2 + 3, h / 2 - 8);

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 4, 6, 5, 5);

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
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
                if (state == 0) {
                    if (lightson) {
                        c1 = QColor(Qt::red);
                        c2 = QColor(Qt::red);
                    }
                    else {
                        c1 = QColor(Qt::darkGray);
                        c2 = QColor(Qt::darkGray);
                    }
                    c3 = QColor(Qt::darkGray);
                    c4 = QColor(Qt::darkGray);
                }
                else if (state == 1) {
                    if (lightson) {
                        c1 = QColor(Qt::red);
                        c4 = QColor(Qt::red);
                    }
                    else {
                        c1 = QColor(Qt::darkGray);
                        c4 = QColor(Qt::darkGray);
                    }
                    c2 = QColor(Qt::darkGray);
                    c3 = QColor(Qt::darkGray);
                }
                else if (state == 2) {
                    c1 = QColor(Qt::darkGray);
                    c2 = QColor(Qt::darkGray);
                    if (lightson) {
                        c3 = QColor(Qt::red);
                        c4 = QColor(Qt::red);
                    }
                    else {
                        c3 = QColor(Qt::darkGray);
                        c4 = QColor(Qt::darkGray);
                    }
                }
                // error indication
                else {
                    c1 = QColor(Qt::red);
                    c2 = QColor(Qt::red);
                    c3 = QColor(Qt::red);
                    c4 = QColor(Qt::red);
                }
            }
            else {
                if (state == 0) {
                    if (!lightson && ((lastdir == 0 && newdir == 2) ||
                            (lastdir == 2 && newdir == 0)))
                        c1 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c1 = QColor(255, 225, 0);
                        else 
                            c1 = QColor(Qt::darkGray);
                    }
                    
                    if (!lightson && ((lastdir != 0 && newdir == 0) ||
                            (lastdir == 0 && newdir != 0)))
                        c2 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c2 = QColor(255, 225, 0);
                        else 
                            c2 = QColor(Qt::darkGray);
                    }
                    
                    c3 = QColor(Qt::darkGray);
                    c4 = QColor(Qt::darkGray);
                }
                else if (state == 1) {
                    if (!lightson && ((lastdir == 1 && newdir == 2) ||
                        (lastdir == 2 && newdir == 1)))
                        c1 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c1 = QColor(255, 225, 0);
                        else 
                            c1 = QColor(Qt::darkGray);
                    }
                    
                    c2 = QColor(Qt::darkGray);
                    c3 = QColor(Qt::darkGray);
                    
                    if (!lightson && ((lastdir == 1 && newdir == 0) ||
                                (lastdir == 0 && newdir == 1)))
                        c4 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c4 = QColor(255, 225, 0);
                        else 
                            c4 = QColor(Qt::darkGray);
                    }
                }
                else if (state == 2) {
                    c1 = QColor(Qt::darkGray);
                    c2 = QColor(Qt::darkGray);
                    
                    if (!lightson && ((lastdir == 2 && newdir != 2) ||
                                (lastdir != 2 && newdir == 2)))
                        c3 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c3 = QColor(255, 225, 0);
                        else 
                            c3 = QColor(Qt::darkGray);
                    }
                    
                    if (!lightson && ((lastdir == 2 && newdir == 0) ||
                                (lastdir == 0 && newdir == 2)))
                        c4 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c4 = QColor(255, 225, 0);
                        else 
                            c4 = QColor(Qt::darkGray);
                    }
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

            p.setPen(QPen(Qt::black, 1));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));
        
        // paint text label
        p.setPen(Qt::black);
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(w - 3, h - 3));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // single slip switch left position 3
    else if (classid == siciSl3) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, h / 2, w, h / 2);
        p.drawLine(0, h - 1, w, 0);
        
        // paint drive symbol
        p.setPen(QPen(Qt::black, 1));
        p.drawLine(w / 2 + 1, h / 2 + 5, w / 2 + 7, h / 2 + 5);
        p.drawLine(w / 2 + 2, h / 2 + 5, w / 2 - 3, h / 2 + 7);

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(3 * w / 4 - 2, h - 11, 5, 5);

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
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
                if (state == 0) {
                    if (lightson) {
                        c1 = QColor(Qt::red);
                        c2 = QColor(Qt::red);
                    }
                    else {
                        c1 = QColor(Qt::darkGray);
                        c2 = QColor(Qt::darkGray);
                    }
                    c3 = QColor(Qt::darkGray);
                    c4 = QColor(Qt::darkGray);
                }
                else if (state == 1) {
                    if (lightson) {
                        c1 = QColor(Qt::red);
                        c4 = QColor(Qt::red);
                    }
                    else {
                        c1 = QColor(Qt::darkGray);
                        c4 = QColor(Qt::darkGray);
                    }
                    c2 = QColor(Qt::darkGray);
                    c3 = QColor(Qt::darkGray);
                }
                else if (state == 2) {
                    c1 = QColor(Qt::darkGray);
                    c2 = QColor(Qt::darkGray);
                    if (lightson) {
                        c3 = QColor(Qt::red);
                        c4 = QColor(Qt::red);
                    }
                    else {
                        c3 = QColor(Qt::darkGray);
                        c4 = QColor(Qt::darkGray);
                    }
                }
                // error indication
                else {
                    c1 = QColor(Qt::red);
                    c2 = QColor(Qt::red);
                    c3 = QColor(Qt::red);
                    c4 = QColor(Qt::red);
                }
            }
            else {
                if (state == 0) {
                    if (!lightson && ((lastdir == 0 && newdir == 2) ||
                            (lastdir == 2 && newdir == 0)))
                        c1 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c1 = QColor(255, 225, 0);
                        else 
                            c1 = QColor(Qt::darkGray);
                    }
                    
                    if (!lightson && ((lastdir != 0 && newdir == 0) ||
                            (lastdir == 0 && newdir != 0)))
                        c2 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c2 = QColor(255, 225, 0);
                        else 
                            c2 = QColor(Qt::darkGray);
                    }
                    
                    c3 = QColor(Qt::darkGray);
                    c4 = QColor(Qt::darkGray);
                }
                else if (state == 1) {
                    if (!lightson && ((lastdir == 1 && newdir == 2) ||
                        (lastdir == 2 && newdir == 1)))
                        c1 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c1 = QColor(255, 225, 0);
                        else 
                            c1 = QColor(Qt::darkGray);
                    }
                    
                    c2 = QColor(Qt::darkGray);
                    c3 = QColor(Qt::darkGray);
                    
                    if (!lightson && ((lastdir == 1 && newdir == 0) ||
                                (lastdir == 0 && newdir == 1)))
                        c4 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c4 = QColor(255, 225, 0);
                        else 
                            c4 = QColor(Qt::darkGray);
                    }
                }
                else if (state == 2) {
                    c1 = QColor(Qt::darkGray);
                    c2 = QColor(Qt::darkGray);
                    
                    if (!lightson && ((lastdir == 2 && newdir != 2) ||
                                (lastdir != 2 && newdir == 2)))
                        c3 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c3 = QColor(255, 225, 0);
                        else 
                            c3 = QColor(Qt::darkGray);
                    }
                    
                    if (!lightson && ((lastdir == 2 && newdir == 0) ||
                                (lastdir == 0 && newdir == 2)))
                        c4 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c4 = QColor(255, 225, 0);
                        else 
                            c4 = QColor(Qt::darkGray);
                    }
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
            p.setPen(QPen(Qt::black, 1));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));
        
        // paint text label
        p.setPen(Qt::black);
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopLeft(QPoint(2, 2));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // single slip switch right position 1
    else if (classid == siciSr1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, h / 2, w, h / 2);
        p.drawLine(0, 0, w, h - 1);
        
        // paint drive symbol
        p.setPen(QPen(Qt::black, 1));
        p.drawLine(w / 2 + 7, h / 2 - 5, w / 2 + 2, h / 2 - 5);
        p.drawLine(w / 2 + 2, h / 2 - 5, w / 2 - 3, h / 2 - 8);

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(3 * w/ 4 - 4, 6, 5, 5);

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
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
                if (state == 0) {
                    if (lightson) {
                        c1 = QColor(Qt::red);
                        c2 = QColor(Qt::red);
                    }
                    else {
                        c1 = QColor(Qt::darkGray);
                        c2 = QColor(Qt::darkGray);
                    }
                    c3 = QColor(Qt::darkGray);
                    c4 = QColor(Qt::darkGray);
                }
                else if (state == 1) {
                    if (lightson) {
                        c1 = QColor(Qt::red);
                        c4 = QColor(Qt::red);
                    }
                    else {
                        c1 = QColor(Qt::darkGray);
                        c4 = QColor(Qt::darkGray);
                    }
                    c2 = QColor(Qt::darkGray);
                    c3 = QColor(Qt::darkGray);
                }
                else if (state == 2) {
                    c1 = QColor(Qt::darkGray);
                    c2 = QColor(Qt::darkGray);
                    if (lightson) {
                        c3 = QColor(Qt::red);
                        c4 = QColor(Qt::red);
                    }
                    else {
                        c3 = QColor(Qt::darkGray);
                        c4 = QColor(Qt::darkGray);
                    }
                }
                // error indication
                else {
                    c1 = QColor(Qt::red);
                    c2 = QColor(Qt::red);
                    c3 = QColor(Qt::red);
                    c4 = QColor(Qt::red);
                }
            }
            else {
                if (state == 0) {
                    if (!lightson && ((lastdir == 0 && newdir == 2) ||
                            (lastdir == 2 && newdir == 0)))
                        c1 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c1 = QColor(255, 225, 0);
                        else 
                            c1 = QColor(Qt::darkGray);
                    }
                    
                    if (!lightson && ((lastdir == 0 && newdir != 0) ||
                            (lastdir != 0 && newdir == 0)))
                        c2 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c2 = QColor(255, 225, 0);
                        else 
                            c2 = QColor(Qt::darkGray);
                    }
                    
                    c3 = QColor(Qt::darkGray);
                    c4 = QColor(Qt::darkGray);
                }
                else if (state == 1) {
                    if (!lightson && ((lastdir == 1 && newdir == 2) ||
                        (lastdir == 2 && newdir == 1)))
                        c1 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c1 = QColor(255, 225, 0);
                        else 
                            c1 = QColor(Qt::darkGray);
                    }
                    
                    c2 = QColor(Qt::darkGray);
                    c3 = QColor(Qt::darkGray);
                    
                    if (!lightson && ((lastdir == 1 && newdir == 0) ||
                                (lastdir == 0 && newdir == 1)))
                        c4 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c4 = QColor(255, 225, 0);
                        else 
                            c4 = QColor(Qt::darkGray);
                    }
                }
                else if (state == 2) {
                    c1 = QColor(Qt::darkGray);
                    c2 = QColor(Qt::darkGray);
                    
                    if (!lightson && ((lastdir == 2 && newdir != 2) ||
                                (lastdir != 2 && newdir == 2)))
                        c3 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c3 = QColor(255, 225, 0);
                        else 
                            c3 = QColor(Qt::darkGray);
                    }
                    
                    if (!lightson && ((lastdir == 2 && newdir == 0) ||
                                (lastdir == 0 && newdir == 2)))
                        c4 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c4 = QColor(255, 225, 0);
                        else 
                            c4 = QColor(Qt::darkGray);
                    }
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

            p.setPen(QPen(Qt::black, 1));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));
        
        // paint text label
        p.setPen(Qt::black);
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomLeft(QPoint(2, h - 3));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // single slip switch right position 3
    else if (classid == siciSr3) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, h / 2, w, h / 2);
        p.drawLine(0, 0, w, h - 1);
        
        // paint drive symbol
        p.setPen(QPen(Qt::black, 1));
        p.drawLine(w / 2 - 1, h / 2 + 5, w / 2 - 7, h / 2 + 5);
        p.drawLine(w / 2 - 3, h / 2 + 5, w / 2 + 2, h / 2 + 7);

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 4, h - 11, 5, 5);

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
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
                if (state == 0) {
                    if (lightson) {
                        c1 = QColor(Qt::red);
                        c2 = QColor(Qt::red);
                    }
                    else {
                        c1 = QColor(Qt::darkGray);
                        c2 = QColor(Qt::darkGray);
                    }
                    c3 = QColor(Qt::darkGray);
                    c4 = QColor(Qt::darkGray);
                }
                else if (state == 1) {
                    if (lightson) {
                        c1 = QColor(Qt::red);
                        c4 = QColor(Qt::red);
                    }
                    else {
                        c1 = QColor(Qt::darkGray);
                        c4 = QColor(Qt::darkGray);
                    }
                    c2 = QColor(Qt::darkGray);
                    c3 = QColor(Qt::darkGray);
                }
                else if (state == 2) {
                    c1 = QColor(Qt::darkGray);
                    c2 = QColor(Qt::darkGray);
                    if (lightson) {
                        c3 = QColor(Qt::red);
                        c4 = QColor(Qt::red);
                    }
                    else {
                        c3 = QColor(Qt::darkGray);
                        c4 = QColor(Qt::darkGray);
                    }
                }
                // error indication
                else {
                    c1 = QColor(Qt::red);
                    c2 = QColor(Qt::red);
                    c3 = QColor(Qt::red);
                    c4 = QColor(Qt::red);
                }
            }
            else {
                if (state == 0) {
                    if (!lightson && ((lastdir == 0 && newdir == 2) ||
                            (lastdir == 2 && newdir == 0)))
                        c1 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c1 = QColor(255, 225, 0);
                        else 
                            c1 = QColor(Qt::darkGray);
                    }
                    
                    if (!lightson && ((lastdir == 0 && newdir != 0) ||
                            (lastdir != 0 && newdir == 0)))
                        c2 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c2 = QColor(255, 225, 0);
                        else 
                            c2 = QColor(Qt::darkGray);
                    }
                    
                    c3 = QColor(Qt::darkGray);
                    c4 = QColor(Qt::darkGray);
                }
                else if (state == 1) {
                    if (!lightson && ((lastdir == 1 && newdir == 2) ||
                        (lastdir == 2 && newdir == 1)))
                        c1 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c1 = QColor(255, 225, 0);
                        else 
                            c1 = QColor(Qt::darkGray);
                    }
                    
                    c2 = QColor(Qt::darkGray);
                    c3 = QColor(Qt::darkGray);
                    
                    if (!lightson && ((lastdir == 1 && newdir == 0) ||
                                (lastdir == 0 && newdir == 1)))
                        c4 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c4 = QColor(255, 225, 0);
                        else 
                            c4 = QColor(Qt::darkGray);
                    }
                }
                else if (state == 2) {
                    c1 = QColor(Qt::darkGray);
                    c2 = QColor(Qt::darkGray);
                    
                    if (!lightson && ((lastdir == 2 && newdir != 2) ||
                                (lastdir != 2 && newdir == 2)))
                        c3 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c3 = QColor(255, 225, 0);
                        else 
                            c3 = QColor(Qt::darkGray);
                    }
                    
                    if (!lightson && ((lastdir == 2 && newdir == 0) ||
                                (lastdir == 0 && newdir == 2)))
                        c4 = QColor(Qt::darkGray);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                            c4 = QColor(255, 225, 0);
                        else 
                            c4 = QColor(Qt::darkGray);
                    }
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

            p.setPen(QPen(Qt::black, 1));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));
        
        // paint text label
        p.setPen(Qt::black);
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopRight(QPoint(w - 3, 2));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // double slip switch left
    else if (classid == siciDl1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, h / 2, w, h / 2);
        p.drawLine(0, h - 1, w, 0);
        
        // paint drive symbol
        p.setPen(QPen(Qt::black, 1));
        p.drawLine(w / 2 + 1, h / 2 + 5, w / 2 + 7, h / 2 + 5);
        p.drawLine(w / 2 + 2, h / 2 + 5, w / 2 - 3, h / 2 + 7);
        p.drawLine(w / 2 - 7, h / 2 - 5, w / 2 - 2, h / 2 - 5);
        p.drawLine(w / 2 - 2, h / 2 - 5, w / 2 + 3, h / 2 - 8);

        // paint lock lights TODO: lock only one
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 + 4, h - 11, 5, 5);
        p.drawEllipse(w / 4, 6, 5, 5);

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
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
            if (state == 0) {
                if (!lightson && (
                            (lastdir == 0 && newdir >= 2)
                            ||
                            (lastdir >= 2 && newdir == 0)
                            )
                   )
                    c1 = QColor(Qt::darkGray);
                else {
                    if (occupied)
                        c1 = QColor(Qt::red);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed
                                || (lockCounter > 0))
                            c1 = QColor(255, 225, 0);
                        else 
                            c1 = QColor(Qt::darkGray);
                    }
                }

                if (!lightson && (
                            ((lastdir == 1 || lastdir == 2) && newdir == 0)
                            ||
                            (lastdir == 0 && (newdir == 1 || newdir == 2))
                            )
                   )
                    c2 = QColor(Qt::darkGray);
                else {
                    if (occupied)
                        c2 = QColor(Qt::red);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed
                                || (lockCounter > 0))
                            c2 = QColor(255, 225, 0);
                        else 
                            c2 = QColor(Qt::darkGray);
                    }
                }

                c3 = QColor(Qt::darkGray);
                c4 = QColor(Qt::darkGray);
            }
            else if (state == 1) {
                if (!lightson && (
                            (lastdir == 1 && (newdir == 2 || newdir == 3))
                            ||
                            ((lastdir == 2 || lastdir == 3) && newdir == 1)
                            )
                   )
                    c1 = QColor(Qt::darkGray);
                else {
                    if (occupied)
                        c1 = QColor(Qt::red);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed
                                || (lockCounter > 0))
                            c1 = QColor(255, 225, 0);
                        else 
                            c1 = QColor(Qt::darkGray);
                    }
                }

                c2 = QColor(Qt::darkGray);
                c3 = QColor(Qt::darkGray);

                if (!lightson && (
                            (lastdir == 1 && (newdir == 0 || newdir == 3))
                            ||
                            ((lastdir == 0 || lastdir == 3) && newdir == 1)
                            )
                   )
                    c4 = QColor(Qt::darkGray);
                else {
                    if (occupied)
                        c4 = QColor(Qt::red);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed
                                || (lockCounter > 0))
                            c4 = QColor(255, 225, 0);
                        else 
                            c4 = QColor(Qt::darkGray);
                    }
                }
            }
            else if (state == 2) {
                c1 = QColor(Qt::darkGray);
                c2 = QColor(Qt::darkGray);

                if (!lightson && (
                            (lastdir == 2 && (newdir == 0 || newdir == 1))
                            ||
                            ((lastdir == 0 || lastdir == 1) && newdir == 2)
                            )
                   )
                    c3 = QColor(Qt::darkGray);
                else {
                    if (occupied)
                        c3 = QColor(Qt::red);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed
                                || (lockCounter > 0))
                            c3 = QColor(255, 225, 0);
                        else 
                            c3 = QColor(Qt::darkGray);
                    }
                }

                if (!lightson && (
                            (lastdir == 2 && (newdir == 0 || newdir == 3))
                            ||
                            ((lastdir == 0 || lastdir == 3) && newdir == 2)
                            )
                   )
                    c4 = QColor(Qt::darkGray);
                else {
                    if (occupied)
                        c4 = QColor(Qt::red);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed
                                || (lockCounter > 0))
                            c4 = QColor(255, 225, 0);
                        else 
                            c4 = QColor(Qt::darkGray);
                    }
                }
            }
            else if (state == 3) {
                c1 = QColor(Qt::darkGray);

                if (!lightson && (
                            (lastdir == 3 && (newdir == 1 || newdir == 2))
                            ||
                            ((lastdir == 1 || lastdir == 2) && newdir == 3)
                            )
                   )
                    c2 = QColor(Qt::darkGray);
                else {
                    if (occupied)
                        c2 = QColor(Qt::red);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed
                                || (lockCounter > 0))
                            c2 = QColor(255, 225, 0);
                        else 
                            c2 = QColor(Qt::darkGray);
                    }
                }

                if (!lightson && (
                            (lastdir == 3 && (newdir == 0 || newdir == 1))
                            ||
                            ((lastdir == 0 || lastdir == 1) && newdir == 3)
                            )
                   )
                    c3 = QColor(Qt::darkGray);
                else {
                    if (occupied)
                        c3 = QColor(Qt::red);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed
                                || (lockCounter > 0))
                            c3 = QColor(255, 225, 0);
                        else 
                            c3 = QColor(Qt::darkGray);
                    }
                }

                c4 = QColor(Qt::darkGray);
            }
            // error indication
            else {
                c1 = QColor(255, 225, 0);
                c2 = QColor(255, 225, 0);
                c3 = QColor(255, 225, 0);
                c4 = QColor(255, 225, 0);
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

            p.setPen(QPen(Qt::black, 1));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));
        
        // paint text label
        p.setPen(Qt::black);
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);
            br.moveBottomRight(QPoint(w - 3, h - 3));
            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // double slip switch right
    else if (classid == siciDr1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, h / 2, w, h / 2);
        p.drawLine(0, 0, w, h - 1);
        
        // paint drive symbol
        p.setPen(QPen(Qt::black, 1));
        p.drawLine(w / 2 - 1, h / 2 + 5, w / 2 - 7, h / 2 + 5);
        p.drawLine(w / 2 - 3, h / 2 + 5, w / 2 + 2, h / 2 + 7);
        p.drawLine(w / 2 + 7, h / 2 - 5, w / 2 + 2, h / 2 - 5);
        p.drawLine(w / 2 + 2, h / 2 - 5, w / 2 - 3, h / 2 - 8);

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 8, h - 11, 5, 5);
        p.drawEllipse(3 * w/ 4 - 2 - 2  , 6, 5, 5);

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(SANGLE);
            for (int i = 0; i < 7; ++i)
                p.fillRect(-23 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
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
            if (state == 0) {
                if (!lightson && (
                            (lastdir == 0 && newdir >= 2)
                            ||
                            (lastdir >= 2 && newdir == 0)
                            )
                   )
                    c1 = QColor(Qt::darkGray);
                else {
                    if (occupied)
                        c1 = QColor(Qt::red);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed
                                || (lockCounter > 0))
                            c1 = QColor(255, 225, 0);
                        else 
                            c1 = QColor(Qt::darkGray);
                    }
                }

                if (!lightson && (
                            ((lastdir == 1 || lastdir == 2) && newdir == 0)
                            ||
                            (lastdir == 0 && (newdir == 1 || newdir == 2))
                            )
                   )
                    c2 = QColor(Qt::darkGray);
                else {
                    if (occupied)
                        c2 = QColor(Qt::red);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed
                                || (lockCounter > 0))
                            c2 = QColor(255, 225, 0);
                        else 
                            c2 = QColor(Qt::darkGray);
                    }
                }

                c3 = QColor(Qt::darkGray);
                c4 = QColor(Qt::darkGray);
            }
            else if (state == 1) {
                if (!lightson && (
                            (lastdir == 1 && (newdir == 2 || newdir == 3))
                            ||
                            ((lastdir == 2 || lastdir == 3) && newdir == 1)
                            )
                   )
                    c1 = QColor(Qt::darkGray);
                else {
                    if (occupied)
                        c1 = QColor(Qt::red);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed
                                || (lockCounter > 0))
                            c1 = QColor(255, 225, 0);
                        else 
                            c1 = QColor(Qt::darkGray);
                    }
                }

                c2 = QColor(Qt::darkGray);
                c3 = QColor(Qt::darkGray);

                if (!lightson && (
                            (lastdir == 1 && (newdir == 0 || newdir == 3))
                            ||
                            ((lastdir == 0 || lastdir == 3) && newdir == 1)
                            )
                   )
                    c4 = QColor(Qt::darkGray);
                else {
                    if (occupied)
                        c4 = QColor(Qt::red);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed
                                || (lockCounter > 0))
                            c4 = QColor(255, 225, 0);
                        else 
                            c4 = QColor(Qt::darkGray);
                    }
                }
            }
            else if (state == 2) {
                c1 = QColor(Qt::darkGray);
                c2 = QColor(Qt::darkGray);

                if (!lightson && (
                            (lastdir == 2 && (newdir == 0 || newdir == 1))
                            ||
                            ((lastdir == 0 || lastdir == 1) && newdir == 2)
                            )
                   )
                    c3 = QColor(Qt::darkGray);
                else {
                    if (occupied)
                        c3 = QColor(Qt::red);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed
                                || (lockCounter > 0))
                            c3 = QColor(255, 225, 0);
                        else 
                            c3 = QColor(Qt::darkGray);
                    }
                }

                if (!lightson && (
                            (lastdir == 2 && (newdir == 0 || newdir == 3))
                            ||
                            ((lastdir == 0 || lastdir == 3) && newdir == 2)
                            )
                   )
                    c4 = QColor(Qt::darkGray);
                else {
                    if (occupied)
                        c4 = QColor(Qt::red);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed
                                || (lockCounter > 0))
                            c4 = QColor(255, 225, 0);
                        else 
                            c4 = QColor(Qt::darkGray);
                    }
                }
            }
            else if (state == 3) {
                c1 = QColor(Qt::darkGray);

                if (!lightson && (
                            (lastdir == 3 && (newdir == 1 || newdir == 2))
                            ||
                            ((lastdir == 1 || lastdir == 2) && newdir == 3)
                            )
                   )
                    c2 = QColor(Qt::darkGray);
                else {
                    if (occupied)
                        c2 = QColor(Qt::red);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed
                                || (lockCounter > 0))
                            c2 = QColor(255, 225, 0);
                        else 
                            c2 = QColor(Qt::darkGray);
                    }
                }

                if (!lightson && (
                            (lastdir == 3 && (newdir == 0 || newdir == 1))
                            ||
                            ((lastdir == 0 || lastdir == 1) && newdir == 3)
                            )
                   )
                    c3 = QColor(Qt::darkGray);
                else {
                    if (occupied)
                        c3 = QColor(Qt::red);
                    else {
                        if ((blinkcounter > 0) || tablelight || routed
                                || (lockCounter > 0))
                            c3 = QColor(255, 225, 0);
                        else 
                            c3 = QColor(Qt::darkGray);
                    }
                }

                c4 = QColor(Qt::darkGray);
            }
            // error indication
            else {
                c1 = QColor(255, 225, 0);
                c2 = QColor(255, 225, 0);
                c3 = QColor(255, 225, 0);
                c4 = QColor(255, 225, 0);
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

            p.setPen(QPen(Qt::black, 1));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));
        
        // paint text label
        p.setPen(Qt::black);
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);
            br.moveBottomLeft(QPoint(2, h - 3));
            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }
    
    // right track with normal route button
    else if (classid == siciZt1 || classid == siciRt1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.fillRect(w / 3, h / 2 - 1, w / 3, 3, QBrush(c));
        }

        // paint track button
        if (classid == siciZt1)
            p.drawPixmap(5 * w / 6 - 4 , h / 2 - 3, 
                    QPixmap(button_red_xpm));
        else
            p.drawPixmap(5 * w / 6 - 4 , h / 2 - 3, 
                    QPixmap(button_gray_xpm));

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(w / 2 + br.width()/2, h - 3));

            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }

    // left track with normal route button
    else if (classid == siciZt3 || classid == siciRt3) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.fillRect(w / 3, h / 2 - 1, w / 3, 3, QBrush(c));
        }

        // paint track button
            if (classid == siciZt3)
                p.drawPixmap(w / 6  - 4, h / 2 - 3,
                        QPixmap(button_red_xpm));
            else
                p.drawPixmap(w / 6  - 4, h / 2 - 3,
                        QPixmap(button_gray_xpm));

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopLeft(QPoint(w / 2 - br.width()/2, 2));

            p.fillRect(br, QBrush(Qt::white));
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }

    // right shunting signals SS, SSH, SSS
    else if (classid == siciSs1 || classid == siciSh1 ||
            classid == siciSd1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
        }

        // paint track button
        int xpos1 = w / 6  - 4;
        int xpos2 = 5 * w / 6 - 4;

        if (classid == siciSd1) {
            p.drawPixmap(xpos1, h / 2 - 3, QPixmap(button_gray_xpm));
            p.drawPixmap(xpos2, h / 2 - 3, QPixmap(button_gray_xpm));
        }

        xpos1 = xpos2;

        if (classid == siciSh1)
            p.drawPixmap(xpos1, h / 2 - 3, QPixmap(button_red_xpm));

        else if (classid == siciSs1) {
            p.drawPixmap(xpos1, h / 2 - 3, QPixmap(button_gray_xpm));
        }

        // paint signal icon
        p.setBrush(Qt::black);

        p.save();
        p.translate(w - 1, h - 1);
        p.rotate(180.0);
        p.drawRect(5, 5, 15, 5);
        p.drawLine(6, 4, 18, 4);
        p.drawLine(6, 10, 18, 10);
        p.drawLine(5 + 15, 7, 6 + 16, 7);
        p.drawLine(7 + 16, 5, 7 + 16, 9);
        p.drawLine(7 + 17, 5, 7 + 17, 9);

        // paint signal light
        // SH0
        if (state == 0) {
            p.setPen(QPen(Qt::red));
            p.drawLine(7, 5, 7, 9);
            p.drawLine(8, 5, 8, 9);
        }
        // SH1
        else {
            p.setPen(Qt::yellow);
            p.drawLine(14, 5, 18, 9);
            p.drawLine(13, 5, 17, 9);
        }
        p.setPen(QPen(Qt::black));
        
        p.restore();

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 4, h - 10, 5, 5);

        // paint FfM
        if (ffm) {
            p.setBrush(ffmactive ? Qt::yellow : Qt::darkGray);
            p.drawRect(w - 11, 4, 6, 6);
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopLeft(QPoint(w / 2 - br.width()/2, 2));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);
            
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }

    // left shunting signals SS, SSH, SSS
    else if (classid == siciSs3 || classid == siciSh3 ||
            classid == siciSd3) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.fillRect(w / 3 + 1, h / 2 - 1, w / 3, 3, QBrush(c));
        }

        // paint track button
        int xpos1 = w / 6  - 4;
        int xpos2 = 5 * w / 6 - 4;

        if (classid == siciSd3) {
            p.drawPixmap(xpos1, h / 2 - 3, QPixmap(button_gray_xpm));
            p.drawPixmap(xpos2, h / 2 - 3, QPixmap(button_gray_xpm));
        }

        if (classid == siciSh3)
            p.drawPixmap(xpos1, h / 2 - 3, QPixmap(button_red_xpm));

        else if (classid == siciSs3) {
            p.drawPixmap(xpos1, h / 2 - 3, QPixmap(button_gray_xpm));
        }

        // paint signal icon
        p.setBrush(Qt::black);

        p.drawRect(5, 5, 15, 5);
        p.drawLine(6, 4, 18, 4);
        p.drawLine(6, 10, 18, 10);
        p.drawLine(5 + 15, 7, 6 + 16, 7);
        p.drawLine(7 + 16, 5, 7 + 16, 9);
        p.drawLine(7 + 17, 5, 7 + 17, 9);

        // paint signal light
        // SH0
        if (state == 0) {
            p.setPen(QPen(Qt::red));
            p.drawLine(7, 5, 7, 9);
            p.drawLine(8, 5, 8, 9);
        }
        // SH1
        else {
            p.setPen(Qt::yellow);
            p.drawLine(14, 5, 18, 9);
            p.drawLine(13, 5, 17, 9);
        }
        p.setPen(QPen(Qt::black));
        

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 1, 5, 5, 5);

        // paint FfM
        if (ffm) {
            p.setBrush(ffmactive ? Qt::yellow : Qt::darkGray);
            p.drawRect(5, h - 10, 6, 6);
        }

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(w / 2 + br.width()/2, h - 3));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);
            
            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }

        setPaletteBackgroundPixmap(background);
    }

    // switch left top (+ old left bottom)
    else if (classid == siciTl1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        p.save();
        p.translate(w / 2, h / 2);

        p.rotate(-SANGLE);

        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, 0, w / 2 + 5, 0);
        p.restore();

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(Qt::lightGray));
            // short track
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-SANGLE);

            for (int i = 0; i < 3; ++i)
                p.fillRect(6 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.restore();

        }
        else {
            // first light
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(5, h / 2 , 19,  h / 2);

            // second light
            if (state == 0 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(w - 5, h / 2 , w - 19,  h / 2);

            // third light
            if (state == 1 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-SANGLE);

            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 2, h - 11, 5, 5);

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomLeft(QPoint(2, h - 3));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }
        setPaletteBackgroundPixmap(background);
    }

    // switch left bottom
    else if (classid == siciTl3) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        p.save();
        p.translate(w / 2, h / 2);

        p.rotate(WANGLE);

        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, 0, w / 2 + 5, 0);
        p.restore();

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(Qt::lightGray));
            // short track
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(WANGLE);

            for (int i = 0; i < 3; ++i)
                p.fillRect(6 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.restore();

        }
        else {
            // first light
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(w - 5, h / 2 , w - 19,  h / 2);

            // second light
            if (state == 0 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(5, h / 2 , 19,  h / 2);

            // third light
            if (state == 1 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(WANGLE);

            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 2, 6, 5, 5);

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopRight(QPoint(w - 3, 2));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }
        setPaletteBackgroundPixmap(background);
    }

    // switch right top
    else if (classid == siciTr3) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        p.save();
        p.translate(w / 2, h / 2);

        p.rotate(-WANGLE);

        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, 0, w / 2 + 5, 0);
        p.restore();

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(Qt::lightGray));
            // short track
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-WANGLE);

            for (int i = 0; i < 3; ++i)
                p.fillRect(6 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.restore();

        }
        else {
            // first light
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(w - 5, h / 2 , w - 19,  h / 2);

            // second light
            if (state == 0 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(5, h / 2 , 19,  h / 2);

            // third light
            if (state == 1 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-WANGLE);

            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 2, h - 11, 5, 5);

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(w - 3, h - 3));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }
        setPaletteBackgroundPixmap(background);
    }

    // switch right bottom (+ old right top)
    else if (classid == siciTr1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        p.save();
        p.translate(w / 2, h / 2);

        p.rotate(SANGLE);

        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, 0, w / 2 + 5, 0);
        p.restore();

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5, QBrush(Qt::lightGray));
            // short track
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(SANGLE);

            for (int i = 0; i < 3; ++i)
                p.fillRect(6 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.restore();

        }
        else {
            // first light
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(5, h / 2 , 19,  h / 2);

            // second light
            if (state == 0 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(w - 5, h / 2 , w - 19,  h / 2);

            // third light
            if (state == 1 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(SANGLE);

            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 2, 6, 5, 5);

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopLeft(QPoint(2, 2));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }
        setPaletteBackgroundPixmap(background);
    }

    // diagonal turnout left top
    else if (classid == siciIl3) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w / 2, 7, QBrush(Qt::black));
 
        p.save();
        p.translate(w / 2, h / 2);

        p.rotate(-WANGLE);

        p.setPen(QPen(Qt::black, 7));
        p.drawLine(-w / 2 - 5, 0, w / 2 + 5, 0);
        p.restore();

        // paint track lights
        if (trackindicatoroff) {
            // short track
            for (int i = 0; i < 3; ++i)
                p.fillRect(4 + 7 * i, h / 2-2, 5, 5, QBrush(Qt::lightGray));

            // long track
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-WANGLE);

            for (int i = 0; i < 3; ++i)
                p.fillRect(7 + 7 * i, - 2, 5, 5, QBrush(Qt::lightGray));

            for (int i = 0; i < 3; ++i)
                p.fillRect(-11 - 7 * i, - 2, 5, 5, QBrush(Qt::lightGray));

            p.restore();

        }
        else {
            QColor c;
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(SANGLE);

            // first light
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(10, 0, 24, 0);

            // second light
            if (state == 0 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(-10, 0, -24, 0);

            p.restore();
                
            // third light
            if (state == 1 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(5, h / 2, 5 + 14, h / 2);

            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 + 5, 8, 5, 5);

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomLeft(QPoint(2, h - 3));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }
        setPaletteBackgroundPixmap(background);
    }

    // diagonal turnout left bottom (+ old top)
    else if (classid == siciIl1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(w / 2, h / 2 - 3, w, 7, QBrush(Qt::black));
 
        p.save();
        p.translate(w / 2, h / 2);

        p.rotate(SANGLE);

        p.setPen(QPen(Qt::black, 7));
        p.drawLine(-w / 2 - 5, 0, w / 2 + 5, 0);
        p.restore();

        // paint track lights
        if (trackindicatoroff) {
            // short track
            for (int i = 0; i < 3; ++i)
                p.fillRect(w / 2 + 5 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));

            // long track
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(SANGLE);

            for (int i = 0; i < 3; ++i)
                p.fillRect(7 + 7 * i, - 2, 5, 5, QBrush(Qt::lightGray));

            for (int i = 0; i < 3; ++i)
                p.fillRect(-11 - 7 * i, - 2, 5, 5, QBrush(Qt::lightGray));

            p.restore();

        }
        else {
            QColor c;
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(SANGLE);

            // first light
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(-10, 0, -24, 0);

            // second light
            if (state == 0 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(10, 0, 24, 0);

            p.restore();
                
            // third light
            if (state == 1 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(w - 5, h / 2, w - 5 - 14, h / 2);

            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 8, h - 14, 5, 5);

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopRight(QPoint(w - 3, 2));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }
        setPaletteBackgroundPixmap(background);
    }

    // diagonal turnout right top (+ old bottom)
    else if (classid == siciIr1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(w / 2, h / 2 - 3, w, 7, QBrush(Qt::black));
 
        p.save();
        p.translate(w / 2, h / 2);

        p.rotate(-SANGLE);

        p.setPen(QPen(Qt::black, 7));
        p.drawLine(-w / 2 - 5, 0, w / 2 + 5, 0);
        p.restore();

        // paint track lights
        if (trackindicatoroff) {
            // short track
            for (int i = 0; i < 3; ++i)
                p.fillRect(w / 2 + 5 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));

            // long track
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-SANGLE);

            for (int i = 0; i < 3; ++i)
                p.fillRect(7 + 7 * i, - 2, 5, 5, QBrush(Qt::lightGray));

            for (int i = 0; i < 3; ++i)
                p.fillRect(-11 - 7 * i, - 2, 5, 5, QBrush(Qt::lightGray));

            p.restore();

        }
        else {
            QColor c;
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-SANGLE);

            // first light
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(-10, 0, -24, 0);

            // second light
            if (state == 0 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(10, 0, 24, 0);

            p.restore();
                
            // third light
            if (state == 1 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(w - 5, h / 2, w - 5 - 14, h / 2);

            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 8, 8, 5, 5);

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(w - 3, h - 3));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }
        setPaletteBackgroundPixmap(background);
    }

    // diagonal turnout right bottom
    else if (classid == siciIr3) {
        QPainter p(&background);

        int w = background.width();
        int h = background.height();

        // paint track
        p.fillRect(0, h / 2 - 3, w / 2, 7, QBrush(Qt::black));

        p.save();
        p.translate(w / 2, h / 2);

        p.rotate(WANGLE);

        p.setPen(QPen(Qt::black, 7));
        p.drawLine(-w / 2 - 5, 0, w / 2 + 5, 0);
        p.restore();

        // paint track lights
        if (trackindicatoroff) {
            // short track
            for (int i = 0; i < 3; ++i)
                p.fillRect(4 + 7 * i, h / 2-2, 5, 5, QBrush(Qt::lightGray));

            // long track
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(WANGLE);

            for (int i = 0; i < 3; ++i)
                p.fillRect(7 + 7 * i, - 2, 5, 5, QBrush(Qt::lightGray));

            for (int i = 0; i < 3; ++i)
                p.fillRect(-11 - 7 * i, - 2, 5, 5, QBrush(Qt::lightGray));

            p.restore();

        }
        else {
            QColor c;
            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-SANGLE);

            // first light
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(10, 0, 24, 0);

            // second light
            if (state == 0 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(-10, 0, -24, 0);

            p.restore();

            // third light
            if (state == 1 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(5, h / 2, 5 + 14, h / 2);

            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 + 5, h - 14, 5, 5);

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopLeft(QPoint(2, 2));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }
        setPaletteBackgroundPixmap(background);
    }

    // y-turnout right (0: -\  1: -/)
    else if (classid == siciSy1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.save();
        p.translate(w / 2, h / 2);
        p.setPen(QPen(Qt::black, 7));

        p.drawLine(-w / 2, 0, 0, 0);
        p.rotate(-SANGLE);
        p.drawLine(0, 0, w / 2 + 5, 0);
        p.rotate(SANGLE * 2);
        p.drawLine(0, 0, w / 2 + 5, 0);

        p.restore();

        // paint track lights
        if (trackindicatoroff) {
            // short track
            p.save();
            p.translate(w / 2, h / 2);

            for (int i = 0; i < 3; ++i)
                p.fillRect(-11 - 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.rotate(-SANGLE);
            for (int i = 0; i < 3; ++i)
                p.fillRect(7 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.rotate(SANGLE * 2);
            for (int i = 0; i < 3; ++i)
                p.fillRect(7 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.restore();

        }
        else {
            // first light
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(5, h / 2 , 19,  h / 2);

            // second light
            if (state == 0 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);
            
            p.rotate(SANGLE);

            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            // third light
            if (state == 1 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-SANGLE);

            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 4, h - 11, 5, 5);

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopLeft(QPoint(2, 2));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }
        setPaletteBackgroundPixmap(background);
    }

    // y-turnout left
    else if (classid == siciSy3) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.save();
        p.translate(w / 2, h / 2);
        p.setPen(QPen(Qt::black, 7));

        p.drawLine(0, 0, w / 2, 0);
        p.rotate(WANGLE);
        p.drawLine(0, 0, w / 2 + 5, 0);
        p.rotate(-WANGLE * 2);
        p.drawLine(0, 0, w / 2 + 5, 0);

        p.restore();

        // paint track lights
        if (trackindicatoroff) {
            // short track
            p.save();
            p.translate(w / 2, h / 2);

            for (int i = 0; i < 3; ++i)
                p.fillRect(6 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.rotate(WANGLE);
            for (int i = 0; i < 3; ++i)
                p.fillRect(7 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.rotate(-WANGLE * 2);
            for (int i = 0; i < 3; ++i)
                p.fillRect(7 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.restore();

        }
        else {
            // first light
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(w - 5, h / 2 , w - 19,  h / 2);

            // second light
            if (state == 0 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);
            
            p.rotate(-WANGLE);

            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            // third light
            if (state == 1 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);
            p.rotate(WANGLE);
            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

            p.drawEllipse(w / 2, 6, 5, 5);

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(w - 3, h - 3));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }
        setPaletteBackgroundPixmap(background);
    }

    // 3-way turnout right (+ old left)
    else if (classid == siciTw1) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        p.save();
        p.translate(w / 2, h / 2);
        p.setPen(QPen(Qt::black, 7));

        p.rotate(-SANGLE);
        p.drawLine(0, 0, w / 2 + 5, 0);
        p.rotate(SANGLE * 2);
        p.drawLine(0, 0, w / 2 + 5, 0);

        p.restore();

        // paint track lights (track indicator)
        if (trackindicatoroff) {
            p.save();
            p.translate(w / 2, h / 2);

            for (int i = 0; i < 3; ++i)
                p.fillRect(-11 - 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            for (int i = 0; i < 3; ++i)
                p.fillRect(6 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.rotate(-SANGLE);
            for (int i = 0; i < 3; ++i)
                p.fillRect(7 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.rotate(SANGLE * 2);
            for (int i = 0; i < 3; ++i)
                p.fillRect(7 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));

            p.restore();

        }
        else {
            // first light
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(5, h / 2 , 19,  h / 2);

            // second light
            if (state == 0 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(w - 5, h / 2 , w - 19,  h / 2);

            // third light
            if (state == 1 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-SANGLE);

            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            // fourth light
            if (state == 2 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(SANGLE);

            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2 - 4, h - 11, 5, 5);

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveTopLeft(QPoint(2, 2));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }
        setPaletteBackgroundPixmap(background);
    }

    // 3-way turnout left
    else if (classid == siciTw3) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // paint track
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));
        
        p.save();
        p.translate(w / 2, h / 2);
        p.setPen(QPen(Qt::black, 7));

        p.rotate(WANGLE);
        p.drawLine(0, 0, w / 2 + 5, 0);
        p.rotate(-WANGLE * 2);
        p.drawLine(0, 0, w / 2 + 5, 0);

        p.restore();

        // paint track lights (track indicator)
        if (trackindicatoroff) {
            p.save();
            p.translate(w / 2, h / 2);

            for (int i = 0; i < 3; ++i)
                p.fillRect(-11 - 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            for (int i = 0; i < 3; ++i)
                p.fillRect(6 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.rotate(WANGLE);
            for (int i = 0; i < 3; ++i)
                p.fillRect(7 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));
            p.rotate(-WANGLE * 2);
            for (int i = 0; i < 3; ++i)
                p.fillRect(7 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));

            p.restore();

        }
        else {
            // first light
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(w - 5, h / 2 , w - 19,  h / 2);

            // second light
            if (state == 0 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.drawLine(5, h / 2 , 19,  h / 2);

            // third light
            if (state == 1 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(WANGLE);

            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            // fourth light
            if (state == 2 && lightson)
                if (occupied)
                    c = QColor(Qt::red);
                else {
                    if ((blinkcounter > 0) || tablelight || routed || (lockCounter > 0))
                        c = QColor(255, 225, 0);
                    else 
                        c = QColor(Qt::darkGray);
                }
            else
                c = QColor(Qt::darkGray);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));

            p.save();
            p.translate(w / 2, h / 2);

            p.rotate(-WANGLE);

            p.drawLine(11, 0, 11 + 14, 0);
            p.restore();

            p.setPen(QPen(Qt::black));
        }

        // paint track button
        p.drawPixmap(w / 2 - 4, h / 2 - 3, QPixmap(button_black_xpm));

        // paint lock light
        if (lockCounter == 0)
            p.setBrush(Qt::darkGray);
        else
            p.setBrush(QColor(255, 225, 0));

        p.drawEllipse(w / 2, 6, 5, 5);

        // paint text label
        if (sSoldText != "-1" && !sSoldText.isEmpty()) {
            QFont f(QApplication::font());
            f.setPointSize(QApplication::font().pointSize() - 3);
            p.setFont(f);
            QFontMetrics fm(f);
            QString s;

            if (pref.addresslabeling)
                s.setNum(address1);
            else
                s = sSoldText;

            QRect br = fm.boundingRect(s);
            br.setWidth(br.width() + 4);
            br.setHeight(br.height() + 2);

            br.moveBottomRight(QPoint(w - 3, h - 3));

            p.fillRect(br, QBrush(Qt::white));
            p.setBrush(Qt::white);
            br.setX(br.x() + 1);
            br.setY(br.y() + 1);
            br.setHeight(br.height() - 2);

            p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                    Qt::DontClip, s);
        }
        setPaletteBackgroundPixmap(background);
    }

    // house 1 (train station middle section)
    else if (classid == siciBuc) {
        QPainter p(&background);

        int w = background.width();
        int h = background.height();
        
        // paint house
        p.setBrush(QColor(192, 0 ,0));
        p.drawRect(0, 9, w, 17);
        p.drawLine(0, h / 2, w, h / 2);

        setPaletteBackgroundPixmap(background);
    }

    // house 2 (building left wing)
    else if (classid == siciBul) {
        QPainter p(&background);

        // paint house
        p.setBrush(QColor(192, 0 ,0));
        p.drawRect(26, 5, 25, 25);
        p.drawLine(26, 5, 50, 29);
        p.drawLine(26, 29, 50, 5);
        p.drawRect(50, 9, 6, 17);
        
        setPaletteBackgroundPixmap(background);
    }
   
    // building right wing
    else if (classid == siciBur) {
        QPainter p(&background);

        // paint house
        p.setBrush(QColor(192, 0 ,0));
        p.drawRect(4, 5, 25, 25);
        p.drawLine(4, 5, 28, 29);
        p.drawLine(4, 29, 28, 5);
        p.drawRect(0, 9, 5, 17);
        
        setPaletteBackgroundPixmap(background);
    }
   
    // straight track with horizontal tunnel (old left + right)
    else if (classid == siciTuh) {
        QPainter p(&background);

        int w = background.width();
        int h = background.height();
        int tracklen = w / 2 - 8;

        // paint track
        // vertical section
        p.fillRect(w / 2 - 3, 0, 7, h, QBrush(Qt::black));

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 4; ++i)
                p.fillRect(w / 2 - 2 , 4 + 7 * i, 5, 5,
                        QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(w / 2, h / 3, w / 2, 2 * h / 3);
            p.setPen(QPen(Qt::black));
        }

        // horizontal sections
        p.fillRect(0, h / 2 - 3, tracklen, 7, QBrush(Qt::black));
        p.fillRect(w - tracklen + 1, h / 2 - 3, tracklen -1, 7,
                QBrush(Qt::black));

        // translate origin to center of pixmap
        p.translate(w / 2 - 6, h / 2);
        
        // paint tunnel entry
        QPointArray tunnel = QPointArray(4);
        tunnel.putPoints(0, 4, -8, -14, 0, -6, 0, 6, -8, 14);
        p.setPen(QPen(Qt::darkGray, 2));
        p.drawPolyline(tunnel);
        p.translate(13, 0);
        p.rotate(180.0);
        p.drawPolyline(tunnel);

        setPaletteBackgroundPixmap(background);
    }

    // straight track with vertical tunnel
    else if (classid == siciTuv) {
        QPainter p(&background);

        int w = background.width();
        int h = background.height();
        int tracklen = h / 2 - 8;

        // paint track
        // horizontal section
        p.fillRect(0, h / 2 - 3, w, 7, QBrush(Qt::black));

        // paint track lights
        if (trackindicatoroff) {
            for (int i = 0; i < 7; ++i)
                p.fillRect(4 + 7 * i, h / 2 - 2, 5, 5,
                        QBrush(Qt::lightGray));
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(w / 3, h / 2, 2 * w / 3, h / 2);
            p.setPen(QPen(Qt::black));
        }

        // vertical sections
        p.fillRect(w / 2 - 3, 0, 7, tracklen, QBrush(Qt::black));
        p.fillRect(w / 2 - 3, h - tracklen, 7, tracklen, QBrush(Qt::black));
        
        // translate origin to center of pixmap
        p.translate(w / 2, h / 2);

        // paint tunnel entry
        QPointArray tunnel = QPointArray(4);
        tunnel.putPoints(0, 4, -8, -14, 0, -6, 0, 6, -8, 14);
        p.setPen(QPen(Qt::darkGray, 2));

        p.rotate(90.0);
        p.translate(-6, 0);
        p.drawPolyline(tunnel);
        p.translate(13, 0);
        p.rotate(180.0);
        p.drawPolyline(tunnel);

        setPaletteBackgroundPixmap(background);
    }

    // left diagonal track with tunnel
    else if (classid == siciTul) {
        QPainter p(&background);

        int w = background.width();
        int h = background.height();
        int tracklen = w / 2 + 5;

        // paint long diagonal track
        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, 0, w, h - 1);

        // paint track lights
        if (trackindicatoroff) {
            p.save();
            p.translate(w / 2, h / 2);
            p.rotate(SANGLE);

            for (int i = -3; i < 4; ++i)
                p.fillRect(-3 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));

            p.restore();
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed && (routedtrack & 1) == 1)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.save();
            p.translate(w / 2, h / 2);
            p.rotate(SANGLE);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(-w / 6, 0, w / 6, 0);

            p.restore();
        }

        // translate origin to center of pixmap
        p.translate(w / 2, h / 2);

        p.rotate(-SANGLE);
        
        // paint two short tracks
        p.drawLine(-tracklen, 0, -9, 0);
        p.drawLine(9, 0, tracklen, 0);
        
        // paint tunnel entry
        QPointArray tunnel = QPointArray(4);
        tunnel.putPoints(0, 4, -8, -14, 0, -6, 0, 6, -8, 14);
        p.setPen(QPen(Qt::darkGray, 2));
        p.rotate(SANGLE - WANGLE + 90.0);
        p.translate(-6, -4);
        p.drawPolyline(tunnel);

        p.translate(6, 4);
        p.rotate(180.0);
        p.translate(-6, -4);
        p.drawPolyline(tunnel);

        setPaletteBackgroundPixmap(background);
    }

    // right diagonal track with tunnel
    else if (classid == siciTur) {
        QPainter p(&background);

        int w = background.width();
        int h = background.height();
        int tracklen = w / 2 + 5;

        // paint long diagonal track
        p.setPen(QPen(Qt::black, 7));
        p.drawLine(0, h - 1, w, 0);

        // paint track lights
        if (trackindicatoroff) {
            p.save();
            p.translate(w / 2, h / 2);
            p.rotate(-SANGLE);

            for (int i = -3; i < 4; ++i)
                p.fillRect(-3 + 7 * i, -2, 5, 5, QBrush(Qt::lightGray));

            p.restore();
        }
        else {
            QColor c;
            if (occupied)
                c = QColor(Qt::red);
            else {
                if (routed && (routedtrack & 1) == 1)
                    c = QColor(255, 225, 0);
                else
                    c = QColor(Qt::darkGray);
            }
            p.save();
            p.translate(w / 2, h / 2);
            p.rotate(-SANGLE);

            p.setPen(QPen(c, 3, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
            p.drawLine(-w / 6, 0, w / 6, 0);

            p.restore();
        }

        // translate origin to center of pixmap
        p.translate(w / 2, h / 2);
        p.rotate(-WANGLE);
        
        // paint two short tracks
        p.drawLine(-tracklen, 0, -9, 0);
        p.drawLine(9, 0, tracklen, 0);
        
        // paint tunnel entry
        QPointArray tunnel = QPointArray(4);
        tunnel.putPoints(0, 4, -8, -14, 0, -6, 0, 6, -8, 14);
        p.setPen(QPen(Qt::darkGray, 2));
        p.rotate(WANGLE - SANGLE + 90.0);
        p.translate(-6, 4);
        p.drawPolyline(tunnel);

        p.translate(6, -4);
        p.rotate(180.0);
        p.translate(-6, 4);
        p.drawPolyline(tunnel);

        setPaletteBackgroundPixmap(background);
    }

    // track with right top or bottom loco shed (lokschuppen)
    else if (classid == siciLt1 || classid == siciLb3) {

        bool istop = (classid == siciLt1);
        int w = background.width();
        int h = background.height();
        
        QPainter p(&background);

        // translate origin to center of pixmap
        p.translate(w / 2, h / 2);

        int tracklen = w / 2 + 5;
        int shedwidth = h + 8;
        int shedxoffset = 9;
        int shedyoffset = 8;

        if (istop)
            p.rotate(-SANGLE);
        else
            p.rotate(WANGLE);
        
        // paint track
        p.fillRect(0, -3, -tracklen, 7, QBrush(Qt::black));
        
        // paint shed
        p.setBrush(QColor(192, 0 ,0));
        p.drawRect(0 - shedxoffset, -shedwidth / 2 + shedyoffset,
                w / 2, shedwidth);
        p.drawLine(w / 4 - shedxoffset, -shedwidth / 2 + shedyoffset,
                w / 4 - shedxoffset, shedwidth / 2 + shedyoffset);

        setPaletteBackgroundPixmap(background);
    }

    // track with left top or bottom loco shed (lokschuppen)
    else if (classid == siciLt3 || classid == siciLb1) {

        bool istop = (classid == siciLt3);
        int w = background.width();
        int h = background.height();
        
        QPainter p(&background);

        // translate origin to center of pixmap
        p.translate(w / 2, h / 2);

        int tracklen = w / 2 + 5;
        int shedwidth = h + 8;
        int shedxoffset = 9;
        int shedyoffset = -8;

        if (istop)
            p.rotate(-WANGLE);
        else
            p.rotate(SANGLE);
        
        // paint track
        p.fillRect(0, -3, -tracklen, 7, QBrush(Qt::black));
        
        // paint shed
        p.setBrush(QColor(192, 0 ,0));
        p.drawRect(0 - shedxoffset, -shedwidth / 2 + shedyoffset,
                w / 2, shedwidth);
        p.drawLine(w / 4 - shedxoffset, -shedwidth / 2 + shedyoffset,
                w / 4 - shedxoffset, shedwidth / 2 + shedyoffset);

        setPaletteBackgroundPixmap(background);
    }

    // track with right loco shed (+ old left)
    else if (classid == siciLs1) {
        QPainter p(&background);

        int w = background.width();
        int h = background.height();

        // translate origin to center of pixmap
        p.translate(w / 2, h / 2);

        int tracklen = w / 2;
        int shedwidth = h;
        int shedxoffset = 0;
        int shedyoffset = 0;

        // paint track
        p.fillRect(0, -3, -tracklen, 7, QBrush(Qt::black));
        
        // paint schuppen
        p.setBrush(QColor(192, 0 ,0));
        p.drawRect(0 - shedxoffset, -shedwidth / 2 + shedyoffset,
                w / 2, shedwidth);
        p.drawLine(w / 4 - shedxoffset, -shedwidth / 2 + shedyoffset,
                w / 4 - shedxoffset, shedwidth / 2 + shedyoffset);

        setPaletteBackgroundPixmap(background);
    }

    // track with left middle loco shed
    else if (classid == siciLs3) {
        QPainter p(&background);

        int w = background.width();
        int h = background.height();

        // translate origin to center of pixmap
        p.translate(w / 2, h / 2);

        int tracklen = w / 2;
        int shedwidth = h;

        p.rotate(180.0);
        
        // paint track
        p.fillRect(0, -3, -tracklen, 7, QBrush(Qt::black));
        
        // paint schuppen
        p.setBrush(QColor(192, 0 ,0));
        p.drawRect(0 , -shedwidth / 2, w / 2, shedwidth);
        p.drawLine(w / 4, -shedwidth / 2, w / 4, shedwidth / 2);

        setPaletteBackgroundPixmap(background);
    }

    // transfer table
    else if (classid == siciSbn) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();

        // paint track
        p.fillRect(0, h / 2 - 3, 4, 7, QBrush(Qt::black));
        p.fillRect(w - 4, h / 2 - 3, 4, 7,
                QBrush(Qt::black));

        // paint table icon
        p.drawPixmap(4, 4, QPixmap(transfertable_xpm));
        
        //paint label
        QFont f(QApplication::font());
        f.setPointSize(QApplication::font().pointSize() - 3);
        p.setFont(f);
        QFontMetrics fm(f);
        if (address2 - address1 == 0)
            sSoldText.setNum(state);
        QRect br = fm.boundingRect(sSoldText);
        br.setWidth(br.width() + 4);
        br.setHeight(br.height() + 2);
        br.moveTopLeft(QPoint(w / 2 - br.width()/2,
                    h / 2 - br.height()/2));
        p.fillRect(br, QBrush(Qt::white));
        p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                Qt::DontClip, sSoldText);

        setPaletteBackgroundPixmap(background);
    }
    
    // turntable
    else if (classid == siciDre) {
        QPainter p(&background);
        
        int w = background.width();
        int h = background.height();
        
        // translate origin to center of pixmap
        p.translate(w / 2, h / 2);

        // paint track s
        int tracklen = w / 4;
        int startx = -h / 2;
        p.fillRect(startx, -3, -tracklen, 7, QBrush(Qt::black));
        p.rotate(SANGLE);
        p.fillRect(startx, -3, -tracklen - 1, 7, QBrush(Qt::black));
        p.rotate(-2 * SANGLE);
        p.fillRect(startx, -3, -tracklen - 1, 7, QBrush(Qt::black));
        p.rotate(180.0);
        p.fillRect(startx, -3, -tracklen - 1, 7, QBrush(Qt::black));
        p.rotate(SANGLE);
        p.fillRect(startx, -3, -tracklen, 7, QBrush(Qt::black));
        p.rotate(SANGLE);
        p.fillRect(startx, -3, -tracklen - 1, 7, QBrush(Qt::black));

        // paint icon
        p.setBrush(Qt::darkGray);
        p.setPen(QPen(QColor(128, 0, 0), 2));
        p.drawEllipse(-h / 2 + 1, -h / 2 + 1, h - 2, h - 2);

        // turning track, may be animated later
        p.setBrush(Qt::white);
        p.rotate(-180.0 -SANGLE/2);
        p.drawRect(-h / 2 + 2, -3, h - 4, 7);
        p.drawLine(-h / 2 + 2, 0, h / 2 - 2, 0);
        p.drawRect(6, -6, 6, 3);

        //paint label
        p.rotate(-SANGLE/2);
        p.setPen(QPen(Qt::black));
        QFont f(QApplication::font());
        f.setPointSize(QApplication::font().pointSize() - 3);
        p.setFont(f);
        QFontMetrics fm(f);
        QString s;
        s.setNum(iSoldSubType);
        QRect br = fm.boundingRect(s);
        br.setWidth(br.width() + 4);
        br.setHeight(br.height() + 2);
        br.moveTopLeft(QPoint(-br.width()/2, 5));
        p.fillRect(br, QBrush(Qt::white));
        p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
                Qt::DontClip, s);

        setPaletteBackgroundPixmap(background);
    }
    
    addTooltip();
}


/*this draws only foreground lines on background pixmap*/
void element::paintEvent(QPaintEvent*)
{
    /* paint optional selection rectangle*/
    if (selectionMode != ksmNormal) {
        QPainter p(this);
        QColor c;

        switch (selectionMode) {
            case ksmStopSig:
                // red if in show route mode, stop signal
                c = QColor(Qt::red);
                break;
            case ksmStartSig:
                // green if in show route mode, start signal
                c = QColor(Qt::green);
                break;
            case ksmDisplay:
                // magenta if in show route mode, train number display
                c = QColor(Qt::magenta);
                break;
            case ksmSwitchEl:
                // yellow if clicked element in record route mode
                c = QColor(251, 251, 0);
                break;
            case ksmFoundEl:
                // found: orange
                c = QColor("DarkOrange");
                break;
            case ksmDropTarget:
                c = QColor(Qt::white);
                break;
            default:
                c = QColor(Qt::black);
                break;
        }

        int h = height();
        int w = width();

        p.setPen(QPen(c, 2, Qt::SolidLine));
        p.drawLine(0, h - 1, w, h - 1);
        p.drawLine(w - 1, h - 1, w - 1, 0);
        p.drawLine(w - 1, 1, 0, 0);
        p.drawLine(1, 1, 1, h - 1);
    }
}


/**
 * remove every tool tip and if configured add new one
 * with current element data
 */
void element::addTooltip()
{
    if (!pref.datatooltips)
        return;

    QToolTip::remove(this);
    QString a1 = QString::number(address1);
    QString a2 = QString::number(address2);

    QString tip1, tip2;

    tip1.sprintf(
            "Item No: %d\n"
            "ClassId: %d\n"
            "Hide LEDs: %s (=%1d)\n"
            "Inverted: %s (=%1d)\n"
            "Protocol: %s\n"
            "Address 1: %s\n",
            iSoldIndex,
            classid,
            (trackindicatoroff ? "No" : "Yes"),
            trackindicatoroff,
            iSoldInvert == -1 ? "N/A" : (iSoldInvert ==
                0 ? "No" : "Yes"),
            iSoldInvert,
            protocol1 == SrcpMessage::proNone ? "N/A (=-1)"
                : (protocol1 == SrcpMessage::proMM ? "Motorola"
                        : (protocol1 == SrcpMessage::proDCC ? "NMRA/DCC"
                            : (protocol1 == SrcpMessage::proSelectrix ?
                                "Selectrix"
                                : "Server"))),
            address1 == -1 ?  "N/A (=-1)" : (char*)a1.data());

    tip2.sprintf(
            "Address 2: %s\n"
            "xc Conn 1: %s (=%1d)\n"
            "xc Conn 2: %s (=%1d)\n"
            "State: %d\n"
            "Subtype: %d\n"
            "Text: %s\n"
            "Locked: %s (=%1d)\n"
            "Time1 (ms): %d\n"
            "FB Contact: %d\n",
            address2 == -1 ? "N/A (=-1)" : (char*)a2.data(),
            xchangeport1 ==
            -1 ? "N/A" : (xchangeport1 == 0 ? "No" : "Yes"),
            xchangeport1,
            xchangeport2 ==
            -1 ? "N/A" : (xchangeport2 == 0 ? "No" : "Yes"),
            xchangeport2, state, iSoldSubType,
            sSoldText == "-1" ? "N/A (=-1)" : (char*)sSoldText.data(),
            lockCounter == -1 ? "N/A" : (isLocked() ? "Yes" : "No"),
            lockCounter, activetime1, iFBContact);

    tip1.append(tip2);

    QToolTip::add(this, tip1);
}


/**
 * paint yellow track and return vertical correction value
 * to_r = routing direction to right layout side
 * C = correction for index
 */

/* two dimensional routing:
 *       entry -> exit (rdC, rdN, rdS, rdW, rdE, rdNW, rdNE, ...)
 *
 *
 *  rdNW | rdN  | rdNE
 * ------+------+------
 *  rdW  | rdC  | rdE
 * ------+------+------
 *  rdSW | rdS  | rdSE
 *
 */
unsigned int element::routeElement(unsigned int entrydir, bool setroute)
{
    unsigned int returnvalue = rdCenter;

    /* - */
    if (classid == siciSt1 || classid == siciEnk
            || classid == siciHs1 || classid == siciHs3
            || classid == siciZt1 || classid == siciZt3
            || classid == siciRt1 || classid == siciRt3
            || classid == siciHss1 || classid == siciHss3
            || classid == siciSs1 || classid == siciSs3
            || classid == siciSh1 || classid == siciSh3
            || classid == siciSd1 || classid == siciSd3
            || classid == siciBue || classid == siciTdr
            || classid == siciTdl || classid == siciTdb 
            || classid == siciVs1 || classid == siciVs3
            || classid == siciWs1 || classid == siciWs3
            || classid == siciZp1 || classid == siciZp3
            || classid == siciBld
            || classid == siciAdr) {

        if (entrydir == rdW)
            returnvalue = rdE;
        else if (entrydir == rdE)
            returnvalue = rdW;
    }

    /* | */
    else if (classid == siciSt2) {
        if (entrydir == rdN)
            returnvalue = rdS;
        else if (entrydir == rdS)
            returnvalue = rdN;
    }

    /* \
       | */
    else if (classid == siciCl2) {
        if (entrydir == rdNW)
            returnvalue = rdS;
        else if (entrydir == rdS)
            returnvalue = rdNW;
    }

    /* /
       | */
    else if (classid == siciCr2) {
        if (entrydir == rdNE)
            returnvalue = rdS;
        else if (entrydir == rdS)
            returnvalue = rdNE;
    }

    /* |
       \ */
    else if (classid == siciCl4) {
        if (entrydir == rdN)
            returnvalue = rdSE;
        else if (entrydir == rdSE)
            returnvalue = rdN;
    }

    /* |
       / */
    else if (classid == siciCr4) {
        if (entrydir == rdN)
            returnvalue = rdSW;
        else if (entrydir == rdSW)
            returnvalue = rdN;
    }

    else if (classid == siciTl1) {
        // --
        if (state == 0) {
            if (entrydir == rdW)
                returnvalue = rdE;
            else if (entrydir == rdE)
                returnvalue = rdW;
        }
        else {
            // -/
            if (entrydir == rdW)
                returnvalue = rdNE;
            else if (entrydir == rdNE)
                returnvalue = rdW;
        }
    }

    else if (classid == siciTl3) {
        // --
        if (state == 0) {
            if (entrydir == rdW)
                returnvalue = rdE;
            else if (entrydir == rdE)
                returnvalue = rdW;
        }
        else {
            // /-
            if (entrydir == rdE)
                returnvalue = rdSW;
            else if (entrydir == rdSW)
                returnvalue = rdE;
        }
    }

    else if (classid == siciTr3) {
        // --
        if (state == 0) {
            if (entrydir == rdW)
                returnvalue = rdE;
            else if (entrydir == rdE)
                returnvalue = rdW;
        }
        else {
            // \-
            if (entrydir == rdE)
                returnvalue = rdNW;
            else if (entrydir == rdNW)
                returnvalue = rdE;
        }
    }

    else if (classid == siciTr1) {
        // --
        if (state == 0) {
            if (entrydir == rdW)
                returnvalue = rdE;
            else if (entrydir == rdE)
                returnvalue = rdW;
        }
        else {
            /* -\ */
            if (entrydir == rdW)
                returnvalue = rdSE;
            else if (entrydir == rdSE)
                returnvalue = rdW;
        }
    }

    /* / */
    else if (classid == siciSt4) {
        if (entrydir == rdSW)
            returnvalue = rdNE;
        else if (entrydir == rdNE)
            returnvalue = rdSW;
    }

    /* \ */
    else if (classid == siciSt3) {
        if (entrydir == rdNW)
            returnvalue = rdSE;
        else if (entrydir == rdSE)
            returnvalue = rdNW;
    }

    else if (classid == siciCl1) {
        /* -/ */
        if (entrydir == rdW)
            returnvalue = rdNE;
        else if (entrydir == rdNE)
            returnvalue = rdW;
    }

    else if (classid == siciCl3) {
        /* /- */
        if (entrydir == rdE)
            returnvalue = rdSW;
        else if (entrydir == rdSW)
            returnvalue = rdE;
    }

    else if (classid == siciCr1) {
        /* -\ */
        if (entrydir == rdW)
            returnvalue = rdSE;
        else if (entrydir == rdSE)
            returnvalue = rdW;
    }

    else if (classid == siciCr3) {
        /* \- */
        if (entrydir == rdE)
            returnvalue = rdNW;
        else if (entrydir == rdNW)
            returnvalue = rdE;
    }

    else if (classid == siciTw1) {
        // --
        if (state == 0) {
            if (entrydir == rdW)
                returnvalue = rdE;
            else if (entrydir == rdE)
                returnvalue = rdW;
        }
        else {
            /* -/ */
            if (state == 1) {
                if (entrydir == rdW)
                    returnvalue = rdNE;
                else if (entrydir == rdNE)
                    returnvalue = rdW;
            }
            /* -\ */
            else {
                if (entrydir == rdW)
                    returnvalue = rdSE;
                else if (entrydir == rdSE)
                    returnvalue = rdW;
            }
        }
    }

    else if (classid == siciTw3) {
        // --
        if (state == 0) {
            if (entrydir == rdW)
                returnvalue = rdE;
            else if (entrydir == rdE)
                returnvalue = rdW;
        }
        else {
            /* /- */
            if (state == 1) {
                if (entrydir == rdE)
                    returnvalue = rdSW;
                else if (entrydir == rdSW)
                    returnvalue = rdE;
            }
            /* \- */
            else {
                if (entrydir == rdE)
                    returnvalue = rdNW;
                else if (entrydir == rdNW)
                    returnvalue = rdE;
            }
        }
    }

    else if (classid == siciSy1) {
        /* -\ */
        if (state == 0) {
            if (entrydir == rdW)
                returnvalue = rdSE;
            else if (entrydir == rdSE)
                returnvalue = rdW;
        }
        /* -/ */
        else {
            if (entrydir == rdW)
                returnvalue = rdNE;
            else if (entrydir == rdNE)
                returnvalue = rdW;
        }
    }

    else if (classid == siciSy3) {
        /* \- */
        if (state == 0) {
            if (entrydir == rdE)
                returnvalue = rdNW;
            else if (entrydir == rdNW)
                returnvalue = rdE;
        }
        /* /- */
        else {
            if (entrydir == rdE)
                returnvalue = rdSW;
            else if (entrydir == rdSW)
                returnvalue = rdE;
        }
    }

    else if (classid == siciIl1) {
        /* \
           \ */
        if (state == 0) {
            if (entrydir == rdNW)
                returnvalue = rdSE;
            else if (entrydir == rdSE)
                returnvalue = rdNW;
        }
        else {
            // \_
            if (entrydir == rdNW)
                returnvalue = rdE;
            else if (entrydir == rdE)
                returnvalue = rdNW;
        }
    }

    else if (classid == siciIl3) {
        /* \
           \ */
        if (state == 0) {
            if (entrydir == rdNW)
                returnvalue = rdSE;
            else if (entrydir == rdSE)
                returnvalue = rdNW;
        }
        else {
            /* _
               \ */
            if (entrydir == rdW)
                returnvalue = rdSE;
            else if (entrydir == rdSE)
                returnvalue = rdW;
        }
    }

    else if (classid == siciIr1) {
        /*  /
            / */
        if (state == 0) {
            if (entrydir == rdNE)
                returnvalue = rdSW;
            else if (entrydir == rdSW)
                returnvalue = rdNE;
        }
        else {
            /*  _
                /  */
            if (entrydir == rdSW)
                returnvalue = rdE;
            else if (entrydir == rdE)
                returnvalue = rdSW;
        }
    }

    else if (classid == siciIr3) {
        /*  /
            / */
        if (state == 0) {
            if (entrydir == rdNE)
                returnvalue = rdSW;
            else if (entrydir == rdSW)
                returnvalue = rdNE;
        }
        else {
            // _/
            if (entrydir == rdNE)
                returnvalue = rdW;
            else if (entrydir == rdW)
                returnvalue = rdNE;
        }
    }

    else if (classid == siciSl3) {
        // --
        if (state == 0) {
            if (entrydir == rdE)
                returnvalue = rdW;
            else if (entrydir == rdW)
                returnvalue = rdE;
        }
        //  /
        // /
        else if (state == 2) {
            if (entrydir == rdNE)
                returnvalue = rdSW;
            else if (entrydir == rdSW)
                returnvalue = rdNE;
        }
        else {
            /*  _
               /  */
            if (entrydir == rdSW)
                returnvalue = rdE;
            else if (entrydir == rdE)
                returnvalue = rdSW;
        }

        // shortcut to avoid double painting
        routed = setroute;
        setupElementIcon();
        return returnvalue;
    }

    else if (classid == siciSl1) {
        // --
        if (state == 0) {
            if (entrydir == rdE)
                returnvalue = rdW;
            else if (entrydir == rdW)
                returnvalue = rdE;
        }
        //  /
        // /
        else if (state == 2) {
            if (entrydir == rdNE)
                returnvalue = rdSW;
            else if (entrydir == rdSW)
                returnvalue = rdNE;
        }
        else {
            // _/
            if (entrydir == rdNE)
                returnvalue = rdW;
            else if (entrydir == rdW)
                returnvalue = rdNE;
        }

        // shortcut to avoid double painting
        routed = setroute;
        setupElementIcon();
        return returnvalue;
    }

    else if (classid == siciSr3) {
        // --
        if (state == 0) {
            if (entrydir == rdE)
                returnvalue = rdW;
            else if (entrydir == rdW)
                returnvalue = rdE;
        }
        /* \
            \ */
        else if (state == 2) {
            if (entrydir == rdNW)
                returnvalue = rdSE;
            else if (entrydir == rdSE)
                returnvalue = rdNW;
        }
        else {
           /* _
               \ */
            if (entrydir == rdW)
                returnvalue = rdSE;
            else if (entrydir == rdSE)
                returnvalue = rdW;
        }

        // shortcut to avoid double painting
        routed = setroute;
        setupElementIcon();
        return returnvalue;
    }

    else if (classid == siciSr1) {
        // --
        if (state == 0) {
            if (entrydir == rdE)
                returnvalue = rdW;
            else if (entrydir == rdW)
                returnvalue = rdE;
        }
        /* \
            \ */
        else if (state == 2) {
            if (entrydir == rdNW)
                returnvalue = rdSE;
            else if (entrydir == rdSE)
                returnvalue = rdNW;
        }
        else {
            // \_
            if (entrydir == rdNW)
                returnvalue = rdE;
            else if (entrydir == rdE)
                returnvalue = rdNW;
        }

        // shortcut to avoid double painting
        routed = setroute;
        setupElementIcon();
        return returnvalue;
    }

    // 4-state-DKWs
    else if (classid == siciDl1 && iSoldSubType == 1) {
        // --
        if (state == 0) {
            if (entrydir == rdE)
                returnvalue = rdW;
            else if (entrydir == rdW)
                returnvalue = rdE;
        }
        // _/
        else if (state == 1) {
            if (entrydir == rdNE)
                returnvalue = rdW;
            else if (entrydir == rdW)
                returnvalue = rdNE;
        }
        //  /
        // /
        else if (state == 2) {
            if (entrydir == rdNE)
                returnvalue = rdSW;
            else if (entrydir == rdSW)
                returnvalue = rdNE;
        }
        /*  _
           /  */
        else if (state == 3) {
            if (entrydir == rdSW)
                returnvalue = rdE;
            else if (entrydir == rdE)
                returnvalue = rdSW;
        }

        // shortcut to avoid double painting
        routed = setroute;
        setupElementIcon();
        return returnvalue;
    }

    else if (classid == siciDr1 && iSoldSubType == 1) {
        // --
        if (state == 0) {
            if (entrydir == rdE)
                returnvalue = rdW;
            else if (entrydir == rdW)
                returnvalue = rdE;
        }
        // \_
        else if (state == 1) {
            if (entrydir == rdE)
                returnvalue = rdNW;
            else if (entrydir == rdNW)
                returnvalue = rdE;
        }
        /* \
            \ */
        else if (state == 2) {
            if (entrydir == rdNW)
                returnvalue = rdSE;
            else if (entrydir == rdSE)
                returnvalue = rdNW;
        }
        /* _
            \ */
        else if (state == 3) {
            if (entrydir == rdW)
                returnvalue = rdSE;
            else if (entrydir == rdSE)
                returnvalue = rdW;
        }

        // shortcut to avoid double painting
        routed = setroute;
        setupElementIcon();
        return returnvalue;
    }


    // 2-state-DKWs
    else if (classid == siciDl1 && iSoldSubType == 0) {
        // --
        if (state == 0) {
            if (entrydir == rdE)
                returnvalue = rdW;
            else if (entrydir == rdW)
                returnvalue = rdE;
        }
        /* _/
           _
          /  */
        else if (state == 1) {
            if (entrydir == rdW)
                returnvalue = rdNE;
            else if (entrydir == rdNE)
                returnvalue = rdW;
            else if (entrydir == rdE)
                returnvalue = rdSW;
            else if (entrydir == rdSW)
                returnvalue = rdE;
        }

        // shortcut to avoid double painting
        routed = setroute;
        setupElementIcon();
        return returnvalue;
    }

    else if (classid == siciDr1 && iSoldSubType == 0) {
        // --
        if (state == 0) {
            if (entrydir == rdE)
                returnvalue = rdW;
            else if (entrydir == rdW)
                returnvalue = rdE;
        }
        /* \_
           _
            \ */
        else if (state == 1) {
            if (entrydir == rdNW)
                returnvalue = rdE;
            else if (entrydir == rdE)
                returnvalue = rdNW;
            else if (entrydir == rdSE)
                returnvalue = rdW;
            else if (entrydir == rdW)
                returnvalue = rdSE;
        }

        // shortcut to avoid double painting
        routed = setroute;
        setupElementIcon();
        return returnvalue;
    }

    // rail crossings
    else if (classid == siciKrh) {

        //  /
        // /
        if (entrydir == rdSW) {
            returnvalue = rdNE;
            routedtrack = 1;
        }
        else if (entrydir == rdNE) {
            returnvalue = rdSW;
            routedtrack = 1;
        }

        /* \
            \ */
        else if (entrydir == rdNW) {
            returnvalue = rdSE;
            routedtrack = 2;
        }
        else if (entrydir == rdSE) {
            returnvalue = rdNW;
            routedtrack = 2;
        }

        if (!setroute) 
            routedtrack = 0;
    }

    else if (classid == siciKl1) {
        // --
        if (entrydir == rdW) {
            returnvalue = rdE;
            routedtrack = 2;
        }
        else if (entrydir == rdE) {
            returnvalue = rdW;
            routedtrack = 2;
        }

        //  /
        // /
        else if (entrydir == rdSW) {
            returnvalue = rdNE;
            routedtrack = 1;
        }
        else if (entrydir == rdNE) {
            returnvalue = rdSW;
            routedtrack = 1;
        }

        if (!setroute) {
            routedtrack = 0;
        }
    }

    else if (classid == siciKr1) {
        // --
        if (entrydir == rdW) {
            returnvalue = rdE;
            routedtrack = 2;
        }
        else if (entrydir == rdE) {
            returnvalue = rdW;
            routedtrack = 2;
        }
        /* \
            \ */
        else if (entrydir == rdNW) {
            returnvalue = rdSE;
            routedtrack = 1;
        }
        else if (entrydir == rdSE) {
            returnvalue = rdNW;
            routedtrack = 1;
        }

        if (!setroute) 
            routedtrack = 0;
    }

    //tunnel crossings
    else if (classid == siciTuv) {

        // | (tunnel)
        // |
        if (entrydir == rdS) {
            returnvalue = rdN;
            if (setroute) 
                routedtrack |= 2;
            else
                routedtrack &= ~2;
            // no repaint neccessary
            return returnvalue;
        }
        else if (entrydir == rdN) {
            returnvalue = rdS;
            if (setroute) 
                routedtrack |= 2;
            else
                routedtrack &= ~2;
            // no repaint neccessary
            return returnvalue;
        }

        /* -- (top) */
        else if (entrydir == rdW) {
            returnvalue = rdE;
            if (setroute) 
                routedtrack |= 1;
            else
                routedtrack &= ~1;
        }
        else if (entrydir == rdE) {
            returnvalue = rdW;
            if (setroute) 
                routedtrack |= 1;
            else
                routedtrack &= ~1;
        }
    }

    else if (classid == siciTuh) {

        // | (top)
        // |
        if (entrydir == rdS) {
            returnvalue = rdN;
            if (setroute) 
                routedtrack |= 1;
            else
                routedtrack &= ~1;
        }
        else if (entrydir == rdN) {
            returnvalue = rdS;
            if (setroute) 
                routedtrack |= 1;
            else
                routedtrack &= ~1;
        }

        /* --  (tunnel) */
        else if (entrydir == rdW) {
            returnvalue = rdE;
            if (setroute) 
                routedtrack |= 2;
            else
                routedtrack &= ~2;
            // no repaint neccessary
            return returnvalue;
        }
        else if (entrydir == rdE) {
            returnvalue = rdW;
            if (setroute) 
                routedtrack |= 2;
            else
                routedtrack &= ~2;
            // no repaint neccessary
            return returnvalue;
        }
    }

    else if (classid == siciTur) {

        //  / (top)
        // /
        if (entrydir == rdSW) {
            returnvalue = rdNE;
            if (setroute) 
                routedtrack |= 1;
            else
                routedtrack &= ~1;
            // repaint neccessary
        }
        else if (entrydir == rdNE) {
            returnvalue = rdSW;
            if (setroute) 
                routedtrack |= 1;
            else
                routedtrack &= ~1;
            // repaint neccessary
        }

        /* \  (tunnel)
            \ */
        else if (entrydir == rdNW) {
            returnvalue = rdSE;
            if (setroute) 
                routedtrack |= 2;
            else
                routedtrack &= ~2;
            // no repaint neccessary
            return returnvalue;
        }
        else if (entrydir == rdSE) {
            returnvalue = rdNW;
            if (setroute) 
                routedtrack |= 2;
            else
                routedtrack &= ~2;
            // no repaint neccessary
            return returnvalue;
        }
    }

    else if (classid == siciTul) {

        //  / (tunnel)
        // /
        if (entrydir == rdSW) {
            returnvalue = rdNE;
            if (setroute) 
                routedtrack |= 2;
            else
                routedtrack &= ~2;
            // no repaint neccessary
            return returnvalue;
        }
        else if (entrydir == rdNE) {
            returnvalue = rdSW;
            if (setroute) 
                routedtrack |= 2;
            else
                routedtrack &= ~2;
            // no repaint neccessary
            return returnvalue;
        }

        /* \  (top)
            \ */
        else if (entrydir == rdNW) {
            returnvalue = rdSE;
            if (setroute) 
                routedtrack |= 1;
            else
                routedtrack &= ~1;
            // repaint neccessary
        }
        else if (entrydir == rdSE) {
            returnvalue = rdNW;
            if (setroute) 
                routedtrack |= 1;
            else
                routedtrack &= ~1;
            // repaint neccessary
        }
    }


    // repaint element according to new routing state
    if (returnvalue != rdCenter)
        setRouted(setroute);

    return returnvalue;
}

/**
 * This slot is always called if a feedback port toggles.
 */
void element::slotOccupyElement(unsigned int bus, unsigned int contact,
        bool ostate)
{
    /*track indicator*/
    if (bus == iFBBusNo) {

        // address panels don't get occupied
        if (classid == siciAdr) {
            if (iSoldInvert != 1) {
                unsigned int targetmod = (contact - 1) / 8 + 1;
                unsigned int selfmod = (iFBContact - 1) / 8 + 1;

                if (targetmod == selfmod)
                    updateEDiTSAddress(contact, ostate);
            }
        }
        else if (contact == (unsigned int)iFBContact)
            setOccupied(ostate);
    }

    /* trigger 1 on and off*/
    if (enable1fbtrigger) {
        if ((bus == button1fbbus) && (contact == button1fbcontact)) {

            switch (classid) {
                case siciRel:
                    switchToDir(ostate ? 1 : 0);
                    break;
                default:
                    break;
            }
        }
    }

    /*shortcut if (state = 0) => button release message*/
    if (!ostate)
        return;

    /*button 1 trigger only on*/
    if (enable1fbtrigger) {
        if ((bus == button1fbbus) && (contact == button1fbcontact)) {

            switch (classid) {
                case siciTaf:
                    /*FIXME: counter update doubled code*/
                    /*
                     * increment counter and repaint symbol, if value
                     * has more than four digits, reset to zero
                     */
                    ++countervalue;
                    if (countervalue == 10000)
                        countervalue = 0;
                    setupElementIcon();
                    emit elementClicked(this, kFhtClicked);
                    break;
                case siciTau:
                    emit elementClicked(this, kUfgtClicked);
                    break;
                case siciTas:
                    emit elementClicked(this, kSgtClicked);
                    break;
                case siciTaw:
                    emit elementClicked(this, kWgtClicked);
                    break;
                case siciTwh:
                    /*FIXME: counter update doubled code*/
                    /*
                     * increment counter and repaint symbol, if value
                     * has more than four digits, reset to zero
                     */
                    ++countervalue;
                    if (countervalue == 10000)
                        countervalue = 0;
                    setupElementIcon();
                    emit elementClicked(this, kWhtClicked);
                    break;
                case siciTal:
                    emit elementClicked(this, kEinClicked);
                    break;
                case siciHss3:
                case siciHs1:
                case siciHs3:
                case siciZt1:
                case siciZt3:
                    emit elementClicked(this, kZfsClicked);
                    break;
                case siciHss1:
                case siciSs1:
                case siciSs3:
                case siciSd1:
                case siciSd3:
                case siciRt1:
                case siciRt3:
                    emit elementClicked(this, kRfsClicked);
                    break;
                case siciSh1:
                case siciSh3:
                    emit elementClicked(this, kZhsClicked);
                    break;
                default:
                    /*Check*/
                    emit elementClicked(this, kTurnoutClicked);
                    break;
            }
        }
    }

    /*button 2 trigger*/
    if (enable2fbtrigger) {
        if ((bus == button2fbbus) && (contact == button2fbcontact)) {
            switch (classid) {
                case siciTau:
                    emit elementClicked(this, kMgtClicked);
                    break;
                case siciTas:
                    emit elementClicked(this, kHagtClicked);
                    break;
                case siciTal:
                    emit elementClicked(this, kAusClicked);
                    break;
                case siciHss3:
                case siciSd1:
                case siciSd3:
                    emit elementClicked(this, kRfsClicked);
                    break;
                case siciHss1:
                    emit elementClicked(this, kZfsClicked);
                    break;
                default:
                    break;
            }
        }
    }
}


/**
 * calculate the EDiTS address by bit field manipulation
 */
void element::updateEDiTSAddress(unsigned int contact, bool bstate)
{
    /*
     * address range for contact is 1..8, 9..16, 17..24, etc.
     * first mask three lower address bits (7), then evaluate bit value
     */
    unsigned int address = (contact - 1) & 7u;
    unsigned int bit = 1u << address;
   
    if (bstate)
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


/**
 * Set train number to EDiTS address view.
 * Only possible, if is in inverted state.
 */
void element::updateTrainNumber(unsigned int value)
{
    if ((iSoldInvert == 1) && (value != editsAddress)) {
       editsAddress = value;
       setupElementIcon();
    }
}


/**
 * return true if this a train number display
 */
bool element::isTrainNumberDisplay()
{
    return (classid == siciAdr);
}


/**
 * Switch occupational state. If element was routed and occupation is
 * removed, route state is also removed.
 */
void element::setOccupied(bool ostate)
{
    if (occupied != ostate) {
        occupied = ostate;
        if (!ostate && routed)
            routed = false;
        /*TODO: repaint only if (!trackindicatoroff)*/
        setupElementIcon();
    }
}


void element::setRouted(bool rstate)
{
    if (routed != rstate || routedtrack != 0) {
        routed = rstate;
        /*TODO: repaint only if (!trackindicatoroff)*/
        setupElementIcon();
    }
}


void element::slotUpdateTurntableData(int keyno, int keycolor)
{
    address1 = address2 + keyno - 1;
    state = keycolor;

    // save target track number in subtype if a track key was pressed
    if (classid == siciDre && keyno >= 4)
        iSoldSubType = keyno * 2 - 9 + keycolor;

    setupElementIcon();
    sendSrcpState();
}

/*
 * copy all available tracks at turntable into element's text
 * field, update tool tip
 */
void element::slotCopyAvailTracks(const QString& trackstr)
{
    sSoldText = trackstr;
    addTooltip();
}


/* 
 * update tooltip visability and
 * redraw backgroud pixmap with the opposite of text/address labels
 */
void element::slotRepaintLayout()
{
    QToolTip::remove(this);
    setupElementIcon();
}


void element::writeFileTextToStream(QTextStream& ts)
{
    ts << GF_CLASSID << DS << classid << endl
        << GF_INDEX << DS << iSoldIndex<< endl;

    switch (classid) {
        case siciFeb:
        case siciFee:
        case siciFeg:
        case siciFen:
        case siciFer:
        case siciFey:
        case siciBs1:
        case siciBs2:
        case siciBs3:
        case siciBs4:
        case siciBuc:
        case siciBul:
        case siciBur:
        case siciLt1:
        case siciLs1:
        case siciLb1:
        case siciLt3:
        case siciLs3:
        case siciLb3:
            break;

            /*dual buttons*/
        case siciTal:
        case siciTas:
        case siciTau:
            ts << GF_BUTTON2FB << DS << enable2fbtrigger << DS
               << button2fbbus << DS << button2fbcontact << endl;
            // fall through

            /*single button*/
        case siciTaf:
        case siciTaw:
        case siciTwh:
            ts << GF_BUTTON1FB << DS << enable1fbtrigger << DS
               << button1fbbus << DS << button1fbcontact << endl;
            break;

        case siciSt1:
        case siciTdr:
        case siciTdb:
            ts << GF_TEXT << DS << sSoldText << endl;
            // fall through
        case siciTuh:
        case siciTuv:
        case siciTul:
        case siciTur:
        case siciSt2:
        case siciSt3:
        case siciSt4:
        case siciCr1:
        case siciCr2:
        case siciCr3:
        case siciCr4:
        case siciCl1:
        case siciCl2:
        case siciCl3:
        case siciCl4:
            ts << GF_FBPORT << DS << iFBBusNo << DS << iFBContact << endl
                << GF_HIDELEDS << DS << trackindicatoroff << endl;
            break;

        case siciTxt:
            ts << GF_INVERSTO  << DS << iSoldInvert << endl
                << GF_TEXT << DS << sSoldText << endl;
            break;

        default:
            ts << GF_INVERSTO  << DS << iSoldInvert << endl
                << GF_PROTOCOL1  << DS << 
                ((protocol1 == SrcpMessage::proMM) ? "M" :
                 (protocol1 == SrcpMessage::proDCC) ? "N" :
                 (protocol1 == SrcpMessage::proServer) ? "P" :
                 (protocol1 == SrcpMessage::proSelectrix) ? "S" : "-1") << endl
                << GF_ADDRESS1  << DS << bus1 << DS << address1 <<
                DS << port1 << endl;
            if (address2 != -1) {
                ts << GF_PROTOCOL2  << DS << 
                ((protocol2 == SrcpMessage::proMM) ? "M" :
                 (protocol2 == SrcpMessage::proDCC) ? "N" :
                 (protocol2 == SrcpMessage::proServer) ? "P" :
                 (protocol2 == SrcpMessage::proSelectrix) ? "S" : "-1") << endl
                << GF_ADDRESS2  << DS << bus2 << DS << address2 <<
                    DS << port2 << endl;
            }
            ts << GF_XCHCONN1  << DS << xchangeport1 << endl;
            if (address2 != -1) {
                ts << GF_XCHCONN2  << DS << xchangeport2 << endl;
            }
            ts << GF_DIRECTION << DS << state << endl
                << GF_SUBTYPE   << DS << iSoldSubType << endl
                << GF_TEXT      << DS << sSoldText << endl
                << GF_ACTTIME1   << DS << activetime1 << endl
                << GF_ACTTIME2   << DS << activetime2 << endl
                << GF_FBPORT    << DS << iFBBusNo << DS << iFBContact << endl
                /*for now: save two triggered buttons, if used or not*/
                << GF_BUTTON1FB << DS << enable1fbtrigger << DS
                << button1fbbus << DS << button1fbcontact << endl
                << GF_BUTTON2FB << DS << enable2fbtrigger << DS
                << button2fbbus << DS << button2fbcontact << endl
                << GF_HIDELEDS  << DS << trackindicatoroff << endl;

            break;
    }
    ts << '%' << endl;
    modified = false;
}


QString element::getLabelText() const
{
    return sSoldText;
}


bool element::hasSameAddress(int bus, int address)
{
    return (bus == bus1 && address == address1
            && address1 > 0);
}


bool element::isLocked()
{
    return (lockCounter > 0);
}


/*already locked crossings are not lockable for a second time*/
bool element::isLockable()
{
    bool result = true;

    if ((classid == siciKrh || classid == siciKr1 || classid == siciKl1)
        && (lockCounter > 0))
            result = false;

    return result;
}

/*return occupancy state condering current track indicator state*/
bool element::isOccupied()
{
    if (trackindicatoroff)
        return false;

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


void element::getStateData(stateElement& se)
{
    se.name = sSoldText;
    se.bus = bus1;
    se.address = address1;
    se.state = state;
    se.elemPtr = this;
}


bool element::hasDifferentState(int dir)
{
    return state != dir;
}


bool element::hasShuntingRouteButtonOnly()
{
    return (classid == siciSs1 || classid == siciSs3 ||
            classid == siciSd1 || classid == siciSd3 ||
            classid == siciRt1 || classid == siciRt3 ||
            classid == siciWs1 || classid == siciWs3);
}


bool element::hasLEDsOn()
{
    return (!trackindicatoroff);
}


int element::getAddressCount()
{
    int returnvalue = 0;

    switch (classid) {
        // virtual addresses
        case siciZt1:
        case siciZt3:
        case siciRt1:
        case siciRt3:
        case siciKr1:
        case siciKl1:
        case siciKrh:
        case siciAdr:
        case siciBld:
            // real addresses
            //signals
        case siciSs1:
        case siciSs3:
        case siciSh1:
        case siciSh3:
        case siciSd1:
        case siciSd3:
        case siciWs1:
        case siciWs3:
        case siciZp1:
        case siciZp3:
            //turnouts
        case siciTr1:
        case siciTr3:
        case siciTl1:
        case siciTl3:
        case siciIr1:
        case siciIr3:
        case siciIl1:
        case siciIl3:
        case siciSy1:
        case siciSy3:
            // tools
        case siciDre:
        case siciSbn:
        case siciEnk:
        case siciRel:
        case siciMdc:
            returnvalue = 1;
            break;

        case siciHs1:
        case siciHs3:
        case siciVs1:
        case siciVs3:
            if (iSoldSubType == 4)
                returnvalue = 2;
            else
                returnvalue = 1;
            break;

        case siciDr1:
        case siciDl1:
            if (iSoldSubType == 1)
                returnvalue = 2;
            else
                returnvalue = 1;
            break;

        case siciHss1:
        case siciHss3:
        case siciTw1:
        case siciTw3:
        case siciSr1:
        case siciSr3:
        case siciSl1:
        case siciSl3:
            returnvalue = 2;
            break;

        default:
            break;
    }
    return returnvalue;
}


void element::updateFeedbackState()
{
    // get current feedback status from server to update LEDstate
    if ((!trackindicatoroff) && (iFBContact > 0)) {
        
        SrcpMessage sm = SrcpMessage(SrcpMessage::msgFbGet);
        sm.setFbData(iFBBusNo,
                (SrcpMessage::Feedback) pref.fbmoduletype, iFBContact);
        emit sendSrcpMessage(&sm);
    }
}


/*
 * update element if system font is changed e.g. by qtconfig
 * do nothing for empty element with no text
 */
void element::fontChange(const QFont&)
{
    if (classid == siciTxt && (sSoldText.isEmpty() || sSoldText == "-1"))
        return;
    else
        setupElementIcon();
}


bool element::sendSRCP08InitGA(unsigned int gano)
{
    bool returnvalue = false;

    if (isSwitchable()) {

        SrcpMessage sm = SrcpMessage(SrcpMessage::msgGaInit);
        
        if (gano == 1)
            sm.setGaData(protocol1, bus1, address1, 0, 0, 0);
        else
            sm.setGaData(protocol2, bus2, address2, 0, 0, 0);

        emit sendSrcpMessage(&sm);

        returnvalue = true;
    }
    return returnvalue;
}


int element::getAddress1()
{               
    return address1;
}   
            
            
int element::getAddress2()
{
    return address2;
}   

                        
int element::getGA1BusNo()
{               
    return bus1;
}   
            
            
int element::getGA2BusNo()
{
    return bus2;
}   

                        
int element::getFBBusNo()
{
    return iFBBusNo;
}   
                

bool element::hasThreeStates()
{
    return (classid == siciHs1 || classid == siciHs3 ||
            classid == siciVs1 || classid == siciVs3) &&
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
    lastdir = state;

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
         int olddir = state;
         state = newdir;
         sendSrcpState();
         // shortcut to prevent blinking
         if (!pref.blinkingturnouts) {
             blinkcounter = 14;
             setLightsOn(false);
         }
         else
             state = olddir;
    }
    if (blinkcounter % 2 == 1)
        setLightsOn(false);
    else {
        if (blinkcounter == 4)
            state = newdir;
        setLightsOn(true);
    }
    ++blinkcounter;   
    if (blinkcounter < 13)
        QTimer::singleShot(500, this, SLOT(runTurnoutBlinkTimer()));
    else {
        blinkcounter = 0;
        if (!tablelight) {
            setupElementIcon();
            //repaint();
        }
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

/*
 * return direction an activated route will enter this signal
 */
unsigned int element::entryDir()
{
    unsigned returnvalue = rdCenter;

    // all signals starting routes to right
    if (
            classid == siciHs1 || classid == siciHss1 || 
            classid == siciSs1 || classid == siciSh1 || 
            classid == siciSd1 || classid == siciWs1 || 
            classid == siciZt1 || classid == siciRt1 
       )
        returnvalue = rdW;

    // all signals starting routes to left
    else if (
            classid == siciHs3 || classid == siciHss3 || 
            classid == siciSs3 || classid == siciSh3 || 
            classid == siciSd3 || classid == siciWs3 || 
            classid == siciZt3 || classid == siciRt3 
            )
        returnvalue = rdE;

    return returnvalue;
}


/*change table light state for turnouts*/
void element::setTableLight(bool ison)
{
    if (tablelight != ison) {
        tablelight = ison;
        if (turnout || (classid == siciTal))
            setupElementIcon();
    }
}


bool element::showsStop()
{
    return (signal && (state == 0));
}


/*all elements that can receive feedback contact information via
 * drag-and-drop*/
bool element::canReceiveFbcDrop()
{
    return routable && !(classid == siciBue ||
            (classid == siciAdr && iSoldInvert == 1));
}


void element::setDroppedFbContact(QByteArray& data)
{
    //wrap iFBBusNo, iFBContact, occupied
    FbContact fbc;

    if (data.size() != sizeof(fbc))
        return;

    memcpy(&fbc, data.data(), data.size());
    selectionMode = ksmNormal;
    iFBBusNo = fbc.bus;
    iFBContact = fbc.contact;
    occupied = fbc.state;
    trackindicatoroff = false;
    setupElementIcon();
}


bool element::hasLabel()
{
    return switchable || routemark || classid == siciSt1 || classid == siciTxt
        || classid == siciTdr || classid == siciTdl || classid == siciTdb;
}


bool element::hasTrackIndicator()
{
    return canReceiveFbcDrop();
}


int element::driveCount()
{
    if (hasVirtualAddress() || classid == siciAdr)
        return 0;

    return getAddressCount();
}


bool element::hasVirtualAddress()
{
    bool returnvalue = false;

    switch (classid) {
        case siciZt1:
        case siciZt3:
        case siciRt1:
        case siciRt3:
        case siciKr1:
        case siciKl1:
        case siciKrh:
        case siciBld:
            returnvalue = true;
            break;
        case siciAdr:
            if (iSoldInvert == 1)
                returnvalue = true;
            break;
        default:
            break;
    }
    return returnvalue;
}


bool element::hasVariants()
{
    return classid == siciHs1 || classid == siciHs3 ||
        classid == siciHss1 || classid == siciHss3 ||
        classid == siciDl1 || classid == siciDr1 ||
        classid == siciEnk || classid == siciDre ||
        classid == siciVs1 || classid == siciVs3 ||
        classid == siciAdr;
}


int element::buttonCount()
{
    if (classid == siciHss1 || classid == siciHss3 || classid == siciTal
            || classid == siciSd1 || classid == siciSd3
            || classid == siciTau || classid == siciTas)
        return 2;

    if (routemark || turnout || classid == siciTwh
            || classid == siciTaf || classid == siciTaw)
        return 1;

    return 0;
}

bool element::showLabelDialog()
{
    bool returnvalue = false;

    ElementLabelDialog* dlg = new ElementLabelDialog(this);
    if (dlg == NULL)
        return false;

    /*move dialog to mouse click point*/
    dlg->move(QCursor::pos());
    dlg->setSymbolText(sSoldText);
    if (dlg->exec() == QDialog::Accepted) {
        sSoldText = dlg->getSymbolText();
        returnvalue = true;
        setupElementIcon();
    }
    delete dlg;
    return returnvalue;
}

bool element::showTrackIndicatorDialog()
{
    bool returnvalue = false;

    //TODO: rename FeedbackTriggerDialog()
    FeedbackTriggerDialog* dlg = new FeedbackTriggerDialog(this);
    if (dlg == NULL)
        return false;

    /*move dialog to mouse click point*/
    dlg->move(QCursor::pos());
    dlg->setCaption(tr("Edit item #%1").arg(iSoldIndex));
    dlg->setCheckBoxText(tr("&Enable track indicator"));
    dlg->enableTrigger(!trackindicatoroff);
    dlg->setFBBus(iFBBusNo);
    dlg->setFBContact(iFBContact);
    connect(dlg, SIGNAL(sigShowFBmodules()),
            this, SIGNAL(sigShowFBmodules()));

    if (dlg->exec() == QDialog::Accepted) {
        trackindicatoroff = !dlg->isTriggerEnabled();
        iFBBusNo = dlg->getFBBus();
        iFBContact = dlg->getFBContact();
        returnvalue = true;
        setupElementIcon();
    }
    
    disconnect(dlg, SIGNAL(sigShowFBmodules()),
            this, SIGNAL(sigShowFBmodules()));
    
    delete dlg;
    return returnvalue;
}

bool element::showDriveDialog()
{
    bool returnvalue = false;

    DriveDialog* dlg = new DriveDialog(this);
    if (dlg == NULL)
        return false;

    /*move dialog to mouse click point*/
    dlg->move(QCursor::pos());

    switch(classid) {
        case siciDre:
        case siciSbn:
        case siciMdc:
            dlg->setProtocol((int) protocol2);
            dlg->setActiveTime(activetime2);
            dlg->setSRCPBus1(bus2);
            dlg->setAddress1(address2);
            dlg->setXChangeConn1(xchangeport2);
            dlg->setPort1(port2);
            break;
        default:
            dlg->setProtocol((int) protocol1);
            dlg->setActiveTime(activetime1);
            dlg->setSRCPBus1(bus1);
            dlg->setAddress1(address1);
            dlg->setXChangeConn1(xchangeport1);
            dlg->setPort1(port1);
            break;
    }

    if (dlg->exec() == QDialog::Accepted) {

        switch(classid) {
            case siciDre:
            case siciSbn:
            case siciMdc:
                protocol2 =
                    (SrcpMessage::Protocol) dlg->getProtocol();
                activetime2 = dlg->getActiveTime();
                bus2 = dlg->getSRCPBus1();
                address2 = dlg->getAddress1();
                xchangeport2 = dlg->getXChangeConn1();
                port2 = dlg->getPort1();
                if (sSoldText.isEmpty() && (classid != siciDre))
                    sSoldText.setNum(address2);
                break;
            default:
                protocol1 =
                    (SrcpMessage::Protocol) dlg->getProtocol();
                activetime1 = dlg->getActiveTime();
                bus1 = dlg->getSRCPBus1();
                address1 = dlg->getAddress1();
                xchangeport1 = dlg->getXChangeConn1();
                port1 = dlg->getPort1();
                if (sSoldText.isEmpty())
                    sSoldText.setNum(address1);
                break;
                break;
        }
        returnvalue = true;
        setupElementIcon();
    }
    
    delete dlg;
    return returnvalue;
}

bool element::showDualDriveDialog()
{
    bool returnvalue = false;

    DualDriveDialog* dlg = new DualDriveDialog(this);
    if (dlg == NULL)
        return false;

    /*move dialog to mouse click point*/
    dlg->move(QCursor::pos());
    dlg->setProtocol1((int) protocol1);
    dlg->setActiveTime1(activetime1);
    dlg->setSRCPBus1(bus1);
    dlg->setAddress1(address1);
    dlg->setXChangeConn1(xchangeport1);
    dlg->setPort1(port1);

    dlg->setProtocol2((int) protocol2);
    dlg->setActiveTime2(activetime2);
    dlg->setSRCPBus2(bus2);
    dlg->setAddress2(address2);
    dlg->setXChangeConn2(xchangeport2);
    dlg->setPort2(port2);

    if (dlg->exec() == QDialog::Accepted) {
        protocol1 =
            (SrcpMessage::Protocol) dlg->getProtocol1();
        activetime1 = dlg->getActiveTime1();
        bus1 = dlg->getSRCPBus1();
        address1 = dlg->getAddress1();
        xchangeport1 = dlg->getXChangeConn1();
        port1 = dlg->getPort1();
        if (sSoldText.isEmpty())
            sSoldText.setNum(address1);

        protocol2 =
            (SrcpMessage::Protocol) dlg->getProtocol2();
        activetime2 = dlg->getActiveTime2();
        bus2 = dlg->getSRCPBus2();
        address2 = dlg->getAddress2();
        xchangeport2 = dlg->getXChangeConn2();
        port2 = dlg->getPort2();
        returnvalue = true;
        setupElementIcon();
    }
    
    delete dlg;
    return returnvalue;
}


bool element::showVirtualAddressDialog()
{
    bool returnvalue = false;

    VirtualAddressDialog* dlg = new VirtualAddressDialog(this);
    if (dlg == NULL)
        return false;

    /*move dialog to mouse click point*/
    dlg->move(QCursor::pos());
    dlg->setClassId(classid);
    dlg->setSRCPBus1(bus1);
    dlg->setAddress1(address1);

    if (dlg->exec() == QDialog::Accepted) {
        bus1 = dlg->getSRCPBus1();
        address1 = dlg->getAddress1();
        returnvalue = true;
        setupElementIcon();
    }
    
    delete dlg;
    return returnvalue;
}

bool element::showVariantDialog()
{
    bool returnvalue = false;
    QPixmap pm;

    VariantDialog* dlg = new VariantDialog(this);
    if (dlg == NULL)
        return false;

    /*move dialog to mouse click point*/
    dlg->move(QCursor::pos());
    
    switch (classid) {
        case siciHss1:
        case siciHss3:
            pm = QPixmap(signal_hss_st1_xpm);
            dlg->addVariant(tr("Hp&0, Hp1 and Sh1"), pm);
            pm = QPixmap(signal_hss_st2_xpm);
            dlg->addVariant(tr("Hp0, Hp&2 and Sh1"), pm);
            pm = QPixmap(signal_hss_st3_xpm);
            dlg->addVariant(tr("Hp0, Hp&1, Hp2 and Sh1"), pm);
            /*
             * index Subtype
             * -------------
             *   0     1
             *   1     7
             *   2     5
             * -------------
             */
            if (iSoldSubType == 1)
                dlg->setChoice(0);
            else if (iSoldSubType == 7)
                dlg->setChoice(1);
            else if (iSoldSubType == 5)
                dlg->setChoice(2);
            break;

        case siciHs1:
        case siciHs3:
            pm = QPixmap(signal_hs_st1_xpm);
            dlg->addVariant(tr("Hp&0 and Hp1"), pm);
            pm = QPixmap(signal_hs_st2_xpm);
            dlg->addVariant(tr("Hp0 and Hp&2"), pm);
            pm = QPixmap(signal_hs_st3_xpm);
            dlg->addVariant(tr("Hp0, Hp&1 and Hp2"), pm);
            /*
             * index Subtype
             * -------------
             *   0     0
             *   1     6
             *   2     4
             * -------------
             */
            if (iSoldSubType == 0)
                dlg->setChoice(0);
            else if (iSoldSubType == 6)
                dlg->setChoice(1);
            else if (iSoldSubType == 4)
                dlg->setChoice(2);
            break;

        case siciVs1:
        case siciVs3:
            pm = QPixmap(signal_vs_st1_xpm);
            dlg->addVariant(tr("Vr&0 and Vr1"), pm);
            pm = QPixmap(signal_vs_st2_xpm);
            dlg->addVariant(tr("Vr0 and Vr&2"), pm);
            pm = QPixmap(signal_vs_st3_xpm);
            dlg->addVariant(tr("Vr0, Vr&1 and Vr2"), pm);
            /*
             * index Subtype
             * -------------
             *   0     0
             *   1     6
             *   2     4
             * -------------
             */
            if (iSoldSubType == 0)
                dlg->setChoice(0);
            else if (iSoldSubType == 6)
                dlg->setChoice(1);
            else if (iSoldSubType == 4)
                dlg->setChoice(2);
            break;

        case siciDr1:
            pm = QPixmap(dkw_rechts_st2_xpm);
            dlg->addVariant(tr("&One drive (two switch positions)"), pm);
            pm = QPixmap(dkw_rechts_st3_xpm);
            dlg->addVariant(tr("&Two drives (four switch positions)"), pm);
            dlg->setChoice(iSoldSubType);
            break;

        case siciDl1:
            pm = QPixmap(dkw_links_st2_xpm);
            dlg->addVariant(tr("&One drive (two switch positions)"), pm);
            pm = QPixmap(dkw_links_st3_xpm);
            dlg->addVariant(tr("&Two drives (four switch positions)"), pm);
            /*
             * index Subtype
             * -------------
             *   0     0
             *   1     1
             * -------------
             */
            dlg->setChoice(iSoldSubType);
            break;

        case siciAdr:
            dlg->addVariant(tr("&EDiTS-Pro indicator"));
            dlg->addVariant(tr("&Train number tracing"));
            /*
             * index Inverted
             * --------------
             *   0     0       feedback address
             *   1     1       virtual address
             * --------------
             */
            // fix -1 default value
            dlg->setChoice(iSoldInvert != 1 ? 0 : 1);
            break;

        case siciDre:
            dlg->addVariant(tr("Typ 1&4 (base address 209)"));
            dlg->addVariant(tr("Typ 1&5 (base address 225)"));
            /*
             * index Address2
             * --------------
             *   0     209
             *   1     225
             * --------------
             */
            if (address2 == MMTT14)
                dlg->setChoice(0);
            else
                dlg->setChoice(1);
            break;

        case siciEnk:
            pm = QPixmap(entkoppler_st1_xpm);
            dlg->addVariant(tr("&Bistable coupler"), pm);
            pm = QPixmap(entkoppler_st2_xpm);
            dlg->addVariant(tr("Momentary coupler on &left connector"), pm);
            pm = QPixmap(entkoppler_st3_xpm);
            dlg->addVariant(tr("Momentary coupler on &right connector"), pm);
            /*
             * index Subtype
             * -------------
             *   0    -1
             *   1     0
             *   2     1
             * -------------
             */
            if (iSoldSubType == -1)
                dlg->setChoice(0);
            else if (iSoldSubType == 0)
                dlg->setChoice(1);
            else if (iSoldSubType == 1)
                dlg->setChoice(2);
            break;

        default:
            break;
    }

    if (dlg->exec() == QDialog::Accepted) {

        switch (classid) {
            case siciHss1:
            case siciHss3:
                iSoldSubType = dlg->getChoice();
                if (iSoldSubType == 0)
                    iSoldSubType = 1;
                else if (iSoldSubType == 1)
                    iSoldSubType = 7;
                else if (iSoldSubType == 2)
                    iSoldSubType = 5;
                break;

            case siciHs1:
            case siciHs3:
            case siciVs1:
            case siciVs3:
                iSoldSubType = dlg->getChoice();
                if (iSoldSubType == 1)
                    iSoldSubType = 6;
                else if (iSoldSubType == 2)
                    iSoldSubType = 4;
                break;

            case siciDr1:
            case siciDl1:
                iSoldSubType = dlg->getChoice();
                break;

            case siciAdr:
                iSoldInvert = dlg->getChoice();
                break;

            case siciDre:
                if (dlg->getChoice() == 0)
                    address2 = MMTT14;
                else
                    address2 = MMTT15;
                break;

            case siciEnk:
                iSoldSubType = dlg->getChoice();
                if (iSoldSubType == 0)
                    iSoldSubType = -1;
                else if (iSoldSubType == 1)
                    iSoldSubType = 0;
                else if (iSoldSubType == 2)
                    iSoldSubType = 1;
                break;

            default:
                break;
        }
        returnvalue = true;
        setupElementIcon();
    }
    
    delete dlg;
    return returnvalue;
}

/*show dialog at click position, position is also used to check if left
 * or right button is edited*/
bool element::showFeedbackTriggerDialog(const QPoint& p)
{
    QString buttontext;
    bool returnvalue = false;
    bool hastwo = false;

    // select Button names; FHT WGT, WHT UfGT,...
    QPoint pos = mapFromParent(p);
    bool left = pos.x() < (width() / 2);

    switch (classid) {
        case siciTaf:
            buttontext = "FHT";
            break;
        case siciTau:
            hastwo = true;
            if (left)
                buttontext = "UfGT";
            else
                buttontext = "MGT";
            break;
        case siciTas:
            hastwo = true;
            if (left)
                buttontext = "SGT";
            else
                buttontext = "HaGT";
            break;
        case siciTaw:
            buttontext = "WGT";
            break;
        case siciTwh:
            buttontext = "WHT";
            break;
        case siciTal:
            hastwo = true;
            if (left)
                buttontext = "Ein";
            else
                buttontext = "Aus";
            break;
        case siciHss1:
            hastwo = true;
            if (left)
                buttontext = tr("%1 Rear").arg(sSoldText);
            else
                buttontext = tr("%1 Front").arg(sSoldText);
            break;
        case siciHss3:
            hastwo = true;
            if (left)
                buttontext = tr("%1 Front").arg(sSoldText);
            else
                buttontext = tr("%1 Rear").arg(sSoldText);
            break;
        case siciSd1:
            hastwo = true;
            if (left)
                buttontext = tr("%1 Rear").arg(sSoldText);
            else
                buttontext = tr("%1 Front").arg(sSoldText);
            break;
        case siciSd3:
            hastwo = true;
            if (left)
                buttontext = tr("%1 Front").arg(sSoldText);
            else
                buttontext = tr("%1 Rear").arg(sSoldText);
            break;
        default:
            buttontext = sSoldText;
            break;
    }

    FeedbackTriggerDialog* dlg = new FeedbackTriggerDialog(this);
    if (dlg == NULL)
        return false;

    /*move dialog to mouse click point*/
    dlg->move(QCursor::pos());
    dlg->setCaption(buttontext);
    dlg->setCheckBoxText(tr("&Enable feedback trigger"));
    if (left || !hastwo) {
        dlg->enableTrigger(enable1fbtrigger);
        dlg->setFBBus(button1fbbus);
        dlg->setFBContact(button1fbcontact);
    }
    else {
        dlg->enableTrigger(enable2fbtrigger);
        dlg->setFBBus(button2fbbus);
        dlg->setFBContact(button2fbcontact);
    }
    connect(dlg, SIGNAL(sigShowFBmodules()),
            this, SIGNAL(sigShowFBmodules()));

    if (dlg->exec() == QDialog::Accepted) {
        if (left || !hastwo) {
            enable1fbtrigger = dlg->isTriggerEnabled();
            button1fbbus = dlg->getFBBus();
            button1fbcontact = dlg->getFBContact();
        }
        else {
            enable2fbtrigger = dlg->isTriggerEnabled();
            button2fbbus = dlg->getFBBus();
            button2fbcontact = dlg->getFBContact();
        }
        returnvalue = true;
        setupElementIcon();
    }
    
    disconnect(dlg, SIGNAL(sigShowFBmodules()),
            this, SIGNAL(sigShowFBmodules()));
    
    delete dlg;
    return returnvalue;
}


bool element::hasFeedbackTrigger()
{
    return (buttonCount() > 0) || (classid == siciRel);
}


/*setup property menu and keep click position */
void element::runPropertyMenue(const QPoint& p)
{
    QPopupMenu* propmenu = new QPopupMenu(this, "propertyMenu");
    int count = 0;

    if (hasLabel()) {
        propmenu->insertItem(tr("&Label..."), 1);
    }

    count = driveCount();
    if (count == 1) {
        propmenu->insertItem(tr("&Drive..."), 2);
    }
    else if (count > 1) {
        propmenu->insertItem(tr("&Drives..."), 3);
    }

    if (hasVirtualAddress()) {
        propmenu->insertItem(tr("Virtual &address..."), 4);
    }

    if (hasVariants()) {
        propmenu->insertItem(tr("&Variant..."), 5);
    }

    if (hasTrackIndicator()) {
        propmenu->insertItem(tr("&Track indicator..."), 6);
    }

    if (hasFeedbackTrigger())
        propmenu->insertItem(tr("Tri&gger..."), 7);

    if (propmenu->idAt(0) != -1) {
        int mitem = propmenu->exec(QCursor::pos());
        bool elchanged = false;

        switch(mitem) {
            case 1:
                elchanged = showLabelDialog();
                break;
            case 2:
                elchanged = showDriveDialog();
                break;
            case 3:
                elchanged = showDualDriveDialog();
                break;
            case 4:
                elchanged = showVirtualAddressDialog();
                break;
            case 5:
                elchanged = showVariantDialog();
                break;
            case 6:
                elchanged = showTrackIndicatorDialog();
                break;
            case 7:
                elchanged = showFeedbackTriggerDialog(p);
                break;
            case -1: //fall through
            default:
                break;
        }
        if (elchanged) //TODO: check
            modified = true;
    }
    delete propmenu;
}

