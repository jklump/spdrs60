/***************************************************************************
                           gbsarea.cpp
                           version 0.4.7
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : 2005-01-18
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

/*cursor pixmaps*/
#include "pixmaps/cursor_wgt_b.xpm"
#include "pixmaps/cursor_wgt_m.xpm"
#include "pixmaps/cursor_fht_b.xpm"
#include "pixmaps/cursor_fht_m.xpm"
#include "pixmaps/cursor_ufgt_b.xpm"
#include "pixmaps/cursor_ufgt_m.xpm"
#include "pixmaps/cursor_mgt_b.xpm"
#include "pixmaps/cursor_mgt_m.xpm"

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

extern int ROUTING_TIME;
extern bool bFBport[MAX_FB];


GBSArea::GBSArea(QWidget* parent, const char* name)
:QWidget(parent, name)
{
    bRouteWindowActive = false; // set all global layout variables
    gkbState = kNoneClicked;
    bRecord = false;
    modified = false;

    iFromSignalIndex = -1;
    iToSignalIndex = -1;
    searchedRoute = kNormal;
    iLastFoundID = 0;
    iConvertCheck = 0;          // variable for check of old route files
    iNumOfElements = 0;
    /*GBSElement[MAX_ROWS*MAX_COLS];  should be filled with "0" */

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
    // Size of GBSArea: Spalten * Elementbreite, Zeilen * Elementhöhe
    // return QSize(iNumOfElements/MAX_ROWS * EL_WIDTH,
    //              iNumOfElements%MAX_ROWS * EL_HEIGHT);

    if (iNumOfElements == 0)
        return QSize(0, 0);
    else
        return QSize(((iNumOfElements + 1) / MAX_ROWS) * (EL_WIDTH - 1) +
                     1, (MAX_ROWS * (EL_HEIGHT - 1)) + 1);
}
// *INDENT-ON*


int GBSArea::slotNew(int iColumns)
{
    QProgressDialog progress(tr("Creating empty layout file"),
                             tr("Abort"), MAX_ROWS * iColumns,
                             this, "progress", true);
    progress.show();
    
    
    QStrList* newElementData = new QStrList(true);

    // the data for a new element stays the same for a whole layout
    newElementData->insert(LIST_ID_INDEX, 0);
    newElementData->insert(LIST_ID_ICON, "leer");
    newElementData->insert(LIST_ID_ROTATE, "-1");
    newElementData->insert(LIST_ID_INVERT, "-1");
    newElementData->insert(LIST_ID_DECODER, "-1");
    newElementData->insert(LIST_ID_PROTOCOL, "-1");
    newElementData->insert(LIST_ID_ADDRESS_1, "-1");
    newElementData->insert(LIST_ID_ADDRESS_2, "-1");
    newElementData->insert(LIST_ID_CHACONN_1, "-1");
    newElementData->insert(LIST_ID_CHACONN_2, "-1");
    newElementData->insert(LIST_ID_DIRECTION, "-1");
    newElementData->insert(LIST_ID_SUBTYPE, "-1");
    newElementData->insert(LIST_ID_TEXT, "-1");
    newElementData->insert(LIST_ID_ACTTIME, "-1");
    newElementData->insert(LIST_ID_FBPORT, "0");        //serd
    newElementData->insert(LIST_ID_LEDOFF, "0");        //serd
    newElementData->insert(LIST_ID_DATA_2, "-1");
    newElementData->insert(LIST_ID_DATA_3, "-1");

    deleteElements();           // deletes a possibly shown old layout
    QString sData;

    for (iNumOfElements = 0; iNumOfElements < MAX_ROWS * iColumns;
         iNumOfElements++) {
        // now create the new elements (but do not show them yet)
        newElementData->remove((uint) LIST_ID_INDEX);
        newElementData->insert(LIST_ID_INDEX,
                               sData.setNum(iNumOfElements));
        GBSElement[iNumOfElements] = new element(newElementData, this);
        GBSElement[iNumOfElements]->move((iNumOfElements / MAX_ROWS) *
                                         (EL_WIDTH - 1),
                                         (iNumOfElements % MAX_ROWS) *
                                         (EL_HEIGHT - 1));

        progress.setProgress(iNumOfElements);
        qApp->processEvents();

        if (progress.wasCanceled()) {
            deleteElements();
            delete newElementData;
            return 1;
        }
    }
    progress.setProgress(MAX_ROWS * iColumns);
    setupElements();            // now setup and show elements
    delete newElementData;
    return 0;
}


