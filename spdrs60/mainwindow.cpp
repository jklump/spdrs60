/***************************************************************************
                           mainwindow.cpp
                           version 0.4.8 $Revision: 1.40 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-12-01 20:37:04 $
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

#include <stdio.h>              // for perror(), sprintf()
#include <stdlib.h>             // for system()
#include <qhbox.h>
#include <qmenubar.h>
#include <qvbox.h>

#include "gbsscrollview.h"
#include "mainwindow.h"

#include "../icons/spdrs60_32.xpm"
/*toolbar icons*/
#include "pixmaps/filenew.xpm"
#include "pixmaps/fileopen.xpm"
#include "pixmaps/filesave.xpm"
#include "pixmaps/filesaveas.xpm"
#include "pixmaps/fileimport.xpm"
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

#define GF_OLDGBSEXT     ".dat.gbs"
#define GF_OLDRTSEXT     ".dat.rts"
/*for srcpCom*/
#define GF_CMDHOST       "cmdhost"
#define GF_FBHOST        "fbhost"
#define GF_FORMATVERSION "formatversion"
#define GF_FV            "1"

extern bool bFBport[MAX_FB];

extern bool SHOW_HP2;           // all global vars are used in this
extern bool SHOW_TOOLTIPS;      // class cause they are read from
extern bool SHOW_DATA_TOOLTIPS; // SpDrS60 config file on startup
extern bool LOAD_DEF_LAYOUT;
extern bool INIT_SIGNALS;
extern bool SHOW_TXT_ADR;
extern bool LOGGING;
extern bool AUTO_ZP9;
extern bool AUTO_TT_DIR;
extern bool SERVERLOGIN;
extern int SERVER;

extern int DEF_COLS;
extern int DEF_PROTOCOL;
extern int ACTIVE_TIME;
extern int ROUTING_TIME;
extern int FEEDBACK;
extern int FB_MODULES_[4];
extern int PORT;
extern double TT_ROUND_TIME;

extern QString DEF_LAYOUT;
extern QString EDITOR;
extern QString BROWSER;
extern QString DEF_DECODER;
extern QString HOST;
extern QString COMX;
extern QString BAUD;
extern QString DATAB;
extern QString STOPB;
extern QString PARI;



MainWindow::MainWindow()
: QMainWindow(0, "SpDrS60", WDestructiveClose | WGroupLeader)
{
    setIcon(QPixmap(spdrs60_32));
    /*Networking */
    cmdHost = "localhost";
    fbHost = "localhost";
    cmdPort = 12345;
    fbPort = 12346;
    cmdLogin = false;
    fbLogin = false;
    CommandPortIsConnected = false;
    FeedbackPortIsConnected = false;
    InfoPortIsConnected = false;
    SRCPCommandStatus = srcpUndefined;
    LayoutPowerIsOn = false;

    CurrentHL = HL_CMND;            // default debug window ist HISTORY
    isFBInitMode = true;        // var to avoid all startup feedback
    visualMode = kvmNormal;         // normal layout mode
    lastDir = QDir::homeDirPath();  // remembers path for FileOpen
    initMainWindow();           // setup main window with all menus
    slotReadConfigFile();       // read user dependend config file

    if (SERVER == SRCP) {
        initAllSockets();       // init connection to daemon ...
        if (SERVERLOGIN)
            ConnectToSRCPServer();
    }

    /* autostart voltage on layout only when these conditions are true?
     * 1) option set in preferences
     * 2) connection to server established
     * 3) layout is loaded
     */
    cmdToDebug(tr("Program succesfully started!"), MT_INFO, HL_CMND);
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
 * write application settings to personal config file, this is typicaly
 * done if application window is closed
 */
void MainWindow::writeConfigFile()
{
    QFile file(QDir::homeDirPath() + "/" + SPDRS60_INIT);
    
    if (!file.open(IO_WriteOnly)) {
        cmdToDebug(tr("Error: Could not save configuration"
                    " file: ~/%1").arg(SPDRS60_INIT), MT_INFO, HL_CMND);
        return;
    }
    cmdToDebug(tr("Writing SpDrS60 configuration"
                " file: ~/%1").arg(SPDRS60_INIT), MT_INFO, HL_CMND);

    QDateTime dt = QDateTime::currentDateTime();
    QTextStream ts(&file);

    ts  << "# SpDrS60 for Linux config file" << endl
        << "# last modified: " << dt.toString(Qt::ISODate) << endl
        << "#" << endl;
    // TODO: continue work
    file.close();
}

/**
 * read application settings from config file, this is typicaly
 * done on application startup
 */
void MainWindow::slotReadConfigFile()
{
    int i;

    QFile file(QDir::homeDirPath() + "/" + SPDRS60_INIT);
    if (!file.open(IO_ReadOnly)) {
        /* if no configuration file is found, just keep defaults */
        cmdToDebug(tr("Personal config file not found") + ": ~/" +
                   SPDRS60_INIT, MT_INFO, HL_CMND);
        return;
    }
    QTextStream ts(&file);
    QString s, key;

    // layout section
    // first omit four section description lines
    for (i = 0; i < 4; i++)
        s = ts.readLine();

    /*
     * read "lastDir" value, was first defined in version 0.4.5, older
     * versions show "#"
     */
    s = ts.readLine();
    if (!s.startsWith("#")){
        key = s.section("=", 0, 0);
        if (key.compare("lastdir") == 0)
            lastDir = s.section("=", 1, 1);
    }

    SHOW_HP2 = (ts.readLine().remove(0, 16) == "1");
    SHOW_TOOLTIPS = (ts.readLine().remove(0, 16) == "1");
    SHOW_DATA_TOOLTIPS = (ts.readLine().remove(0, 16) == "1");
    SHOW_TXT_ADR = (ts.readLine().remove(0, 16) == "text");
    INIT_SIGNALS = (ts.readLine().remove(0, 16) == "red");
    DEF_COLS = ts.readLine().remove(0, 16).toInt();
    LOAD_DEF_LAYOUT = (ts.readLine().remove(0, 16) == "1");
    DEF_LAYOUT = ts.readLine().remove(0, 16);
    EDITOR = ts.readLine().remove(0, 16);
    BROWSER = ts.readLine().remove(0, 16);

    // data section
    for (i = 0; i < 3; i++)     // omit three section description lines
        s = ts.readLine();

    DEF_PROTOCOL = (ts.readLine().remove(0, 16) == "Motorola");
    DEF_DECODER = ts.readLine().remove(0, 16);
    ACTIVE_TIME = ts.readLine().remove(0, 16).toInt();
    AUTO_TT_DIR = ts.readLine().remove(0, 16).toInt();
    TT_ROUND_TIME = ts.readLine().remove(0, 16).toDouble();
    AUTO_ZP9 = (ts.readLine().remove(0, 16) == "1");
    ROUTING_TIME = ts.readLine().remove(0, 16).toInt();
    FEEDBACK = ts.readLine().remove(0, 16) == "S88_16" ? FB_16 : FB_8;

    for (i = 0; i < 4; i++)
        FB_MODULES_[i] = ts.readLine().remove(0, 16).toInt();

    // interface section
    for (i = 0; i < 3; i++)     // omit three section description lines
        s = ts.readLine();

    s = ts.readLine().remove(0, 16);
    if (s == "SRCP")
        SERVER = SRCP;
    else if (s == "EditsPro")
        SERVER = EDITS;
    else if (s == "Märklin-6050/6051")
        SERVER = MAERKIF;
    else if (s == "Intellibox")
        SERVER = INTELLI;

    HOST = ts.readLine().remove(0, 16);
    PORT = ts.readLine().remove(0, 16).toInt();
    COMX = ts.readLine().remove(0, 16);
    BAUD = ts.readLine().remove(0, 16);
    DATAB = ts.readLine().remove(0, 16);
    STOPB = ts.readLine().remove(0, 16);
    PARI = ts.readLine().remove(0, 16);
    /* old config files have here a closing "----..." */
    SERVERLOGIN = ts.readLine().remove(0, 16).toInt();

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

    connect(this, SIGNAL(sendFBChangeLayout(unsigned int, unsigned int,
                    bool)),
            gbs, SIGNAL(feedbackPortChanged(unsigned int, unsigned int,
                    bool)));
    connect(this, SIGNAL(switchedVisualMode(elemVisualMode)),
            gbs, SIGNAL(switchVisualMode(elemVisualMode)));
    connect(gbs, SIGNAL(showLogMessage(const QString&, int, int)),
            this, SLOT(cmdToDebug(const QString&, int, int)));
    connect(gbs, SIGNAL(sendCommand(const QString&)),
            this, SLOT(SendCommandToSRCPServer(const QString&)));
    connect(gbs, SIGNAL(sigShowFBmodules()),
            this, SLOT(slotShowModules()));


    /*history line*/
    // this container should be a separate class:
    QHBox *hBox = new QHBox(vBox, "hbox", 0);
    hBox->setSpacing(2);

    lblStack = new QWidgetStack(hBox, "cbstack");
    lblStack->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
                QSizePolicy::Fixed, false));

    QLabel *HistLabel = new QLabel(lblStack, "Cmd");
    HistLabel->setText(tr("Commands"));
    HistLabel->setIndent(3);
    lblStack->addWidget(HistLabel, 0);

    QLabel *InfoLabel = new QLabel(lblStack, "Info");
    InfoLabel->setText(tr("Infoport"));
    InfoLabel->setIndent(3);
    lblStack->addWidget(InfoLabel, 1);

    QLabel *FeedBackLabel = new QLabel(lblStack, "Feedb");
    FeedBackLabel->setText(tr("Feedbacks"));
    FeedBackLabel->setIndent(3);
    lblStack->addWidget(FeedBackLabel, 2);

    cbStack = new QWidgetStack(hBox, "cbstack");
    cbStack->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                           QSizePolicy::Fixed, false));

    QFont f;
    f.setFamily("Courier");

    HistCB = new QComboBox(false, cbStack);
    HistCB->setSizeLimit(15);
    HistCB->setFont(f);
    cbStack->addWidget(HistCB, 0);

    InfoCB = new QComboBox(false, cbStack);
    InfoCB->setSizeLimit(15);
    InfoCB->setFont(f);
    cbStack->addWidget(InfoCB, 1);

    FeedBackCB = new QComboBox(false, cbStack);
    FeedBackCB->setSizeLimit(15);
    FeedBackCB->setFont(f);
    cbStack->addWidget(FeedBackCB, 2);
