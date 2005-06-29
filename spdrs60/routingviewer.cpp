/***************************************************************************
                           routingviewer.cpp
                           version 0.4.8 $Revision: 1.8 $
                           -------------------------------
    copyright            : (C) 2004-2005 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2005-06-29 20:42:36 $
****************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
   This code creates a dockable window with a routing list 
 ***************************************************************************/

#include <qhbox.h>

#include "routingviewer.h"
#include "route.h"


RoutingViewer::RoutingViewer(QWidget* parent, const char* name,
        Router* router): QDockWindow(parent, name)
{
    lastrow = -1;
    
    setResizeEnabled(true);
    setCaption(tr("Routings"));
    setCloseMode(Always);

    rTable = new RoutingTable(this);
    Q_CHECK_PTR(rTable);
    boxLayout()->addWidget(rTable);
    connect(rTable, SIGNAL(currentChanged(int, int)),
            this, SLOT(selectedRouteChanged(int, int)));
    connect(rTable, SIGNAL(toggleRouteState(int)),
            this, SLOT(slotToggleRouteState(int)));
    connect(rTable, SIGNAL(editRoute(int)),
            this, SLOT(slotEditRouteNo(int)));

    gbsRouter = router;
}


void RoutingViewer::updateRouteAt(int row)
{
    /*security checks*/
    if (row >= 0 && row < rTable->numRows())
        populateTableRow(row);
}


void RoutingViewer::updateRoutes()
{
    if (gbsRouter == NULL)
        return;

    /*same as in updateRoutesFromFile*/
    unsigned int routecount = gbsRouter->getRouteCount();
    rTable->setNumRows(routecount);

    if (routecount > 0) {
        /*fill table cells*/
        for (unsigned int row = 0; row < routecount; row++) {
            populateTableRow(row);
        }

        /*adjust column width to new text length*/
        rTable->adjustColumn(1);
        rTable->adjustColumn(2);
        rTable->adjustColumn(3);
        rTable->adjustColumn(4);
    }
}


void RoutingViewer::populateTableRow(unsigned int row)
{
    if (gbsRouter == NULL)
        return;

    Route* rowRoute = gbsRouter->getRouteAt(row);
    if (rowRoute == NULL)
        return;

    /*LockState*/
    rTable->updateLockStateIcon(row, rowRoute->isLocked());
    /*fromeName*/
    rTable->setItem(row, 1, new QTableItem(
                rTable, QTableItem::Never,
                rowRoute->getName()));
    /*fromSignalName*/
    rTable->setItem(row, 2, new QTableItem(
                rTable, QTableItem::Never,
                rowRoute->getFromSignalName()));
    /*toSignalName*/
    rTable->setItem(row, 3, new QTableItem(
                rTable, QTableItem::Never,
                rowRoute->getToSignalName()));
    /*Type*/
    rTable->setItem(row, 4, new QTableItem(
                rTable, QTableItem::Never,
                rowRoute->getTypeStr()));
}


void RoutingViewer::slotRouteStart()
{
    if (gbsRouter == NULL)
        return;

    /*TODO: optimize, may be it is better, the route sends a "lock state
     * changed" signal*/
    if (gbsRouter->activateRouteAt(rTable->currentRow())) {
        /*update toolbar buttons and table lock icon*/
        rTable->updateCurrentRowLockStateIcon(true);
        /*update menuitems/toolbar in mainwindow*/
        emit selectedRouteIsLocked(true);
    }
}


void RoutingViewer::slotToggleRouteState(int routeidx)
{
    if (gbsRouter == NULL)
        return;

    Route* selectedRoute = gbsRouter->getRouteAt(routeidx);
    if (selectedRoute->isLocked())
         slotStopRouteNo(routeidx);
    else
         slotStartRouteNo(routeidx);
}


void RoutingViewer::slotStartRouteNo(int routeidx)
{
    if (gbsRouter == NULL)
        return;

    /*TODO: optimize, may be it is better, the route sends a "lock state
     * changed" signal*/
    if (gbsRouter->activateRouteAt(routeidx)) {
        /*update toolbar buttons and table lock icon*/
        rTable->updateLockStateIcon(routeidx, true);

        if (rTable->isRowSelected(routeidx))
            emit selectedRouteIsLocked(true);
    }
}


