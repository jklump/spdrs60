/***************************************************************************
                           router.h
                           version 0.5.3 $Revision: 1.31 $
                           -------------------------------
    copyright            : (C) 2004-2008 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-07-31 18:21:32 $
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

#include "crcfmessage.h"
#include "element.h"
#include "route.h"
#include "srcpmessage.h"

#define RF_OLDROUTEEXT ".dat.rts"


class Router: public QObject
{
    Q_OBJECT
        
public:
    Router(QObject* parent = NULL, const char* name = NULL);
    Router(QObject* parent = NULL, QPtrVector<element>* elPtr = NULL,
            const char* name = NULL);
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
    void withdrawRoute(Route*);
    void setInfoSessionId(unsigned int);
    void setServerHasGm(bool);
    bool serverHasGm();
    void processGenericMessage(unsigned int, unsigned int,
            const CrcfMessage*);

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
    void changeTrainNumber(unsigned int, unsigned int);
    
private:
    QPtrVector<element>* gbsElements;
    QPtrList<Route> routeList;
    Route* selectedRoute;
    Route* resetRt;
    element* selectedStartSig;
    bool modified;
    bool serverhasgm;
    elemVisualMode visualmode;
    GbsButtonState lastcb;
    unsigned int infosessionid;

    void initVariables();
    void setupRouteElements();
    void updateRouteElements();
    Route* getRouteWithId(unsigned int);
    //Route* getRouteWithSection(unsigned int);
    //Route* getRouteWithTrain(unsigned int);
    Route* getLockedRouteWithEntrySignal(element*);
    Route* getUnlockedRouteWithEntrySignal(element*, GbsButtonState,
            GbsButtonState);
    Route* getUnlockedRouteWithExitSignal(element*, GbsButtonState,
            GbsButtonState);
    Route* getUtilizedRouteWithExitSignal(element*);
    void transferTrainNumber(Route*);
    unsigned int getMaximumRouteIdNumber();
    void sendGmCrcfMessage(unsigned int, unsigned int, const QString&);
    
signals:
    void getElementByAddress(const int, const int, element**);
    void routeFunctionFinished();
    void routeDataChanged(Route*);
    void routeListChanged();
    void routeStateChanged(Route*);
    void sendSrcpMessage(SrcpMessage*);
    void statusMessage(const QString&);
    void startRouteTimer(Route::RouteType);
    void updateRoutePathLEDs(const stateElement&, const stateElement&,
            Route::RouteSetAction&);
};
#endif // ROUTER_H