#if QT_VERSION < 0x030200
    // force painting of history lines and their labels
    cbStack->raiseWidget(0);
    lblStack->raiseWidget(0);
#endif

    setCentralWidget(vBox);

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
    
    /*route viewer*/
    rtViewer = new RoutingViewer(this, "Routings", rtController);
    Q_CHECK_PTR(rtViewer);
    rtViewer->setFixedExtentWidth(360);
    moveDockWindow(rtViewer, Right);
    rtViewer->hide();
    connect(this, SIGNAL(switchedVisualMode(elemVisualMode)),
            rtViewer, SLOT(switchVisualMode(elemVisualMode)));
    connect(rtViewer, SIGNAL(visibilityChanged(bool)),
            this, SLOT(updateRouteMenu(bool)));
    connect(rtViewer, SIGNAL(selectedRouteIsLocked(bool)),
            this, SLOT(updateRouteMenuActivateItems(bool)));
    connect(rtViewer, SIGNAL(showLogMessage(const QString&, int, int)),
            this, SLOT(cmdToDebug(const QString&, int, int)));
    connect(rtController, SIGNAL(updateRoutingViewer()),
            rtViewer, SLOT(updateRoutes()));
    connect(rtController, SIGNAL(updateRoutingViewerAt(int)),
            rtViewer, SLOT(updateRouteAt(int)));
    connect(rtController, SIGNAL(routeStateChanged(int, bool)),
            rtViewer, SLOT(updateRouteStateAt(int, bool)));
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
            CTRL+Key_N, this, "fileNew" );
    actionFileNew->setToolTip(tr("Create empty layout"));
#else
    actionFileNew = new QAction(tr("Create empty layout"),
		    QPixmap(filenew_xpm), tr("&New..."), CTRL+Key_N,
                    this, "fileNew" );
#endif
    connect(actionFileNew, SIGNAL(activated()), this,
            SLOT(slotFileNew()));
    actionFileNew->addTo(filemenu);
    actionFileNew->addTo(filetb);

#if QT_VERSION >= 0x030200
    actionFileOpen = new QAction(QPixmap(fileopen_xpm), tr("&Open..."),
            CTRL+Key_O, this, "fileOpen" );
    actionFileOpen->setToolTip(tr("Open layout file"));
#else
    actionFileOpen = new QAction(tr("Open layout file"), 
            QPixmap(fileopen_xpm), tr("&Open..."), CTRL+Key_O, this,
            "fileOpen" );
#endif
    connect(actionFileOpen, SIGNAL(activated()), this,
            SLOT(slotFileOpen()));
    actionFileOpen->addTo(filemenu);
    actionFileOpen->addTo(filetb);

#if QT_VERSION >= 0x030200
    actionFileSave = new QAction(QPixmap(filesave_xpm), tr("&Save"),
            CTRL+Key_S, this, "fileSave" );
#else
    actionFileSave = new QAction("", QPixmap(filesave_xpm), tr("&Save"),
            CTRL+Key_S, this, "fileSave" );
#endif
    connect(actionFileSave, SIGNAL(activated()), this,
            SLOT(slotFileSave()));
    actionFileSave->addTo(filemenu);
    actionFileSave->addTo(filetb);

#if QT_VERSION >= 0x030200
    actionFileSaveAs = new QAction(QPixmap(filesaveas_xpm), tr("Save &as..."),
            CTRL+Key_A, this, "fileSaveAs" );
#else
    actionFileSaveAs = new QAction("", QPixmap(filesaveas_xpm), tr("Save &as..."),
            CTRL+Key_A, this, "fileSaveAs" );
#endif
    connect(actionFileSaveAs, SIGNAL(activated()), this,
            SLOT(slotFileSaveAs()));
    actionFileSaveAs->addTo(filemenu);
    //actionFileSaveAs->addTo(filetb);

#if QT_VERSION >= 0x030200
    actionFileImport = new QAction(QPixmap(fileimport_xpm), tr("&Import..."),
            CTRL+Key_I, this, "fileImport" );
#else
    actionFileImport = new QAction("", QPixmap(fileimport_xpm), tr("&Import..."),
            CTRL+Key_I, this, "fileImport" );
#endif
    connect(actionFileImport, SIGNAL(activated()), this,
            SLOT(slotFileImport()));
    actionFileImport->addTo(filemenu);
    //actionFileImport->addTo(filetb);

    filemenu->insertSeparator();
    //filetb->addSeparator();

#if QT_VERSION >= 0x030200
    actionFileNewWindow = new QAction(QPixmap(filenewwindow_xpm),
            tr("New &window"), 0, this, "fileNewWindow" );
#else
    actionFileNewWindow = new QAction("", QPixmap(filenewwindow_xpm),
            tr("New &window"), 0, this, "fileNewWindow" );
#endif
    connect(actionFileNewWindow, SIGNAL(activated()), this,
            SLOT(slotFileNewWin()));
    actionFileNewWindow->addTo(filemenu);
    //actionFileNewWindow->addTo(filetb);

#if QT_VERSION >= 0x030200
    actionFileClose = new QAction(QPixmap(fileclose_xpm), tr("&Close"),
            CTRL+Key_W, this, "fileClose" );
#else
    actionFileClose = new QAction("", QPixmap(fileclose_xpm), tr("&Close"),
            CTRL+Key_W, this, "fileClose" );
#endif
    connect(actionFileClose, SIGNAL(activated()), this,
            SLOT(close()));
    actionFileClose->addTo(filemenu);
    //actionFileClose->addTo(filetb);

#if QT_VERSION >= 0x030200
    actionFileQuit = new QAction(QPixmap(filequit_xpm), tr("&Quit"),
            CTRL+Key_Q, this, "fileQuit" );
#else
    actionFileQuit = new QAction("", QPixmap(filequit_xpm), tr("&Quit"),
            CTRL+Key_Q, this, "fileQuit" );
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
            "editFileLayout" );
    actionEditFileLayout->setToolTip(tr(
                "Edit layout file with external editor"));
#else
    actionEditFileLayout = new QAction(tr("Edit layout file with"
                " external editor"), tr("&Layout"), 0, this,
            "editFileLayout" );
#endif
    connect(actionEditFileLayout, SIGNAL(activated()), this,
            SLOT(slotEditGBSFiles()));
    actionEditFileLayout->addTo(editfilemenu);
    //actionEditFileLayout->addTo(edittb);

