/***************************************************************************
                           routedialog.h
                           version 0.4.7 $Revision: 1.5 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-06-16 21:04:35 $
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
#include <qdialog.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qstring.h>
#include <qspinbox.h>
#include <qtooltip.h>

#include "resources.h"
#include "element.h"


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
   QButtonGroup *bgRouteType;
   QPixmap      *pix;

   QRadioButton *rbRouteNormal;
   QRadioButton *rbRouteDetour;
   QRadioButton *rbRouteHelp;
   QRadioButton *rbRouteShuntg;
   QRadioButton *rbRouteDShuntg;

   QLineEdit*    routeNameLE;

   QLineEdit*    startSignalNameLE;
   QLineEdit*    startSignalSrcpBusLE;
   QLineEdit*    startSignalAddressLE;
   QLineEdit*    startSignalStateLE;
   
   QLineEdit*    stopSignalNameLE;
   QLineEdit*    stopSignalSrcpBusLE;
   QLineEdit*    stopSignalAddressLE;
   
   QLineEdit*    activateSrcpBusLE;
   QLineEdit*    activateContactLE;
   QLineEdit*    activateModuleLE;
   QLineEdit*    activatePortLE;

   QLineEdit*    releaseSrcpBusLE;
   QLineEdit*    releaseContactLE;
   QLineEdit*    releaseModuleLE;
   QLineEdit*    releasePortLE;

   QButtonGroup* typeGB;
};

#endif    //ROUTEDIALOG_H
