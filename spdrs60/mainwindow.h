/***************************************************************************
                           mainwindow.h
                           version 0.5.2 $Revision: 1.58 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
    copyright            : (C) 2004-2014 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2009-03-18 17:14:03 $
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
#if QT_VERSION >= 0x040000
#include <QCloseEvent>
#include <q3mainwindow.h>
#include <q3filedialog.h>
#include <q3popupmenu.h>
#include <q3scrollview.h>
#include <q3socket.h>
#include <q3widgetstack.h>
#else
#include <qmainwindow.h>
#include <qfiledialog.h>
#include <qnamespace.h>
#include <qpopupmenu.h>
#include <qscrollview.h>
#include <qsocket.h>
#include <qwidgetstack.h>
#endif
#include <qmessagebox.h>
#include <qpixmap.h>

#include "commandport.h"
#include "crcfmessage.h"
#include "srcpmessage.h"
#include "spdrpanel.h"


// forward declarations to reduce compile time after code changes
class CommandPort;
class CrcfMessage;
class FeedbackViewer;
class GBSArea;
class InfoPort;
class keyboard;
class MessageHistory;
class PaintItemWindow;
class RouteListWindow;
class Router;
class TrainNumberDialog;
class QToolBar;


enum SRCPMode {    
    srcpUndefined = 0,
    srcpConnected,
    srcp07GetFBStates,
    srcp07GetPower,
    srcp08GetBusPower,
    srcp08InitPower,
    srcp08InitGADevices,
    srcp08RunInfoMode,
    srcp08ServerError,
    srcp08SetBusPower,
    srcp08GoCommandMode,
    srcp08TermServer
};


class MainWindow: public QMainWindow
{
   Q_OBJECT

public:
   MainWindow();
   ~MainWindow();
   void readAutoloadFile();
   void openFile(const QString&);
   void openFileWindow(const QString&);

private:
   QAction *actionFileNew;
   QAction *actionFileOpen;
   QAction *actionFileSave;
   QAction *actionFileSaveAs;
   QAction *actionFileNewWindow;
   QAction *actionFileClose;
   QAction *actionFileQuit;
   
   QAction *actionEditCut;
   QAction *actionEditCopy;
   QAction *actionEditPaste;
   QAction *actionEditFind;
   QAction *actionEditOptions;
   QAction *actionEditFileLayout;
   QAction *actionEditFileOptions;
   
   QAction *actionViewRoutes;
   QAction *actionViewFBModules;
   QAction *actionViewClock;
   QAction *actionViewKeyboard;
   QAction *actionViewTrainNumberDialog;
   QAction *actionViewToggleHistory;
   QAction *actionViewNormalMode;
   QAction *actionViewLayoutEditMode;
   QAction *actionViewRouteEditMode;
   QAction *actionViewToggleFullScreen;
   QAction *actionViewMenu;
   QAction *actionViewToolbar;
   
   QAction *actionDaemonConnect;
   QAction *actionDaemonDisconnect;
   QAction *actionDaemonReset;
   QAction *actionDaemonKill;
   QAction *actionDaemonInfo;
   
   QAction *actionLayoutPower;
   QAction *actionLayoutFht;
   QAction *actionLayoutHagt;
   QAction *actionLayoutWgt;
   QAction *actionLayoutSgt;
   QAction *actionLayoutUfgt;
   QAction *actionLayoutNotRot;
   QAction *actionLayoutToggleAll;
   QAction *actionLayoutSendAll;
   QAction *actionLayoutUpdateFB;
   QAction *actionLayoutChangeSize;

   QAction *actionRouteActivate;
   QAction *actionRouteWithdraw;
   QAction *actionRouteRelease;
   QAction *actionRouteAdd;
   QAction *actionRouteEdit;
   QAction *actionRouteCopy;
   QAction *actionRouteDelete;
   QAction *actionRouteUnlockAll;

   QToolBar* layoutedittb;
   QToolBar* daemontb;
   QToolBar* layouttb;

   QPopupMenu* fileRecentlyOpenedFiles;
   QStringList recentFiles;
   elemVisualMode  visualMode;
   bool LayoutPowerIsOn;
   bool isFBInitMode;
   QString fileName;
   QString lastDir;

   // CRCF data
   unsigned int rwccid;
   QString rwccname;

   SrcpPort::CommunicationStyle infoStyle;
   SrcpPort::CommunicationStyle commandStyle;

   CommandPort* commandPort;
   InfoPort* infoPort;
   InfoPort* feedbackPort;
   GBSArea* gbs;
   MessageHistory* messageHistory;
   FeedbackViewer* fbViewer;
   RouteListWindow* rtViewer;
   PaintItemWindow* piw;
   Router* router;
   keyboard* keybWindow;
   TrainNumberDialog* trainnumberdialog;

   bool cmdAutoLogin;
   bool cmdAutoPower;
   bool cmdAutoSendAll;
   SRCPMode SRCPCommandState;
   SRCPMode SRCPInfoState;

   void initMainWindow();
   void updateDaemonMenu();
   void updateLayoutPowerAction();
   void importFile(const QString&);
   void resetMenu();
   bool saveFile();
   void newFile();
   void chooseFile();
   int querySaveChanges();
   bool isModified();
   bool isSave();
   void writeConfigFile();
   void readConfigFile();
   void runBrowserUrl(const QString&);
   void addRecentlyOpenedFile(const QString&, QStringList&);
   void processGenericMessage(unsigned int, unsigned int,
            const CrcfMessage*);
   QString getCrcfInfoMessage(CrcfMessage::CrcfAttribute) const;
   void sendGmCrcfMessage(unsigned int, unsigned int, const QString&);
   void switchToNormalMode();
   void switchToEditLayoutMode();
   void switchToEditRouteMode();
   void propagateViewModeSwitch();

   /* New Networking code: */
   void ConnectCommandPort();

   /* SRCP event loop */
   void runGaInitSequence();
   void runPowerInitSequence();
   void runGetBusPowerSequence();
   void runSetBusPowerSequence();