#if QT_VERSION >= 0x030200
    actionEditFileOptions = new QAction(NULL,
            QDir::homeDirPath() + "/" + SPDRS60_INIT, 0, this,
            "editFileOptions" );
    actionEditFileOptions->setToolTip(tr(
                "Edit config file with external editor"));
#else
    actionEditFileOptions = new QAction(tr("Edit config file with external editor"),
            QDir::homeDirPath() + "/" + SPDRS60_INIT, 0, this,
            "editFileOptions" );
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
            CTRL+Key_X, this, "editCut" );
    actionEditCut->setToolTip(tr("Cut selection to clipboard"));
#else
    actionEditCut = new QAction(tr("Cut selection to clipboard"),
            QPixmap(editcut_xpm), tr("Cu&t"),
            CTRL+Key_X, this, "editCut" );
#endif
    connect(actionEditCut, SIGNAL(activated()), this,
            SLOT(slotEditCut()));
    actionEditCut->addTo(editmenu);
    actionEditCut->addTo(edittb);
    actionEditCut->setEnabled(false);

#if QT_VERSION >= 0x030200
    actionEditCopy = new QAction(QPixmap(editcopy_xpm), tr("&Copy"),
            CTRL+Key_C, this, "editCopy" );
    actionEditCopy->setToolTip(tr("Copy selection to clipboard"));
#else
    actionEditCopy = new QAction(tr("Copy selection to clipboard"),
            QPixmap(editcopy_xpm), tr("&Copy"),
            CTRL+Key_C, this, "editCopy" );
#endif
    connect(actionEditCopy, SIGNAL(activated()), this,
            SLOT(slotEditCopy()));
    actionEditCopy->addTo(editmenu);
    actionEditCopy->addTo(edittb);
    actionEditCopy->setEnabled(false);

#if QT_VERSION >= 0x030200
    actionEditPaste = new QAction(QPixmap(editpaste_xpm), tr("&Paste"),
            CTRL+Key_V, this, "editPaste" );
    actionEditPaste->setToolTip(tr("Paste from clipboard"));
#else
    actionEditPaste = new QAction(tr("Paste from clipboard"),
            QPixmap(editpaste_xpm), tr("&Paste"),
            CTRL+Key_V, this, "editPaste" );
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
            CTRL+Key_F, this, "editFind" );
    actionEditFind->setToolTip(tr("Find information in layout element"));
#else
    actionEditFind = new QAction(tr("Find information in layout element"),
            QPixmap(editfind_xpm), tr("&Find..."),
            CTRL+Key_F, this, "editFind" );
#endif
    connect(actionEditFind, SIGNAL(activated()), this,
            SLOT(slotEditFind()));
    actionEditFind->addTo(editmenu);
    actionEditFind->addTo(edittb);
    //actionEditFind->setEnabled(false);

#if QT_VERSION >= 0x030200
    actionEditOptions = new QAction(QPixmap(editoptions_xpm),
            tr("Pr&eferences..."), CTRL+Key_P, this, "editPreferences" );
    actionEditOptions->setToolTip(tr("Edit application preferences"));
#else
    actionEditOptions = new QAction("", QPixmap(editoptions_xpm),
            tr("Pr&eferences..."), CTRL+Key_P, this, "editPreferences" );
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
    actionViewRoutes = new QAction(QPixmap(viewroute_xpm),
            tr("Routing &table"), CTRL + Key_R, this, "viewRoutes" );
    actionViewRoutes->setToolTip(tr("Show routing table"));
#else
    actionViewRoutes = new QAction(tr("Show routing table"),
            QPixmap(viewroute_xpm),
            tr("Routing &table"), CTRL + Key_R, this, "viewRoutes" );
#endif
    connect(actionViewRoutes, SIGNAL(activated()), this,
            SLOT(slotShowRoutes()));
    actionViewRoutes->addTo(viewmenu);
    actionViewRoutes->addTo(viewtb);

#if QT_VERSION >= 0x030200
    actionViewFBModules = new QAction(QPixmap(viewfeedback_xpm),
            tr("&Feedback modules"), CTRL + Key_M, this, "viewFBModules" );
    actionViewFBModules->setToolTip(tr("Show feedback module window"));
#else
    actionViewFBModules = new QAction(tr("Show feedback module window"),
            QPixmap(viewfeedback_xpm),
            tr("&Feedback modules"), CTRL + Key_M, this, "viewFBModules" );
#endif
    connect(actionViewFBModules, SIGNAL(activated()), this,
            SLOT(slotShowModules()));
    actionViewFBModules->addTo(viewmenu);
    actionViewFBModules->addTo(viewtb);
    actionViewFBModules->setEnabled(false);

#if QT_VERSION >= 0x030200
    actionViewClock = new QAction(QPixmap(viewclock_xpm),
            tr("&Central clock"), 0, this, "viewClock" );
    actionViewClock->setToolTip(tr("Show central clock"));
#else
    actionViewClock = new QAction(tr("Show central clock"),
            QPixmap(viewclock_xpm),
            tr("&Central clock"), 0, this, "viewClock" );
#endif
    connect(actionViewClock, SIGNAL(activated()), this,
            SLOT(slotShowClock()));
    actionViewClock->addTo(viewmenu);
    actionViewClock->addTo(viewtb);

#if QT_VERSION >= 0x030200
    actionViewKeyboard = new QAction(QPixmap(viewkeyboard_xpm),
            tr("&Keyboard"), CTRL + Key_K, this, "viewKeyboard" );
    actionViewKeyboard->setToolTip(tr("Show basic keyboard"));
#else
    actionViewKeyboard = new QAction(tr("Show basic keyboard"),
            QPixmap(viewkeyboard_xpm),
            tr("&Keyboard"), CTRL + Key_K, this, "viewKeyboard" );
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
            tr("&Normal mode"), CTRL + Key_L, ViewGrp, "normalmode");
#else
    actionViewNormalMode = new QAction("", QPixmap(viewnormalmode_xpm),
            tr("&Normal mode"), CTRL + Key_L, ViewGrp, "normalmode");
#endif
    actionViewNormalMode->setToggleAction(true);
    
#if QT_VERSION >= 0x030200
    actionViewLayoutEditMode = new QAction(QPixmap(viewlayouteditmode_xpm),
            tr("&Layout edit mode"), CTRL + Key_E, ViewGrp, "layouteditmode");
#else
    actionViewLayoutEditMode = new QAction("", QPixmap(viewlayouteditmode_xpm),
            tr("&Layout edit mode"), CTRL + Key_E, ViewGrp, "layouteditmode");
#endif
    actionViewLayoutEditMode->setToggleAction(true);
    
#if QT_VERSION >= 0x030200
    actionViewRouteEditMode = new QAction(QPixmap(viewrouteeditmode_xpm),
            tr("&Route edit mode"), CTRL + Key_B, ViewGrp, "routeeditmode");
#else
    actionViewRouteEditMode = new QAction("", QPixmap(viewrouteeditmode_xpm),
            tr("&Route edit mode"), CTRL + Key_B, ViewGrp, "routeeditmode");
#endif
    actionViewRouteEditMode->setToggleAction(true);
    
    ViewGrp->addTo(viewmenu);
    ViewGrp->addTo(viewtb);

    viewmenu->insertSeparator();

#if QT_VERSION >= 0x030200
    actionViewToggleHistory = new QAction(NULL,
            tr("Toggle &history line"), CTRL + Key_D, // Ctrl H/T
            this, "viewToggleHistory" );
    actionViewToggleHistory->setToolTip(tr("Show basic keyboard"));
#else
    actionViewToggleHistory = new QAction(tr("Show basic keyboard"),
            tr("Toggle &history line"), CTRL + Key_D, // Ctrl H/T
            this, "viewToggleHistory" );
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
            tr("&Connect"), 0, this, "daemonConnect" ); // Ctrl D
    actionDaemonConnect->setToolTip(tr("Connect to SRCP daemon"));
#else
    actionDaemonConnect = new QAction(tr("Connect to SRCP daemon"),
            QPixmap(daemonconnect_xpm),
            tr("&Connect"), 0, this, "daemonConnect" ); // Ctrl D
#endif
    connect(actionDaemonConnect, SIGNAL(activated()), this,
            SLOT(ConnectToSRCPServer()));
    actionDaemonConnect->addTo(daemonmenu);
    actionDaemonConnect->addTo(daemontb);

