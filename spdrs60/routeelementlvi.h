/***************************************************************************
                           routeelementlvi.h
                           version 0.5.2 $Revision: 1.7 $
                           -------------------------------
    copyright            : (C) 2005-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-02-17 07:23:32 $
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
   this is the headerfile for routeelementlvi.cpp
 ***************************************************************************/

#ifndef ROUTEELEMENTLVI_H
#define ROUTEELEMENTLVI_H

#include <qlistview.h>

#include "element.h"


class RouteElementLVI: public QListViewItem
{

private:
    stateElement routeElement;

public:
    RouteElementLVI(QListView *parent=0, int index = 0, stateElement* se = 0);
    void getStateElementData(stateElement* se = 0);
    void setStateElementData(const stateElement* se = 0);
    virtual int compare(QListViewItem* i, int col, bool ascending) const;    
};
#endif // ROUTEELEMENTLVI_H

