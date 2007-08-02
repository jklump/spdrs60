/***************************************************************************
                           mainwindow.cpp
                           version 0.5.2 $Revision: 1.102 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2007 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-08-02 18:55:29 $
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
   this file provides a menubar, a toolbar, and a scrollable view for the
   elements and sets up the connection to the SRCP server
 ***************************************************************************/

#include <stdio.h>              // for sprintf()
#include <stdlib.h>             // for system()
#include <qhbox.h>
#include <qmenubar.h>
#include <qvbox.h>

#include "finder.h"
#include "gbsscrollview.h"
#include "mainwindow.h"
#include "options.h"
#include "preferences.h"
#include "resources.h"

#include "../icons/spdrs60_32.xpm"
/*toolbar icons*/
#include "pixmaps/filenew.xpm"
#include "pixmaps/fileopen.xpm"
#include "pixmaps/filesave.xpm"
#include "pixmaps/filesaveas.xpm"
#include "pixmaps/filenewwindow.xpm"
#include "pixmaps/fileclose.xpm"
#include "pixmaps/filequit.xpm"

#include "pixmaps/editcut.xpm"
#include "pixmaps/editcopy.xpm"
#include "pixmaps/editpaste.xpm"
#include "pixmaps/editfind.xpm"
#include "pixmaps/editoptions.xpm"

#include "pixmaps/daemonconnect.xpm"
#include "pixmaps/daemondisconnect.xpm"
#include "pixmaps/daemonkill.xpm"
#include "pixmaps/daemonreset.xpm"
#include "pixmaps/daemoninfo.xpm"

#include "pixmaps/viewroute.xpm"
#include "pixmaps/viewfeedback.xpm"
#include "pixmaps/viewkeyboard.xpm"
#include "pixmaps/viewclock.xpm"
#include "pixmaps/viewnormalmode.xpm"
#include "pixmaps/viewlayouteditmode.xpm"
#include "pixmaps/viewrouteeditmode.xpm"

#include "pixmaps/layoutstart.xpm"
#include "pixmaps/layoutstop.xpm"
#include "pixmaps/layoutnotrot.xpm"

#include "pixmaps/route_start.xpm"
#include "pixmaps/route_stop.xpm"
#include "pixmaps/route_edit.xpm"
#include "pixmaps/route_new.xpm"
#include "pixmaps/route_copy.xpm"
#include "pixmaps/route_clear.xpm"

/*for srcpCom*/
#define GF_CMDHOST       "cmdhost"
#define GF_FBHOST        "fbhost"
#define GF_FORMATVERSION "formatversion"
#define GF_FV            "2"

/*string constants for personal config file*/
#define CF_SHOWHP2      "showhp2"
#define CF_BLINKINGTURNOUTS "blinkingturnouts"
#define CF_TOOLTIPS     "tooltips"
#define CF_DATATOOLTIPS "datatooltips"
#define CF_ADDRESSLABEL "addresslabel"
#define CF_INITSIGNALS  "initsignals"
#define CF_LAYOUTCOLS   "layoutcolumns"
#define CF_LAYOUTROWS   "layoutrows"
#define CF_SENDSTATE    "sendstate"
#define CF_CONVERTTIME  "converttime"
#define CF_AUTOLOAD     "autoload"
#define CF_AUTOLAYOUT   "autolayout"
#define CF_AUTOSAVE     "autosave"
#define CF_EDITOR       "editor"
#define CF_BROWSER      "browser"
#define CF_PROTOCOL     "protocol"
#define CF_DECODER      "decoder"
#define CF_ACTIVETIME   "activationtime"
#define CF_AUTOTTDIR    "autoturntabledir"
#define CF_TTROUNDTIME  "turntableroundtime"
#define CF_ROUTINGTIME  "routingtime"
#define CF_FEEDBACKTYPE "feedbacktype"
#define CF_FBMODSIZE    "feedbackmodulesize"
#define CF_FBMODTYPE    "feedbackmoduletype"
#define CF_FIXEDBUSNUM  "fixedbusnumbers"
#define CF_FBBUS1       "fbbus1"
#define CF_FBBUS2       "fbbus2"
#define CF_FBBUS3       "fbbus3"
#define CF_FBBUS4       "fbbus4"
#define CF_LASTDIR      "lastdir"

#define SPDRS60_INIT   ".spdrs60rc" // program init filename
#define MAX_HISTORY    100 // max lines in debugging history


MainWindow::MainWindow() : QMainWindow(NULL, "SpDrS60",
        Qt::WDestructiveClose | Qt::WGroupLeader)
{
    setIcon(QPixmap(spdrs60_32));
    /*Networking */
    cmdHost = "localhost";
    fbHost = "localhost";
    cmdPort = 4303;
    fbPort = 4303;
    cmdAutoLogin = false;
    cmdAutoPower = false;
    cmdAutoSendAll = true;
    fbLogin = false;
    CommandPortIsConnected = false;
    FeedbackPortIsConnected = false;
    InfoPortIsConnected = false;
    SRCPCommandState = srcpUndefined;
    SRCPInfoState = srcpUndefined;
    srcpVersion = 7;
    srcpCommandSessionID = 0;
    srcpInfoSessionID = 0;
    LayoutPowerIsOn = false;

    keybWindow = NULL;
    CurrentHL = HL_HINT;            // default debug window ist HISTORY
    isFBInitMode = true;            // var to avoid all startup feedback
    visualMode = kvmNormal;         // normal layout mode
    lastDir = QDir::homeDirPath();  // remembers path for FileOpen
    initMainWindow();               // setup main window with all menus
    readConfigFile();               // read user dependend config file
    initAllSockets();               // init connection to daemon ...
    cmdToDebug(tr("Program succesfully started!"), MT_INFO, HL_HINT);
}

/* Cleanup by destructor */
MainWindow::~MainWindow()
{
    // send LOGOUT command to daemon
    CloseSRCPServerConnection();
    delete CommandSocket;
    delete FeedbackSocket;
    delete InfoSocket;
}

/**
 * read application settings from config file, this is typicaly
 * done on application startup
 */
void MainWindow::readConfigFile()
{
    QFile file(QDir::homeDirPath() + "/" + SPDRS60_INIT);
    if (!file.open(IO_ReadOnly)) {
        /* if no configuration file is found, just keep defaults */
        cmdToDebug(tr("Personal config file not found") + ": ~/" +
                   SPDRS60_INIT, MT_INFO, HL_HINT);
        return;
    }
    QTextStream ts(&file);
    QString s, key, value;

    while (!ts.eof()) {
        s = ts.readLine();
        if (!s.startsWith("#")) {
            key = s.section("=", 0, 0);
            value = s.section("=", 1, 1).stripWhiteSpace();

            if (key.compare(CF_SHOWHP2) == 0){
                pref.hp2 = value.toInt();
            }
            else if (key.compare(CF_BLINKINGTURNOUTS) == 0){
                pref.blinkingturnouts = value.toInt();
            }
            else if (key.compare(CF_TOOLTIPS) == 0){
                pref.tooltips = value.toInt();
            }
            else if (key.compare(CF_DATATOOLTIPS) == 0){
                pref.datatooltips = value.toInt();
            }
            else if (key.compare(CF_ADDRESSLABEL) == 0){
                pref.addresslabeling = value.toInt();
            }
            else if (key.compare(CF_INITSIGNALS) == 0){
                pref.initsignalsred = value.toInt();
            }
            else if (key.compare(CF_LAYOUTCOLS) == 0){
                pref.layoutcols = value.toUInt();
            }
            else if (key.compare(CF_LAYOUTROWS) == 0){
                pref.layoutrows = value.toUInt();
            }
            else if (key.compare(CF_SENDSTATE) == 0){
                pref.sendstate = value.toInt();
            }
            else if (key.compare(CF_CONVERTTIME) == 0){
                pref.converttime = value.toInt();
            }
            else if (key.compare(CF_AUTOLOAD) == 0){
                pref.autoload = value.toInt();
            }
            else if (key.compare(CF_AUTOLAYOUT) == 0){
                pref.autolayout = value;
            }
            else if (key.compare(CF_AUTOSAVE) == 0){
                pref.autosave = value.toInt();
            }
            else if (key.compare(CF_EDITOR) == 0){
                pref.editor = value;
            }
            else if (key.compare(CF_BROWSER) == 0){
                pref.browser = value;
            }
            else if (key.compare(CF_PROTOCOL) == 0){
                pref.protocol = 2;
                if (value == "DCC")
                    pref.protocol = 0;
                else if (value == "Motorola")
                    pref.protocol = 1;
                else if (value == "Selectrix")
                    pref.protocol = 3;
            }
            else if (key.compare(CF_DECODER) == 0){
                pref.decoder = value;
            }
            else if (key.compare(CF_ACTIVETIME) == 0){
                pref.activetime = value.toInt();
            }
            else if (key.compare(CF_AUTOTTDIR) == 0){
                pref.autottdir = value.toInt();
            }
            else if (key.compare(CF_TTROUNDTIME) == 0){
                pref.ttroundtime = value.toDouble();
            }
            else if (key.compare(CF_ROUTINGTIME) == 0){
                pref.routingtime = value.toInt();
            }
            //TODO: remove before release
            else if (key.compare(CF_FEEDBACKTYPE) == 0){
                pref.fbfactor = value.toInt();
            }
            else if (key.compare(CF_FBMODSIZE) == 0){
                pref.fbfactor = value.toInt();
            }
            else if (key.compare(CF_FBMODTYPE) == 0){
                pref.fbmoduletype = value.toInt();
            }
            else if (key.compare(CF_FIXEDBUSNUM) == 0){
                pref.fixedbusnum = value.toInt();
            }
            else if (key.compare(CF_FBBUS1) == 0){
                pref.fbbus1.number = value.section(":", 0, 0).toUInt();
                pref.fbbus1.modules = value.section(":", 1, 1).toUInt();
            }
            else if (key.compare(CF_FBBUS2) == 0){
                pref.fbbus2.number = value.section(":", 0, 0).toUInt();
                pref.fbbus2.modules = value.section(":", 1, 1).toUInt();
            }
            else if (key.compare(CF_FBBUS3) == 0){
                pref.fbbus3.number = value.section(":", 0, 0).toUInt();
                pref.fbbus3.modules = value.section(":", 1, 1).toUInt();
            }
            else if (key.compare(CF_FBBUS4) == 0){
                pref.fbbus4.number = value.section(":", 0, 0).toUInt();
                pref.fbbus4.modules = value.section(":", 1, 1).toUInt();
            }
            else if (key.compare(CF_LASTDIR) == 0){
                lastDir = value.stripWhiteSpace();
                // check if directory is valid
                if (!QFile::exists(lastDir))
                    lastDir = QDir::homeDirPath();
            }
        }
    }
    file.close();
    //tell the feedback viewer about changed feedback module layout
    fbViewer->updateBusAndModuleStructure();
}


/**
 * write application settings to user config file, this is typicaly
 * done when application window is closed
 */
void MainWindow::writeConfigFile()
{
    QFile file(QDir::homeDirPath() + "/" + SPDRS60_INIT);
    
    if (!file.open(IO_WriteOnly)) {
        cmdToDebug(tr("Error: Could not save configuration"
                    " file: ~/%1").arg(SPDRS60_INIT), MT_INFO, HL_HINT);
        return;
    }
    cmdToDebug(tr("Writing SpDrS60 configuration"
                " file: ~/%1").arg(SPDRS60_INIT), MT_INFO, HL_HINT);

    QDateTime dt = QDateTime::currentDateTime();
    QTextStream ts(&file);

    QString rtstr;
    rtstr.sprintf("%.2f", pref.ttroundtime);

    ts  << "# spdrs60 configuration file" << endl
        << "# last modified: " << dt.toString(Qt::ISODate) << endl
        << "#" << endl
        << CF_SHOWHP2      << "=" << (int) pref.hp2 << endl
        << CF_BLINKINGTURNOUTS << "=" << (int) pref.blinkingturnouts << endl
        << CF_TOOLTIPS     << "=" << (int) pref.tooltips << endl
        << CF_DATATOOLTIPS << "=" << (int) pref.datatooltips << endl
        << CF_ADDRESSLABEL    "=" << (int) pref.addresslabeling << endl
        << CF_INITSIGNALS  << "=" << (int) pref.initsignalsred << endl
        << CF_LAYOUTCOLS   << "=" << pref.layoutcols << endl
        << CF_LAYOUTROWS   << "=" << pref.layoutrows << endl
        << CF_SENDSTATE    << "=" << pref.sendstate << endl
        << CF_CONVERTTIME  << "=" << pref.converttime << endl
        << CF_AUTOLOAD     << "=" << (int) pref.autoload << endl
        << CF_AUTOLAYOUT   << "=" << pref.autolayout << endl
        << CF_AUTOSAVE     << "=" << (int) pref.autosave << endl
        << CF_EDITOR       << "=" << pref.editor << endl
        << CF_BROWSER      << "=" << pref.browser << endl
        << CF_PROTOCOL     << "=" <<
            ((pref.protocol == 1) ? "Motorola" 
            : (pref.protocol == 0) ? "DCC"
            : (pref.protocol == 3) ? "Selectrix" : "Server") << endl
        << CF_DECODER      << "=" << pref.decoder << endl
        << CF_ACTIVETIME   << "=" << pref.activetime << endl
        << CF_AUTOTTDIR    << "=" << (int) pref.autottdir << endl
        << CF_TTROUNDTIME  << "=" << rtstr << endl
        << CF_ROUTINGTIME  << "=" << pref.routingtime << endl
        << CF_FBMODSIZE    << "=" << pref.fbfactor << endl
        << CF_FBMODTYPE    << "=" << pref.fbmoduletype << endl
        << CF_FIXEDBUSNUM  << "=" << pref.fixedbusnum << endl
        << CF_FBBUS1       << "=" << pref.fbbus1.number
                           << ":" << pref.fbbus1.modules << endl
        << CF_FBBUS2       << "=" << pref.fbbus2.number
                           << ":" << pref.fbbus2.modules << endl
        << CF_FBBUS3       << "=" << pref.fbbus3.number
                           << ":" << pref.fbbus3.modules << endl
        << CF_FBBUS4       << "=" << pref.fbbus4.number
                           << ":" << pref.fbbus4.modules << endl
        << CF_LASTDIR      << "=" << lastDir << endl;

    file.close();
}

