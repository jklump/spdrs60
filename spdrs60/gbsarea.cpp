/***************************************************************************
                           gbsarea.cpp
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2009 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2009-10-26 21:59:44 $
                           $Revision: 1.121 $
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
   this file handles loading, saving of layout files, provides an empty
   layout, sets up the elements of your layout and handles the routing
   dependant actions
 ***************************************************************************/

#include <stdlib.h>            //for free, calloc, realloc

#include <qdragobject.h>
#include <qpopupmenu.h>

#include "resources.h"
#include "gbsarea.h"

/*cursor pixmaps*/
#include "pixmaps/cursor_wgt_b.xpm"
#include "pixmaps/cursor_wgt_m.xpm"
#include "pixmaps/cursor_wht_b.xpm"
#include "pixmaps/cursor_wht_m.xpm"
#include "pixmaps/cursor_fht_b.xpm"
#include "pixmaps/cursor_fht_m.xpm"
#include "pixmaps/cursor_ufgt_b.xpm"
#include "pixmaps/cursor_ufgt_m.xpm"
#include "pixmaps/cursor_mgt_b.xpm"
#include "pixmaps/cursor_mgt_m.xpm"
#include "pixmaps/cursor_sgt_b.xpm"
#include "pixmaps/cursor_sgt_m.xpm"
#include "pixmaps/cursor_hagt_b.xpm"
#include "pixmaps/cursor_hagt_m.xpm"

#include "pixmaps/cursor_rzs_b.xpm"
#include "pixmaps/cursor_rzs_m.xpm"
#include "pixmaps/cursor_uzs_b.xpm"
#include "pixmaps/cursor_uzs_m.xpm"
#include "pixmaps/cursor_zhs_b.xpm"
#include "pixmaps/cursor_zhs_m.xpm"
#include "pixmaps/cursor_rrs_b.xpm"
#include "pixmaps/cursor_rrs_m.xpm"
#include "pixmaps/cursor_urs_b.xpm"
#include "pixmaps/cursor_urs_m.xpm"
#include "pixmaps/cursor_paint_b.xpm"
#include "pixmaps/cursor_paint_m.xpm"
#include "pixmaps/cursor_erase_b.xpm"
#include "pixmaps/cursor_erase_m.xpm"

// search options
enum {
    SRCH_TX = 0,   // search string should be in text field
    SRCH_A1,   // search string should be in address 1 field
    SRCH_A2   // search string should be in address 2 field
};



GBSArea::GBSArea(QWidget* parent, const char* name)
: QWidget(parent, name)
{
    // set all global layout variables
    gkbState = kNoneClicked;
    visualMode = kvmNormal;
    lyeditMode = lemSelect;
    paintItem = element::siciSt1;
    lastelement = NULL;
    modified = false;
    cols = 0;
    rows = 0;
    layoutid = 0;
    tablelight = true;
    layoutname = tr("Untitled");
    setPaletteBackgroundColor(QColor(Qt::lightGray));
     
    setAcceptDrops(true);
    dragging = false;
    erasing = false;
    painting = false;

    elements.setAutoDelete(true);

    // SRCP 0.8 data
    SRCP08GA1InitWalker = 0;
    SRCP08GA2InitWalker = 0;
    SRCP08BusCount = 0;
    SRCP08BusWalker = 0;
    pSRCP08BusList = NULL;
    
    /*cursor setup */
    QPixmap cb = QPixmap(cursor_wgt_b_xpm);
    QPixmap cm = QPixmap(cursor_wgt_m_xpm);
#if QT_VERSION >= 0x040000
    cb.setMask(cm.mask());
#else
    cb.setMask(*cm.mask());
#endif
    WGTCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_wht_b_xpm);
    cm = QPixmap(cursor_wht_m_xpm);
#if QT_VERSION >= 0x040000
    cb.setMask(cm.mask());
#else
    cb.setMask(*cm.mask());
#endif
    WHTCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_fht_b_xpm);
    cm = QPixmap(cursor_fht_m_xpm);
#if QT_VERSION >= 0x040000
    cb.setMask(cm.mask());
#else
    cb.setMask(*cm.mask());
#endif
    FHTCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_ufgt_b_xpm);
    cm = QPixmap(cursor_ufgt_m_xpm);
#if QT_VERSION >= 0x040000
    cb.setMask(cm.mask());
#else
    cb.setMask(*cm.mask());
#endif
    UfGTCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_mgt_b_xpm);
    cm = QPixmap(cursor_mgt_m_xpm);
#if QT_VERSION >= 0x040000
    cb.setMask(cm.mask());
#else
    cb.setMask(*cm.mask());
#endif
    MGTCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_sgt_b_xpm);
    cm = QPixmap(cursor_sgt_m_xpm);
#if QT_VERSION >= 0x040000
    cb.setMask(cm.mask());
#else
    cb.setMask(*cm.mask());
#endif
    SGTCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_hagt_b_xpm);
    cm = QPixmap(cursor_hagt_m_xpm);
#if QT_VERSION >= 0x040000
    cb.setMask(cm.mask());
#else
    cb.setMask(*cm.mask());
#endif
    HaGTCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_rzs_b_xpm);
    cm = QPixmap(cursor_rzs_m_xpm);
#if QT_VERSION >= 0x040000
    cb.setMask(cm.mask());
#else
    cb.setMask(*cm.mask());
#endif
    RZSCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_uzs_b_xpm);
    cm = QPixmap(cursor_uzs_m_xpm);
#if QT_VERSION >= 0x040000
    cb.setMask(cm.mask());
#else
    cb.setMask(*cm.mask());
#endif
    UZSCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_zhs_b_xpm);
    cm = QPixmap(cursor_zhs_m_xpm);
#if QT_VERSION >= 0x040000
    cb.setMask(cm.mask());
#else
    cb.setMask(*cm.mask());
#endif
    ZHSCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_rrs_b_xpm);
    cm = QPixmap(cursor_rrs_m_xpm);
#if QT_VERSION >= 0x040000
    cb.setMask(cm.mask());
#else
    cb.setMask(*cm.mask());
#endif
    RRSCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_urs_b_xpm);
    cm = QPixmap(cursor_urs_m_xpm);
#if QT_VERSION >= 0x040000
    cb.setMask(cm.mask());
#else
    cb.setMask(*cm.mask());
#endif
    URSCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_paint_b_xpm);
    cm = QPixmap(cursor_paint_m_xpm);
#if QT_VERSION >= 0x040000
    cb.setMask(cm.mask());
#else
    cb.setMask(*cm.mask());
#endif
    paintCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_erase_b_xpm);
    cm = QPixmap(cursor_erase_m_xpm);
