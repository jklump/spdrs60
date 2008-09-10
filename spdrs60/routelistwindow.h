/***************************************************************************
                           routelistwindow.h
                           version 0.5.2 $Revision: 1.6 $
                           -------------------------------
    copyright            : (C) 2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-09-10 18:34:16 $
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

class RouteListWindow: public QDockWindow
{
    Q_OBJECT
        
public:
    RouteListWindow(QWidget* parent=0, const char* name=0,
            Router* router=0);
    bool hasCurrentItem();
    int getCurrentItemState();
    
public slots:
    void currentRouteChanged(QListViewItem*);
    void slotEditRouteAt(QListViewItem*, const QPoint&, int);
    void slotEditRoute(QListViewItem*);
    void slotDeleteRoute(QListViewItem*);
    void slotRouteAdd();
    void slotRouteCopy();
    void slotRouteDelete();
    void slotRouteEdit();
    void slotRouteActivate();
    void slotRouteRelease();
    void slotRouteWithdraw();
    void slotActivateRoute(Route*);
    void slotReleaseRoute(Route*);
    void slotWithdrawRoute(Route*);
    void slotToggleRouteState(QListViewItem*);
    void switchVisualMode(elemVisualMode);
    void updateRouteData(Route*);
    void updateRouteList();
    void updateRouteState(Route*);

signals:
    void routeListIsEmpty();
    void selectedRouteChangedState();
    
private:
    RouteListView* routeLV;
    Router* gbsRouter;
    elemVisualMode  visualMode;
};
#endif // ROUTELISTWINDOW_H

