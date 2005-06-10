/***************************************************************************
                           router.h
                           version 0.4.8 $Revision: 1.8 $
                           -------------------------------
    copyright            : (C) 2004-2005 by Guido Scholz
    email                : guido.scholz@ bayernline.de
    last modified        : $Date: 2005-06-10 15:52:47 $
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
    void importFile(const QString&);
    void readFileTextFromStream(QTextStream&);
    void writeFileTextToStream(QTextStream&);
    unsigned int getRouteCount();
    unsigned int addNewRoute();
    Route* getRouteAt(unsigned int);
    void deleteRouteAt(unsigned int);
    void copyRouteAt(unsigned int);
    bool isModified();
    void clear();
    void setElementListPtr(QPtrVector<element>*);
    void selectedRouteChanged(int, int);
    void showRouteAt(int);
    void startRecordModeAt(unsigned int);
    bool activateRoute(Route*);
    bool activateRouteAt(unsigned int);

public slots:
    void clearRoutes();
    void recordElement(element*, elemRecordType);
    void resetRoute(element*, GbsButtonState);
    void resetSelectedSignal();
    void setRoute(element*, GbsButtonState, GbsButtonState);
    void switchVisualMode(elemVisualMode);
    void unlockAllLockedRoutes();
    void feedbackPortChanged(unsigned int);
    
private:
    QPtrVector<element>* gbsElements;
    QPtrList<Route> routeList;
    Route* recRoute;
    Route* resRoute;
    element* selectedStartSig;
    bool modified;
    elemVisualMode visualmode;
    GbsButtonState lastcb;

    void setupRouteElements();
    Route* getLockedRouteWithStartSignal(element*);
    Route* getUnlockedRouteWithStartSignal(element*, GbsButtonState,
            GbsButtonState);
    Route* getUnlockedRouteWithStopSignal(element*, GbsButtonState,
            GbsButtonState);
    
signals:
    void updateRoutingViewer();
    void updateRoutingViewerAt(int);
    void routeFunctionFinished();
    void showLogMessage(const QString&, int, int);
    void startRouteTimer(TypeOfRoute);
    void updateRoutePathLEDs(const stateElement&, const stateElement&,
            RouteSetAction&);
};
#endif // ROUTER_H

