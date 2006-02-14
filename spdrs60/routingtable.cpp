/***************************************************************************
                           routingtable.cpp
                           version 0.5.0 $Revision: 1.4 $
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
   this code implements a table showing a list of routes
 ***************************************************************************/

#include "routingtable.h"

#include "pixmaps/route_locked.xpm"
#include "pixmaps/route_unlocked.xpm"
#include "pixmaps/route_wflock.xpm"
#include "pixmaps/route_wfunlock.xpm"


RoutingTable::RoutingTable(QWidget* parent) :
    QTable(0, 5, parent, "routingTable")
{
    setSelectionMode(QTable::SingleRow);
    setFocusStyle(QTable::FollowStyle);
    setShowGrid(false);
    /*TODO: if sorting is "true" also sort route list*/
    setSorting(false);
    setReadOnly(true);
    horizontalHeader()->setLabel(0, tr("S"));
    horizontalHeader()->setLabel(1, tr("Name"));
    horizontalHeader()->setLabel(2, tr("From"));
    horizontalHeader()->setLabel(3, tr("To"));
    horizontalHeader()->setLabel(4, tr("Type"));
    setColumnWidth(0, 20);
    setColumnWidth(1, 120);
    setColumnWidth(2, 60);
    setColumnWidth(3, 60);
    setColumnWidth(4, 40);
    setColumnStretchable(1, true);
    pLocked = QPixmap(route_locked_xpm);
    pUnlocked = QPixmap(route_unlocked_xpm);
    pWfLock = QPixmap(route_wflock_xpm);
    pWfUnlock = QPixmap(route_wfunlock_xpm);
}

/* sorting is allways done for the whole row */
void RoutingTable::sortColumn(int col, bool ascending, bool /*wholeRows*/)
{
    QTable::sortColumn(col, ascending, true);
}


void RoutingTable::updateLockStateIcon(int row, int rs)
{
    switch ((Route::RouteState)rs) {
        case Route::rsUnlocked:
            setItem(row, 0, new QTableItem(this, QTableItem::Never, "",
                        pUnlocked));
            break;
        case Route::rsLocked:
            setItem(row, 0, new QTableItem(this, QTableItem::Never, "",
                        pLocked));
            break;
        case Route::rsWfLock:
            setItem(row, 0, new QTableItem(this, QTableItem::Never, "",
                        pWfLock));
            break;
        case Route::rsWfUnlock:
            setItem(row, 0, new QTableItem(this, QTableItem::Never, "",
                        pWfUnlock));
            break;
    }
}


void RoutingTable::updateCurrentRowLockStateIcon(int rs)
{
    updateLockStateIcon(currentRow(), rs);
}


void RoutingTable::keyPressEvent(QKeyEvent* e)
{
    switch (e->key()) {
        case Key_Space:
            if (numRows() > 0)
                emit toggleRouteState(currentRow());
            e->accept();
            break;
        case Key_Return:
            if (numRows() > 0)
                emit editRoute(currentRow());
            e->accept();
            break;
        default:
            QTable::keyPressEvent(e);
    }
}
