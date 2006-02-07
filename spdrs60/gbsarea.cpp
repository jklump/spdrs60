/***************************************************************************
                           gbsarea.cpp
                           version 0.4.8 $Revision: 1.45 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2006-02-07 17:17:38 $
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
#include <qprogressdialog.h>

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

#define OLD_MAX_ROWS 18;



GBSArea::GBSArea(QWidget* parent, const char* name)
: QWidget(parent, name)
{
    // set all global layout variables
    gkbState = kNoneClicked;
    modified = false;
    cols = 0;
    rows = 0;

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

    delayTimer = new QTimer(this);
    connect(delayTimer, SIGNAL(timeout()),
            this, SLOT(slotElementClickedTimeout()));
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
    // Size of GBSArea: columns * element width, rows * element height
    if (elements.count() == 0)
        return QSize(0, 0);
    else
        return QSize(cols * EL_WIDTH, rows * EL_HEIGHT);
}
// *INDENT-ON*


void GBSArea::newFile(int iColumns, int iRows)
{
    cols = iColumns;
    rows = iRows;

    // delete a possibly shown old layout
    deleteElements();
    elements.resize(iRows * iColumns);

    QProgressDialog progress(tr("Creating empty layout file"),
                             tr("Abort"), iRows * iColumns,
                             this, "progress", true);
    progress.show();

    QString sData;

    for (int i = 0; i < (iRows * iColumns); i++) {
        // now create the new elements (but do not show them yet)
        element* anElement = new element(this);
        anElement->setIndexNo(i);
        elements.insert(i, anElement);
        anElement->move((i / iRows) * EL_WIDTH,
                        (i % iRows) * EL_HEIGHT);

        if ((i % 10) == 0)
            progress.setProgress(i);
        qApp->processEvents();

#if QT_VERSION >= 0x030200
        if (progress.wasCanceled()) {
#else
        if (progress.wasCancelled()) {
#endif
            deleteElements();
            return;
        }
    }
    progress.setProgress(iRows * iColumns);
    // now setup and show elements
    setupElements();
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
        element* e = elements[i];
        if (e != NULL && !e->isEmpty()) {
            ts << "%% element " << i << endl;
            e->writeFileTextToStream(ts);
        }
    }
    setModified(false);
}


void GBSArea::readFileTextFromStream(QTextStream& ts)
{
    /*clear old element list*/
    if (!elements.isEmpty())
        deleteElements();
    
    QProgressDialog progress(tr("Loading layout file"),
                             tr("Abort"), 0, this, "progress", true);
    progress.show();
    
    QString s, key, value;
    unsigned int ecount = 0;

    while (!ts.eof()) {
        s = ts.readLine();
        
        /* ignore comment lines */
        if (!s.startsWith("#")) {
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

                /* setup progress dialog */
                progress.setTotalSteps(ecount);
            }
            /*here we read allways up to start marker of a new route*/
            else if (s.startsWith("%% element")) {
                
                element* fe = new element(ts, this, true);
                if (fe != NULL) {
                    unsigned int idx = fe->getIndexNo();
                    if ((idx % 10) == 0)
                        progress.setProgress(idx);
                    qApp->processEvents();

                    if (idx < ecount) {
                        fe->move((idx / rows) * EL_WIDTH,
                                 (idx % rows) * EL_HEIGHT);
                        elements.insert(idx, fe);
                    }
                }
#if QT_VERSION >= 0x030200
                if (progress.wasCanceled()) {
#else
                if (progress.wasCancelled()) {
#endif
                    deleteElements();
                    ecount = 0;
                    break;
                }
            }
            else if (s.startsWith("%% route"))
                break;
        }
    }
    /*
     * may be this some time can be removed when empty elements are
     * handled in an other way by gbsarea
     */
    progress.setLabelText(tr("Adding empty elements"));
    progress.setTotalSteps(ecount);
    /*add mising empty elements*/
    for (unsigned int i = 0; i < ecount; i++) {
        if (elements[i] == NULL){
            element* ee = new element(this);
            ee->setIndexNo(i);
            elements.insert(i, ee);
            ee->move((i / rows) * EL_WIDTH,
                    (i % rows) * EL_HEIGHT);
            if ((i % 10) == 0)
                progress.setProgress(i);
            qApp->processEvents();

#if QT_VERSION >= 0x030200
            if (progress.wasCanceled()) {
#else
            if (progress.wasCancelled()) {
#endif
                deleteElements();
                break;
            }
        }
    }
    progress.setProgress(ecount);
    adjustSize();
    setModified(false);
    
    // now setup and show elements, send element states to SRCP-server
    // and load routes
    setupElements();
    slotSendAll();
}