void RoutingViewer::slotRouteStop()
{
    if (gbsRouter == NULL)
        return;

    Route* sr = gbsRouter->getRouteAt(rTable->currentRow());
    if (sr != NULL) {
        sr->stopRouting();
        /*update toolbar buttons and table lock icon*/
        rTable->updateCurrentRowLockStateIcon(false);
        emit showLogMessage(tr("Resetting route '%1'")
                .arg(sr->getName()), M_INFO, HIST);
        emit selectedRouteIsLocked(false);
    }
}


void RoutingViewer::slotStopRouteNo(int routeidx)
{
    /*TODO: optimize, may be it is better, the route sends a "lock state
     * changed" signal*/
    Route* sr = gbsRouter->getRouteAt(routeidx);
    if (sr != NULL) {
        sr->stopRouting();
        emit showLogMessage(tr("Resetting route '%1'")
                .arg(sr->getName()), M_INFO, HIST);

        /*update toolbar buttons and table lock icon*/
        rTable->updateLockStateIcon(routeidx, false);
        if (rTable->isRowSelected(routeidx))
            emit selectedRouteIsLocked(false);
    }
}


void RoutingViewer::slotRouteAdd()
{
    unsigned int idx = gbsRouter->addNewRoute();
    rTable->setNumRows(idx);
    populateTableRow(idx - 1);
}


void RoutingViewer::slotRouteEdit()
{
    slotEditRouteNo(rTable->currentRow());
}


void RoutingViewer::slotEditRouteNo(int routeidx)
{
    Route* sr = gbsRouter->getRouteAt(routeidx);
    if (sr != NULL) {
        connect(sr, SIGNAL(getElementByAddress(const int, const int,
                        element**)), this,
                SIGNAL(getElementByAddress(const int, const int,
                        element**)));
        
        if (sr->runEditRouteDialog(this)) {
            populateTableRow(routeidx);
        }

        disconnect(sr, SIGNAL(getElementByAddress(const int,
                        const int, element**)), this,
                SIGNAL(getElementByAddress(const int, const int,
                        element**)));
    }
}


void RoutingViewer::slotRouteCopy()
{
    int row = rTable->currentRow();
    rTable->insertRows(row + 1, 1);
    gbsRouter->copyRouteAt(row);
    populateTableRow(row + 1);
}


void RoutingViewer::slotRouteDelete()
{
    /*TODO: ask for "Do you realy want to delete this route?*/
    int row = rTable->currentRow();
    rTable->removeRow(row);
    gbsRouter->deleteRouteAt(row);
    /*update rootingtoolbar buttons*/
    if (rTable->numRows() == 0)
        emit noRoutesAvailable();
}


void RoutingViewer::selectedRouteChanged(int row, int col)
{
    /*
     * QTable sends this signal for every cell selection change, but we
     * are only interessted in changed rows
     */
    //fprintf(stderr, "row: %d  lastrow: %d\n", row, lastrow);

    if (row != lastrow){
        /* send route lock state to rountingtoolbar to update button
         * states */
        if (row >= 0){
            Route* selectedRoute = gbsRouter->getRouteAt(row);
            if (selectedRoute != NULL)
                // send message to mainwindow to update toolbar and menu
                // items
                emit selectedRouteIsLocked(selectedRoute->isLocked());
            gbsRouter->selectedRouteChanged(lastrow, row);
        }
        lastrow = row;
    }
}


void RoutingViewer::switchVisualMode(elemVisualMode vm)
{
    if (isVisible()) {
        int row = rTable->currentRow();

        if (kvmEditRoute == vm) {
            gbsRouter->selectedRouteChanged(-1, row);
        }

        else if (kvmNormal == vm) {
            Route* sr = gbsRouter->getRouteAt(row);
            // send message to mainwindow to update toolbar and menu items
            if (sr != NULL)
                emit selectedRouteIsLocked(sr->isLocked());
        }
    }
}

