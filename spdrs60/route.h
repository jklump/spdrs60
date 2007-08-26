/***************************************************************************
                           route.h
                           version 0.5.2 $Revision: 1.26 $
                           -------------------------------
    copyright            : (C) 2004-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-08-26 15:08:12 $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/******************************************************************************
   This is the header file for route.cpp.
 ******************************************************************************/

#ifndef ROUTE_H
#define ROUTE_H

#include <qptrlist.h>
#include <qtextstream.h>
#include <qptrvector.h>

#include "element.h"

/*some magic strings for reading and writing routing files*/
#define RF_ID           "id"
#define RF_NAME         "name"
#define RF_FROMSIGNAL   "from signal"
#define RF_TOSIGNAL     "to signal"
#define RF_SWITCHXTOY   "switch x to y"
#define RF_ACTIVATEPORT "activate port"
#define RF_RELEASEPORT  "release port"
#define RF_ACTIVATEPORT "activate port"
#define RF_ACTIVATELOCO "active by loco"
#define RF_TYPE         "type"
#define RF_DETOURLEVEL  "level"


struct PortState {
    bool used;
    bool switchtooff;
    unsigned int bus, address;
};

struct Loco {
    unsigned int bus, address;
};

/* we have to differentiate between following route types:
 *
 * - Zugstrassen
 *   + Regelzugstrasse      -> RZS (0)  normal route
 *   + Umfahrzugstrasse     -> UZS (1)  detour route
 *   + Zughilfsstrasse      -> ZHS (2)  help route
 *
 * - Rangierstrassen
 *   + Regelrangierstrasse  -> RRS (3)  normal shunting route
 *   + Umfahrrangierstrasse -> URS (4)  detour shunting route
 *
 */

class Route: public QObject
{
    Q_OBJECT
        
public:
    enum RouteState {rsUnlocked = 0, rsLocked, rsWfLock, rsWfUnlock,
        rsLocking};

    enum RouteType {RZS = 0, UZS, ZHS, RRS, URS};

    /* Type of routing action for route path highlighting */
    enum RouteSetAction {krouteReset = 0, krouteZfs, krouteRfs};
        
    Route(unsigned int anid,
          RouteType arouteType,
          const QString& aName,
          const stateElement& aexitSignal,
          const stateElement& aentrySignal,
          const PortState& arePort,
          const PortState& aacPort,
          const Loco& aacLoco,
          unsigned int adetourLevel,
          const QPtrList<stateElement>& swis);
    
    Route(element* = 0);
    Route(QTextStream&, bool isNewFormat = false);
    Route(const QString& aName);
    ~Route();
    
    void readFileTextFromStream(QTextStream&);
    void readOldFileTextFromStream(QTextStream&);
    void writeFileTextToStream(QTextStream&);
    Route* getClone();
    int getState();
    unsigned int getId();
    QString getName() const;
    QString getFromSignalName() const;
    QString getToSignalName() const;
    int getType() const;
    QString getTypeStr() const;
    int startRouting();
    void stopRouting();
    void hideRoute();
    void showRoute();
    void viewRoute();
    bool runEditRouteDialog(QWidget*);
    void setupElementLists(QPtrVector<element>*);
    void updateElementLists(QPtrVector<element>*);
    bool isLockedWithEntrySignal(element*);
    bool isUnlockedWithEntrySignalType(element*, GbsButtonState,
            GbsButtonState);
    bool isUnlockedType(element*, element*, GbsButtonState,
            GbsButtonState);
    bool hasExitSignal();
    bool hasEntrySignal();
    bool hasThisExitSignal(element*);
    void setEntrySignal(element*);
    void setExitSignal(element*);
    void addSwitchElement(element*);
    void removeElement(element*);
    bool canActivateByFeedbackPort(unsigned int, unsigned int, bool);
    bool canReleaseByFeedbackPort(unsigned int, unsigned int, bool);
    
signals:
    void stateChanged(Route*, int);
    void showElement(int, int, int);
    void updateRoutePathLEDs(const stateElement&, const stateElement&,
            RouteSetAction&);
    void getElementByAddress(const int, const int, element**);

public slots:
    void switchTurnouts();
    void showRoutePath();
    
private:
    QString Name;
    stateElement exitSignal, entrySignal;
    RouteType routeType;
    PortState acPort, rePort;
    Loco acLoco;
    unsigned int idnumber;
    unsigned int detourLevel;
    RouteState routestate;
    int turnouts;
    int tocounter;
    element* triggerto;

    /*list with raw item data*/
    QPtrList<stateElement> switchItems;
    void updateRouteName();
    void updateRouteType();
};
#endif // ROUTE_H

