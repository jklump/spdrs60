/***************************************************************************
                           mainwindow.cpp
                           version 0.4.7 $Revision: 1.2 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-01-28 20:35:04 $
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

#include <string.h>             // for bzero()
#include <stdio.h>              // for perror(), sprintf()
#include <unistd.h>             // for write(), exit()
#include <stdlib.h>             // for system()
#include <math.h>               // for fabs
#include <signal.h>             // for signal()
#include <qhbox.h>
#include <qvbox.h>

#include "gbsscrollview.h"
#include "mainwindow.h"

/*toolbar icons*/
#include "pixmaps/filenew.xpm"
#include "pixmaps/fileopen.xpm"
#include "pixmaps/filesave.xpm"
#include "pixmaps/editcut.xpm"
#include "pixmaps/editcopy.xpm"
#include "pixmaps/editpaste.xpm"
#include "pixmaps/viewroute.xpm"
#include "pixmaps/viewfeedback.xpm"
#include "pixmaps/viewkeyboard.xpm"
#include "pixmaps/viewclock.xpm"
#include "pixmaps/layoutstart.xpm"
#include "pixmaps/layoutstop.xpm"
#include "pixmaps/layoutnotrot.xpm"


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
:  QMainWindow(0, "SpDrS60", WDestructiveClose | WGroupLeader)
{
    /*Networking */
    CommandPortIsConnected = false;
    FeedbackPortIsConnected = false;
    InfoPortIsConnected = false;

    loadTimer = NULL;
    iDebugNo = HIST;            // default debug window ist HISTORY
    isFBInitMode = true;        // var to avoid all startup feedback
    // changes to be shown in debug window
    iEditMode = NOEDIT;         // normal layout mode
    lastDir = QDir::homeDirPath();      // remembers path for FileOpen
    initMainWindow();           // setup main window with all menus
    slotReadConfigFile();       // read user dependend config file


    if (SERVER == SRCP) {
        initAllSockets();       // init connection to daemon ...
        if (SERVERLOGIN)
            ConnectToSRCPServer();
    }

    /* autostart voltage on layout only when these conditions are true?
       1) option set in preferences
       2) connection to server established
       3) layout is loaded
     */
    //bRunLayout = (AUTO_ZP9 && CommandPortIsConnected && LOAD_DEF_LAYOUT);
    bRunLayout = false;

    /* Hint: This time delay may not be sufficent on a host to host
     * connection; idealy we have to wait here for a established
     * connection (connected event) before we send layout data to the
     * server. May be this code can simply be shifted to slot
     * "FeedbackSocketConnected" */
    /*
       DefaultLayoutIsLoaded = false;
       if (LOAD_DEF_LAYOUT && !AUTO_ZP9)
       {
       slotLoadTimerTimeout();
       DefaultLayoutIsLoaded = true;
       }
     */
/* Load last/default layout automatically if defined so in options menu.
 First let the prog start completly, else the MainWindow will show up
 _after_ loading dialog */
    if (LOAD_DEF_LAYOUT) {
        loadTimer = new QTimer(this);
        connect(loadTimer, SIGNAL(timeout()),
                this, SLOT(slotLoadTimerTimeout()));
        // load the layout with 1000 ms delay
        loadTimer->start(LOAD_TIMER_TIME, true);
    }

    cmdToDebug(tr("Program succesfully started!"), INFO, HIST);
}

/* Cleanup by destructor */
MainWindow::~MainWindow()
{
    // send the LOGOUT command to daemon
    CloseSRCPServerConnection();
    delete CommandSocket;
    delete FeedbackSocket;
    delete InfoSocket;
}

