/***************************************************************************
                           routelistview.h
                           version 0.5.1 $Revision: 1.2 $
                           -------------------------------
    copyright            : (C) 2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-01-30 20:01:33 $
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
   this is the headerfile for routelvi.cpp
 ***************************************************************************/

#ifndef ROUTELISTVIEW_H
#define ROUTELISTVIEW_H

#include <qlistview.h>

#include "routelvi.h"
#include "route.h"


class RouteListView: public QListView
{
    Q_OBJECT

public:
    RouteListView(QWidget* parent = 0, const char* name = 0);
    RouteLVI* getRouteLVIByRoute(Route*);
    
protected:
    virtual void keyPressEvent(QKeyEvent *e);

signals:
    void insertPressed();
    void deletePressed(QListViewItem *);
};
#endif // ROUTELISTVIEW_H

