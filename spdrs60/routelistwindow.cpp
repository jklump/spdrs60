/***************************************************************************
                           routelistwindow.cpp
                           version 0.5.2 $Revision: 1.6 $
                           -------------------------------
    copyright            : (C) 2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-09-01 07:35:33 $
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

#include "routelistwindow.h"
#include "route.h"


#if QT_VERSION >= 0x030100
RouteListWindow::RouteListWindow(QWidget* parent, const char* name,
        Router* router): QDockWindow(parent, name)
#else
RouteListWindow::RouteListWindow(QWidget* parent, const char* name,
        Router* router): QDockWindow(QDockWindow::InDock, parent, name)
#endif
{
    setResizeEnabled(true);
    setCaption(tr("Routes"));
    setCloseMode(Always);

    routeLV = new RouteListView(this, "routeListView");
    Q_CHECK_PTR(routeLV);
    boxLayout()->addWidget(routeLV);

    connect(routeLV, SIGNAL(currentChanged(QListViewItem *)),
            this, SLOT(currentRouteChanged(QListViewItem *)));

    connect(routeLV, SIGNAL(doubleClicked(QListViewItem *,
                    const QPoint &, int)),
            this, SLOT(slotEditRouteAt(QListViewItem *,
                    const QPoint &, int)));
    
    connect(routeLV, SIGNAL(returnPressed(QListViewItem *)),
            this, SLOT(slotEditRoute(QListViewItem *)));

    connect(routeLV, SIGNAL(spacePressed(QListViewItem *)),
           this, SLOT(slotToggleRouteState(QListViewItem *)));
    
    connect(routeLV, SIGNAL(deletePressed(QListViewItem *)),
           this, SLOT(slotDeleteRoute(QListViewItem *)));
    
    connect(routeLV, SIGNAL(insertPressed()),
            this, SLOT(slotRouteAdd()));

    gbsRouter = router;
    visualMode = kvmNormal;
}


/**
 * the router triggers this slot when
 *   - a new route element is recorded/modified
 * */
void RouteListWindow::updateRouteData(Route* rt)
{
    if (routeLV == NULL)
        return;

    RouteLVI* lvi = routeLV->getRouteLVIByRoute(rt);

    if (lvi != NULL)
        lvi->updateRouteData();
}


/**
 * the router triggers this slot when
 *   - the state of a route changes
 * */
void RouteListWindow::updateRouteState(Route* rt)
{
    if (rt == NULL)
        return;

    RouteLVI* lvi = routeLV->getRouteLVIByRoute(rt);

    if (lvi != NULL) {
        lvi->updateRouteStatePixmap();

        // if row is selected also update menu buttons
        if (lvi == routeLV->currentItem())
            emit selectedRouteChangedState();
    }
}

/**
 * the router triggers this slot when
 *   - a new file is loaded
 *   - all routes are cleared
 * */
void RouteListWindow::updateRouteList()
{
    if (gbsRouter == NULL)
        return;

    Route* rt = NULL;
    routeLV->clear();
    emit routeListIsEmpty();

    unsigned int routecount = gbsRouter->getRouteCount();

    for (unsigned int row = 0; row < routecount; row++) {
        rt = gbsRouter->getRouteAt(row);
        if (rt != NULL)
            //RouteLVI* nlvi = new RouteLVI(routeLV, rt);
            new RouteLVI(routeLV, rt);
    }
    if (routecount > 0) {
        routeLV->setCurrentItem(routeLV->firstChild());
        currentRouteChanged(routeLV->firstChild());
    }
}


/**
 * Triggered by main window when the user activates a route.
 * */
void RouteListWindow::slotRouteActivate()
{
    if (gbsRouter == NULL || visualMode == kvmEditLayout)
        return;
    
    RouteLVI* lvi = (RouteLVI*) routeLV->currentItem();

    if (lvi != NULL)
        gbsRouter->activateRoute(lvi->getRoute());
}


/**
 * Triggerd by route list view when route toggling is done by keyboard.
 * */
void RouteListWindow::slotToggleRouteState(QListViewItem * lvi)
{
    if (gbsRouter == NULL || visualMode == kvmEditLayout)
        return;

    Route* sr = static_cast<RouteLVI*>(lvi)->getRoute();

    if (sr != NULL) {
        if (sr->getState() == Route::rsLocked)
            slotWithdrawRoute(sr);
        else
            slotActivateRoute(sr);
    }
}


/**
 * internal use for route state toggling
 */
void RouteListWindow::slotActivateRoute(Route* sr)
{
    if (gbsRouter == NULL || visualMode == kvmEditLayout)
        return;

    gbsRouter->activateRoute(sr);
}


/**
 * Triggered by main window when the user releases a route.
 * */
void RouteListWindow::slotRouteRelease()
{
    if (gbsRouter == NULL || visualMode == kvmEditLayout)
        return;

    RouteLVI* lvi = (RouteLVI*) routeLV->currentItem();

    if (lvi != NULL)
        slotReleaseRoute(lvi->getRoute());
}


