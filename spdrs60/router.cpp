/***************************************************************************
                           router.cpp
                           version 0.5.2 $Revision: 1.41 $
                           -------------------------------
    copyright            : (C) 2004-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-08-26 15:08:12 $
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
  This code implements the router object. This object cares for the routing
  list, loads it from file, save it to file, shows routing edit window.
 ***************************************************************************/

#include <qfile.h>
#include <qdatetime.h>

#include "resources.h"
#include "router.h"


Router::Router(QObject* parent, const char* name):
    QObject(parent, name)
{
    gbsElements = NULL;
    selectedRoute = NULL;
    resetRt = NULL;
    selectedStartSig = NULL;
    routeList.setAutoDelete(true);
    modified = false;
    visualmode = kvmNormal;
}


Router::Router(QObject* parent, QPtrVector<element>* elPtr, const char* name):
    QObject(parent, name)
{
    selectedRoute = NULL;
    resetRt = NULL;
    selectedStartSig = NULL;
    gbsElements = elPtr;
    routeList.setAutoDelete(true);
    modified = false;
    visualmode = kvmNormal;
}


Router::~Router()
{
    routeList.clear();
}


void Router::readFileTextFromStream(QTextStream& ts)
{
    /*clear old list*/
    if (!routeList.isEmpty())
        routeList.clear();
    
    while (!ts.eof())
        routeList.append(new Route(ts, true));

    modified = false;
    setupRouteElements();
    /*send update signal to routing viewer*/
    emit routeListChanged();
}

void Router::setupRouteElements()
{
    if (gbsElements == NULL)
        return;

    QPtrListIterator<Route> routeit(routeList);
    Route* sr;
    while ((sr = routeit.current()) != 0 ) {
        ++routeit;
        sr->setupElementLists(gbsElements);
        connect(sr, SIGNAL(stateChanged(Route*, int)),
                this, SLOT(processRouteState(Route*, int)));
        connect(sr, SIGNAL(updateRoutePathLEDs(const stateElement&,
                        const stateElement&, RouteSetAction&)),
                this, SIGNAL(updateRoutePathLEDs(const stateElement&,
                        const stateElement&, RouteSetAction&)));
    }
}


void Router::updateRouteElements()
{
    if (gbsElements == NULL)
        return;

    QPtrListIterator<Route> routeit(routeList);
    Route* sr;
    while ((sr = routeit.current()) != 0 ) {
        ++routeit;
        sr->updateElementLists(gbsElements);
        // tell route list window about changed data
        emit routeDataChanged(sr);
    }
}


unsigned int Router::getRouteCount()
{
    return routeList.count();
}


void Router::copyRouteAt(unsigned int index)
{
    copyRoute(getRouteAt(index));
}


Route* Router::copyRoute(Route* cr)
{
    Route* nr = NULL;

    if (cr != NULL) {
        nr = cr->getClone();
        if (nr != NULL) {
            routeList.insert(routeList.find(cr), nr);
            connect(nr, SIGNAL(stateChanged(Route*, int)),
                    this, SLOT(processRouteState(Route*, int)));
            connect(nr, SIGNAL(updateRoutePathLEDs(const stateElement&,
                            const stateElement&, RouteSetAction&)),
                    this, SIGNAL(updateRoutePathLEDs(const stateElement&,
                            const stateElement&, RouteSetAction&)));
            modified = true;
        }
    }
    return nr;
}


void Router::deleteRouteAt(unsigned int index)
{
    deleteRoute(routeList.at(index));
}


void Router::deleteRoute(Route* dr)
{
    if (selectedRoute == dr)
        selectedRoute = NULL;

    if (dr != NULL) { 
        if (dr->getState() != Route::rsUnlocked)
            dr->stopRouting();
        dr->hideRoute();
        disconnect(dr, SIGNAL(updateRoutePathLEDs(const stateElement&,
                        const stateElement&, RouteSetAction&)),
                this, SIGNAL(updateRoutePathLEDs(const stateElement&,
                        const stateElement&, RouteSetAction&)));
        disconnect(dr, SIGNAL(stateChanged(Route*, int)),
                this, SLOT(processRouteState(Route*, int)));
        routeList.remove(dr);
        modified = true;
    }
}


