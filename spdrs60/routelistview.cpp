/***************************************************************************
                           routelistview.cpp
                           version 0.5.2 $Revision: 1.8 $
                           -------------------------------
    copyright            : (C) 2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-09-25 21:07:42 $
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


RouteListView::RouteListView(QWidget* parent, const char* name):
    QListView(parent, name)
{
    setAllColumnsShowFocus(true);
    addColumn(tr("S"));
    addColumn(tr("Name"));
    addColumn(tr("From"));
    addColumn(tr("To"));
    addColumn(tr("Type"));
    addColumn(tr("Id"));
    addColumn(tr("Train"));

    // sort list by route name
    setSorting(2, true); 

    // set id and train column alignment
    setColumnAlignment(5, Qt::AlignRight);
    setColumnAlignment(6, Qt::AlignRight);
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
    return NULL;
}


void RouteListView::keyPressEvent(QKeyEvent* e)
{
    switch (e->key()) {
        case Key_Insert:
            emit insertPressed();
            e->accept();
            break;
        case Key_Delete:
            if (currentItem() != NULL)
                emit deletePressed(currentItem());
            e->accept();
            break;
        default:
            QListView::keyPressEvent(e);
    }
}

