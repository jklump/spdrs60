/***************************************************************************
                           routelvi.h
                           version 0.5.1 $Revision: 1.1 $
                           -------------------------------
    copyright            : (C) 2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-01-28 15:41:00 $
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

#include "route.h"


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
    Route* getRoute();
    void setRoute(Route*);
    void updateRouteStatePixmap();
    void updateRouteData();
};
#endif // ROUTELVI_H

