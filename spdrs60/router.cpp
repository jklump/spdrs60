/***************************************************************************
                           router.cpp
                           version 0.4.8 $Revision: 1.3 $
                           -------------------------------
    copyright            : (C) 2004-2005 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2005-05-31 19:54:49 $
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
    routeList.setAutoDelete(true);
    modified = false;
    visualmode = kvmNormal;
}


Router::Router(QObject* parent, QPtrVector<element>* elPtr, const char* name):
    QObject(parent, name)
{
    recRoute == NULL;
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
    //TODO: implement function
        //activeStartSignal = el;
        //activeRoute = sr;
        // emit showCursor();
}


void Router::resetRoute(element* el, GbsButtonState cb)
{
    Route* sr = getLockedRouteWithStartSignal(el, cb);
    if (sr == NULL) {
        emit showLogMessage(tr("No matching route found for start "
                    "signal '%1'").arg(el->getName()), M_INFO, HIST);
        emit routeFunctionFinished();
    }
    else {
        sr->stopRouting();
        emit routeFunctionFinished();
    }
}


Route* Router::getLockedRouteWithStartSignal(element* el, GbsButtonState cb)
{
    QPtrListIterator<Route> routeit(routeList);
    Route* rt;
    while ((rt = routeit.current()) != 0 ) {
        ++routeit;
        if (rt->hasMatchingStartSignal(el, cb) && rt->isLocked())
            break;
    }
    return rt;
}