void MainWindow::slotReadConfigFile()
{
    int i;
    // open SpDrS60 config file and read all global parameters
    QFile file(QDir::homeDirPath() + "/" + SPDRS60_INIT);
    if (!file.open(IO_ReadOnly)) {
        /* if no configuration file is found, just keep defaults */
        cmdToDebug(tr("Personal config file not found") + ": ~/" +
                   SPDRS60_INIT, INFO, HIST);
        return;
    }
    QTextStream ts(&file);
    QString s;

    // layout section
    for (i = 0; i < 5; i++)     // omit five section description lines
        s = ts.readLine();

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
    // setup the menubar with all pulldown menus
    // all *->setItemEnabled  and ->setFocusPolicy moved to new function
    // resetMenu() //dirk.
    // Thus we can call resetMenu each time we close a layout, before we
    // open a new one. The menubar will be in the initial state when no
    // layout is loaded
    filemenu = new QPopupMenu;
    filemenu->insertItem(tr("&New..."),
                         this, SLOT(slotNew()), CTRL + Key_N, FILE_ID_NEW);
    filemenu->insertItem(tr("&Open..."),
                         this, SLOT(slotLoad()), CTRL + Key_O,
                         FILE_ID_OPEN);
    filemenu->insertItem(tr("&Save"), this, SLOT(slotSave()), CTRL + Key_S,
                         FILE_ID_SAVE);
    filemenu->insertItem(tr("&Save As..."), this, SLOT(slotSaveAs()), 0,
                         FILE_ID_SAVE_AS);
    filemenu->insertSeparator();
    //filemenu->insertItem(tr("New &Window"),
    //  this, SLOT(slotNewWin()), 0         , FILE_ID_NEWWIN);
    //filemenu->insertSeparator();
    filemenu->insertItem(tr("&Quit"),
                         this, SLOT(slotQuit()), CTRL + Key_Q,
                         FILE_ID_QUIT);

    editfilemenu = new QPopupMenu;
    editfilemenu->insertItem(tr("Layout"),
                             this, SLOT(slotEditGBSFiles()), 0,
                             EDITFILE_ID_GBS);
    editfilemenu->insertItem(tr("Routings"), this,
                             SLOT(slotEditRTSFiles()), 0, EDITFILE_ID_RTS);
    editfilemenu->insertItem(QDir::homeDirPath() + "/" + SPDRS60_INIT,
                             this, SLOT(slotEditConfigFile()), 0,
                             EDITFILE_ID_CON);

    editmenu = new QPopupMenu;
    editmenu->insertItem(tr("&Cut"), this, SLOT(slotEditCut()),
                         CTRL + Key_X, EDIT_ID_CUT);
    editmenu->insertItem(tr("C&opy"), this, SLOT(slotEditCopy()),
                         CTRL + Key_C, EDIT_ID_COPY);
    editmenu->insertItem(tr("&Paste"), this, SLOT(slotEditPaste()),
                         CTRL + Key_V, EDIT_ID_PASTE);
    /*disable this items until they are implemented */
    editmenu->setItemEnabled(EDIT_ID_CUT, false);
    editmenu->setItemEnabled(EDIT_ID_COPY, false);
    editmenu->setItemEnabled(EDIT_ID_PASTE, false);

    editmenu->insertSeparator();
    editmenu->insertItem(tr("&Data files"), editfilemenu);
    editmenu->insertItem(tr("&Find..."),
                         this, SLOT(slotFind()), CTRL + Key_F,
                         EDIT_ID_FIND);
    editmenu->insertItem(tr("&Preferences..."), this,
                         SLOT(slotShowOptions()), CTRL + Key_P,
                         EDIT_ID_OPT);
    editmenu->setCheckable(true);

    viewmenu = new QPopupMenu;
    viewmenu->insertItem(tr("&Routing table"),
                         this, SLOT(slotRoutesDialog()), CTRL + Key_R,
                         VIEW_ID_ROUTES);
    viewmenu->insertItem(tr("&Feedback modules"),
                         this, SLOT(slotShowModules()), CTRL + Key_M,
                         VIEW_ID_FBMOD);
    viewmenu->insertItem(tr("&Central clock"),
                         this, SLOT(slotShowClock()), 0, VIEW_ID_CLOCK);
    viewmenu->insertItem(tr("&Keyboard"),
                         this, SLOT(slotKeyboard()), CTRL + Key_K,
                         VIEW_ID_KEYB);
    viewmenu->insertSeparator();
    viewmenu->insertItem(tr("Toggle &Debugging"), this,
                         SLOT(slotViewDebug()), CTRL + Key_D,
                         VIEW_ID_DEBG);
    viewmenu->insertItem(tr("&Editmode"),
                           this, SLOT(slotEditLayout()), CTRL + Key_E,
                           VIEW_ID_EDITMODE);

    daemonmenu = new QPopupMenu;
    daemonmenu->insertItem(tr("&Connect"),
                           this, SLOT(ConnectToSRCPServer()), 0,
                           DAEMON_ID_CONNECT);
    daemonmenu->insertItem(tr("&Disconnect"), this,
                           SLOT(CloseSRCPServerConnection()), 0,
                           DAEMON_ID_DISCONNECT);
    daemonmenu->insertSeparator();
    daemonmenu->insertItem(tr("&Reset"), this, SLOT(slotResetDaemon()), 0,
                           DAEMON_ID_RESET);
    daemonmenu->insertItem(tr("&Kill"), this, SLOT(slotKillDaemon()), 0,
                           DAEMON_ID_KILL);
    daemonmenu->insertItem(tr("&Info..."), this, SLOT(slotAboutDaemon()),
                           0, DAEMON_ID_INFO);

    layoutmenu = new QPopupMenu;
    layoutmenu->insertItem(tr("&Start power"),
                           this, SLOT(slotToggleLayoutPower()), Key_F4,
                           LAYOUT_ID_START);
    layoutmenu->insertItem(tr("Use &FHT"),
                           this, SIGNAL(FHTclicked()), Key_F5,
                           LAYOUT_ID_FHT);
    layoutmenu->insertItem(tr("Use &WGT"), this, SIGNAL(WGTclicked()),
                           Key_F6, LAYOUT_ID_WGT);
    layoutmenu->insertItem(tr("Use &UfGT"), this, SIGNAL(UfGTclicked()),
                           Key_F7, LAYOUT_ID_UFGT);
    layoutmenu->insertSeparator();
    layoutmenu->insertItem(tr("&Halt signals"), this, SIGNAL(notrot()),
                           Key_F12, LAYOUT_ID_NOTROT);
    layoutmenu->insertItem(tr("&Toggle all"),
                           this, SIGNAL(toggleAll()), Key_F10,
                           LAYOUT_ID_TOGGLE);
    layoutmenu->insertItem(tr("&Send all"), this, SIGNAL(sendAll()),
                           Key_F11, LAYOUT_ID_SEND);
    layoutmenu->insertItem(tr("&Unlock routings"), this,
                           SIGNAL(unlockRoutings()), CTRL + Key_U,
                           LAYOUT_ID_UNLOCKR);

    helpmenu = new QPopupMenu;
    helpmenu->insertItem(tr("&Help"), this, SLOT(slotHelp()), Key_F1);
    helpmenu->insertItem(tr("&SpDrS60 for Linux on the web"),
                         this, SLOT(slotWeb()));
    helpmenu->insertSeparator();
    helpmenu->insertItem(QString(tr("&About")) + " \"" + APP_NAME + "\"",
                         this, SLOT(slotAbout()));
    helpmenu->insertItem(tr("About &Qt"), this, SLOT(slotAboutQt()));

    menubar = new QMenuBar(this);
    menubar->insertItem(tr("&File"), filemenu);
    menubar->insertItem(tr("&Edit"), editmenu);
    menubar->insertItem(tr("&View"), viewmenu);
    menubar->insertItem(tr("&Daemon"), daemonmenu);
    menubar->insertItem(tr("&Layout"), layoutmenu);
    menubar->insertItem(tr("&Help"), helpmenu);


    // setup the toolbar
    toolbar = new QToolBar(this, "toolbar");
    Q_CHECK_PTR(toolbar);
    toolbar->setLabel(tr("File operations"));

    tbFileNew =
        new QToolButton(QPixmap(filenew_xpm), tr("Create empty layout"), 0,
                        this, SLOT(slotNew()), toolbar);
    tbFileOpen =
        new QToolButton(QPixmap(fileopen_xpm), tr("Open layout file"), 0,
                        this, SLOT(slotLoad()), toolbar);
    tbFileSave =
        new QToolButton(QPixmap(filesave_xpm), tr("Save layout file"), 0,
                        this, SLOT(slotSave()), toolbar);
    toolbar->addSeparator();

    tbEditCut =
        new QToolButton(QPixmap(editcut_xpm),
                        tr("Cut selection to clipboard"), 0,
                        this, SLOT(slotEditCut()), toolbar);
    tbEditCopy =
        new QToolButton(QPixmap(editcopy_xpm),
                        tr("Copy selection to clipboard"), 0,
                        this, SLOT(slotEditCopy()), toolbar);
    tbEditPaste =
        new QToolButton(QPixmap(editpaste_xpm),
                        tr("Paste from clipboard"), 0,
                        this, SLOT(slotEditPaste()), toolbar);
    /*disable this items until the functions are implemented */
    tbEditCut->setEnabled(false);
    tbEditCopy->setEnabled(false);
    tbEditPaste->setEnabled(false);

    toolbar->addSeparator();

    tbViewRoute =
        new QToolButton(QPixmap(viewroute_xpm), tr("Show routing table"),
                        0, this, SLOT(slotRoutesDialog()), toolbar);
    tbViewFeedb =
        new QToolButton(QPixmap(viewfeedback_xpm),
                        tr("Show feedback window"), 0, this,
                        SLOT(slotShowModules()), toolbar);
    tbViewKeyb =
        new QToolButton(QPixmap(viewkeyboard_xpm),
                        tr("Show basic keyboard"), 0, this,
                        SLOT(slotKeyboard()), toolbar);

    tbViewClock =
        new QToolButton(QPixmap(viewclock_xpm), tr("Show central clock"),
                        0, this, SLOT(slotShowClock()), toolbar);

    toolbar->addSeparator();

    tbLayoutStart =
        new QToolButton(QPixmap(layoutstart_xpm), tr("Start layout"), 0,
                        this, SLOT(slotToggleLayoutPower()), toolbar);
    tbLayoutStop =
        new QToolButton(QPixmap(layoutstop_xpm), tr("Stop layout"), 0,
                        this, SLOT(slotToggleLayoutPower()), toolbar);
    tbLayoutNotRot =
        new QToolButton(QPixmap(layoutnotrot_xpm), tr("Halt signals"), 0,
                        this, SIGNAL(notrot()), toolbar);
    // all *->setItemEnabled  and ->setFocusPolicy  moved to resetMenu(), dirk.
    resetMenu();
    // statusBar();  // create a StatusBar; no need for that up to now

    /* This is the window layout in detail:  (guido)
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
       | | +---------------+---------------------------------+ | |
       | | |  QHBox        |                                 | | |
       | | + +-------------+ + +---------------------------+ + | |
       | | | |QWidgetStack | | |QWidgetStack+----------+   | | | |
       | | | |   +-------+ | | |            |+----------+  | | | |
       | | | |   |+-------+| | |            +|+----------+ | | | |
       | | | |   +|QLabel || | |             +|QListView | | | | |
       | | | |    +-------+| | |              +----------+ | | | |
       | | + +-----------+ + +-----------------------------+ + | |
       | | +---------------+---------------------------------+ | |
       | +-----------------------------------------------------+ |
       +---------------------------------------------------------+
     */

    QVBox *vBox = new QVBox(this, "vbox", 0);

    scrollview = new GBSScrollView(vBox);
    gbs = new GBSArea(scrollview->viewport(), "gbsarea");
    Q_CHECK_PTR(gbs);
    scrollview->addChild(gbs);

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

    setCentralWidget(vBox);

    // now connect the different signals and slots
    // between "gbsArea" and "MainWindow"
    connect(this, SIGNAL(save()), gbs, SLOT(slotSave()));
    connect(this, SIGNAL(load()), gbs, SLOT(slotLoad()));
    connect(this, SIGNAL(showRoutings()), gbs, SLOT(slotShowRoutings()));
    connect(this, SIGNAL(newLayout(int)), gbs, SLOT(slotNew(int)));
    connect(this, SIGNAL(FHTclicked()), gbs, SLOT(slotFHTclicked()));
    connect(this, SIGNAL(WGTclicked()), gbs, SLOT(slotWGTclicked()));
    connect(this, SIGNAL(UfGTclicked()), gbs, SLOT(slotUfGTclicked()));
    connect(this, SIGNAL(unlockRoutings()), gbs,
            SLOT(slotUnlockRoutings()));
    connect(this, SIGNAL(toggleAll()), gbs, SLOT(slotToggleAll()));
    connect(this, SIGNAL(sendAll()), gbs, SLOT(slotSendAll()));
    connect(this, SIGNAL(notrot()), gbs, SLOT(slotNotrot()));
    connect(this, SIGNAL(sendFBChangeLayout(unsigned int)),
            gbs, SLOT(slotFBportChanged(unsigned int)));
    connect(this, SIGNAL(EditMode(int)), gbs, SIGNAL(EditMode(int)));
    connect(gbs, SIGNAL(cmdToDebug(const QString &)),
            this, SLOT(slotCmdToDebugExtern(const QString &)));
    connect(gbs, SIGNAL(sendCommand(const QString &)),
            this, SLOT(SendCommandToSRCPServer(const QString &)));
    connect(gbs, SIGNAL(sigUpdateEditmenu()),
            this, SLOT(slotUpdateEditmenu()));
    connect(gbs, SIGNAL(sigShowFBmodules()),
            this, SLOT(slotShowModules()));
}