int GBSArea::slotLoad()
{
    QFile file(FILENAME + GBS_FILE_SUFFIX);
    if (!file.open(IO_ReadOnly))
        return 1;
    deleteElements();           // deletes a possibly shown old layout

    // now the gbs layout is loaded from the GBS_FILE_SUFFIX file
    QTextStream ts(&file);
    QStrList* ElementDataList = new QStrList(true);
    QString sListText;
    QString s = ts.readLine();  // number of total routes
    int totalRoutes = s.remove(0, 16).toInt();

    QProgressDialog progress(tr("Loading layout file:") + " " +
                             FILENAME + GBS_FILE_SUFFIX,
                             tr("Abort"), totalRoutes,
                             this, "progress", TRUE);
    progress.show();

    s = ts.readLine();          // == last modified line
    s = ts.readLine();          // version line ---> TODO
    s = ts.readLine();          // ---------- line

    int iLoadIndex = 0;         // counter for LIST_IDs

    while (!ts.eof()) {
        s = ts.readLine();      // read a line
        // check for separator line
        if (!s.startsWith("-")) {
            ElementDataList->insert(iLoadIndex, s.remove(0, 16));
            iLoadIndex += 1;    // store data in QStrList at LIST_ID
        }
         // whole data for one element collected
        // now create the new elements (but do not show them yet)
        else { 
            GBSElement[iNumOfElements] =
                new element(ElementDataList, this);
            GBSElement[iNumOfElements]->move((iNumOfElements / MAX_ROWS) *
                                             (EL_WIDTH - 1),
                                             (iNumOfElements % MAX_ROWS) *
                                             (EL_HEIGHT - 1));

            progress.setProgress(iNumOfElements);
            qApp->processEvents();
            ElementDataList->clear();       // clear QStrList for new data
            iNumOfElements += 1;
            iLoadIndex = 0;

            if (progress.wasCanceled()) {
                deleteElements();
                iNumOfElements = 0;
                break;
            }
        }
    }
    file.close();
    delete ElementDataList;
    setModified(false);
    progress.setProgress(totalRoutes);

    setupElements();            // now setup and show elements
    slotSendAll();
    if (iNumOfElements != 0)    // if layout successfully loaded ...
        loadRoutes(LOCK);       // ... try to load routings
    return (iNumOfElements) ? 0 : 1;    // 0 = o.k., 1 = error. //dirk
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
    QFile file(FILENAME + RTS_FILE_SUFFIX);
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
        if (s.left(10) == "to signal:") {
            // "while" is converter from 0.2.x files
            s = s.remove(0, 16);
            while (s.left(1) == "0")
                s = s.remove(0, 1);
            listOfToSignals->append(s);
            if (bRenewLockList_)
                listOfLockedRoutes->append("0");        // "0" = UNLOCKED
        }
        // a list of signal to start from, add start signal address
        else if (s.left(12) == "from signal:") {
            s = s.remove(0, 16);        
            while (s.left(1) == "0")    // "while" is converter from 0.2.x files
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
        QString sConvert = "convertrts " + FILENAME + RTS_FILE_SUFFIX;
        system(sConvert);
    }
}


int GBSArea::slotSave()
{
    QFile file(FILENAME + GBS_FILE_SUFFIX);
    if (!file.open(IO_WriteOnly)) {     //supply return code. //dirk
        return (QMessageBox::information(this, tr("Error"),
                                         tr("Cannot save file\n") +
                                         FILENAME + GBS_FILE_SUFFIX +
                                         tr("\nContinue anyway?"),
                                         tr("&Yes"), tr("&No"), 0, 0,
                                         1)) ? 2 : 1;
    }

    QString s;
    s.sprintf(tr(">Writing layout: %s%s"), FILENAME.data(),
              GBS_FILE_SUFFIX);
    emit cmdToDebug(s);
    QDateTime dt = QDateTime::currentDateTime();
    QTextStream ts(&file);

    ts << "Symbols total #:" << iNumOfElements << endl; // write the header
    ts << "Last modified:  " << dt.toString() << endl;
    ts << "version:        " << VERSION << endl;
    ts << "-------------------------------------" << endl;

    for (int i = 0; i < iNumOfElements; i++) {
        ts << "SYMBOL -------> " << i << endl;
        GBSElement[i]->writeFileTextToStream(ts);
        ts << "-------------------------------------" << endl;
    }

    file.close();
    setModified(false);
    return 0;
}


