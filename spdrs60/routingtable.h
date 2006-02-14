/***************************************************************************
                           routingtable.h
                           version 0.5.0 $Revision: 1.3 $
                           -------------------------------
    copyright            : (C) 2004-2006 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2006-02-14 21:54:24 $
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
   this is the headerfile for routingtable.cpp
 ***************************************************************************/

#ifndef ROUTINGTABLE_H
#define ROUTINGTABLE_H

#include <qtable.h>

#include "route.h"


class RoutingTable: public QTable
{
    Q_OBJECT
        
private:
    QPixmap pLocked;
    QPixmap pUnlocked;
    QPixmap pWfLock;
    QPixmap pWfUnlock;

protected:
    virtual void keyPressEvent(QKeyEvent *e);

public:
    RoutingTable(QWidget *parent=0);
    void sortColumn(int, bool, bool);
    
public slots:
    void updateLockStateIcon(int, int);
    void updateCurrentRowLockStateIcon(int);
    
signals:
    void toggleRouteState(int);
    void editRoute(int);

};
#endif // ROUTINGTABLE_H