public slots:
   void updateRouteListMenuItems();
   void updateRouteMenu(bool);
   void updateRouteMenuActivateItems();

private slots:
   void slotAbout();
   void slotAboutHelp();
   void slotAboutQt();
   void slotAboutWeb();
   void slotEditConfigFile();
   void slotEditCopy();
   void slotEditCut();
   void slotEditFind();
   void slotEditLayoutFile();
   void slotEditPaste();
   void slotEditOptions();
   void slotFileNew();
   void slotFileNewWin();
   void slotFileOpen();
   void slotFileSave();
   void slotFileSaveAs();
   void slotDaemonReset();
   void slotDaemonKill();
   void slotDaemonInfo();
   void slotRouteAdd();
   void slotRouteDelete();
   void slotShowClock();
   void slotShowModules();
   void slotShowRoutes();
   void slotToggleLayoutPower();
   void slotViewKeyboard();
   void slotViewTrainNumberDialog();
   void slotViewSwitchMode(QAction*);
   void slotViewToggleFullscreen();
   void layoutChangeSize();
   void layoutUpdateFB();
   void layoutSendAll();
   void updateCaption();
   void updateFileMenuItems();
   void saveKeyboardProtocol(int);
   void recentFileActivated(int);
   void setupRecentFilesMenu();
   /* New Networking code: */
   void connectToSrcpServer();
   void closeSrcpServerConnection();
   void sendSrcpMessage(SrcpMessage*);
   void processCommandMessage(const QString&);
   void processFeedbackMessage(const QString&);
   void processInfoMessage(const QString&);
   void updateCommandConnectionState(bool);
   void updateInfoConnectionState(bool);
   void updateFeedbackConnectionState(bool);

signals:
   void findElement(const QString&, int, int);
   void repaintLayout();
   void sendFBChangeLayout(unsigned int, unsigned int, bool);
   void sendFBBiDiChangeLayout(unsigned int, unsigned int, bool,
           unsigned int);
   void sendFBChangeModule(unsigned int, unsigned int, unsigned int);
   void sendFBChangeRoute(unsigned int, unsigned int, bool);
   void statusMessage(const QString&);
   void switchedVisualMode(elemVisualMode);

protected:
   void closeEvent(QCloseEvent*);
};

#endif  //MAINWINDOW_H