#if QT_VERSION >= 0x040000
    cb.setMask(cm.mask());
#else
    cb.setMask(*cm.mask());
#endif
    eraseCursor = QCursor(cb, 0, 0);

    delayTimer = new QTimer(this);
    connect(delayTimer, SIGNAL(timeout()),
            this, SLOT(slotElementClickedTimeout()));

    toggleAction = new QAction(tr("&Toggle"), 0, this, "toggleaction");
}


GBSArea::~GBSArea()
{
    elements.clear();

    /* clear SRCP bus lists */
    if ((SRCP08BusCount > 0) && (pSRCP08BusList != NULL))
        free(pSRCP08BusList);
}


// *INDENT-OFF*
QSize GBSArea::sizeHint() const
{
    return QSize(1 + cols * (EL_WIDTH + 1), 1 + rows * (EL_HEIGHT + 1));
}
// *INDENT-ON*


void GBSArea::newFile(int newcols, int newrows, unsigned int newid,
        const QString& newname)
{
    // delete all old routes and layout data
    emit clearRoutes();
    emit updateRoutingViewer("");
    elements.clear();

    cols = newcols;
    rows = newrows;
    layoutid = newid;
    layoutname = newname;
    tablelight = true;

    elements.resize(rows * cols);
    adjustSize();
    setModified(false);
}
        

void GBSArea::writeFileTextToStream(QTextStream& ts)
{
    ts << "# start of layout section" << endl
       << "%% layout" << endl
       << "# layout dimensions=columns" << DS << "rows" << endl
       << GF_DIMENSIONS << DS << cols << DS << rows << endl
       << GF_ID << DS << layoutid << DS << layoutname << endl
       << GF_TABLELIGHT << DS << (tablelight ? 1 : 0) << endl
       << "# start of element section" << endl;
       //<< "# elements=" << elements.count() << endl;
    
    for (unsigned int i = 0; i < elements.size(); i++) {
        element* el = elements[i];
        if (el != NULL) {
            ts << "%% element " << i << endl;
            el->writeFileTextToStream(ts);
        }
    }
    setModified(false);
}


void GBSArea::readFileTextFromStream(QTextStream& ts)
{
    /*clear old element list*/
    if (!elements.isEmpty()) {
        emit clearRoutes();
        elements.clear();
    }
    /*set default value to make light switchable*/
    tablelight = true;
    
    QString s, key, value;
    unsigned int ecount = 0;
    bool tablelighton = true;

    while (!ts.eof()) {
        s = ts.readLine();

        /* ignore comment lines */
        if (s.startsWith("#"))
            continue;

        /* read layout dimensions */ 
        /* key/value pairs are read sequence independent */
        key = s.section(DS, 0, 0);
        value = s.section(DS, 1, 1).stripWhiteSpace();

        if (key.compare(GF_DIMENSIONS) == 0) {
            cols = value.toInt();
            value = s.section(DS, 2, 2).stripWhiteSpace();
            rows = value.toInt();
            ecount = cols * rows;
            elements.resize(ecount);
            adjustSize();
        }
        else if (key.compare(GF_ID) == 0) {
            layoutid = value.toUInt();
            layoutname = s.section(DS, 2).stripWhiteSpace();
        }
        else if (key.compare(GF_TABLELIGHT) == 0) {
            tablelighton = (value.toInt() == 1);
        }

        /*here we read allways up to start marker of a new route*/
        else if (s.startsWith("%% element")) {

            element* el = new element(ts, this);
            if (el != NULL) {
                unsigned int idx = el->getIndexNo();

                if (idx < ecount) {
                    moveElementToIndexPos(el, idx);
                    elements.insert(idx, el);
                    connectElement(el);
                    el->show();
                }
                else {
                    qWarning("Error: Element outside of layout found "
                            "(Index = %d).", idx);
                    delete el;
                }
                //qWarning("Element number %d inserted.", el->getIndexNo());
            }
        }
        else if (s.startsWith("%% route"))
            break;
        //TODO:
        /*else if (s.startsWith("%% fbsection"))
            break;*/
    }

    /*switch table light only off if this state was saved, default is on*/
    if (!tablelighton)
        switchTableLight(false);
    
    setModified(false);
}


void GBSArea::slotFHTclicked()
{
    externalButtonClicked(kFhtClicked);
}


void GBSArea::slotFRTclicked()
{
    externalButtonClicked(kFrtClicked);
}


void GBSArea::slotWGTclicked()
{
    externalButtonClicked(kWgtClicked);
}


void GBSArea::slotUfGTclicked()
{
    externalButtonClicked(kUfgtClicked);
}


void GBSArea::slotMGTclicked()
{
    externalButtonClicked(kMgtClicked);
}


void GBSArea::slotSGTclicked()
{
    externalButtonClicked(kSgtClicked);
}


void GBSArea::slotHaGTclicked()
{
    externalButtonClicked(kHagtClicked);
}


void GBSArea::externalButtonClicked(GbsButtonState externalButton)
{
    /*
     * store pressed control button, activate cursor and start timer
     */
    delayTimer->start(cDelayTime);
    gkbState = externalButton;
    
    switch (externalButton) {
        case kFhtClicked:
            setCursor(FHTCursor);
            break;
        case kMgtClicked:
            setCursor(MGTCursor);
            /*TODO: implement MGT-function*/
            QApplication::beep();
            emit statusMessage(tr("MGT-Function not supported."));
            delayTimer->start(500);
            break;
        case kUfgtClicked:
            setCursor(UfGTCursor);
            break;
        case kWgtClicked:
            setCursor(WGTCursor);
            break;
        case kWhtClicked:
            setCursor(WHTCursor);
            break;
        case kSgtClicked:
            setCursor(SGTCursor);
            break;
        case kHagtClicked:
            setCursor(HaGTCursor);
            break;
        default:
            break;
    }
}