void MainWindow::initMainWindow()
{
    /* 
     * This is the window layout in detail:  (guido)
     +---------------------------------------------------------+
     |       QMainWindow                                       |
     | +-----------------------------------------------------+ |
     | |     QVBox                                           | |
     | | +-------------------------------------------------+ | |
     | | |   QScrollView                                   | | |
     | | | +---------------------------------------------+ | | |
     | | | | GBSArea                                     | | | |
     | | | |                                             | | | |
     | | | |                                             | | | |
     | | | |                                             | | | |
     | | | |                                             | | | |
     | | | |                                             | | | |
     | | | |                                             | | | |
     | | | |                                             | | | |
     | | | |                                             | | | |
     | | | |                                             | | | |
     | | | |                                             | | | |
     | | | +---------------------------------------------+ | | |
     | | +-------------------------------------------------+ | |
     | +-----------------------------------------------------+ |
     | | +-----------------+-------------------------------+ | |
     | | |  QHBox          |                               | | |
     | | | +-------------+ | +---------------------------+ | | |
     | | | |QWidgetStack | | |QWidgetStack+----------+   | | | |
     | | | |   +-------+ | | |            |+----------+  | | | |
     | | | |   |+-------+| | |            +|+----------+ | | | |
     | | | |   +|QLabel || | |             +|QListView | | | | |
     | | | |    +-------+| | |              +----------+ | | | |
     | | | +-------------+ | +---------------------------+ | | |
     | | +-----------------+-------------------------------+ | |
     | +-----------------------------------------------------+ |
     +---------------------------------------------------------+
     */

    QVBox *vBox = new QVBox(this, "vbox", 0);

    GBSScrollView* scrollview = new GBSScrollView(vBox);
    /*gbs*/
    gbs = new GBSArea(scrollview->viewport(), "gbsArea");
    Q_CHECK_PTR(gbs);
    scrollview->addChild(gbs);

    connect(this, SIGNAL(repaintLayout()),
            gbs, SIGNAL(sigRepaintLayout()));
    connect(this, SIGNAL(findElement(const QString&, int, int)),
            gbs, SLOT(slotEditFind(const QString&, int, int)));
    connect(this, SIGNAL(sendFBChangeLayout(unsigned int, unsigned int,
                    bool)),
            gbs, SIGNAL(feedbackPortChanged(unsigned int, unsigned int,
                    bool)));
    connect(this, SIGNAL(switchedVisualMode(elemVisualMode)),
            gbs, SLOT(switchVisualMode(elemVisualMode)));
    connect(gbs, SIGNAL(showLogMessage(const QString&, int, int)),
            this, SLOT(cmdToDebug(const QString&, int, int)));
    connect(gbs, SIGNAL(sendSrcpMessage(SrcpMessage*)),
            this, SLOT(sendSrcpMessage(SrcpMessage*)));
    connect(gbs, SIGNAL(sigShowFBmodules()),
            this, SLOT(slotShowModules()));


    /*history line*/
    // this container should be a separate class:
    QHBox *hBox = new QHBox(vBox, "hbox", 0);
    hBox->setSpacing(2);

    lblStack = new QWidgetStack(hBox, "cbstack");
    lblStack->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
                QSizePolicy::Fixed, false));

    QLabel *HintLabel = new QLabel(lblStack, "Hint");
    HintLabel->setText(tr("Hints"));
    HintLabel->setIndent(3);
    lblStack->addWidget(HintLabel, 0);

    QLabel *CmdLabel = new QLabel(lblStack, "Cmd");
    CmdLabel->setText(tr("Commands"));
    CmdLabel->setIndent(3);
    lblStack->addWidget(CmdLabel, 1);

    QLabel *InfoLabel = new QLabel(lblStack, "Info");
    InfoLabel->setText(tr("Infoport"));
    InfoLabel->setIndent(3);
    lblStack->addWidget(InfoLabel, 2);

    QLabel *FeedBackLabel = new QLabel(lblStack, "Feedb");
    FeedBackLabel->setText(tr("Feedbacks"));
    FeedBackLabel->setIndent(3);
    lblStack->addWidget(FeedBackLabel, 3);

    cbStack = new QWidgetStack(hBox, "cbstack");
    cbStack->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                           QSizePolicy::Fixed, false));

    QFont f;
    f.setFamily("Courier");

    HintCB = new QComboBox(false, cbStack);
    HintCB->setSizeLimit(15);
    HintCB->setFont(f);
    cbStack->addWidget(HintCB, 0);

    CmdCB = new QComboBox(false, cbStack);
    CmdCB->setSizeLimit(15);
    CmdCB->setFont(f);
    cbStack->addWidget(CmdCB, 1);

    InfoCB = new QComboBox(false, cbStack);
    InfoCB->setSizeLimit(15);
    InfoCB->setFont(f);
    cbStack->addWidget(InfoCB, 2);

    FeedBackCB = new QComboBox(false, cbStack);
    FeedBackCB->setSizeLimit(15);
    FeedBackCB->setFont(f);
    cbStack->addWidget(FeedBackCB, 3);
#if QT_VERSION < 0x030200
    // force painting of history lines and their labels
    cbStack->raiseWidget(0);
    lblStack->raiseWidget(0);
#endif

    setCentralWidget(vBox);

    /*feedback module viewer*/
    fbViewer = new FeedbackViewer(this, "feedbackviewer");
    Q_CHECK_PTR(fbViewer);
    moveDockWindow(fbViewer, Left);
    connect(this, SIGNAL(sendFBChangeLayout(unsigned int, unsigned int,
                    bool)),
            fbViewer, SLOT(feedbackPortChanged(unsigned int, unsigned int,
                    bool)));
    fbViewer->hide();
    
    /*route controller*/
    rtController = new Router(this, gbs->getGbsElementListPtr(),
            "rtController");
    Q_CHECK_PTR(rtController);
    connect(this, SIGNAL(switchedVisualMode(elemVisualMode)),
            rtController, SLOT(switchVisualMode(elemVisualMode)));
    connect(this, SIGNAL(sendFBChangeRoute(unsigned int, unsigned int, bool)),
            rtController, SLOT(feedbackPortChanged(unsigned int,
                    unsigned int, bool)));
    connect(rtController, SIGNAL(showLogMessage(const QString&, int,
                    int)),
            this, SLOT(cmdToDebug(const QString&, int, int)));
    connect(gbs, SIGNAL(clearRoutes()),
            rtController, SLOT(clearRoutes()));
    connect(gbs, SIGNAL(recordElement(element*, elemRecordType)),
            rtController, SLOT(recordElement(element*, elemRecordType)));
    connect(gbs, SIGNAL(setRoute(element*, GbsButtonState,
                    GbsButtonState)),
            rtController, SLOT(setRoute(element*, GbsButtonState,
                    GbsButtonState)));
    connect(gbs, SIGNAL(resetRoute(element*, GbsButtonState)),
            rtController, SLOT(resetRoute(element*, GbsButtonState)));
    connect(rtController, SIGNAL(routeFunctionFinished()),
            gbs, SLOT(slotElementClickedTimeout()));
    connect(rtController, SIGNAL(startRouteTimer(TypeOfRoute)),
            gbs, SLOT(startRouteTimer(TypeOfRoute)));
    connect(gbs, SIGNAL(resetSelectedSignal()),
            rtController, SLOT(resetSelectedSignal()));
    connect(rtController, SIGNAL(updateRoutePathLEDs(const stateElement&,
                    const stateElement&, RouteSetAction&)),
            gbs, SLOT(updateRoutePathLEDs(const stateElement&,
                    const stateElement&, RouteSetAction&)));
    
    /*route list window*/
    rtViewer = new RouteListWindow(this, "routeListWindow", rtController);
    Q_CHECK_PTR(rtViewer);
    moveDockWindow(rtViewer, Right);
    rtViewer->hide();
    connect(this, SIGNAL(switchedVisualMode(elemVisualMode)),
            rtViewer, SLOT(switchVisualMode(elemVisualMode)));
    connect(rtViewer, SIGNAL(visibilityChanged(bool)),
            this, SLOT(updateRouteMenu(bool)));
    connect(rtViewer, SIGNAL(selectedRouteChangedState()),
            this, SLOT(updateRouteMenuActivateItems()));
    connect(rtViewer, SIGNAL(showLogMessage(const QString&, int, int)),
            this, SLOT(cmdToDebug(const QString&, int, int)));
    connect(rtViewer, SIGNAL(routeListIsEmpty()),
            this, SLOT(updateRouteListMenuItems()));
    connect(rtController, SIGNAL(routeListChanged()),
            rtViewer, SLOT(updateRouteList()));
    connect(rtController, SIGNAL(routeDataChanged(Route*)),
            rtViewer, SLOT(updateRouteData(Route*)));
    connect(rtController, SIGNAL(routeStateChanged(Route*)),
            rtViewer, SLOT(updateRouteState(Route*)));
    connect(rtController, SIGNAL(getElementByAddress(const int, const int,
                    element**)), gbs,
            SLOT(getElementByAddress(const int, const int, element**)));


    /*file toolbar*/
    QToolBar* filetb = new QToolBar(this, "filetb");
    Q_CHECK_PTR(filetb);
    filetb->setLabel(tr("File operations"));

    /*file menu*/
    QPopupMenu* filemenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&File"), filemenu);

    /*file actions*/
#if QT_VERSION >= 0x030200
    actionFileNew = new QAction(QPixmap(filenew_xpm), tr("&New..."),
            Qt::CTRL + Qt::Key_N, this, "fileNew");
    actionFileNew->setToolTip(tr("Create empty layout"));
#else
    actionFileNew = new QAction(tr("Create empty layout"),
		    QPixmap(filenew_xpm), tr("&New..."), Qt::CTRL + Qt::Key_N,
                    this, "fileNew");
#endif
    connect(actionFileNew, SIGNAL(activated()), this,
            SLOT(slotFileNew()));
    actionFileNew->addTo(filemenu);
    actionFileNew->addTo(filetb);

#if QT_VERSION >= 0x030200
    actionFileOpen = new QAction(QPixmap(fileopen_xpm), tr("&Open..."),
            Qt::CTRL + Qt::Key_O, this, "fileOpen");
    actionFileOpen->setToolTip(tr("Open layout file"));
#else
    actionFileOpen = new QAction(tr("Open layout file"), 
            QPixmap(fileopen_xpm), tr("&Open..."), Qt::CTRL + Qt::Key_O, this,
            "fileOpen");
#endif
    connect(actionFileOpen, SIGNAL(activated()), this,
            SLOT(slotFileOpen()));
    actionFileOpen->addTo(filemenu);
    actionFileOpen->addTo(filetb);

#if QT_VERSION >= 0x030200
    actionFileSave = new QAction(QPixmap(filesave_xpm), tr("&Save"),
            Qt::CTRL + Qt::Key_S, this, "fileSave");
#else
    actionFileSave = new QAction("", QPixmap(filesave_xpm), tr("&Save"),
            Qt::CTRL + Qt::Key_S, this, "fileSave");
#endif
    connect(actionFileSave, SIGNAL(activated()), this,
            SLOT(slotFileSave()));
    actionFileSave->addTo(filemenu);
    actionFileSave->addTo(filetb);

#if QT_VERSION >= 0x030200
    actionFileSaveAs = new QAction(QPixmap(filesaveas_xpm), tr("Save &as..."),
            Qt::CTRL + Qt::Key_A, this, "fileSaveAs");
#else
    actionFileSaveAs = new QAction("", QPixmap(filesaveas_xpm), tr("Save &as..."),
            Qt::CTRL + Qt::Key_A, this, "fileSaveAs");
#endif
    connect(actionFileSaveAs, SIGNAL(activated()), this,
            SLOT(slotFileSaveAs()));
    actionFileSaveAs->addTo(filemenu);
    //actionFileSaveAs->addTo(filetb);

    filemenu->insertSeparator();
    //filetb->addSeparator();

#if QT_VERSION >= 0x030200
    actionFileNewWindow = new QAction(QPixmap(filenewwindow_xpm),
            tr("New &window"), 0, this, "fileNewWindow");
#else
    actionFileNewWindow = new QAction("", QPixmap(filenewwindow_xpm),
            tr("New &window"), 0, this, "fileNewWindow");
#endif
    connect(actionFileNewWindow, SIGNAL(activated()), this,
            SLOT(slotFileNewWin()));
    actionFileNewWindow->addTo(filemenu);
    //actionFileNewWindow->addTo(filetb);

#if QT_VERSION >= 0x030200
    actionFileClose = new QAction(QPixmap(fileclose_xpm), tr("&Close"),
            Qt::CTRL + Qt::Key_W, this, "fileClose");
#else
    actionFileClose = new QAction("", QPixmap(fileclose_xpm), tr("&Close"),
            Qt::CTRL + Qt::Key_W, this, "fileClose");
#endif
    connect(actionFileClose, SIGNAL(activated()), this,
            SLOT(close()));
    actionFileClose->addTo(filemenu);
    //actionFileClose->addTo(filetb);

#if QT_VERSION >= 0x030200
    actionFileQuit = new QAction(QPixmap(filequit_xpm), tr("&Quit"),
            Qt::CTRL + Qt::Key_Q, this, "fileQuit");
#else
    actionFileQuit = new QAction("", QPixmap(filequit_xpm), tr("&Quit"),
            Qt::CTRL + Qt::Key_Q, this, "fileQuit");
#endif
    connect(actionFileQuit, SIGNAL(activated()), qApp,
            SLOT(closeAllWindows()));
    actionFileQuit->addTo(filemenu);
    //actionFileQuit->addTo(filetb);

    /*edit toolbar*/
    QToolBar* edittb = new QToolBar(this, "edittb");
    Q_CHECK_PTR(edittb);
    edittb->setLabel(tr("Edit operations"));

    QPopupMenu* editfilemenu = new QPopupMenu(this);
    
#if QT_VERSION >= 0x030200
    actionEditFileLayout = new QAction(NULL, tr("&Layout"), 0, this,
            "editFileLayout");
    actionEditFileLayout->setToolTip(tr(
                "Edit layout file with external editor"));
#else
    actionEditFileLayout = new QAction(tr("Edit layout file with"
                " external editor"), tr("&Layout"), 0, this,
            "editFileLayout");
#endif
    connect(actionEditFileLayout, SIGNAL(activated()), this,
            SLOT(slotEditGBSFiles()));
    actionEditFileLayout->addTo(editfilemenu);
    //actionEditFileLayout->addTo(edittb);

#if QT_VERSION >= 0x030200
    actionEditFileOptions = new QAction(NULL,
            QDir::homeDirPath() + "/" + SPDRS60_INIT, 0, this,
            "editFileOptions");
    actionEditFileOptions->setToolTip(tr(
                "Edit config file with external editor"));
#else
    actionEditFileOptions = new QAction(tr("Edit config file with "
                "external editor"),
            QDir::homeDirPath() + "/" + SPDRS60_INIT, 0, this,
            "editFileOptions");
