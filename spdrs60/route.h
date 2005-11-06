/***************************************************************************
                           route.h
                           version 0.4.8 $Revision: 1.18 $
                           -------------------------------
    copyright            : (C) 2004-2005 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2005-11-06 20:49:44 $
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

enum TypeOfRoute {RZS = 0, UZS, ZHS, RRS, URS};

/* Type of routing action for route path highlighting */
enum RouteSetAction {krouteReset = 0, krouteZfs, krouteRfs};

class Route: public QObject
{
    Q_OBJECT
        
public:
    Route(TypeOfRoute arouteType,
          const QString& aName,
          const stateElement& atoSignal,
          const stateElement& afromSignal,
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
    bool isLocked();
    QString getName() const;
    QString getFromSignalName() const;
    QString getToSignalName() const;
    TypeOfRoute getType();
    QString getTypeStr() const;
    int startRouting();
    void stopRouting();
    void hideRoute();
    void showRoute();
    void viewRoute();
    bool runEditRouteDialog(QWidget*);
    void setupElementLists(QPtrVector<element>*);
    bool isLockedWithStartSignal(element*);
    bool isUnlockedWithStartSignalType(element*, GbsButtonState,
            GbsButtonState);
    bool isUnlockedType(element*, element*, GbsButtonState,
            GbsButtonState);
    bool hasStopSignal();
    bool hasStartSignal();
    bool hasThisStopSignal(element*);
    void setStartSignal(element*);
    void setStopSignal(element*);
    void addSwitchElement(element*);
    void removeElement(element*);
    bool canActivateByFeedbackPort(unsigned int, unsigned int, bool);
    bool canReleaseByFeedbackPort(unsigned int, unsigned int, bool);
    
signals:
    void showElement(int, int, int);
    void updateRoutePathLEDs(const stateElement&, const stateElement&,
            RouteSetAction&);
    void getElementByAddress(const int, const int, element**);

private:
    QString Name;
    stateElement toSignal, fromSignal;
    TypeOfRoute routeType;
    PortState acPort, rePort;
    Loco acLoco;
    unsigned int detourLevel;
    bool locked;

    /*list with raw item data*/
    QPtrList<stateElement> switchItems;
    void updateRouteName();
    void updateRouteType();
};
#endif // ROUTE_H