void GBSArea::slotElementClicked(element* el, GbsButtonState gbsButton)
{
    /* stop running timer but do not change cursor shape until we know
     * what exactly happens next */

    if (delayTimer->isActive())
        delayTimer->stop();

    switch (gbsButton) {
        
        /*external control buttons*/
        case kFhtClicked:
        case kFrtClicked:
        case kHagtClicked:
        case kMgtClicked:
        case kUfgtClicked:
        case kWgtClicked:
        case kWhtClicked:
        case kSgtClicked:
            externalButtonClicked(gbsButton);
            break;
            
        /*signal buttons*/
        case kRfsClicked:
        case kZfsClicked:
        case kZhsClicked:
            if (kSgtClicked == gkbState || kHagtClicked == gkbState) {
                if (el->isLocked()) {
                    QApplication::beep();
                    emit statusMessage(tr("No switching possible, "
                                "signal '%1' is locked by an active route.")
                            .arg(el->getLabelText()));
                }
                else
                    if (kSgtClicked == gkbState)
                        el->toggle();
                    else
                        el->switchToDir(0);
                slotElementClickedTimeout();
            }
            else if (kFhtClicked == gkbState) {
                // give direct button pressed feedback to user
                setCursor(Qt::ArrowCursor);
                /* send signal to route controller*/
                emit resetRoute(el, gbsButton);
            }
            else if (kUfgtClicked == gkbState || kNoneClicked ==
                    gkbState) {
                if (el->hasFfMLock()) {
                    QApplication::beep();
                    emit statusMessage(tr("No routing possible; signal '%1'"
                                " is already locked by an active route.")
                            .arg(el->getLabelText()));
                    slotElementClickedTimeout();
                }
                else {
                    // give button pressed feedback to user
                    setCursor(Qt::ArrowCursor);
                    /* send signal to route controller*/
                    emit setRoute(el, gbsButton, gkbState);
                }
            }
            else if (kWgtClicked == gkbState) {
                QApplication::beep();
                emit statusMessage(tr("Signals can not be switched "
                            "using WGT"));
                slotElementClickedTimeout();
            }
            else {
                QApplication::beep();
                emit statusMessage(tr("Operation not allowed"));
                slotElementClickedTimeout();
            }
            break;
            
        /*turnout button*/
        case kTurnoutClicked:
            if (kWgtClicked == gkbState) {
                if (el->isLocked()) {
                    QApplication::beep();
                    emit statusMessage(tr("No switching possible, "
                                "solenoid '%1' is locked by an active route.")
                            .arg(el->getLabelText()));
                }
                else if (el->isOccupied()) {
                    QApplication::beep();
                    emit statusMessage(tr("No switching possible, "
                                "turnout is occupied"));
                }
                else
                    el->toggle();
            }

            else if (kWhtClicked == gkbState) {
                if (el->isLocked()) {
                    QApplication::beep();
                    emit statusMessage(tr("No switching possible, "
                                "solenoid '%1' is locked by an active route.")
                            .arg(el->getLabelText()));
                }
                else
                    el->toggle();
            }

            else if (kFhtClicked == gkbState) {
                QApplication::beep();
                emit statusMessage(tr("Turnouts can not be switched "
                            "using FHT"));
            }

            else if (kSgtClicked == gkbState) {
                QApplication::beep();
                emit statusMessage(tr("Turnouts can not be switched "
                            "using SGT"));
            }

            else if (kUfgtClicked == gkbState) {
                QApplication::beep();
                emit statusMessage(tr("Turnouts can not be switched "
                            "using UfGT"));
            }

            else {
                QApplication::beep();
                emit statusMessage(tr("Operation not allowed"));
            }
            slotElementClickedTimeout();
            break;

        /*light on button*/
        case kEinClicked:
            switchTableLight(true);
            break;

        /*light off button*/
        case kAusClicked:
            switchTableLight(false);
            break;

        default:
            break;
    }
}


void GBSArea::startRouteTimer(Route::RouteType tor)
{
    switch (tor) {
        case Route::rtRZS:
            setCursor(RZSCursor);
            break;
        case Route::rtUZS:
            setCursor(UZSCursor);
            break;
        case Route::rtZHS:
            setCursor(ZHSCursor);
            break;
        case Route::rtRRS:
            setCursor(RRSCursor);
            break;
        case Route::rtURS:
            setCursor(URSCursor);
            break;
    }
    delayTimer->start(cDelayTime);
}


void GBSArea::slotElementClickedTimeout()
{
    emit resetSelectedSignal();
    gkbState = kNoneClicked;
    setCursor(Qt::ArrowCursor);
}


void GBSArea::updateRoutePathLEDs(const stateElement& fSig,
        const stateElement& tSig, Route::RouteSetAction& setRoute)
{
    if (fSig.elemPtr == NULL || tSig.elemPtr == NULL)
        return;

    if (rows <= 1)
        return;

    int idx = fSig.elemPtr->getIndexNo();
    int maxIdx = (int) elements.size();
    bool finished = false;
    bool setrt = (Route::rsaReset != setRoute);
    
    unsigned int entrydir;
    unsigned int exitdir;

    entrydir = fSig.elemPtr->entryDir();

    while (!finished) {
        if ((idx < 0) || (idx >= maxIdx)) {
            emit statusMessage(tr("Route hit layout edge at element %1")
                    .arg(idx));
            break;
        }
        
        element* rel = elements[idx];
        if (rel == NULL) {
            emit statusMessage(tr("Route end at empty element %1")
                    .arg(idx));
            break;
        }

        if (!rel->isRoutable()) {
            emit statusMessage(tr("Route end at not routable element %1")
                    .arg(idx));
            break;
        }

        // interrupt operation when normal route meets occupied element
        // start signal is allowed to be occupied
        // give also feedback for caller
        if ((Route::rsaZfs == setRoute) && rel->isOccupied() &&
                rel != fSig.elemPtr) {
            setRoute = Route::rsaReset;
            return;
        }

        // paint or remove yellow track and get back new route direction
        // rel->setRouted(setrt);
        // rel->getExitDirection(entrydir);
        exitdir = rel->routeElement(entrydir, setrt);
        
        if (exitdir == rdCenter) {
            emit statusMessage(tr("Route found dead end at element %1")
                    .arg(idx));
            break;
        }

        entrydir = rdCenter;
        
        if (exitdir & rdN) {
            --idx;
            entrydir = entrydir | rdS;
        }
        else if (exitdir & rdS) {
            ++idx;
            entrydir = entrydir | rdN;
        }
        if (exitdir & rdW) {
            idx -= rows;
            entrydir = entrydir | rdE;
        }
        else if (exitdir & rdE) {
            idx += rows;
            entrydir = entrydir | rdW;
        }

        finished = (rel == tSig.elemPtr);
    }
}


void GBSArea::slotEditFind(const QString& ftext, int type, int multiple)
{
    bool found = findElement(ftext, type, multiple);

    // no element could be located -> show this information
    if (!found)
        QMessageBox::information(this, tr("Locate error"),
                                 tr("There's no element which\n"
                                    "matches your search criteria."));
}


/**
 * Search all elements for a matching address or text label
 */