// resetMenu added by dirk. Before loading a new layout, the settings of
// the old one must be reset.

void MainWindow::resetMenu()
{
    // always false on setup, a layout
    filemenu->setItemEnabled(FILE_ID_SAVE, false);
    // must be loaded first
    filemenu->setItemEnabled(FILE_ID_SAVE_AS, false);
    // always false on setup, a layout
    editfilemenu->setItemEnabled(EDITFILE_ID_GBS, false);
    // must be loaded first
    editfilemenu->setItemEnabled(EDITFILE_ID_RTS, false);
    editmenu->setItemEnabled(EDIT_ID_FIND, false);
    // must be loaded first
    viewmenu->setItemEnabled(VIEW_ID_ROUTES, false);
    // always false on setup, a layout
    viewmenu->setItemEnabled(VIEW_ID_ROUTES, false);
    layoutmenu->setItemEnabled(VIEW_ID_EDITMODE, false);
    // setup depends on daemonstartup
    updateDaemonMenu();
    // must be loaded first
    layoutmenu->setItemEnabled(LAYOUT_ID_WGT, false);
    layoutmenu->setItemEnabled(LAYOUT_ID_FHT, false);
    layoutmenu->setItemEnabled(LAYOUT_ID_UFGT, false);
    // must be loaded first
    layoutmenu->setItemEnabled(LAYOUT_ID_UNLOCKR, false);
    // always false on setup, a layout
    tbLayoutNotRot->setEnabled(false);  // disabled if no layout loaded
    tbFileSave->setEnabled(false);      // same
    tbViewRoute->setEnabled(false);     // same
    tbViewFeedb->setEnabled(false);
}