#endif
    connect(actionEditFileOptions, SIGNAL(activated()), this,
            SLOT(slotEditConfigFile()));
    actionEditFileOptions->addTo(editfilemenu);
    //actionEditFileOptions->addTo(edittb);


    /*edit menu*/
    QPopupMenu* editmenu = new QPopupMenu(this);
    //editmenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&Edit"), editmenu);

#if QT_VERSION >= 0x030200
    actionEditCut = new QAction(QPixmap(editcut_xpm), tr("Cu&t"),
            Qt::CTRL + Qt::Key_X, this, "editCut");
    actionEditCut->setToolTip(tr("Cut selection to clipboard"));
#else
    actionEditCut = new QAction(tr("Cut selection to clipboard"),
            QPixmap(editcut_xpm), tr("Cu&t"),
            Qt::CTRL + Qt::Key_X, this, "editCut");
#endif
    connect(actionEditCut, SIGNAL(activated()), this,
            SLOT(slotEditCut()));
    actionEditCut->addTo(editmenu);
    actionEditCut->addTo(edittb);
    actionEditCut->setEnabled(false);

#if QT_VERSION >= 0x030200
    actionEditCopy = new QAction(QPixmap(editcopy_xpm), tr("&Copy"),
            Qt::CTRL + Qt::Key_C, this, "editCopy");
    actionEditCopy->setToolTip(tr("Copy selection to clipboard"));
#else
    actionEditCopy = new QAction(tr("Copy selection to clipboard"),
            QPixmap(editcopy_xpm), tr("&Copy"),
            Qt::CTRL + Qt::Key_C, this, "editCopy");
#endif
    connect(actionEditCopy, SIGNAL(activated()), this,
            SLOT(slotEditCopy()));
    actionEditCopy->addTo(editmenu);
    actionEditCopy->addTo(edittb);
    actionEditCopy->setEnabled(false);

#if QT_VERSION >= 0x030200
    actionEditPaste = new QAction(QPixmap(editpaste_xpm), tr("&Paste"),
            Qt::CTRL + Qt::Key_V, this, "editPaste");
    actionEditPaste->setToolTip(tr("Paste from clipboard"));
#else
    actionEditPaste = new QAction(tr("Paste from clipboard"),
            QPixmap(editpaste_xpm), tr("&Paste"),
            Qt::CTRL + Qt::Key_V, this, "editPaste");
#endif
    connect(actionEditPaste, SIGNAL(activated()), this,
            SLOT(slotEditPaste()));
    actionEditPaste->addTo(editmenu);
    actionEditPaste->addTo(edittb);
    actionEditPaste->setEnabled(false);

    editmenu->insertSeparator();
    editmenu->insertItem(tr("&Data files"), editfilemenu);

    editmenu->insertSeparator();
    edittb->addSeparator();

#if QT_VERSION >= 0x030200
    actionEditFind = new QAction(QPixmap(editfind_xpm), tr("&Find..."),
            Qt::CTRL + Qt::Key_F, this, "editFind");
    actionEditFind->setToolTip(tr("Find information in layout element"));
#else
    actionEditFind = new QAction(tr("Find information in layout element"),
            QPixmap(editfind_xpm), tr("&Find..."),
            Qt::CTRL + Qt::Key_F, this, "editFind");
#endif
    connect(actionEditFind, SIGNAL(activated()), this,
            SLOT(slotEditFind()));
    actionEditFind->addTo(editmenu);
    actionEditFind->addTo(edittb);
    //actionEditFind->setEnabled(false);

#if QT_VERSION >= 0x030200
    actionEditOptions = new QAction(QPixmap(editoptions_xpm),
            tr("Pr&eferences..."), Qt::CTRL + Qt::Key_P, this, "editPreferences");
    actionEditOptions->setToolTip(tr("Edit application preferences"));
#else
    actionEditOptions = new QAction("", QPixmap(editoptions_xpm),
            tr("Pr&eferences..."), Qt::CTRL + Qt::Key_P, this, "editPreferences");
#endif
    connect(actionEditOptions, SIGNAL(activated()), this,
            SLOT(slotEditOptions()));
    actionEditOptions->addTo(editmenu);
    //actionEditOptions->addTo(edittb);


    /*view toolbar*/
    QToolBar* viewtb = new QToolBar(this, "viewtb");
    Q_CHECK_PTR(viewtb);
    viewtb->setLabel(tr("View operations"));

    QPopupMenu* viewmenu = new QPopupMenu(this);
    //viewmenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&View"), viewmenu);

#if QT_VERSION >= 0x030200
    actionViewFBModules = new QAction(QPixmap(viewfeedback_xpm),
            tr("&Feedback modules"), Qt::CTRL + Qt::Key_M, this, "viewFBModules");
    actionViewFBModules->setToolTip(tr("Show feedback module window"));
#else
    actionViewFBModules = new QAction(tr("Show feedback module window"),
            QPixmap(viewfeedback_xpm),
            tr("&Feedback modules"), Qt::CTRL + Qt::Key_M, this, "viewFBModules");
#endif
    connect(actionViewFBModules, SIGNAL(activated()), this,
            SLOT(slotShowModules()));
    actionViewFBModules->addTo(viewmenu);
    actionViewFBModules->addTo(viewtb);

#if QT_VERSION >= 0x030200
    actionViewRoutes = new QAction(QPixmap(viewroute_xpm),
            tr("Routing &table"), Qt::CTRL + Qt::Key_R, this, "viewRoutes");
    actionViewRoutes->setToolTip(tr("Show routing table"));
#else
    actionViewRoutes = new QAction(tr("Show routing table"),
            QPixmap(viewroute_xpm),
            tr("Routing &table"), Qt::CTRL + Qt::Key_R, this, "viewRoutes");
#endif
    connect(actionViewRoutes, SIGNAL(activated()), this,
            SLOT(slotShowRoutes()));
    actionViewRoutes->addTo(viewmenu);
    actionViewRoutes->addTo(viewtb);

#if QT_VERSION >= 0x030200
    actionViewClock = new QAction(QPixmap(viewclock_xpm),
            tr("&Central clock"), 0, this, "viewClock");
    actionViewClock->setToolTip(tr("Show central clock"));
#else
    actionViewClock = new QAction(tr("Show central clock"),
            QPixmap(viewclock_xpm),
            tr("&Central clock"), 0, this, "viewClock");
#endif
    connect(actionViewClock, SIGNAL(activated()), this,
            SLOT(slotShowClock()));
    actionViewClock->addTo(viewmenu);
    actionViewClock->addTo(viewtb);

#if QT_VERSION >= 0x030200
    actionViewKeyboard = new QAction(QPixmap(viewkeyboard_xpm),
            tr("&Keyboard"), Qt::CTRL + Qt::Key_K, this, "viewKeyboard");
    actionViewKeyboard->setToolTip(tr("Show basic keyboard"));
#else
    actionViewKeyboard = new QAction(tr("Show basic keyboard"),
            QPixmap(viewkeyboard_xpm),
            tr("&Keyboard"), Qt::CTRL + Qt::Key_K, this, "viewKeyboard");
#endif
    connect(actionViewKeyboard, SIGNAL(activated()), this,
            SLOT(slotViewKeyboard()));
    actionViewKeyboard->addTo(viewmenu);
    actionViewKeyboard->addTo(viewtb);
    
    viewmenu->insertSeparator();
    viewtb->addSeparator();

    QActionGroup *ViewGrp = new QActionGroup(this);
    connect(ViewGrp, SIGNAL(selected(QAction*)), this,
            SLOT(slotViewSwitchMode(QAction*)));
    
#if QT_VERSION >= 0x030200
    actionViewNormalMode = new QAction(QPixmap(viewnormalmode_xpm),
            tr("&Normal mode"), Qt::CTRL + Qt::Key_L, ViewGrp, "normalmode");
#else
    actionViewNormalMode = new QAction("", QPixmap(viewnormalmode_xpm),
            tr("&Normal mode"), Qt::CTRL + Qt::Key_L, ViewGrp, "normalmode");
#endif
    actionViewNormalMode->setToggleAction(true);
    
#if QT_VERSION >= 0x030200
    actionViewLayoutEditMode = new QAction(QPixmap(viewlayouteditmode_xpm),
            tr("&Layout edit mode"), Qt::CTRL + Qt::Key_E, ViewGrp, "layouteditmode");
#else
    actionViewLayoutEditMode = new QAction("", QPixmap(viewlayouteditmode_xpm),
            tr("&Layout edit mode"), Qt::CTRL + Qt::Key_E, ViewGrp, "layouteditmode");
#endif
    actionViewLayoutEditMode->setToggleAction(true);
    
#if QT_VERSION >= 0x030200
    actionViewRouteEditMode = new QAction(QPixmap(viewrouteeditmode_xpm),
            tr("&Route edit mode"), Qt::CTRL + Qt::Key_B, ViewGrp, "routeeditmode");
#else
    actionViewRouteEditMode = new QAction("", QPixmap(viewrouteeditmode_xpm),
            tr("&Route edit mode"), Qt::CTRL + Qt::Key_B, ViewGrp, "routeeditmode");
#endif
    actionViewRouteEditMode->setToggleAction(true);
    
    ViewGrp->addTo(viewmenu);
    ViewGrp->addTo(viewtb);

    viewmenu->insertSeparator();

#if QT_VERSION >= 0x030200
    actionViewToggleHistory = new QAction(NULL,
            tr("Toggle &history line"), Qt::CTRL + Qt::Key_D, // Ctrl H/T
            this, "viewToggleHistory");
    actionViewToggleHistory->setToolTip(tr("Show basic keyboard"));
#else
    actionViewToggleHistory = new QAction(tr("Show basic keyboard"),
            tr("Toggle &history line"), Qt::CTRL + Qt::Key_D, // Ctrl H/T
            this, "viewToggleHistory");
#endif
    connect(actionViewToggleHistory, SIGNAL(activated()), this,
            SLOT(slotViewDebug()));
    actionViewToggleHistory->addTo(viewmenu);
    //actionViewToggleHistory->addTo(viewtb);


    /*daemon toolbar*/
    QToolBar* daemontb = new QToolBar(this, "daemontb");
    Q_CHECK_PTR(daemontb);
    daemontb->setLabel(tr("Daemon operations"));

    QPopupMenu* daemonmenu = new QPopupMenu(this);
    //daemonmenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&Daemon"), daemonmenu);

#if QT_VERSION >= 0x030200
    actionDaemonConnect = new QAction(QPixmap(daemonconnect_xpm),
            tr("&Connect"), 0, this, "daemonConnect"); // Ctrl D
    actionDaemonConnect->setToolTip(tr("Connect to SRCP daemon"));
#else
    actionDaemonConnect = new QAction(tr("Connect to SRCP daemon"),
            QPixmap(daemonconnect_xpm),
            tr("&Connect"), 0, this, "daemonConnect"); // Ctrl D
#endif
    connect(actionDaemonConnect, SIGNAL(activated()), this,
            SLOT(ConnectToSRCPServer()));
    actionDaemonConnect->addTo(daemonmenu);
    actionDaemonConnect->addTo(daemontb);

#if QT_VERSION >= 0x030200
    actionDaemonDisconnect = new QAction(QPixmap(daemondisconnect_xpm),
            tr("&Disconnect"), 0, this, "daemonDisconnect");
    actionDaemonDisconnect->setToolTip(tr("Disconnect from SRCP daemon"));
#else
    actionDaemonDisconnect = new QAction(tr("Disconnect from SRCP daemon"),
            QPixmap(daemondisconnect_xpm),
            tr("&Disconnect"), 0, this, "daemonDisconnect");
#endif
    connect(actionDaemonDisconnect, SIGNAL(activated()), this,
            SLOT(CloseSRCPServerConnection()));
    actionDaemonDisconnect->addTo(daemonmenu);
    actionDaemonDisconnect->addTo(daemontb);

    daemonmenu->insertSeparator();
    
#if QT_VERSION >= 0x030200
    actionDaemonReset = new QAction(QPixmap(daemonreset_xpm),
            tr("&Reset"), 0, this, "daemonReset");
    actionDaemonReset->setToolTip(tr("Reset SRCP daemon"));
#else
    actionDaemonReset = new QAction(tr("Reset SRCP daemon"),
            QPixmap(daemonreset_xpm),
            tr("&Reset"), 0, this, "daemonReset");
#endif
    connect(actionDaemonReset, SIGNAL(activated()), this,
            SLOT(slotDaemonReset()));
    actionDaemonReset->addTo(daemonmenu);
    //actionDaemonReset->addTo(daemontb);

#if QT_VERSION >= 0x030200
    actionDaemonKill = new QAction(QPixmap(daemonkill_xpm),
            tr("&Kill"), 0, this, "daemonKill");
    actionDaemonKill->setToolTip(tr("Kill SRCP daemon"));
#else
    actionDaemonKill = new QAction(tr("Kill SRCP daemon"),
            QPixmap(daemonkill_xpm),
            tr("&Kill"), 0, this, "daemonKill");
#endif
    connect(actionDaemonKill, SIGNAL(activated()), this,
            SLOT(slotDaemonKill()));
    actionDaemonKill->addTo(daemonmenu);
    //actionDaemonKill->addTo(daemontb);

#if QT_VERSION >= 0x030200
    actionDaemonInfo = new QAction(QPixmap(daemoninfo_xpm),
            tr("&Info..."), 0, this, "daemonInfo");
    actionDaemonInfo->setToolTip(tr("Info about SRCP daemon"));
#else
    actionDaemonInfo = new QAction(tr("Info about SRCP daemon"),
            QPixmap(daemoninfo_xpm),
            tr("&Info..."), 0, this, "daemonInfo");
#endif
    connect(actionDaemonInfo, SIGNAL(activated()), this,
            SLOT(slotDaemonInfo()));
    actionDaemonInfo->addTo(daemonmenu);
    actionDaemonInfo->addTo(daemontb);


    /*layout toolbar*/
    QToolBar* layouttb = new QToolBar(this, "layouttb");
    Q_CHECK_PTR(layouttb);
    layouttb->setLabel(tr("Layout operations"));

    QPopupMenu* layoutmenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&Layout"), layoutmenu);

#if QT_VERSION >= 0x030200
    actionLayoutPower = new QAction(QPixmap(layoutstart_xpm),
            tr("Start &power"), Qt::Key_F4, this, "layoutPower");
    actionLayoutPower->setToolTip(tr("Switch layout power on"));
#else
    actionLayoutPower = new QAction(tr("Switch layout power on"),
            QPixmap(layoutstart_xpm),
            tr("Start &power"), Qt::Key_F4, this, "layoutPower");
#endif
    connect(actionLayoutPower, SIGNAL(activated()), this,
            SLOT(slotToggleLayoutPower()));
    actionLayoutPower->addTo(layoutmenu);
    actionLayoutPower->addTo(layouttb);

