/***************************************************************************
                           routelvi.h
                           version 0.5.3 $Revision: 1.6 $
                           -------------------------------
    copyright            : (C) 2007-2008 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-11-05 08:42:40 $
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

#ifndef ROUTELVI_H
#define ROUTELVI_H

#include <qlistview.h>
#include <qpixmap.h>


// forward declaration
class Route;

class RouteLVI: public QListViewItem
{

private:
    Route* route;
    QPixmap pLocked;
    QPixmap pUnlocked;
    QPixmap pWfLock;
    QPixmap pWfUnlock;

public:
    RouteLVI(QListView *parent=0, Route* rt = 0);
    virtual int compare(QListViewItem* i, int col, bool ascending) const;    
    int getRouteState();
    unsigned int getRouteId();
    unsigned int getRouteTrain();
    Route* getRoute();
    void setRoute(Route*);
    void updateRouteStatePixmap();
    void updateRouteData();
};
#endif // ROUTELVI_H

