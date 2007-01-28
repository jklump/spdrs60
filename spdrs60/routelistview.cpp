/***************************************************************************
                           routelistview.cpp
                           version 0.5.1 $Revision: 1.1 $
                           -------------------------------
    copyright            : (C) 2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-01-28 15:40:57 $
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

#include "routelistview.h"


RouteListView::RouteListView(QWidget* parent, const char * name):
    QListView(parent, name)
{
    setAllColumnsShowFocus(true);
    addColumn(tr("S"));
    //addColumn(tr("Id"));
    addColumn(tr("Name"));
    addColumn(tr("From"));
    addColumn(tr("To"));
    addColumn(tr("Type"));

    // sort list by route name
    setSorting(1, true); 
    setItemMargin(0); 
}


RouteLVI* RouteListView::getRouteLVIByRoute(Route* sr)
{
    if (sr == NULL)
        return NULL;

    RouteLVI* lvi;

    QListViewItemIterator it(this);
    while (it.current()) {
        lvi = (RouteLVI*) it.current();
        if (lvi->getRoute() == sr) {
            return lvi;
        }
        ++it;
    }
}

