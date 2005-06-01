/***************************************************************************
                           router.cpp
                           version 0.4.8 $Revision: 1.4 $
                           -------------------------------
    copyright            : (C) 2004-2005 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2005-06-01 20:25:34 $
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

#include "router.h"
#include "config.h" //for VERSION


Router::Router(QObject* parent, const char* name):
    QObject(parent, name)
{
    gbsElements = NULL;
    recRoute == NULL;
    selectedStartSig = NULL;
    routeList.setAutoDelete(true);
    modified = false;
    visualmode = kvmNormal;
}


Router::Router(QObject* parent, QPtrVector<element>* elPtr, const char* name):
    QObject(parent, name)
{
    recRoute == NULL;
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
    emit updateRoutingViewer();
}


void Router::importFile(const QString& fn)
{
    /*clear old list*/
    if (!routeList.isEmpty())
        routeList.clear();
    
    QFile routingfile(fn);
    if (!routingfile.exists()) {
        /*TODO: show error message "Route file '%1' not found"*/
        return;
    }

    if (!routingfile.open(IO_ReadOnly)) {
        /*TODO: show error message "Error reading route file '%1'"*/
        return;
    }

    QString s;
    QTextStream ts(&routingfile);

    while (!ts.eof()) {
        s = ts.readLine();
        /* ignore comment lines */
        if (!s.startsWith("#")) {
            /*here we read allways up to start marker of a new route*/
            if (s.startsWith("ROUTE"))
                routeList.append(new Route(ts, false));
        }
    }
    routingfile.close();
    /*if loaded from old file format, data is defined as "modified"*/
    modified = true;
    setupRouteElements();
    /*send update signal to routing viewer*/
    emit updateRoutingViewer();
}


void Router::setupRouteElements()
{
    if (gbsElements == NULL)
        return;

    QPtrListIterator<Route> routeit(routeList);
    Route* setuproute;
    while ((setuproute = routeit.current()) != 0 ) {
        ++routeit;
        setuproute->setupElementLists(gbsElements);
    }
}


unsigned int Router::getRouteCount()
{
    return routeList.count();
}


void Router::copyRouteAt(unsigned int index)
{
    Route* selectedRoute = getRouteAt(index);
    if (selectedRoute != NULL) {
        routeList.insert(index, selectedRoute->getClone());
        modified = true;
    }
}


void Router::deleteRouteAt(unsigned int index)
{
    routeList.remove(index);
    modified = true;
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
    emit updateRoutingViewer();
}


void Router::setElementListPtr(QPtrVector<element>* elp)
{
    gbsElements = elp;
}


void Router::selectedRouteChanged(int last, int current)
{
    if (visualmode == kvmEditRoute) {
        Route* cr = getRouteAt(last);
        if (cr != NULL)
            cr->hideRoute();

        cr = getRouteAt(current);
        if (cr != NULL) {
            cr->showRoute();
        }
        recRoute = cr;
    }
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
    visualmode = vm;
    if (vm != kvmEditRoute)
        recRoute = NULL;
    //TODO: get selected route from routingviewer
}


void Router::recordElement(element* el, elemRecordType rtype)
{
    if (visualmode == kvmEditRoute && recRoute != NULL) {
        switch (rtype) {
            case (krecStartStop):
                if (!recRoute->hasStartSignal())
                    recRoute->setStartSignal(el);
                else if (!recRoute->hasStopSignal())
                    recRoute->setStopSignal(el);
               
                /*send update signal to routingviewer to show altered
                  route name*/
                //TODO: update only last route entry to prevent flicker
                emit updateRoutingViewerAt(routeList.find(recRoute));
                break;
            case (krecNormal):
                recRoute->addSwitchElement(el);
                break;
            case (krecClear):
                recRoute->removeElement(el);
                break;
            default:
                break;
        }
    }
}


unsigned int Router::addNewRoute()
{
    routeList.append(new Route(tr("New Route")));
    return routeList.count();
}


void Router::startRecordModeAt(unsigned int index)
{
    recRoute = getRouteAt(index);
    showRouteAt(index);
}


void Router::setRoute(element* el, GbsButtonState cb, GbsButtonState sb)
{
    /*check if start signal is allready choosen*/
    if (selectedStartSig == NULL) {
        Route* sr = getUnlockedRouteWithStartSignal(el, cb, sb);
        if (sr != NULL) {
            selectedStartSig = el;
            // TODO: check for selected route: activeRoute = sr;
            emit startRouteTimer(sr->getType());
        }
        else {
            /*TODO: more detailed error message*/
            QApplication::beep();
            emit showLogMessage(tr("No matching route found for start "
                        "signal '%1'").arg(el->getName()), M_INFO, HIST);
            /*send cursor time out to gbs*/
            emit routeFunctionFinished();
        }
    }
    /*stop signal button is pressed*/
    else {
        Route* sr = getUnlockedRouteWithStopSignal(el, cb, sb);
        /*send cursor time out to gbs*/
        emit routeFunctionFinished();
        if (sr != NULL) {
            // activate route
            if (sr->startRouting()) {
                emit showLogMessage(tr("Activating route '%1'")
                        .arg(sr->getName()), M_INFO, HIST);
                // TODO: send state to routingviewer
            }
            else {
                QApplication::beep();
                emit showLogMessage(tr("No routing possible; "
                            "route '%1' is locked by another route.")
                        .arg(sr->getName()), M_INFO, HIST);
            }
        }
        else {
            QApplication::beep();
            emit showLogMessage(tr("No matching route found from '%1'"
                        "to '%2'").arg(selectedStartSig->getName(),
                            el->getName()), M_INFO, HIST);
        }
        selectedStartSig = NULL;
    }
}


void Router::resetRoute(element* el)
{
    Route* sr = getLockedRouteWithStartSignal(el);
    if (sr == NULL) {
        QApplication::beep();
        emit showLogMessage(tr("No active route found for start "
                    "signal '%1'").arg(el->getName()), M_INFO, HIST);
    }
    else {
        emit showLogMessage(tr("Resetting route '%1'")
                .arg(sr->getName()), M_INFO, HIST);
        sr->stopRouting();
    }
    // TODO: send state to routingviewer
}


Route* Router::getLockedRouteWithStartSignal(element* el)
{
    QPtrListIterator<Route> routeit(routeList);
    Route* rt;
    while ((rt = routeit.current()) != 0 ) {
        ++routeit;
        if (rt->isLockedWithStartSignal(el))
            break;
    }
    return rt;
}


Route* Router::getUnlockedRouteWithStartSignal(element* el, GbsButtonState cb,
        GbsButtonState sb)
{
    QPtrListIterator<Route> routeit(routeList);
    Route* rt;
    while ((rt = routeit.current()) != 0 ) {
        ++routeit;
        if (rt->isUnlockedWithStartSignalType(el, cb, sb))
            break;
    }
    return rt;
}


Route* Router::getUnlockedRouteWithStopSignal(element* el, GbsButtonState cb,
        GbsButtonState sb)
{
    QPtrListIterator<Route> routeit(routeList);
    Route* rt;
    while ((rt = routeit.current()) != 0 ) {
        ++routeit;
        if (rt->isUnlockedType(selectedStartSig, el, cb, sb))
            break;
    }
    return rt;
}


void Router::resetSelectedSignal()
{
    selectedStartSig = NULL;
}