#if QT_VERSION >= 0x030200
    actionDaemonDisconnect = new QAction(QPixmap(daemondisconnect_xpm),
            tr("&Disconnect"), 0, this, "daemonDisconnect" );
    actionDaemonDisconnect->setToolTip(tr("Disconnect from SRCP daemon"));
#else
    actionDaemonDisconnect = new QAction(tr("Disconnect from SRCP daemon"),
            QPixmap(daemondisconnect_xpm),
            tr("&Disconnect"), 0, this, "daemonDisconnect" );
#endif
    connect(actionDaemonDisconnect, SIGNAL(activated()), this,
            SLOT(CloseSRCPServerConnection()));
    actionDaemonDisconnect->addTo(daemonmenu);
    actionDaemonDisconnect->addTo(daemontb);

    daemonmenu->insertSeparator();
    
#if QT_VERSION >= 0x030200
    actionDaemonReset = new QAction(QPixmap(daemonreset_xpm),
            tr("&Reset"), 0, this, "daemonReset" );
    actionDaemonReset->setToolTip(tr("Reset SRCP daemon"));
#else
    actionDaemonReset = new QAction(tr("Reset SRCP daemon"),
            QPixmap(daemonreset_xpm),
            tr("&Reset"), 0, this, "daemonReset" );
#endif
    connect(actionDaemonReset, SIGNAL(activated()), this,
            SLOT(slotDaemonReset()));
    actionDaemonReset->addTo(daemonmenu);
    //actionDaemonReset->addTo(daemontb);

#if QT_VERSION >= 0x030200
    actionDaemonKill = new QAction(QPixmap(daemonkill_xpm),
            tr("&Kill"), 0, this, "daemonKill" );
    actionDaemonKill->setToolTip(tr("Kill SRCP daemon"));
#else
    actionDaemonKill = new QAction(tr("Kill SRCP daemon"),
            QPixmap(daemonkill_xpm),
            tr("&Kill"), 0, this, "daemonKill" );
#endif
    connect(actionDaemonKill, SIGNAL(activated()), this,
            SLOT(slotDaemonKill()));
    actionDaemonKill->addTo(daemonmenu);
    //actionDaemonKill->addTo(daemontb);

#if QT_VERSION >= 0x030200
    actionDaemonInfo = new QAction(QPixmap(daemoninfo_xpm),
            tr("&Info..."), 0, this, "daemonInfo" );
    actionDaemonInfo->setToolTip(tr("Info about SRCP daemon"));
#else
    actionDaemonInfo = new QAction(tr("Info about SRCP daemon"),
            QPixmap(daemoninfo_xpm),
            tr("&Info..."), 0, this, "daemonInfo" );
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
            tr("Start &power"), Key_F4, this, "layoutPower" );
    actionLayoutPower->setToolTip(tr("Switch layout power on"));
#else
    actionLayoutPower = new QAction(tr("Switch layout power on"),
            QPixmap(layoutstart_xpm),
            tr("Start &power"), Key_F4, this, "layoutPower" );
#endif
    connect(actionLayoutPower, SIGNAL(activated()), this,
            SLOT(slotToggleLayoutPower()));
    actionLayoutPower->addTo(layoutmenu);
    actionLayoutPower->addTo(layouttb);

#if QT_VERSION >= 0x030200
    actionLayoutFht = new QAction(NULL,
            tr("Use &FHT"), Key_F5, this, "layoutFht" );
    actionLayoutFht->setToolTip(tr("Use route help button"));
#else
    actionLayoutFht = new QAction(tr("Use route help button"),
            tr("Use &FHT"), Key_F5, this, "layoutFht" );
#endif
    connect(actionLayoutFht, SIGNAL(activated()), gbs,
            SLOT(slotFHTclicked()));
    actionLayoutFht->addTo(layoutmenu);
    //actionLayoutFht->addTo(layouttb);

#if QT_VERSION >= 0x030200
    actionLayoutWgt = new QAction(NULL,
            tr("Use &WGT"), Key_F6, this, "layoutWgt" );
    actionLayoutWgt->setToolTip(tr("Use turnout group button"));
#else
    actionLayoutWgt = new QAction(tr("Use turnout group button"),
            tr("Use &WGT"), Key_F6, this, "layoutWgt" );
#endif
    connect(actionLayoutWgt, SIGNAL(activated()), gbs,
            SLOT(slotWGTclicked()));
    actionLayoutWgt->addTo(layoutmenu);
    //actionLayoutWgt->addTo(layouttb);

#if QT_VERSION >= 0x030200
    actionLayoutSgt = new QAction(NULL,
            tr("Use &SGT"), Key_F7, this, "layoutSgt" );
    actionLayoutSgt->setToolTip(tr("Use signal group button"));
#else
    actionLayoutSgt = new QAction(tr("Use signal group button"),
            tr("Use &SGT"), Key_F7, this, "layoutSgt" );
#endif
    connect(actionLayoutSgt, SIGNAL(activated()), gbs,
            SLOT(slotSGTclicked()));
    actionLayoutSgt->addTo(layoutmenu);
    //actionLayoutSgt->addTo(layouttb);

#if QT_VERSION >= 0x030200
    actionLayoutUfgt = new QAction(NULL,
            tr("Use &UfGT"), Key_F8, this, "layoutUfgt" );
    actionLayoutUfgt->setToolTip(tr("Use detour group button"));
#else
    actionLayoutUfgt = new QAction(tr("Use detour group button"),
            tr("Use &UfGT"), Key_F8, this, "layoutUfgt" );
#endif
    connect(actionLayoutUfgt, SIGNAL(activated()), gbs,
            SLOT(slotUfGTclicked()));
    actionLayoutUfgt->addTo(layoutmenu);
    //actionLayoutUfgt->addTo(layouttb);

    layoutmenu->insertSeparator();

#if QT_VERSION >= 0x030200
    actionLayoutNotRot = new QAction(QPixmap(layoutnotrot_xpm),
            tr("&Halt signals"), Key_F12, this, "layoutNotRot" );
    actionLayoutNotRot->setToolTip(tr("Switch all signals to halt"));
#else
    actionLayoutNotRot = new QAction(tr("Switch all signals to halt"),
            QPixmap(layoutnotrot_xpm),
            tr("&Halt signals"), Key_F12, this, "layoutNotRot" );
#endif
    connect(actionLayoutNotRot, SIGNAL(activated()), gbs,
            SLOT(slotNotrot()));
    actionLayoutNotRot->addTo(layoutmenu);
    actionLayoutNotRot->addTo(layouttb);

#if QT_VERSION >= 0x030200
    actionLayoutToggleAll = new QAction(NULL,
            tr("&Toggle all"), Key_F10, this, "layoutToggleAll" );
    actionLayoutToggleAll->setToolTip(tr("Toggle all switchable elements"));
#else
    actionLayoutToggleAll = new QAction(tr("Toggle all switchable elements"),
            tr("&Toggle all"), Key_F10, this, "layoutToggleAll" );
#endif
    connect(actionLayoutToggleAll, SIGNAL(activated()), gbs,
            SLOT(slotToggleAll()));
    actionLayoutToggleAll->addTo(layoutmenu);
    //actionLayoutToggleAll->addTo(layouttb);

#if QT_VERSION >= 0x030200
    actionLayoutSendAll = new QAction(NULL,
            tr("Send &all"), Key_F11, this, "layoutSendAll" );
    actionLayoutSendAll->setToolTip(tr("Send current states of all "
                "switchable elements to SRCP server"));
#else
    actionLayoutSendAll = new QAction(tr("Send current states of all "
                                "switchable elements to SRCP server"), 
            tr("Send &all"), Key_F11, this, "layoutSendAll" );
#endif
    connect(actionLayoutSendAll, SIGNAL(activated()), gbs,
            SLOT(slotSendAll()));
    actionLayoutSendAll->addTo(layoutmenu);
    //actionLayoutSendAll->addTo(layouttb);

#if QT_VERSION >= 0x030200
    actionLayoutUpdateFB = new QAction(NULL,
            tr("Up&date feedback states"), 0, this, "layoutUpdateFB" );
    actionLayoutUpdateFB->setToolTip(tr("Get all current feedback "
			    "states from SRCP server"));
