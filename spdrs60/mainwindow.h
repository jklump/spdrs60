/***************************************************************************
                           mainwindow.h
                           version 0.4.6
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
    email                : stefan.preis@wdr.de
    last modified        : 2004-12-31
***************************************************************************/
/********************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
   this file is the header file to mainwindow.cpp
 ***************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qapplication.h>
#include <qmainwindow.h>
#include <qfiledialog.h>
#include <qkeycode.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qscrollview.h>
#include <qsocket.h>
#include <qsocketnotifier.h>
/*#include <qstatusbar.h>*/
#include <qtimer.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include "qwidgetstack.h"

#include "gbsarea.h"
#include "newlayoutdialog.h"
#include "feedback.h"
#include "options.h"
#include "keyboard.h"
#include "finder.h"


class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow();
   virtual ~MainWindow();

private:
   void initMainWindow();
   void updateDaemonMenu();
   void updateFeedbackMenu();
   void cmdToDebug(const QString&, int, int);
   void LoadFile();
   void setFilename();
   void resetMenu();           //dirk

   /* New Networking code: */
   void initAllSockets();
   void ConnectCommandPort();
   void ConnectFeedbackPort();
   void ConnectInfoPort();
   bool isValidSRCPVersion(const QString&);
   QString GetSocketErrorString(int e);


private slots:
   void slotReadConfigFile();
   void slotAbout();
   void slotAboutQt();
   void slotAboutDaemon();
   void slotLoad();
   void slotUpdateEditmenu();
   int  slotSave();
   void slotSaveAs();
   void slotNew();
   void slotQuit();
   void slotToggleLayoutPower();
   void slotResetDaemon();
   void slotKillDaemon();
   void slotRoutesDialog();
   void slotEditCut();
   void slotEditCopy();
   void slotEditPaste();
   void slotEditGBSFiles();
   void slotEditRTSFiles();
   void slotEditConfigFile();
   void slotHelp();
   void slotWeb();
   void slotShowClock();
   void slotLoadTimerTimeout();
   void slotShowModules();
   void slotShowOptions();
   void slotKeyboard();
   void slotViewDebug();
   void slotCmdToDebugExtern(const QString&);
   void slotEditLayout();
   void slotNewWin();
   void slotFind();
   /* New Networking code: */
   void ConnectToSRCPServer();
   void CloseSRCPServerConnection();
   void SendCommandToSRCPServer(const QString&);
   void CommandSocketHostFound();
   void CommandSocketReadyRead();
   void CommandSocketConnected();
   void CommandSocketConnectionClosed();
   void CommandSocketConnectionClosedByServer();
   void CommandSocketError(int);
   void FeedbackSocketReadyRead();
   void FeedbackSocketConnected();
   void FeedbackSocketConnectionClosed();
   void FeedbackSocketConnectionClosedByServer();
   void FeedbackSocketError(int);
   void InfoSocketReadyRead();
   void InfoSocketConnected();
   void InfoSocketConnectionClosed();
   void InfoSocketConnectionClosedByServer();
   void InfoSocketError(int);

signals:
   void load();
   void save();
   void newLayout(int);
   void showRoutings();
   void FHTclicked();
   void WGTclicked();
   void UfGTclicked();
   void unlockRoutings();
   void toggleAll();
   void sendAll();
   void progressCancelled();
   void sendFBChangeLayout(unsigned int);
   void sendFBChangeModule(unsigned int);
   void repaintLayout();
   void EditMode(int);
   void notrot();

protected:
   virtual void closeEvent (QCloseEvent* ce);

private:
   bool            bRunLayout;
   bool            LoginIsRunning;
   bool            DefaultLayoutIsLoaded;

   QMenuBar        *menubar;
   QPopupMenu      *filemenu;
   QPopupMenu      *editmenu;
   QPopupMenu      *editfilemenu;
   QPopupMenu      *viewmenu;
   QPopupMenu      *daemonmenu;
   QPopupMenu      *layoutmenu;
   QPopupMenu      *helpmenu;

   QToolBar        *toolbar;
   QToolButton     *tbFileNew;
   QToolButton     *tbFileOpen;
   QToolButton     *tbFileSave;
   QToolButton     *tbEditCut;
   QToolButton     *tbEditCopy;
   QToolButton     *tbEditPaste;
   QToolButton     *tbLayoutStart;
   QToolButton     *tbLayoutStop;
   QToolButton     *tbLayoutNotRot;
   QToolButton     *tbViewRoute;
   QToolButton     *tbViewClock;
   QToolButton     *tbViewFeedb;
   QToolButton     *tbViewKeyb;

   QTimer          *loadTimer;
   QScrollView     *scrollview;
   QSocketNotifier *snFBnotify;
   QSocketNotifier *snINnotify;
   QString         sWelcome;

   QWidgetStack    *cbStack;
   QWidgetStack    *lblStack;
   QComboBox       *HistCB;
   QComboBox       *InfoCB;
   QComboBox       *FeedBackCB;

   GBSArea         *gbs;
   feedback        *modulesWindow;
   optionsDialog   *optionsWindow;
   Finder          *findWindow;
   keyboard        *keybWindow;

   /*Networking*/
   QSocket *CommandSocket;
   QSocket *FeedbackSocket;
   QSocket *InfoSocket;
   bool CommandPortIsConnected;
   bool FeedbackPortIsConnected;
   bool InfoPortIsConnected;
   
   int  isFBInitMode;
   int  iDebugNo;
   int  iEditMode;
   QString  FILENAME;  //serd
   QString  lastDir;
};

#endif  //MAINWINDOW_H