void Router::writeFileTextToStream(QTextStream& ts)
{
    ts
        << "# start of route section" << endl
        << "# routes=" << routeList.count() << endl;

    unsigned int i = 0;
    
    QPtrListIterator<Route> routeit(routeList);
    Route* saveroute;
    while ((saveroute = routeit.current()) != 0 ) {
        ++routeit;
        i++;
        ts << "%% route " << i << endl;
        saveroute->writeFileTextToStream(ts);
    }
    modified = false;
}


Route* Router::getRouteAt(unsigned int aRouteNo)
{
    return routeList.at(aRouteNo);
}


bool Router::isModified()
{
    return modified;
}


void Router::clear()
{
    routeList.clear();
    modified = false;
}


void Router::clearRoutes()
{
    clear();
    /*send update signal to routingviewer*/
    emit routeListChanged();
}


void Router::setElementListPtr(QPtrVector<element>* elp)
{
    gbsElements = elp;
}

/**
 * update route element highlighting in route edit mode
 * first hide last selected route then show current selected one
 */
void Router::selectedRouteChanged(Route* currentRoute)
{
    if (visualmode == kvmEditRoute) {
        if (selectedRoute != NULL)
            selectedRoute->hideRoute();

        if (currentRoute != NULL)
            currentRoute->showRoute();
    }
    selectedRoute = currentRoute;
}


void Router::showRouteAt(int idx)
{
    if (visualmode == kvmEditRoute) {
        Route* cr = getRouteAt(idx);
        if (cr != NULL)
            cr->showRoute();
    }
}


void Router::switchVisualMode(elemVisualMode vm)
{
    // if layout was in edit mode update route data to make shure
    // that changed elements are found at the right place
    if (visualmode == kvmEditLayout)
        updateRouteElements();

    visualmode = vm;
    if (vm != kvmEditRoute)
        selectedRoute = NULL;
}


void Router::recordElement(element* el, elemRecordType rtype)
{
    if (visualmode == kvmEditRoute && selectedRoute != NULL) {
        switch (rtype) {
            case (krecStartStop):
                if (!selectedRoute->hasEntrySignal())
                    selectedRoute->setEntrySignal(el);
                else if (!selectedRoute->hasExitSignal())
                    selectedRoute->setExitSignal(el);
               
                /*send update signal to routingviewer to show changed
                  route name*/
                emit routeDataChanged(selectedRoute);
                modified = true;
                break;
            case (krecNormal):
                selectedRoute->addSwitchElement(el);
                modified = true;
                break;
            case (krecClear):
                selectedRoute->removeElement(el);
                /*send update signal to routingviewer to show changed
                  route name if changed element was entry or exit signal*/
                emit routeDataChanged(selectedRoute);
                modified = true;
                break;
            default:
                break;
        }
    }
}


Route* Router::addNewRoute()
{
    Route* nr = new Route(tr("New route"));
    if (nr != NULL) {
        selectedRouteChanged(nr);
        routeList.append(nr);

        connect(nr, SIGNAL(stateChanged(Route*, int)),
                this, SLOT(processRouteState(Route*, int)));
        connect(nr, SIGNAL(updateRoutePathLEDs(const stateElement&,
                        const stateElement&, RouteSetAction&)),
                this, SIGNAL(updateRoutePathLEDs(const stateElement&,
                        const stateElement&, RouteSetAction&)));
    }
    return nr;
}


void Router::startRecordModeAt(unsigned int index)
{
    selectedRoute = getRouteAt(index);
    showRouteAt(index);
}


void Router::processRouteState(Route* rt, int rs)
{
    int index = 0;
    
    index = routeList.find(rt);
    emit routeStateChanged(rt);

    switch ((Route::RouteState)rs) {
        case Route::rsUnlocked:
            // send signal to routing viewer to update state icon
            emit showLogMessage(tr("Route '%1' released")
                    .arg(rt->getName()), MT_INFO, HL_HINT);
            break;
        case Route::rsLocked:
            // send signal to routing viewer to update state icon
            emit showLogMessage(tr("Route '%1' activated")
                    .arg(rt->getName()), MT_INFO, HL_HINT);
            break;
        case Route::rsWfLock:
            emit showLogMessage(tr("Route '%1' waiting for activation")
                    .arg(rt->getName()), MT_INFO, HL_HINT);
            break;
        case Route::rsWfUnlock:
            emit showLogMessage(tr("Route '%1' waiting for release")
                    .arg(rt->getName()), MT_INFO, HL_HINT);
            break;
        case Route::rsLocking:
            emit showLogMessage(tr("Route '%1' is in activating state")
                    .arg(rt->getName()), MT_INFO, HL_HINT);
            break;
    }
}


