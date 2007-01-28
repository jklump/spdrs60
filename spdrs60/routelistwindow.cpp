/***************************************************************************
                           routelistwindow.cpp
                           version 0.5.1 $Revision: 1.1 $
                           -------------------------------
    copyright            : (C) 2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-01-28 15:40:58 $
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

    connect(routeLV, SIGNAL(selectionChanged(QListViewItem *)),
            this, SLOT(selectedRouteChanged(QListViewItem *)));

    connect(routeLV, SIGNAL(doubleClicked(QListViewItem *,
                    const QPoint &, int)),
            this, SLOT(slotEditRouteAt(QListViewItem *,
                    const QPoint &, int)));
    
    connect(routeLV, SIGNAL(returnPressed(QListViewItem *)),
            this, SLOT(slotEditRoute(QListViewItem *)));

    connect(routeLV, SIGNAL(spacePressed(QListViewItem *)),
           this, SLOT(slotToggleRouteState(QListViewItem *)));
    
    gbsRouter = router;
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
        if (routeLV->isSelected(lvi))
            emit selectedRouteIsLocked(rt->getState() == Route::rsLocked);
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

    /*same as in updateRoutesFromFile*/
    unsigned int routecount = gbsRouter->getRouteCount();

    for (unsigned int row = 0; row < routecount; row++) {
        rt = gbsRouter->getRouteAt(row);
        if (rt != NULL)
            RouteLVI* nlvi = new RouteLVI(routeLV, rt);
    }
}


/**
 * Triggered by main window when the user activates a route.
 * */
void RouteListWindow::slotRouteStart()
{
    if (gbsRouter == NULL)
        return;
    
    RouteLVI* lvi = (RouteLVI*) routeLV->selectedItem();

    if (lvi != NULL)
        gbsRouter->activateRoute(lvi->getRoute());
}


/**
 * Triggerd by route list view when route toggling is done by keyboard.
 * */
void RouteListWindow::slotToggleRouteState(QListViewItem * lvi)
{
    if (gbsRouter == NULL)
        return;

    Route* sr = static_cast<RouteLVI*>(lvi)->getRoute();

    if (sr != NULL) {
        if (sr->getState() == Route::rsLocked)
            slotStopRoute(sr);
        else
            slotStartRoute(sr);
    }
}


/**
 * internal use for route state toggling
 */
void RouteListWindow::slotStartRoute(Route* sr)
{
    if (gbsRouter == NULL)
        return;

    gbsRouter->activateRoute(sr);
}


/**
 * Triggered by main window when the user releases a route.
 * */
void RouteListWindow::slotRouteStop()
{
    if (gbsRouter == NULL)
        return;

    RouteLVI* lvi = (RouteLVI*) routeLV->selectedItem();

    if (lvi != NULL)
        slotStopRoute(lvi->getRoute());
}


/**
 * internal use
 */
void RouteListWindow::slotStopRoute(Route* sr)
{
    if (gbsRouter == NULL)
        return;

    gbsRouter->releaseRoute(sr);
}


/**
 * Triggered by main window when the user adds a new route.
 * */
void RouteListWindow::slotRouteAdd()
{
    Route* nr = gbsRouter->addNewRoute();

    if (nr != NULL) {
        RouteLVI* nlvi = new RouteLVI(routeLV, nr);
        routeLV->setSelected(nlvi, true);
    }
}


/**
 * Triggered by main window when the user edits the selected route.
 * */
void RouteListWindow::slotRouteEdit()
{
    //TODO: only in edit mode;  if (kvmEditRoute == vm) {
    slotEditRoute(routeLV->selectedItem());
}


/**
 * Used by
 *   - route table when route edit ist triggered by keyboard.
 *   - main window via slotRouteEdit()
 * */
void RouteListWindow::slotEditRoute(QListViewItem* lvi)
{
    if (lvi == NULL)
        return;
    
    if (gbsRouter->editRoute(this,
                static_cast<RouteLVI*>(lvi)->getRoute()))
        static_cast<RouteLVI*>(lvi)->updateRouteData();
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
    RouteLVI* lvi = (RouteLVI*) routeLV->selectedItem();
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


/**
 * Triggered by main window when the user deletes the selected route.
 * */
void RouteListWindow::slotRouteDelete()
{
    /*TODO: ask for "Do you realy want to delete this route?*/
    RouteLVI* lvi = (RouteLVI*) routeLV->selectedItem();

    if (lvi != NULL) {
        gbsRouter->deleteRoute(lvi->getRoute());
        routeLV->takeItem(lvi);
        delete lvi;
    }
}


/**
 * QListview emits this signal for every list selection change:
 * void QListView::selectionChanged(QListViewItem *)
 *
 *   - send current route state to main window to
 *     update toolbar and menu items
 */
void RouteListWindow::selectedRouteChanged(QListViewItem* lvi)
{
    if (lvi != NULL) {
        Route* cr = static_cast<RouteLVI*>(lvi)->getRoute();

        if (cr != NULL) {
            // send route state to main window
            emit selectedRouteIsLocked(cr->getState());

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
    // TODO: store mode to disable route editing by keystroke
    if (isVisible())
        selectedRouteChanged(routeLV->selectedItem());
}