void MainWindow::slotEditCut()
{
 /*TODO*/}


void MainWindow::slotEditCopy()
{
 /*TODO*/}


void MainWindow::slotEditPaste()
{
 /*TODO*/}


void MainWindow::slotViewDebug()
{
    // circle-toggle between all three debugging windows
    iDebugNo += 1;
    if (iDebugNo > FEED)
        iDebugNo = HIST;

    switch (iDebugNo) {
    case (HIST):
        cbStack->raiseWidget(0);
        lblStack->raiseWidget(0);
        break;
    case (INFO):
        cbStack->raiseWidget(1);
        lblStack->raiseWidget(1);
        break;
    case (FEED):
        cbStack->raiseWidget(2);
        lblStack->raiseWidget(2);
        break;
    }
}



void MainWindow::slotLoadTimerTimeout()
{
    /* QTimer object is destroyed automatically when its parent object
       is  destroyed

       if (loadTimer != NULL) {
       delete loadTimer;     // waiting period is over, so now autoload
       loadTimer = NULL;
       }
     */

    // if autoload file from config data does
    // not exist ask user to changes options     
    QFile f(DEF_LAYOUT + GBS_FILE_SUFFIX);
    if (!f.exists()) {                           
        qApp->beep();
        int choice = QMessageBox::warning(this, tr("Autoloader failed"),
                         tr("The selected autoload file does not,\n"
                             "exist, please adjust your options.\n"),
                         tr("&Now"), tr("&Later"), 0, 0, 0);
        if (choice == 0)
            slotShowOptions();
    }
    else {
        FILENAME = DEF_LAYOUT;  // save autoload filename and open it
        setFilename();          //serd
        LoadFile();
    }
}



void MainWindow::closeEvent(QCloseEvent *)
{
    // when the whole application is to be closed by double-clicking on window
    // click-marks, the usual result is to close the application at once. thus an
    // open layout can't be saved. now get the window's closing event and turn it
    // into a well-defined saving action
    slotQuit();
}



void MainWindow::slotNew()
{
    int iNewCols = 0;

    iEditMode = NOEDIT;
    //if (!FILENAME.isEmpty())  // if a layout is already shown, save it first
    if (gbs->isModified())      // if a layout is already shown, save it first
        if (slotSave() == 2)
            return;             // in case save was cancelled  //dirk

    // show a dialog where the user inputs number of columns for an empty layout
    newLayoutDialog *enterNewCols = new newLayoutDialog(this);
    enterNewCols->setCaption(tr("New layout"));

    if (enterNewCols->exec() != QDialog::Accepted) {
        delete enterNewCols;
        return;
    }
    iNewCols = enterNewCols->iNumberOfColumns;
    delete enterNewCols;


    FILENAME = QString("./default");    // use temp filename "default"
    setFilename();              //serd
    setCaption(QString(APP_NAME) + " - [" + FILENAME + GBS_FILE_SUFFIX +
               "]");

    if (!gbs->slotNew(iNewCols)) {      // create empty layout from gbs

        tbFileSave->setEnabled(true);   // now enable all
        tbLayoutNotRot->setEnabled(true);       // senseful menu items
        tbViewRoute->setEnabled(true);  // or toolbuttons

        filemenu->setItemEnabled(FILE_ID_SAVE, false);  // only saveAs is corr
        filemenu->setItemEnabled(FILE_ID_SAVE_AS, true);

        layoutmenu->setItemEnabled(LAYOUT_ID_WGT, true);
        layoutmenu->setItemEnabled(LAYOUT_ID_FHT, true);
        layoutmenu->setItemEnabled(LAYOUT_ID_UFGT, true);

        layoutmenu->setItemEnabled(VIEW_ID_EDITMODE, true);
        viewmenu->setItemEnabled(VIEW_ID_ROUTES, false);
        //serd: war false, neues muß aber editierbar sein
        //editmenu->setItemEnabled(LAYOUT_ID_GBS, true);
        viewmenu->setItemEnabled(VIEW_ID_ROUTES, false);
        editmenu->setItemEnabled(EDIT_ID_FIND, true);

        editfilemenu->changeItem(tr("Layout file not saved yet"),
                                 EDITFILE_ID_GBS);
        editfilemenu->changeItem(tr("Route file not yet available"),
                                 EDITFILE_ID_RTS);
        // temporarily assign saveAs slot to save toolbutton
        disconnect(tbFileSave, SIGNAL(clicked()), this, 0);
        connect(tbFileSave, SIGNAL(clicked()), this, SLOT(slotSaveAs()));

        if (CommandPortIsConnected) {
            layoutmenu->setItemEnabled(LAYOUT_ID_TOGGLE, true);
            layoutmenu->setItemEnabled(LAYOUT_ID_SEND, true);
            layoutmenu->setItemEnabled(LAYOUT_ID_NOTROT, true);
        }
    }
    else                        //dirk
    {
        FILENAME = "";
        setCaption(QString(APP_NAME));
        resetMenu();
    }
}



void MainWindow::slotLoad()
{
    if (!FILENAME.isEmpty())    // if a layout is shown, save it first
        if (slotSave() == 2)    // in save-as-dialog cancel clicked by user
            return;

    // Open a file dialog for loading. The default directory is the
    // home directory, filter is GBS_FILE_SUFFIX (value: see Resources.h)
    QString sLoadFilename = QFileDialog::getOpenFileName(lastDir,
                                             QString(tr("Layouts"))
                                             + "(*" +
                                             GBS_FILE_SUFFIX + ")", this);

    if (!sLoadFilename.isEmpty()) {
        lastDir = sLoadFilename.left(sLoadFilename.findRev('/'));
        FILENAME = sLoadFilename.left(sLoadFilename.length() - 8);
        setFilename();          //serd
        LoadFile();
    }                           // the load()-slot has been divided into two pieces so the autoloader
}                               // can use it too