#else
    actionLayoutUpdateFB = new QAction(tr("Get all current feedback "
                                            "states from SRCP server"),
            tr("Up&date feedback states"), 0, this, "layoutUpdateFB" );
#endif
    connect(actionLayoutUpdateFB, SIGNAL(activated()), this,
            SLOT(layoutUpdateFB()));
    actionLayoutUpdateFB->addTo(layoutmenu);
    //actionLayoutUpdateFB->addTo(layouttb);

    layoutmenu->insertSeparator();

#if QT_VERSION >= 0x030200
    actionLayoutChangeSize = new QAction(NULL,
            tr("&Change size..."), 0, this, "layoutChangeSize" );
    actionLayoutChangeSize->setToolTip(tr("Change layout size"));
#else
    actionLayoutChangeSize = new QAction(tr("Change layout size"),
            tr("&Change size..."), 0, this, "layoutChangeSize" );
#endif
    connect(actionLayoutChangeSize, SIGNAL(activated()), this,
            SLOT(layoutChangeSize()));
    actionLayoutChangeSize->addTo(layoutmenu);
    //actionLayoutChangeSize->addTo(layouttb);


    /*route toolbar*/
    QToolBar* routetb = new QToolBar(this, "routetb");
    Q_CHECK_PTR(routetb);
    routetb->setLabel(tr("Route operations"));

    /*Route menu*/
    QPopupMenu* routemenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&Route"), routemenu);

#if QT_VERSION >= 0x030200
    actionRouteStart = new QAction(QPixmap(route_start_xpm),
            tr("&Start"), 0, this, "routestart" );
    actionRouteStart->setToolTip(tr("Activate route"));
#else
    actionRouteStart = new QAction(tr("Activate route"),
            QPixmap(route_start_xpm),
            tr("&Start"), 0, this, "routestart" );
#endif
    connect(actionRouteStart, SIGNAL(activated()), rtViewer,
            SLOT(slotRouteStart()));
    actionRouteStart->addTo(routemenu);
    actionRouteStart->addTo(routetb);

#if QT_VERSION >= 0x030200
    actionRouteStop = new QAction(QPixmap(route_stop_xpm), tr("Sto&p"),
            0, this, "routestop" );
    actionRouteStop->setToolTip(tr("Release route"));
#else
    actionRouteStop = new QAction(tr("Release route"),
            QPixmap(route_stop_xpm), tr("Sto&p"),
            0, this, "routestop" );
#endif
    connect(actionRouteStop, SIGNAL(activated()), rtViewer,
            SLOT(slotRouteStop()));
    actionRouteStop->addTo(routemenu);
    actionRouteStop->addTo(routetb);

    routemenu->insertSeparator();
    routetb->addSeparator();

#if QT_VERSION >= 0x030200
    actionRouteAdd = new QAction(QPixmap(route_new_xpm), tr("&Add"),
            0, this, "routeadd" );
    actionRouteAdd->setToolTip(tr("Add new route"));
#else
    actionRouteAdd = new QAction(tr("Add new route"),
            QPixmap(route_new_xpm), tr("&Add"),
            0, this, "routeadd" );
#endif
    connect(actionRouteAdd, SIGNAL(activated()), this,
            SLOT(slotRouteAdd()));
    actionRouteAdd->addTo(routemenu);
    actionRouteAdd->addTo(routetb);

#if QT_VERSION >= 0x030200
    actionRouteEdit = new QAction(QPixmap(route_edit_xpm),
            tr("&Edit..."), 0, this, "routeedit" );
    actionRouteEdit->setToolTip(tr("Edit selected route"));
#else
    actionRouteEdit = new QAction(tr("Edit selected route"),
            QPixmap(route_edit_xpm),
            tr("&Edit..."), 0, this, "routeedit" );
#endif
    connect(actionRouteEdit, SIGNAL(activated()), rtViewer,
            SLOT(slotRouteEdit()));
    actionRouteEdit->addTo(routemenu);
    actionRouteEdit->addTo(routetb);

#if QT_VERSION >= 0x030200
    actionRouteCopy = new QAction(QPixmap(route_copy_xpm),
            tr("Dupli&cate"), 0, this, "routecopy" );
    actionRouteCopy->setToolTip(tr("Duplicate selected route"));
#else
    actionRouteCopy = new QAction(tr("Duplicate selected route"),
            QPixmap(route_copy_xpm),
            tr("Dupli&cate"), 0, this, "routecopy" );
#endif
    connect(actionRouteCopy, SIGNAL(activated()), rtViewer,
            SLOT(slotRouteCopy()));
    actionRouteCopy->addTo(routemenu);
    actionRouteCopy->addTo(routetb);

#if QT_VERSION >= 0x030200
    actionRouteDelete = new QAction(QPixmap(route_clear_xpm),
            tr("&Delete"), 0, this, "routedelete" );
    actionRouteDelete->setToolTip(tr("Delete selected route"));
#else
    actionRouteDelete = new QAction(tr("Delete selected route"),
            QPixmap(route_clear_xpm),
            tr("&Delete"), 0, this, "routedelete" );
#endif
    connect(actionRouteDelete, SIGNAL(activated()), this,
            SLOT(slotRouteDelete()));
    actionRouteDelete->addTo(routemenu);
    actionRouteDelete->addTo(routetb);

    routemenu->insertSeparator();

#if QT_VERSION >= 0x030200
    actionRouteUnlockAll = new QAction(NULL,
            tr("&Unlock all"), CTRL + Key_U, this, "layoutUnlockRoutes" );
    actionRouteUnlockAll->setToolTip(tr("Unlock all routes"));
#else
    actionRouteUnlockAll = new QAction(tr("Unlock all routes"),
            tr("&Unlock all"), CTRL + Key_U, this, "layoutUnlockRoutes" );
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

    helpmenu->insertItem(tr("&Help"), this, SLOT(slotAboutHelp()), Key_F1);
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


void MainWindow::slotViewDebug()
{
    // circular toggle through all three history lines
    CurrentHL += 1;
    if (CurrentHL > HL_FEED)
        CurrentHL = HL_CMND;

    switch (CurrentHL) {
        case (HL_CMND):
            cbStack->raiseWidget(0);
            lblStack->raiseWidget(0);
            break;
        case (HL_INFO):
            cbStack->raiseWidget(1);
            lblStack->raiseWidget(1);
            break;
        case (HL_FEED):
            cbStack->raiseWidget(2);
            lblStack->raiseWidget(2);
            break;
    }
}


void MainWindow::readAutoloadFile()
{
    // if autoload file from config data does
    // not exist ask user to change options

    bool oldFileFormat = false;
    /* check for file extension, compatible to version <= 0.4.7*/
    if (DEF_LAYOUT.findRev(GF_GBSEXT) == -1) {
        oldFileFormat = true;
        DEF_LAYOUT.append(GF_OLDGBSEXT);
    }
    
    if (!QFile::exists(DEF_LAYOUT)) {                           
        qApp->beep();
        int choice = QMessageBox::warning(this, tr("Autoloader failed"),
                         tr("The selected autoload file '%1'\n"
                            "does not exist. Please adjust your"
                            " options.").arg(DEF_LAYOUT),
                         tr("&Now"), tr("&Later"), 0, 0, 0);
        if (choice == 0)
            slotEditOptions();
    }
    else
        if (oldFileFormat)
            importFile(DEF_LAYOUT);
        else
            openFile(DEF_LAYOUT);
}


void MainWindow::closeEvent(QCloseEvent* e)
{

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
    
    /*TODO: check why this may be necessary:*/
    //emit switchEditMode(kvmNormal);
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

    if (nlDlg->exec() != QDialog::Accepted) {
        delete nlDlg;
        return;
    }
    int iNewCols = nlDlg->getColumns();
    int iNewRows = nlDlg->getRows();
    delete nlDlg;
    
    fileName = "";
    gbs->newFile(iNewCols, iNewRows);

    updateCaption();
    updateFileMenuItems();
    cmdToDebug(tr("New layout file created"), MT_INFO, HL_CMND);
}


