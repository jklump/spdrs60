/***************************************************************************
                           route.h
                           version 0.5.3 $Revision: 1.39 $
                           -------------------------------
    copyright            : (C) 2004-2008 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-03-30 08:57:07 $
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

#include "crcfmessage.h"
#include "element.h"
#include "section.h"


struct PortState {
    bool used;
    bool switchtooff;
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

class Route: public Section
{
    Q_OBJECT
        
public:
    enum RouteState {rsUnlocked = 0, rsLocked, rsWfLock, rsWfUnlock,
        rsLocking};

    enum RouteType {rtRZS = 0, rtUZS, rtZHS, rtRRS, rtURS};

    /* Type of routing action for route path highlighting */
    enum RouteSetAction {rsaReset = 0, rsaZfs, rsaRfs};
        
    Route(unsigned int anid,
          RouteType arouteType,
          const QString& aName,
          const stateElement& aexitSignal,
          const stateElement& aentrySignal,
          const stateElement& tnDisplay,
          const PortState& arePort,
          const PortState& aacPort,
          unsigned int adetourLevel,
          const QPtrList<stateElement>& swis,
          QObject* parent = NULL, const char* name = NULL);
    
    Route(element* = NULL, QObject* parent = NULL,
            const char* name = NULL);
    Route(QTextStream&, QObject* parent = NULL,
            const char* name = NULL);
    Route(const QString& aName, QObject* parent = NULL,
            const char* name = NULL);
    ~Route();
    
    bool runEditDialog(QWidget*);
    void readFileTextFromStream(QTextStream&);
    void writeFileTextToStream(QTextStream&);
    Route* getClone();
    int getState();
    void setType(RouteType);
    int getType() const;
    QString getFromSignalName() const;
    QString getToSignalName() const;
    QString getTypeStr() const;
    QString getCrcfInfoMessage(CrcfMessage::CrcfAttribute) const;
    int startRouting();
    void stopRouting();
    void hideRoute();
    void showRoute();
    void viewRoute();
    void setupElementList(QPtrVector<element>*);
    void updateElementList(QPtrVector<element>*);
    bool isLockedWithEntrySignal(element*);
    bool isUnlockedWithEntrySignalType(element*, GbsButtonState,
            GbsButtonState);
    bool isUnlockedType(element*, element*, GbsButtonState,
            GbsButtonState);
    bool hasExitSignal();
    bool hasEntrySignal();
    bool hasThisExitSignal(element*);
    element* getEntrySignalElementPtr();
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
            Route::RouteSetAction&);
    void getElementByAddress(const int, const int, element**);

public slots:
    void switchTurnouts();
    void showRoutePath();
    
private:
    stateElement exitSignal, entrySignal;
    RouteType routeType;
    PortState acPort, rePort;
    unsigned int detourLevel;
    RouteState routestate;
    int turnouts;
    int tocounter;
    element* triggerto;

    /*list with raw item data*/
    QPtrList<stateElement> switchItems;

    void initVariables();
    void updateRouteName();
    void updateRouteType();
};
#endif // ROUTE_H