void MainWindow::LoadFile()
{
    // show filename in caption only after successful load. //dirk.
    setCaption(QString(APP_NAME));

    //emit load();   // start the actual loading code in "gbs"
    // I need a return code to see if load was successful. //dirk
    if (!gbs->slotLoad()) {
        setCaption(QString(APP_NAME) + " - [" + FILENAME +
                   GBS_FILE_SUFFIX + "]");

        tbFileSave->setEnabled(true);   // now enable all
        tbLayoutNotRot->setEnabled(true);       // senseful menu items
        tbViewRoute->setEnabled(true);  // or toolbuttons
        filemenu->setItemEnabled(FILE_ID_SAVE, true);
        filemenu->setItemEnabled(FILE_ID_SAVE_AS, true);

        layoutmenu->setItemEnabled(LAYOUT_ID_WGT, true);
        layoutmenu->setItemEnabled(LAYOUT_ID_FHT, true);
        layoutmenu->setItemEnabled(LAYOUT_ID_UFGT, true);
        layoutmenu->setItemEnabled(VIEW_ID_EDITMODE, true);

        editmenu->setItemEnabled(EDIT_ID_FIND, true);

        editfilemenu->setItemEnabled(EDITFILE_ID_GBS, true);
        editfilemenu->changeItem(FILENAME + GBS_FILE_SUFFIX,
                                 EDITFILE_ID_GBS);
        QFile file(FILENAME + RTS_FILE_SUFFIX);
        if (file.exists()) {
            editfilemenu->changeItem(FILENAME + RTS_FILE_SUFFIX,
                                     EDITFILE_ID_RTS);
            editfilemenu->setItemEnabled(EDITFILE_ID_RTS, true);
        }
        else {
            editfilemenu->changeItem(tr("Route file not yet available"),
                                     EDITFILE_ID_RTS);
            editfilemenu->setItemEnabled(EDITFILE_ID_RTS, false);
        }

        viewmenu->setItemEnabled(VIEW_ID_ROUTES, true);
        layoutmenu->setItemEnabled(LAYOUT_ID_UNLOCKR, true);
        // reassign the right slot to save toolbutton
        disconnect(tbFileSave, SIGNAL(clicked()), this, 0);
        connect(tbFileSave, SIGNAL(clicked()), this, SLOT(slotSave()));

        if (CommandPortIsConnected) {
            layoutmenu->setItemEnabled(LAYOUT_ID_SEND, true);
            layoutmenu->setItemEnabled(LAYOUT_ID_TOGGLE, true);
            layoutmenu->setItemEnabled(LAYOUT_ID_NOTROT, true);
        }
    }
    else {
        FILENAME = "";
        setCaption(QString(APP_NAME));
        resetMenu();
    }
}


void MainWindow::slotSaveAs()
{
    // Open a file dialog for saving. The default directory is the
    // current directory, the filter is GBS_FILE_SUFFIX (value: see Resources.h)
    QString sSaveFilename = QFileDialog::getSaveFileName(lastDir,
                                                         QString("*") +
                                                         GBS_FILE_SUFFIX,
                                                         this);

    if (!sSaveFilename.isEmpty()) {
        if (sSaveFilename.contains(GBS_FILE_SUFFIX, 1))
            sSaveFilename = sSaveFilename.left(sSaveFilename.length() - 8);

        QFile qf(sSaveFilename + GBS_FILE_SUFFIX);

        if (qf.exists() &&
            (1 == QMessageBox::warning(this, tr("Warning"),
                                       tr
                                       ("File exists!\nDo you want to overwrite it?"),
                                       tr("&Yes"), tr("&No"), 0, 0, 1)))
            return;

        gbs->FILENAME = sSaveFilename;
        if (!gbs->slotSave()) { // start the actual saving code in "gbs"
            // only at success:
            FILENAME = sSaveFilename;
            setCaption(QString(APP_NAME) + " - [" + FILENAME +
                       GBS_FILE_SUFFIX + " ]");

            filemenu->setItemEnabled(FILE_ID_SAVE, true);

            // now enable all
            // senseful menu items
            // or toolbuttons
            layoutmenu->setItemEnabled(VIEW_ID_EDITMODE, true);
            viewmenu->setItemEnabled(VIEW_ID_ROUTES, true);
            editmenu->setItemEnabled(EDITFILE_ID_GBS, true);
            editmenu->setItemEnabled(EDIT_ID_FIND, true);

            editfilemenu->changeItem(FILENAME + GBS_FILE_SUFFIX,
                                     EDITFILE_ID_GBS);
            editfilemenu->changeItem(tr("Route file not yet available"),
                                     EDITFILE_ID_RTS);
            // reassign the right slot to save toolbutton
            disconnect(tbFileSave, SIGNAL(clicked()), this, 0);
            connect(tbFileSave, SIGNAL(clicked()), this, SLOT(slotSave()));
            tbFileSave->setEnabled(true);       // serd: must be enabled also
        }
        else
            gbs->FILENAME = FILENAME;
    }
}


int MainWindow::slotSave()
{
    int choice = 0;
    // if the shown layout has been created just before, call saveAs() to save it
    // under a wonderful new name
    if (FILENAME.contains("default", 0) == true) {
        // a "default" layout must be saved as
        // if the user wants it to
        qApp->beep();
        choice = QMessageBox::warning(this, tr("Warning"),
                                      tr("New layout not saved yet!\n"
                                         "Do you want to save it?"),
                                      tr("&Yes"), tr("&No"), tr("Cancel"),
                                      0, 1);
        // enter button no = button 0 = "Yes"
        // ecape button no = button 1 = "No"
        switch (choice) {
        case 0:
            slotSaveAs();       // now save the "default" layout
            break;

        case 1:                //qApp->quit();        // quit everything and immediately
            break;

        case 2:                //return 2;            // return to programm
            break;
        }
    }
    else
        return gbs->slotSave(); // start the actual saving code in "gbs"

    return choice;
}


void MainWindow::slotUpdateEditmenu()
{
    // called by gbsArea if a non-existing routing file has been autocreated
    editmenu->setItemEnabled(EDITFILE_ID_RTS, true);
    editfilemenu->changeItem(FILENAME + RTS_FILE_SUFFIX, EDITFILE_ID_RTS);
}