void MainWindow::updateFileMenuItems()
{
    actionFileSave->setEnabled(isModified());
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

    if (CommandPortIsConnected) {
        actionLayoutToggleAll->setEnabled(LayoutPowerIsOn);
        actionLayoutSendAll->setEnabled(LayoutPowerIsOn);
	actionLayoutUpdateFB->setEnabled(LayoutPowerIsOn);
    }

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
        /*TODO: check this*/
        return true;
    }

    QFile f(fileName);
    if (!f.open(IO_WriteOnly)) {
        cmdToDebug(tr("Could not write to file '%1'").arg(fileName),
                MT_INFO, HL_CMND);
        return false;
    }

    QTextStream ts(&f);
    
    QDateTime dt = QDateTime::currentDateTime();
    
    /*TODO: srcpCom->writeFileTextToStream(ts);*/
    // write the header
    ts << "# spdrs60 data file" << endl
       << "# version=" << VERSION << endl
       << "# last modified=" << dt.toString(Qt::ISODate) << endl
       << GF_FORMATVERSION << DS << GF_FV << endl
       << GF_CMDHOST << DS << cmdHost << DS << cmdPort <<
                        DS << cmdLogin << endl
       << GF_FBHOST << DS << fbHost << DS << fbPort << DS << fbLogin <<
       endl;

    gbs->writeFileTextToStream(ts);
    rtController->writeFileTextToStream(ts);

    f.close();

    updateCaption();
    updateFileMenuItems();

    cmdToDebug(tr("Layout file '%1' saved").arg(fileName), MT_INFO, HL_CMND);
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
        cmdToDebug(tr("Saving aborted"), MT_INFO, HL_CMND);
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


void MainWindow::slotFileImport()
{
    if (isModified()) {
        int choice = querySaveChanges();
        switch (choice) {
            case 0:
                if (saveFile())
                    chooseImportFile();
                break;
            case 1:
                chooseImportFile();
                break;
            case 2:
            default:
                break;
        }
    }
    else {
        chooseImportFile();
    }
}


void MainWindow::chooseFile()
{
    QString fn = QFileDialog::getOpenFileName(lastDir,
        QString(tr("Layouts")) + " (*" + GF_GBSEXT + ")", this);
    if (fn.isEmpty())
        return;
    openFile(fn);
}


void MainWindow::chooseImportFile()
{
    QString fn = QFileDialog::getOpenFileName(lastDir,
        QString(tr("Layouts")) + " (*" GF_OLDGBSEXT + ")", this);
    if (fn.isEmpty())
        return;
    importFile(fn);
}


void MainWindow::openFile(const QString& fn)
{
    /*remember last directory we used*/
    lastDir = fn.left(fn.findRev('/'));

    QFile f(fn);
    if (!f.open(IO_ReadOnly)){
        cmdToDebug(tr("Could not read file '%1'").arg(fn), MT_INFO, HL_CMND);
        return;
    }
    fileName = fn;

    QTextStream ts(&f);
    
    QString s, key, value;
    /*TODO: srcpCom->readFileTextFromStream(ts);*/
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
            }
            else if (key.compare(GF_FBHOST) == 0){
                fbHost = value;
                value = s.section(DS, 2, 2).stripWhiteSpace();
                fbPort = value.toInt();
            }
            else if (s.startsWith("%% layout"))
                break;
        }
    }
    gbs->readFileTextFromStream(ts);
    rtController->readFileTextFromStream(ts);

    f.close();
    
    cmdToDebug(tr("Layout file '%1' opened").arg(fn), MT_INFO, HL_CMND);
    updateCaption();
    updateFileMenuItems();
    // update feedback states
    layoutUpdateFB();
}


void MainWindow::importFile(const QString& fn)
{
    if (gbs == NULL || rtController == NULL)
        return;

    /*remember last directory we used*/
    lastDir = fn.left(fn.findRev('/'));

    QFile f(fn);
    if (!f.open(IO_ReadOnly)){
        cmdToDebug(tr("Could not read file '%1'").arg(fn), MT_INFO, HL_CMND);
        return;
    }
    fileName = "";

    QTextStream ts(&f);
    gbs->readOldFileTextFromStream(ts);
    f.close();

    /*now read old routes file*/
    int pos = fn.findRev(GF_OLDGBSEXT);
    QString rfn = fn.left(pos);
    rfn.append(GF_OLDRTSEXT);
    rtController->importFile(rfn);

    cmdToDebug(tr("Layout file '%1' imported").arg(fn), MT_INFO, HL_CMND);
    updateCaption();
    updateFileMenuItems();
}


void MainWindow::updateCaption()
{
    if (fileName.isEmpty())
        setCaption(QString(APP_NAME) + " - [" + tr("noname") + "]");
    else
        setCaption(QString(APP_NAME) + " - [" + fileName + "]");
}


/* New event driven networking code starts here: (guido)
   This should be a separate class like "SRCPCommunicator"
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
    cmdToDebug(tr("Command port: Host '%1' found.").arg(HOST), MT_INFO, HL_CMND);
}


void MainWindow::CommandSocketReadyRead()
{
    QString sSRCPVer, ServerInfo;

    while (CommandSocket->canReadLine()) {
        ServerInfo = CommandSocket->readLine();
        cmdToDebug(ServerInfo, MT_INFO, HL_CMND);
        
        if (SRCPCommandStatus == srcpLogin) {
            sWelcome = ServerInfo;
            sSRCPVer = sWelcome.mid(sWelcome.find("SRCP ", 0, 0) + 5, 5);

            if (isValidSRCPVersion(sSRCPVer)) {
                cmdToDebug(tr("SRCP: %1 ===> PASS").arg(sSRCPVer), MT_INFO,
                           HL_CMND);
                /* first is OK, next two readonly ports follow */
                ConnectFeedbackPort();
                ConnectInfoPort();

                /*
                 * ask server about power state, may be an other client
                 * allready switched power on
                 */
                SRCPCommandStatus = srcp07GetPower;
                SendCommandToSRCPServer("GET POWER");
            }
            else {
                cmdToDebug(tr("SRCP: %1 ===> FAILED; "
                            "spdrs60 requires SRCP >= 0.7.0 and < 0.8.0")
                           .arg(sSRCPVer), MT_INFO, HL_CMND);
                /* close connection */
                // TODO: a SRCP 0.8.x server will not understand this:
                SendCommandToSRCPServer("LOGOUT");
            }
        }
        else if (SRCPCommandStatus == srcp07GetPower) {
            /* INFO POWER ON */
            if (ServerInfo.contains("POWER ON")) {
                LayoutPowerIsOn = true;
                updateLayoutPowerAction();
            }
            else {
                if (AUTO_ZP9) {
                    LayoutPowerIsOn = !AUTO_ZP9;
                    slotToggleLayoutPower();
                }
            }
            SRCPCommandStatus = srcp07Connected;
            updateDaemonMenu();
            return;
        }
        else if (SRCPCommandStatus == srcp07Connected) {
            /*close command port if string with zero length is send*/
            if (ServerInfo.length() == 0) {
                if (CommandSocket->isOpen()) {
                    CommandSocket->close();
                    CommandSocketConnectionClosed();
                }
            }
        }
        
        else if (SRCPCommandStatus == srcp07GetFBStates) {
            /*
             * INFO FB <module_type> * < all states>
             *   0   1      2        3       4
             */
            if (ServerInfo.startsWith("INFO FB")) {
                QString allstates = ServerInfo.section(" ", 4, 4);
                
                unsigned int limit = allstates.length();
                if (limit > MAX_FB)
                    limit = MAX_FB;
               
                unsigned int fbbus, fbcontact, fbstate;
                
                for (unsigned int port = 0; port < limit; port++) {
                    fbstate = allstates[port].digitValue();
                    bFBport[port] = (fbstate == 1);
                    fbcontact = port % 496 + 1;
                    fbbus = port / 496 + 1;

                    // update module window and gbs
                    emit sendFBChangeModule(fbbus, fbcontact, fbstate);
                    emit sendFBChangeLayout(fbbus, fbcontact, fbstate == 1);
                }
            }

            SRCPCommandStatus = srcp07Connected;
	}
        
        else {
            /* else: no login but connection close */
            cmdToDebug(tr("Cannot read server welcome message!"),
                    MT_INFO, HL_CMND);
            /*close command port */
            if (ServerInfo.length() == 0) {
                if (CommandSocket->isOpen()) {
                    CommandSocket->close();
                    CommandSocketConnectionClosed();
                }
            }
        }
    }
}


void MainWindow::CommandSocketConnected()
{
    cmdToDebug(tr("Command port connected!"), MT_INFO, HL_CMND);
    CommandPortIsConnected = true;
    updateDaemonMenu();
}


