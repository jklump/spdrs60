/***************************************************************************
                           gbsarea.cpp
                           version 0.4.8 $Revision: 1.9 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-05-14 20:13:43 $
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

#include <unistd.h>             // for write()
#include <stdlib.h>             // for atoi(), abs()
#include <qprogressdialog.h>

#include "resources.h"
#include "gbsarea.h"
#include "element.h"

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

extern int ROUTING_TIME;
extern bool bFBport[MAX_FB];


GBSArea::GBSArea(QWidget* parent, const char* name)
: QWidget(parent, name)
{
    // set all global layout variables
    cmdHost = "localhost";
    fbHost = "localhost";
    cmdPort = 12345;
    fbPort = 12346;
    cmdLogin = false;
    fbLogin = false;
    
    bRouteWindowActive = false;
    gkbState = kNoneClicked;
    bRecord = false;
    modified = false;
    cols = 0;
    rows = 0;

    iFromSignalIndex = -1;
    iToSignalIndex = -1;
    searchedRoute = kNormal;
    iLastFoundID = 0;
    iConvertCheck = 0;          // variable for check of old route files
    elements.setAutoDelete(true);
    
    listOfActivatePorts = new QStrList(true);
    listOfFromSignals = new QStrList(true);     // create a QStrList for: start
    listOfLockedRoutes = new QStrList(true);    // list of locked routes
    listOfReleasePorts = new QStrList(true);
    listOfRouteTypes = new QStrList(true);
    listOfToSignals = new QStrList(true);       // signals, stop signals and

    routeWindow = NULL;

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
    delete listOfFromSignals;
    delete listOfToSignals;
    delete listOfReleasePorts;
    delete listOfActivatePorts;
    delete listOfLockedRoutes;
    delete listOfRouteTypes;
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


int GBSArea::newFile(int iColumns, int iRows)
{
    cols = iColumns;
    rows = iRows;
    routeFileName = "";

    deleteElements();
    elements.resize(iRows * iColumns);

    QProgressDialog progress(tr("Creating empty layout file"),
                             tr("Abort"), iRows * iColumns,
                             this, "progress", true);
    progress.show();
    
    // deletes a possibly shown old layout

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
            return 1;
        }
    }
    progress.setProgress(iRows * iColumns);
    // now setup and show elements
    setupElements();
    emit updateRoutingViewer("");
    return 0;
}
        

void GBSArea::writeFileTextToStream(QTextStream& ts)
{
    QDateTime dt = QDateTime::currentDateTime();
    
    // write the header
    ts << "# spdrs60 data file" << endl
       << "# version=" << VERSION << endl
       << "# last modified=" << dt.toString(Qt::ISODate) << endl
       << "# layout dimensions=columns:rows" << endl
       << GF_DIMENSIONS << ":" << cols << ":" << rows << endl
       << GF_CMDHOST << ":" << cmdHost << ":" << cmdPort <<
                        ":" << cmdLogin << endl
       << GF_FBHOST << ":" << fbHost << ":" << fbPort << ":" << fbLogin << endl
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
            key = s.section(":", 0, 0);
            value = s.section(":", 1, 1).stripWhiteSpace();
            /* key/value pairs are read sequence independent */
            if (key.compare(GF_DIMENSIONS) == 0){
                cols = value.toInt();
                value = s.section(":", 2, 2).stripWhiteSpace();
                rows = value.toInt();
                ecount = cols * rows;
                elements.resize(ecount);

                /* setup progress dialog */
                progress.setTotalSteps(ecount);

            }
            else if (key.compare(GF_CMDHOST) == 0){
                cmdHost = value;
                value = s.section(":", 2, 2).stripWhiteSpace();
                cmdPort = value.toInt();
            }
            else if (key.compare(GF_FBHOST) == 0){
                fbHost = value;
                value = s.section(":", 2, 2).stripWhiteSpace();
                fbPort = value.toInt();
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
    if (elements.count() != 0)
        loadRoutes(LOCK);
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
    if (elements.count() != 0)
        loadRoutes(LOCK);
}


