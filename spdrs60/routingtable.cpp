/***************************************************************************
                           routingtable.cpp
                           version 0.4.8 $Revision: 1.2 $
                           -------------------------------
    copyright            : (C) 2004-2005 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2005-06-29 20:42:36 $
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


RoutingTable::RoutingTable(QWidget* parent) :
    QTable(0, 5, parent, "routingTable")
{
    setSelectionMode(QTable::SingleRow);
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
}

/* sorting is allways done for the whole row */
void RoutingTable::sortColumn(int col, bool ascending, bool /*wholeRows*/)
{
    QTable::sortColumn(col, ascending, true);
}


void RoutingTable::updateLockStateIcon(int row, bool isLocked)
{
    setItem(row, 0, new QTableItem(this, QTableItem::Never, "",
            isLocked ? pLocked : pUnlocked));
}


void RoutingTable::updateCurrentRowLockStateIcon(bool isLocked)
{
    updateLockStateIcon(currentRow(), isLocked);
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