void GBSArea::slotShowRoutings()
{
    //serd: only create new routing window, if it does not exist
    if (!bRouteWindowActive) {
        // create a new routing window
        routeWindow = new RouteDialog(this, listOfLockedRoutes);
        Q_CHECK_PTR(routeWindow);
        bRouteWindowActive = true;
        routeWindow->
            setCaption(QString
                       (tr("Routings for layout") + " [" + FILENAME +
                        GBS_FILE_SUFFIX + "]"));
        // show the routing file name in the editfilemenu
        emit sigUpdateEditmenu();

        connect(routeWindow, SIGNAL(sendRouteIndex(int, int)),
                this, SLOT(slotStartRouting(int, int)));
        connect(routeWindow, SIGNAL(cmdToDebug(const QString &)),
                this, SIGNAL(cmdToDebug(const QString &)));
        connect(routeWindow, SIGNAL(sendReloadRoutes()),
                this, SLOT(slotUpdateRouteLists()));
        connect(routeWindow, SIGNAL(sigRecord(int)),
                this, SIGNAL(sigRecordMode(int)));
        connect(routeWindow, SIGNAL(sigShowElement(int, int, int)),
                this, SIGNAL(sigShowElement(int, int, int)));
        connect(routeWindow, SIGNAL(sigReadElemName(QString)),
                this, SLOT(slotReadElemName(QString)));
        connect(this, SIGNAL(updateRouteWindow()),
                routeWindow, SLOT(slotUpdateRouteWindow()));
        connect(this, SIGNAL(sigRecordElement(int, QString, int, int)),
                routeWindow,
                SLOT(slotRecordElement(int, QString, int, int)));
    }
    // modeless, you can still use the main prog
    routeWindow->show();
    // serd: Nice to get the Routing Window in Front
    routeWindow->setActiveWindow();
    routeWindow->raise();
}


void GBSArea::slotReadElemName(QString sReadElemAddr_)
{
    int iReadElemID = -1;
    // get element's name for a certain address
    // to be displayed in routing window
    iReadElemID = locateIndex(sReadElemAddr_, SRCH_A1, SINGLE);

    if (iReadElemID > 0)
        routeWindow->sReadElemName = GBSElement[iReadElemID]->sSoldText;
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
    }
}


