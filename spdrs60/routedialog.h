/***************************************************************************
                           routedialog.h
                           version 0.4.7 $Revision: 1.7 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-06-18 14:33:00 $
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
#include <qlineedit.h>
#include <qstring.h>
#include <qspinbox.h>
#include <qtooltip.h>

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
    void setRouteType(int);
    void setStartSignalData(const stateElement&);
    void setStopSignalData(const stateElement&);
    QString getRouteName();
    int getRouteType();
  
private:
    QString addZeros(const QString&);

public slots:

private slots:

signals:
    void showLogMessage(const QString&, int, int);

protected:

private:
   QLineEdit*    routeNameLE;

   QLineEdit*    startSignalNameLE;
   QLineEdit*    startSignalSrcpBusLE;
   QLineEdit*    startSignalAddressLE;
   QLineEdit*    startSignalStateLE;
   
   QLineEdit*    stopSignalNameLE;
   QLineEdit*    stopSignalSrcpBusLE;
   QLineEdit*    stopSignalAddressLE;
   
   QLineEdit*    activateSrcpBusLE;
   QSpinBox*     activateContactSB;
   QLineEdit*    activateModuleLE;
   QLineEdit*    activatePortLE;

   QLineEdit*    releaseSrcpBusLE;
   QSpinBox*     releaseContactSB;
   QLineEdit*    releaseModuleLE;
   QLineEdit*    releasePortLE;

   QButtonGroup* typeBG;
   QButtonGroup* activateRouteBG;
   QButtonGroup* releaseRouteBG;

   QPushButton*  addPB;
   QPushButton*  removePB;

   QSpinBox*     uzsLevelSB;
   QSpinBox*     ursLevelSB;
};

#endif    //ROUTEDIALOG_H