/**
 * internal use
 */
void RouteListWindow::slotReleaseRoute(Route* sr)
{
    if (gbsRouter == NULL || visualMode == kvmEditLayout)
        return;

    gbsRouter->releaseRoute(sr);
}


/**
 * Triggered by main window when the user withdraws a route.
 * */
void RouteListWindow::slotRouteWithdraw()
{
    if (gbsRouter == NULL || visualMode == kvmEditLayout)
        return;

    RouteLVI* lvi = (RouteLVI*) routeLV->currentItem();

    if (lvi != NULL)
        slotWithdrawRoute(lvi->getRoute());
}


/**
 * internal use
 */
void RouteListWindow::slotWithdrawRoute(Route* sr)
{
    if (gbsRouter == NULL || visualMode == kvmEditLayout)
        return;

    gbsRouter->withdrawRoute(sr);
}


/**
 * Triggered by main window when the user adds a new route.
 * */
void RouteListWindow::slotRouteAdd()
{
    if (visualMode == kvmEditRoute) {
        Route* nr = gbsRouter->addNewRoute();

        if (nr != NULL) {
            RouteLVI* nlvi = new RouteLVI(routeLV, nr);
            routeLV->setCurrentItem(nlvi);
            currentRouteChanged(nlvi);

            // tell main window that list now is _not_ empty
            if (routeLV->childCount() == 1)
                emit routeListIsEmpty();
        }
    }
}


/**
 * Triggered by main window when the user edits the selected route.
 * */
void RouteListWindow::slotRouteEdit()
{
    if (visualMode == kvmEditRoute)
        slotEditRoute(routeLV->currentItem());
}


/**
 * Used by
 *   - route table when route edit ist triggered by keyboard.
 *   - main window via slotRouteEdit()
 * */
void RouteListWindow::slotEditRoute(QListViewItem* lvi)
{
    if (visualMode == kvmEditRoute) {
        if (lvi == NULL)
            return;

        if (gbsRouter->editRoute(this,
                    static_cast<RouteLVI*>(lvi)->getRoute()))
            static_cast<RouteLVI*>(lvi)->updateRouteData();
    }
}


void RouteListWindow::slotEditRouteAt(QListViewItem* lvi,
        const QPoint &, int)
{
    slotEditRoute(lvi);
}

/**
 * Triggered by main window when the user copies the selected route.
 * */
void RouteListWindow::slotRouteCopy()
{
    if (visualMode == kvmEditRoute) {
        RouteLVI* lvi = (RouteLVI*) routeLV->currentItem();
        if (lvi != NULL) {
            Route* cr = lvi->getRoute();
            if (cr != NULL) {
                Route* nr = gbsRouter->copyRoute(cr);
                if (nr != NULL) {
                    RouteLVI* nlvi = new RouteLVI(routeLV, nr);
                    routeLV->setSelected(nlvi, true);
                }
            }
        }
    }
}


/**
 * Triggered by main window when the user deletes the current route.
 * */
void RouteListWindow::slotRouteDelete()
{
    slotDeleteRoute(routeLV->currentItem());
}


void RouteListWindow::slotDeleteRoute(QListViewItem * lvi)
{
    if (visualMode == kvmEditRoute) {
        /*TODO: ask for "Do you realy want to delete this route?*/

        if (lvi != NULL) {
            gbsRouter->deleteRoute(static_cast<RouteLVI*>(lvi)->getRoute());
            routeLV->takeItem(lvi);
            delete lvi;

            // if number of routes is null send signal to main window
            // to update edit button states
            if (routeLV->childCount() == 0)
                emit routeListIsEmpty();
        }
    }
}

/**
 * QListview emits this signal for every list selection change:
 * void QListView::selectionChanged(QListViewItem *)
 *
 *   - send current route state to main window to
 *     update toolbar and menu items
 */
void RouteListWindow::currentRouteChanged(QListViewItem* lvi)
{
    if (lvi != NULL) {
        Route* cr = static_cast<RouteLVI*>(lvi)->getRoute();

        if (cr != NULL) {
            // send route state to main window
            emit selectedRouteChangedState();

            // tell router to clear highlighting of last selected route
            gbsRouter->selectedRouteChanged(cr);
        }
    }
}

/**
 * Triggered by main window when the user switches between
 * different visual modes.
 * */
void RouteListWindow::switchVisualMode(elemVisualMode vm)
{
    if (visualMode != vm) {
        visualMode = vm;
        if (visualMode == kvmEditRoute)
            currentRouteChanged(routeLV->currentItem());
    }
}


bool RouteListWindow::hasCurrentItem()
{
    return (routeLV->currentItem() != NULL);
}


int RouteListWindow::getCurrentItemState()
{
    int returnvalue = -1;
    
    RouteLVI* lvi = (RouteLVI*) routeLV->currentItem();
    if (lvi != NULL) {
        Route* cr = lvi->getRoute();
        if (cr != NULL)
            returnvalue = cr->getState();
    }
        
    return returnvalue;
}

