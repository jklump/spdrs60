/***************************************************************************
                           routeelementlvi.cpp
                           version 0.4.8 $Revision: 1.1 $
                           -------------------------------
    copyright            : (C) 2005 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2005-10-22 05:43:44 $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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
       routeElement.elemPtr2 = se->elemPtr2;
   }
   else {
       routeElement.name = "";
       routeElement.bus = 1;
       routeElement.address = 0;
       routeElement.state = 0;
       routeElement.elemPtr = NULL;
       routeElement.elemPtr2 = NULL;
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
   routeElement.elemPtr2 = se->elemPtr2;
     
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
   se->elemPtr2 = routeElement.elemPtr2;
}

