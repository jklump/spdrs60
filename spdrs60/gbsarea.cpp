/***************************************************************************
                           gbsarea.cpp
                           version 0.5.2 $Revision: 1.78 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-09-14 15:47:58 $
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

#include "resources.h"
#include "gbsarea.h"

/*cursor pixmaps*/
#include "pixmaps/cursor_wgt_b.xpm"
#include "pixmaps/cursor_wgt_m.xpm"
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

// search options
#define SRCH_TX 0   // search string should be in text field
#define SRCH_A1 1   // search string should be in address 1 field
#define SRCH_A2 2   // search string should be in address 2 field

// constants for context menu
#define   CTX_ID_REP       901
#define   CTX_ID_TOGGLE    902
#define   CTX_ID_CLEAR     903
#define   CTX_ID_ROTATE    904

// mime type for layout elements
#define MIME_LE "application/x-spdrs60-le"


GBSArea::GBSArea(QWidget* parent, const char* name)
: QWidget(parent, name)
{
    // set all global layout variables
    gkbState = kNoneClicked;
    visualMode = kvmNormal;
    lyeditMode = lemSelect;
    modified = false;
    cols = 0;
    rows = 0;
    lastElementName = "";
    setPaletteBackgroundColor(QColor(lightGray));
     
    setAcceptDrops(true);
    dragging = false;
    erasing = false;
    painting = false;

    elements.setAutoDelete(true);

    // SRCP 0.8 data
    SRCP08GA1InitWalker = 0;
    SRCP08GA2InitWalker = 0;
    SRCP08GABusCount = 0;
    SRCP08GABusWalker = 0;
    pSRCP08GABusList = NULL;
    SRCP08FBBusCount = 0;
    SRCP08FBBusWalker = 0;
    pSRCP08FBBusList = NULL;
    
    /*cursor setup */
    QPixmap cb = QPixmap(cursor_wgt_b_xpm);
    QPixmap cm = QPixmap(cursor_wgt_m_xpm);
    cb.setMask(*cm.mask());
    WGTCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_fht_b_xpm);
    cm = QPixmap(cursor_fht_m_xpm);
    cb.setMask(*cm.mask());
    FHTCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_ufgt_b_xpm);
    cm = QPixmap(cursor_ufgt_m_xpm);
    cb.setMask(*cm.mask());
    UfGTCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_mgt_b_xpm);
    cm = QPixmap(cursor_mgt_m_xpm);
    cb.setMask(*cm.mask());
    MGTCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_sgt_b_xpm);
    cm = QPixmap(cursor_sgt_m_xpm);
    cb.setMask(*cm.mask());
    SGTCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_hagt_b_xpm);
    cm = QPixmap(cursor_hagt_m_xpm);
    cb.setMask(*cm.mask());
    HaGTCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_rzs_b_xpm);
    cm = QPixmap(cursor_rzs_m_xpm);
    cb.setMask(*cm.mask());
    RZSCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_uzs_b_xpm);
    cm = QPixmap(cursor_uzs_m_xpm);
    cb.setMask(*cm.mask());
    UZSCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_zhs_b_xpm);
    cm = QPixmap(cursor_zhs_m_xpm);
    cb.setMask(*cm.mask());
    ZHSCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_rrs_b_xpm);
    cm = QPixmap(cursor_rrs_m_xpm);
    cb.setMask(*cm.mask());
    RRSCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_urs_b_xpm);
    cm = QPixmap(cursor_urs_m_xpm);
    cb.setMask(*cm.mask());
    URSCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_paint_b_xpm);
    cm = QPixmap(cursor_paint_m_xpm);
    cb.setMask(*cm.mask());
    paintCursor = QCursor(cb, 0, 0);

    cb = QPixmap(cursor_erase_b_xpm);
    cm = QPixmap(cursor_erase_m_xpm);
    cb.setMask(*cm.mask());
    eraseCursor = QCursor(cb, 0, 0);

    delayTimer = new QTimer(this);
    connect(delayTimer, SIGNAL(timeout()),
            this, SLOT(slotElementClickedTimeout()));

    ctxNorm = new QPopupMenu(this, "ctxNormPM");
    ctxNorm->insertItem(tr("&Toggle"), CTX_ID_TOGGLE);
    
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

    //TODO: make element names translatable
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
}