#if QT_VERSION >= 0x030200
    actionLayoutFht = new QAction(NULL,
            tr("Use &FHT"), Qt::Key_F5, this, "layoutFht");
    actionLayoutFht->setToolTip(tr("Use route help button"));
#else
    actionLayoutFht = new QAction(tr("Use route help button"),
            tr("Use &FHT"), Qt::Key_F5, this, "layoutFht");
#endif
    connect(actionLayoutFht, SIGNAL(activated()), gbs,
            SLOT(slotFHTclicked()));
    actionLayoutFht->addTo(layoutmenu);
    //actionLayoutFht->addTo(layouttb);

#if QT_VERSION >= 0x030200
    actionLayoutUfgt = new QAction(NULL,
            tr("Use &UfGT"), Qt::Key_F6, this, "layoutUfgt");
    actionLayoutUfgt->setToolTip(tr("Use detour group button"));
#else
    actionLayoutUfgt = new QAction(tr("Use detour group button"),
            tr("Use &UfGT"), Qt::Key_F6, this, "layoutUfgt");
#endif
    connect(actionLayoutUfgt, SIGNAL(activated()), gbs,
            SLOT(slotUfGTclicked()));
    actionLayoutUfgt->addTo(layoutmenu);
    //actionLayoutUfgt->addTo(layouttb);

#if QT_VERSION >= 0x030200
    actionLayoutWgt = new QAction(NULL,
            tr("Use &WGT"), Qt::Key_F7, this, "layoutWgt");
    actionLayoutWgt->setToolTip(tr("Use turnout group button"));
#else
    actionLayoutWgt = new QAction(tr("Use turnout group button"),
            tr("Use &WGT"), Qt::Key_F7, this, "layoutWgt");
#endif
    connect(actionLayoutWgt, SIGNAL(activated()), gbs,
            SLOT(slotWGTclicked()));
    actionLayoutWgt->addTo(layoutmenu);
    //actionLayoutWgt->addTo(layouttb);

#if QT_VERSION >= 0x030200
    actionLayoutSgt = new QAction(NULL,
            tr("Use &SGT"), Qt::Key_F8, this, "layoutSgt");
    actionLayoutSgt->setToolTip(tr("Use signal group button"));
#else
    actionLayoutSgt = new QAction(tr("Use signal group button"),
            tr("Use &SGT"), Qt::Key_F8, this, "layoutSgt");
#endif
    connect(actionLayoutSgt, SIGNAL(activated()), gbs,
            SLOT(slotSGTclicked()));
    actionLayoutSgt->addTo(layoutmenu);
    //actionLayoutSgt->addTo(layouttb);

#if QT_VERSION >= 0x030200
    actionLayoutHagt = new QAction(NULL,
            tr("Use H&aGT"), Qt::Key_F9, this, "layoutHagt");
    actionLayoutHagt->setToolTip(tr("Use signal halt group button"));
#else
    actionLayoutHagt = new QAction(tr("Use signal halt group button"),
            tr("Use H&aGT"), Qt::Key_F9, this, "layoutHagt");
#endif
    connect(actionLayoutHagt, SIGNAL(activated()), gbs,
            SLOT(slotHaGTclicked()));
    actionLayoutHagt->addTo(layoutmenu);
    //actionLayoutHagt->addTo(layouttb);

    layoutmenu->insertSeparator();

#if QT_VERSION >= 0x030200
    actionLayoutNotRot = new QAction(QPixmap(layoutnotrot_xpm),
            tr("&Halt signals"), Qt::Key_F12, this, "layoutNotRot");
    actionLayoutNotRot->setToolTip(tr("Switch all signals to halt"));
#else
    actionLayoutNotRot = new QAction(tr("Switch all signals to halt"),
            QPixmap(layoutnotrot_xpm),
            tr("&Halt signals"), Qt::Key_F12, this, "layoutNotRot");
#endif
    connect(actionLayoutNotRot, SIGNAL(activated()), gbs,
            SLOT(slotNotrot()));
    actionLayoutNotRot->addTo(layoutmenu);
    actionLayoutNotRot->addTo(layouttb);

#if QT_VERSION >= 0x030200
    actionLayoutToggleAll = new QAction(NULL,
            tr("&Toggle all"), Qt::Key_F10, this, "layoutToggleAll");
    actionLayoutToggleAll->setToolTip(tr("Toggle all switchable elements"));
#else
    actionLayoutToggleAll = new QAction(tr("Toggle all switchable elements"),
            tr("&Toggle all"), Qt::Key_F10, this, "layoutToggleAll");
#endif
    connect(actionLayoutToggleAll, SIGNAL(activated()), gbs,
            SLOT(slotToggleAll()));
    actionLayoutToggleAll->addTo(layoutmenu);
    //actionLayoutToggleAll->addTo(layouttb);

#if QT_VERSION >= 0x030200
    actionLayoutSendAll = new QAction(NULL,
            tr("Send &all"), Qt::Key_F11, this, "layoutSendAll");
    actionLayoutSendAll->setToolTip(tr("Send current states of all "
                "switchable elements to SRCP server"));
#else
    actionLayoutSendAll = new QAction(tr("Send current states of all "
                                "switchable elements to SRCP server"), 
            tr("Send &all"), Qt::Key_F11, this, "layoutSendAll");
#endif
    connect(actionLayoutSendAll, SIGNAL(activated()), this,
            SLOT(layoutSendAll()));
    actionLayoutSendAll->addTo(layoutmenu);
    //actionLayoutSendAll->addTo(layouttb);

#if QT_VERSION >= 0x030200
    actionLayoutUpdateFB = new QAction(NULL,
            tr("Up&date feedback states"), 0, this, "layoutUpdateFB");
    actionLayoutUpdateFB->setToolTip(tr("Get all current feedback "
			    "states from SRCP server"));
#else
    actionLayoutUpdateFB = new QAction(tr("Get all current feedback "
                                            "states from SRCP server"),
            tr("Up&date feedback states"), 0, this, "layoutUpdateFB");
#endif
    connect(actionLayoutUpdateFB, SIGNAL(activated()), this,
            SLOT(layoutUpdateFB()));
    actionLayoutUpdateFB->addTo(layoutmenu);
    //actionLayoutUpdateFB->addTo(layouttb);

    layoutmenu->insertSeparator();

#if QT_VERSION >= 0x030200
    actionLayoutChangeSize = new QAction(NULL,
            tr("S&ettings..."), 0, this, "layoutChangeSettings");
    actionLayoutChangeSize->setToolTip(tr("Change layout settings"));
#else
    actionLayoutChangeSize = new QAction(tr("Change layout settings"),
            tr("S&ettings..."), 0, this, "layoutChangeSettings");
#endif
    connect(actionLayoutChangeSize, SIGNAL(activated()), this,
            SLOT(layoutChangeSize()));
    actionLayoutChangeSize->addTo(layoutmenu);
    //actionLayoutChangeSize->addTo(layouttb);


    /*route toolbar*/
    QToolBar* routetb = new QToolBar(this, "routetb");
    Q_CHECK_PTR(routetb);
    routetb->setLabel(tr("Route operations"));
    routetb->hide();
    //connect visibility of toolbar to visibility of routingviewer
    connect(rtViewer, SIGNAL(visibilityChanged(bool)),
               routetb, SLOT(setShown(bool)));

    /*Route menu*/
    QPopupMenu* routemenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&Route"), routemenu);

#if QT_VERSION >= 0x030200
    actionRouteStart = new QAction(QPixmap(route_start_xpm),
            tr("&Start"), 0, this, "routestart");
    actionRouteStart->setToolTip(tr("Activate route"));
#else
    actionRouteStart = new QAction(tr("Activate route"),
            QPixmap(route_start_xpm),
            tr("&Start"), 0, this, "routestart");
#endif
    connect(actionRouteStart, SIGNAL(activated()), rtViewer,
            SLOT(slotRouteStart()));
    actionRouteStart->addTo(routemenu);
    actionRouteStart->addTo(routetb);

#if QT_VERSION >= 0x030200
    actionRouteStop = new QAction(QPixmap(route_stop_xpm), tr("Sto&p"),
            0, this, "routestop");
    actionRouteStop->setToolTip(tr("Release route"));
#else
    actionRouteStop = new QAction(tr("Release route"),
            QPixmap(route_stop_xpm), tr("Sto&p"),
            0, this, "routestop");
#endif
    connect(actionRouteStop, SIGNAL(activated()), rtViewer,
            SLOT(slotRouteStop()));
    actionRouteStop->addTo(routemenu);
    actionRouteStop->addTo(routetb);

    routemenu->insertSeparator();
    routetb->addSeparator();

#if QT_VERSION >= 0x030200
    actionRouteAdd = new QAction(QPixmap(route_new_xpm), tr("&Add"),
            0, this, "routeadd");
    actionRouteAdd->setToolTip(tr("Add new route"));
#else
    actionRouteAdd = new QAction(tr("Add new route"),
            QPixmap(route_new_xpm), tr("&Add"),
            0, this, "routeadd");
#endif
    connect(actionRouteAdd, SIGNAL(activated()), this,
            SLOT(slotRouteAdd()));
    actionRouteAdd->addTo(routemenu);
    actionRouteAdd->addTo(routetb);

#if QT_VERSION >= 0x030200
    actionRouteEdit = new QAction(QPixmap(route_edit_xpm),
            tr("&Edit..."), 0, this, "routeedit");
    actionRouteEdit->setToolTip(tr("Edit selected route"));
#else
    actionRouteEdit = new QAction(tr("Edit selected route"),
            QPixmap(route_edit_xpm),
            tr("&Edit..."), 0, this, "routeedit");
#endif
    connect(actionRouteEdit, SIGNAL(activated()), rtViewer,
            SLOT(slotRouteEdit()));
    actionRouteEdit->addTo(routemenu);
    actionRouteEdit->addTo(routetb);

#if QT_VERSION >= 0x030200
    actionRouteCopy = new QAction(QPixmap(route_copy_xpm),
            tr("Dupli&cate"), 0, this, "routecopy");
    actionRouteCopy->setToolTip(tr("Duplicate selected route"));
#else
    actionRouteCopy = new QAction(tr("Duplicate selected route"),
            QPixmap(route_copy_xpm),
            tr("Dupli&cate"), 0, this, "routecopy");
#endif
    connect(actionRouteCopy, SIGNAL(activated()), rtViewer,
            SLOT(slotRouteCopy()));
    actionRouteCopy->addTo(routemenu);
    actionRouteCopy->addTo(routetb);

#if QT_VERSION >= 0x030200
    actionRouteDelete = new QAction(QPixmap(route_clear_xpm),
            tr("&Delete"), 0, this, "routedelete");
    actionRouteDelete->setToolTip(tr("Delete selected route"));
#else
    actionRouteDelete = new QAction(tr("Delete selected route"),
            QPixmap(route_clear_xpm),
            tr("&Delete"), 0, this, "routedelete");
#endif
    connect(actionRouteDelete, SIGNAL(activated()), this,
            SLOT(slotRouteDelete()));
    actionRouteDelete->addTo(routemenu);
    actionRouteDelete->addTo(routetb);

    routemenu->insertSeparator();

#if QT_VERSION >= 0x030200
    actionRouteUnlockAll = new QAction(NULL,
            tr("&Unlock all"), Qt::CTRL + Qt::Key_U, this, "layoutUnlockRoutes");
    actionRouteUnlockAll->setToolTip(tr("Unlock all routes"));
#else
    actionRouteUnlockAll = new QAction(tr("Unlock all routes"),
            tr("&Unlock all"), Qt::CTRL + Qt::Key_U, this, "layoutUnlockRoutes");
#endif
    connect(actionRouteUnlockAll, SIGNAL(activated()), rtController,
            SLOT(unlockAllLockedRoutes()));
    actionRouteUnlockAll->addTo(routemenu);
    //actionRouteUnlockAll->addTo(routetb);


    /*help toolbar*/
    //QToolBar* helptb = new QToolBar(this, "helptb");
    //Q_CHECK_PTR(helptb);
    //helptb->setLabel(tr("Help operations"));

    QPopupMenu* helpmenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&Help"), helpmenu);

    helpmenu->insertItem(tr("&Help"), this, SLOT(slotAboutHelp()), Qt::Key_F1);
    helpmenu->insertItem(tr("&SpDrS60 for Linux on the web"),
            this, SLOT(slotAboutWeb()));
    helpmenu->insertSeparator();
    helpmenu->insertItem(QString(tr("&About")) + " \"" + APP_NAME + "\"",
            this, SLOT(slotAbout()));
    helpmenu->insertItem(tr("About &Qt"), this, SLOT(slotAboutQt()));


    resetMenu(); //may be is obsolete

    // method causes segfault if called too early (?)
    actionViewNormalMode->setOn(true);
}


// resetMenu added by dirk. Before loading a new layout, the settings of
// the old one must be reset.

void MainWindow::resetMenu()
{
    // always false on setup, a layout must be loaded first
    actionFileSave->setEnabled(false);
    actionFileSaveAs->setEnabled(false);
    actionEditFileLayout->setEnabled(false);
    actionEditFind->setEnabled(false);
    actionViewRoutes->setEnabled(false);
    actionViewLayoutEditMode->setEnabled(false);
    actionViewRouteEditMode->setEnabled(false);
    // setup depends on daemonstartup
    updateDaemonMenu();
    // always false on setup, a layout must be loaded first
    actionLayoutFht->setEnabled(false);
    actionLayoutWgt->setEnabled(false);
    actionLayoutUfgt->setEnabled(false);
    actionLayoutSgt->setEnabled(false);
    actionLayoutNotRot->setEnabled(false);
    actionLayoutChangeSize->setEnabled(false);
    actionRouteUnlockAll->setEnabled(false);
}


void MainWindow::slotEditCut()
{
 /*TODO*/
}


void MainWindow::slotEditCopy()
{
 /*TODO*/
}


void MainWindow::slotEditPaste()
{
 /*TODO*/
}


// circular toggle through all three history lines
void MainWindow::slotViewDebug()
{
    CurrentHL += 1;
    if (CurrentHL > HL_FEED)
        CurrentHL = HL_HINT;

    switch (CurrentHL) {
        case (HL_HINT):
            cbStack->raiseWidget(0);
            lblStack->raiseWidget(0);
            break;
        case (HL_CMND):
            cbStack->raiseWidget(1);
            lblStack->raiseWidget(1);
            break;
        case (HL_INFO):
            cbStack->raiseWidget(2);
            lblStack->raiseWidget(2);
            break;
        case (HL_FEED):
            cbStack->raiseWidget(3);
            lblStack->raiseWidget(3);
            break;
    }
}