void MainWindow::CommandSocketConnectionClosedByServer()
{
    if (CommandSocket->isOpen()) {
        CommandSocket->close();
    }
    cmdToDebug(tr("Command port closed by foreign host!"), MT_INFO, HL_CMND);
    CommandPortIsConnected = false;
    SRCPCommandStatus = srcpUndefined;
    updateDaemonMenu();
}


void MainWindow::CommandSocketConnectionClosed()
{
    cmdToDebug(tr("Command port closed!"), MT_INFO, HL_CMND);
    CommandPortIsConnected = false;
    updateDaemonMenu();
}


void MainWindow::CommandSocketError(int e)
{
    QString ErrMessage = GetSocketErrorString(e);
    cmdToDebug(tr("Command port: Error number %1 occurred (%2)")
               .arg(e).arg(ErrMessage), MT_INFO, HL_CMND);
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

        /*
         * this feedback buffer will be removed in next release, it is
         * left here only for feedback module window 
         */
        // check address range just for case 
        if (fbport > 0 && fbport <= MAX_FB)
            bFBport[fbport - 1] = fbstate;

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
    cmdToDebug(tr("Feedback port connected!"), MT_INFO, HL_CMND);
    FeedbackPortIsConnected = true;
    // flag to avoid history line flooding by startup feedback
    isFBInitMode = true;
    updateFeedbackMenu();

    // on startup init ports
    for (int i = 0; i < MAX_FB; i++)
        bFBport[i] = 0;

    /*may be this makes only sense when a layout is loaded: */
    SendCommandToSRCPServer((FEEDBACK <=
                             1) ? "INIT FB S88" : "INIT FB I8255");
    cmdToDebug(tr
               ("Feedback port changes are omitted while initialization"),
               MT_INFO, HL_FEED);
}


void MainWindow::FeedbackSocketConnectionClosedByServer()
{
    if (FeedbackSocket->isOpen()) {
        FeedbackSocket->close();
    }
    cmdToDebug(tr("Feedback port closed by foreign host!"), MT_INFO, HL_CMND);
    FeedbackPortIsConnected = false;
    updateFeedbackMenu();
}


void MainWindow::FeedbackSocketConnectionClosed()
{
    cmdToDebug(tr("Feedback port closed!"), MT_INFO, HL_CMND);
    FeedbackPortIsConnected = false;
    updateFeedbackMenu();
}


void MainWindow::FeedbackSocketError(int e)
{
    QString ErrMessage = GetSocketErrorString(e);
    cmdToDebug(tr("Feedback port: Error number %1 occurred (%2)")
               .arg(e).arg(ErrMessage), MT_INFO, HL_CMND);
}