bool GBSArea::findElement(const QString& ftext, int ftype, int fmulti)
{
    QString s = "";
    unsigned int idx;
    bool returnvalue = false;

    // locate element with certain address 1
    if (ftype == SRCH_A1)
        for (idx = 0; idx < elements.size(); idx++) {
            element* el = elements[idx];

            if (el == NULL)
                continue;

            if (ftext.toInt() == el->getAddress1()) {
                returnvalue = true;
                el->locateMe();
                if (fmulti == 1)
                    continue;
                else
                    return returnvalue;
            }
        }

    // locate element with certain address 2
    else if (ftype == SRCH_A2)
        for (idx = 0; idx < elements.size(); idx++) {
            element* el = elements[idx];

            if (el == NULL)
                continue;

            if (ftext.toInt() == el->getAddress2()) {
                returnvalue = true;
                el->locateMe();
                if (fmulti == 1)
                    continue;
                else
                    return returnvalue;
            }
        }

    // locate element with certain textfield
    else if (ftype == SRCH_TX)
        for (idx = 0; idx < elements.size(); idx++) {
            element* el = elements[idx];

            if (el == NULL)
                continue;

            s = el->sSoldText;
            if (s.contains(ftext, 0)) {
                returnvalue = true;
                el->locateMe();
                if (fmulti == 1)
                    continue;
                else
                    return returnvalue;
            }
        }
    return returnvalue;
}


/**
 * Toggle all elements but no couplers, motors no shifting bridges,
 * no turntables
 **/
void GBSArea::slotToggleAll()
{
    for (unsigned int i = 0; i < elements.size(); i++) {
        element* el = elements[i];

        if (el != NULL &&
                el->classId() != element::siciEnk &&
                el->classId() != element::siciMdc &&
                el->classId() != element::siciSbn &&
                el->classId() != element::siciDre)
            el->toggle();
    }
}


/**
 *  Send current state off all element to SRCP server
 **/
void GBSArea::slotSendAll()
{
    for (unsigned int i = 0; i < elements.size(); i++) {
        element* el = elements[i];

        if (el != NULL &&
                el->classId() != element::siciEnk &&
                el->classId() != element::siciMdc &&
                el->classId() != element::siciSbn &&
                el->classId() != element::siciDre)
            el->sendSrcpState();
    }
}


/**
 * set all signals to red state
 * */
void GBSArea::slotNotrot()
{
    for (unsigned int i = 0; i < elements.size(); i++) {
        element* el = elements[i];

        if (el != NULL && el->isSignal())
            el->switchToDir(0);
    }
    
    emit statusMessage(tr("Switched all signals to halt/stop"));
}


/**
 * connect element to gbs events
 * 
 * disconnect is done automatically by the Qt library when element
 * is deleted
 */
void GBSArea::connectElement(element* el)
{
    if (el == NULL)
        return;

    connect(el, SIGNAL(elementClicked(element*, GbsButtonState)),
            this, SLOT(slotElementClicked(element*, GbsButtonState)));
    connect(el, SIGNAL(sigShowFBmodules()),
            this, SIGNAL(sigShowFBmodules()));

    connect(this, SIGNAL(switchedVisualMode(elemVisualMode)),
            el, SLOT(switchVisualMode(elemVisualMode)));
    connect(this, SIGNAL(sigShowElement(int, int,
                    elemSelectionMode)),
            el, SLOT(slotShowElement(int, int, elemSelectionMode)));
    connect(this, SIGNAL(sigRepaintLayout()),
            el, SLOT(slotRepaintLayout()));
    connect(el, SIGNAL(recordElement(element*, elemRecordType)),
            this, SIGNAL(recordElement(element*, elemRecordType)));
    connect(this, SIGNAL(processInfoPortMessage(unsigned int,
                    unsigned int, unsigned int, unsigned int)),
            el, SLOT(processInfoPortMessage(unsigned int,
                    unsigned int, unsigned int, unsigned int)));
    connect(el, SIGNAL(sendSrcpMessage(SrcpMessage*)),
            this, SIGNAL(sendSrcpMessage(SrcpMessage*)));
    connect(this, SIGNAL(feedbackPortChanged(unsigned int,
                    unsigned int, bool)),
            el, SLOT(slotOccupyElement(unsigned int,
                    unsigned int, bool)));
}


bool GBSArea::isModified()
{
    return modified;
}


void GBSArea::setModified(bool m)
{
    if (modified != m)
        modified = m;

    //TODO: check this, should be updated if element was
    //  - removed
    //  - added
    //  - edited
    updateSRCP08BusList();
}


/*return number of layout columns*/
int GBSArea::getColumns()
{
    return cols;
}


/*return number of layout rows*/
int GBSArea::getRows()
{
    return rows;
}


/*set new layout size*/
void GBSArea::setLayoutSize(int newcols, int newrows)
{
    if (newcols < 1)
        newcols = 1;

    if (newrows < 1)
        newrows = 1;

    if (newcols > MAX_COLS)
        newcols = MAX_COLS;

    if (newrows > MAX_ROWS)
        newrows = MAX_ROWS;
 
    QPtrVector<element> tmpelements;

    tmpelements.resize(newcols * newrows);
    
    /* delete elements outside new gbs area */
    if (newcols < cols) {
        for (int i = 0; i < (cols - newcols); i++) {
            removeColumnElements(cols - i);
        }
    }

    if (newrows < rows) {
        for (int i = 0; i < (rows - newrows); i++) {
            removeRowElements(rows - i);
        }
    }

    /* rearrange elements in altered gbs */
    for (int c = 1; c <= newcols; c++) {
        for (int r = 1; r <= newrows; r++) {
            element* el = item(r, c);

            if (el != NULL) {
                unsigned int idx = newrows * (c - 1) + r - 1;
                el->setIndexNo(idx);
                tmpelements.insert(idx, el);
            }
        }
    }

    elements.setAutoDelete(false);
    elements = tmpelements;
    elements.setAutoDelete(true);
    
    cols = newcols;
    rows = newrows;

    adjustSize();
    setModified(true);
}


/*set new layout id*/
void GBSArea::setLayoutId(unsigned int newid)
{
    if (layoutid != newid) {
        layoutid = newid;
        modified = true;
    }
}


/*return layout id*/
unsigned int GBSArea::getLayoutId()
{
    return layoutid;
}


/*set new layout name*/
void GBSArea::setLayoutName(const QString& newname)
{
    if (layoutname != newname) {
        layoutname = newname;
        modified = true;
    }
}


/*return layout name*/
QString GBSArea::getLayoutName() const
{
    return layoutname;
}


/*remove all elements of a single column*/
void GBSArea::removeColumnElements(int col)
{
    for (int r = 1; r <= rows; r++)
        elements.remove(indexOf(r, col));
}

/*remove all elements of a single row*/
void GBSArea::removeRowElements(int row)
{
    for (int c = 1; c <= cols; c++)
            elements.remove(indexOf(row, c));
}

/* 
 *  row is defined from 1 to rows
 *  col is defined from 1 to cols
 */
