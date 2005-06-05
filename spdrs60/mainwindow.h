/***************************************************************************
                           mainwindow.h
                           version 0.4.8 $Revision: 1.9 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
    copyright            : (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-06-05 13:04:19 $
****************************************************************************/

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

#include <qaction.h>
#include <qapplication.h>
#include <qmainwindow.h>
#include <qfiledialog.h>
#include <qkeycode.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qscrollview.h>
#include <qsocket.h>
#include <qwidgetstack.h>

#include "gbsarea.h"
#include "routingviewer.h"
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
   void openFile(const QString&);

private:
   bool            bRunLayout;
   bool            LoginIsRunning;
   bool            isFBInitMode;
   int             iDebugNo;
   QString         fileName;  //serd
   QString         lastDir;
   elemVisualMode  visualMode;

   QAction         *actionFileNew;
   QAction         *actionFileOpen;
   QAction         *actionFileSave;
   QAction         *actionFileSaveAs;
   QAction         *actionFileImport;
   QAction         *actionFileNewWindow;
   QAction         *actionFileClose;
   QAction         *actionFileQuit;
   
   QAction         *actionEditCut;
   QAction         *actionEditCopy;
   QAction         *actionEditPaste;
   QAction         *actionEditFind;
   QAction         *actionEditOptions;
   QAction         *actionEditFileLayout;
   QAction         *actionEditFileOptions;
   
   QAction         *actionViewRoutes;
   QAction         *actionViewFBModules;
   QAction         *actionViewClock;
   QAction         *actionViewKeyboard;
   QAction         *actionViewToggleHistory;
   QAction         *actionViewNormalMode;
   QAction         *actionViewLayoutEditMode;
   QAction         *actionViewRouteEditMode;
   
   QAction         *actionDaemonConnect;
   QAction         *actionDaemonDisconnect;
   QAction         *actionDaemonReset;
   QAction         *actionDaemonKill;
   QAction         *actionDaemonInfo;
   
   QAction         *actionLayoutPower;
   QAction         *actionLayoutFht;
   QAction         *actionLayoutWgt;
   QAction         *actionLayoutSgt;
   QAction         *actionLayoutUfgt;
   QAction         *actionLayoutNotRot;
   QAction         *actionLayoutToggleAll;
   QAction         *actionLayoutSendAll;
   QAction         *actionLayoutChangeSize;

   QAction         *actionRouteStart;
   QAction         *actionRouteStop;
   QAction         *actionRouteAdd;
   QAction         *actionRouteEdit;
   QAction         *actionRouteCopy;
   QAction         *actionRouteClear;
   QAction         *actionRouteUnlockAll;
   
   QString         sWelcome;

   QWidgetStack    *cbStack;
   QWidgetStack    *lblStack;
   QComboBox       *HistCB;
   QComboBox       *InfoCB;
   QComboBox       *FeedBackCB;

   GBSArea         *gbs;
   RoutingViewer   *rtViewer;
   Router          *rtController;
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

   void initMainWindow();
   void updateDaemonMenu();
   void updateFeedbackMenu();
   void importFile(const QString&);
   void resetMenu();           //dirk
   bool saveFile();
   void newFile();
   void chooseFile();
   void chooseImportFile();
   int querySaveChanges();
   bool isModified();

   /* New Networking code: */
   void initAllSockets();
   void ConnectCommandPort();
   void ConnectFeedbackPort();
   void ConnectInfoPort();
   bool isValidSRCPVersion(const QString&);
   QString GetSocketErrorString(int e);

public slots:
   void cmdToDebug(const QString&, int, int);
   void updateRouteMenu(bool);
   void updateRouteMenuActivateItems(bool);

private slots:
   void slotAbout();
   void slotAboutHelp();
   void slotAboutQt();
   void slotAboutWeb();
   void slotEditConfigFile();
   void slotEditCopy();
   void slotEditCut();
   void slotEditFind();
   void slotEditGBSFiles();
   void slotEditPaste();
   void slotEditOptions();
   void slotFileNew();
   void slotFileNewWin();
   void slotFileOpen();
   void slotFileSave();
   void slotFileSaveAs();
   void slotFileImport();
   void slotReadConfigFile();
   void slotDaemonReset();
   void slotDaemonKill();
   void slotDaemonInfo();
   void slotShowClock();
   void slotShowModules();
   void slotShowRoutes();
   void slotToggleLayoutPower();
   void slotUpdateEditmenu();
   void slotViewDebug();
   void slotViewKeyboard();
   void slotViewSwitchMode(QAction*);
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
   void switchedVisualMode(elemVisualMode);
   void sendFBChangeLayout(unsigned int);
   void sendFBChangeModule(unsigned int);
   void sendFBChangeRoute(unsigned int);

protected:
   virtual void closeEvent(QCloseEvent* ce);
};

#endif  //MAINWINDOW_H