GBSArea::~GBSArea()
{
    elements.clear();

    /* clear SRCP bus lists */
    if ((SRCP08GABusCount > 0) && (pSRCP08GABusList != NULL))
        free(pSRCP08GABusList);
    if ((SRCP08FBBusCount > 0) && (pSRCP08FBBusList != NULL))
        free(pSRCP08FBBusList);

}


// *INDENT-OFF*
QSize GBSArea::sizeHint() const
{
    return QSize(1 + cols * (EL_WIDTH + 1), 1 + rows * (EL_HEIGHT + 1));
}
// *INDENT-ON*


void GBSArea::newFile(int iColumns, int iRows)
{
    cols = iColumns;
    rows = iRows;

    // delete a possibly shown old layout
    emit clearRoutes();
    elements.clear();
    elements.resize(rows * cols);
    adjustSize();
    emit updateRoutingViewer("");
}
        

void GBSArea::writeFileTextToStream(QTextStream& ts)
{
    ts << "# start of layout section" << endl
       << "%% layout" << endl
       << "# layout dimensions=columns" << DS "rows" << endl
       << GF_DIMENSIONS << DS << cols << DS << rows << endl
       << "# start of element section" << endl;
       //<< "# elements=" << elements.count() << endl;
    
    for (unsigned int i = 0; i < elements.size(); i++) {
        element* el = elements[i];
        if (el != NULL && !el->isEmpty()) {
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
    
    QString s, key, value;
    unsigned int ecount = 0;

    while (!ts.eof()) {
        s = ts.readLine();
        
        /* ignore comment lines */
        if (s.startsWith("#"))
            continue;

        /*TODO: read layout dimensions */ 
        key = s.section(DS, 0, 0);
        value = s.section(DS, 1, 1).stripWhiteSpace();
        /* key/value pairs are read sequence independent */
        if (key.compare(GF_DIMENSIONS) == 0){
            cols = value.toInt();
            value = s.section(DS, 2, 2).stripWhiteSpace();
            rows = value.toInt();
            ecount = cols * rows;
            elements.resize(ecount);
            adjustSize();
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
            }
        }
        else if (s.startsWith("%% route"))
            break;
        }
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
    
    switch (externalButton){
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
                                " is allready locked by an active route.")
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
            else if (kFhtClicked == gkbState){
                QApplication::beep();
                emit statusMessage(tr("Turnouts can not be switched "
                            "using FHT"));
            }
            else if (kSgtClicked == gkbState){
                QApplication::beep();
                emit statusMessage(tr("Turnouts can not be switched "
                            "using SGT"));
            }
            else if (kUfgtClicked == gkbState){
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
    element* endPtr = tSig.elemPtr;
    bool finished = false;
    bool setrt = (Route::rsaReset != setRoute);
    
    unsigned int entrydir;
    unsigned int exitdir;

    if (fSig.elemPtr->iSoldRotate == 1)
        entrydir = rdE;
    else
        entrydir = rdW;

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

        finished = (rel == endPtr);

        // paint yellow track and get back new route direction
        exitdir = rel->routeElement(entrydir, setrt);
        entrydir = rdCenter;
        
        if (exitdir == rdCenter) {
            emit statusMessage(tr("Route found dead end at element %1")
                    .arg(idx));
            break;
        }
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
                el->sSoldIcon != SYM_ENK &&
                el->sSoldIcon != SYM_MDC &&
                el->sSoldIcon != SYM_SBN &&
                el->sSoldIcon != SYM_DRE)
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
                el->sSoldIcon != SYM_ENK &&
                el->sSoldIcon != SYM_MDC &&
                el->sSoldIcon != SYM_SBN &&
                el->sSoldIcon != SYM_DRE)
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
            el, SLOT(slotShowElement(int, int,
                    elemSelectionMode)));
    connect(this, SIGNAL(sigRepaintLayout()),
            el, SLOT(slotRepaintLayout()));
    connect(el, SIGNAL(recordElement(element*,
                    elemRecordType)),
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


// *INDENT-OFF*
bool GBSArea::isModified() const
// *INDENT-ON*
{
    return modified;
}


void GBSArea::setModified(bool m)
{
    if (modified != m)
        modified = m;

    updateSRCP08BusLists();
}


int GBSArea::getColumns()
{
    return cols;
}


int GBSArea::getRows()
{
    return rows;
}


void GBSArea::setLayoutSize(int newcols, int newrows)
{
    if (newcols < 1 || newrows < 1)
        return;

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
unsigned int GBSArea::indexOf(QPoint ep) const
{
    int row = ep.y() / (EL_HEIGHT + 1) + 1;
    int col = ep.x() / (EL_WIDTH + 1) + 1;
    return (rows * (col - 1) + row - 1); 
}
// *INDENT-ON*


void GBSArea::moveElementToIndexPos(element* el, unsigned int idx)
{
    if (el != NULL && idx >= 0 && idx < elements.size())
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
 * init or term every single FB bus, but only one at a time
 * return true while there are unchanged busses left
 */
/*
bool GBSArea::switchSRCP08FBBusState(bool setInitOn)
{
    bool WalkerChanged = false;

    *
     * generate
     *   INIT <bus> FB
     * or
     *   TERM <bus> FB
     *
    if ((SRCP08FBBusCount > 0)
        && (SRCP08FBBusWalker < SRCP08FBBusCount)) {

        //TODO: (SrcpMessage::Feedback) pref.fbmoduletype
        SrcpMessage* sm = new SrcpMessage(setInitOn ?
                SrcpMessage::msgFbInit : SrcpMessage::msgFbTerm);
        if (sm == NULL)
            return WalkerChanged;
       
        sm->setBus(pSRCP08FBBusList[SRCP08FBBusWalker]);
        
        emit sendSrcpMessage(sm);

        delete sm;

        SRCP08FBBusWalker++;
        WalkerChanged = true;
    }

    if (!WalkerChanged)
        SRCP08FBBusWalker = 0;

    return WalkerChanged;
}
*/
/*
 * ask server about power status off every single bus, but only one at
 * a time return true while there are unasked busses left
 */
// sendSRCP08BusMessage()
bool GBSArea::sendSRCP08BusMessage(SrcpMessage::Message smt)
{
    bool WalkerChanged = false;

    /*
     * first all GA busses are asked step by step, allways waiting
     * for server OK
     */
    if ((SRCP08GABusCount > 0)
        && (SRCP08GABusWalker < SRCP08GABusCount)) {

        //SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgPowerGet);
        SrcpMessage* sm = new SrcpMessage(smt);
        if (sm == NULL)
            return WalkerChanged;
       
        sm->setBus(pSRCP08GABusList[SRCP08GABusWalker]);
        emit sendSrcpMessage(sm);
        delete sm;

        SRCP08GABusWalker++;
        WalkerChanged = true;
    }

    /*
     * when GAs are ready, ask attached FB busses,
     * also after each bus waiting for server OK
     */
    if (!WalkerChanged && (SRCP08FBBusCount > 0) &&
        (SRCP08FBBusWalker < SRCP08FBBusCount)) {

        //SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgPowerGet);
        SrcpMessage* sm = new SrcpMessage(smt);
        if (sm == NULL)
            return WalkerChanged;
       
        sm->setBus(pSRCP08FBBusList[SRCP08FBBusWalker]);
        emit sendSrcpMessage(sm);
        delete sm;
        
        SRCP08FBBusWalker++;
        WalkerChanged = true;
    }

    /* reset when this process is finished for all busses */
    if (!WalkerChanged) {
        SRCP08GABusWalker = 0;
        SRCP08FBBusWalker = 0;
    }

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
     * first all GA busses are initialized step by step, allways waiting
     * for server OK
     */
    if ((SRCP08GABusCount > 0)
        && (SRCP08GABusWalker < SRCP08GABusCount)) {
        SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgPowerSet);
        if (sm == NULL)
            CounterChanged = true;
       
        sm->setPowerData(pSRCP08GABusList[SRCP08GABusWalker],
                setPowerOn);
        emit sendSrcpMessage(sm);
        delete sm;

        SRCP08GABusWalker++;
        CounterChanged = true;
    }

    /*
     * when GAs are ready, start init process of all attached FB busses,
     * also after each bus init waiting for server OK
     */
    if (!CounterChanged && (SRCP08FBBusCount > 0) &&
        (SRCP08FBBusWalker < SRCP08FBBusCount)) {
        SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgPowerSet);
        if (sm == NULL)
            CounterChanged = true;
       
        sm->setPowerData(pSRCP08FBBusList[SRCP08FBBusWalker],
                setPowerOn);
        emit sendSrcpMessage(sm);
        delete sm;

        SRCP08FBBusWalker++;
        CounterChanged = true;
    }

    /* reset when init process for all busses is finished */
    if (!CounterChanged) {
        SRCP08GABusWalker = 0;
        SRCP08FBBusWalker = 0;
    }

    return CounterChanged;
}


void GBSArea::updateSRCP08BusLists()
{
    updateSRCP08GABusList();
    updateSRCP08FBBusList();
}


void GBSArea::updateSRCP08GABusList()
{
    unsigned int count = 0, busno = 0;
    bool busNoIsKnown;
    unsigned int *tempbuslist;

    /*how many different GA busses do we have? */
    for (unsigned int i = 0; i < elements.size(); i++) {

        element* el = elements[i];
        if (el == NULL)
            continue;

        if (!el->isSwitchable())
            continue;

        for (int k = 0; k < 2; k++) {
            switch (k) {
            case 0:
                busno = el->getGA1BusNo();
                break;
            case 1:
                busno = el->getGA2BusNo();
                break;
            }

            if (busno > 0) {
                if (count == 0) {
                    count++;

                    pSRCP08GABusList = (unsigned int *) calloc(count, sizeof(unsigned int));
                    if (pSRCP08GABusList == NULL) {
                        fprintf(stderr, "Memory allocation error!");
                        SRCP08GABusCount = count - 1;
                        return;
                    }
                    pSRCP08GABusList[0] = busno;
                }
                /*count > 0 */
                else {
                    busNoIsKnown = false;
                    for (unsigned int j = 0; j < count; j++) {
                        if (pSRCP08GABusList[j] == busno) {
                            busNoIsKnown = true;
                            break;
                        }
                    }
                    if (!busNoIsKnown) {
                        count++;

                        tempbuslist =
                            (unsigned int *) realloc(pSRCP08GABusList,
                                            sizeof(unsigned int) * count);

                        if (tempbuslist == NULL) {
                            fprintf(stderr, "Memory allocation error!");
                            SRCP08GABusCount = count - 1;
                            return;
                        }
                        pSRCP08GABusList = tempbuslist;
                        pSRCP08GABusList[count - 1] = busno;
                    }
                }
            }
        }                       /* for j */
    }                           /* for i */

    if (count == 0)
        statusMessage(tr("Layout does not contain a SRCP bus"
                    " configuration for GAs"));
    else if (count == 1)
        statusMessage(tr("Layout contains 1 configured GA bus"));
    else
        statusMessage(tr("Layout contains %1 configured GA busses").
                   arg(count));

    SRCP08GABusCount = count;
}


void GBSArea::updateSRCP08FBBusList()
{
    unsigned int count = 0, busno = 0;
    unsigned int *tempbuslist;
    bool busNoIsKnown;

    if (pSRCP08FBBusList != NULL) {
        free(pSRCP08FBBusList);
        pSRCP08FBBusList = NULL;
    }

    /*how many different FB busses do we have? */
    for (unsigned int i = 0; i < elements.size(); i++) {

        element* el = elements[i];
        if (el == NULL)
            continue;

        if (!el->isSwitchable())
            continue;

        busno = el->getFBBusNo();

        if (busno > 0) {
            if (count == 0) {
                count++;
                pSRCP08FBBusList = (unsigned int *) calloc(count,
                        sizeof(busno));
                if (pSRCP08FBBusList == NULL) {
                    fprintf(stderr, "Memory allocation error!");
                    SRCP08FBBusCount = count - 1;
                    return;
                }
                pSRCP08FBBusList[0] = busno;
            }

            /*count > 0 */
            else {
                busNoIsKnown = false;
                for (unsigned int j = 0; j < count; j++) {
                    if (pSRCP08FBBusList[j] == busno) {
                        busNoIsKnown = true;
                        break;
                    }
                }
                if (!busNoIsKnown) {
                    count++;
                    tempbuslist = (unsigned int *) realloc(pSRCP08FBBusList,
                                              sizeof(busno) * count);
                    if (tempbuslist == NULL) {
                        fprintf(stderr, "Memory allocation error!");
                        SRCP08FBBusCount = count - 1;
                        return;
                    }

                    pSRCP08FBBusList = tempbuslist;
                    memset(&pSRCP08FBBusList[count - 1], 0,
                           sizeof(busno));
                    pSRCP08FBBusList[count - 1] = busno;
                }
            }
        }
    }                           /* for i */

    if (count == 0)
        statusMessage(tr("Layout does not contain a SRCP bus"
                    " configuration for FBs"));
    else if (count == 1)
        statusMessage(tr("Layout contains 1 configured FB bus"));
    else
        statusMessage(tr("Layout contains %1 configured FB busses").
                arg(count));

    SRCP08FBBusCount = count;
}


bool GBSArea::hasSrcp08GaBus(unsigned int bus)
{
    bool returnvalue = false;

    if (SRCP08GABusCount > 0) 
        for (unsigned int i = 0; i < SRCP08GABusCount; i++)
            if (bus == pSRCP08GABusList[i]){
                returnvalue = true;
                break;
            }

    return returnvalue;
}


void GBSArea::mouseReleaseEvent(QMouseEvent* e)
{
    /*normal mode*/
    if (visualMode == kvmNormal) {
        if (e->button() == RightButton){
            element* el = (element*)childAt(e->pos());

            if (el != NULL && el->isSwitchable()) {
                ctxNorm->setItemEnabled(CTX_ID_TOGGLE,
                        el->ctxCanSwitch());

                if (ctxNorm->exec(QCursor::pos()) != -1)
                    el->toggle();
            }
            e->accept();
        }
    }

    /*layout edit mode*/
    else if (visualMode == kvmEditLayout) {
        if (e->button() == LeftButton) {

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
        else if (e->button() == MidButton) {
            element* el = (element*)childAt(e->pos());
            unsigned int idx = indexOf(e->pos());

            // add new empty element
            if (el == NULL) {
                el = new element(this);
                el->setIndexNo(idx);
                el->switchVisualMode(visualMode);
                moveElementToIndexPos(el, idx);
                elements.insert(idx, el);
                connectElement(el);
                el->show();
            }

            // first update name of last edited element
            ctxEdit->setItemEnabled(CTX_ID_REP,
                    !lastElementName.isEmpty());

            QString s = QString(tr("&Repeat: %1")).arg(lastElementName);
            ctxEdit->changeItem(s, CTX_ID_REP);
            ctxEdit->setItemEnabled(CTX_ID_CLEAR, true);
            ctxEdit->setItemEnabled(CTX_ID_ROTATE, el->isRotatable());
            int value = ctxEdit->exec(QCursor::pos());

            if (value != -1) {
                switch (value) {
                    case CTX_ID_REP:
                        el->setElementName(lastElementName);
                        break;
                    case CTX_ID_ROTATE:
                        el->rotate();
                        break;
                    case CTX_ID_CLEAR:
                        //route data is updated when edit mode is left
                        elements.remove(idx);
                        lastElementName = "";
                        break;
                    case 5:
                        el->setElementName(SYM_GER);
                        lastElementName = SYM_GER;
                        break;
                    case 6:
                        el->setElementName(SYM_KUL);
                        lastElementName = SYM_KUL;
                        break;
                    case 7:
                        el->setElementName(SYM_KUR);
                        lastElementName = SYM_KUR;
                        break;
                    case 8:
                        el->setElementName(SYM_DIL);
                        lastElementName = SYM_DIL;
                        break;
                    case 9:
                        el->setElementName(SYM_DIR);
                        lastElementName = SYM_DIR;
                        break;
                    case 10:
                        el->setElementName(SYM_WEL);
                        lastElementName = SYM_WEL;
                        break;
                    case 11:
                        el->setElementName(SYM_WER);
                        lastElementName = SYM_WER;
                        break;
                }
                modified = true;
            }
            e->accept();
        }
        else if (e->button() == RightButton){
            element* el = (element*)childAt(e->pos());

            // add new empty element
            if (el == NULL) {
                el = new element(this);
                unsigned int idx = indexOf(e->pos());
                el->setIndexNo(idx);
                el->switchVisualMode(visualMode);
                moveElementToIndexPos(el, idx);
                elements.insert(idx, el);
                connectElement(el);
                el->show();
            }

            el->showPropertyDlg();
            lastElementName = el->sSoldIcon;
            e->accept();
        }
    }

    /*route edit mode*/
    else if (visualMode == kvmEditRoute) {
        /*show context menu to switch element only without selection*/
        if (e->button() == RightButton){
            element* el = (element*)childAt(e->pos());

            if (el != NULL && el->isSwitchable()) {
                ctxNorm->setItemEnabled(CTX_ID_TOGGLE,
                        el->ctxCanSwitch());

                if (ctxNorm->exec(QCursor::pos()) != -1)
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
    /*layout edit mode*/
    if (visualMode == kvmEditLayout) {
        if (e->button() == LeftButton) {

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
                    lastElementName = "";
                    modified = true;
                }
                erasing = true;
            }

            /* paint item */
            else if (lyeditMode == lemPaint) {
                /*
                element* el = (element*)childAt(e->pos());
                unsigned int idx = indexOf(e->pos());

                if (el != NULL) {
                    if (el->classId() != sici..) {
                    elements.remove(idx);
                    elements.insert(idx, new element());
                    lastElementName = "";
                    modified = true;
                    }
                }
                else {
                    //TODO: add painted element
                }
                */
                painting = true;
            }
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
                d->setPixmap(*el->paletteBackgroundPixmap(),
                        QPoint(EL_WIDTH / 2, EL_HEIGHT / 2));
                d->dragMove();
                dragging = false;
            }
        }

        /* erase elements */
        else if ((lyeditMode == lemErase) && erasing) {
                element* el = (element*)childAt(e->pos());
                unsigned int idx = indexOf(e->pos());

                if (el != NULL) {
                        elements.remove(idx);
                        lastElementName = "";
                        modified = true;
                }
        }
        
        /* paint elements */
        else if ((lyeditMode == lemPaint) && painting) {
        }
    }
}

/**
 * give feedback if this widget cares about the offered data
 * */
void GBSArea::dragEnterEvent(QDragEnterEvent* e)
{
    if (visualMode == kvmEditLayout) {
        //TODO: enable DnD between different windows
        if (e->provides(MIME_LE) && e->source() == this) {
            e->accept();
        }
    }
}

/**
 * take the dropped data and do something usefull with it
 * */
void GBSArea::dropEvent(QDropEvent *e)
{
    if (visualMode == kvmEditLayout) {

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
/* TODO:
void GBSArea::changeLayoutPaintItem(SpdrItemClassId sici)
{
    if (sici != paintItem)
        paintItem = sici;
}
*/
