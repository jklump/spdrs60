/***************************************************************************
                           mainwindow.cpp
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2007 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-11-21 18:12:02 $
                           $Revision: 1.140 $
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

#include <qhbox.h>
#include <qmenubar.h>
#include <qprocess.h>
#include <qtextcodec.h>
#include <qvbox.h>

#include "aboutdialog.h"
#include "finder.h"
#include "gbsscrollview.h"
#include "layouteditmodeagrp.h"
#include "mainwindow.h"
#include "options.h"
#include "paintitemwindow.h"
#include "preferences.h"
#include "resources.h"
#include "serverinfodialog.h"


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
#include "pixmaps/viewtrainnumber.xpm"
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

/*some constants for data files*/
#define GF_CMDHOST       "cmdhost"
#define GF_FORMATVERSION "formatversion"
#define GF_FV            "2"

/*string constants for personal config file*/
#define KS              "="
#define CF_DS           ":"
#define CF_SHOWHP2      "showhp2"
#define CF_TURNOUTBLINK "blinkingturnouts"
#define CF_TOOLTIPS     "tooltips"
#define CF_DATATOOLTIPS "datatooltips"
#define CF_ADDRESSLABEL "addresslabel"
#define CF_INITSIGNALS  "initsignals"
#define CF_LAYOUTCOLS   "layoutcolumns"
#define CF_LAYOUTROWS   "layoutrows"
#define CF_SENDSTATE    "sendstate"
#define CF_CONVERTTIME  "converttime"
#define CF_SHOWTIME     "showtime"
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
#define CF_KEYBOARD     "keyboardprotocol"

#define SPDRS60_INIT   ".spdrs60rc" // program init filename


MainWindow::MainWindow(): QMainWindow(NULL, PACKAGE,
        Qt::WDestructiveClose | Qt::WGroupLeader)
{
    setIcon(QPixmap(spdrs60_32));

    cmdAutoLogin = false;
    cmdAutoPower = false;
    cmdAutoSendAll = true;

    /*Networking */
    commandStyle = SrcpPort::csNone;
    infoStyle = SrcpPort::csNone;
    SRCPCommandState = srcpUndefined;
    SRCPInfoState = srcpUndefined;
    LayoutPowerIsOn = false;

    keybWindow = NULL;
    trainnumberdialog = NULL;
    isFBInitMode = true;            // var to avoid all startup feedback
    visualMode = kvmNormal;         // normal layout mode
    lastDir = QDir::homeDirPath();  // remembers path for FileOpen

    // init two sockets with default values, feedback socket for SRCP
    // 0.7 communication is created on demand
    commandPort = new CommandPort(this, "commandPort");
    Q_CHECK_PTR(commandPort);

    connect(commandPort, SIGNAL(connectionStateChanged(bool)),
            this, SLOT(updateCommandConnectionState(bool)));
    connect(commandPort, SIGNAL(messageReceived(const QString&)),
            this, SLOT(processCommandMessage(const QString&)));


    infoPort = new InfoPort(this, "infoPort");
    Q_CHECK_PTR(infoPort);

    connect(infoPort, SIGNAL(connectionStateChanged(bool)),
            this, SLOT(updateInfoConnectionState(bool)));
    connect(infoPort, SIGNAL(messageReceived(const QString&)),
            this, SLOT(processInfoMessage(const QString&)));


    feedbackPort = new InfoPort(this, "feedbackPort");
    Q_CHECK_PTR(feedbackPort);

    connect(feedbackPort, SIGNAL(connectionStateChanged(bool)),
            this, SLOT(updateFeedbackConnectionState(bool)));
    connect(feedbackPort, SIGNAL(messageReceived(const QString&)),
            this, SLOT(processFeedbackMessage(const QString&)));

    // setup main window with all menus
    initMainWindow();
    // read user dependend config file
    readConfigFile();
    statusMessage(tr("Program succesfully started!"));
}

/* Cleanup by destructor */
MainWindow::~MainWindow()
{
    // send LOGOUT command to daemon
    CloseSRCPServerConnection();

    // port destructors take care to close server connection
    delete commandPort;
    delete infoPort;
    delete feedbackPort;
}

/**
 * read application settings from config file, this is typicaly
 * done on application startup
 */