void MainWindow::readAutoloadFile()
{
    if (!pref.autoload)
        return;

    // if autoload file from config data does
    // not exist ask user to change options

    if (!QFile::exists(pref.autolayout)) {                           
        qApp->beep();
        int choice = QMessageBox::warning(this, tr("Autoloader failed"),
                         tr("The selected autoload file '%1'\n"
                            "does not exist. Please adjust your"
                            " options.").arg(pref.autolayout),
                         tr("&Now"), tr("&Later"), 0, 0, 0);
        if (choice == 0)
            slotEditOptions();
    }
    else
        openFile(pref.autolayout);
}


void MainWindow::closeEvent(QCloseEvent* e)
{
    if (pref.autosave) {
        if (saveFile())
            e->accept();
        else 
            e->ignore();
        return;
    }

    if (!isModified()) {
        e->accept();
        return;
    }

    int choice = querySaveChanges();
    switch (choice) {
        case 0:
            if (saveFile())
                e->accept();
            else 
                e->ignore();
            break;
        case 1:
            e->accept();
            break;
        case 2:
        default:
            e->ignore();
            break;
    }
}


int MainWindow::querySaveChanges()
{
    QString queryStr;
    
    if (fileName.isEmpty())
        queryStr = tr("Unnamed file was changed.\nSave changes?");
    else
        queryStr = tr("File '%1' was changed.\n"
                "Save changes?").arg(fileName);
    
    return QMessageBox::warning(this, tr("Save changes"),
            queryStr, tr("&Yes"), tr("&No"), tr("Cancel"));
}


void MainWindow::slotFileNew()
{
    if (isModified()) {
        int choice = querySaveChanges();
        switch (choice) {
            case 0:
                if (saveFile())
                    newFile();
                break;
            case 1:
                newFile();
                break;
            case 2:
            default:
                break;
        }
    }
    else {
        newFile();
    }
}


void MainWindow::newFile()
{
    // show a dialog where the user can put in layout dimensions
    newLayoutDialog* nlDlg = new newLayoutDialog(this);
    if (nlDlg == NULL)
        return;

    nlDlg->setColumns(pref.layoutcols);
    nlDlg->setRows(pref.layoutrows);
    nlDlg->setHost(cmdHost);
    nlDlg->setPort(cmdPort);
    nlDlg->setAutoLogin(cmdAutoLogin);
    nlDlg->setAutoPower(cmdAutoPower);
    nlDlg->setAutoSendAll(cmdAutoSendAll);
    
    if (nlDlg->exec() != QDialog::Accepted) {
        delete nlDlg;
        return;
    }
    int iNewCols = nlDlg->getColumns();
    int iNewRows = nlDlg->getRows();
    cmdHost = nlDlg->getHost();
    cmdPort = nlDlg->getPort();
    cmdAutoLogin = nlDlg->getAutoLogin();
    cmdAutoPower = nlDlg->getAutoPower();
    cmdAutoSendAll = nlDlg->getAutoSendAll();
    delete nlDlg;
    
    CloseSRCPServerConnection();
    fileName = "";
    gbs->newFile(iNewCols, iNewRows);

    updateCaption();
    updateFileMenuItems();
    cmdToDebug(tr("New layout file created"), MT_INFO, HL_HINT);
}


void MainWindow::updateFileMenuItems()
{
    actionFileSave->setEnabled(true);
    actionFileSaveAs->setEnabled(true);

    if (fileName.isEmpty()){
        actionEditFileLayout->setMenuText(tr("Layout file not saved yet"));
        actionEditFileLayout->setEnabled(false);
    }
    else {
        actionEditFileLayout->setMenuText(fileName);
        actionEditFileLayout->setEnabled(true);
    }
    actionEditFind->setEnabled(true);
    
    actionLayoutFht->setEnabled(true);
    actionLayoutWgt->setEnabled(true);
    actionLayoutUfgt->setEnabled(true);
    actionLayoutSgt->setEnabled(true);
    actionLayoutNotRot->setEnabled(true);
    actionLayoutChangeSize->setEnabled(true);

    //if (CommandPortIsConnected) {
        actionLayoutToggleAll->setEnabled(LayoutPowerIsOn);
        actionLayoutSendAll->setEnabled(LayoutPowerIsOn);
	actionLayoutUpdateFB->setEnabled(LayoutPowerIsOn &&
                srcpVersion == 7);
    //}

    //viewmenu->setItemEnabled(VIEW_ID_ROUTES, !fileName.isEmpty());
    actionViewRoutes->setEnabled(true);
    actionViewLayoutEditMode->setEnabled(true);
    actionViewRouteEditMode->setEnabled(true);
    actionRouteUnlockAll->setEnabled(true);
}


bool MainWindow::saveFile()
{
    if (fileName.isEmpty()){
        slotFileSaveAs();
        return true;
    }

    QFile f(fileName);
    if (!f.open(IO_WriteOnly)) {
        cmdToDebug(tr("Could not write to file '%1'").arg(fileName),
                MT_INFO, HL_HINT);
        return false;
    }

    QTextStream ts(&f);
    ts.setEncoding(QTextStream::UnicodeUTF8);
    
    QDateTime dt = QDateTime::currentDateTime();
    
    // write the header
    ts << "# spdrs60 data file" << endl
       << "# version=" << VERSION << endl
       << "# last modified=" << dt.toString(Qt::ISODate) << endl
       << GF_FORMATVERSION << DS << GF_FV << endl
       << GF_CMDHOST << DS << cmdHost << DS << cmdPort <<
                        DS << cmdAutoLogin << DS << cmdAutoPower <<
                        DS << cmdAutoSendAll << endl
       << GF_FBHOST << DS << fbHost << DS << fbPort << DS << fbLogin <<
       endl;

    /*TODO: srcpCom->writeFileTextToStream(ts);*/
    gbs->writeFileTextToStream(ts);
    rtController->writeFileTextToStream(ts);

    f.close();

    updateCaption();
    updateFileMenuItems();

    cmdToDebug(tr("Layout file '%1' saved").arg(fileName), MT_INFO, HL_HINT);
    return true;
}


void MainWindow::slotFileSave()
{
    this->saveFile();
}


void MainWindow::slotFileSaveAs()
{
    QString fn = QFileDialog::getSaveFileName(lastDir,
            QString(tr("Layouts")) + " (*" + GF_GBSEXT + ")", this);

    if (!fn.isEmpty()) {
        /*check for file extension*/
        if (!fn.endsWith(GF_GBSEXT))
            fn.append(GF_GBSEXT);

        lastDir = fn.left(fn.findRev('/'));

        /*check for existend file*/
        if (QFile::exists(fn)){
            int choice = QMessageBox::warning(this, tr("Warning"),
                    tr("File '%1' exists!\n"
                        "Do you want to overwrite it?").arg(fn),
                    tr("&Yes"), tr("&No"), 0, 0, 1);
            if (choice == 1)
                return;
        }

        fileName = fn;
        saveFile();
    }
    else
        cmdToDebug(tr("Saving aborted"), MT_INFO, HL_HINT);
}


void MainWindow::slotFileOpen()
{
    if (isModified()) {
        int choice = querySaveChanges();
        switch (choice) {
            case 0:
                if (saveFile())
                    chooseFile();
                break;
            case 1:
                chooseFile();
                break;
            case 2:
            default:
                break;
        }
    }
    else {
        chooseFile();
    }
}


void MainWindow::chooseFile()
{
    QStringList fnl = QFileDialog::getOpenFileNames(
        QString(tr("Layouts")) + " (*" + GF_GBSEXT + ")", lastDir, this);

    if (fnl.isEmpty())
        return;
    else {
        openFile(fnl.first());
        fnl.pop_front();

        QStringList::Iterator it = fnl.begin();
        while (it != fnl.end()) {
                openFileWindow(*it);
                ++it;
            }
    }
}


void MainWindow::openFile(const QString& fn)
{
    /*remember last directory we used*/
    lastDir = fn.left(fn.findRev('/'));

    QFile f(fn);

    if (!f.open(IO_ReadOnly)){
        cmdToDebug(tr("Could not read file '%1'").arg(fn), MT_INFO, HL_HINT);
        return;
    }
    CloseSRCPServerConnection();

    // give application some time to handle socket closing and toolbar
    // painting
    qApp->processEvents();

    fileName = fn;
    QTextStream ts(&f);
    ts.setEncoding(QTextStream::UnicodeUTF8);
    
    QString s, key, value;
    int fversion = 0;
    while (!ts.eof()) {
        s = ts.readLine();
        
        /* ignore comment lines */
        if (!s.startsWith("#")) {
            key = s.section(DS, 0, 0);
            value = s.section(DS, 1, 1).stripWhiteSpace();
            /* key/value pairs are read sequence independent */
            if (key.compare(GF_CMDHOST) == 0){
                cmdHost = value;
                value = s.section(DS, 2, 2).stripWhiteSpace();
                cmdPort = value.toInt();
                value = s.section(DS, 3, 3).stripWhiteSpace();
                cmdAutoLogin = value.toInt() == 1;
                value = s.section(DS, 4, 4).stripWhiteSpace();
                cmdAutoPower = value.toInt() == 1;
                value = s.section(DS, 5, 5).stripWhiteSpace();
                cmdAutoSendAll = value.toInt() == 1;
            }
            else if (key.compare(GF_FBHOST) == 0){
                fbHost = value;
                value = s.section(DS, 2, 2).stripWhiteSpace();
                fbPort = value.toInt();
                value = s.section(DS, 3, 3).stripWhiteSpace();
                fbLogin = value.toInt() == 1;
            }
            else if (key.compare(GF_FORMATVERSION) == 0){
                fversion = value.toInt();
            }
            else if (s.startsWith("%% layout"))
                break;
        }
    }

    /*TODO: srcpCom->readFileTextFromStream(ts);*/
    gbs->readFileTextFromStream(ts);
    qApp->processEvents();
    rtController->readFileTextFromStream(ts);

    f.close();
    
    cmdToDebug(tr("Layout file '%1' opened").arg(fn), MT_INFO, HL_HINT);
    updateCaption();
    updateFileMenuItems();

    if (cmdAutoLogin) {
        ConnectToSRCPServer();
        qApp->processEvents();

        // update feedback states
        layoutUpdateFB();
    }
}


void MainWindow::updateCaption()
{
    if (fileName.isEmpty())
        setCaption(QString(APP_NAME) + " - [" + tr("noname") + "]");
    else
        setCaption(QString(APP_NAME) + " - [" + fileName + "]");
}


/*
 * New event driven networking code starts here: (guido)
 * This should be a separate class like "SRCPCommunicator" or
 * "srcpSessionController"
 */
void MainWindow::initAllSockets()
{
    /*1. */
    CommandSocket = new QSocket(this, "CommandSocket");
    connect(CommandSocket, SIGNAL(hostFound()),
            SLOT(CommandSocketHostFound()));
    connect(CommandSocket, SIGNAL(connected()),
            SLOT(CommandSocketConnected()));
    connect(CommandSocket, SIGNAL(readyRead()),
            SLOT(CommandSocketReadyRead()));
    connect(CommandSocket, SIGNAL(connectionClosed()),
            SLOT(CommandSocketConnectionClosedByServer()));
    connect(CommandSocket, SIGNAL(error(int)),
            SLOT(CommandSocketError(int)));
    /*2. */
    FeedbackSocket = new QSocket(this, "FeedbackSocket");
    connect(FeedbackSocket, SIGNAL(connected()),
            SLOT(FeedbackSocketConnected()));
    connect(FeedbackSocket, SIGNAL(readyRead()),
            SLOT(FeedbackSocketReadyRead()));
    connect(FeedbackSocket, SIGNAL(connectionClosed()),
            SLOT(FeedbackSocketConnectionClosedByServer()));
    connect(FeedbackSocket, SIGNAL(error(int)),
            SLOT(FeedbackSocketError(int)));
    /*3. */
    InfoSocket = new QSocket(this, "InfoSocket");
    connect(InfoSocket, SIGNAL(connected()), SLOT(InfoSocketConnected()));
    connect(InfoSocket, SIGNAL(readyRead()), SLOT(InfoSocketReadyRead()));
    connect(InfoSocket, SIGNAL(connectionClosed()),
            SLOT(InfoSocketConnectionClosedByServer()));
    connect(InfoSocket, SIGNAL(error(int)), SLOT(InfoSocketError(int)));
}


void MainWindow::CommandSocketHostFound()
{
    cmdToDebug(tr("Command socket: Host '%1' found.").arg(cmdHost), MT_INFO,
            HL_HINT);
}


