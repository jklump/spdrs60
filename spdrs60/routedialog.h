/***************************************************************************
                           RouteDialog.h
                           version 0.4.7 $Revision: 1.4 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-06-15 20:13:04 $
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

#include <qbuttongroup.h>
#include <qdatetime.h>
#include <qdialog.h>
#include <qframe.h>
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
#include <qtooltip.h>

#include "resources.h"
#include "element.h"

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
    RouteDialog(QWidget* parent = 0);
    void setRouteName(const QString&);
    QString getRouteName();
    void setRouteType(int);
    int getRouteType();
  
private:
    QString addZeros(const QString&);

public slots:

private slots:

signals:
    void showLogMessage(const QString&, int, int);

protected:

private:
   QGroupBox    *groupRoutes;
   QGroupBox    *groupActivate;
   QGroupBox    *groupStartStop;
   QGroupBox    *groupElements;

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

   QLineEdit*    routeNameLE;
   QLineEdit*    startSignalNameLE;
   QLineEdit*    startSignalAddressLE;
   QLineEdit*    startSignalStateLE;
   QLineEdit*    stopSignalNameLE;
   QLineEdit*    stopSignalAddressLE;
   QButtonGroup* typeGB;
   
};

#endif    //ROUTEDIALOG_H