void MainWindow::readConfigFile()
{
    QFile file(QDir::homeDirPath() + "/" + SPDRS60_INIT);
#if QT_VERSION >= 0x040000
    if (!file.open(QIODevice::ReadOnly)) {
#else
    if (!file.open(IO_ReadOnly)) {
#endif
        /* if no configuration file is found, just keep defaults */
        statusMessage(tr("User preferences file not found") + ": ~/" +
                   SPDRS60_INIT);
        return;
    }
    QTextStream ts(&file);
    QString s, key, value;

    while (!ts.eof()) {
        s = ts.readLine();
        if (!s.startsWith("#")) {
            key = s.section(KS, 0, 0);
            value = s.section(KS, 1, 1).stripWhiteSpace();

            if (key.compare(CF_SHOWHP2) == 0) {
                pref.hp2 = value.toInt();
            }
            else if (key.compare(CF_TURNOUTBLINK) == 0) {
                pref.blinkingturnouts = value.toInt();
            }
#if QT_VERSION >= 0x040000
#else
            else if (key.compare(CF_TOOLTIPS) == 0) {
                QToolTip::setGloballyEnabled(value.toInt());
            }
#endif
            else if (key.compare(CF_DATATOOLTIPS) == 0) {
                pref.datatooltips = value.toInt();
            }
            else if (key.compare(CF_ADDRESSLABEL) == 0) {
                pref.addresslabeling = value.toInt();
            }
            else if (key.compare(CF_INITSIGNALS) == 0) {
                pref.initsignalsred = value.toInt();
            }
            else if (key.compare(CF_LAYOUTCOLS) == 0) {
                pref.layoutcols = value.toUInt();
            }
            else if (key.compare(CF_LAYOUTROWS) == 0) {
                pref.layoutrows = value.toUInt();
            }
            else if (key.compare(CF_SENDSTATE) == 0) {
                pref.sendstate = value.toInt();
            }
            else if (key.compare(CF_CONVERTTIME) == 0) {
                pref.converttime = value.toInt();
            }
            else if (key.compare(CF_SHOWTIME) == 0) {
                pref.showtime = value.toInt();
            }
            else if (key.compare(CF_AUTOLOAD) == 0) {
                pref.autoload = value.toInt();
            }
            else if (key.compare(CF_AUTOLAYOUT) == 0) {
                pref.autolayout = value;
            }
            else if (key.compare(CF_AUTOSAVE) == 0) {
                pref.autosave = value.toInt();
            }
            else if (key.compare(CF_EDITOR) == 0) {
                pref.editor = value;
            }
            else if (key.compare(CF_BROWSER) == 0) {
                pref.browser = value;
            }
            else if (key.compare(CF_PROTOCOL) == 0) {
                pref.protocol = 2;
                if (value == "DCC")
                    pref.protocol = 0;
                else if (value == "Motorola")
                    pref.protocol = 1;
                else if (value == "Selectrix")
                    pref.protocol = 3;
            }
            else if (key.compare(CF_DECODER) == 0) {
                pref.decoder = value;
            }
            else if (key.compare(CF_ACTIVETIME) == 0) {
                pref.activetime = value.toInt();
            }
            else if (key.compare(CF_AUTOTTDIR) == 0) {
                pref.autottdir = value.toInt();
            }
            else if (key.compare(CF_TTROUNDTIME) == 0) {
                pref.ttroundtime = value.toDouble();
            }
            else if (key.compare(CF_ROUTINGTIME) == 0) {
                pref.routingtime = value.toInt();
            }
            //TODO: remove before release
            else if (key.compare(CF_FEEDBACKTYPE) == 0) {
                pref.fbfactor = value.toInt();
            }
            else if (key.compare(CF_FBMODSIZE) == 0) {
                pref.fbfactor = value.toInt();
            }
            else if (key.compare(CF_FBMODTYPE) == 0) {
                pref.fbmoduletype = value.toInt();
            }
            else if (key.compare(CF_FIXEDBUSNUM) == 0) {
                pref.fixedbusnum = value.toInt();
            }
            else if (key.compare(CF_FBBUS1) == 0) {
                pref.fbbus1.number = value.section(CF_DS, 0, 0).toUInt();
                pref.fbbus1.modules = value.section(CF_DS, 1, 1).toUInt();
            }
            else if (key.compare(CF_FBBUS2) == 0) {
                pref.fbbus2.number = value.section(CF_DS, 0, 0).toUInt();
                pref.fbbus2.modules = value.section(CF_DS, 1, 1).toUInt();
            }
            else if (key.compare(CF_FBBUS3) == 0) {
                pref.fbbus3.number = value.section(CF_DS, 0, 0).toUInt();
                pref.fbbus3.modules = value.section(CF_DS, 1, 1).toUInt();
            }
            else if (key.compare(CF_FBBUS4) == 0) {
                pref.fbbus4.number = value.section(CF_DS, 0, 0).toUInt();
                pref.fbbus4.modules = value.section(CF_DS, 1, 1).toUInt();
            }
            else if (key.compare(CF_LASTDIR) == 0) {
                lastDir = value.stripWhiteSpace();
                // check if directory is valid
                if (!QFile::exists(lastDir))
                    lastDir = QDir::homeDirPath();
            }
            else if (key.compare(CF_KEYBOARD) == 0) {
                pref.keyboardprot = value.toInt();
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
    
#if QT_VERSION >= 0x040000
    if (!file.open(QIODevice::WriteOnly)) {
#else
    if (!file.open(IO_WriteOnly)) {
#endif
        statusMessage(tr("Error: Could not save configuration"
                    " file: ~/%1").arg(SPDRS60_INIT));
        return;
    }
    statusMessage(tr("Writing " PACKAGE " configuration"
                " file: ~/%1").arg(SPDRS60_INIT));

    QDateTime dt = QDateTime::currentDateTime();
    QTextStream ts(&file);

    QString rtstr;
    rtstr.sprintf("%.2f", pref.ttroundtime);

    ts  << "# " PACKAGE " configuration file" << endl
        << "# last modified: " << dt.toString(Qt::ISODate) << endl
        << "#" << endl
        << CF_SHOWHP2      << KS << (int) pref.hp2 << endl
        << CF_TURNOUTBLINK << KS << (int) pref.blinkingturnouts << endl
#if QT_VERSION >= 0x040000
#else
        << CF_TOOLTIPS     << KS << (int) QToolTip::isGloballyEnabled() << endl
#endif
        << CF_DATATOOLTIPS << KS << (int) pref.datatooltips << endl
        << CF_ADDRESSLABEL    KS << (int) pref.addresslabeling << endl
        << CF_INITSIGNALS  << KS << (int) pref.initsignalsred << endl
        << CF_LAYOUTCOLS   << KS << pref.layoutcols << endl
        << CF_LAYOUTROWS   << KS << pref.layoutrows << endl
        << CF_SENDSTATE    << KS << pref.sendstate << endl
        << CF_CONVERTTIME  << KS << pref.converttime << endl
        << CF_AUTOLOAD     << KS << (int) pref.autoload << endl
        << CF_AUTOLAYOUT   << KS << pref.autolayout << endl
        << CF_AUTOSAVE     << KS << (int) pref.autosave << endl
        << CF_EDITOR       << KS << pref.editor << endl
        << CF_BROWSER      << KS << pref.browser << endl
        << CF_PROTOCOL     << KS <<
            ((pref.protocol == 1) ? "Motorola" 
            : (pref.protocol == 0) ? "DCC"
            : (pref.protocol == 3) ? "Selectrix" : "Server") << endl
        << CF_DECODER      << KS << pref.decoder << endl
        << CF_ACTIVETIME   << KS << pref.activetime << endl
        << CF_AUTOTTDIR    << KS << (int) pref.autottdir << endl
        << CF_TTROUNDTIME  << KS << rtstr << endl
        << CF_ROUTINGTIME  << KS << pref.routingtime << endl
        << CF_FBMODSIZE    << KS << pref.fbfactor << endl
        << CF_FBMODTYPE    << KS << pref.fbmoduletype << endl
        << CF_FIXEDBUSNUM  << KS << pref.fixedbusnum << endl
        << CF_FBBUS1       << KS << pref.fbbus1.number
                           << CF_DS << pref.fbbus1.modules << endl
        << CF_FBBUS2       << KS << pref.fbbus2.number
                           << CF_DS << pref.fbbus2.modules << endl
        << CF_FBBUS3       << KS << pref.fbbus3.number
                           << CF_DS << pref.fbbus3.modules << endl
        << CF_FBBUS4       << KS << pref.fbbus4.number
                           << CF_DS << pref.fbbus4.modules << endl
        << CF_LASTDIR      << KS << lastDir << endl
        << CF_KEYBOARD     << KS << pref.keyboardprot << endl;

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
     | | | +---------------------------------------------+ | | |
     | | +-------------------------------------------------+ | |
     | +-----------------------------------------------------+ |
     | | +-------------------------------------------------+ | |
     | | |  MessageHistory                                 | | |
     | | +-------------------------------------------------+ | |
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
    connect(gbs, SIGNAL(sendSrcpMessage(SrcpMessage*)),
            this, SLOT(sendSrcpMessage(SrcpMessage*)));
    connect(gbs, SIGNAL(sigShowFBmodules()),
            this, SLOT(slotShowModules()));


    /*history line*/
    messageHistory = new MessageHistory(vBox);
    Q_CHECK_PTR(messageHistory);

    connect(commandPort, SIGNAL(messageReceived(const QString&)),
            messageHistory, SLOT(addCommand(const QString&)));
    connect(commandPort, SIGNAL(messageSend(const QString&)),
            messageHistory, SLOT(addCommand(const QString&)));
    connect(commandPort, SIGNAL(statusMessage(const QString&)),
            messageHistory, SLOT(addHint(const QString&)));

    connect(infoPort, SIGNAL(messageReceived(const QString&)),
            messageHistory, SLOT(addInfo(const QString&)));
    connect(infoPort, SIGNAL(messageSend(const QString&)),
            messageHistory, SLOT(addInfo(const QString&)));
    connect(infoPort, SIGNAL(statusMessage(const QString&)),
            messageHistory, SLOT(addHint(const QString&)));

    connect(feedbackPort, SIGNAL(messageReceived(const QString&)),
            messageHistory, SLOT(addInfo(const QString&)));
    connect(feedbackPort, SIGNAL(messageSend(const QString&)),
            messageHistory, SLOT(addInfo(const QString&)));
    connect(feedbackPort, SIGNAL(statusMessage(const QString&)),
            messageHistory, SLOT(addHint(const QString&)));

    // FIXME: this will cause doubled messages or there will be no
    // server time translation
    //connect(this, SIGNAL(commandMessage(const QString&)),
    //        messageHistory, SLOT(addCommand(const QString&)));
    //connect(this, SIGNAL(infoMessage(const QString&)),
    //        messageHistory, SLOT(addInfo(const QString&)));
    connect(this, SIGNAL(statusMessage(const QString&)),
            messageHistory, SLOT(addHint(const QString&)));
    connect(gbs, SIGNAL(statusMessage(const QString&)),
            messageHistory, SLOT(addHint(const QString&)));

    setCentralWidget(vBox);

    /*feedback module viewer*/
    fbViewer = new FeedbackViewer(this, "feedbackviewer");
    Q_CHECK_PTR(fbViewer);
    moveDockWindow(fbViewer, Qt::DockLeft);
    connect(this, SIGNAL(sendFBChangeLayout(unsigned int, unsigned int,
                    bool)),
            fbViewer, SLOT(feedbackPortChanged(unsigned int, unsigned int,
                    bool)));
    fbViewer->hide();
    
    /*route controller*/
    router = new Router(this, gbs->getGbsElementListPtr(),
            "router");
    Q_CHECK_PTR(router);
    connect(this, SIGNAL(switchedVisualMode(elemVisualMode)),
            router, SLOT(switchVisualMode(elemVisualMode)));
    connect(this, SIGNAL(sendFBChangeRoute(unsigned int, unsigned int, bool)),
            router, SLOT(feedbackPortChanged(unsigned int,
                    unsigned int, bool)));
    connect(gbs, SIGNAL(clearRoutes()),
            router, SLOT(clearRoutes()));
    connect(gbs, SIGNAL(recordElement(element*, elemRecordType)),
            router, SLOT(recordElement(element*, elemRecordType)));
    connect(gbs, SIGNAL(setRoute(element*, GbsButtonState,
                    GbsButtonState)),
            router, SLOT(setRoute(element*, GbsButtonState,
                    GbsButtonState)));
    connect(gbs, SIGNAL(resetRoute(element*, GbsButtonState)),
            router, SLOT(resetRoute(element*, GbsButtonState)));
    connect(router, SIGNAL(routeFunctionFinished()),
            gbs, SLOT(slotElementClickedTimeout()));
    connect(router, SIGNAL(startRouteTimer(Route::RouteType)),
            gbs, SLOT(startRouteTimer(Route::RouteType)));
    connect(gbs, SIGNAL(resetSelectedSignal()),
            router, SLOT(resetSelectedSignal()));
    connect(router, SIGNAL(updateRoutePathLEDs(const stateElement&,
                    const stateElement&, Route::RouteSetAction&)),
            gbs, SLOT(updateRoutePathLEDs(const stateElement&,
                    const stateElement&, Route::RouteSetAction&)));
    connect(router, SIGNAL(statusMessage(const QString&)),
            messageHistory, SLOT(addHint(const QString&)));
    
    /*route list window*/
    rtViewer = new RouteListWindow(this, "routeListWindow", router);
    Q_CHECK_PTR(rtViewer);
    moveDockWindow(rtViewer, Qt::DockRight);
    rtViewer->hide();
    connect(this, SIGNAL(switchedVisualMode(elemVisualMode)),
            rtViewer, SLOT(switchVisualMode(elemVisualMode)));
    connect(rtViewer, SIGNAL(visibilityChanged(bool)),
            this, SLOT(updateRouteMenu(bool)));
    connect(rtViewer, SIGNAL(selectedRouteChangedState()),
            this, SLOT(updateRouteMenuActivateItems()));
    connect(rtViewer, SIGNAL(routeListIsEmpty()),
            this, SLOT(updateRouteListMenuItems()));
    connect(router, SIGNAL(routeListChanged()),
            rtViewer, SLOT(updateRouteList()));
    connect(router, SIGNAL(routeDataChanged(Route*)),
            rtViewer, SLOT(updateRouteData(Route*)));
    connect(router, SIGNAL(routeStateChanged(Route*)),
            rtViewer, SLOT(updateRouteState(Route*)));
    connect(router, SIGNAL(getElementByAddress(const int, const int,
                    element**)), gbs,
            SLOT(getElementByAddress(const int, const int, element**)));

    /*paint item window*/
    PaintItemWindow* piw = new PaintItemWindow(this, "piw");
    Q_CHECK_PTR(piw);
    moveDockWindow(piw, Qt::DockLeft);
    piw->hide();
    connect(piw, SIGNAL(paintItemChanged(element::SpdrItemClassId)),
            gbs, SLOT(changeLayoutPaintItem(element::SpdrItemClassId)));

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
    
#if QT_VERSION >= 0x040000
    actionEditFileLayout = new QAction(tr("&Layout"), this);
#elif QT_VERSION >= 0x030200
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

#if QT_VERSION >= 0x040000
    actionEditFileOptions = new QAction(
            QDir::homeDirPath() + "/" + SPDRS60_INIT, this);
#elif QT_VERSION >= 0x030200
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
    actionViewTrainNumberDialog = new QAction(QPixmap(viewtrainnumber_xpm),
            tr("&Train number dialog"), 0, this, "viewTrainNumberDlg");
    actionViewTrainNumberDialog->setToolTip(tr("Show train number input dialog"));
#else
    actionViewTrainNumberDialog = new QAction(
            tr("Show train number input dialog"), QPixmap(viewtrainnumber_xpm),
            tr("&Keyboard"), 0, this, "viewTrainNumberDlr");
#endif
    connect(actionViewTrainNumberDialog, SIGNAL(activated()), this,
            SLOT(slotViewTrainNumberDialog()));
    actionViewTrainNumberDialog->addTo(viewmenu);
    actionViewTrainNumberDialog->addTo(viewtb);
    
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
            tr("&Layout edit mode"), Qt::CTRL + Qt::Key_E, ViewGrp,
            "layouteditmode");
#else
    actionViewLayoutEditMode = new QAction("", QPixmap(viewlayouteditmode_xpm),
            tr("&Layout edit mode"), Qt::CTRL + Qt::Key_E, ViewGrp,
            "layouteditmode");
#endif
    actionViewLayoutEditMode->setToggleAction(true);
    
#if QT_VERSION >= 0x030200
    actionViewRouteEditMode = new QAction(QPixmap(viewrouteeditmode_xpm),
            tr("&Route edit mode"), Qt::CTRL + Qt::Key_B, ViewGrp,
            "routeeditmode");
#else
    actionViewRouteEditMode = new QAction("", QPixmap(viewrouteeditmode_xpm),
            tr("&Route edit mode"), Qt::CTRL + Qt::Key_B, ViewGrp,
            "routeeditmode");
#endif
    actionViewRouteEditMode->setToggleAction(true);
    
    ViewGrp->addTo(viewmenu);
    ViewGrp->addTo(viewtb);

    viewmenu->insertSeparator();

#if QT_VERSION >= 0x040000
    QAction* actionViewMenu = new QAction(
            tr("&Menu"), Qt::CTRL + Qt::Key_U, this, "viewMenu");
#elif QT_VERSION >= 0x030200
    QAction* actionViewMenu = new QAction(NULL,
            tr("&Menu"), Qt::CTRL + Qt::Key_U, this, "viewMenu");
    actionViewMenu->setToolTip(tr("Show the main menu"));
#else
    QAction* actionViewMenu = new QAction(tr("Show the main menu"),
            tr("&Menu"), Qt::CTRL + Qt::Key_U, this, "viewMenu");
#endif
    actionViewMenu->setToggleAction(true);
    actionViewMenu->setOn(true);
    actionViewMenu->addTo(viewmenu);
    connect(actionViewMenu, SIGNAL(toggled(bool)), menuBar(),
            SLOT(setShown(bool)));

#if QT_VERSION >= 0x040000
    QAction* actionViewToolbar = new QAction(
            tr("Tool&bars"), 0, this, "viewToolbar");
#elif QT_VERSION >= 0x030200
    QAction* actionViewToolbar = new QAction(NULL,
            tr("Tool&bars"), 0, this, "viewToolbar");
    actionViewToolbar->setToolTip(tr("Show the toolbars"));
#else
    QAction* actionViewToolbar = new QAction(tr("Show the toolbars"),
            tr("Tool&bars"), 0, this, "viewToolbar");
#endif
    actionViewToolbar->setToggleAction(true);
    actionViewToolbar->setOn(true);
    actionViewToolbar->addTo(viewmenu);

#if QT_VERSION >= 0x040000
    QAction* actionViewStatusline = new QAction(
            tr("Status&line"), 0, this, "viewStatusline");
#elif QT_VERSION >= 0x030200
    QAction* actionViewStatusline = new QAction(NULL,
            tr("Status&line"), 0, this, "viewStatusline");
    actionViewStatusline->setToolTip(tr("Show the statusline"));
#else
    QAction* actionViewStatusline = new QAction(tr("Show the statusline"),
            tr("Status&line"), 0, this, "viewStatusline");
#endif
    actionViewStatusline->setToggleAction(true);
    actionViewStatusline->setOn(true);
    connect(actionViewStatusline, SIGNAL(toggled(bool)), messageHistory,
            SLOT(setShown(bool)));
    actionViewStatusline->addTo(viewmenu);

    viewmenu->insertSeparator();

#if QT_VERSION >= 0x040000
    actionViewToggleHistory = new QAction(
            tr("Toggle &history line"), Qt::CTRL + Qt::Key_D, // Ctrl H/T
            this, "viewToggleHistory");
#elif QT_VERSION >= 0x030200
    actionViewToggleHistory = new QAction(NULL,
            tr("Toggle &history line"), Qt::CTRL + Qt::Key_D, // Ctrl H/T
            this, "viewToggleHistory");
    actionViewToggleHistory->setToolTip(tr("Toggle the history line"));
#else
    actionViewToggleHistory = new QAction(tr("Toggle the history line"),
            tr("Toggle &history line"), Qt::CTRL + Qt::Key_D, // Ctrl H/T
            this, "viewToggleHistory");
#endif
    connect(actionViewToggleHistory, SIGNAL(activated()), messageHistory,
            SLOT(toggleLine()));
    actionViewToggleHistory->addTo(viewmenu);
    //actionViewToggleHistory->addTo(viewtb);


    /*daemon toolbar*/
    QToolBar* daemontb = new QToolBar(this, "daemontb");
    Q_CHECK_PTR(daemontb);
    daemontb->setLabel(tr("Daemon operations"));
    connect(actionViewNormalMode, SIGNAL(toggled(bool)),
            daemontb, SLOT(setShown(bool)));

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
    connect(actionViewNormalMode, SIGNAL(toggled(bool)),
            layouttb, SLOT(setShown(bool)));

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

#if QT_VERSION >= 0x040000
    actionLayoutFht = new QAction(
            tr("Use &FHT"), Qt::Key_F5, this, "layoutFht");
#elif QT_VERSION >= 0x030200
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

#if QT_VERSION >= 0x040000
    actionLayoutUfgt = new QAction(
            tr("Use &UfGT"), Qt::Key_F6, this, "layoutUfgt");
#elif QT_VERSION >= 0x030200
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

#if QT_VERSION >= 0x040000
    actionLayoutWgt = new QAction(
            tr("Use &WGT"), Qt::Key_F7, this, "layoutWgt");
#elif QT_VERSION >= 0x030200
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

#if QT_VERSION >= 0x040000
    actionLayoutSgt = new QAction(
            tr("Use &SGT"), Qt::Key_F8, this, "layoutSgt");
#elif QT_VERSION >= 0x030200
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

#if QT_VERSION >= 0x040000
    actionLayoutHagt = new QAction(
            tr("Use H&aGT"), Qt::Key_F9, this, "layoutHagt");
#elif QT_VERSION >= 0x030200
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

#if QT_VERSION >= 0x040000
    actionLayoutToggleAll = new QAction(
            tr("&Toggle all"), Qt::Key_F10, this, "layoutToggleAll");
#elif QT_VERSION >= 0x030200
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

#if QT_VERSION >= 0x040000
    actionLayoutSendAll = new QAction(
            tr("Send &all"), Qt::Key_F11, this, "layoutSendAll");
#elif QT_VERSION >= 0x030200
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

#if QT_VERSION >= 0x040000
    actionLayoutUpdateFB = new QAction(
            tr("Up&date feedback states"), 0, this, "layoutUpdateFB");
#elif QT_VERSION >= 0x030200
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

#if QT_VERSION >= 0x040000
    actionLayoutChangeSize = new QAction(
            tr("S&ettings..."), 0, this, "layoutChangeSettings");
#elif QT_VERSION >= 0x030200
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
    actionRouteActivate = new QAction(QPixmap(route_start_xpm),
            tr("Ac&tivate"), 0, this, "routeActivate");
    actionRouteActivate->setToolTip(tr("Activate route"));
#else
    actionRouteActivate = new QAction(tr("Activate route"),
            QPixmap(route_start_xpm),
            tr("Ac&tivate"), 0, this, "routeActivate");
#endif
    connect(actionRouteActivate, SIGNAL(activated()), rtViewer,
            SLOT(slotRouteActivate()));
    actionRouteActivate->addTo(routemenu);
    actionRouteActivate->addTo(routetb);

#if QT_VERSION >= 0x030200
    actionRouteWithdraw = new QAction(QPixmap(route_stop_xpm), tr("W&ithdraw"),
            0, this, "routeWithdraw");
    actionRouteWithdraw->setToolTip(tr("Withdraw a wrong activated route"));
    actionRouteWithdraw->setText(tr("Withdraw route"));
#else
    actionRouteWithdraw = new QAction(tr("Withdraw route"),
            QPixmap(route_stop_xpm), tr("W&ithdraw"),
            0, this, "routeWithdraw");
#endif
    connect(actionRouteWithdraw, SIGNAL(activated()), rtViewer,
            SLOT(slotRouteWithdraw()));
    actionRouteWithdraw->addTo(routemenu);
    actionRouteWithdraw->addTo(routetb);

#if QT_VERSION >= 0x040000
    actionRouteRelease = new QAction(tr("&Release"),
            0, this, "routeRelease");
#elif QT_VERSION >= 0x030200
    //actionRouteRelease = new QAction(QPixmap(route_stop_xpm), tr("&Release"),
    actionRouteRelease = new QAction(NULL, tr("&Release"),
            0, this, "routeRelease");
    actionRouteRelease->setToolTip(tr("Release a activated route. This\n"
                "includes a train number transfer."));
    actionRouteRelease->setText(tr("Release route"));
#else
    actionRouteRelease = new QAction(tr("Release route"),
            QPixmap(route_stop_xpm), tr("&Release"),
            0, this, "routeRelease");
#endif
    connect(actionRouteRelease, SIGNAL(activated()), rtViewer,
            SLOT(slotRouteRelease()));
    actionRouteRelease->addTo(routemenu);
    //actionRouteRelease->addTo(routetb);

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

#if QT_VERSION >= 0x040000
    actionRouteUnlockAll = new QAction(
            tr("&Unlock all"), Qt::CTRL + Qt::Key_U, this,
            "layoutUnlockRoutes");
#elif QT_VERSION >= 0x030200
    actionRouteUnlockAll = new QAction(NULL,
            tr("&Unlock all"), Qt::CTRL + Qt::Key_U, this,
            "layoutUnlockRoutes");
    actionRouteUnlockAll->setToolTip(tr("Unlock all routes"));
#else
    actionRouteUnlockAll = new QAction(tr("Unlock all routes"),
            tr("&Unlock all"), Qt::CTRL + Qt::Key_U, this,
            "layoutUnlockRoutes");
#endif
    connect(actionRouteUnlockAll, SIGNAL(activated()), router,
            SLOT(unlockAllLockedRoutes()));
    actionRouteUnlockAll->addTo(routemenu);
    //actionRouteUnlockAll->addTo(routetb);


    /*layout edit toolbar*/
    QToolBar* layoutedittb = new QToolBar(this, "layoutedittb");
    Q_CHECK_PTR(layoutedittb);
    layoutedittb->setLabel(tr("Layout edit operations"));
    layoutedittb->hide();
    layoutedittb->setEnabled(false);

    //connect visibility of toolbar to toggle state of layout edit mode
    //action 
    connect(actionViewLayoutEditMode, SIGNAL(toggled(bool)),
            layoutedittb, SLOT(setShown(bool)));
    connect(actionViewLayoutEditMode, SIGNAL(toggled(bool)),
            layoutedittb, SLOT(setEnabled(bool)));

    //connect visibility of paint item window 
    connect(actionViewLayoutEditMode, SIGNAL(toggled(bool)),
            piw, SLOT(setShown(bool)));

    LayoutEditModeAgrp *layoutEditGrp = new LayoutEditModeAgrp(this,
            "layoutEditModeGroup");
    layoutEditGrp->addTo(layoutedittb);

    connect(layoutEditGrp, SIGNAL(modeChanged(GBSArea::LayoutEditMode)),
            gbs, SLOT(changeLayoutEditMode(GBSArea::LayoutEditMode)));
    connect(actionViewLayoutEditMode, SIGNAL(toggled(bool)),
            layoutEditGrp, SLOT(enableEditMode(bool)));
    connect(layoutEditGrp, SIGNAL(modeChanged(GBSArea::LayoutEditMode)),
            piw, SLOT(changeLayoutEditMode(GBSArea::LayoutEditMode)));
    
    
    /*help toolbar*/
    //QToolBar* helptb = new QToolBar(this, "helptb");
    //Q_CHECK_PTR(helptb);
    //helptb->setLabel(tr("Help operations"));

    QPopupMenu* helpmenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&Help"), helpmenu);

    helpmenu->insertItem(tr("&Help"), this, SLOT(slotAboutHelp()), Qt::Key_F1);
    helpmenu->insertItem(tr("&spdrs60 for Linux on the web"),
            this, SLOT(slotAboutWeb()));
    helpmenu->insertSeparator();
    helpmenu->insertItem(tr("&About %1...").arg(PACKAGE),
            this, SLOT(slotAbout()));
    helpmenu->insertItem(tr("About &Qt..."), this, SLOT(slotAboutQt()));

    // connect all toolbars to be hidable
    connect(actionViewToolbar, SIGNAL(toggled(bool)), filetb,
            SLOT(setShown(bool)));
    connect(actionViewToolbar, SIGNAL(toggled(bool)), edittb,
            SLOT(setShown(bool)));
    connect(actionViewToolbar, SIGNAL(toggled(bool)), viewtb,
            SLOT(setShown(bool)));
    connect(actionViewToolbar, SIGNAL(toggled(bool)), daemontb,
            SLOT(setShown(bool)));
    connect(actionViewToolbar, SIGNAL(toggled(bool)), layouttb,
            SLOT(setShown(bool)));
    //connect(actionViewHideToolbar, SIGNAL(toggled(bool)), routetb,
    //        SLOT(setShown(bool)));

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
    nlDlg->setHost(commandPort->getHostname());
    nlDlg->setPort(commandPort->getPortNumber());
    nlDlg->setAutoLogin(cmdAutoLogin);
    nlDlg->setAutoPower(cmdAutoPower);
    nlDlg->setAutoSendAll(cmdAutoSendAll);
    
    if (nlDlg->exec() != QDialog::Accepted) {
        delete nlDlg;
        return;
    }
    int iNewCols = nlDlg->getColumns();
    int iNewRows = nlDlg->getRows();
    gbs->setLayoutSize(iNewCols, iNewRows);

    commandPort->setServer(nlDlg->getHost(), nlDlg->getPort());
    infoPort->setServer(nlDlg->getHost(), nlDlg->getPort());
    feedbackPort->setServer(nlDlg->getHost(), nlDlg->getPort());

    cmdAutoLogin = nlDlg->getAutoLogin();
    cmdAutoPower = nlDlg->getAutoPower();
    cmdAutoSendAll = nlDlg->getAutoSendAll();
    delete nlDlg;
    
    CloseSRCPServerConnection();
    fileName = "";
    gbs->newFile(iNewCols, iNewRows);

    updateCaption();
    updateFileMenuItems();
    statusMessage(tr("New layout file created"));
}


void MainWindow::updateFileMenuItems()
{
    actionFileSave->setEnabled(true);
    actionFileSaveAs->setEnabled(true);

    if (fileName.isEmpty()) {
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
                SrcpPort::csOld == commandStyle);
    //}

    //viewmenu->setItemEnabled(VIEW_ID_ROUTES, !fileName.isEmpty());
    actionViewRoutes->setEnabled(true);
    actionViewLayoutEditMode->setEnabled(true);
    actionViewRouteEditMode->setEnabled(true);
    actionRouteUnlockAll->setEnabled(true);
}


bool MainWindow::saveFile()
{
    if (fileName.isEmpty()) {
        slotFileSaveAs();
        return true;
    }

    QFile f(fileName);
#if QT_VERSION >= 0x040000
    if (!f.open(QIODevice::WriteOnly)) {
#else
    if (!f.open(IO_WriteOnly)) {
#endif
        statusMessage(tr("Could not write to file '%1'").arg(fileName));
        return false;
    }

    QTextStream ts(&f);
    ts.setEncoding(QTextStream::UnicodeUTF8);
    
    QDateTime dt = QDateTime::currentDateTime();
    
    // write the header
    ts << "# spdrs60 data file" << endl
       << "# version=" << VERSION << endl
       << "# last modified=" << dt.toString(Qt::ISODate) << endl
       << GF_FORMATVERSION << CF_DS << GF_FV << endl
       << GF_CMDHOST << CF_DS << commandPort->getHostname()
       << CF_DS << commandPort->getPortNumber()
       << CF_DS << cmdAutoLogin << CF_DS << cmdAutoPower
       << CF_DS << cmdAutoSendAll << endl;

    gbs->writeFileTextToStream(ts);
    router->writeFileTextToStream(ts);

    f.close();

    updateCaption();
    updateFileMenuItems();

    statusMessage(tr("Layout file '%1' saved").arg(fileName));
    return true;
}


void MainWindow::slotFileSave()
{
    saveFile();
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
        if (QFile::exists(fn)) {
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
        statusMessage(tr("Saving aborted"));
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

#if QT_VERSION >= 0x040000
    if (!f.open(QIODevice::ReadOnly)) {
#else
    if (!f.open(IO_ReadOnly)) {
#endif
        statusMessage(tr("Could not read file '%1'").arg(fn));
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
            key = s.section(CF_DS, 0, 0);
            value = s.section(CF_DS, 1, 1).stripWhiteSpace();

            /* key/value pairs are read sequence independent */
            if (key.compare(GF_CMDHOST) == 0) {
                QStringList tokens = QStringList::split(CF_DS, s);

                unsigned int prt = tokens[2].toUInt();
                commandPort->setServer(tokens[1], prt);
                infoPort->setServer(tokens[1], prt);
                feedbackPort->setServer(tokens[1], prt);

                cmdAutoLogin = tokens[3].toInt() == 1;
                cmdAutoPower = tokens[4].toInt() == 1;
                cmdAutoSendAll = tokens[5].toInt() == 1;
            }
            else if (key.compare(GF_FORMATVERSION) == 0)
                fversion = value.toInt();
            else if (s.startsWith("%% layout"))
                break;
        }
    }

    gbs->readFileTextFromStream(ts);
    qApp->processEvents();
    router->readFileTextFromStream(ts);

    f.close();
    
    statusMessage(tr("Layout file '%1' opened").arg(fn));
    updateCaption();
    updateFileMenuItems();

    if (cmdAutoLogin) {
        ConnectToSRCPServer();
        qApp->processEvents();

        //TODO: check if this is necessary here, effect may be doubled
        //with fbInit
        // update feedback states
        // layoutUpdateFB();
    }
}


void MainWindow::updateCaption()
{
    if (fileName.isEmpty())
        setCaption(QString(PACKAGE) + " - [" + tr("noname") + "]");
    else
        setCaption(QString(PACKAGE) + " - [" + fileName + "]");
}


void MainWindow::processCommandMessage(const QString& command)
{

    if ((SrcpPort::csNew == commandStyle) && pref.converttime) {
        QString msgstr = ConvertMessageTime(command);
        commandMessage(msgstr);
    }
    else
        commandMessage(command);

    /*
     * respond to SRCP 0.7 messages 
     */
    if (SRCPCommandState == srcp07GetPower) {
        /* INFO POWER ON */
        if (command.contains("POWER ON")) {
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
    }

    else if (SRCPCommandState == srcp07GetFBStates) {
        if (command.startsWith("INFO FB")) {

            /*
             * 1) all feedback ports
             * INFO FB <module_type> * <all states>
             *   0   1      2        3       4
             */
            if ("*" == command.section(" ", 3, 3)) {
                QString allstates = command.section(" ", 4, 4);

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

                fbport = command.section(" ", 3, 3).toUInt();
                fbstate = command.section(" ", 4, 4).toUInt();
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
    else if (SRCPCommandState == srcp08TermServer) {
        if (command.contains("200 OK")) {
            SRCPCommandState = srcpUndefined;
            CloseSRCPServerConnection();
        }
    }

    else if (SRCPCommandState == srcp08InitGADevices) {
        /*
         * start FB bus init sequence
         * when GA init is done, go to FB bus init
         */
        if (!gbs->runSRCP08GAInitSequence()) {
            SRCPCommandState = srcp08InitFBBusses;
            if (!gbs->sendSRCP08BusMessage(SrcpMessage::msgPowerInit)) {
                if (cmdAutoPower) {
                    SRCPCommandState = srcp08GetBusPower;
                    if (!gbs->sendSRCP08BusMessage(
                                SrcpMessage::msgPowerGet)) {
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
            if (cmdAutoPower) {
                SRCPCommandState = srcp08GetBusPower;
                if (!gbs->sendSRCP08BusMessage(
                            SrcpMessage::msgPowerGet)) {
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
        if (command.section(" ", 1, 2) == "100 INFO") {
            if (command.section(" ", 5, 5) == "OFF") {
                SendCommandToSRCPServer(QString("SET %1 POWER ON")
                        .arg(command.section(" ", 3, 3)));
                PowerSwitched = true;
            }
            /* echo "200 OK" is only displayed in history line */
        }
        /* 
         * if power was switched wait for server response and switch
         * next bus at next cycle
         */
        if (!PowerSwitched)
            if (!gbs->sendSRCP08BusMessage(SrcpMessage::msgPowerGet)) {
                // all busses are switched on, we are ready 
                LayoutPowerIsOn = true;
                updateLayoutPowerAction();
                SRCPCommandState = srcpConnected;
                if (cmdAutoSendAll)
                    layoutSendAll();
            }
    }
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
 *  SRCP 0.7 mode, incomming FB info for SRCP 0.8 see processInfoMessage()
 */
void MainWindow::processFeedbackMessage(const QString& info)
{
    unsigned int fbcontact, fbbus, fbport, fbstate;

    // if we got error code, break loop
    if (info.contains("-", 0)) {
        infoMessage(info);
        return;
    }

    /*
     * Qstring sections:
     * INFO FB <module_type> <portnr> <state>
     *  0   1       2           3        4
     */

    fbport = info.section(" ", 3, 3).toUInt();
    fbstate = info.section(" ", 4, 4).toUInt();

    fbcontact = (fbport - 1) % 496 + 1;
    fbbus = (fbport - 1) / 496 + 1;

    /* switch on port change messages after initialization */
    if (isFBInitMode) {
        if (fbport == MAX_FB)
            isFBInitMode = false;
    }
    else
        infoMessage(info);

    /* should'nt we only send modules which are realy connected? */
    // send feedback updates to:
    // 1. module window
    // 2. all elements via gbs
    // 3. all routes
    emit sendFBChangeModule(fbbus, fbcontact, fbstate);
    emit sendFBChangeLayout(fbbus, fbcontact, fbstate == 1);
    emit sendFBChangeRoute(fbbus, fbcontact, fbstate == 1);
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


/*
 * react to connect/disconnect changes
 *
 * in SRCP 0.7 mode connection is established to second port
 * e.g.: 4303 + 2
 * in SRCP 0.8 mode connection is established to same port as
 * command channel, but other login type
 */
void MainWindow::updateCommandConnectionState(bool connected)
{
    // CommunicationStyle{csNone, csOld, csNew, csBoth};
    commandStyle = (SrcpPort::CommunicationStyle)
        commandPort->getCurrentStyle();

    if (connected) {

        if (SrcpPort::csOld == commandStyle) {
            statusMessage(tr("Command port connected with old "
                        "communication style."));
            infoPort->setServer(SrcpPort::csOld,
                    commandPort->getHostname(),
                    commandPort->getPortNumber() + 2);
            infoPort->serverConnect();

            /*
             * ask server about power state, may be an other client
             * already switched power on
             */
            SRCPCommandState = srcp07GetPower;
            SendCommandToSRCPServer("GET POWER");
        }

        else if (SrcpPort::csNew == commandStyle) {
            statusMessage(tr("Command port connected with new "
                        "communication style."));
            infoPort->setServer(SrcpPort::csNew,
                    commandPort->getHostname(),
                    commandPort->getPortNumber());
            infoPort->serverConnect();

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
            if (!gbs->runSRCP08GAInitSequence())
            {
                SRCPCommandState = srcp08InitFBBusses;
                if (!gbs->sendSRCP08BusMessage(SrcpMessage::msgPowerInit))
                {
                    if (cmdAutoPower)
                    {
                        SRCPCommandState = srcp08GetBusPower;
                        if (!gbs->sendSRCP08BusMessage(
                                    SrcpMessage::msgPowerGet))
                        {
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
        else {
            statusMessage(tr("Error: Command port connected with "
                        "unknown style '%1'").arg(commandStyle));
        }
        // layout area is set modified to make changes of GA directions
        // saveable
        gbs->setModified(true);
    }
    else {
        SRCPCommandState = srcpUndefined;
    }
    updateDaemonMenu();
}


/*
 * react to connect/disconnect changes
 */
void MainWindow::updateInfoConnectionState(bool connected)
{
    // CommunicationStyle{csNone, csOld, csNew, csBoth};
    infoStyle = (SrcpPort::CommunicationStyle) infoPort->getCurrentStyle();

    if (connected) {
        SRCPInfoState = srcp08RunInfoMode;

        //connect feedback port depending on command session style
        if (SrcpPort::csOld == infoStyle) {
            statusMessage(tr("Info port connected with old "
                        "communication style."));

            feedbackPort->setServer(SrcpPort::csOld,
                    commandPort->getHostname(),
                    commandPort->getPortNumber() + 1);
            feedbackPort->serverConnect();
        }
        else if (SrcpPort::csNew == infoStyle)
            statusMessage(tr("Info port connected with new "
                        "communication style."));
        else
            statusMessage(tr("Error: Info port connected with "
                        "unknown style '%1'").arg(infoStyle));
    }
    else {
        SRCPInfoState = srcpUndefined;
    }
}


/*
 * react to connect/disconnect changes
 */
void MainWindow::updateFeedbackConnectionState(bool connected)
{
    if (connected) {

        // flag to avoid history line flooding by startup feedback
        isFBInitMode = true;

        SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgFbInit);
        if (sm == NULL)
            return;

        sm->setFbData(0, (SrcpMessage::Feedback) pref.fbmoduletype, 0);
        sendSrcpMessage(sm);
        delete sm;

        statusMessage(tr("Feedback port changes should be avoided during "
                    "initialization"));
    }
    else {
    }
}


void MainWindow::processInfoMessage(const QString& info)
{
    /* reaktions to SRCP 0.7 commands */
    if (SrcpPort::csOld == infoStyle) {

        infoMessage(info);
        QString device = info.section(" ", 1, 1);
        /*
         * check for incomming GA actions and send them to gbs
         * INFO GA <protocol> <addr> <port> <state>
         *   0   1     2        3      4       5
         */
        if ("GA" == device && info.section(" ", 5, 5).toUInt() == 1) {
            gbs->sendInfoPortMessage(1,
                    info.section(" ", 3, 3).toUInt(),
                    info.section(" ", 4, 4).toUInt(),
                    info.section(" ", 5, 5).toUInt());
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

            fbport = info.section(" ", 3, 3).toUInt();
            fbstate = info.section(" ", 4, 4).toUInt();

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
    else if (SrcpPort::csNew == infoStyle) {
        if (pref.converttime) {
            QString msgstr = ConvertMessageTime(info);
            infoMessage(msgstr);
        }
        else
            infoMessage(info);


        /* respond to incomming info messages */
        if (SRCPInfoState == srcp08RunInfoMode) {
            QString devGroup = info.section(" ", 4, 4);

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
                fbbus = info.section(" ", 3, 3).toUInt();
                fbcontact = info.section(" ", 5, 5).toUInt();
                fbstate  = info.section(" ", 6, 6).toUInt();

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
                if (info.section(" ", 1, 1).toUInt() == 100 &&
                        info.section(" ", 0, 0) != "0.0")
                    // (bus, addr, port, value)
                    gbs->sendInfoPortMessage(
                            info.section(" ", 3, 3).toUInt(),
                            info.section(" ", 5, 5).toUInt(),
                            info.section(" ", 6, 6).toUInt(),
                            info.section(" ", 7, 7).toUInt());
            }

            /*
             * respond to incomming power messages
             *
             * <time> 100 INFO <bus> POWER <on/off>
             *   0     1   2     3     4      5     : Qstring sections
             */
            else if (devGroup == "POWER") {
                if (info.section(" ", 1, 1).toUInt() == 100) {
                    unsigned int bus = info.section(" ", 3, 3).toUInt();
                    bool poweron = info.section(" ", 5, 5) == "ON";

                    //check if bus is relevant for this layout
                    if (gbs->hasSrcp08GaBus(bus)) 
                        if (poweron != LayoutPowerIsOn) {
                            LayoutPowerIsOn = poweron;
                            updateLayoutPowerAction();
                        }
                }
            }
            /*
             * respond to incomming power messages
             *
             * <time> 100 INFO <bus> DESCRIPTION <device list>
             *   0     1   2     3     4      5     : Qstring sections
             */
            else if (devGroup == "DESCRIPTION") {
                QStringList tokens = QStringList::split(" ", info);
                if ((tokens[1].toUInt() == 100) &&
                        (tokens[3].toUInt() == 0)){

                    QString gm = "GM";
                    int pos = tokens.findIndex(gm);
                    router->setServerHasGm(pos != -1);
                }
            }
            /**
             * add other device groups here (SESSION, SERVER, TIME, GL,
             * LOCK, DESCRIPTION)
             * <time> 100 INFO <bus=0> GM <generic message>
             * <time> 100 INFO <bus=0> MACRO <macro message>
             **/
        }
    }
    else {
        //TODO: error message wrong connection style
    }
}


void MainWindow::ConnectToSRCPServer()
{
    ConnectCommandPort();
}

/*
 * connection to all server ports is done in a cascade, first try is
 * command port
 * also preset Generic Massage capabilities of server to "no"
 */
void MainWindow::ConnectCommandPort()
{
    router->setServerHasGm(false);
    commandPort->serverConnect();
}


void MainWindow::CloseSRCPServerConnection()
{
    /* 1. Command socket */
    if (commandPort->hasServerConnection()) {
        
        if (SrcpPort::csOld == commandStyle)
            SendCommandToSRCPServer("LOGOUT");
        else if (SrcpPort::csNew == commandStyle)
            SendCommandToSRCPServer("TERM 0 SESSION");

        commandPort->serverDisconnect();
        commandStyle = SrcpPort::csNone;
        SRCPCommandState = srcpUndefined;
        LayoutPowerIsOn = false;
    }        

    /* 2. Feedback socket, relevant only in SRCP 0.7 mode */
    if (feedbackPort->hasServerConnection()) {
        feedbackPort->serverDisconnect();
    }

    /* 3. Info socket */
    if (infoPort->hasServerConnection()) {
        infoPort->serverDisconnect();
        infoStyle = SrcpPort::csNone;
        SRCPInfoState = srcpUndefined;
    }
}


void MainWindow::SendCommandToSRCPServer(const QString& cmdstr)
{
    if (commandPort->hasServerConnection()) {
        commandPort->sendToServer(cmdstr);
        commandMessage(cmdstr);
    }
}

/* this is only used in SRCP 0.8 mode */
void MainWindow::SendInfoCommandToSRCPServer(const QString& cmdstr)
{
    if (infoPort->hasServerConnection()) {
        infoPort->sendToServer(cmdstr);
        infoMessage(cmdstr);
    }
}


void MainWindow::sendSrcpMessage(SrcpMessage* sm)
{
    if (sm == NULL)
        return;
    
    if (!commandPort->hasServerConnection())
        return;

    QString cmd = sm->getSrcpMessageStr(commandStyle);
    commandMessage(cmd);
    commandPort->sendToServer(cmd);

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

    if (SrcpPort::csOld == commandStyle) {
        SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgPowerSet);

        if (sm == NULL)
            return;

        sm->setPowerData(0, LayoutPowerIsOn);
        sendSrcpMessage(sm);
        delete sm;

        if (LayoutPowerIsOn && cmdAutoSendAll)
            layoutSendAll();
    }
    else if (SrcpPort::csNew == commandStyle) {
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
                SrcpPort::csOld == commandStyle);
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
    if (SrcpPort::csOld == commandStyle) {
        SendCommandToSRCPServer("SHUTDOWN");
        CloseSRCPServerConnection();
    }
    else if (SrcpPort::csNew == commandStyle) {
        SRCPCommandState = srcp08TermServer;
        SendCommandToSRCPServer("TERM 0 SERVER");
    }

    statusMessage(tr("Daemon has been killed. Restart server to "
             "reconnect " PACKAGE " for Linux"));
}


/*show SRCP server info window*/
void MainWindow::slotDaemonInfo()
{
    ServerInfoDialog* sid = new ServerInfoDialog(commandStyle, this,
            "serverInfoDialog");

    if (sid == NULL)
        return;

    if (SrcpPort::csNew == commandStyle) {
        if (commandPort->hasServerConnection())
            sid->setCommandSessionData(
                    commandPort->getSrcpServer(),
                    commandPort->getSrcpVersion(),
                    commandPort->getSrcpOther(),
                    commandPort->getSessionId(),
                    router->serverHasGm());
        else
            sid->setCommandSessionData(tr("Not connected"), "", "", 0, false); 

        if (infoPort->hasServerConnection())
            sid->setInfoSessionData(
                    infoPort->getSrcpServer(),
                    infoPort->getSrcpVersion(),
                    infoPort->getSrcpOther(),
                    infoPort->getSessionId());
        else
            sid->setInfoSessionData(tr("Not connected"), "", "", 0); 
    }
    else {
        sid->setOldCommandData(
                commandPort->getSrcpServer(),
                commandPort->getSrcpVersion());

        sid->setOldFeedbackConnected(feedbackPort->hasServerConnection());
        sid->setOldInfoConnected(infoPort->hasServerConnection());
    }
    sid->exec();
    delete sid;
}


/*update state of daemon/server menu items*/
void MainWindow::updateDaemonMenu()
{
    bool connected = commandPort->hasServerConnection();

    if (!connected)
        LayoutPowerIsOn = false;
    updateLayoutPowerAction();

    // disable all daemon related menus and toolbuttons if the daemon is
    // not running or has been killed
    actionViewKeyboard->setEnabled(connected);

    actionDaemonConnect->setEnabled(!connected);
    actionDaemonDisconnect->setEnabled(connected);
    actionDaemonReset->setEnabled(connected);
    actionDaemonKill->setEnabled(connected);
    actionDaemonInfo->setEnabled(connected);
    
    actionLayoutPower->setEnabled(connected);

    actionLayoutToggleAll->setEnabled(LayoutPowerIsOn);
    actionLayoutSendAll->setEnabled(LayoutPowerIsOn);
    actionLayoutUpdateFB->setEnabled(LayoutPowerIsOn &&
                commandStyle == SrcpPort::csOld);
}


/*show spdrs60 copyright message window*/
void MainWindow::slotAbout()
{
    /*
       "For more information please have a look at the\n"
       "documentation (see Help menu or press F1).\n\n"
       "Please report ANY bugs, hints and thanks to:\n") +
       PACKAGE_BUGREPORT);
     */
    AboutDialog* ad = new AboutDialog(this);
    if (ad == NULL)
        return;

    ad->exec();
    delete ad;
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
    QProcess* proc = new QProcess(this, "browserProcess");
    proc->addArgument(pref.editor);
    proc->addArgument(fileName);

    if (!proc->start()) {
        statusMessage(tr("Error starting editor '%1'!").arg(pref.editor));
    }
}


/*open config file with external editor*/
void MainWindow::slotEditConfigFile()
{
    QProcess* proc = new QProcess(this, "browserProcess");
    proc->addArgument(pref.editor);
    proc->addArgument(QDir::homeDirPath() + "/" SPDRS60_INIT);

    if (!proc->start()) {
        statusMessage(tr("Error starting editor '%1'!").arg(pref.editor));
    }
}


/*switch edit modes of layout area*/
void MainWindow::slotViewSwitchMode(QAction* ac)
{
    bool rtvIsVisible = rtViewer->isVisible();

    if (ac == actionViewNormalMode) {
            visualMode = kvmNormal;
            updateRouteMenu(rtvIsVisible);
            statusMessage(tr("Layout in normal view mode"));
    }
    else if (ac == actionViewLayoutEditMode) {
            visualMode = kvmEditLayout;
            // when layout was in edit mode, it is
            // assumed to be modified
            gbs->setModified(true);
            rtViewer->hide();
            updateRouteMenu(false);
            statusMessage(tr("Entering layout edit mode"));
    }
    else if (ac == actionViewRouteEditMode) {
            visualMode = kvmEditRoute;
            rtViewer->show();
            updateRouteMenu(true);
            statusMessage(tr("Entering route edit mode"));
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
        actionRouteActivate->setEnabled(false);
        actionRouteWithdraw->setEnabled(false);
        actionRouteRelease->setEnabled(false);
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
                actionRouteActivate->setEnabled(false);
                actionRouteWithdraw->setEnabled(false);
                actionRouteRelease->setEnabled(false);
                break;
            case 0:
                actionRouteActivate->setEnabled(true);
                actionRouteWithdraw->setEnabled(false);
                actionRouteRelease->setEnabled(false);
                break;
            default:
                actionRouteActivate->setEnabled(false);
                actionRouteWithdraw->setEnabled(true);
                actionRouteRelease->setEnabled(true);
                break;
        }
    }
    else {
        actionRouteActivate->setEnabled(false);
        actionRouteWithdraw->setEnabled(false);
        actionRouteRelease->setEnabled(false);
    }
}


/* update menu items if route list gets empty*/
void MainWindow::updateRouteListMenuItems()
{
    updateRouteMenu(rtViewer->isVisible());
}

/*
 * Open browser window with URL
 */
void MainWindow::runBrowserUrl(const QString& url)
{
    QProcess* proc = new QProcess(this, "browserProcess");
    proc->addArgument(pref.browser);
    proc->addArgument(url);

    if (!proc->start()) {
        statusMessage(tr("Error starting browser '%1'!").arg(pref.browser));
    }
}

/*
 * Open handbook in external browser
 */
void MainWindow::slotAboutHelp()
{
    QString langenv, sURL;

    langenv = QTextCodec::locale();
    if (langenv.startsWith("de"))
        langenv = "de";
    else
        langenv = "en";

    sURL = QString("%1/%2/index.html")
        .arg(HTML_DOC_DIR)
        .arg(langenv);

    runBrowserUrl(sURL);
}

/*
 * Open spdrs60 web resources with external browser
 */
void MainWindow::slotAboutWeb()
{    
    runBrowserUrl("http://spdrs60.sourceforge.net/");
}

/*
 * show an original DB clock with minute delay
 */
void MainWindow::slotShowClock()
{
    QProcess* proc = new QProcess(this, "clockProcess");
    proc->addArgument("centralclock");

    if (!proc->start()) {
        statusMessage(tr("Error starting clock!"));
    }
}                               


/* 
 * show feedback module window
 */
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
        keybWindow = new keyboard(commandStyle, pref.keyboardprot,
                this, "Keyboard");
        connect(keybWindow, SIGNAL(sendSrcpMessage(SrcpMessage*)),
                this, SLOT(sendSrcpMessage(SrcpMessage*)));
        connect(keybWindow, SIGNAL(protocolSelected(int)),
                this, SLOT(saveKeyboardProtocol(int)));

        keybWindow->move(QCursor::pos());
        keybWindow->show();
    }
}

/* show train number input dialog */
void MainWindow::slotViewTrainNumberDialog()
{
    if (trainnumberdialog != NULL) {
        if (trainnumberdialog->isVisible())
            trainnumberdialog->hide();
        else {
            trainnumberdialog->show();
            trainnumberdialog->setActiveWindow();
            trainnumberdialog->raise();
        }
    }
    else {
        trainnumberdialog = new TrainNumberDialog(this, "trainnumberdialog");
        connect(trainnumberdialog,
                SIGNAL(trainNumberChanged(unsigned int, unsigned int)),
                router, SLOT(changeTrainNumber(unsigned int, unsigned int)));
        trainnumberdialog->show();
    }
}

/*
 * save keyboard protocol settings for next time usage
 */
void MainWindow::saveKeyboardProtocol(int protocol)
{
    pref.keyboardprot = protocol;
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
        bool et = pref.datatooltips;
        
        optDlg->getPreferences(pref);
        writeConfigFile();
        
        fbViewer->updateBusAndModuleStructure();
        messageHistory->enableTime(pref.showtime);

        if ((al != pref.addresslabeling) || (et != pref.datatooltips))
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
    if (SrcpPort::csOld == commandStyle) {
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
    nlDlg->setHost(commandPort->getHostname());
    nlDlg->setPort(commandPort->getPortNumber());
    nlDlg->setAutoLogin(cmdAutoLogin);
    nlDlg->setAutoPower(cmdAutoPower);
    nlDlg->setAutoSendAll(cmdAutoSendAll);
    
    if (nlDlg->exec() == QDialog::Accepted) {
        int iNewCols = nlDlg->getColumns();
        int iNewRows = nlDlg->getRows();
        gbs->setLayoutSize(iNewCols, iNewRows);

        commandPort->setServer(nlDlg->getHost(), nlDlg->getPort());
        infoPort->setServer(nlDlg->getHost(), nlDlg->getPort());
        feedbackPort->setServer(nlDlg->getHost(), nlDlg->getPort());

        cmdAutoLogin = nlDlg->getAutoLogin();
        cmdAutoPower = nlDlg->getAutoPower();
        cmdAutoSendAll = nlDlg->getAutoSendAll();

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
    return (gbs->isModified() || router->isModified());
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