void MainWindow::slotQuit()
{
    //if (!FILENAME.isEmpty())   // only if a layout is shown we must save it
    if (gbs->isModified())      // only if a layout is shown we must save it
        if (slotSave() == 2)    // in dialog cancel clicked
            return;

    emit EditMode(NOEDIT);
    //delete gbs;
    // now really quit everything
    qApp->quit();
}


/* New event driven networking code starts here: (guido)*/
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
    cmdToDebug(tr("Command port: Host '%1' found.").arg(HOST), INFO, HIST);
}


void MainWindow::CommandSocketReadyRead()
{
    QString sSRCPVer;

    if (LoginIsRunning) {
        if (CommandSocket->canReadLine()) {
            sWelcome = CommandSocket->readLine();
            sSRCPVer = sWelcome.mid(sWelcome.find("SRCP ", 0, 0) + 5, 5);

            if (isValidSRCPVersion(sSRCPVer)) {
                cmdToDebug(tr("SRCP: %1 ===> PASS").arg(sSRCPVer), INFO,
                           HIST);
                /* first is OK, next two readonly ports follow */
                ConnectFeedbackPort();
                ConnectInfoPort();
                LoginIsRunning = false;
            }
            else {
                cmdToDebug(tr
                           ("SRCP: %1 ===> FAILED; spdrs60 requires SRCP >= 0.7.0 and < 0.8.0")
                           .arg(sSRCPVer), INFO, HIST);
                /* close connection */
                SendCommandToSRCPServer("LOGOUT");
            }
        }
        else {
            cmdToDebug(tr("Cannot read welcome message!"), INFO, HIST);
            /*close command port */
            if (CommandSocket->isOpen()) {
                CommandSocket->close();
                CommandSocketConnectionClosed();
            }
        }
        LoginIsRunning = false;
        return;
    }

    /* else: no login but connection close */
    if (CommandSocket->canReadLine()) {
        sWelcome = CommandSocket->readLine();
        /* this seems to be a connection shutdown "passive close" */
        if (sWelcome.length() == 0) {
            if (CommandSocket->isOpen()) {
                CommandSocket->close();
                CommandSocketConnectionClosed();
            }
        }
    }

}


void MainWindow::CommandSocketConnected()
{
    cmdToDebug(tr("Command port connected!"), INFO, HIST);
    CommandPortIsConnected = true;
    updateDaemonMenu();
}


void MainWindow::CommandSocketConnectionClosedByServer()
{
    if (CommandSocket->isOpen()) {
        CommandSocket->close();
    }
    cmdToDebug(tr("Command port closed by foreign host!"), INFO, HIST);
    CommandPortIsConnected = false;
    updateDaemonMenu();
}


void MainWindow::CommandSocketConnectionClosed()
{
    cmdToDebug(tr("Command port closed!"), INFO, HIST);
    CommandPortIsConnected = false;
    updateDaemonMenu();
}


void MainWindow::CommandSocketError(int e)
{
    QString ErrMessage = GetSocketErrorString(e);
    cmdToDebug(tr("Command port: Error number %1 occurred (%2)")
               .arg(e).arg(ErrMessage), INFO, HIST);
}


void MainWindow::FeedbackSocketReadyRead()
{
    QString sInfo = "";
    QString sDebug;
    unsigned int iPortNr, iState, uiModule = 0, uiPort = 0;

    while (FeedbackSocket->canReadLine()) {
        sInfo = FeedbackSocket->readLine();

        /*INFO FB <module_type> <portnr> <state> */
        /* 0   1       2           3        4   : Qstring sections */

        if (sInfo.contains("-", 0)) {   // error-code
            cmdToDebug(sInfo, INFO, FEED);
            return;
        }

        iPortNr = sInfo.section(" ", 3, 3).toInt();
        iPortNr--;

        iState = sInfo.section(" ", 4, 4).toInt();

        if (iPortNr < MAX_FB)   //just for case 
            bFBport[iPortNr] = iState;

        /* switch on port change messages after initialization */
        if (isFBInitMode) {
            if (iPortNr == MAX_FB - 1)
                isFBInitMode = false;
        }
        else {
            if (FEEDBACK == FB_16) {
                uiModule = iPortNr >> 4;
                uiModule++;
                uiPort = iPortNr % 16;
                uiPort++;
            }
            else if (FEEDBACK == FB_8) {
                uiModule = iPortNr >> 3;
                uiModule++;
                uiPort = iPortNr % 8;
                uiPort++;
            }
            sDebug = QString(tr("Feedback port change: M %1 / P %2 = %3"))
                .arg(uiModule, 3, 10)
                .arg(uiPort, 2, 10)
                .arg(iState);
            cmdToDebug(sDebug, CMD, FEED);
        }

        /* should'nt we only send modules which are realy connected? */
        emit sendFBChangeModule(iPortNr);       // send updates to module window
        emit sendFBChangeLayout(iPortNr);       // send updates to all elements via gbs
    }
}


void MainWindow::FeedbackSocketConnected()
{
    cmdToDebug(tr("Feedback port connected!"), INFO, HIST);
    FeedbackPortIsConnected = true;
    isFBInitMode = true;        // flag to avoid all startup feedback
    updateFeedbackMenu();

    // on startup init ports
    for (int i = 0; i < MAX_FB; i++)
        bFBport[i] = 0;

    /*may be this makes only sense when a layout is loaded: */
    SendCommandToSRCPServer((FEEDBACK <=
                             1) ? "INIT FB S88" : "INIT FB I8255");
    cmdToDebug(tr
               ("Feedback port changes are omitted while initialization"),
               INFO, FEED);

    /* now hopefully all ports are connected an we can start sending
     * layout depending commands; hint: layout may not be loaded at this
     * time */
    /*bRunLayout is inverted in this procedure: */
    bRunLayout = !AUTO_ZP9;
    slotToggleLayoutPower();

    /* load defaultlayout, but only with "auto power on" is enabled and
     * the layout is not yet loaded; else see "MainWindow" constructor */
    /*
       if (LOAD_DEF_LAYOUT && AUTO_ZP9 && !DefaultLayoutIsLoaded)
       {
       slotLoadTimerTimeout();
       DefaultLayoutIsLoaded = true;
       }
     */
}


void MainWindow::FeedbackSocketConnectionClosedByServer()
{
    if (FeedbackSocket->isOpen()) {
        FeedbackSocket->close();
    }
    cmdToDebug(tr("Feedback port closed by foreign host!"), INFO, HIST);
    FeedbackPortIsConnected = false;
    updateFeedbackMenu();
}