void Router::activateRouteAt(unsigned int index)
{
    Route* rt = getRouteAt(index);
    activateRoute(rt);
}


void Router::activateRoute(Route* rt)
{
    if (rt == NULL || visualmode == kvmEditLayout)
        return;


    int result = rt->startRouting();
    switch (result) {
        case 1:
            processRouteState(rt, Route::rsLocked);
            break;
        case 0: 
            QApplication::beep();
            emit showLogMessage(tr("No routing possible; "
                        "route '%1' is locked by an other route.")
                    .arg(rt->getName()), MT_INFO, HL_HINT);
            break;
        case -1: 
            QApplication::beep();
            emit showLogMessage(tr("No routing possible; "
                        "route '%1' is blocked by occupied element.")
                    .arg(rt->getName()), MT_INFO, HL_HINT);
            break;
        case -2: 
            QApplication::beep();
            emit showLogMessage(tr("No routing possible; "
                        "route '%1' is blocked by occupied turnout.")
                    .arg(rt->getName()), MT_INFO, HL_HINT);
            break;
    }
}


void Router::releaseRouteAt(unsigned int index)
{
    Route* rt = getRouteAt(index);
    releaseRoute(rt);
}


void Router::releaseRoute(Route* rt)
{
    if (rt == NULL || visualmode == kvmEditLayout)
        return;

    rt->stopRouting();
    processRouteState(rt, Route::rsUnlocked);
}


/*
   old error messages
   case kNormal:
   (tr("No normal route found for entry signal '%1'!").arg(s.data()));
   case kDetour:
   (tr("No detour route found for entry signal '%1'!").arg(s.data()));
   case kHelp:
   (tr("No help route found for entry signal '%1'!").arg(s.data()));
   case kShunting:
   (tr("No shunting route found for entry signal '%1'!").arg(s.data()));
   case kShuntingD:
   (tr("No detour shunting route found for entry signal '%1'!").arg(s.data()));
*/
void Router::setRoute(element* el, GbsButtonState cb, GbsButtonState sb)
{
    if (el == NULL)
        return;

    /*check if entry signal is allready choosen*/
    if (selectedStartSig == NULL) {
        Route* sr = getUnlockedRouteWithEntrySignal(el, cb, sb);
        if (sr != NULL) {
            selectedStartSig = el;
            lastcb = cb;
            // send signal to gbs to change mouse cursor
            emit startRouteTimer((Route::RouteType)sr->getType());
        }
        else {
            /*TODO: more detailed error message*/
            QApplication::beep();
            /*send cursor time out to gbs*/
            emit routeFunctionFinished();
            emit showLogMessage(tr("No matching route found for entry "
                        "signal '%1'").arg(el->getLabelText()),
                    MT_INFO, HL_HINT);
        }
    }
    /*exit signal button is pressed*/
    else {
        if (lastcb != cb && !((lastcb == kZhsClicked && cb == kZfsClicked)
                    || (cb == kZhsClicked && lastcb == kZfsClicked))) {
            QApplication::beep();
            emit showLogMessage(tr("Mixing signal buttons of different"
                        " type is not allowed."), MT_INFO, HL_HINT);
        }
        else {
            Route* sr = getUnlockedRouteWithExitSignal(el, cb, sb);
            if (sr != NULL)
                activateRoute(sr);
            else {
                QApplication::beep();
                emit showLogMessage(tr("No matching route found from '%1'"
                            " to '%2'").arg(selectedStartSig->getLabelText())
                                .arg(el->getLabelText()), MT_INFO, HL_HINT);
            }
        }
        selectedStartSig = NULL;
        /*send cursor time out to gbs*/
        emit routeFunctionFinished();
    }
}