// *INDENT-OFF*
unsigned int GBSArea::indexOf(int row, int col) const
{
    return (rows * (col - 1) + row - 1); 
}
// *INDENT-ON*


// *INDENT-OFF*
unsigned int GBSArea::indexOf(const QPoint& ep) const
{
    int row = ep.y() / (EL_HEIGHT + 1) + 1;
    int col = ep.x() / (EL_WIDTH + 1) + 1;
    return (rows * (col - 1) + row - 1); 
}
// *INDENT-ON*


void GBSArea::moveElementToIndexPos(element* el, unsigned int idx)
{
    if (el != NULL && idx < elements.size())
        el->move(1 + (idx / rows) * (EL_WIDTH + 1),
                1 + (idx % rows) * (EL_HEIGHT + 1));
}


// *INDENT-OFF*
element* GBSArea::item(int row, int col) const
// *INDENT-ON*
{
    if (row < 0 || col < 0 || row > rows ||
            col > cols || (unsigned int)row * col >= elements.size())
        return NULL;

    return elements[indexOf(row, col)];
}


QPtrVector<element>* GBSArea::getGbsElementListPtr()
{
    return &elements;
}


void GBSArea::sendInfoPortMessage(unsigned int bus,
        unsigned int addr, unsigned int port, unsigned int value)
{
    /* send incomming GA actions to all elements*/
    emit processInfoPortMessage(bus, addr, port, value);
}

/*used by route dialog to look for element names*/
void GBSArea::getElementByAddress(const int bus, const int address,
        element** el)
{
    for (unsigned int i = 0; i < elements.size(); i++) {
        element* gbse = elements[i];

        if (gbse != NULL && gbse->hasSameAddress(bus, address)) {
            *el = gbse;
            break;
        }
    }
}


bool GBSArea::runSRCP08GAInitSequence()
{
    bool returnvalue = false;
    bool CounterChanged = false;

    for (unsigned int i = SRCP08GA1InitWalker; i < elements.size(); i++) {

        element* el = elements[i];
        if (el == NULL)
            continue;

        if (!el->isSwitchable())
            continue;

        if (el->sendSRCP08InitGA(1)) {
            SRCP08GA1InitWalker = i;
            SRCP08GA1InitWalker++;
            returnvalue = (SRCP08GA1InitWalker < elements.size());
            CounterChanged = true;
            break;
        }
    }

    if (!returnvalue && !CounterChanged)
        for (unsigned int i = SRCP08GA2InitWalker; i < elements.size(); i++) {

            element* el = elements[i];
            if (el == NULL)
                continue;

            if (!el->isSwitchable() || el->getAddressCount() != 2)
                continue;

            if (el->sendSRCP08InitGA(2)) {
                SRCP08GA2InitWalker = i;
                SRCP08GA2InitWalker++;
                returnvalue = (SRCP08GA2InitWalker < elements.size());
                break;
            }
        }

    /* reset when init process for all GAs is finished */
    if (!returnvalue) {
        SRCP08GA1InitWalker = 0;
        SRCP08GA2InitWalker = 0;
    }

    return returnvalue;
}


/*
 * ask server about power status off every single bus, but only one at
 * a time return true while there are unasked busses left
 */
bool GBSArea::sendSRCP08BusMessage(SrcpMessage::Message smt)
{
    bool WalkerChanged = false;

    /*
     * all busses are asked step by step, allways waiting
     * for server OK
     */
    if ((SRCP08BusCount > 0)
        && (SRCP08BusWalker < SRCP08BusCount)) {

        //SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgPowerGet);
        SrcpMessage sm = SrcpMessage(smt);
        sm.setBus(pSRCP08BusList[SRCP08BusWalker].id);
        emit sendSrcpMessage(&sm);

        SRCP08BusWalker++;
        WalkerChanged = true;
    }

    /* reset when this process is finished for all busses */
    if (!WalkerChanged)
        SRCP08BusWalker = 0;

    return WalkerChanged;
}


/*
 * switch on every single bus, but only one at a time
 * return true while there are unswitched busses left
 */
bool GBSArea::setSRCP08BusPower(bool setPowerOn)
{
    bool CounterChanged = false;

    /*
     * all busses are switched step by step, allways waiting
     * for server OK
     */
    if ((SRCP08BusCount > 0)
        && (SRCP08BusWalker < SRCP08BusCount)) {
        SrcpMessage sm = SrcpMessage(SrcpMessage::msgPowerSet);
        sm.setPowerData(pSRCP08BusList[SRCP08BusWalker].id,
                setPowerOn);
        emit sendSrcpMessage(&sm);

        SRCP08BusWalker++;
        CounterChanged = true;
    }

    /* reset when init process for all busses is finished */
    if (!CounterChanged)
        SRCP08BusWalker = 0;

    return CounterChanged;
}


void GBSArea::updateSRCP08BusList()
{
    unsigned int count = 0, busno = 0;
    bool busNoIsKnown;
    SrcpBus *tempbuslist;

    /*how many different GA busses do we have? */
    for (unsigned int i = 0; i < elements.size(); i++) {

        element* el = elements[i];
        if (el == NULL)
            continue;

        for (int k = 0; k <= 2; k++) {
            switch (k) {
                case 0:
                    busno = el->getGA1BusNo();
                    break;
                case 1:
                    busno = el->getGA2BusNo();
                    break;
                case 2:
                    busno = el->getFBBusNo();
                    break;
            }

            if (busno > 0) {
                if (count == 0) {
                    count++;

                    pSRCP08BusList = (SrcpBus *) calloc(count,
                            sizeof(SrcpBus));
                    if (pSRCP08BusList == NULL) {
                        qWarning("Memory allocation error!");
                        SRCP08BusCount = count - 1;
                        return;
                    }
                    pSRCP08BusList[0].id = busno;
                    pSRCP08BusList[0].hasFb = false;
                    pSRCP08BusList[0].hasGa = false;

                    if (k == 2)
                        pSRCP08BusList[0].hasFb = true;
                    else 
                        pSRCP08BusList[0].hasGa = true;
                }
                /*count > 0 */
                else {
                    busNoIsKnown = false;
                    for (unsigned int j = 0; j < count; j++) {
                        if (pSRCP08BusList[j].id == busno) {
                            busNoIsKnown = true;
                            if (k == 2)
                                pSRCP08BusList[count - 1].hasFb = true;
                            else 
                                pSRCP08BusList[count - 1].hasGa = true;
                            break;
                        }
                    }
                    if (!busNoIsKnown) {
                        count++;

                        tempbuslist =
                            (SrcpBus *) realloc(pSRCP08BusList,
                                            sizeof(SrcpBus) * count);

                        if (tempbuslist == NULL) {
                            qWarning("Memory allocation error!");
                            SRCP08BusCount = count - 1;
                            return;
                        }
                        pSRCP08BusList = tempbuslist;
                        pSRCP08BusList[count - 1].id = busno;
                        pSRCP08BusList[count - 1].hasFb = false;
                        pSRCP08BusList[count - 1].hasGa = false;

                        if (k == 2)
                            pSRCP08BusList[count - 1].hasFb = true;
                        else 
                            pSRCP08BusList[count - 1].hasGa = true;
                    }
                }
            }   /* for busno */
        }       /* for k */
    }           /* for i */

    if (count == 0)
        statusMessage(tr("Layout does not contain a SRCP bus"
                    " configuration"));
    else if (count == 1)
        statusMessage(tr("Layout contains 1 configured bus"));
    else
        statusMessage(tr("Layout contains %1 configured busses").
                   arg(count));

    SRCP08BusCount = count;
}