void MainWindow::FeedbackSocketConnectionClosed()
{
    cmdToDebug(tr("Feedback port closed!"), INFO, HIST);
    FeedbackPortIsConnected = false;
    updateFeedbackMenu();
}


void MainWindow::FeedbackSocketError(int e)
{
    QString ErrMessage = GetSocketErrorString(e);
    cmdToDebug(tr("Feedback port: Error number %1 occurred (%2)")
               .arg(e).arg(ErrMessage), INFO, HIST);
}


void MainWindow::InfoSocketReadyRead()
{
    QString sInfo = "";

    if (InfoSocket->canReadLine()) {
        sInfo = InfoSocket->readLine();
        cmdToDebug(sInfo, CMD, INFO);
    }
}


void MainWindow::InfoSocketConnected()
{
    cmdToDebug(tr("Info port connected!"), INFO, HIST);
    InfoPortIsConnected = true;
}


void MainWindow::InfoSocketConnectionClosedByServer()
{
    if (InfoSocket->isOpen()) {
        InfoSocket->close();
    }
    cmdToDebug(tr("Info port closed by foreign host!"), INFO, HIST);
    InfoPortIsConnected = false;
}


void MainWindow::InfoSocketConnectionClosed()
{
    cmdToDebug(tr("Info port closed!"), INFO, HIST);
    InfoPortIsConnected = false;
}


void MainWindow::InfoSocketError(int e)
{
    QString ErrMessage = GetSocketErrorString(e);
    cmdToDebug(tr("Info port: Error number %1 occurred (%2)")
               .arg(e).arg(ErrMessage), INFO, HIST);
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
    LoginIsRunning = true;
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
    CommandSocket->connectToHost(HOST, PORT);   /*e.g.: 12345 */
}


void MainWindow::ConnectFeedbackPort()
{
    FeedbackSocket->connectToHost(HOST, PORT + 1);      /*e.g.: 12366 */
}


void MainWindow::ConnectInfoPort()
{
    InfoSocket->connectToHost(HOST, PORT + 2);  /*e.g.: 12367 */
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
        else {
            // The socket is closed.
            FeedbackSocketConnectionClosed();
        }
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
        cmdToDebug(CommandStr, CMD, HIST);
    }
}
/* End of new Networking code */


void MainWindow::slotToggleLayoutPower()
{
    bRunLayout = !bRunLayout;   // toggle var and setup toolbuttons
    tbLayoutStart->setEnabled(!bRunLayout);     // related to daemon state
    tbLayoutStop->setEnabled(bRunLayout);

    SendCommandToSRCPServer(bRunLayout ==
                            true ? "SET POWER ON" : "SET POWER OFF");
    // if daemon started set menuitem to "stop" cause this's the only
    // next thing you can choose
    layoutmenu->changeItem(bRunLayout ==
                           true ? tr("&Stop power") : tr("&Start power"),
                           LAYOUT_ID_START);
}


void MainWindow::slotResetDaemon()
{
    SendCommandToSRCPServer("RESET");   // resets the daemon
}


void MainWindow::slotKillDaemon()
{
    int choice = QMessageBox::warning(this, tr("Kill SRCP daemon"),
                     tr("You are about to kill the daemon forever.\n\n"
                        "If you really want to do it, click \"Kill\".\n\n"
                        "(Note: if you plan to use this program again\n"
                        "please restart daemon first, then this program)."),
                        tr("&Kill"), tr("&Go back"), 0, 1, 1);
    // ??, defaultButt, EscapeButt
    if (choice == 1)            // do not kill daemon -> return
        return;

    // user decided to kill daemon, so kill it immediately
    SendCommandToSRCPServer("SHUTDOWN");        // kills the daemon
    cmdToDebug(tr
               ("Daemon has been killed. Restart server, "
                "then reconnect \"SpDrS60 for Linux\""),
               INFO, HIST);

    CloseSRCPServerConnection();
}


void MainWindow::slotAboutDaemon()
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
        bRunLayout = false;

    // disable all daemon related menus and toolbuttons if the daemon is
    // not running or has been killed
    tbLayoutStart->setEnabled(CommandPortIsConnected);
    tbLayoutStop->setEnabled(bRunLayout);

    layoutmenu->setItemEnabled(LAYOUT_ID_START, CommandPortIsConnected);
    daemonmenu->setItemEnabled(DAEMON_ID_RESET, CommandPortIsConnected);
    daemonmenu->setItemEnabled(DAEMON_ID_KILL, CommandPortIsConnected);
    daemonmenu->setItemEnabled(DAEMON_ID_INFO, CommandPortIsConnected);
    daemonmenu->setItemEnabled(DAEMON_ID_CONNECT, !CommandPortIsConnected);
    daemonmenu->setItemEnabled(DAEMON_ID_DISCONNECT,
                               CommandPortIsConnected);
    viewmenu->setItemEnabled(VIEW_ID_KEYB, CommandPortIsConnected);
    layoutmenu->setItemEnabled(LAYOUT_ID_TOGGLE, CommandPortIsConnected);
    layoutmenu->setItemEnabled(LAYOUT_ID_SEND, CommandPortIsConnected);
    layoutmenu->setItemEnabled(LAYOUT_ID_NOTROT, CommandPortIsConnected);
}


void MainWindow::updateFeedbackMenu()
{
    tbViewFeedb->setEnabled(FeedbackPortIsConnected);
    viewmenu->setItemEnabled(VIEW_ID_FBMOD, FeedbackPortIsConnected);
}


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
    QMessageBox::aboutQt(this, tr("About Qt")); // self-explaining
}


void MainWindow::slotRoutesDialog()
{
    emit showRoutings();        // notifies "gbs" to show the routing table
}


void MainWindow::slotEditGBSFiles()
{
    // edits the layout file with editor prog
    QString sCommand = EDITOR;
    sCommand.append(" " + FILENAME + GBS_FILE_SUFFIX + (" &"));
    system(sCommand.data());
    tbFileSave->setEnabled(true);
    //serd: after editing layout must be saveable ist hier wirkungslos?!
}


