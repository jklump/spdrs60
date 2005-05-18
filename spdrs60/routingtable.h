/***************************************************************************
                           routingtable.h
                           version 0.4.8 $Revison$
                           -------------------------------
    copyright            : (C) 2004-2005 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2005-05-18 21:16:01 $
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


class RoutingTable: public QTable
{
    Q_OBJECT
        
private:
    QPixmap pLocked;
    QPixmap pUnlocked;

protected:
    virtual void keyPressEvent(QKeyEvent *e);

public:
    RoutingTable(QWidget *parent=0);
    void sortColumn(int, bool, bool);
    
public slots:
    void updateLockStateIcon(int, bool);
    void updateCurrentRowLockStateIcon(bool);
    
signals:
    void toggleRouteState(int);
    void editRoute(int);

};
#endif // ROUTINGTABLE_H

