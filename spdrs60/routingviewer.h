/***************************************************************************
                           routingviewer.h
                           version 0.4.8 $Revision: 1.9 $
                           -------------------------------
    copyright            : (C) 2004-2005 by Guido Scholz
    email                : guido.scholz@ bayernline.de
    last modified        : $Date: 2005-11-14 17:28:49 $
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
   header file for routingviewer.cpp 
 **************************************************************************/

#ifndef ROUTINGVIEWER_H
#define ROUTINGVIEWER_H

#include <qlayout.h>
#include <qdockwindow.h>

#include "routingtable.h"
#include "router.h"
#include "element.h"

class RoutingViewer: public QDockWindow
{
    Q_OBJECT
        
public:
    RoutingViewer(QWidget* parent=0, const char* name=0,
            Router* router=0);
    
public slots:
    void updateRoutes();
    void updateRouteAt(int);
    void updateRouteStateAt(int, bool);
    void selectedRouteChanged(int, int);
    void slotEditRouteNo(int);
    void slotRouteDelete();
    void slotRouteCopy();
    void slotRouteEdit();
    void slotRouteAdd();
    void slotRouteStart();
    void slotRouteStop();
    void slotStartRouteNo(int);
    void slotStopRouteNo(int);
    void slotToggleRouteState(int);
    void switchVisualMode(elemVisualMode);

signals:
    void selectedRouteIsLocked(bool);
    void showLogMessage(const QString&, int, int);
    
private:
    int lastrow;
    RoutingTable* rTable;
    Router* gbsRouter;
    void populateTableRow(unsigned int);
};
#endif // ROUTINGVIEWER_H

