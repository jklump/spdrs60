/***************************************************************************
                           RouteDialog.h
                           version 0.4.7 $Revision: 1.2 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-05-07 12:22:44 $
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
   this is the header file to RouteDialog.cpp
 ***************************************************************************/
#ifndef ROUTEDIALOG_H
#define ROUTEDIALOG_H

#include <qaccel.h>
#include <qbuttongroup.h>
#include <qdatetime.h>
#include <qdialog.h>
#include <qframe.h>
#include <qfile.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qstring.h>
#include <qstrlist.h>
#include <qspinbox.h>
#include <qtextstream.h>
#include <qtooltip.h>

#include "resources.h"

#define MAX_SW_ELEM 15   // max no of switched solenoids in a route

#define NAMES       0    // only read the names of routes
#define DATA        1    // read data of one certain route

#define NOTITLE     0    // clear data area and show no new title
#define TITLE       1    // clear data area with a new title

#define ADDCHANGE   0    // write routing file with a changed or new route
#define DELETE      1    // write routing file and delete a route

enum RouteType {
    kNormal = 0,
    kDetour,
    kHelp,
    kShunting,
    kShuntingD};

class RouteDialog: public QDialog
{
   Q_OBJECT

public:
   RouteDialog(QWidget* pParent = NULL, QStrList* listOfLockedRoutes_ = NULL);
   virtual ~RouteDialog();
  
private:
   void setupRoutingTable();
   void setupProgArea();
   void setRouteTableTitle();
   void readRoutingFile(bool);
   void fillProgArea(int);
   void fillProgAreaEmpty(bool);
   void enableProgArea(int, int);
   int  readProgFields(bool);
   void stopRecord();
   void createRoutingFile();
   QString addZeros(QString);
   QWidget *parent;

public slots:
   void slotUpdateRouteWindow();
   void slotRecordElement(int, QString, int, int);

private slots:
   void slotSendRouteIndex();
   void slotEnableRouteButton(int);
   void slotResizeCommander(bool);
   void slotSaveRouteType(int);
   void slotSave();
   void slotSaveRoute(bool);
   void slotDelRoute();
   void slotNewRoute();
   void slotCopyRoute();
   void slotSomethingChanged();
   void slotValueChanged(int);
   void slotTextChanged(const QString&);
   void slotAddressChanged(const QString&);
   void slotStatusChanged(const QString&);
   void slotDisableRelPort(int);
   void slotDisableActPort(int);
   void slotRecordRoute();
   void slotShowRoute();

signals:
   void sendRouteIndex(int, int);
   void sendReloadRoutes();
   void cmdToDebug(const QString&);
   void sigRecord(int);
   void sigShowElement(int, int, int);
   void sigReadElemName(QString);

protected:
   virtual void closeEvent(QCloseEvent*);

public:
   QString      sReadElemName;


private:
   QPushButton  *buttStartRouting;
   QPushButton  *buttStoppRouting;
   QPushButton  *buttSetup;
   QPushButton  *buttApply;
   QPushButton  *buttDel;
   QPushButton  *buttNew;
   QPushButton  *buttCopy;
   QPushButton  *buttRecord;
   QPushButton  *buttShow;

   QStrList     *listOfLockedRoutes;
   QStrList     *listOfRoutes;
   QListBox     *lbRouteTable;

   QGroupBox    *groupRoutes;
   QGroupBox    *groupActivate;
   QGroupBox    *groupStartStop;
   QGroupBox    *groupElements;

   QLabel       *lRouteTitle;
   QLabel       *lElem[MAX_SW_ELEM];
   QLabel       *lStart;
   QLabel       *lStopp;
   QLabel       *lRel;
   QLabel       *lAct;
   QLabel       *lAdd;
   QLabel       *lMod;
   QLabel       *lPort;
   QLabel       *lName;
   QLabel       *lAddr;
   QLabel       *lStat;
   QLabel       *lNo;
   QLabel       *lName2;
   QLabel       *lAddr2;
   QLabel       *lStat2;
   QLabel       *lStartName;
   QLabel       *lStoppName;
   QLabel       *lElemName[MAX_SW_ELEM];

   QButtonGroup *bgRouteType;
   QPixmap      *pix;

   QRadioButton *rbRouteNormal;
   QRadioButton *rbRouteDetour;
   QRadioButton *rbRouteHelp;
   QRadioButton *rbRouteShuntg;
   QRadioButton *rbRouteDShuntg;

   QLineEdit    *leRouteName;
   QLineEdit    *leStartAddr;
   QLineEdit    *leStoppAddr;
   QLineEdit    *leStartStat;
   QLineEdit    *leLocoAddr;
   QLineEdit    *leElemAddr[MAX_SW_ELEM];
   QLineEdit    *leElemStat[MAX_SW_ELEM];

   QSpinBox     *sbRelMod;
   QSpinBox     *sbRelPort;
   QSpinBox     *sbActMod;
   QSpinBox     *sbActPort;
   QSpinBox     *sbDetourLevel;
   QSpinBox     *sbShDetourLevel;

   bool         bHasChanged;
   bool         bBlockSignals;
   bool         bLockTable;

   // routing array variables
   int          iRouteNumber;
   int          iRouteTotal;
   RouteType    tRouteType;
   int          iRouteRelPort;
   int          iRouteActPort;
   int          iRouteLocoPort;
   int          iRouteLevel;
   QString      sRouteName;
   QString      sRouteLoco;
   QString      sRouteStart;
   QString      sRouteStopp;
   QString      sRouteStartStat;
   QString      sRouteElem[MAX_SW_ELEM];
   QString      sRouteElemStat[MAX_SW_ELEM];
};

#endif    //ROUTEDIALOG_H