void MainWindow::InfoSocketReadyRead()
{
    if (gbs == NULL)
        return;

    QString sInfo = "";

    while (InfoSocket->canReadLine()) {
        sInfo = InfoSocket->readLine();
        cmdToDebug(sInfo, MT_CMD, HL_INFO);

        QString device = sInfo.section(" ", 1, 1);
        /*
         * check for incomming GA actions and send them to gbs
         * INFO GA <protocol> <addr> <port> <state>
         *   0   1     2        3      4       5
         */
        if ("GA" == device) {
            gbs->sendInfoPortMessage(
                    sInfo.section(" ", 2, 2),
                    sInfo.section(" ", 3, 3).toInt(),
                    sInfo.section(" ", 4, 4).toInt(),
                    sInfo.section(" ", 5, 5).toInt());
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

            // just for case 
            if (fbport > 0 && fbport <= MAX_FB)
                bFBport[fbport - 1] = fbstate;

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
}


void MainWindow::InfoSocketConnected()
{
    cmdToDebug(tr("Info port connected!"), MT_INFO, HL_CMND);
    InfoPortIsConnected = true;
}


void MainWindow::InfoSocketConnectionClosedByServer()
{
    if (InfoSocket->isOpen()) {
        InfoSocket->close();
    }
    cmdToDebug(tr("Info port closed by foreign host!"), MT_INFO, HL_CMND);
    InfoPortIsConnected = false;
}


void MainWindow::InfoSocketConnectionClosed()
{
    cmdToDebug(tr("Info port closed!"), MT_INFO, HL_CMND);
    InfoPortIsConnected = false;
}


void MainWindow::InfoSocketError(int e)
{
    QString ErrMessage = GetSocketErrorString(e);
    cmdToDebug(tr("Info port: Error number %1 occurred (%2)")
               .arg(e).arg(ErrMessage), MT_INFO, HL_CMND);
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
    //LoginIsRunning = true;
    ConnectCommandPort();
}


bool MainWindow::isValidSRCPVersion(const QString& SRCPVerStr)
{
    bool returnvalue = false;

    if ((QString::compare(SRCPVerStr, "0.7.0") >= 0) &&
        QString::compare(SRCPVerStr, "0.8.0") < 0) {
        returnvalue = true;
    }
    return returnvalue;
}


void MainWindow::ConnectCommandPort()
{
    SRCPCommandStatus = srcpLogin;
    CommandSocket->connectToHost(HOST, PORT);   /*e.g.: 12345 */
}


void MainWindow::ConnectFeedbackPort()
{
    FeedbackSocket->connectToHost(HOST, PORT + 1);      /*e.g.: 12346 */
}


void MainWindow::ConnectInfoPort()
{
    InfoSocket->connectToHost(HOST, PORT + 2);  /*e.g.: 12347 */
}


void MainWindow::CloseSRCPServerConnection()
{
    /* Command port is closed by "passive close" */
    SendCommandToSRCPServer("LOGOUT");

    /* Feedback port is closed by "active close" */
    if (FeedbackSocket->isOpen()) {
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

    /* Info port is closed by "active close" */
    if (InfoSocket->isOpen()) {
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


void MainWindow::SendCommandToSRCPServer(const QString& CommandStr)
{
    if (CommandSocket->isOpen()) {
        QCString Command = CommandStr.ascii();
        /*temporary solution for command strings with and without '\n' */
        if (Command.find("\n", Command.length() - 1, true) == -1)
            Command.append("\n");
        CommandSocket->writeBlock(Command, (ulong) Command.length());
        cmdToDebug(CommandStr, MT_CMD, HL_CMND);
    }
}
/* End of new Networking code */


void MainWindow::slotToggleLayoutPower()
{
    LayoutPowerIsOn = !LayoutPowerIsOn;

    SendCommandToSRCPServer(LayoutPowerIsOn ? "SET POWER ON" : "SET POWER OFF");
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
}


void MainWindow::slotDaemonReset()
{
    // reset the daemon
    SendCommandToSRCPServer("RESET");
}


void MainWindow::slotDaemonKill()
{
    int choice = QMessageBox::warning(this, tr("Kill SRCP daemon"),
                     tr("You are about to kill the daemon forever.\n\n"
                        "If you really want to do it, click \"Kill\".\n\n"
                        "(Note: if you plan to use this program again\n"
                        "please restart daemon first, then this program)."),
                        tr("&Kill"), tr("Cancel"), 0, 1, 1);
    if (choice == 1)            // do not kill daemon -> return
        return;

    SendCommandToSRCPServer("SHUTDOWN");
    cmdToDebug(tr
               ("Daemon has been killed. Restart server, "
                "then reconnect \"SpDrS60 for Linux\""),
               MT_INFO, HL_CMND);

    CloseSRCPServerConnection();
}


void MainWindow::slotDaemonInfo()
{
    int iSep = sWelcome.find(';', 0, 0);
    QString sServer = sWelcome.left(iSep);
    QString sSRCP = sWelcome.right(sWelcome.length() - iSep - 2);
    QString sInfo;
    sInfo.sprintf(tr("Server name and version number:\n%s\n"
                     "\nSRCP version number:\n%s"),
                     sServer.data(), sSRCP.data());
    QMessageBox::information(this, tr("Daemon info"), sInfo);
}


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
    actionLayoutUpdateFB->setEnabled(LayoutPowerIsOn);
}


void MainWindow::updateFeedbackMenu()
{
    actionViewFBModules->setEnabled(FeedbackPortIsConnected);
}


/*Help menu slots*/
void MainWindow::slotAbout()
{
    QMessageBox::information(this, QString(tr("About ")) + APP_NAME,
      QString(APP_NAME) + " " + VERSION + "\n" +
      tr("(C) 1999-2003 by Stefan Preis\n"
         "(C) 2004-2005 by Guido Scholz\n"
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


void MainWindow::slotAboutQt()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}


void MainWindow::slotShowRoutes()
{
    if (rtViewer!= NULL)
        rtViewer->show();
}


void MainWindow::slotEditGBSFiles()
{
    // edit layout file with editor program
    QString sCommand = EDITOR;
    sCommand.append(" " + fileName + (" &"));
    system(sCommand.data());
    // TODO: check this: tbFileSave->setEnabled(true);
}


void MainWindow::slotEditConfigFile()
{
    // edit program´s config file with editor program
    QString sCommand = EDITOR + " " + QDir::homeDirPath() + "/" +
        SPDRS60_INIT + (" &");
    system(sCommand.data());
}


void MainWindow::slotViewSwitchMode(QAction* ac)
{
    bool rtvIsVisible = rtViewer->isVisible();

    if (ac == actionViewNormalMode) {
            visualMode = kvmNormal;
            updateRouteMenu(rtvIsVisible);
            cmdToDebug(tr("Layout in normal view mode"), MT_INFO, HL_CMND);
    }
    else if (ac == actionViewLayoutEditMode) {
            visualMode = kvmEditLayout;
            // when layout was in edit mode, it is
            // assumed to be modified
            gbs->setModified(true);
            updateRouteMenu(rtvIsVisible);
            cmdToDebug(tr("Entering layout edit mode"), MT_INFO, HL_CMND);
    }
    else if (ac == actionViewRouteEditMode) {
            visualMode = kvmEditRoute;
            updateRouteMenu(rtvIsVisible);
            cmdToDebug(tr("Entering route edit mode"), MT_INFO, HL_CMND);
        }
    // send new visual mode to router and gbs
    emit switchedVisualMode(visualMode);

    // change edit related menus
    actionFileNew->setEnabled(visualMode == kvmNormal);
    actionFileOpen->setEnabled(visualMode == kvmNormal);
    actionFileSave->setEnabled(visualMode == kvmNormal);
    actionFileSaveAs->setEnabled(visualMode == kvmNormal);
    actionFileImport->setEnabled(visualMode == kvmNormal);
}

/* called when layout viewmode is changed, visibility of routingviewer
 * changes and activity state of a selected route changes
 */
void MainWindow::updateRouteMenu(bool rtvIsVisible)
{
     //activate rtviewer to update route visibility
     if (rtvIsVisible)
         rtViewer->switchVisualMode(visualMode);

     if (!rtvIsVisible) {
         actionRouteStart->setEnabled(false);
         actionRouteStop->setEnabled(false);
     }
     else if (visualMode != kvmNormal) {
         actionRouteStart->setEnabled(false);
         actionRouteStop->setEnabled(false);
     }
     actionRouteAdd->setEnabled(rtvIsVisible &&
             visualMode == kvmEditRoute);
     // the next three items should only be enabled when route list
     // count > 0
     bool hasroutes = (rtController->getRouteCount() > 0);
     
     actionRouteEdit->setEnabled(rtvIsVisible &&
             visualMode == kvmEditRoute && hasroutes);
     actionRouteCopy->setEnabled(rtvIsVisible &&
             visualMode == kvmEditRoute && hasroutes);
     actionRouteDelete->setEnabled(rtvIsVisible &&
             visualMode == kvmEditRoute && hasroutes);
}


void MainWindow::updateRouteMenuActivateItems(bool isLocked)
{
    if (rtViewer->isVisible() && visualMode == kvmNormal) {
        actionRouteStart->setEnabled(!isLocked);
        actionRouteStop->setEnabled(isLocked);
    }
    else {
        actionRouteStart->setEnabled(false);
        actionRouteStop->setEnabled(false);
    }
}


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

    if (BROWSER == "mozilla" || BROWSER == "firefox") {
        if (system(BROWSER + " -remote 'ping()'") == 0)
            system(BROWSER + " -remote 'openURL(" + sURL + ",new-tab)'");
        else
            system(BROWSER + " " + sURL + " &");
    }
    else
        system(BROWSER + " " + sURL + " &");
}


// open SpDrS60 web resources
void MainWindow::slotAboutWeb()
{    
    QString sURL = QString("http://spdrs60.sourceforge.net/");

    if (BROWSER == "mozilla" || BROWSER == "firefox") {
        if (system(BROWSER + " -remote 'ping()'") == 0)
            system(BROWSER + " -remote 'openURL(" + sURL + ",new-tab)'");
        else
            system(BROWSER + " " + sURL + " &");
    }
    else
        system(BROWSER + " " + sURL + " &");
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
    modulesWindow = new feedback(this);
    connect(this, SIGNAL(sendFBChangeModule(unsigned int, unsigned int,
                    unsigned int)),
            modulesWindow, SLOT(slotUpdateModules(unsigned int,
                    unsigned int, unsigned int)));
    modulesWindow->show();
}


// show a simple keyboard
void MainWindow::slotViewKeyboard()
{
    keybWindow = new keyboard(this);
    connect(keybWindow, SIGNAL(sendCommand(const QString&)),
            this, SLOT(SendCommandToSRCPServer(const QString&)));

    keybWindow->move(QCursor::pos());
    keybWindow->show();
}


void MainWindow::cmdToDebug(const QString& hl_message, int m_type,
                            int hl_type)
{
    QString t;

    QTime cmdTime = QTime::currentTime();
    t.sprintf("%02d:%02d:%02d ", cmdTime.hour(), cmdTime.minute(),
              cmdTime.second());

    if (m_type == MT_INFO)
        t.append("> ");         // == an info line
    else
        t.append("# ");         // == a command

    t.append(hl_message);

    // delete oldest entries and add newer ones
    switch (hl_type) {
        case HL_CMND:
            if (HistCB->count() == MAX_HISTORY)
                HistCB->removeItem(0);
            HistCB->insertItem(t);
            HistCB->setCurrentItem(HistCB->count() - 1);
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


void MainWindow::slotEditOptions()
{
    // open dialog window with program options
    optionsWindow = new optionsDialog(this);
    connect(optionsWindow, SIGNAL(repaintLayout()),
            gbs, SIGNAL(sigRepaintLayout()));
    connect(optionsWindow, SIGNAL(refreshConfigData()),
            this, SLOT(slotReadConfigFile()));
    connect(optionsWindow, SIGNAL(showLogMessage(const QString&, int, int)),
            this, SLOT(cmdToDebug(const QString&, int, int)));
    optionsWindow->show();
}


void MainWindow::slotFileNewWin()
{
    /*create new application window*/
    MainWindow *sw = new MainWindow;
    sw->resize(740, 480);
    sw->show();

}


/*
 * create a new locator window and connect its signals directly to gbs
 */
void MainWindow::slotEditFind()
{
    findWindow = new Finder(this);
    connect(findWindow, SIGNAL(sigFind(const QString&, int, bool)),
            gbs, SLOT(slotEditFind(const QString&, int, bool)));
    findWindow->exec();
}


void MainWindow::layoutUpdateFB()
{
    SendCommandToSRCPServer((FEEDBACK <=
        1) ? "GET FB S88 *" : "GET FB I8255 *");
    SRCPCommandStatus = srcp07GetFBStates;
}


void MainWindow::layoutChangeSize()
{
    newLayoutDialog* nlDlg = new newLayoutDialog(this);

    nlDlg->setColumns(gbs->getColumns());
    nlDlg->setRows(gbs->getRows());
    
    if (nlDlg->exec() == QDialog::Accepted) {
        int iNewCols = nlDlg->getColumns();
        int iNewRows = nlDlg->getRows();
        gbs->setLayoutSize(iNewCols, iNewRows);
    }
    delete nlDlg;

    bool im = isModified();
    actionFileSave->setEnabled(im);
}


bool MainWindow::isModified()
{
    return (gbs->isModified() || rtController->isModified());
}


void MainWindow::slotRouteAdd()
{
    if (rtViewer != NULL)
        rtViewer->slotRouteAdd();
        bool rtvIsVisible = rtViewer->isVisible();
        /*update rootingtoolbar buttons*/
        updateRouteMenu(rtvIsVisible);
}


void MainWindow::slotRouteDelete()
{
    if (rtViewer != NULL) {
        rtViewer->slotRouteDelete();
        bool rtvIsVisible = rtViewer->isVisible();
        /*update rootingtoolbar buttons*/
        updateRouteMenu(rtvIsVisible);
    }
}