bool GBSArea::hasSrcp08Bus(unsigned int bus)
{
    bool returnvalue = false;

    if (SRCP08BusCount > 0) 
        for (unsigned int i = 0; i < SRCP08BusCount; i++)
            if (bus == pSRCP08BusList[i].id) {
                returnvalue = true;
                break;
            }

    return returnvalue;
}

/*setup property menu and keep click position */
void GBSArea::runPropertyMenue(element* el, const QPoint& p)
{
    QPopupMenu* propmenu = new QPopupMenu(this, "propertyMenu");
    int count = 0;

    if (el->hasLabel()) {
        propmenu->insertItem(tr("&Label..."), 1);
    }

    count = el->driveCount();
    if (count == 1) {
        propmenu->insertItem(tr("&Drive..."), 2);
    }
    else if (count > 1) {
        propmenu->insertItem(tr("&Drives..."), 3);
    }

    if (el->hasVirtualAddress()) {
        propmenu->insertItem(tr("Virtual &address..."), 4);
    }

    if (el->hasVariants()) {
        propmenu->insertItem(tr("&Variant..."), 5);
    }

    if (el->hasTrackIndicator()) {
        propmenu->insertItem(tr("&Track indicator..."), 6);
    }

    if (el->hasFeedbackTrigger())
        propmenu->insertItem(tr("Tri&gger..."), 7);

    if (propmenu->idAt(0) != -1) {
        int mitem = propmenu->exec(QCursor::pos());
        bool elchanged = false;

        switch(mitem) {
            case 1:
                elchanged = el->showLabelDialog();
                break;
            case 2:
                elchanged = el->showDriveDialog();
                break;
            case 3:
                elchanged = el->showDualDriveDialog();
                break;
            case 4:
                elchanged = el->showVirtualAddressDialog();
                break;
            case 5:
                elchanged = el->showVariantDialog();
                break;
            case 6:
                elchanged = el->showTrackIndicatorDialog();
                break;
            case 7:
                elchanged = el->showFeedbackTriggerDialog(p);
                break;
            case -1: //fall through
            default:
                break;
        }
        if (elchanged)
            modified = true;
    }
    delete propmenu;
}


void GBSArea::mouseReleaseEvent(QMouseEvent* e)
{
    /*normal mode*/
    if (visualMode == kvmNormal)
        e->ignore();

    /*layout edit mode*/
    else if (visualMode == kvmEditLayout) {
        if (e->button() == Qt::LeftButton) {

            if (lyeditMode == lemSelect) {
            /*TODO: handle drop action*/
            }

            else if (lyeditMode == lemErase) {
                erasing = false;
            }

            else if (lyeditMode == lemPaint) {
                painting = false;
            }

            e->accept();
        }

        else if (e->button() == Qt::MidButton) {
            // nothing happens here
        }

        else if (e->button() == Qt::RightButton) {
            element* el = (element*)childAt(e->pos());

            if (el == NULL) {
                e->accept();
                return;
            }

            runPropertyMenue(el, e->pos());

            e->accept();
        }

    }

    /*route edit mode*/
    else if (visualMode == kvmEditRoute) {
        /*show context menu to switch element only without selection*/
        if (e->button() == Qt::RightButton) {
            element* el = (element*)childAt(e->pos());

            if (el != NULL && el->isSwitchable()) {
                toggleAction->setEnabled(el->ctxCanSwitch());

                QPopupMenu menu;
                toggleAction->addTo(&menu);

                if (menu.exec(QCursor::pos()) != -1)
                    el->toggle();
            }
            e->accept();
        }
    }
}

/**
 * start dragging of an element
 * */
void GBSArea::mousePressEvent(QMouseEvent* e)
{
    if (visualMode == kvmNormal) {
        if (e->button() == Qt::LeftButton) {
            element* el = (element*)childAt(e->pos());
            if (el != NULL) {
                if (el->classId() == element::siciMdc) {
                    //TODO: create motor commander
                    //if (!el->hasCommander()) {
                    //  emit createMotorCommander(el);
                    //  }
                    e->accept();
                }
                else if (el->classId() == element::siciSbn) {
                    //TODO: create shifting bridge commander
                    e->accept();
                }
                else if (el->classId() == element::siciDre) {
                    //TODO: create turn table commander
                    e->accept();
                }
            }
        }
    }
    /*layout edit mode*/
    else if (visualMode == kvmEditLayout) {
        if (e->button() == Qt::LeftButton) {

            /* drag item */
            if (lyeditMode == lemSelect) {
                element* el = (element*)childAt(e->pos());
                if (el != NULL)
                    dragging = true;
            }

            /* erase item */
            else if (lyeditMode == lemErase) {
                element* el = (element*)childAt(e->pos());
                unsigned int idx = indexOf(e->pos());

                if (el != NULL) {
                    elements.remove(idx);
                    modified = true;
                }
                erasing = true;
            }

            /* paint item */
            else if (lyeditMode == lemPaint) {
                element* el = (element*)childAt(e->pos());
                unsigned int idx = indexOf(e->pos());

                if (el != NULL) {
                    if (el->classId() != paintItem) {
                        elements.remove(idx);
                        el = new element(this, paintItem);
                        el->setIndexNo(idx);
                        moveElementToIndexPos(el, idx);
                        elements.insert(idx, el);
                        connectElement(el);
                        el->show();
                        modified = true;
                    }
                }
                else {
                    el = new element(this, paintItem);
                    el->setIndexNo(idx);
                    moveElementToIndexPos(el, idx);
                    elements.insert(idx, el);
                    connectElement(el);
                    el->show();
                    modified = true;
                }
                painting = true;
            }
            e->accept();
        }
    }
}

