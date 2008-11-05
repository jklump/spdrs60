/***************************************************************************
                           routeelementdialog.h
                           version 0.5.3 $Revision: 1.7 $
                           -------------------------------
    copyright            : (C) 2005-2008 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-11-05 08:42:40 $
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
   this is the header file to routeelementdialog.cpp
 ***************************************************************************/
#ifndef ROUTEELEMENTDIALOG_H
#define ROUTEELEMENTDIALOG_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qstring.h>

#include "element.h"


class RouteElementDialog: public QDialog
{
    Q_OBJECT

public:
    RouteElementDialog(QWidget* parent = 0);
    void setStateElementData(const stateElement*);
    void getStateElementData(stateElement*);
  
private slots:
    void reBusChanged(const QString&);
    void reAddressChanged(const QString&);

signals:
    void getElementByAddress(const int, const int, element**);

private:
   element* rePtr1;
    
   QLineEdit* reNameLE;
   QLineEdit* reSrcpBusLE;
   QLineEdit* reAddressLE;
   QSpinBox*  reStateSB;
   
   void updateRouteElementName(int, int);
};

#endif    //ROUTEELEMENTDIALOG_H
