/***************************************************************************
                           routelistview.h
                           version 0.5.3 $Revision: 1.6 $
                           -------------------------------
    copyright            : (C) 2007-2008 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-11-23 21:05:25 $
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


// forward declarations
class Route;
class RouteLVI;

class RouteListView: public QListView
{
    Q_OBJECT

public:
    RouteListView(QWidget* parent = 0, const char* name = 0);
    RouteLVI* getRouteLVIByRoute(Route*);
    
protected:
    void keyPressEvent(QKeyEvent *e);

signals:
    void insertPressed();
    void deletePressed(QListViewItem *);
};
#endif // ROUTELISTVIEW_H

