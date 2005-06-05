/***************************************************************************
                           routingviewer.cpp
                           version 0.4.8 $Revision: 1.4 $
                           -------------------------------
    copyright            : (C) 2004-2005 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2005-06-05 20:57:11 $
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

    //boxLayout()->setDirection(QBoxLayout::TopToBottom);;
    //boxLayout()->setMargin(2);
    
    rTable = new RoutingTable(this);
    Q_CHECK_PTR(rTable);
    boxLayout()->addWidget(rTable);
    connect(rTable, SIGNAL(currentChanged(int, int)),
            this, SLOT(selectedRouteChanged(int, int)));
    connect(rTable, SIGNAL(toggleRouteState(int)),
            this, SLOT(slotToggleRouteState(int)));
    connect(rTable, SIGNAL(editRoute(int)),
            this, SLOT(slotEditRouteNo(int)));

    /*at last get a new Router instance*/
    /*
    gbsRouter = new Router(this, "gbsRouter");
    Q_CHECK_PTR(gbsRouter);
    */
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
    //if (!isVisible())
    //    adjustSize();
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
    Route* selectedRoute = gbsRouter->getRouteAt(rTable->currentRow());
    if (selectedRoute->startRouting()){
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
    Route* sr = gbsRouter->getRouteAt(routeidx);
    if (sr->startRouting()){
        /*update toolbar buttons and table lock icon*/
        rTable->updateLockStateIcon(routeidx, true);
        emit showLogMessage(tr("Activating route '%1'")
                .arg(sr->getName()), M_INFO, HIST);

        if (rTable->isRowSelected(routeidx))
            emit selectedRouteIsLocked(true);
    }
    else {
        QApplication::beep();
        emit showLogMessage(tr("No routing possible; "
                    "route '%1' is locked by an other route.")
                .arg(sr->getName()), M_INFO, HIST);
    }
}


void RoutingViewer::slotRouteStop()
{
    if (gbsRouter == NULL)
        return;

    Route* sr = gbsRouter->getRouteAt(rTable->currentRow());
    if (sr->stopRouting()){
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
    if (sr->stopRouting()){
        /*update toolbar buttons and table lock icon*/
        rTable->updateLockStateIcon(routeidx, false);
        emit showLogMessage(tr("Resetting route '%1'")
                .arg(sr->getName()), M_INFO, HIST);

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
    //Route* selectedRoute = gbsRouter->getRouteAt(routeidx);
   
    /*
    Route->showRouteDialog(this, true);
    */
    
    /* 
    routeDialog* rtDlg = new routeDialog(this);

    if (rtDlg->exec() = QDialog::Accepted) {
        iNewCols = rtDlg->getColumns();
        iNewRows = rtDlg->getRows();
    }
    delete rtDlg;
    */
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
    rTable->removeRow(rTable->currentRow());
    gbsRouter->deleteRouteAt(rTable->currentRow());
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
                emit selectedRouteIsLocked(selectedRoute->isLocked());
            gbsRouter->selectedRouteChanged(lastrow, row);
        }
        lastrow = row;
    }
}

