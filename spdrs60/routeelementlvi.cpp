/***************************************************************************
                           routeelementlvi.cpp
                           version 0.5.5 $Revision: 1.13 $
                           -------------------------------
    copyright            : (C) 2005-2009 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2009-03-08 08:24:06 $
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
   this code implements a route element displayed as listview item
 ***************************************************************************/

#include "routeelementlvi.h"


RouteElementLVI::RouteElementLVI(QListView* parent, int index,
    stateElement* se): QListViewItem(parent)
{
   if (se != NULL) {
       routeElement.name = se->name;
       routeElement.bus = se->bus;
       routeElement.address = se->address;
       routeElement.state = se->state;
       routeElement.elemPtr = se->elemPtr;
   }
   else {
       routeElement.name = "";
       routeElement.bus = 1;
       routeElement.address = 0;
       routeElement.state = 0;
       routeElement.elemPtr = NULL;
   }

   setText(0, QString::number(index));  
   setText(1, routeElement.name);  
   setText(2, QString::number(routeElement.bus));  
   setText(3, QString::number(routeElement.address));  
   setText(4, QString::number(routeElement.state));  
}


void RouteElementLVI::setStateElementData(const stateElement* se)
{
   if (se == NULL)
       return;

   routeElement.name = se->name;
   routeElement.bus = se->bus;
   routeElement.address = se->address;
   routeElement.state = se->state;
   routeElement.elemPtr = se->elemPtr;
     
   setText(1, routeElement.name);  
   setText(2, QString::number(routeElement.bus));  
   setText(3, QString::number(routeElement.address));  
   setText(4, QString::number(routeElement.state));  
}


void RouteElementLVI::getStateElementData(stateElement* se)
{
   if (se == NULL)
       return;

   se->name = routeElement.name;
   se->bus = routeElement.bus;
   se->address = routeElement.address;
   se->state = routeElement.state;
   se->elemPtr = routeElement.elemPtr;  
}


/*
 * sort all columns:
 *
 * No Name      Type
 * -------------------------
 * 0  No        int
 * 1  Name      QString
 * 2  SRCP-Bus  unsigned int
 * 3  Address   unsigned int
 * 4  State     unsigned int
 * -------------------------
 */
int RouteElementLVI::compare(QListViewItem* i, int col,
        bool ascending) const
{
    int returnvalue = 0;
    stateElement ce;
    RouteElementLVI* item = (RouteElementLVI*) i;
    item->getStateElementData(&ce);
    
    int key1 = key(col, ascending).toInt();
    int key2 = i->key(col, ascending).toInt();

    switch (col) {
        case 0:
            if (key1 > key2)
                returnvalue = 1;
            else if (key1 < key2)
                returnvalue = -1;
            break;
        case 1:
            returnvalue = key(col,
                    ascending).localeAwareCompare(i->key(col, ascending));
            break;
        case 2:
            if (routeElement.bus > ce.bus)
                returnvalue = 1;
            else if (routeElement.bus < ce.bus)
                returnvalue = -1;
            break;
        case 3:
            if (routeElement.address > ce.address)
                returnvalue = 1;
            else if (routeElement.address < ce.address)
                returnvalue = -1;
            break;
        case 4:
            if (routeElement.state > ce.state)
                returnvalue = 1;
            else if (routeElement.state < ce.state)
                returnvalue = -1;
            break;
    }
    return returnvalue;
}
