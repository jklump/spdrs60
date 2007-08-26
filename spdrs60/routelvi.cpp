/***************************************************************************
                           routelvi.cpp
                           version 0.5.2 $Revision: 1.3 $
                           -------------------------------
    copyright            : (C) 2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-08-26 15:08:12 $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
   this code implements a listview item displaying route data
 ***************************************************************************/

#include "routelvi.h"

#include "pixmaps/route_locked.xpm"
#include "pixmaps/route_unlocked.xpm"
#include "pixmaps/route_wflock.xpm"
#include "pixmaps/route_wfunlock.xpm"


RouteLVI::RouteLVI(QListView* parent, Route* rt): QListViewItem(parent)
{
    // TODO: move pixmaps to parent
    pLocked = QPixmap(route_locked_xpm);
    pUnlocked = QPixmap(route_unlocked_xpm);
    pWfLock = QPixmap(route_wflock_xpm);
    pWfUnlock = QPixmap(route_wfunlock_xpm);
    
    route = rt;
    updateRouteData();
}


/*
 * sort all columns:
 *
 * No Name      Type
 * -------------------------------
 * 0  State     int
 * 1  Id        unsigned int
 * 2  Name      QString
 * 3  From      QString
 * 4  To        QString
 * 5  Type      (int, int)/QString
 * -------------------------------
 */
int RouteLVI::compare(QListViewItem* i, int col,
        bool ascending) const
{
    if (route == NULL)
        return 0;
    
    int returnvalue = 0;

    RouteLVI* item = (RouteLVI*) i;
    int key1;
    int key2;
    unsigned int idkey1;
    unsigned int idkey2;
    
    switch (col) {
        case 0:
            // compare state integer values
            key1 = route->getState();
            key2 = item->getRouteState();
            if (key1 > key2)
                returnvalue = 1;
            else if (key1 < key2)
                returnvalue = -1;
            if (!ascending)
                returnvalue *= -1;
            break;

        case 1:
            // compare id integer values
            idkey1 = route->getId();
            idkey2 = item->getRouteId();
            if (idkey1 > idkey2)
                returnvalue = 1;
            else if (idkey1 < idkey2)
                returnvalue = -1;
            if (!ascending)
                returnvalue *= -1;
            break;
        default:
            // compare string values
            returnvalue = key(col, ascending).localeAwareCompare(
                    i->key(col, ascending));
            break;
    }
    return returnvalue;
}


unsigned int RouteLVI::getRouteId()
{
    if (route == NULL)
        return 0;
    else
        return route->getId();
}


int RouteLVI::getRouteState()
{
    if (route == NULL)
        return 0;
    else
        return route->getState();
}


Route* RouteLVI::getRoute()
{
    return route;
}


void RouteLVI::setRoute(Route* rt)
{
    if (rt != route) {
        route = rt;
        updateRouteData();
    }
}


void RouteLVI::updateRouteData()
{
    if (route != NULL) {
        updateRouteStatePixmap();  
        setText(1, QString::number(route->getId()));  
        setText(2, route->getName());  
        setText(3, route->getFromSignalName());  
        setText(4, route->getToSignalName());  
        setText(5, route->getTypeStr());  
    }
    else {
        setPixmap(0, pUnlocked);  
        setText(1, "");  
        setText(2, "");  
        setText(3, "");  
        setText(4, "");  
        setText(5, "");  
    }
}


void RouteLVI::updateRouteStatePixmap()
{
    if (route == NULL)
        return;
    
    int rs = route->getState();

    switch (rs) {
        case Route::rsUnlocked:
            setPixmap(0, pUnlocked);
            break;
        case Route::rsLocked:
            setPixmap(0, pLocked);
            break;
        case Route::rsWfLock:
            setPixmap(0, pWfLock);
            break;
        case Route::rsWfUnlock:
            setPixmap(0, pWfUnlock);
            break;
    }
}