void MainWindow::CommandSocketReadyRead()
{
    QString sSRCPVer, ServerInfo;

    while (CommandSocket->canReadLine()) {
        ServerInfo = CommandSocket->readLine();

        if (pref.converttime) {
            QString msgstr = ConvertMessageTime(ServerInfo);
            cmdToDebug(msgstr, MT_INFO, HL_CMND);
        }
        else
            cmdToDebug(ServerInfo, MT_INFO, HL_CMND);
        
        if (SRCPCommandState == srcpLogin) {
            sWelcome = ServerInfo;
            sSRCPVer = sWelcome.mid(sWelcome.find("SRCP ", 0, 0) + 5, 5);

            /*SRCP 0.7.x */
            if (isValidSRCP07Version(sSRCPVer)) {
                srcpVersion = 7;
                cmdToDebug(tr("SRCP: %1 ===> PASS").arg(sSRCPVer), MT_INFO,
                           HL_HINT);
                /* first is OK, next two readonly ports follow */
                ConnectFeedbackPort();
                ConnectInfoPort();

                /*
                 * ask server about power state, may be an other client
                 * allready switched power on
                 */
                SRCPCommandState = srcp07GetPower;
                SendCommandToSRCPServer("GET POWER");
            }
            /*SRCP 0.8.x */
            else if (isValidSRCP08Version(sSRCPVer)) {
                srcpVersion = 8;
                cmdToDebug(tr("SRCP: %1 ===> PASS").arg(sSRCPVer), MT_INFO,
                           HL_HINT);

                SRCPCommandState = srcp08SetConnectionModeCommand;
                SendCommandToSRCPServer("SET CONNECTIONMODE SRCP COMMAND");

                /* command channel is OK, now also establish info channel */
                ConnectInfoPort();
            }
            /*SRCP ?.?.? */
            else {
                cmdToDebug(tr("SRCP: %1 ===> FAILED; "
                            "spdrs60 requires SRCP >= 0.7.0 and < 0.9.0")
                           .arg(sSRCPVer), MT_INFO, HL_HINT);
                /* close connection */
                CommandSocket->close();
                CommandSocketConnectionClosed();
                SRCPCommandState = srcpUndefined;
            }
        }
        /*end srcp login*/

        /*
         * respond to SRCP 0.7 messages 
         */

        else if (SRCPCommandState == srcp07GetPower) {
            /* INFO POWER ON */
            if (ServerInfo.contains("POWER ON")) {
                LayoutPowerIsOn = true;
                updateLayoutPowerAction();
                if (cmdAutoSendAll)
                    layoutSendAll();
            }
            else {
                if (cmdAutoPower) {
                    LayoutPowerIsOn = !cmdAutoPower;
                    slotToggleLayoutPower();
                }
            }
            SRCPCommandState = srcpConnected;
            updateDaemonMenu();
            //return;
        }
        
        else if (SRCPCommandState == srcpConnected) {
            /*close command port if string with zero length is send*/
            if (ServerInfo.length() == 0) {
                if (CommandSocket->isOpen()) {
                    CommandSocket->close();
                    CommandSocketConnectionClosed();
                }
            }
        }
        
        else if (SRCPCommandState == srcp07GetFBStates) {
            if (ServerInfo.startsWith("INFO FB")) {
                
                /*
                 * 1) all feedback ports
                 * INFO FB <module_type> * <all states>
                 *   0   1      2        3       4
                 */
                if ("*" == ServerInfo.section(" ", 3, 3)) {
                    QString allstates = ServerInfo.section(" ", 4, 4);

                    unsigned int limit = allstates.length();
                    if (limit > MAX_FB)
                        limit = MAX_FB;

                    unsigned int fbbus, fbcontact, fbstate;

                    for (unsigned int port = 0; port < limit; port++) {
                        fbstate = allstates[port].digitValue();
                        fbcontact = port % 496 + 1;
                        fbbus = port / 496 + 1;

                        // update module window and gbs
                        emit sendFBChangeModule(fbbus, fbcontact, fbstate);
                        emit sendFBChangeLayout(fbbus, fbcontact, fbstate == 1);
                    }
                }

                /*
                 * 2) a single feedback port
                 * INFO FB <module_type> <portnr> <state>
                 *   0   1      2           3        4
                 */
                else {
                    unsigned int fbport, fbbus, fbcontact, fbstate;

                    fbport = ServerInfo.section(" ", 3, 3).toUInt();
                    fbstate = ServerInfo.section(" ", 4, 4).toUInt();
                    fbcontact = (fbport - 1) % 496 + 1;
                    fbbus = (fbport - 1) / 496 + 1;

                    // update module window and gbs
                    emit sendFBChangeModule(fbbus, fbcontact, fbstate);
                    emit sendFBChangeLayout(fbbus, fbcontact, fbstate == 1);
                }
            }

            SRCPCommandState = srcpConnected;
	}
        
        /*
         * respond to SRCP 0.8 messages 
         */

        else if (SRCPCommandState == srcp08SetConnectionModeCommand) {
            /* 202 OK CONNECTION MODE */
            if (ServerInfo.contains("202 OK")) {
                SRCPCommandState = srcp08GoCommandMode;
                SendCommandToSRCPServer("GO");
            }
            else {
                SRCPCommandState = srcp08ServerError;
                cmdToDebug("Server communication error!", MT_INFO, HL_HINT);
            }
        }

        else if (SRCPCommandState == srcp08TermServer) {
            if (ServerInfo.contains("200 OK")) {
                SRCPCommandState = srcpUndefined;
                CloseSRCPServerConnection();
            }
        }

        else if (SRCPCommandState == srcp08GoCommandMode) {
            if (ServerInfo.contains("OK GO")) {
                srcpCommandSessionID = ServerInfo.section(" ", 4, 4).toInt();
                updateDaemonMenu();
                
                /*
		 * now the server is ready for basic commands
                 *
                 * the complete server initialization will go through
                 * this sequence:
                 *   1) initialize all GAs
                 *   2) init all FB busses
                 *   3) power on all busses if automode set
                 */

                SRCPCommandState = srcp08InitGADevices;
                /*when GA init is done, go to FB bus init */
                if (!gbs->runSRCP08GAInitSequence()){
                    SRCPCommandState = srcp08InitFBBusses;
                    if (!gbs->sendSRCP08BusMessage(
                                SrcpMessage::msgPowerInit)){
                        if (cmdAutoPower){
                            SRCPCommandState = srcp08GetBusPower;
                            if (!gbs->sendSRCP08BusMessage(
                                        SrcpMessage::msgPowerGet)){
                                LayoutPowerIsOn = true;
                                updateLayoutPowerAction();
                                SRCPCommandState = srcpConnected;
                                if (cmdAutoSendAll)
                                    layoutSendAll();
                            }
                        }
                        else
                            SRCPCommandState = srcpConnected;
                    }
                }
            }
            else
                SRCPCommandState = srcp08ServerError;
        }

        else if (SRCPCommandState == srcp08InitGADevices) {
	    /*
	     * start FB bus init sequence
	     * when GA init is done, go to FB bus init
	    */
            if (!gbs->runSRCP08GAInitSequence()){
                SRCPCommandState = srcp08InitFBBusses;
                if (!gbs->sendSRCP08BusMessage(SrcpMessage::msgPowerInit)){
		    if (cmdAutoPower){
                        SRCPCommandState = srcp08GetBusPower;
			if (!gbs->sendSRCP08BusMessage(
                                    SrcpMessage::msgPowerGet)){
			    LayoutPowerIsOn = true;
                            updateLayoutPowerAction();
                            SRCPCommandState = srcpConnected;
                            if (cmdAutoSendAll)
                                layoutSendAll();
			}
		    }
		    else
                        SRCPCommandState = srcpConnected;
		}
	    }
        }

        else if (SRCPCommandState == srcp08InitFBBusses) {
	    /*
	     * walk through FB bus list step by step
	     * keep SRCPCommandState while initialization is not finished
	     */
             if (!gbs->sendSRCP08BusMessage(SrcpMessage::msgPowerInit))
		if (cmdAutoPower){
                    SRCPCommandState = srcp08GetBusPower;
		    if (!gbs->sendSRCP08BusMessage(
                                SrcpMessage::msgPowerGet)){
			LayoutPowerIsOn = true;
                        updateLayoutPowerAction();
                        SRCPCommandState = srcpConnected;
                        if (cmdAutoSendAll)
                            layoutSendAll();
		    }
		}
		else
                    SRCPCommandState = srcpConnected;
        }

        else if (SRCPCommandState == srcp08SetBusPower) {
	    /*
	     * walk through bus list step by step
	     * keep SRCPCommandState while power switching is not finished
	     */
	    if (!gbs->setSRCP08BusPower(LayoutPowerIsOn)) {
                SRCPCommandState = srcpConnected;
                if (LayoutPowerIsOn && cmdAutoSendAll)
                    layoutSendAll();
            }
        }

        else if (SRCPCommandState == srcp08GetBusPower) {
	    bool PowerSwitched = false;
	    /*
	     * walk through bus list step by step
	     * keep SRCPCommandState while getting bus power states is
             * not finished
	     *
	     * messages arriving here should look like
             *   1101459005.557 100 INFO 2 POWER ON
	     * or
	     *   1101459034.300 100 INFO 2 POWER OFF
	     *        0          1    2  3   4    5    -> QString sections
	     */
            if (ServerInfo.section(" ", 1, 2) == "100 INFO"){
                if (ServerInfo.section(" ", 5, 5) == "OFF"){
                    SendCommandToSRCPServer(QString("SET %1 POWER ON")
                        .arg(ServerInfo.section(" ", 3, 3)));
		    PowerSwitched = true;
		}
		/* echo "200 OK" is only displayed in history line */
            }
	    /* 
	     * if power was switched wait for server response and switch
	     * next bus at next cycle
	     */
	    if (!PowerSwitched)
		if (!gbs->sendSRCP08BusMessage(SrcpMessage::msgPowerGet)){
		    // all busses are switched on, we are ready 
		    LayoutPowerIsOn = true;
                    updateLayoutPowerAction();
                    SRCPCommandState = srcpConnected;
                    if (cmdAutoSendAll)
                        layoutSendAll();
		}
        }

        else {
            /* else: no login but connection close */
            cmdToDebug(tr("Cannot read server welcome message!"),
                    MT_INFO, HL_HINT);
            /*close command port */
            if (ServerInfo.length() == 0) {
                if (CommandSocket->isOpen()) {
                    CommandSocket->close();
                    CommandSocketConnectionClosed();
                    SRCPCommandState = srcpUndefined;
                }
            }
        }
    }
}


void MainWindow::CommandSocketConnected()
{
    cmdToDebug(tr("Command socket connected!"), MT_INFO, HL_HINT);
    CommandPortIsConnected = true;
    updateDaemonMenu();
    // layout area is set modified to make changes of GA directions saveable
    gbs->setModified(true);
}


void MainWindow::CommandSocketConnectionClosedByServer()
{
    if (CommandSocket->isOpen()) {
        CommandSocket->close();
    }
    cmdToDebug(tr("Command socket closed by foreign host!"), MT_INFO, HL_HINT);
    CommandPortIsConnected = false;
    SRCPCommandState = srcpUndefined;
    updateDaemonMenu();
}


void MainWindow::CommandSocketConnectionClosed()
{
    cmdToDebug(tr("Command socket closed!"), MT_INFO, HL_HINT);
    CommandPortIsConnected = false;
    updateDaemonMenu();
}


void MainWindow::CommandSocketError(int e)
{
    QString ErrMessage = GetSocketErrorString(e);
    cmdToDebug(tr("Command socket: Error number %1 occurred (%2)")
               .arg(e).arg(ErrMessage), MT_INFO, HL_HINT);
}


/*
 * SRCP 0.7 port transformations
 *
 *   07port     contact     bus      module       input
 * (1 - 1984)  (1 - 496)  (1 - 4)  (1 - 31/62)  (1 - 16/8)
 * -------------------------------------------------------
 *      1           1        1          1           1
 *      2           2        1          1           2
 *      .           .        .          .           .
 *     16          16        1          1          16
 *     17          17        1          2           1
 *      .           .        .          .           .
 *     32          32        1          2          16
 *     33          33        1          3           1
 *      .           .        .          .           .
 *    496         496        1         31          16
 *    497           1        2          1           1
 *      .           .        .          .           .
 *    512          16        2          1          16
 *    513          17        2          2           1
 *      .           .        .          .           .
 *   1984         496        4         31          16
 * -------------------------------------------------------
 * 
 * calculation (integer based):
 * 
 *   contact = (07port - 1) mod 496 + 1;
 *   bus     = (07port - 1) / 496 + 1;
 *   module  = (contact - 1) / 16 + 1;
 *   input   = (contact - 1) mod 16 + 1;
 */

/**
 * read incomming feedback infos, this procedure is only used in
 *  SRCP 0.7 mode, incomming FB info for SRCP 0.8 see InfoSocketReadyRead()
 */
void MainWindow::FeedbackSocketReadyRead()
{
    QString sInfo = "";
    unsigned int fbcontact, fbbus, fbport, fbstate;

    while (FeedbackSocket->canReadLine()) {
        sInfo = FeedbackSocket->readLine();

        // if we got error code, break loop
        if (sInfo.contains("-", 0)) {
            cmdToDebug(sInfo, MT_INFO, HL_FEED);
            break;
        }

        /*
         * Qstring sections:
         * INFO FB <module_type> <portnr> <state>
         *  0   1       2           3        4
         */

        fbport = sInfo.section(" ", 3, 3).toUInt();
        fbstate = sInfo.section(" ", 4, 4).toUInt();

        fbcontact = (fbport - 1) % 496 + 1;
        fbbus = (fbport - 1) / 496 + 1;

        /* switch on port change messages after initialization */
        if (isFBInitMode) {
            if (fbport == MAX_FB)
                isFBInitMode = false;
        }
        else
            cmdToDebug(sInfo, MT_CMD, HL_FEED);

        /* should'nt we only send modules which are realy connected? */
        // send feedback updates to:
        // 1. module window
        // 2. all elements via gbs
        // 3. all routes
        emit sendFBChangeModule(fbbus, fbcontact, fbstate);
        emit sendFBChangeLayout(fbbus, fbcontact, fbstate == 1);
	emit sendFBChangeRoute(fbbus, fbcontact, fbstate == 1);
    }
}


void MainWindow::FeedbackSocketConnected()
{
    cmdToDebug(tr("Feedback socket connected!"), MT_INFO, HL_HINT);
    FeedbackPortIsConnected = true;
    // flag to avoid history line flooding by startup feedback
    isFBInitMode = true;

    SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgFbInit);
    if (sm == NULL)
        return;

    sm->setFbData(0, (SrcpMessage::Feedback) pref.fbmoduletype, 0);
    sendSrcpMessage(sm);
    delete sm;

    cmdToDebug(tr("Feedback port changes should be avoided during "
             "initialization"), MT_INFO, HL_FEED);
}


void MainWindow::FeedbackSocketConnectionClosedByServer()
{
    if (FeedbackSocket->isOpen()) {
        FeedbackSocket->close();
    }
    cmdToDebug(tr("Feedback socket closed by foreign host!"), MT_INFO, HL_HINT);
    FeedbackPortIsConnected = false;
}


void MainWindow::FeedbackSocketConnectionClosed()
{
    cmdToDebug(tr("Feedback socket closed!"), MT_INFO, HL_HINT);
    FeedbackPortIsConnected = false;
}


void MainWindow::FeedbackSocketError(int e)
{
    QString ErrMessage = GetSocketErrorString(e);
    cmdToDebug(tr("Feedback socket: Error number %1 occurred (%2)")
               .arg(e).arg(ErrMessage), MT_INFO, HL_HINT);
}


QString MainWindow::ConvertMessageTime(const QString& msg)
{
    QString msgstr;
    QString timestr = msg.section(" ", 0, 0);
    if (!timestr.startsWith("0.")) {
        QDateTime srvtime = QDateTime();
        srvtime.setTime_t(timestr.section(".", 0 , 0).toUInt());
        QTime msgtime = srvtime.time();
        msgstr = msgtime.toString("[hh:mm:ss.");

        msgstr.append(timestr.section(".", 1 , 1));
        msgstr.append("] ");
    }
    else
        msgstr = "[--:--:--.---] ";

    msgstr.append(msg.section(" ", 1));
    return msgstr;
}


