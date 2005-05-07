/***************************************************************************
                           mainwindow.h
                           version 0.4.8 $Revision: 1.2 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
    copyright            : (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-05-07 12:22:43 $
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
#include <qwidgetstack.h>

#include "gbsarea.h"
//#include "routingviewer.h"
//#include "routingtoolbar.h"
#include "newlayoutdialog.h"
#include "feedback.h"
#include "options.h"
#include "keyboard.h"
#include "finder.h"


class MainWindow: public QMainWindow
{
   Q_OBJECT

public:
   MainWindow();
   virtual ~MainWindow();
   void readAutoloadFile();
   QString getFilename();

private:
   void initMainWindow();
   void updateDaemonMenu();
   void updateFeedbackMenu();
   void cmdToDebug(const QString&, int, int);
   void openFile(const QString&);
   void resetMenu();           //dirk
   bool saveFile();
   void newFile();
   void chooseFile();

   /* New Networking code: */
   void initAllSockets();
   void ConnectCommandPort();
   void ConnectFeedbackPort();
   void ConnectInfoPort();
   bool isValidSRCPVersion(const QString&);
   QString GetSocketErrorString(int e);


private slots:
   void slotAbout();
   void slotAboutDaemon();
   void slotAboutHelp();
   void slotAboutQt();
   void slotAboutWeb();
   void slotCmdToDebugExtern(const QString&);
   void slotEditConfigFile();
   void slotEditCopy();
   void slotEditCut();
   void slotEditFind();
   void slotEditGBSFiles();
   void slotEditLayout();
   void slotEditPaste();
   void slotEditRTSFiles();
   void slotFileNew();
   void slotFileNewWin();
   void slotFileOpen();
   void slotFileSave();
   void slotFileSaveAs();
   void slotKeyboard();
   void slotKillDaemon();
   void slotReadConfigFile();
   void slotResetDaemon();
   void slotShowClock();
   void slotShowModules();
   void slotShowOptions();
   void slotShowRoutes();
   void slotToggleLayoutPower();
   void slotUpdateEditmenu();
   void slotViewDebug();
   void layoutChangeSize();
   void updateCaption();
   void updateFileMenuItems();
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
   void EditMode(int);
   void FHTclicked();
   void load();
   void newLayout(int, int);
   void notrot();
   void progressCancelled();
   void repaintLayout();
   void save();
   void sendAll();
   void sendFBChangeLayout(unsigned int);
   void sendFBChangeModule(unsigned int);
   void showRoutings();
   void toggleAll();
   void UfGTclicked();
   void unlockRoutings();
   void WGTclicked();

protected:
   virtual void closeEvent(QCloseEvent* ce);

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
//   RoutingToolBar  *routingtoolbar;
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
//   RoutingViewer   *rtViewer;
   feedback        *modulesWindow;
   optionsDialog   *optionsWindow;
   Finder          *findWindow;
   keyboard        *keybWindow;

   /*Networking*/
   QSocket* CommandSocket;
   QSocket* FeedbackSocket;
   QSocket* InfoSocket;
   bool CommandPortIsConnected;
   bool FeedbackPortIsConnected;
   bool InfoPortIsConnected;
   
   int  isFBInitMode;
   int  iDebugNo;
   int  iEditMode;
   QString  fileName;  //serd
   QString  lastDir;
};

#endif  //MAINWINDOW_H