void MainWindow::slotEditRTSFiles()
{
    // edits the routing file with editor prog
    QString sCommand = EDITOR;
    sCommand.append(" " + FILENAME + RTS_FILE_SUFFIX + (" &"));
    system(sCommand.data());
}


void MainWindow::slotEditConfigFile()
{
    // edits the program´s config file with editor prog
    QString sCommand = EDITOR + " " + QDir::homeDirPath() + "/" +
        SPDRS60_INIT + (" &");
    system(sCommand.data());
}


void MainWindow::slotEditLayout()
{
    if (iEditMode == NOEDIT) {
        cmdToDebug(tr("Entering edit mode"), INFO, HIST);
        // wenn das Layout einmel im Editiermodus war, gilt es als modifiziert
        gbs->setModified(true);
    }
    else
        cmdToDebug(tr("Leaving edit mode"), INFO, HIST);

    iEditMode = !iEditMode;
    layoutmenu->setItemChecked(VIEW_ID_EDITMODE, iEditMode);

    tbFileOpen->setEnabled(!iEditMode); // change edit related menus
    tbFileNew->setEnabled(!iEditMode);
    filemenu->setItemEnabled(FILE_ID_NEW, !iEditMode);
    filemenu->setItemEnabled(FILE_ID_OPEN, !iEditMode);

    emit EditMode(iEditMode);   // send emit mode to all elements
}


void MainWindow::slotHelp()
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

    /* bei mozilla und firefox auf schon laufende instanz prüfen, */
    /* dann mit "-remote" Option starten */

    if (BROWSER == "mozilla" || BROWSER == "firefox") {
        if (system(BROWSER + " -remote 'ping()'") == 0)
            system(BROWSER + " -remote 'openURL(" + sURL + ",new-tab)'");
        else
            system(BROWSER + " " + sURL + " &");
    }
    else
        system(BROWSER + " " + sURL + " &");
}


// opens SpDrS60 web resources
void MainWindow::slotWeb()
{    
    QString sURL = QString("http://www.linux-modellbahn.de/");

    if (BROWSER == "mozilla" || BROWSER == "firefox") {
        if (system(BROWSER + " -remote 'ping()'") == 0)
            system(BROWSER + " -remote 'openURL(" + sURL + ",new-tab)'");
        else
            system(BROWSER + " " + sURL + " &");
    }
    else
        system(BROWSER + " " + sURL + " &");
}

// shows an original DB clock
// with minute delay
void MainWindow::slotShowClock()
{
    QString sCommand = "centralclock &";
    system(sCommand.data());    
}                               


void MainWindow::slotShowModules()
{
    modulesWindow = new feedback(this); // show feedback module window
    connect(this, SIGNAL(sendFBChangeModule(unsigned int)),
            modulesWindow, SLOT(slotUpdateModules(unsigned int)));
    modulesWindow->show();
}


void MainWindow::slotKeyboard()
{
    keybWindow = new keyboard(this);    // show a simple keyboard
    connect(keybWindow, SIGNAL(sendCommand(const QString&)),
            this, SLOT(SendCommandToSRCPServer(const QString&)));

    keybWindow->show();
    keybWindow->move(QCursor::pos());
}


void MainWindow::cmdToDebug(const QString& debugCommand_, int mode_,
                            int type_)
{
    QString t;

    QTime cmdTime = QTime::currentTime();
    t.sprintf("%02d:%02d:%02d ", cmdTime.hour(), cmdTime.minute(),
              cmdTime.second());
    if (mode_ == INFO)
        t.append("> ");         // == an info line
    else
        t.append("# ");         // == a command

    t.append(debugCommand_);
    switch (type_) {            // delete oldest entries and add newer ones
    case HIST:
        if (HistCB->count() == MAX_HISTORY)
            HistCB->removeItem(0);
        HistCB->insertItem(t);
        HistCB->setCurrentItem(HistCB->count() - 1);
        break;
    case INFO:
        if (InfoCB->count() == MAX_HISTORY)
            InfoCB->removeItem(0);
        InfoCB->insertItem(t);
        InfoCB->setCurrentItem(InfoCB->count() - 1);
        break;
    case FEED:
        if (FeedBackCB->count() == MAX_HISTORY)
            FeedBackCB->removeItem(0);
        FeedBackCB->insertItem(t);
        FeedBackCB->setCurrentItem(FeedBackCB->count() - 1);
        break;
    }
}


void MainWindow::slotCmdToDebugExtern(const QString& sDebugCommand_)
{
    QString sPrefix = sDebugCommand_.left(1);
    // erst Prefix entfernen, dann wieder hinzufuegen, scheint erstmal
    // unnütz zu sein aber da cmdToDebug() nur mit Argumenten aufzurufen
    // ist, muss das so passieren
    // ansonsten könnte man den übergebenen String einfach ausgeben
    QString s = sDebugCommand_.right(sDebugCommand_.length() - 1);

    if (sPrefix == ">")         // info line
        cmdToDebug(s, INFO, HIST);
    else if (sPrefix == "#")    // command line
        cmdToDebug(s, CMD, HIST);
}


void MainWindow::slotShowOptions()
{
    // open dialog window with program options
    optionsWindow = new optionsDialog(this);
    connect(optionsWindow, SIGNAL(repaintLayout()),
            gbs, SIGNAL(sigRepaintLayout()));
    connect(optionsWindow, SIGNAL(refreshConfigData()),
            this, SLOT(slotReadConfigFile()));
    connect(optionsWindow, SIGNAL(cmdToDebug(const QString&)),
            this, SLOT(slotCmdToDebugExtern(const QString&)));
    optionsWindow->show();
}


void MainWindow::slotNewWin()
{
    // creates a new instance of this program
    /*
       QString sCommand = "spdrs60 &";
       system(sCommand);
     */
    // gleicher Adressraum, also labiler (aber schneller):
    /* problematisch ist die "autoload"-Funktion */
    MainWindow *sw = new MainWindow;
    sw->resize(640, 480);
    sw->show();

}


void MainWindow::slotFind()
{
    findWindow = new Finder();  // create a new locator window
    // connect its signals directly to gbs
    connect(findWindow, SIGNAL(sigFind(QString, int, bool)),
            gbs, SLOT(slotFind(QString, int, bool)));
    findWindow->exec();
}


void MainWindow::setFilename()  //serd
{
    gbs->FILENAME = FILENAME;
}
