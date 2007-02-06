/***************************************************************************
                           router.h
                           version 0.5.1 $Revision: 1.19 $
                           -------------------------------
    copyright            : (C) 2004-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-02-06 20:49:15 $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
  This code implements the router object. This object cares for the routing
  list, loads it from file, save it to file, shows routing edit window.
 ***************************************************************************/

#ifndef ROUTER_H
#define ROUTER_H

#include <qptrlist.h>
#include <qptrvector.h>

#include "route.h"
#include "element.h"

#define RF_OLDROUTEEXT ".dat.rts"


class Router: public QObject
{
    Q_OBJECT
        
public:
    Router(QObject* parent=0, const char* name=0);
    Router(QObject* parent=0, QPtrVector<element>* elPtr=0, const char* name=0);
    ~Router();
    void readFileTextFromStream(QTextStream&);
    void writeFileTextToStream(QTextStream&);
    unsigned int getRouteCount();
    Route* addNewRoute();
    Route* getRouteAt(unsigned int);
    void deleteRouteAt(unsigned int);
    void deleteRoute(Route*);
    void copyRouteAt(unsigned int);
    Route* copyRoute(Route*);
    bool editRouteAt(QWidget*, int);
    bool editRoute(QWidget*, Route*);
    bool isModified();
    void clear();
    void setElementListPtr(QPtrVector<element>*);
    void selectedRouteChanged(Route*);
    void showRouteAt(int);
    void startRecordModeAt(unsigned int);
    void activateRoute(Route*);
    void activateRouteAt(unsigned int);
    void releaseRoute(Route*);
    void releaseRouteAt(unsigned int);

public slots:
    void clearRoutes();
    void processRouteState(Route*, int);
    void recordElement(element*, elemRecordType);
    void resetRoute(element*, GbsButtonState);
    void resetSelectedSignal();
    void setRoute(element*, GbsButtonState, GbsButtonState);
    void switchVisualMode(elemVisualMode);
    void unlockAllLockedRoutes();
    void feedbackPortChanged(unsigned int, unsigned int, bool);
    
private:
    QPtrVector<element>* gbsElements;
    QPtrList<Route> routeList;
    Route* selectedRoute;
    Route* resetRt;
    element* selectedStartSig;
    bool modified;
    elemVisualMode visualmode;
    GbsButtonState lastcb;

    void setupRouteElements();
    void updateRouteElements();
    Route* getLockedRouteWithEntrySignal(element*);
    Route* getUnlockedRouteWithEntrySignal(element*, GbsButtonState,
            GbsButtonState);
    Route* getUnlockedRouteWithExitSignal(element*, GbsButtonState,
            GbsButtonState);
    
signals:
    void getElementByAddress(const int, const int, element**);
    void routeFunctionFinished();
    void routeDataChanged(Route*);
    void routeListChanged();
    void routeStateChanged(Route*);
    void showLogMessage(const QString&, int, int);
    void startRouteTimer(TypeOfRoute);
    void updateRoutePathLEDs(const stateElement&, const stateElement&,
            RouteSetAction&);
};
#endif // ROUTER_H