void MainWindow::InfoSocketReadyRead()
{
    if (gbs == NULL)
        return;

    QString sInfo = "";

    while (InfoSocket->canReadLine()) {
        sInfo = InfoSocket->readLine();

        /* reaktions to SRCP 0.7 commands */
        if (srcpVersion == 7) {
            cmdToDebug(sInfo, MT_CMD, HL_INFO);
            QString device = sInfo.section(" ", 1, 1);
            /*
             * check for incomming GA actions and send them to gbs
             * INFO GA <protocol> <addr> <port> <state>
             *   0   1     2        3      4       5
             */
            if ("GA" == device && sInfo.section(" ", 5, 5).toUInt() == 1) {
                gbs->sendInfoPortMessage(1,
                        sInfo.section(" ", 3, 3).toUInt(),
                        sInfo.section(" ", 4, 4).toUInt(),
                        sInfo.section(" ", 5, 5).toUInt());
            }
            /*
             * check for requested FB states and send them to gbs, module
             * window and route controller
             * INFO FB <module_type> <portnr> <state>
             *   0   1      2            3       4
             */
            else if ("FB" == device) {
                // one state of a single FB port
                unsigned int fbport, fbcontact, fbbus, fbstate;

                fbport = sInfo.section(" ", 3, 3).toUInt();
                fbstate = sInfo.section(" ", 4, 4).toUInt();

                fbcontact = (fbport - 1) % 496 + 1;
                fbbus = (fbport - 1) / 496 + 1;

                // send updates to:
                // 1. module window
                // 2. all elements via gbs
                // 3. all routes if not in init mode
                emit sendFBChangeModule(fbbus, fbcontact, fbstate);
                emit sendFBChangeLayout(fbbus, fbcontact, fbstate == 1);
                if (!isFBInitMode)
                    emit sendFBChangeRoute(fbbus, fbcontact, fbstate == 1);
            }
        }

        /* respond to SRCP 0.8 messages */
        else {
            if (pref.converttime) {
                QString msgstr = ConvertMessageTime(sInfo);
                cmdToDebug(msgstr, MT_CMD, HL_INFO);
            }
            else
                cmdToDebug(sInfo, MT_CMD, HL_INFO);
 
            if (SRCPInfoState == srcpLogin) {
                /*
                 * version verification is neglected here, because this
                 * is allready done in COMMAND mode
                 */
                SRCPInfoState = srcp08SetConnectionModeInfo;
                SendInfoCommandToSRCPServer("SET CONNECTIONMODE SRCP INFO");
            }

            else if (SRCPInfoState == srcp08SetConnectionModeInfo) {
                if (sInfo.contains("202 OK")) {
                    SRCPInfoState = srcp08GoCommandMode;
                    SendInfoCommandToSRCPServer("GO");
                }
                else {
                    SRCPInfoState = srcp08ServerError;
                    cmdToDebug("Server communication error!", MT_INFO,
                            HL_INFO);
                    // TODO: What to do now? Close info socket?
                }
            }

            else if (SRCPInfoState == srcp08GoCommandMode) {
                if (sInfo.contains("OK GO"))
                    srcpInfoSessionID = sInfo.section(" ", 4, 4).toInt();
                SRCPInfoState = srcp08RunInfoMode;
                //TODO: else communication error
            }

            /* respond to incomming info messages */
            else if (SRCPInfoState == srcp08RunInfoMode) {
                QString devGroup = sInfo.section(" ", 4, 4);

                /*
                 * respond to incomming feedback messages
                 *
                 * <time> 100 INFO <bus> FB <addr> <value>
                 *   0     1   2     3   4    5       6 : Qstring sections
                 */
                if (devGroup == "FB") {
                    unsigned int fbbus, fbcontact, fbstate;
                    /*TODO: implement FB for other hardware then s88 */

                    /* which bus is first one when FB type is s88? */
                    fbbus = sInfo.section(" ", 3, 3).toUInt();
                    fbcontact = sInfo.section(" ", 5, 5).toUInt();
                    fbstate  = sInfo.section(" ", 6, 6).toUInt();

                    // send updates to:
                    // 1. module window
                    // 2. all elements via gbs
                    // 3. all routes if not in init mode
                    emit sendFBChangeModule(fbbus, fbcontact, fbstate);
                    emit sendFBChangeLayout(fbbus, fbcontact, fbstate == 1);
                    emit sendFBChangeRoute(fbbus, fbcontact, fbstate == 1);
                }

                /*
                 * respond to incomming generic article messages
                 *
                 * <time> 100 INFO <bus> GA <addr> <port> <value>
                 * <time> 101 INFO <bus> GA <prot> <optionales>
                 *   0     1   2     3   4    5       6     7: Qstring sections
                 */
                else if (devGroup == "GA") {
                    if (sInfo.section(" ", 1, 1).toUInt() == 100 &&
                                    sInfo.section(" ", 0, 0) != "0.0")
                        // (bus, addr, port, value)
                        gbs->sendInfoPortMessage(
                                sInfo.section(" ", 3, 3).toUInt(),
                                sInfo.section(" ", 5, 5).toUInt(),
                                sInfo.section(" ", 6, 6).toUInt(),
                                sInfo.section(" ", 7, 7).toUInt());
                }
                
                /*
                 * respond to incomming power messages
                 *
                 * <time> 100 INFO <bus> POWER <on/off>
                 *   0     1   2     3     4      5     : Qstring sections
                 */
                else if (devGroup == "POWER") {
                    if (sInfo.section(" ", 1, 1).toUInt() == 100) {
                        unsigned int bus = sInfo.section(" ", 3, 3).toUInt();
                        bool poweron = sInfo.section(" ", 5, 5) == "ON";
                        
                        //check if bus is relevant for this layout
                        if (gbs->hasSrcp08GaBus(bus)) 
                            if (poweron != LayoutPowerIsOn) {
                                LayoutPowerIsOn = poweron;
                                updateLayoutPowerAction();
                            }
                    }
                }
                /**
                 * add other device groups here (ECHO, MACRO)
                 * <time> 100 INFO <bus=0> ECHO <echo message>
                 * <time> 100 INFO <bus=0> MACRO <macro message>
                 **/
            }
        }
    }
}


void MainWindow::InfoSocketConnected()
{
    cmdToDebug(tr("Info socket connected!"), MT_INFO, HL_HINT);
    InfoPortIsConnected = true;
}


void MainWindow::InfoSocketConnectionClosedByServer()
{
    if (InfoSocket->isOpen()) {
        InfoSocket->close();
    }
    cmdToDebug(tr("Info socket closed by foreign host!"), MT_INFO, HL_HINT);
    InfoPortIsConnected = false;
}


void MainWindow::InfoSocketConnectionClosed()
{
    cmdToDebug(tr("Info socket closed!"), MT_INFO, HL_HINT);
    InfoPortIsConnected = false;
}


void MainWindow::InfoSocketError(int e)
{
    QString ErrMessage = GetSocketErrorString(e);
    cmdToDebug(tr("Info socket: Error number %1 occurred (%2)")
               .arg(e).arg(ErrMessage), MT_INFO, HL_HINT);
}


QString MainWindow::GetSocketErrorString(int e)
{
    QString ErrMessage = "";

    switch (e) {
        case (QSocket::ErrConnectionRefused):
            ErrMessage = tr("connection refused");
            break;
        case (QSocket::ErrHostNotFound):
            ErrMessage = tr("host not found");
            break;
        case (QSocket::ErrSocketRead):
            ErrMessage = tr("socket read error");
            break;
    }
    return ErrMessage;
}


void MainWindow::ConnectToSRCPServer()
{
    ConnectCommandPort();
}


bool MainWindow::isValidSRCP07Version(const QString& SRCPVerStr)
{
    bool returnvalue = false;

    if ((QString::compare(SRCPVerStr, "0.7.0") >= 0) &&
        QString::compare(SRCPVerStr, "0.8.0") < 0) {
        returnvalue = true;
    }
    return returnvalue;
}


bool MainWindow::isValidSRCP08Version(const QString& SRCPVerStr)
{
    bool returnvalue = false;

    if ((QString::compare(SRCPVerStr, "0.8.0") >= 0) &&
        QString::compare(SRCPVerStr, "0.9.0") < 0) {
        returnvalue = true;
    }
    return returnvalue;
}


void MainWindow::ConnectCommandPort()
{
    cmdToDebug(tr("Command socket: Try to connect host \"%1\" on port \"%2\"")
            .arg(cmdHost).arg(cmdPort), MT_INFO, HL_HINT);
    SRCPCommandState = srcpLogin;
    CommandSocket->connectToHost(cmdHost, cmdPort);   /*e.g.: 4303 */
}


void MainWindow::ConnectFeedbackPort()
{
    cmdToDebug(tr("Feedback socket: Try to connect host \"%1\" on port \"%2\"")
            .arg(cmdHost).arg(cmdPort + 1), MT_INFO, HL_HINT);
    FeedbackSocket->connectToHost(cmdHost, cmdPort + 1);    /*e.g.: 4303 + 1 */
}


void MainWindow::ConnectInfoPort()
{
    /*
     * in SRCP 0.7 mode connection is established to second port
     * e.g.: 4303 + 2
     * in SRCP 0.8 mode connection is established to same port as
     * command channel, but other login type
     */
    if (srcpVersion == 7) {
        cmdToDebug(tr("Info socket: Try to connect host \"%1\" on port \"%2\"")
                .arg(cmdHost).arg(cmdPort + 2), MT_INFO, HL_HINT);
        InfoSocket->connectToHost(cmdHost, cmdPort + 2);
    }
    else {
        cmdToDebug(tr("Info socket: Try to connect host \"%1\" on port \"%2\"")
                .arg(cmdHost).arg(cmdPort), MT_INFO, HL_HINT);
        SRCPInfoState = srcpLogin;
        InfoSocket->connectToHost(cmdHost, cmdPort);
    }
}


void MainWindow::CloseSRCPServerConnection()
{
    /* 1. Command socket */
    if (CommandSocket->isOpen()) {
        
        if (srcpVersion == 7)
            SendCommandToSRCPServer("LOGOUT");
        else if (srcpVersion == 8)
            SendCommandToSRCPServer("TERM 0 SESSION");

        CommandSocket->close();
        if (CommandSocket->state() == QSocket::Closing) {
            // We have a delayed close.
            connect(CommandSocket, SIGNAL(delayedCloseFinished()),
                    SLOT(CommandSocketConnectionClosed()));
        } 
        else
            // The socket is closed.
            CommandSocketConnectionClosed();
    }        

    /* 2. Feedback socket, but only in SRCP 0.7 mode */
    if (srcpVersion == 7 && FeedbackSocket->isOpen()) {

        FeedbackSocket->close();
        if (FeedbackSocket->state() == QSocket::Closing) {
            // We have a delayed close.
            connect(FeedbackSocket, SIGNAL(delayedCloseFinished()),
                    SLOT(FeedbackSocketConnectionClosed()));
        }
        else
            // The socket is closed.
            FeedbackSocketConnectionClosed();
    }

    /* 3. Info socket */
    if (InfoSocket->isOpen()) {
        
        if (srcpVersion == 8)
            SendInfoCommandToSRCPServer("TERM 0 SESSION");
        
        InfoSocket->close();
        if (InfoSocket->state() == QSocket::Closing) {
            // We have a delayed close.
            connect(InfoSocket, SIGNAL(delayedCloseFinished()),
                    SLOT(InfoSocketConnectionClosed()));
        }
        else {
            // The socket is closed.
            InfoSocketConnectionClosed();
        }
    }
}


void MainWindow::SendCommandToSRCPServer(const QString& cmdstr)
{
    if (CommandSocket->isOpen()) {
        QCString cmd = cmdstr.ascii();
        /*temporary solution for command strings with and without '\n' */
        if (!cmdstr.endsWith("\n"))
            cmd.append("\n");
        CommandSocket->writeBlock(cmd, (ulong) cmd.length());
        cmdToDebug(cmdstr, MT_CMD, HL_CMND);
    }
}

/* this is only used in SRCP 0.8 mode */
void MainWindow::SendInfoCommandToSRCPServer(const QString& cmdstr)
{
    if (InfoSocket->isOpen()) {
        QCString cmd = cmdstr.ascii();
        /*temporary solution for command strings with and without '\n' */
        if (!cmdstr.endsWith("\n"))
            cmd.append("\n");
        InfoSocket->writeBlock(cmd, (unsigned long) cmd.length());
        //InfoSocket->flush();
        cmdToDebug(cmdstr, MT_CMD, HL_INFO);
    }
}


void MainWindow::sendSrcpMessage(SrcpMessage* sm)
{
    if (sm == NULL)
        return;
    
    if (!CommandSocket->isOpen())
        return;

    QString cmd = sm->getSrcpMessageStr(srcpVersion);
    cmdToDebug(cmd, MT_CMD, HL_CMND);
    cmd.append("\n");
    CommandSocket->writeBlock(cmd.ascii(), (unsigned long) cmd.length());

    switch(sm->getMessage()) {
        //TODO: set apropriate SRCPCommandStates
        case SrcpMessage::msgFbGet:
            SRCPCommandState = srcp07GetFBStates;
            break;
        default:
            break;
    }
}
/* End of new Networking code */


void MainWindow::slotToggleLayoutPower()
{
    LayoutPowerIsOn = !LayoutPowerIsOn;

    if (srcpVersion == 7) {
        SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgPowerSet);

        if (sm == NULL)
            return;

        sm->setPowerData(0, LayoutPowerIsOn);
        sendSrcpMessage(sm);
        delete sm;

        if (LayoutPowerIsOn && cmdAutoSendAll)
            layoutSendAll();
    }
    else if (srcpVersion == 8) {
        SRCPCommandState = srcp08SetBusPower;

        if (!gbs->setSRCP08BusPower(LayoutPowerIsOn)) {
            SRCPCommandState = srcpConnected;

            if (LayoutPowerIsOn && cmdAutoSendAll)
                layoutSendAll();
        }
    }
    updateLayoutPowerAction();
}


void MainWindow::updateLayoutPowerAction()
{
    if (LayoutPowerIsOn) {
        actionLayoutPower->setMenuText(tr("&Stop power"));
        actionLayoutPower->setToolTip(tr("Switch layout power off"));
        actionLayoutPower->setIconSet(QPixmap(layoutstop_xpm));
    }
    else {
        actionLayoutPower->setMenuText(tr("&Start power"));
        actionLayoutPower->setToolTip(tr("Switch layout power on"));
        actionLayoutPower->setIconSet(QPixmap(layoutstart_xpm));
    }
    actionLayoutToggleAll->setEnabled(LayoutPowerIsOn);
    actionLayoutSendAll->setEnabled(LayoutPowerIsOn);
    actionLayoutUpdateFB->setEnabled(LayoutPowerIsOn &&
                srcpVersion == 7);
    
    //fprintf(stderr, "Power: %d  SrcpV: %d\n", LayoutPowerIsOn, srcpVersion);
}


// reset the daemon
void MainWindow::slotDaemonReset()
{
    SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgServerReset);

    if (sm == NULL)
        return;

    sendSrcpMessage(sm);
    delete sm;
}