/**
 * take element icon and its date, move it around
 */
void GBSArea::mouseMoveEvent(QMouseEvent* e)
{
    /*layout edit mode*/
    if (visualMode == kvmEditLayout) {
        /* drag elements */
        if ((lyeditMode == lemSelect) && dragging) {
            element* el = (element*)childAt(e->pos());
            if (el != NULL) {
                unsigned int idx = el->getIndexNo();
                QByteArray data(sizeof(idx));
                memcpy(data.data(), &idx, sizeof(idx));
                QStoredDrag* d = new QStoredDrag(MIME_LE, this, "spdrs60-le");
                d->setEncodedData(data);

                if (NULL != el->paletteBackgroundPixmap())
                    d->setPixmap(*el->paletteBackgroundPixmap(),
                            QPoint(EL_WIDTH / 2, EL_HEIGHT / 2));
                else {
                    QPixmap pm = QPixmap::grabWidget(el);
                    d->setPixmap(pm, QPoint(EL_WIDTH / 2, EL_HEIGHT / 2));
                }

                d->dragMove();
                dragging = false;
            }
        }

        /* erase elements */
        else if ((lyeditMode == lemErase) && erasing) {
                element* el = (element*)childAt(e->pos());

                if (el != NULL) {
                    unsigned int idx = indexOf(e->pos());
                    elements.remove(idx);
                    modified = true;
                }
        }
        
        /* paint elements */
        else if ((lyeditMode == lemPaint) && painting) {
                unsigned int idx = indexOf(e->pos());

                if (idx > (unsigned int)(rows * cols))
                    return;

                element* el = (element*)childAt(e->pos());

                if (el != NULL) {
                    if (el->classId() != paintItem) {
                        elements.remove(idx);
                        el = new element(this, paintItem);
                        el->setIndexNo(idx);
                        moveElementToIndexPos(el, idx);
                        elements.insert(idx, el);
                        connectElement(el);
                        el->show();
                        modified = true;
                    }
                }
                else {
                    el = new element(this, paintItem);
                    el->setIndexNo(idx);
                    moveElementToIndexPos(el, idx);
                    elements.insert(idx, el);
                    connectElement(el);
                    el->show();
                    modified = true;
                }
        }
        e->accept();
    }
}

/**
 * give feedback if this widget cares about the offered data
 * */
void GBSArea::dragMoveEvent(QDragMoveEvent* e)
{
    if (visualMode == kvmEditLayout) {
        //TODO: enable DnD between different windows
        if (e->provides(MIME_LE) && e->source() == this)
            e->accept();

        else if (e->provides(MIME_FBC)) {
            element* el = (element*)childAt(e->pos());
            
            if (el == NULL) {
                if (lastelement != NULL) {
                    lastelement->switchSelectionMode(ksmNormal);
                    lastelement = NULL;
                    e->ignore();
                }  
                return;
            }
            
            if (el->canReceiveFbcDrop()) {
                if (lastelement != NULL) {
                    if (lastelement != el) {
                        lastelement->setDropTargetView(false);
                        lastelement = NULL;
                        el->setDropTargetView(true);
                        lastelement = el;
                        e->accept();
                    }
                }
                else {
                    el->setDropTargetView(true);
                    lastelement = el;
                    e->accept();
                }
            }
            else {
                if (lastelement != NULL) {
                    lastelement->setDropTargetView(false);
                    lastelement = NULL;
                    e->ignore();
                }  
            }
        }
    }
}

/**
 * take the dropped data and do something usefull with it
 * */
void GBSArea::dropEvent(QDropEvent *e)
{
    if (visualMode == kvmEditLayout) {

        if (e->provides(MIME_LE)) {
            // decode data and insert element
            QByteArray data = e->encodedData(MIME_LE);
            unsigned int idx = 0;

            if (data.size() != sizeof(idx))
                return;

            memcpy(&idx, data.data(), sizeof(idx));

            // move element from old position to new position
            element* el = elements.take(idx);
            if (el != NULL) {
                unsigned int pidx = indexOf(e->pos());
                el->setIndexNo(pidx);
                moveElementToIndexPos(el, pidx);
                elements.insert(pidx, el);
                modified = true;
            }
            e->accept();
        }

        else if (e->provides(MIME_FBC)) {
            if (lastelement != NULL) {
                lastelement->setDropTargetView(false);
                lastelement = NULL;
            }  
            element* el = (element*)childAt(e->pos());
            if (el != NULL && el->canReceiveFbcDrop()) {
                QByteArray data = e->encodedData(MIME_FBC);
                el->setDroppedFbContact(data);
                modified = true;
            }
            e->accept();
        }
    }
}


void GBSArea::switchVisualMode(elemVisualMode vm)
{
    if (vm != visualMode) {
        visualMode = vm;
        emit switchedVisualMode(vm);
        update();
    }
}

/* paint colored lines */
void GBSArea::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    QColor c;

    /* paint visual mode lines*/
    switch (visualMode) {
        case kvmEditLayout:
            // edit mode: red
            c = QColor(Qt::red);
            break;
        case kvmEditRoute:
            // show route mode: blue
            c = QColor(Qt::blue);
            break;
        default:
            // normal mode: grey
            c = QColor(Qt::gray);
            break;
    }
    
    p.setPen(c);
    int h = height();
    int w = width();
    
    for (int x = 0; x < w; x += (EL_WIDTH + 1))
        p.drawLine(x, 0, x, h - 1);
    
    for (int y = 0; y < h; y += (EL_HEIGHT + 1))
        p.drawLine(0, y, w - 1, y);
}

/*
 * change current edit mode
 */
void GBSArea::changeLayoutEditMode(LayoutEditMode lem)
{
    if (lem != lyeditMode)
        lyeditMode = lem;

    switch(lem) {
        case lemSelect:
            setCursor(Qt::ArrowCursor);
            break;
        case lemPaint:
            setCursor(paintCursor);
            break;
        case lemErase:
            setCursor(eraseCursor);
            break;
    }
}

/*
 * change current paint item
 */
void GBSArea::changeLayoutPaintItem(element::SpdrItemClassId sici)
{
    if (sici != paintItem)
        paintItem = sici;
}


/*
 * respond to incomming Generic Messages
 */
