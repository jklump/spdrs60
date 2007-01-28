/***************************************************************************
                           routelistwindow.h
                           version 0.5.1 $Revision: 1.1 $
                           -------------------------------
    copyright            : (C) 2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-01-28 15:40:59 $
***************************************************************************/

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/

/**************************************************************************
   header file for routelistwindow.cpp 
 **************************************************************************/

#ifndef ROUTELISTWINDOW_H
#define ROUTELISTWINDOW_H

#include <qlayout.h>
#include <qdockwindow.h>

#include "routelistview.h"
#include "router.h"
#include "element.h"

class RouteListWindow: public QDockWindow
{
    Q_OBJECT
        
public:
    RouteListWindow(QWidget* parent=0, const char* name=0,
            Router* router=0);
    
public slots:
    void selectedRouteChanged(QListViewItem*);
    void slotEditRouteAt(QListViewItem*, const QPoint &, int);
    void slotEditRoute(QListViewItem*);
    void slotRouteAdd();
    void slotRouteCopy();
    void slotRouteDelete();
    void slotRouteEdit();
    void slotRouteStart();
    void slotRouteStop();
    void slotStartRoute(Route*);
    void slotStopRoute(Route*);
    void slotToggleRouteState(QListViewItem*);
    void switchVisualMode(elemVisualMode);
    void updateRouteData(Route*);
    void updateRouteList();
    void updateRouteState(Route*);

signals:
    void selectedRouteIsLocked(bool);
    void showLogMessage(const QString&, int, int);
    
private:
    RouteListView* routeLV;
    Router* gbsRouter;
};
#endif // ROUTELISTWINDOW_H