void GBSArea::loadRoutes(bool bRenewLockList_)
{
    listOfActivatePorts->clear();
    listOfFromSignals->clear();
    listOfReleasePorts->clear();
    listOfRouteTypes->clear();
    listOfToSignals->clear();
    
    if (bRenewLockList_) {
        listOfLockedRoutes->clear();
    }

    // try to open routing file
    QFile file(routeFileName);
    if (!file.open(IO_ReadOnly))        // open the routing file
        return;

    QTextStream ts(&file);
    QString s;
    iConvertCheck = 0;

    // read routing file
    while (!ts.eof()) {
        s = ts.readLine();
        if (!iConvertCheck && s.contains("Route   # -------> 1", 0)) {
            iConvertCheck = true;
            QMessageBox::information(this, tr("Information"),
                tr("Routing file contains old data format.\n"
                   "File has been converted into new format!\n\n"
                   "Please reload this layout."));
            break;
        }

        // and a list of signals to go to
        // "while" is converter from 0.2.x files
        if (s.left(10) == "to signal:") {
            s = s.remove(0, 16);
            while (s.left(1) == "0")
                s = s.remove(0, 1);
            listOfToSignals->append(s);
            if (bRenewLockList_)
                listOfLockedRoutes->append("0");        // "0" = UNLOCKED
        }
        // list of signal to start from
        // "while" is converter from 0.2.x files
        else if (s.left(12) == "from signal:") {
            s = s.remove(0, 16);        
            while (s.left(1) == "0")
                s = s.remove(0, 1);
            listOfFromSignals->append(s.left(s.find(" ", 0, 0)));
        }
        /*setup list with routes released by FB signals */
        else if (s.startsWith("release")) {
            listOfReleasePorts->append(s.section(
                        ":", 1, 1).stripWhiteSpace());
        }
        /*setup list with routes activated by FB signals */
        else if (s.startsWith("activate")) {
            listOfActivatePorts->append(s.section(
                        ":", 1, 1).stripWhiteSpace());
        }
        else if (s.startsWith("type")) {
            listOfRouteTypes->append(s.section(
                        ":", 1, 1).stripWhiteSpace());
        }
    }
    file.close();               // no dis- or enable route buttons

    if (iConvertCheck) {
        QString sConvert = "convertrts " + routeFileName;
        system(sConvert);
    }
    /*FIXME: send update signal to rtViewer (temporary solution)*/
    emit updateRoutingViewer(routeFileName);
}


void GBSArea::slotShowRoutings()
{
    //serd: only create new routing window, if it does not exist
    if (!bRouteWindowActive) {
        // create a new routing window
        routeWindow = new RouteDialog(this, listOfLockedRoutes);
        Q_CHECK_PTR(routeWindow);
        bRouteWindowActive = true;
        routeWindow->setCaption(
                QString(tr("Routings for layout") + " [" + routeFileName + "]"));
        // show the routing file name in the editfilemenu
        emit sigUpdateEditmenu();

        connect(routeWindow, SIGNAL(sendRouteIndex(int, int)),
                this, SLOT(slotStartRouting(int, int)));
        connect(routeWindow, SIGNAL(cmdToDebug(const QString&)),
                this, SIGNAL(cmdToDebug(const QString&)));
        connect(routeWindow, SIGNAL(sendReloadRoutes()),
                this, SLOT(slotUpdateRouteLists()));
        connect(routeWindow, SIGNAL(sigRecord(elemVisualMode)),
                this, SIGNAL(sigRecordMode(elemVisualMode)));
        connect(routeWindow, SIGNAL(sigShowElement(int, int, elemSelectionMode)),
                this, SIGNAL(sigShowElement(int, int, elemSelectionMode)));
        connect(routeWindow, SIGNAL(sigReadElemName(const QString&)),
                this, SLOT(slotReadElemName(const QString&)));
        connect(this, SIGNAL(updateRouteWindow()),
                routeWindow, SLOT(slotUpdateRouteWindow()));
        connect(this, SIGNAL(sigRecordElement(int, const QString&, int, int)),
                routeWindow,
                SLOT(slotRecordElement(int, const QString&, int, int)));

        routeWindow->exec();
    }
    // modeless, you can still use the main prog
    // serd: Nice to get the Routing Window in Front
    routeWindow->setActiveWindow();
    routeWindow->raise();
}


void GBSArea::slotReadElemName(const QString& sReadElemAddr_)
{
    int iReadElemID = -1;
    // get element's name for a certain address
    // to be displayed in routing window
    iReadElemID = locateIndex(sReadElemAddr_, SRCH_A1, SINGLE);

    if (iReadElemID > 0)
        routeWindow->sReadElemName = elements[iReadElemID]->sSoldText;
    else
        routeWindow->sReadElemName = tr("unknown");
}