void GBSArea::slotElementClickedRecord(int iIndex_, int iType_)
{
    emit sigRecordElement(GBSElement[iIndex_]->iSoldAddress_1,
                          GBSElement[iIndex_]->sSoldText,
                          GBSElement[iIndex_]->iSoldDirection, iType_);
    if (iIndex_ == 0 && iType_ == REC_FINISH)
        slotSendAll();
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
        if (!GBSElement[iIndex]->isLocked())
            // element is occupied
            if (GBSElement[iIndex]->iSoldLEDstate == LED_RED) {
                QApplication::beep();
                emit cmdToDebug(tr
                              (">No switching possible, element is occupied"));
            }
            else
                GBSElement[iIndex]->slotToggle();

        else {
            QApplication::beep();
            emit cmdToDebug(tr(">No switching possible, solenoid '%1' "
                        "is locked by an active route.")
                    .arg(GBSElement[iIndex]->getName()));
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
                    GBSElement[iIndex]->isLocked()){
                QApplication::beep();
                emit cmdToDebug(tr(">No routing possible, signal '%1' "
                            "is allready locked by an active route.")
                        .arg(GBSElement[iIndex]->getName()));
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
                
                if (GBSElement[iIndex]->hasSameAddress(FromSigAddr)) {
                    
                    RouteType routeType = (RouteType)QString(
                        listOfRouteTypes->at(iRoutingNo)).toInt();
                    
    //fprintf(stderr, "2 gkbState: %d, gbsBtn: %d, iIndex: %d, RouteT: %d\n", gkbState, gbsButton, iIndex, routeType);

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
                s = GBSElement[iIndex]->getName();
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

                if (GBSElement[iFromSignalIndex]->hasSameAddress(FromSigAddr) &&
                    GBSElement[iToSignalIndex]->hasSameAddress(ToSigAddr) &&
                    ((searchedRoute == FoundRoute) ||
                     ((kNormal == searchedRoute) && (kHelp == FoundRoute)))
                    ){

                    /*is this route locked?*/
                    QString sListText = listOfLockedRoutes->at(iRoutingNo);
                    bool isLocked = (bool) sListText.toInt();

                    /*fprintf(stderr, "fAddr: %d, tAddr: %d, Lock: %d, "
                            "RouteT: %d\n", iFromSignalIndex,
                            iToSignalIndex, isLocked, routeType);*/

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
                s = GBSElement[iFromSignalIndex]->getName();
                QString tS = GBSElement[iToSignalIndex]->getName();
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
    QFile file(FILENAME + RTS_FILE_SUFFIX);
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
            s = ts.readLine();  // dummy-read "name:" entry not rele-
        }                       // vant here, only for route window
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

        for (uint i = 0; i < listOfSolenoids_Number->count(); i++) {
            // UNLOCK all addresses from the routing list, only the last item
            // == start signal must be UNLOCKED and set to Hp0!
            // that is wrong because it ignores shunting signals as part
            // of the route; was corrected in 0.4.7
            ID = locateIndex(listOfSolenoids_Number->at(i), SRCH_A1,
                             SINGLE);
            
            /*only switch signals in the route back to the old state*/
            bool doSwitch = GBSElement[ID]->sSoldIcon.startsWith("signal");

            GBSElement[ID]->
                slotSwitchIt(doSwitch ? 0 : GBSElement[ID]->iSoldDirection, -1);
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

        /*fprintf(stderr, "i: %d, ID: %d, oDir: %d, nDir: %d\n", i, ID,
                GBSElement[ID]->iSoldDirection, newDir);*/

        if ((GBSElement[ID]->iSoldDirection != newDir ||
             ((GBSElement[ID]->sSoldIcon == SYM_DKL
               || GBSElement[ID]->sSoldIcon == SYM_DKR)
              && GBSElement[ID]->iSoldSubType == 0))
            && GBSElement[ID]->iSoldLocked) {
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

    for (uint i = 0; i < listOfSolenoids_Number_->count(); i++) {
        ID = locateIndex(listOfSolenoids_Number_->at(i), SRCH_A1, SINGLE);

        QString sAction;
        sAction = listOfSolenoids_Action_->at(i);

        // correct a wrong signal direction entry in the routing file
        // correct value is NOT written back to the file
        if (GBSElement[ID]->sSoldIcon == SYM_HS
            || GBSElement[ID]->sSoldIcon == SYM_HSS
            || GBSElement[ID]->sSoldIcon == SYM_VS) {
            if ((GBSElement[ID]->iSoldSubType == 0
                 || GBSElement[ID]->iSoldSubType == 1)
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
            else if ((GBSElement[ID]->iSoldSubType == 6
                      || GBSElement[ID]->iSoldSubType == 7)
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
        if ((GBSElement[ID]->sSoldIcon == SYM_EKL || 
             GBSElement[ID]->sSoldIcon == SYM_EKR) && sAction.toInt() == 3) {
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
        GBSElement[ID]->slotSwitchIt(atoi(listOfSolenoids_Action_->at(i)),
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
    bool bRouteDir = !(GBSElement[iFromIndex]->iSoldRotate);
    int iCorr = 0;
    int iIndex = iFromIndex;

    for (int k = 0;
         k < abs(iFromIndex / MAX_ROWS - iToIndex / MAX_ROWS) + 1; k++) {

        if ((iIndex < 0) || (iIndex >= iNumOfElements)) {
            cmdToDebug(tr
                       (">Try to route over a forbidden element (No %1). "
                        "Check your entries in routing-file!").
                       arg(iIndex));
            break;
        }
        //fprintf(stderr, "k: %d, iCorr: %d, iIndex: %d\n", k, iCorr, iIndex);

        if (GBSElement[iIndex]->sSoldIcon == SYM_LEE) {
            QApplication::beep();
            cmdToDebug(tr(">Try to route over an empty element (No %1). "
                          "Check your entries in routing-file!").
                       arg(iIndex));
            break;
        }

        iCorr = GBSElement[iIndex]->routeElement(bRouteDir, !iSet_, iCorr);
        iIndex +=
            (MAX_ROWS * ((bRouteDir == 1) - (bRouteDir == 0)) + iCorr);

    }
}


int GBSArea::locateIndex(QString sLocateString_, int iLocateType_,
                         int iMultiple_)
{
    // search all elements for the desired addresses or text and
    // return its index
    QString s = "";
    int iFound = 0;

    if (iLocateType_ == SRCH_A1) 
        for (int iIndex = 0; iIndex < iNumOfElements; iIndex++) {
            // locate element with certain address 1
            if (sLocateString_.toInt() ==
                    GBSElement[iIndex]->iSoldAddress_1) {
                iFound += 1;
                if (iFound != iMultiple_ + 1)
                    continue;
                else
                    return iIndex;
            }
        }

    // locate element with certain address 2
    else if (iLocateType_ == SRCH_A2) 
        for (int iIndex = 0; iIndex < iNumOfElements; iIndex++) {
            if (sLocateString_.toInt() ==
                    GBSElement[iIndex]->iSoldAddress_2) {
                iFound += 1;
                if (iFound != iMultiple_ + 1)
                    continue;
                else
                    return iIndex;
            }
        }

    // locate element with certain textfield
    else if (iLocateType_ == SRCH_TX) 
        for (int iIndex = 0; iIndex < iNumOfElements; iIndex++) {
            s = GBSElement[iIndex]->sSoldText;
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
    for (int i = 0; i < (int) listOfLockedRoutes->count(); i++) {                       sListText = listOfLockedRoutes->at(i);
        if (sListText.toInt() == LOCKED)
            slotStartRouting(i, RESET);
    }
    QApplication::beep();
}


void GBSArea::slotToggleAll()
{
    for (int j = 0; j < iNumOfElements; j++)    // toggles all elements
        if (GBSElement[j]->sSoldIcon != SYM_ENK &&      // but no couplers, motors
            GBSElement[j]->sSoldIcon != SYM_MDC &&      // no shifting bridges
            GBSElement[j]->sSoldIcon != SYM_SBN &&      // no turntables
            GBSElement[j]->sSoldIcon != SYM_DRE)
            GBSElement[j]->slotToggle();
    QApplication::beep();
}


void GBSArea::slotSendAll()
{
    for (int j = 0; j < iNumOfElements; j++)    // toggles all elements
        if (!
            (GBSElement[j]->sSoldIcon == SYM_ENK
             && GBSElement[j]->iSoldSubType != -1) &&
//GBSElement[j]->sSoldIcon != SYM_ENK &&
GBSElement[j]->sSoldIcon != SYM_MDC &&
GBSElement[j]->sSoldIcon != SYM_SBN && GBSElement[j]->sSoldIcon != SYM_DRE)
            GBSElement[j]->sendState();
    QApplication::beep();
}


void GBSArea::slotNotrot()
{
    for (int j = 0; j < iNumOfElements; j++)    // sets all signals
        if (GBSElement[j]->sSoldIcon == SYM_HS ||       // to red state
            GBSElement[j]->sSoldIcon == SYM_HSS ||
            GBSElement[j]->sSoldIcon == SYM_SS ||
            GBSElement[j]->sSoldIcon == SYM_SSH ||
            GBSElement[j]->sSoldIcon == SYM_SSS ||
            GBSElement[j]->sSoldIcon == SYM_WS ||
            GBSElement[j]->sSoldIcon == SYM_VS ||
            GBSElement[j]->sSoldIcon == SYM_ZP)
            GBSElement[j]->slotSwitchIt(0, 0);  // sec.  "0" = NONE (RouteStatus)
    emit cmdToDebug(tr(">Switched all signals to halt/stop"));
}


void GBSArea::deleteElements()
{
    // delete previously shown elements if there are any
    if (iNumOfElements != 0) {                           
        closeRouteWindow();
        for (int i = 0; i < iNumOfElements; i++) {
            if (GBSElement[i] != NULL) {
                delete GBSElement[i];
                GBSElement[i] = NULL;
            }
        }
    }
    iNumOfElements = 0;
    move(0, 0);
    updateGeometry();

    iConvertCheck = false;
}


void GBSArea::closeRouteWindow()
{
    if ((bRouteWindowActive) && (routeWindow != NULL)) {
        routeWindow->close(true);       // Force kill to routing table window
        routeWindow = NULL;
        bRouteWindowActive = false;     // deletes all route information, f.e.
        // if a new layout is created or loaded
        listOfActivatePorts->clear();
        listOfFromSignals->clear();
        listOfLockedRoutes->clear();
        listOfReleasePorts->clear();
        listOfRouteTypes->clear();
        listOfToSignals->clear();       // is NOT called if user closes the
    }
}


void GBSArea::setupElements()
{
    for (int j = 0; j < iNumOfElements; j++) {
        GBSElement[j]->show();  // now show the elements
        connect(GBSElement[j], SIGNAL(elementClicked(int, GbsButtonState)),
                this, SLOT(slotElementClicked(int, GbsButtonState)));
        connect(GBSElement[j], SIGNAL(sigElementClickedRecord(int, int)),
                this, SLOT(slotElementClickedRecord(int, int)));
        connect(GBSElement[j], SIGNAL(sendCommand(const QString &)),
                this, SIGNAL(sendCommand(const QString &)));
        connect(GBSElement[j], SIGNAL(setRepeatIcon(QString)),
                this, SIGNAL(setRepeatIcon(QString)));
        connect(GBSElement[j], SIGNAL(sigShowFBmodules()),
                this, SIGNAL(sigShowFBmodules()));

        connect(this, SIGNAL(EditMode(int)),
                GBSElement[j], SLOT(slotEditMode(int)));
        connect(this, SIGNAL(sigRecordMode(int)),
                GBSElement[j], SLOT(slotRecordMode(int)));
        connect(this, SIGNAL(sigShowElement(int, int, int)),
                GBSElement[j], SLOT(slotShowElement(int, int, int)));
        connect(this, SIGNAL(FBportChanged(unsigned int)),
                GBSElement[j], SLOT(slotOccupyElement(unsigned int)));
        connect(this, SIGNAL(setRepeatIcon(QString)),
                GBSElement[j], SLOT(slotRepeatIcon(QString)));
        connect(this, SIGNAL(sigRepaintLayout()),
                GBSElement[j], SLOT(slotRepaintLayout()));
    }
    move(0, 0);
    updateGeometry();
}


void GBSArea::slotFBportChanged(unsigned int iPortNr_)
{
    /*do nothing if there's no layout loaded */
    if (iNumOfElements == 0)
        return;

    // first check if a changed port can reset a route
    // if there are any routes available at all
    // check if routing file exists
    if (FILENAME != "") {

        QString s;

        /*1) check for routes to release */
        if (!listOfReleasePorts->isEmpty())
            if (listOfReleasePorts->find(s.setNum(iPortNr_)) != -1) {
                QString sRP, sLR;
                for (unsigned int i = 0; i < listOfReleasePorts->count();
                     i++) {
                    /*
                     * now reset a route if:
                     *  - the port number equals the release value in routing file
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

        /*2) check for routes to activate */
        if (!listOfActivatePorts->isEmpty())
            if (listOfActivatePorts->find(s.setNum(iPortNr_)) != -1) {
                QString sRP, sLR;
                for (unsigned int i = 0; i < listOfActivatePorts->count();
                     i++) {
                    /*
                     * now activate a route if:
                     *  - the port number equals the activate value in routing file
                     *  - if port changed from 0 to 1
                     *  - if this route is in UNLOCKED state (= is not active)
                     * there may be more than one route to be activated with same
                     * FB port, so do NOT break the for() loop!
                     */
                    sRP = listOfActivatePorts->at(i);
                    sLR = listOfLockedRoutes->at(i);

                    if (iPortNr_ == sRP.toUInt() && bFBport[iPortNr_] == 1
                        && sLR == "0")
                        slotStartRouting(i, SET);
                }
            }
    }
    /*FILENAME*/
   /*at last inform all elements to switch the track-LEDs to correct colour */
        emit FBportChanged(iPortNr_);
}


void GBSArea::slotFind(QString sSearch_, int iType_, bool bMultiple_)
{
    int iElemID = -1, i = 0, bFound = 0;
    do {
        iElemID = locateIndex(sSearch_, iType_, i++);
        // search for first or all occurence (es)
        if (iElemID != 0)       // of desired element data
        {
            bFound = 1;
            GBSElement[iElemID]->locateMe();
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
{
    return modified;
}
// *INDENT-ON*

void GBSArea::setModified(bool m)
{
    if (modified != m)
        modified = m;
}