/*send shut down SRCP server message*/
void MainWindow::slotDaemonKill()
{
    int choice = QMessageBox::warning(this, tr("Shutdown SRCP server"),
                     tr("You are about to shutdown the SRCP server.\n"
                        "Do you really want to proceed?\n"
                        "(Note: To continue using this program, restart\n"
                        "the server daemon after shutdown has finished)"),
                        tr("&Shutdown"), tr("Cancel"), 0, 1, 1);

    // do not shutdown server -> return
    if (choice == 1)
        return;

    /*TODO:
    SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgServerShutdown);

    if (sm == NULL)
        return;

    sendSrcpMessage(sm);
    delete sm;
    */
    if (srcpVersion == 7) {
        SendCommandToSRCPServer("SHUTDOWN");
        CloseSRCPServerConnection();
    }
    else if (srcpVersion == 8) {
        SRCPCommandState = srcp08TermServer;
        SendCommandToSRCPServer("TERM 0 SERVER");
    }

    cmdToDebug(tr("Server was shutdown. Restart server to "
             "reconnect with \"SpDrS60 for Linux\""),
            MT_INFO, HL_HINT);
}


/*show SRCP server info window*/
void MainWindow::slotDaemonInfo()
{
    int iSep = sWelcome.find(';', 0, 0);
    QString sServer = sWelcome.left(iSep);
    QString sSRCP = sWelcome.right(sWelcome.length() - iSep - 2);
    QString sInfo;
    sInfo.sprintf(tr("Server name and version number:\n%s\n"
                     "\nSRCP version number:\n%s"),
                     sServer.data(), sSRCP.data());
    QMessageBox::information(this, tr("Server information"), sInfo);
}


/*update state of daemon/server menu items*/
void MainWindow::updateDaemonMenu()
{
    if (!CommandPortIsConnected)
        LayoutPowerIsOn = false;
    updateLayoutPowerAction();

    // disable all daemon related menus and toolbuttons if the daemon is
    // not running or has been killed
    actionViewKeyboard->setEnabled(CommandPortIsConnected);

    actionDaemonConnect->setEnabled(!CommandPortIsConnected);
    actionDaemonDisconnect->setEnabled(CommandPortIsConnected);
    actionDaemonReset->setEnabled(CommandPortIsConnected);
    actionDaemonKill->setEnabled(CommandPortIsConnected);
    actionDaemonInfo->setEnabled(CommandPortIsConnected);
    
    actionLayoutPower->setEnabled(CommandPortIsConnected);

    actionLayoutToggleAll->setEnabled(LayoutPowerIsOn);
    actionLayoutSendAll->setEnabled(LayoutPowerIsOn);
    actionLayoutUpdateFB->setEnabled(LayoutPowerIsOn &&
                srcpVersion == 7);
}


/*show spdrs60 copyright message window*/
void MainWindow::slotAbout()
{
    QMessageBox::information(this, QString(tr("About ")) + APP_NAME,
      QString(APP_NAME) + " " + VERSION + "\n" +
      tr("(C) 1999-2003 by Stefan Preis\n"
         "(C) 2004-2007 by Guido Scholz\n"
         "with the gorgeous help of:\n"
	 " Ruediger Seidel\n"
	 " Dirk Armbrust\n"
	 " Björn Schließmann\n"
	 " Dietmar Toelg\n"
	 "For more information please have a look at the\n"
	 "documentation (see Help menu or press F1).\n\n"
	 "Please report ANY bugs, hints and thanks to:\n") +
	 PACKAGE_BUGREPORT);
}


/*show Qt copyright window*/
void MainWindow::slotAboutQt()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}


/*show/hide route list window*/
void MainWindow::slotShowRoutes()
{
    if (rtViewer!= NULL)
        if (rtViewer->isVisible())
            rtViewer->hide();
        else {
            rtViewer->show();
            rtViewer->setActiveWindow();
            rtViewer->raise();
        }
}


/* edit layout file with external editor*/
void MainWindow::slotEditGBSFiles()
{
    QString sCommand = pref.editor;
    sCommand.append(" " + fileName + (" &"));
    system(sCommand.data());
}


/*open config file with external editor*/
void MainWindow::slotEditConfigFile()
{
    // edit program´s config file with editor program
    QString sCommand = pref.editor + " " + QDir::homeDirPath() + "/" +
        SPDRS60_INIT + (" &");
    system(sCommand.data());
}


/*switch edit modes of layout area*/
void MainWindow::slotViewSwitchMode(QAction* ac)
{
    bool rtvIsVisible = rtViewer->isVisible();

    if (ac == actionViewNormalMode) {
            visualMode = kvmNormal;
            updateRouteMenu(rtvIsVisible);
            cmdToDebug(tr("Layout in normal view mode"), MT_INFO, HL_HINT);
    }
    else if (ac == actionViewLayoutEditMode) {
            visualMode = kvmEditLayout;
            // when layout was in edit mode, it is
            // assumed to be modified
            gbs->setModified(true);
            rtViewer->hide();
            updateRouteMenu(false);
            cmdToDebug(tr("Entering layout edit mode"), MT_INFO, HL_HINT);
    }
    else if (ac == actionViewRouteEditMode) {
            visualMode = kvmEditRoute;
            rtViewer->show();
            updateRouteMenu(true);
            cmdToDebug(tr("Entering route edit mode"), MT_INFO, HL_HINT);
        }
    // send new visual mode to router, gbs and route list window
    emit switchedVisualMode(visualMode);

    // change edit related menus
    actionFileNew->setEnabled(visualMode == kvmNormal);
    actionFileOpen->setEnabled(visualMode == kvmNormal);
    actionFileSaveAs->setEnabled(true);
}

/*
 * called when layout viewmode is changed, visibility of route list
 * window changes and activity state of a selected route changes
 */
void MainWindow::updateRouteMenu(bool rtvIsVisible)
{
    if (!rtvIsVisible) {
        actionRouteStart->setEnabled(false);
        actionRouteStop->setEnabled(false);
        actionRouteAdd->setEnabled(false);
        actionRouteEdit->setEnabled(false);
        actionRouteCopy->setEnabled(false);
        actionRouteDelete->setEnabled(false);
    }
    else {
        updateRouteMenuActivateItems();

        if (visualMode == kvmEditRoute) {
            actionRouteAdd->setEnabled(true);

            bool hasitem = rtViewer->hasCurrentItem();
            actionRouteEdit->setEnabled(hasitem);
            actionRouteCopy->setEnabled(hasitem);
            actionRouteDelete->setEnabled(hasitem);
        }
        else {
            actionRouteAdd->setEnabled(false);
            actionRouteEdit->setEnabled(false);
            actionRouteCopy->setEnabled(false);
            actionRouteDelete->setEnabled(false);
        }
    }
}


/*update state of route activate menu items*/
void MainWindow::updateRouteMenuActivateItems()
{
    // disable route switching while element pointers are not valid
    if (rtViewer->isVisible() && visualMode != kvmEditLayout) {

        int state = rtViewer->getCurrentItemState();

        switch (state) {
            case -1:
                actionRouteStart->setEnabled(false);
                actionRouteStop->setEnabled(false);
                break;
            case 0:
                actionRouteStart->setEnabled(true);
                actionRouteStop->setEnabled(false);
                break;
            default:
                actionRouteStart->setEnabled(false);
                actionRouteStop->setEnabled(true);
                break;
        }
    }
    else {
        actionRouteStart->setEnabled(false);
        actionRouteStop->setEnabled(false);
    }
}


/* update menu items if route list gets empty*/
void MainWindow::updateRouteListMenuItems()
{
    updateRouteMenu(rtViewer->isVisible());
}


/* open handbook in external browser*/
void MainWindow::slotAboutHelp()
{
    QString langenv, sURL;

    langenv = getenv("LANG");
    if (langenv.find("de", 0, TRUE) >= 0)
        langenv = "de";
    else
        langenv = "en";

    sURL = QString("%1/%2/index.html")
        .arg(HTML_DOC_DIR)
        .arg(langenv);

    /* if browser is mozilla or firefox first check for running program
     * instance; run with "-remote" option to get a new tab */

    if (pref.browser == "mozilla" || pref.browser == "firefox") {
        if (system(pref.browser + " -remote 'ping()'") == 0)
            system(pref.browser + " -remote 'openURL(" + sURL + ",new-tab)'");
        else
            system(pref.browser + " " + sURL + " &");
    }
    else
        system(pref.browser + " " + sURL + " &");
}


/* open SpDrS60 web resources with external browser */
void MainWindow::slotAboutWeb()
{    
    QString sURL = QString("http://spdrs60.sourceforge.net/");

    if (pref.browser == "mozilla" || pref.browser == "firefox") {
        if (system(pref.browser + " -remote 'ping()'") == 0)
            system(pref.browser + " -remote 'openURL(" + sURL + ",new-tab)'");
        else
            system(pref.browser + " " + sURL + " &");
    }
    else
        system(pref.browser + " " + sURL + " &");
}

// show an original DB clock with minute delay
void MainWindow::slotShowClock()
{
    QString sCommand = "centralclock &";
    system(sCommand.data());    
}                               


// show feedback module window
void MainWindow::slotShowModules()
{
    if (fbViewer != NULL)
        if (fbViewer->isVisible())
            fbViewer->hide();
        else {
            fbViewer->show();
            fbViewer->setActiveWindow();
            fbViewer->raise();
        }
}


/* show a simple keyboard */
void MainWindow::slotViewKeyboard()
{
    if (keybWindow != NULL) {
        if (keybWindow->isVisible())
            keybWindow->hide();
        else {
            keybWindow->show();
            keybWindow->setActiveWindow();
            keybWindow->raise();
        }
    }
    else {
        keybWindow = new keyboard(this, srcpVersion);
        connect(keybWindow, SIGNAL(sendCommand(const QString&)),
                this, SLOT(SendCommandToSRCPServer(const QString&)));

        keybWindow->move(QCursor::pos());
        keybWindow->show();
    }
}

/* display message at bottom of main window */
void MainWindow::cmdToDebug(const QString& hl_message, int m_type,
                            int hl_type)
{
    QTime cmdTime = QTime::currentTime();
    QString t = cmdTime.toString("hh:mm:ss.zzz");

    if (m_type == MT_INFO)
        t.append("> ");         // == an info line
    else
        t.append("# ");         // == a command

    t.append(hl_message);

    // delete oldest entries and add newer ones
    switch (hl_type) {
        case HL_HINT:
            if (HintCB->count() == MAX_HISTORY)
                HintCB->removeItem(0);
            HintCB->insertItem(t);
            HintCB->setCurrentItem(HintCB->count() - 1);
            break;
        case HL_CMND:
            if (CmdCB->count() == MAX_HISTORY)
                CmdCB->removeItem(0);
            CmdCB->insertItem(t);
            CmdCB->setCurrentItem(CmdCB->count() - 1);
            break;
        case HL_INFO:
            if (InfoCB->count() == MAX_HISTORY)
                InfoCB->removeItem(0);
            InfoCB->insertItem(t);
            InfoCB->setCurrentItem(InfoCB->count() - 1);
            break;
        case HL_FEED:
            if (FeedBackCB->count() == MAX_HISTORY)
                FeedBackCB->removeItem(0);
            FeedBackCB->insertItem(t);
            FeedBackCB->setCurrentItem(FeedBackCB->count() - 1);
            break;
    }
}


/* show dialog window with user preferences */
void MainWindow::slotEditOptions()
{
    optionsDialog* optDlg = new optionsDialog(this);
    if (optDlg == NULL)
        return;

    optDlg->setPreferences(pref);

    if (optDlg->exec() == QDialog::Accepted) {
        bool al = pref.addresslabeling;
        optDlg->getPreferences(pref);
        writeConfigFile();
        fbViewer->updateBusAndModuleStructure();
        if (al != pref.addresslabeling)
            emit repaintLayout();
    }
    delete optDlg;
}


/*create new application window*/
void MainWindow::slotFileNewWin()
{
    MainWindow *sw = new MainWindow();
    sw->resize(740, 480);
    sw->show();
}


/*create new application window and open file*/
void MainWindow::openFileWindow(const QString& fn)
{
    MainWindow *sw = new MainWindow();
    sw->resize(740, 480);
    sw->show();
    sw->openFile(fn);
}


/*
 * create a new locator window and connect its signals directly to gbs
 */
void MainWindow::slotEditFind()
{
    Finder* findWindow = new Finder(this);
    Q_CHECK_PTR(findWindow);
    if (findWindow->exec() == QDialog::Accepted) {
        emit findElement(findWindow->getSearchText(),
                findWindow->getDataType(),
                findWindow->getMatchType());
    }
    delete findWindow;
}


/**
 * This command asks the server for states of all feedback ports, it is
 * only available for SRCP 0.7
 */
void MainWindow::layoutUpdateFB()
{
    if (srcpVersion == 7) {
        SRCPCommandState = srcp07GetFBStates;
        SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgFbGet);
        if (sm == NULL)
            return;

        sm->setFbData(0, (SrcpMessage::Feedback) pref.fbmoduletype, 0);
        sendSrcpMessage(sm);
        delete sm;
    }
}


void MainWindow::layoutChangeSize()
{
    newLayoutDialog* nlDlg = new newLayoutDialog(this);
    if (nlDlg == NULL)
        return;

    nlDlg->setCaption(tr("Change layout settings"));
    nlDlg->setColumns(gbs->getColumns());
    nlDlg->setRows(gbs->getRows());
    nlDlg->setHost(cmdHost);
    nlDlg->setPort(cmdPort);
    nlDlg->setAutoLogin(cmdAutoLogin);
    nlDlg->setAutoPower(cmdAutoPower);
    nlDlg->setAutoSendAll(cmdAutoSendAll);
    
    if (nlDlg->exec() == QDialog::Accepted) {
        int iNewCols = nlDlg->getColumns();
        int iNewRows = nlDlg->getRows();
        gbs->setLayoutSize(iNewCols, iNewRows);
        cmdHost = nlDlg->getHost();
        cmdPort = nlDlg->getPort();
        cmdAutoLogin = nlDlg->getAutoLogin();
        cmdAutoPower = nlDlg->getAutoPower();
        cmdAutoSendAll = nlDlg->getAutoSendAll();
        //TODO:
        //srcpCom->setCmdHost(cmdHost, cmdPort);
        gbs->setModified(true);
    }
    delete nlDlg;
}


void MainWindow::layoutSendAll()
{
    qApp->processEvents();
    gbs->slotSendAll();
}


/*check for changed file data*/
bool MainWindow::isModified()
{
    return (gbs->isModified() || rtController->isModified());
}


/*add a route*/
void MainWindow::slotRouteAdd()
{
    if (rtViewer != NULL)
        rtViewer->slotRouteAdd();
}


/*delete a route*/
void MainWindow::slotRouteDelete()
{
    if (rtViewer != NULL)
        rtViewer->slotRouteDelete();
}