void GBSArea::readOldFileTextFromStream(QTextStream& ts)
{
    /*clear old element list and old routes*/
    if (!elements.isEmpty())
        deleteElements();
    
    QString s, key, value;
    unsigned int ecount = 0;

    s = ts.readLine();
    key = s.section(":", 0, 0);
    value = s.section(":", 1, 1).stripWhiteSpace();

    if (key.compare("Symbols total #") == 0){
        ecount = value.toUInt();
        elements.resize(ecount);
        cols = ecount/OLD_MAX_ROWS;
        rows = OLD_MAX_ROWS;
    }
    QProgressDialog progress(tr("Loading layout file"),
                             tr("Abort"), ecount,
                             this, "progress", TRUE);
    progress.show();

    s = ts.readLine(); // modify date
    s = ts.readLine(); // version
    s = ts.readLine(); // empty line
    /*here we read allways up to start marker of a new route*/
    while (!ts.eof()) {
        s = ts.readLine(); // SYMBOL
        if (s.startsWith("SYMBOL")) {
            value = s.section(" ", 2, 2).stripWhiteSpace();
            unsigned int idx = value.toUInt();
            /* contructor with old file format*/
            element* fe = new element(ts, this, false);
            if (fe != NULL) {
                fe->move((idx / rows) * EL_WIDTH,
                        (idx % rows) * EL_HEIGHT);
                fe->setIndexNo(idx);
                elements.insert(idx, fe);
                if ((idx % 10) == 0)
                    progress.setProgress(idx);
                qApp->processEvents();
            }
#if QT_VERSION >= 0x030200
            if (progress.wasCanceled()) {
#else
            if (progress.wasCancelled()) {
#endif
                deleteElements();
                break;
            }
        }
    }
    /* files of old format must be saved later*/
    progress.setProgress(ecount);
    adjustSize();
    setModified(true);

    // now setup and show elements, send element states to SRCP-server
    // and load routes
    setupElements();
    slotSendAll();
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
            emit showLogMessage(tr("MGT-Function not supported."),
                    MT_INFO, HL_CMND);
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
                    emit showLogMessage(tr("No switching possible, "
                                "signal '%1' is locked by an active route.")
                            .arg(el->getName()), MT_INFO, HL_CMND);
                }
                else
                    if (kSgtClicked == gkbState)
                        el->slotToggle();
                    else
                        el->slotSwitchIt(0, UNLOCKED);
                slotElementClickedTimeout();
            }
            else if (kFhtClicked == gkbState) {
                // give direct button pressed feedback to user
                setCursor(ArrowCursor);
                /* send signal to route controller*/
                emit resetRoute(el, gbsButton);
            }
            else if (kUfgtClicked == gkbState || kNoneClicked ==
                    gkbState) {
                if (el->hasFfMLock()) {
                    QApplication::beep();
                    emit showLogMessage(tr("No routing possible; signal '%1'"
                                " is allready locked by an active route.")
                            .arg(el->getName()), MT_INFO, HL_CMND);
                    slotElementClickedTimeout();
                }
                else {
                    // give button pressed feedback to user
                    setCursor(ArrowCursor);
                    /* send signal to route controller*/
                    emit setRoute(el, gbsButton, gkbState);
                }
            }
            else if (kWgtClicked == gkbState) {
                QApplication::beep();
                emit showLogMessage(tr("Signals can not be switched "
                            "using WGT"), MT_INFO, HL_CMND);
                slotElementClickedTimeout();
            }
            else {
                QApplication::beep();
                emit showLogMessage(tr("Operation not allowed"),
                        MT_INFO, HL_CMND);
                slotElementClickedTimeout();
            }
            break;
            
        /*turnout button*/
        case kTurnoutClicked:
            if (kWgtClicked == gkbState) {
                if (el->isLocked()) {
                    QApplication::beep();
                    emit showLogMessage(tr("No switching possible, "
                                "solenoid '%1' is locked by an active route.")
                            .arg(el->getName()), MT_INFO, HL_CMND);
                }
                else if (el->isOccupied()) {
                    QApplication::beep();
                    emit showLogMessage(tr("No switching possible, "
                                "turnout is occupied"), MT_INFO, HL_CMND);
                }
                else
                    el->slotToggle();
            }
            else if (kFhtClicked == gkbState){
                QApplication::beep();
                emit showLogMessage(tr("Turnouts can not be switched "
                            "using FHT"),
                        MT_INFO, HL_CMND);
            }
            else if (kSgtClicked == gkbState){
                QApplication::beep();
                emit showLogMessage(tr("Turnouts can not be switched "
                            "using SGT"),
                        MT_INFO, HL_CMND);
            }
            else if (kUfgtClicked == gkbState){
                QApplication::beep();
                emit showLogMessage(tr("Turnouts can not be switched "
                            "using UfGT"),
                        MT_INFO, HL_CMND);
            }
            else {
                QApplication::beep();
                emit showLogMessage(tr("Operation not allowed"),
                        MT_INFO, HL_CMND);
            }
            slotElementClickedTimeout();
            break;

        default:
            break;
    }
}