void Router::resetRoute(element* el, GbsButtonState cb)
{
    if (el == NULL)
        return;

    /*check if is route to reset is allready choosen*/
    if (resetRt == NULL) {
        resetRt = getLockedRouteWithEntrySignal(el);
        if (resetRt != NULL) {
            lastcb = cb;
            selectedStartSig = el;
            // send signal to gbs to change mouse cursor
            emit startRouteTimer((Route::RouteType)resetRt->getType());
        }
        else {
            QApplication::beep();
            emit showLogMessage(tr("No active route found for entry "
                        "signal '%1'").arg(el->getLabelText()),
                    MT_INFO, HL_HINT);
            emit routeFunctionFinished();
        }
    }
    else {
        if (lastcb != cb) {
            QApplication::beep();
            emit showLogMessage(tr("Mixing signal buttons of different"
                        " type is not allowed."), MT_INFO, HL_HINT);
        }
        else {
            //check if selected route has same exit signal
            if (resetRt->hasThisExitSignal(el)) {
                resetRt->stopRouting();
                // send signal to routing viewer to update state icon
                emit routeStateChanged(resetRt);
                emit showLogMessage(tr("Route '%1' released")
                        .arg(resetRt->getName()), MT_INFO, HL_HINT);
            }
            else {
                QApplication::beep();
                if (selectedStartSig != NULL)
                    emit showLogMessage(tr("No active route found from "
                                "entry signal '%1' to exit signal '%2'")
                            .arg(selectedStartSig->getLabelText())
                            .arg(el->getLabelText()), MT_INFO, HL_HINT);
            }
        }
        /*send cursor time out to gbs*/
        emit routeFunctionFinished();
        resetRt = NULL;
        selectedStartSig = NULL;
    }
}


Route* Router::getLockedRouteWithEntrySignal(element* el)
{
    QPtrListIterator<Route> routeit(routeList);
    Route* rt;
    while ((rt = routeit.current()) != 0 ) {
        ++routeit;
        if (rt->isLockedWithEntrySignal(el))
            return rt;
    }
    return NULL;
}


Route* Router::getUnlockedRouteWithEntrySignal(element* el, GbsButtonState cb,
        GbsButtonState sb)
{
    QPtrListIterator<Route> routeit(routeList);
    Route* rt;
    while ((rt = routeit.current()) != 0 ) {
        ++routeit;
        if (rt->isUnlockedWithEntrySignalType(el, cb, sb))
            return rt;
    }
    return NULL;
}


Route* Router::getUnlockedRouteWithExitSignal(element* el, GbsButtonState cb,
        GbsButtonState sb)
{
    QPtrListIterator<Route> routeit(routeList);
    Route* rt;
    while ((rt = routeit.current()) != 0 ) {
        ++routeit;
        if (rt->isUnlockedType(selectedStartSig, el, cb, sb))
            return rt;
    }
    return NULL;
}


void Router::resetSelectedSignal()
{
    selectedStartSig = NULL;
    resetRt = NULL;
}


void Router::unlockAllLockedRoutes()
{
    QPtrListIterator<Route> routeit(routeList);
    Route* rt;
    int index = 0;
    while ((rt = routeit.current()) != 0 ) {
        ++routeit;
        if (rt->getState() != Route::rsUnlocked) {
            rt->stopRouting();
            // send signal to routing viewer to update state icon
            emit routeStateChanged(rt);
        }
        ++index;
    }
    emit showLogMessage(tr("All active routes released"), MT_INFO, HL_HINT);
}


void Router::feedbackPortChanged(unsigned int bus, unsigned int port,
    bool ison)
{
    QPtrListIterator<Route> routeit(routeList);
    Route* rt;

    /*first release locked routes*/
    while ((rt = routeit.current()) != 0 ) {
        ++routeit;
        if (rt->canReleaseByFeedbackPort(bus, port, ison))
            releaseRoute(rt);
    }
    
    /*second activate unlocked routes*/
    routeit.toFirst();
    while ((rt = routeit.current()) != 0 ) {
        ++routeit;
        if (rt->canActivateByFeedbackPort(bus, port, ison))
            activateRoute(rt);
    }
}


bool Router::editRouteAt(QWidget* owner, int index)
{
    return editRoute(owner, routeList.at(index));
}


bool Router::editRoute(QWidget* owner, Route* er)
{
    bool isEdited = false;
    
    if (er != NULL) {
        connect(er, SIGNAL(getElementByAddress(const int, const int,
                        element**)), this,
                SIGNAL(getElementByAddress(const int, const int,
                        element**)));
        
        isEdited = er->runEditRouteDialog(owner);

        disconnect(er, SIGNAL(getElementByAddress(const int,
                        const int, element**)), this,
                SIGNAL(getElementByAddress(const int, const int,
                        element**)));
        if (isEdited)
            modified = true;
    }
    return isEdited;
}

