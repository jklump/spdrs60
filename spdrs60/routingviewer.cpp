/***************************************************************************
                           routingviewer.cpp
                           version 0.4.8 $Revision: 1.16 $
                           -------------------------------
    copyright            : (C) 2004-2005 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2005-12-09 18:07:08 $
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


#if QT_VERSION >= 0x030100
RoutingViewer::RoutingViewer(QWidget* parent, const char* name,
        Router* router): QDockWindow(parent, name)
#else
RoutingViewer::RoutingViewer(QWidget* parent, const char* name,
        Router* router): QDockWindow(QDockWindow::InDock, parent, name)
#endif
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


void RoutingViewer::updateRouteStateAt(int row, bool state)
{
    /*security checks*/
    if (row >= 0 && row < rTable->numRows()) {
        rTable->updateLockStateIcon(row, state);
        // if row is selected also update menu buttons
        if (rTable->currentRow() == row)
            emit selectedRouteIsLocked(state);
    }
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

    gbsRouter->activateRouteAt(rTable->currentRow());
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

    gbsRouter->activateRouteAt(routeidx);
}


void RoutingViewer::slotRouteStop()
{
    if (gbsRouter == NULL)
        return;

    slotStopRouteNo(rTable->currentRow());
}


void RoutingViewer::slotStopRouteNo(int routeidx)
{
    if (gbsRouter == NULL)
        return;

    gbsRouter->releaseRouteAt(routeidx);
}


void RoutingViewer::slotRouteAdd()
{
    unsigned int idx = gbsRouter->addNewRoute();
    rTable->setNumRows(idx);
    --idx;
    populateTableRow(idx);
    // set focus to new row
#if QT_VERSION >= 0x030200
    rTable->selectRow(idx);
#else
    rTable->removeSelection(rTable->currentSelection());
    // no idea how to get QTableSelection working here
#endif
}


void RoutingViewer::slotRouteEdit()
{
    slotEditRouteNo(rTable->currentRow());
}


void RoutingViewer::slotEditRouteNo(int routeidx)
{
    if (gbsRouter->editRouteAt(this, routeidx))
        populateTableRow(routeidx);
}


void RoutingViewer::slotRouteCopy()
{
    int row = rTable->currentRow();
    rTable->insertRows(row + 1, 1);
    gbsRouter->copyRouteAt(row);
    ++row;
    populateTableRow(row);
    // set focus to new row
#if QT_VERSION >= 0x030200
    rTable->selectRow(row);
#else
    rTable->removeSelection(rTable->currentSelection());
    // no idea how to get QTableSelection working here
#endif
}


void RoutingViewer::slotRouteDelete()
{
    /*TODO: ask for "Do you realy want to delete this route?*/
    int row = rTable->currentRow();

    if (row >= 0){
        rTable->removeRow(row);
        gbsRouter->deleteRouteAt(row);
        //TODO: set focus to row above removed row
        /* not possible without flickering
        if (row > 0)
            rTable->selectRow(row - 1);
        */
    }
}


/*
 * QTable sends this signal for every cell selection change, but we
 * are only interessted in changed rows
 */
void RoutingViewer::selectedRouteChanged(int row, int col)
{
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

