/***************************************************************************
                           mainwindow.h
                           version 0.4.8 $Revision: 1.24 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
    copyright            : (C) 2004-2006 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2006-01-03 22:00:55 $
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


enum SRCPMode {    
    srcpUndefined = 0,
    srcpLogin,
    srcpConnected,
    srcp07GetFBStates,
    srcp07GetPower,
    srcp08GetBusPower,
    srcp08GoCommandMode,
    srcp08GoInfoMode,
    srcp08InitFBBusses,
    srcp08InitGADevices,
    srcp08RunInfoMode,
    srcp08ServerError,
    srcp08SetBusPower,
    srcp08SetConnectionModeCommand,
    srcp08SetConnectionModeInfo,
    srcp08TermServer
};


class MainWindow: public QMainWindow
{
   Q_OBJECT

public:
   MainWindow();
   virtual ~MainWindow();
   void readAutoloadFile();
   void openFile(const QString&);

private:
   bool            LayoutPowerIsOn;
   bool            isFBInitMode;
   int             CurrentHL;
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
   QAction         *actionLayoutUpdateFB;
   QAction         *actionLayoutChangeSize;

   QAction         *actionRouteStart;
   QAction         *actionRouteStop;
   QAction         *actionRouteAdd;
   QAction         *actionRouteEdit;
   QAction         *actionRouteCopy;
   QAction         *actionRouteDelete;
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

   /*SRCP Networking (srcpCom)*/
   QString     cmdHost;
   QString     fbHost;
   int         cmdPort;
   int         fbPort;
   bool        cmdLogin;
   bool        fbLogin;
   QSocket* CommandSocket;
   QSocket* FeedbackSocket;
   QSocket* InfoSocket;
   bool CommandPortIsConnected;
   bool FeedbackPortIsConnected;
   bool InfoPortIsConnected;
   SRCPMode     SRCPCommandState;
   SRCPMode     SRCPInfoState;
   unsigned int srcpVersion;
   int          srcpCommandSessionID;
   int          srcpInfoSessionID;

   void initMainWindow();
   void updateDaemonMenu();
   void updateFeedbackMenu();
   void updateLayoutPowerAction();
   void importFile(const QString&);
   void resetMenu();           //dirk
   bool saveFile();
   void newFile();
   void chooseFile();
   void chooseImportFile();
   int querySaveChanges();
   bool isModified();
   void writeConfigFile();

   /* New Networking code: */
   void initAllSockets();
   void ConnectCommandPort();
   void ConnectFeedbackPort();
   void ConnectInfoPort();
   bool isValidSRCP07Version(const QString&);
   bool isValidSRCP08Version(const QString&);
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
   void slotRouteAdd();
   void slotRouteDelete();
   void slotDaemonReset();
   void slotDaemonKill();
   void slotDaemonInfo();
   void slotShowClock();
   void slotShowModules();
   void slotShowRoutes();
   void slotToggleLayoutPower();
   void slotViewDebug();
   void slotViewKeyboard();
   void slotViewSwitchMode(QAction*);
   void layoutUpdateFB();
   void layoutChangeSize();
   void updateCaption();
   void updateFileMenuItems();
   /* New Networking code: */
   void ConnectToSRCPServer();
   void CloseSRCPServerConnection();
   void SendCommandToSRCPServer(const QString&);
   void SendInfoCommandToSRCPServer(const QString&);
   void sendSrcpMessage(SrcpMessage*);
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
   void sendFBChangeLayout(unsigned int, unsigned int, bool);
   void sendFBChangeModule(unsigned int, unsigned int, unsigned int);
   void sendFBChangeRoute(unsigned int, unsigned int, bool);

protected:
   virtual void closeEvent(QCloseEvent* ce);
};

#endif  //MAINWINDOW_H