void GBSArea::startRouteTimer(TypeOfRoute tor)
{
    switch (tor) {
        case RZS:
            setCursor(RZSCursor);
            break;
        case UZS:
            setCursor(UZSCursor);
            break;
        case ZHS:
            setCursor(ZHSCursor);
            break;
        case RRS:
            setCursor(RRSCursor);
            break;
        case URS:
            setCursor(URSCursor);
            break;
    }
    delayTimer->start(cDelayTime);
}


void GBSArea::slotElementClickedTimeout()
{
    emit resetSelectedSignal();
    gkbState = kNoneClicked;
    setCursor(ArrowCursor);
}


void GBSArea::updateRoutePathLEDs(const stateElement& fSig,
        const stateElement& tSig, RouteSetAction& setRoute)
{
    if (fSig.elemPtr == NULL || tSig.elemPtr == NULL)
        return;

    if (rows <= 1)
        return;

    int iCorr = 0;
    int idx = fSig.elemPtr->getIndexNo();
    int maxIdx = (int) elements.size();
    element* endPtr = tSig.elemPtr;
    bool layoutEdge = false;
    bool finished = false;
    bool toRight = !fSig.elemPtr->iSoldRotate;
    bool setrt = (krouteReset != setRoute);

    while (!finished) {
        if ((idx < 0) || (idx >= maxIdx)) {
            layoutEdge = true;
            break;
        }
        
        element* rel = elements[idx];
        if (rel == NULL) {
            layoutEdge = true;
            break;
        }

        if (!rel->isRoutable()) {
            layoutEdge = true;
            break;
        }

        // interrupt operation when normal route meets occupied element
        // start signal is allowed to be occupied
        if ((krouteZfs == setRoute) && rel->isOccupied() &&
                rel != fSig.elemPtr) {
            // feedback for caller
            setRoute = krouteReset;
            return;
        }

        finished = (rel == endPtr);

        // paint yellow track and get back vertical correction value
        iCorr = rel->routeElement(toRight, setrt, iCorr);

        // calculate column of next element
        if (toRight)
            idx += rows + iCorr;
        else
            idx += -rows + iCorr;
    }

    // Routing over layout edges:
    // 1) start1 -> stop1 (normal)
    // 2) start2 -> stop1
    // 3) start1 -> stop2
    // 4) start2 -> stop2
    if (layoutEdge) {
        bool secondRun = false;
        if (fSig.elemPtr2 != NULL) {
            idx = fSig.elemPtr2->getIndexNo();
            secondRun = true;
        }
        else if (tSig.elemPtr2 != NULL) {
            endPtr = tSig.elemPtr2;
            secondRun = true;
        }
        else if (fSig.elemPtr2 != NULL && tSig.elemPtr2 != NULL) {
            idx = fSig.elemPtr2->getIndexNo();
            endPtr = tSig.elemPtr2;
            secondRun = true;
        }

        if (!secondRun)
            return;

        iCorr = 0;
        finished = false;
        /*second run*/
        while (!finished) {
            if ((idx < 0) || (idx >= maxIdx)) {
                break;
            }

            element* rel = elements[idx];
            if (rel == NULL) {
                break;
            }

            if (!rel->isRoutable()) {
                break;
            }
            finished = (rel == endPtr);

            iCorr = rel->routeElement(toRight, setrt, iCorr);

            if (toRight)
                idx += rows + iCorr;
            else
                idx += -rows + iCorr;
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


bool GBSArea::findElement(const QString& ftext, int ftype, int fmulti)
{
    // search all elements for the desired addresses or text and
    // return its index
    QString s = "";
    unsigned int idx;
    bool returnvalue = false;

    // locate element with certain address 1
    if (ftype == SRCH_A1) 
        for (idx = 0; idx < elements.size(); idx++) {
            if (ftext.toInt() ==
                    elements[idx]->getAddress1()) {
                returnvalue = true;
                elements[idx]->locateMe();
                if (fmulti == 1)
                    continue;
                else
                    return returnvalue;
            }
        }

    // locate element with certain address 2
    else if (ftype == SRCH_A2) 
        for (idx = 0; idx < elements.size(); idx++) {
            if (ftext.toInt() ==
                    elements[idx]->getAddress2()) {
                returnvalue = true;
                elements[idx]->locateMe();
                if (fmulti == 1)
                    continue;
                else
                    return returnvalue;
            }
        }

    // locate element with certain textfield
    else if (ftype == SRCH_TX) 
        for (idx = 0; idx < elements.size(); idx++) {
            s = elements[idx]->sSoldText;
            if (s.contains(ftext, 0)) {
                returnvalue = true;
                elements[idx]->locateMe();
                if (fmulti == 1)
                    continue;
                else
                    return returnvalue;
            }
        }
    return returnvalue;
}


void GBSArea::slotToggleAll()
{
    // toggles all elements but no couplers, motors no shifting bridges,
    // no turntables
    for (unsigned int j = 0; j < elements.size(); j++)
        if (elements[j]->sSoldIcon != SYM_ENK &&
            elements[j]->sSoldIcon != SYM_MDC &&
            elements[j]->sSoldIcon != SYM_SBN &&
            elements[j]->sSoldIcon != SYM_DRE)
            elements[j]->slotToggle();
    QApplication::beep();
}


void GBSArea::slotSendAll()
{
    // send current element states to SRCP server
    for (unsigned int j = 0; j < elements.size(); j++)
        if (elements[j] != NULL)
            if (!(elements[j]->sSoldIcon == SYM_ENK
                        && elements[j]->iSoldSubType != -1) &&
                    //GBSElement[j]->sSoldIcon != SYM_ENK &&
                    elements[j]->sSoldIcon != SYM_MDC &&
                    elements[j]->sSoldIcon != SYM_SBN &&
                    elements[j]->sSoldIcon != SYM_DRE)
                elements[j]->sendSrcpState();
    QApplication::beep();
}


void GBSArea::slotNotrot()
{
    // sets all signals to red state
    for (unsigned int j = 0; j < elements.size(); j++)
        if (elements[j]->isSignal())
            elements[j]->slotSwitchIt(0, 0);  // sec. "0" = NONE (RouteStatus)
    emit showLogMessage(tr("Switched all signals to halt/stop"),
            MT_INFO, HL_CMND);
}


void GBSArea::deleteElements()
{
    /* send signal to router */
    emit clearRoutes();
    //TODO: disconnect all elements
    elements.clear();
    move(0, 0);
    updateGeometry();
}


void GBSArea::setupElements()
{
    for (unsigned int j = 0; j < elements.size(); j++) {
        element* el = elements[j];
        if (el != NULL) {
            el->show();  // now show the element
            connect(el, SIGNAL(elementClicked(element*, GbsButtonState)),
                    this, SLOT(slotElementClicked(element*, GbsButtonState)));
            connect(el, SIGNAL(setRepeatIcon(const QString&)),
                    this, SIGNAL(setRepeatIcon(const QString&)));
            connect(el, SIGNAL(sigShowFBmodules()),
                    this, SIGNAL(sigShowFBmodules()));

            connect(this, SIGNAL(switchVisualMode(elemVisualMode)),
                    el, SLOT(switchVisualMode(elemVisualMode)));
            connect(this, SIGNAL(sigShowElement(int, int,
                            elemSelectionMode)),
                    el, SLOT(slotShowElement(int, int,
                            elemSelectionMode)));
            connect(this, SIGNAL(setRepeatIcon(const QString&)),
                    el, SLOT(slotRepeatIcon(const QString&)));
            connect(this, SIGNAL(sigRepaintLayout()),
                    el, SLOT(slotRepaintLayout()));
            connect(el, SIGNAL(recordElement(element*,
                            elemRecordType)),
                    this, SIGNAL(recordElement(element*, elemRecordType)));
            //if (el->isSwitchable()) {
            connect(this, SIGNAL(processInfoPortMessage(unsigned int,
                            unsigned int, unsigned int)),
                    el, SLOT(processInfoPortMessage(unsigned int,
                            unsigned int, unsigned int)));
            connect(el, SIGNAL(sendSrcpMessage(SrcpMessage*)),
                    this, SIGNAL(sendSrcpMessage(SrcpMessage*)));
            //}
            //if (el->hasLEDsOn())
            connect(this, SIGNAL(feedbackPortChanged(unsigned int,
                            unsigned int, bool)),
                    el, SLOT(slotOccupyElement(unsigned int,
                            unsigned int, bool)));
            //TODO: el->updateFeedbackState();
        }
    }
    move(0, 0);
    updateGeometry();
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

    QPtrVector<element> tmpelements;

    tmpelements.resize(newcols * newrows);
    
    if (newcols < 1 || newrows < 1)
        return;

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
            unsigned int idx = newrows * (c - 1) + r - 1;
            if (el == NULL) {
                /* insert empty element to unoccupied position */
                el = new element(this);
                el->move((c - 1) * EL_WIDTH, (r - 1) * EL_HEIGHT);
                el->show();
                connect(el, SIGNAL(elementClicked(element*, GbsButtonState)),
                        this, SLOT(slotElementClicked(element*,
                                GbsButtonState)));
                connect(el, SIGNAL(setRepeatIcon(const QString&)),
                        this, SIGNAL(setRepeatIcon(const QString&)));
                connect(el, SIGNAL(sigShowFBmodules()),
                        this, SIGNAL(sigShowFBmodules()));
                connect(this, SIGNAL(switchVisualMode(elemVisualMode)),
                        el, SLOT(switchVisualMode(elemVisualMode)));
                connect(this, SIGNAL(sigShowElement(int, int,
                                elemSelectionMode)),
                        el, SLOT(slotShowElement(int, int,
                                elemSelectionMode)));
                connect(this, SIGNAL(setRepeatIcon(const QString&)),
                        el, SLOT(slotRepeatIcon(const QString&)));
                connect(this, SIGNAL(sigRepaintLayout()),
                        el, SLOT(slotRepaintLayout()));
                connect(el, SIGNAL(recordElement(element*, elemRecordType)),
                        this, SIGNAL(recordElement(element*, elemRecordType)));
                //if (el->isSwitchable()) {
                connect(this, SIGNAL(processInfoPortMessage(unsigned int,
                                unsigned int, unsigned int)),
                        el, SLOT(processInfoPortMessage(unsigned int,
                                unsigned int, unsigned int)));
                connect(el, SIGNAL(sendSrcpMessage(SrcpMessage*)),
                        this, SIGNAL(sendSrcpMessage(SrcpMessage*)));
                //}
                //if (el->hasLEDsOn())
                connect(this, SIGNAL(feedbackPortChanged(unsigned int,
                                unsigned int, bool)),
                        el, SLOT(slotOccupyElement(unsigned int,
                                unsigned int, bool)));
            }
            el->setIndexNo(idx);
            /*TODO: set current visual mode */
            tmpelements.insert(idx, el);
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
    for (int r = 1; r <= rows; r++) {
        elements.remove(indexOf(r, col));
    }
}

/*remove all elements of a single row*/
void GBSArea::removeRowElements(int row)
{
    for (int c = 1; c <= cols; c++) {
        elements.remove(indexOf(row, c));
    }
}

/* 
 *  row is defined from 1 to rows
 *  col is defined from 1 to cols
 */
// *INDENT-OFF*
int GBSArea::indexOf(int row, int col) const
{
    return (rows * (col - 1) + row - 1); 
}
// *INDENT-ON*


// *INDENT-OFF*
element* GBSArea::item(int row, int col) const
// *INDENT-ON*
{
    if (row < 0 || col < 0 || row > this->rows ||
            col > this->cols || row * col >= (int)elements.size())
        return 0;

    return elements[indexOf(row, col)];
}


QPtrVector<element>* GBSArea::getGbsElementListPtr()
{
    return &elements;
}


void GBSArea::sendInfoPortMessage(unsigned int bus,
        unsigned int addr, unsigned int port)
{
    /* send incomming GA actions to all elements*/
    emit processInfoPortMessage(bus, addr, port);
}


void GBSArea::getElementByAddress(const int bus, const int address,
        element** el)
{
    for (unsigned int i = 0; i < elements.size(); i++) {
        element* gbse = elements.at(i);

        //TODO: search also bus
        if ((gbse != 0) && gbse->hasSameAddress(bus, address)) {
            *el = gbse;
            break;
        }
    }
}


bool GBSArea::runSRCP08GAInitSequence()
{
    bool returnvalue = false;
    bool CounterChanged = false;

    for (int i = SRCP08GA1InitWalker; i < elements.size(); i++) {

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
        for (int i = SRCP08GA2InitWalker; i < elements.size(); i++) {

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
    int count = 0, busno = 0;
    bool busNoIsKnown;
    int *tempbuslist;

    /*how many different GA busses do we have? */
    for (int i = 0; i < elements.size(); i++) {

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

                    pSRCP08GABusList = (int *) calloc(count, sizeof(int));
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
                    for (int j = 0; j < count; j++) {
                        if (pSRCP08GABusList[j] == busno) {
                            busNoIsKnown = true;
                            break;
                        }
                    }
                    if (!busNoIsKnown) {
                        count++;

                        tempbuslist =
                            (int *) realloc(pSRCP08GABusList,
                                            sizeof(int[count]));

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
        showLogMessage(tr("Layout does not contain a SRCP bus"
                    " configuration for GAs"), MT_INFO, HL_CMND);
    else if (count == 1)
        showLogMessage(tr("Layout contains 1 configured GA bus"),
                MT_INFO, HL_CMND);
    else
        showLogMessage(tr("Layout contains %1 configured GA busses").
                   arg(count), MT_INFO, HL_CMND);

    SRCP08GABusCount = count;
}


void GBSArea::updateSRCP08FBBusList()
{
    int count = 0, busno = 0;
    bool busNoIsKnown;
    int *tempbuslist;

    if (pSRCP08FBBusList != NULL) {
        free(pSRCP08FBBusList);
        pSRCP08FBBusList = NULL;
    }

    /*how many different FB busses do we have? */
    for (int i = 0; i < elements.size(); i++) {

        element* el = elements[i];
        if (el == NULL)
            continue;

        if (!el->isSwitchable())
            continue;

        busno = elements[i]->getFBBusNo();

        if (busno > 0) {
            if (count == 0) {
                count++;
                pSRCP08FBBusList = (int *) calloc(count,
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
                for (int j = 0; j < count; j++) {
                    if (pSRCP08FBBusList[j] == busno) {
                        busNoIsKnown = true;
                        break;
                    }
                }
                if (!busNoIsKnown) {
                    count++;
                    tempbuslist = (int *) realloc(pSRCP08FBBusList,
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
        showLogMessage(tr("Layout does not contain a SRCP bus"
                    " configuration for FBs"), MT_INFO, HL_CMND);
    else if (count == 1)
        showLogMessage(tr("Layout contains 1 configured FB bus"),
                MT_INFO, HL_CMND);
    else
        showLogMessage(tr("Layout contains %1 configured FB busses").
                   arg(count), MT_INFO, HL_CMND);

    SRCP08FBBusCount = count;
}