void GBSArea::processGenericMessage(unsigned int sendto,
        unsigned int replyto, const CrcfMessage* cm)
{
    QString cms = "";

    if (NULL == cm)
        return;

    if (cm->getActorId() != layoutid)
        return;

    switch (cm->getMethod()) {
        case CrcfMessage::meGet:
            /* LAYOUT <layoutid> GET <attribute> */
            cms = getCrcfInfoMessage(cm->getAttribute());
            if (cms.isEmpty()) 
                emit statusMessage(tr("Unsupported CRCF attribute '%1' "
                            "detected.").arg(cm->getAttributeStr()));
            else
                sendGmCrcfMessage(sendto, replyto, cms);
            break;

        case CrcfMessage::meSet:
            /* LAYOUT <layoutid> SET <attribute> <att_value> */

            switch (cm->getAttribute()) {

                /* LAYOUT <layoutid> SET COLUMNS <att_value> */
                case CrcfMessage::atColumns:
                    if (visualMode == kvmEditLayout) {
                        int cc = cm->getAttValue();
                        if ((cc != cols) && (cc > 0) && (cc <= MAX_COLS)) {
                            setLayoutSize(cc, rows);
                            cms = getCrcfInfoMessage(CrcfMessage::atColumns);
                            if (!cms.isEmpty())
                                sendGmCrcfMessage(sendto, replyto, cms);
                            else
                                emit statusMessage(tr("Error assembling "
                                            "CRCF message for layout "
                                            "COLUMNS '%1'.").arg(cc));
                        }
                        else
                            emit statusMessage(tr("Unvalid COLUMNS "
                                        "value '%1' detected.").arg(cc));
                    }
                    else
                        emit statusMessage(tr("Layout data editing via "
                                    "CRCF messages is only allowed in "
                                    "layout edit mode."));
                    break;

                    /* LAYOUT <routeid> SET ROWS <att_value> */
                case CrcfMessage::atRows:
                    if (visualMode == kvmEditLayout) {
                        int cr = cm->getAttValue();
                        if (cr != rows && (cr > 0) && cr <= MAX_ROWS) {
                            setLayoutSize(cols, cr);
                            cms = getCrcfInfoMessage(CrcfMessage::atRows);
                            if (!cms.isEmpty())
                                sendGmCrcfMessage(sendto, replyto, cms);
                            else
                                emit statusMessage(tr("Error assembling "
                                            "CRCF message for layout "
                                            "ROWS '%1'.").arg(cr));
                        }
                        else
                            emit statusMessage(tr("Unvalid ROWS "
                                        "value '%1' detected.")
                                    .arg(cm->getAttValue()));
                    }
                    else
                        emit statusMessage(tr("Layout data editing via "
                                    "CRCF messages is only allowed in "
                                    "layout edit mode."));
                    break;

                    /* LAYOUT <layoutid> SET ID <att_value> */
                case CrcfMessage::atId:
                    if (visualMode == kvmEditLayout) {
                        unsigned int mid = cm->getAttValue();
                        if (layoutid != mid) {
                            setLayoutId(mid);
                            cms = getCrcfInfoMessage(CrcfMessage::atId);
                            if (!cms.isEmpty())
                                sendGmCrcfMessage(sendto, replyto, cms);
                            else
                                emit statusMessage(tr("Error assembling "
                                            "CRCF message for layout ID "
                                            "'%1'.").arg(mid));
                        }
                    }
                    else
                        emit statusMessage(tr("Layout data editing via "
                                    "CRCF messages is only allowed in "
                                    "layout edit mode."));
                    break;

                    /* LAYOUT <layoutid> SET NAME <att_value> */
                case CrcfMessage::atName:
                    if (visualMode == kvmEditLayout) {
                        QString ln = cm->getAttValueStr();
                        if (ln != layoutname) {
                            setLayoutName(ln);
                            cms = getCrcfInfoMessage(CrcfMessage::atName);
                            if (!cms.isEmpty())
                                sendGmCrcfMessage(sendto, replyto, cms);
                            else
                                emit statusMessage(tr("Error assembling "
                                            "CRCF message for layout NAME "
                                            "'%1'.").arg(ln));
                        }
                    }
                    else
                        emit statusMessage(tr("Layout data editing via "
                                    "CRCF messages is only allowed in "
                                    "layout edit mode."));
                    break;

                    /* LAYOUT <layoutid> SET TABLELIGHT <att_value> */
                case CrcfMessage::atTableLight:
                    {
                        unsigned int value = cm->getAttValue();
                        if (value > 1) 
                            emit statusMessage(tr("Unvalid TABLELIGHT "
                                        "value '%1' detected.").arg(value));
                        else {
                            bool tl = (bool) value;
                            if (tl != tablelight) {
                                switchTableLight(tl);
                                cms = getCrcfInfoMessage(CrcfMessage::atTableLight);
                                if (!cms.isEmpty())
                                    sendGmCrcfMessage(sendto, replyto, cms);
                                else
                                    emit statusMessage(tr("Error assembling "
                                                "CRCF message for layout TABLELIGHT "
                                                "'%1'.").arg(tl));
                            }
                        }
                    }
                    break;

                default:
                    emit statusMessage(tr("Uneditable CRCF attribute '%1' "
                                "detected.").arg(cm->getAttributeStr()));
                    break;

            }// end SET switch
            break;

        default:
            emit statusMessage(tr("Unsupported CRCF method '%1' "
                        "detected.").arg(cm->getMethodStr()));
            break;
    }
}


/* send a SRCP GM CRCF message to server, method identical to router */
void GBSArea::sendGmCrcfMessage(unsigned int sendto,
        unsigned int replyto, const QString& cms)
{
    SrcpMessage sm = SrcpMessage(SrcpMessage::msgGmSet);
    sm.setGmData(sendto, replyto, "CRCF", cms);
    emit sendSrcpMessage(&sm);
}


/*assemble CRCF layout info message string*/
QString GBSArea::getCrcfInfoMessage(CrcfMessage::CrcfAttribute at) const
{
    unsigned int result = 0;

    switch (at) {
        case CrcfMessage::atId:
            result = layoutid;
            break;

        case CrcfMessage::atName:
            return CrcfMessage::message(CrcfMessage::acLayout, layoutid,
                    CrcfMessage::meInfo, at, layoutname);
            break;

        case CrcfMessage::atColumns:
            result = cols;
            break;

        case CrcfMessage::atRows:
            result = rows;
            break;

        case CrcfMessage::atTableLight:
            result = tablelight;
            break;

        default:
            return "";
            break;
    }

    return CrcfMessage::message(CrcfMessage::acLayout, layoutid,
            CrcfMessage::meInfo, at, result);
}

/*switch table light nn/off*/
void GBSArea::switchTableLight(bool ison)
{
    if (tablelight != ison) {
        tablelight = ison;
        for (unsigned int i = 0; i < elements.size(); i++) {
            element* el = elements[i];
            if (el != NULL)
                el->setTableLight(ison);
        }
        if (tablelight)
            emit statusMessage(tr("Table light switched on"));
        else
            emit statusMessage(tr("Table light switched off"));
    }
}