void GBSArea::slotUpdateRouteLists()
{
    loadRoutes(NOLOCK);
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
            emit cmdToDebug(tr(">MGT-Function not supported."));
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


void GBSArea::slotElementClickedRecord(int idx, int recAction)
{
    emit sigRecordElement(elements[idx]->iSoldAddress_1,
                          elements[idx]->sSoldText,
                          elements[idx]->iSoldDirection, recAction);
    if (idx == 0 && recAction == REC_FINISH) {
        /*switch to normal view mode and clear selections*/
        emit EditMode(kvmNormal);
        emit cmdToDebug(tr(">Route record mode finished"));
    }
}


void GBSArea::slotElementClicked(int iIndex, GbsButtonState gbsButton)
{
    QString s;
    bool routefound = false;

    // Checking for activities of external buttons
    /* this comparison is slightly dangerous:*/
    if (gbsButton >= kFhtClicked) {
        externalButtonClicked(gbsButton);
        return;
    }

    if (kWgtClicked == gkbState) {
        // only toggle a solenoid by means of WGT if it is UNLOCKED
        /*TODO: check if signals may be switched also */
        if (!elements[iIndex]->isLocked())
            // element is occupied
            if (elements[iIndex]->isOccupied()) {
                QApplication::beep();
                emit cmdToDebug(tr
                              (">No switching possible, element is occupied"));
            }
            else
                elements[iIndex]->slotToggle();

        else {
            QApplication::beep();
            emit cmdToDebug(tr(">No switching possible, solenoid '%1' "
                        "is locked by an active route.")
                    .arg(elements[iIndex]->getName()));
        }

        slotElementClickedTimeout();    // reset vars and timer
        return;
    }

    // Check for activated normal route when a signal is clicked
    if (kZfsClicked == gbsButton || kRfsClicked == gbsButton ||
            kZhsClicked == gbsButton) {
        
        if (iFromSignalIndex == -1) {
            /*
               the first signal (route start) is clicked; fist check
               what type of route has to be seached for; each type of
               route has its own cursor what shows up when a
               preconfigured route is found
               normal route and help route are only difficult to
               differentiate
            */

            if (gkbState != kFhtClicked &&
                    elements[iIndex]->isLocked()){
                QApplication::beep();
                emit cmdToDebug(tr(">No routing possible, signal '%1' "
                            "is allready locked by an active route.")
                        .arg(elements[iIndex]->getName()));
                slotElementClickedTimeout();
                return; 
            }

            if (kUfgtClicked == gkbState) {
                if (kZfsClicked == gbsButton)
                    searchedRoute = kDetour;
                else /*if (RfsClicked == gbsButton)*/
                    searchedRoute = kShuntingD;
            }
            else {
                if (kZhsClicked == gbsButton)
                    searchedRoute = kHelp; 
                else if (kZfsClicked == gbsButton)
                    searchedRoute = kNormal;    // or kHelp
                else /*if (RfsClicked == gbsButton)*/
                    searchedRoute = kShunting;
            }

            bool isRouteStart = false;

            /*check if this signal is a start signal of an available route */
            for (unsigned int iRoutingNo = 0;
                 iRoutingNo < listOfFromSignals->count(); iRoutingNo++) {
                
                int FromSigAddr = 
                    QString(listOfFromSignals->at(iRoutingNo)).toInt();
                
                if (elements[iIndex]->hasSameAddress(FromSigAddr)) {
                    
                    RouteType routeType = (RouteType)QString(
                        listOfRouteTypes->at(iRoutingNo)).toInt();
                    
                    if (routeType == searchedRoute){
                        switch (routeType) {
                            case kNormal:
                                setCursor(RZSCursor);
                                break;
                            case kDetour:
                                setCursor(UZSCursor);
                                break;
                            case kHelp:
                                setCursor(ZHSCursor);
                                break;
                            case kShunting:
                                setCursor(RRSCursor);
                                break;
                            case kShuntingD:
                                setCursor(URSCursor);
                                break;
                        }
                        isRouteStart = true;
                        break;
                    }
                    /*TODO: route sequence */
                    else if ((kZfsClicked == gbsButton) &&
                             (routeType == kHelp)){
                        setCursor(ZHSCursor);
                        isRouteStart = true;
                        break;
                    }
                        
                }
            }

            if (isRouteStart) {
                if (gkbState != kFhtClicked)
                    delayTimer->start(cDelayTime);

                // save index value of start signal (== clicked element)
                iFromSignalIndex = iIndex;
            }

            else {
                QApplication::beep();
                s = elements[iIndex]->getName();
                switch (searchedRoute) {
                    case kNormal:
                        emit cmdToDebug(tr(
                                ">No normal route found for start signal '%1'!")
                                .arg(s.data()));
                        break;
                    case kDetour:
                        emit cmdToDebug(tr(
                                ">No detour route found for start signal '%1'!")
                                .arg(s.data()));
                        break;
                    case kHelp:
                        emit cmdToDebug(tr(
                                ">No help route found for start signal '%1'!")
                                .arg(s.data()));
                        break;
                    case kShunting:
                        emit cmdToDebug(tr(
                              ">No shunting route found for start signal '%1'!")
                                .arg(s.data()));
                        break;
                    case kShuntingD:
                        emit cmdToDebug(tr(
                                ">No detour shunting route found for start"
                                " signal '%1'!")
                                .arg(s.data()));
                        break;
                }
                slotElementClickedTimeout();        // reset vars and timer
            }
            return;
        }

        else {
            /* 
             * the second signal (route target) is clicked
             * stop timer, so it can not timeout when routing lasts
             * longer than 2 seconds then save stop signal element index
             * then check whether there is a route with the choosen start-
             * and stop-signals
             */
            /*TODO: check fpr same route type*/

            delayTimer->stop();
            iToSignalIndex = iIndex;
            setCursor(ArrowCursor);

            for (unsigned int iRoutingNo = 0; iRoutingNo <
                 listOfFromSignals->count(); iRoutingNo++) {

                int FromSigAddr = 
                    QString(listOfFromSignals->at(iRoutingNo)).toInt();

                int ToSigAddr = 
                    QString(listOfToSignals->at(iRoutingNo)).toInt();

                int FoundRoute = (RouteType)QString(
                        listOfRouteTypes->at(iRoutingNo)).toInt();

                if (elements[iFromSignalIndex]->hasSameAddress(FromSigAddr) &&
                    elements[iToSignalIndex]->hasSameAddress(ToSigAddr) &&
                    ((searchedRoute == FoundRoute) ||
                     ((kNormal == searchedRoute) && (kHelp == FoundRoute)))
                    ){

                    /*is this route locked?*/
                    QString sListText = listOfLockedRoutes->at(iRoutingNo);
                    bool isLocked = (bool) sListText.toInt();

                    if (isLocked) {
                        if (kFhtClicked == gkbState)
                            /*release locked route */
                            slotStartRouting((int) iRoutingNo,
                                            kFhtClicked == gkbState);
                        else {
                            /*misapplyed operation */
                            QApplication::beep();
                            emit cmdToDebug(tr
                                            (">Route %1 is allready active!")
                                            .arg(iRoutingNo));
                        }
                        routefound = true;
                        break;
                    }
                    /*not locked */
                    else {
                        if (kFhtClicked == gkbState) {
                            /* check each route type if there is more
                             * than one route for this signal; this
                             * typecast is not a very clean solution */
                            if (searchedRoute < kShuntingD){
                                ++(int)searchedRoute;
                                continue;
                            }
                            /*misapplyed operation */
                            QApplication::beep();
                            emit cmdToDebug(tr(">Route %1 is not active!")
                                            .arg(iRoutingNo));
                        }
                        else
                            slotStartRouting((int) iRoutingNo,
                                             kFhtClicked == gkbState);
                        routefound = true;
                        break;
                    }
                }
            } /* end of for loop */
            
            if (!routefound) {
                QApplication::beep();
                s = elements[iFromSignalIndex]->getName();
                QString tS = elements[iToSignalIndex]->getName();
                QString RtName;
                
                switch (searchedRoute){
                    case kNormal:
                        RtName = tr("normal route");
                        break;
                    case kDetour:
                        RtName = tr("detour route");
                        break;
                    case kHelp:
                        RtName = tr("help route");
                        break;
                    case kShunting:
                        RtName = tr("shunting route");
                        break;
                    case kShuntingD:
                        RtName = tr("detour shunting route");
                        break;
                }

                emit cmdToDebug(tr
                        (">No %1 found from %2 to %3!")
                        .arg(RtName)
                        .arg(s)
                        .arg(tS));
            }
            

            slotElementClickedTimeout();        // reset vars and timer
            return;
        }
    }
}


void GBSArea::slotElementClickedTimeout()
{
    iFromSignalIndex = -1;      // set back all click-related variables
    iToSignalIndex = -1;
    searchedRoute = kNormal;
    gkbState = kNoneClicked;
    setCursor(ArrowCursor);
}


void GBSArea::slotStartRouting(int iRouteIndex_, int iSet_)
{
    // iSet == 1 -> FHT clicked -> RESET
    // iSet == 0 -> no FHT      -> SET
    // read the routing data for desired route only out of routing file
    QFile file(routeFileName);
    if (!file.open(IO_ReadOnly))
        return;
    
    QStrList* listOfSolenoids_Number = new QStrList(true);
    QStrList* listOfSolenoids_Action = new QStrList(true);
    int iRouteNo = -1;

    // store all addresses and new direction into the two QStrList's
    QTextStream ts(&file);
    while (!ts.eof()) {

        QString s = ts.readLine();
        // finds beginning of a route section
        if (s.contains("ROUTE -------->", 0)) { 
            iRouteNo += 1;
            // dummy-read "name:" entry not rele-
            // vant here, only for route window
            s = ts.readLine();
        }
        if (iRouteNo == iRouteIndex_) {
            if ((s.startsWith("switch x to y:")) ||
                (s.startsWith("from signal:"))) {
                // if these are the "switch ..."-lines, also append start signal
                s = s.remove(0, 16);    // "while" is converter from 0.2.x files
                while (s.left(1) == "0")
                    s = s.remove(0, 1);
                // separate the address and direction
                listOfSolenoids_Number->append(s.left(s.find(" ", 0, 0)));
                listOfSolenoids_Action->append(s.right(1));
            }
        }
    }
    file.close();

    int ID;
    // RESET a route
    if (iSet_ == RESET) {
        QString s;
        s.sprintf(tr(">Resetting route # %d."), iRouteIndex_ + 1);
        emit cmdToDebug(s);
        listOfLockedRoutes->remove(iRouteIndex_);
        listOfLockedRoutes->insert(iRouteIndex_, "0");  // UNLOCKED

        for (unsigned int i = 0; i < listOfSolenoids_Number->count(); i++) {
            // UNLOCK all addresses from the routing list, only the last item
            // == start signal must be UNLOCKED and set to Hp0!
            // that is wrong because it ignores shunting signals as part
            // of the route; was corrected in 0.4.7
            ID = locateIndex(listOfSolenoids_Number->at(i), SRCH_A1,
                             SINGLE);
            
            /*only switch signals in the route back to the old state*/
            bool doSwitch = elements[ID]->sSoldIcon.startsWith("signal");

            elements[ID]->
                slotSwitchIt(doSwitch ? 0 : elements[ID]->iSoldDirection, -1);
        }
        showLEDs(iRouteIndex_, RESET);
    }
    else if (iSet_ == SET)      // SET a route
        setRoute(iRouteIndex_, listOfSolenoids_Number,
                 listOfSolenoids_Action);

    if (bRouteWindowActive)     // update an open route window
        emit updateRouteWindow();

    QApplication::beep();
    /*remove stringlists, new in 0.4.7*/
    delete listOfSolenoids_Number;
    delete listOfSolenoids_Action;
}


void GBSArea::setRoute(int iRouteIndex_,
                       QStrList* listOfSolenoids_Number_,
                       QStrList* listOfSolenoids_Action_)
{
    int ID;
    QString s;
    // now check if at least one of the routing solenoids is locked by another
    // route and the actual direction is different from the new direction, or
    // if we have a 2-state-DKW which is locked,
    // then exit immediately; only do that if you want to SET a route
    // RESETting a route does not require this!
    for (unsigned int i = 0; i < listOfSolenoids_Number_->count(); i++) {
        ID = locateIndex(listOfSolenoids_Number_->at(i), SRCH_A1, SINGLE);
        int newDir = QString(listOfSolenoids_Action_->at(i)).toInt();

        if ((elements[ID]->iSoldDirection != newDir ||
             ((elements[ID]->sSoldIcon == SYM_DKL
               || elements[ID]->sSoldIcon == SYM_DKR)
              && elements[ID]->iSoldSubType == 0))
            && elements[ID]->iSoldLocked) {
            s.sprintf(tr
                      (">No routing possible, route # %d is locked by "
                       "another route."), iRouteIndex_ + 1);
            QApplication::beep();
            emit cmdToDebug(s);
            return;
        }
    }

    listOfLockedRoutes->remove(iRouteIndex_);
    listOfLockedRoutes->insert(iRouteIndex_, "1");      // LOCKED

    s.sprintf(tr(">Start routing of route # %d."), iRouteIndex_ + 1);
    emit cmdToDebug(s);

    for (unsigned int i = 0; i < listOfSolenoids_Number_->count(); i++) {
        ID = locateIndex(listOfSolenoids_Number_->at(i), SRCH_A1, SINGLE);

        QString sAction;
        sAction = listOfSolenoids_Action_->at(i);

        // correct a wrong signal direction entry in the routing file
        // correct value is NOT written back to the file
        if (elements[ID]->sSoldIcon == SYM_HS
            || elements[ID]->sSoldIcon == SYM_HSS
            || elements[ID]->sSoldIcon == SYM_VS) {
            if ((elements[ID]->iSoldSubType == 0
                 || elements[ID]->iSoldSubType == 1)
                && sAction == DIR_HP2) {
                s.sprintf(tr
                          (">Directional value of signal # %s in route # %d is"
                           " wrong. Correct value \"2\" to value \"1\"."),
                          listOfSolenoids_Number_->at(i),
                          iRouteIndex_ + 1);
                QApplication::beep();
                emit cmdToDebug(s);
                listOfSolenoids_Action_->remove(i);
                listOfSolenoids_Action_->insert(i, "1");
            }
            else if ((elements[ID]->iSoldSubType == 6
                      || elements[ID]->iSoldSubType == 7)
                     && sAction == DIR_HP1) {
                s.sprintf(tr
                          (">Directional value of signal # %s in route "
                           "# %d is wrong. Correct value \"1\" to "
                           "value \"2\"."), listOfSolenoids_Number_->at(i),
                          iRouteIndex_ + 1);
                QApplication::beep();
                emit cmdToDebug(s);
                listOfSolenoids_Action_->remove(i);
                listOfSolenoids_Action_->insert(i, "2");
            }
        }

        // correct a wrong ekw direction entry in the routing file
        // correct value is NOT written back to the file
        // 3 means a turnout at the no-possible branches
        if ((elements[ID]->sSoldIcon == SYM_EKL ||
             elements[ID]->sSoldIcon == SYM_EKR) && sAction.toInt() == 3) {
            // DIR_SKU not allowed, use DIR_SKO instead
            s.sprintf(tr(">Directional value of single-cross turnout # %s "
                         "in route # %d is not allowed with EKL/EKR. Correct value \"3\""
                         "to value \"1\"."),
                      listOfSolenoids_Number_->at(i), iRouteIndex_ + 1);
            QApplication::beep();
            emit cmdToDebug(s);
            listOfSolenoids_Action_->remove(i);
            listOfSolenoids_Action_->insert(i, "1");
        }

        // wait a little bit (= 200 ms) that the user can enjoy the "GBS feeling"
        // no break here 'cause layout can contain two element with the
        // same address (e.g. if one element is repeated)
        elements[ID]->slotSwitchIt(atoi(listOfSolenoids_Action_->at(i)),
                                     LOCKED);
        usleep(1000 * ROUTING_TIME);    // here timer activated switching
    }
    showLEDs(iRouteIndex_, SET);
}


void GBSArea::showLEDs(int iRouteIndex_, int iSet_)
{
    // we exactly have to activate so much elements as we have columns between
    // start and stopsignal (including signal elements itself)
    // this means: we cannot route in circles or over the layout edges!
    // now we call the function in element to determine the next element in
    // layout, this function also calls the setup of the activated icon;
    // there's no solenoid in the 3 different rail crossings, so we just
    // do the function to setup the icon and keep the correction factor
    // from the calculations one element before
    // SET=0, RESET=1, but we need either ROUTE_NONE=0 or ROUTE_SHOW=1;
    // so we use the negation of iSet
    int iFromIndex = locateIndex(listOfFromSignals->at(iRouteIndex_),
                                 SRCH_A1, SINGLE);
    int iToIndex = locateIndex(listOfToSignals->at(iRouteIndex_),
                               SRCH_A1, SINGLE);
    bool bRouteDir = !(elements[iFromIndex]->iSoldRotate);
    int iCorr = 0;
    int iIndex = iFromIndex;

    for (int k = 0;
         k < abs(iFromIndex / rows - iToIndex / rows) + 1; k++) {

        if ((iIndex < 0) || (iIndex >= (int) elements.size())) {
            cmdToDebug(tr
                       (">Try to route over a forbidden element (No %1). "
                        "Check your entries in routing-file!").
                       arg(iIndex));
            break;
        }

        if (elements[iIndex]->sSoldIcon == SYM_LEE) {
            QApplication::beep();
            cmdToDebug(tr(">Try to route over an empty element (No %1). "
                          "Check your entries in routing-file!").
                       arg(iIndex));
            break;
        }

        iCorr = elements[iIndex]->routeElement(bRouteDir, !iSet_, iCorr);
        iIndex +=
            (rows * ((bRouteDir == 1) - (bRouteDir == 0)) + iCorr);

    }
}


int GBSArea::locateIndex(const QString& sLocateString_, int iLocateType_,
                         int iMultiple_)
{
    // search all elements for the desired addresses or text and
    // return its index
    QString s = "";
    int iFound = 0;

    // locate element with certain address 1
    if (iLocateType_ == SRCH_A1) 
        for (unsigned int iIndex = 0; iIndex < elements.size(); iIndex++) {
            if (sLocateString_.toInt() ==
                    elements[iIndex]->iSoldAddress_1) {
                iFound += 1;
                if (iFound != iMultiple_ + 1)
                    continue;
                else
                    return iIndex;
            }
        }

    // locate element with certain address 2
    else if (iLocateType_ == SRCH_A2) 
        for (unsigned int iIndex = 0; iIndex < elements.size(); iIndex++) {
            if (sLocateString_.toInt() ==
                    elements[iIndex]->iSoldAddress_2) {
                iFound += 1;
                if (iFound != iMultiple_ + 1)
                    continue;
                else
                    return iIndex;
            }
        }

    // locate element with certain textfield
    else if (iLocateType_ == SRCH_TX) 
        for (unsigned int iIndex = 0; iIndex < elements.size(); iIndex++) {
            s = elements[iIndex]->sSoldText;
            if (s.contains(sLocateString_, 0)) {
                iFound += 1;
                if (iFound != iMultiple_ + 1)
                    continue;
                else
                    return iIndex;
            }
        }

    // element 0 must be none-switching all the time
    return 0;
}


void GBSArea::slotUnlockRoutings()
{
    QString sListText;
    // reset all routes which are active
    for (int i = 0; i < (int) listOfLockedRoutes->count(); i++) {
        sListText = listOfLockedRoutes->at(i);
        if (sListText.toInt() == LOCKED)
            slotStartRouting(i, RESET);
    }
    QApplication::beep();
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
    for (unsigned int j = 0; j < elements.size(); j++) // toggles all elements
        if (elements[j] != NULL)
            if (!(elements[j]->sSoldIcon == SYM_ENK
                        && elements[j]->iSoldSubType != -1) &&
                    //GBSElement[j]->sSoldIcon != SYM_ENK &&
                    elements[j]->sSoldIcon != SYM_MDC &&
                    elements[j]->sSoldIcon != SYM_SBN &&
                    elements[j]->sSoldIcon != SYM_DRE)
                elements[j]->sendState();
    QApplication::beep();
}


void GBSArea::slotNotrot()
{
    // sets all signals to red state
    for (unsigned int j = 0; j < elements.size(); j++)
        if (elements[j]->sSoldIcon == SYM_HS ||
            elements[j]->sSoldIcon == SYM_HSS ||
            elements[j]->sSoldIcon == SYM_SS ||
            elements[j]->sSoldIcon == SYM_SSH ||
            elements[j]->sSoldIcon == SYM_SSS ||
            elements[j]->sSoldIcon == SYM_WS ||
            elements[j]->sSoldIcon == SYM_VS ||
            elements[j]->sSoldIcon == SYM_ZP)
            elements[j]->slotSwitchIt(0, 0);  // sec. "0" = NONE (RouteStatus)
    emit cmdToDebug(tr(">Switched all signals to halt/stop"));
}


void GBSArea::deleteElements()
{
    closeRouteWindow();
    elements.clear();
    move(0, 0);
    updateGeometry();
    iConvertCheck = false;
}


void GBSArea::closeRouteWindow()
{
    if ((bRouteWindowActive) && (routeWindow != NULL)) {
        // force kill to routing table window deletes all route information,
        // e.g. if a new layout is created or loaded
        // NOT called if user closes the window
        routeWindow->close(true);
        routeWindow = NULL;
        bRouteWindowActive = false;
        listOfActivatePorts->clear();
        listOfFromSignals->clear();
        listOfLockedRoutes->clear();
        listOfReleasePorts->clear();
        listOfRouteTypes->clear();
        listOfToSignals->clear();
    }
}


void GBSArea::setupElements()
{
    for (unsigned int j = 0; j < elements.size(); j++) {
        if (elements[j] != 0) {
            elements[j]->show();  // now show the elements
            connect(elements[j], SIGNAL(elementClicked(int, GbsButtonState)),
                    this, SLOT(slotElementClicked(int, GbsButtonState)));
            connect(elements[j], SIGNAL(sigElementClickedRecord(int, int)),
                    this, SLOT(slotElementClickedRecord(int, int)));
            connect(elements[j], SIGNAL(sendCommand(const QString&)),
                    this, SIGNAL(sendCommand(const QString&)));
            connect(elements[j], SIGNAL(setRepeatIcon(const QString&)),
                    this, SIGNAL(setRepeatIcon(const QString&)));
            connect(elements[j], SIGNAL(sigShowFBmodules()),
                    this, SIGNAL(sigShowFBmodules()));

            connect(this, SIGNAL(EditMode(elemVisualMode)),
                    elements[j], SLOT(slotEditMode(elemVisualMode)));
            connect(this, SIGNAL(sigRecordMode(elemVisualMode)),
                    elements[j], SLOT(slotRecordMode(elemVisualMode)));
            connect(this, SIGNAL(switchToRouteViewMode()),
                    elements[j], SLOT(switchToRouteViewMode()));
            connect(this, SIGNAL(sigShowElement(int, int,
                            elemSelectionMode)),
                    elements[j], SLOT(slotShowElement(int, int,
                            elemSelectionMode)));
            connect(this, SIGNAL(FBportChanged(unsigned int)),
                    elements[j], SLOT(slotOccupyElement(unsigned int)));
            connect(this, SIGNAL(setRepeatIcon(const QString&)),
                    elements[j], SLOT(slotRepeatIcon(const QString&)));
            connect(this, SIGNAL(sigRepaintLayout()),
                    elements[j], SLOT(slotRepaintLayout()));
        }
    }
    move(0, 0);
    updateGeometry();
}


void GBSArea::slotFBportChanged(unsigned int iPortNr_)
{
    /*do nothing if there's no layout loaded */
    if (elements.count() == 0)
        return;

    // first check if a changed port can reset a route
    // if there are any routes available at all
    // check if routing file exists
    if (!routeFileName.isEmpty()) {

        QString s;

        /* 1) check for routes to release */
        if (!listOfReleasePorts->isEmpty())
            if (listOfReleasePorts->find(s.setNum(iPortNr_)) != -1) {
                QString sRP, sLR;
                for (unsigned int i = 0; i < listOfReleasePorts->count();
                     i++) {
                    /*
                     * now reset a route if:
                     *  - port number equals release value in routing file
                     *  - if port changed from 0 to 1
                     *  - if this route is in SET state (= is active)
                     * there may be more than one route to be released with same
                     * FB port, so do NOT break the for() loop!
                     */
                    sRP = listOfReleasePorts->at(i);
                    sLR = listOfLockedRoutes->at(i);

                    if (iPortNr_ == sRP.toUInt() && bFBport[iPortNr_] == 1
                        && sLR == "1")
                        slotStartRouting(i, RESET);
                }
            }

        /* 2) check for routes to activate */
        if (!listOfActivatePorts->isEmpty())
            if (listOfActivatePorts->find(s.setNum(iPortNr_)) != -1) {
                QString sRP, sLR;
                for (unsigned int i = 0; i < listOfActivatePorts->count();
                     i++) {
                    /*
                     * now activate a route if:
                     *  - port number equals activate value in routing file
                     *  - if port changed from 0 to 1
                     *  - if this route is in UNLOCKED state (= is not active)
                     * there may be more than one route to be activated with
                     * same FB port, so do NOT break the for() loop!
                     */
                    sRP = listOfActivatePorts->at(i);
                    sLR = listOfLockedRoutes->at(i);

                    if (iPortNr_ == sRP.toUInt() && bFBport[iPortNr_] == 1
                        && sLR == "0")
                        slotStartRouting(i, SET);
                }
            }
    }
    /*routeFileName*/
    /*at last inform all elements to switch the track-LEDs to correct colour */
    emit FBportChanged(iPortNr_);
}


void GBSArea::slotEditFind(const QString& sSearch_, int iType_, bool bMultiple_)
{
    int iElemID = -1, i = 0, bFound = 0;
    do {
        iElemID = locateIndex(sSearch_, iType_, i++);
        // search for first or all occurence (es)
        // of desired element data
        if (iElemID != 0) {
            bFound = 1;
            elements[iElemID]->locateMe();
        }
    }
    while (iElemID != 0 && bMultiple_ == MULTI);

    // no element could be located -> show this information
    if (!bFound)
        QMessageBox::information(this, tr("Locate error"),
                                 tr("There's no element which\n"
                                    "matches your search criteria."));
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
            element* e = item(r, c);
            unsigned int idx = newrows * (c - 1) + r - 1;
            if (e == NULL) {
                /* insert empty element to unoccupied position */
                e = new element(this);
                e->move((c - 1) * EL_WIDTH, (r - 1) * EL_HEIGHT);
                e->show();
                connect(e, SIGNAL(elementClicked(int, GbsButtonState)),
                        this, SLOT(slotElementClicked(int, GbsButtonState)));
                connect(e, SIGNAL(sigElementClickedRecord(int, int)),
                        this, SLOT(slotElementClickedRecord(int, int)));
                connect(e, SIGNAL(sendCommand(const QString&)),
                        this, SIGNAL(sendCommand(const QString&)));
                connect(e, SIGNAL(setRepeatIcon(const QString&)),
                        this, SIGNAL(setRepeatIcon(const QString&)));
                connect(e, SIGNAL(sigShowFBmodules()),
                        this, SIGNAL(sigShowFBmodules()));
                connect(this, SIGNAL(EditMode(elemVisualMode)),
                        e, SLOT(slotEditMode(elemVisualMode)));
                connect(this, SIGNAL(sigRecordMode(int)),
                        e, SLOT(slotRecordMode(int)));
                connect(this, SIGNAL(switchToRouteViewMode()),
                        e, SLOT(switchToRouteViewMode()));
                connect(this, SIGNAL(sigShowElement(int, int,
                                elemSelectionMode)),
                        e, SLOT(slotShowElement(int, int,
                                elemSelectionMode)));
                connect(this, SIGNAL(FBportChanged(unsigned int)),
                        e, SLOT(slotOccupyElement(unsigned int)));
                connect(this, SIGNAL(setRepeatIcon(const QString&)),
                        e, SLOT(slotRepeatIcon(const QString&)));
                connect(this, SIGNAL(sigRepaintLayout()),
                        e, SLOT(slotRepaintLayout()));
            }
            e->setIndexNo(idx);
            /*TODO: set current edit mode */
            tmpelements.insert(idx, e);
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


void GBSArea::setRouteFileName(const QString& fn)
{
    if (fn.isEmpty()) {
        routeFileName = "";
        return;
    }

    int pos = fn.findRev(GF_GBSEXT);
    if (pos == -1)
        pos = fn.findRev(GF_OLDGBSEXT);

    if (pos != -1){
        routeFileName = fn.left(pos);
        routeFileName.append(RTS_FILE_SUFFIX);
    }
    else 
        routeFileName = "";
    //fprintf(stderr, "routefn: %s\n", routeFileName.data());
}


QString GBSArea::getRouteFileName()
{
    return routeFileName;
}

