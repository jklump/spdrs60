/***************************************************************************
                           route.cpp
                           version 0.5.0 $Revision: 1.40 $
                           -------------------------------
    copyright            : (C) 2004-2006 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2006-08-30 17:25:02 $
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
   This code implements the route object.
 ***************************************************************************/

#include <stdlib.h> // for abs()
#include <unistd.h> // for usleep()

#include "preferences.h"
#include "route.h"
#include "routedialog.h"

Route::Route(TypeOfRoute arouteType,
        const QString& aName,
        const stateElement& aexitSignal,
        const stateElement& aentrySignal,
        const PortState& arePort,
        const PortState& aacPort,
        const Loco& aacLoco,
        unsigned int adetourLevel,
        const QPtrList<stateElement>& swis)
{
    routestate = rsUnlocked;
    turnouts = 0;
    tocounter = 0;
    triggerto = NULL;
    switchItems.setAutoDelete(true);

    routeType = arouteType;
    Name = aName;

    exitSignal.name = aexitSignal.name;
    exitSignal.bus = aexitSignal.bus;
    exitSignal.address = aexitSignal.address;
    exitSignal.state = aexitSignal.state;
    exitSignal.elemPtr = aexitSignal.elemPtr;
    exitSignal.elemPtr2 = aexitSignal.elemPtr2;
    
    entrySignal.name = aentrySignal.name;
    entrySignal.bus = aentrySignal.bus;
    entrySignal.address = aentrySignal.address;
    entrySignal.state = aentrySignal.state;
    entrySignal.elemPtr = aentrySignal.elemPtr;
    entrySignal.elemPtr2 = aentrySignal.elemPtr2;
    
    rePort.used = arePort.used;
    rePort.switchtooff = arePort.switchtooff;
    rePort.bus = arePort.bus;
    rePort.address = arePort.address;
    
    acPort.used = aacPort.used;
    acPort.switchtooff = aacPort.switchtooff;
    acPort.bus = aacPort.bus;
    acPort.address = aacPort.address;
    
    acLoco.bus = aacLoco.bus;
    acLoco.address = aacLoco.address;
    detourLevel = adetourLevel;

    /*copy switchitem list*/
    QPtrListIterator<stateElement> it(swis);
    stateElement* swElement;
    while ((swElement = it.current()) != 0) {
        ++it;

        stateElement* el = new stateElement;
        if (el != NULL) {
            el->name = swElement->name;
            el->bus = swElement->bus;
            el->address = swElement->address;
            el->state = swElement->state;
            el->elemPtr = swElement->elemPtr;
            el->elemPtr2 = swElement->elemPtr2;

            switchItems.append(el);
        }
        //else no memory available
    }
}

Route::Route(element* startEl)
{
    routestate = rsUnlocked;
    turnouts = 0;
    tocounter = 0;
    triggerto = NULL;
    switchItems.setAutoDelete(true);

    exitSignal.state = 0;
    exitSignal.address = 0;
    exitSignal.bus = 0;
    exitSignal.name = "";
    exitSignal.elemPtr = NULL;
    exitSignal.elemPtr2 = NULL;
    
    routeType = RZS;

    rePort.used = false;
    rePort.switchtooff = false;
    rePort.bus = 1;
    rePort.address = 0;
    
    acPort.used = false;
    acPort.switchtooff = false;
    acPort.bus = 1;
    acPort.address = 0;
    
    acLoco.bus = 0;
    acLoco.address = 0;
    detourLevel = 0;

    Name = tr("New route");
    if (startEl != NULL) {
        Name.append(startEl->getName());
        startEl->getStateData(entrySignal);
        //select route type element name dependent
        if (startEl->hasShuntingRouteButtonOnly())
            routeType = RRS;
    }
    else {
        entrySignal.state = 0;
        entrySignal.address = 0;
        entrySignal.bus = 0;
        entrySignal.name = "";
        entrySignal.elemPtr = NULL;
        entrySignal.elemPtr2 = NULL;
    }
}

Route::Route(QTextStream& ts, bool isNewFormat)
{
    routestate = rsUnlocked;
    turnouts = 0;
    tocounter = 0;
    triggerto = NULL;
    switchItems.setAutoDelete(true);

    /*exit signals are red by default*/
    exitSignal.state = 0;
    exitSignal.name = "";
    exitSignal.elemPtr = NULL;
    exitSignal.elemPtr2 = NULL;
    entrySignal.name = "";
    entrySignal.elemPtr = NULL;
    entrySignal.elemPtr2 = NULL;

    if (isNewFormat)
        readFileTextFromStream(ts);
    else
        readOldFileTextFromStream(ts);
}


Route::Route(const QString& aName)
{
    routestate = rsUnlocked;
    turnouts = 0;
    tocounter = 0;
    triggerto = NULL;
    switchItems.setAutoDelete(true);

    routeType = RZS;
    Name = aName;

    exitSignal.name = "";
    exitSignal.bus = 1;
    exitSignal.address = 0;
    exitSignal.state = 0;
    exitSignal.elemPtr = NULL;
    exitSignal.elemPtr2 = NULL;
    
    entrySignal.name = "";
    entrySignal.bus = 1;
    entrySignal.address = 0;
    entrySignal.state = 0;
    entrySignal.elemPtr = NULL;
    entrySignal.elemPtr2 = NULL;
    
    rePort.used = false;
    rePort.switchtooff = false;
    rePort.bus = 1;
    rePort.address = 0;
    
    acPort.used = false;
    acPort.switchtooff = false;
    acPort.bus = 1;
    acPort.address = 0;
    
    acLoco.bus = 1;
    acLoco.address = 0;
    detourLevel = 0;
}


Route::~Route()
{
    switchItems.clear();
}


void Route::setupElementLists(QPtrVector<element>* elements)
{
    if (elements == NULL)
        return;

    for (unsigned int i = 0; i < elements->size(); i++) {
        element* gbse = elements->at(i);

        /*first add switchable elements "between" entry and exit signals*/
        QPtrListIterator<stateElement> it(switchItems);
        stateElement* swElement;
        while ((swElement = it.current()) != 0) {
            ++it;
        
            if ((gbse != 0) && gbse->hasSameAddress(swElement->bus,
                        swElement->address)) {
                swElement->name = gbse->getName();
                if (swElement->elemPtr == NULL)
                    swElement->elemPtr = gbse;
                else {
                    /*may be this element is twice on layout*/
                    swElement->elemPtr2 = gbse;
                    break;
                }
            }
        }

        /*add exit signal*/
        if ((gbse != 0) && gbse->hasSameAddress(exitSignal.bus,
                    exitSignal.address)) {
            //routePathItems.append(gbse);
            exitSignal.name = gbse->getName();
            if (exitSignal.elemPtr == NULL)
                exitSignal.elemPtr = gbse;
            else {
                /*may be this signal is twice on layout*/
                exitSignal.elemPtr2 = gbse;
            }
        }

        /*add entry signal*/
        if ((gbse != 0) && gbse->hasSameAddress(entrySignal.bus,
                    entrySignal.address)) {
            //routePathItems.append(gbse);
            entrySignal.name = gbse->getName();
            if (entrySignal.elemPtr == NULL)
                entrySignal.elemPtr = gbse;
            else {
                /*may be this signal is twice on layout*/
                entrySignal.elemPtr2 = gbse;
            }
        }
    }
}


void Route::readFileTextFromStream(QTextStream& ts)
{
    QString s, key;

    while (!ts.eof()) {
        s = ts.readLine();
        if (!s.startsWith("#")) {
            key = s.section(DS, 0, 0);
            /* key/value pairs are read sequence independent */
            if (key.compare(RF_NAME) == 0){
                Name = s.section(DS, 1, 1);
            }
            else if (key.compare(RF_TOSIGNAL) == 0){
                exitSignal.bus = s.section(DS, 1, 1).toUInt();
                exitSignal.address = s.section(DS, 2, 2).toUInt();
            }
            else if (key.compare(RF_SWITCHXTOY) == 0){
                stateElement *switchElement = new stateElement;
                
                if (switchElement != NULL) {
                    switchElement->bus = s.section(DS, 1, 1).toUInt();
                    switchElement->address = s.section(DS, 2, 2).toUInt();
                    switchElement->state = s.section(DS, 3, 3).toUInt();
                    switchElement->elemPtr = NULL;
                    switchElement->elemPtr2 = NULL;
                    switchElement->name = "";

                    switchItems.append(switchElement);
                }
                // else no memory available
            }
            else if (key.compare(RF_FROMSIGNAL) == 0){
                entrySignal.bus = s.section(DS, 1, 1).toUInt();
                entrySignal.address = s.section(DS, 2, 2).toUInt();
                entrySignal.state = s.section(DS, 3, 3).toUInt();
            }
            else if (key.compare(RF_RELEASEPORT) == 0){
                rePort.bus = s.section(DS, 1, 1).toUInt();
                rePort.address = s.section(DS, 2, 2).toUInt();
                rePort.used = (s.section(DS, 3, 3).toInt() == 1);
                rePort.switchtooff = (s.section(DS, 4, 4).toInt() == 1);
            }
            else if (key.compare(RF_ACTIVATEPORT) == 0){
                acPort.bus = s.section(DS, 1, 1).toUInt();
                acPort.address = s.section(DS, 2, 2).toUInt();
                acPort.used = (s.section(DS, 3, 3).toInt() == 1);
                acPort.switchtooff = (s.section(DS, 4, 4).toInt() == 1);
            }
            else if (key.compare(RF_ACTIVATELOCO) == 0){
                acLoco.bus = s.section(DS, 1, 1).toUInt();
                acLoco.address = s.section(DS, 2, 2).toUInt();
            }
            else if (key.compare(RF_TYPE) == 0){
                routeType = (TypeOfRoute)s.section(DS, 1, 1).toUInt();
                detourLevel = s.section(DS, 2, 2).toUInt();
            }
            /*end of route data*/
            else if (s.startsWith("%%"))
                break;
        }
    }
}


void Route::readOldFileTextFromStream(QTextStream& ts)
{    
    QString s, key, value;
    int intvalue = 0;

    while (!ts.eof()) {
        s = ts.readLine();
        if (!s.startsWith("#")) {
            key = s.section(IDS, 0, 0);
            value = s.section(IDS, 1, 1).stripWhiteSpace();
            /* key/value pairs are read sequence independent */
            if (key.compare(RF_NAME) == 0){
                  Name = value.stripWhiteSpace();
            }
            else if (key.compare(RF_TOSIGNAL) == 0){
                exitSignal.bus = 1;
                exitSignal.address = value.toUInt();
            }
            else if (key.compare(RF_SWITCHXTOY) == 0){
                stateElement* switchElement = new stateElement;
                
                switchElement->bus = 1;
                switchElement->address = value.section(" ", 0, 0).toUInt();
                switchElement->state = value.section(" ", 1, 1).toUInt();
                switchElement->elemPtr = NULL;
                switchElement->elemPtr2 = NULL;
                switchElement->name = "";
                
                switchItems.append(switchElement);
            }
            else if (key.compare(RF_FROMSIGNAL) == 0){
                entrySignal.bus = 1;
                entrySignal.address = value.section(" ", 0, 0).toUInt();
                entrySignal.state = value.section(" ", 1, 1).toUInt();
            }
            else if (key.compare(RF_RELEASEPORT) == 0){
                rePort.switchtooff = false;
                intvalue = value.toInt();
                if (intvalue <= -1) {
                    rePort.address = 0;
                    rePort.bus = 1;
                    rePort.used = false;
                }
                else {
                    rePort.address = intvalue % 496 + 1;
                    rePort.bus = intvalue / 496 + 1;
                    rePort.used = true;
                }
            }
            else if (key.compare(RF_ACTIVATEPORT) == 0){
                acPort.switchtooff = false;
                intvalue = value.toInt();
                if (intvalue == -1) {
                    acPort.address = 0;
                    acPort.bus = 1;
                    acPort.used = false;
                }
                else {
                    acPort.address = intvalue % 496 + 1;
                    acPort.bus = intvalue / 496 + 1;
                    acPort.used = true;
                }
            }
            else if (key.compare(RF_ACTIVATELOCO) == 0){
                acLoco.bus = 1;
                intvalue = value.toInt();
                if (intvalue == -1)
                    acLoco.address = 0;
                else
                    acLoco.address = (unsigned int)intvalue;
            }
            else if (key.compare(RF_TYPE) == 0){
                routeType = (TypeOfRoute)value.toUInt();
            }
            else if (key.compare(RF_DETOURLEVEL) == 0){
                intvalue = value.toInt();
                if (intvalue == -1)
                    detourLevel = 0;
                else
                    detourLevel = (unsigned int)intvalue;
                /*this is the last parameter, now exit while loop*/
                break;

            }
        }
    }
}


void Route::writeFileTextToStream(QTextStream& ts)
{
    ts
        << RF_NAME << DS << Name << endl
        << RF_TOSIGNAL << DS << exitSignal.bus << DS << exitSignal.address << endl
        << RF_FROMSIGNAL << DS << entrySignal.bus << DS
        << entrySignal.address << DS << entrySignal.state << endl 
        << RF_RELEASEPORT << DS << rePort.bus << DS << rePort.address
        << DS << rePort.used << DS << rePort.switchtooff << endl
        << RF_ACTIVATEPORT << DS << acPort.bus << DS << acPort.address
        << DS << acPort.used << DS << acPort.switchtooff << endl
        << RF_ACTIVATELOCO << DS << acLoco.bus << DS << acLoco.address << endl
        << RF_TYPE << DS << routeType << DS << detourLevel << endl;

    QPtrListIterator<stateElement> it(switchItems);
    stateElement* swElement;
    while ((swElement = it.current()) != 0) {
        ++it;
        ts
            << RF_SWITCHXTOY << DS
            << swElement->bus << DS
            << swElement->address << DS
            << swElement->state << endl;
    }
}


Route* Route::getClone()
{
    return new Route(routeType, Name, exitSignal, entrySignal,
            rePort, acPort, acLoco, detourLevel, switchItems);
}


int Route::getState()
{
    return routestate;
}


QString Route::getName() const
{
    return Name;
}


QString Route::getFromSignalName() const
{
    if (entrySignal.name.isEmpty())
        return QString::number(entrySignal.address);
    else
        return entrySignal.name;
}


QString Route::getToSignalName() const
{
    if (exitSignal.name.isEmpty())
        return QString::number(exitSignal.address);
    else
        return exitSignal.name;
}


TypeOfRoute Route::getType()
{
    return routeType;
}


QString Route::getTypeStr() const
{
    QString typeStr;

    /*returns decoded route type */
    switch (routeType){
        case RZS:
            typeStr = QString(QObject::tr("NR")); // normal route
            break;
        case UZS:                                 // detour route
            typeStr = QString(QObject::tr("DR%1").arg(detourLevel));
            break;
        case ZHS:
            typeStr = QString(QObject::tr("HR")); // help route
            break;
        case RRS:
            typeStr = QString(QObject::tr("NS")); // normal shunting
            break;
        case URS:                                 // detour shunting
            typeStr = QString(QObject::tr("DS%1").arg(detourLevel));
            break;
    }
    return typeStr;
}

/**
 * Start an unlocked route. This function returns tree differend values,
 * depending on processing results.
 *   -2: routing is interrupted because a turnout to switch is occupied
 *   -1: routing is interrupted because nonshunting route leads over
         occupied elements
 *    0: routing is interrupted due to a locked turnout/signal
 *    1: routing is OK
 */
int Route::startRouting()
{
    /*
     * 1) check for locked elements
     */
    if (entrySignal.elemPtr != NULL && entrySignal.elemPtr->isLocked())
               return 0;

    if (entrySignal.elemPtr2 != NULL && entrySignal.elemPtr2->isLocked())
               return 0;

    if (exitSignal.elemPtr != NULL && exitSignal.elemPtr->isLocked() &&
                exitSignal.elemPtr->hasDifferentDirection(exitSignal.state))
               return 0;

    if (exitSignal.elemPtr2 != NULL && exitSignal.elemPtr2->isLocked() &&
                exitSignal.elemPtr2->hasDifferentDirection(exitSignal.state))
               return 0;

    turnouts = 0;
    tocounter = 0;

    QPtrListIterator<stateElement> it(switchItems);
    stateElement* se;
    while ((se = it.current()) != 0) {
        ++it;
        element* el = se->elemPtr;
        if (el != NULL && el->isLocked() &&
                (el->hasDifferentDirection(se->state) ||
                 el->is2StateDKW()))
            return 0;
        // count turnouts for timer activation
        if (el != NULL && el->isTurnout() &&
                el->hasDifferentDirection(se->state)) {
            ++turnouts;
            el->setSwitched(false);
            if (el->isOccupied())
                return -2;

        el = se->elemPtr2;
        if (el != NULL && el->isLocked() &&
                (el->hasDifferentDirection(se->state) ||
                 el->is2StateDKW()))
            return 0;

        }
    }
    //fprintf(stderr, "counter: %d  turnouts: %d\n", tocounter, turnouts);

    /*
     * 2) switch route elements but without locking
     */
    if (exitSignal.elemPtr != NULL) {
           exitSignal.elemPtr->switchToDir(exitSignal.state);
           exitSignal.elemPtr->repaint();
    }

    if (exitSignal.elemPtr2 != NULL) {
           exitSignal.elemPtr2->switchToDir(exitSignal.state);
           exitSignal.elemPtr2->repaint();
    }
    // start timer controlled turnout switching
    switchTurnouts();
}

/* 
 * Timer controlled loop over all turnouts.
 * The original SpDr waits 250 ms until next turnout is
 * switched to avoid high power consumption. Signals on
 * route path are switched after "Fahrstrassenfestlegemelder".
 * Option to force turnout switching: prf.sendstate
 */
void Route::switchTurnouts()
{
    if (turnouts > 0) {
        QPtrListIterator<stateElement> it(switchItems);
        stateElement* se;
        while ((se = it.current()) != 0) {
            ++it;
            element* el = se->elemPtr;

            if (el != NULL && el->isTurnout() && !el->isSwitched())
                if (el->hasDifferentDirection(se->state)) {

                    ++tocounter;
                    // last turnout will trigger route path highlighting
                    if (tocounter == turnouts) {
                        triggerto = el;
                        connect(el, SIGNAL(turnoutIsSwitched()),
                                this, SLOT(showRoutePath()));
                    }

                    el->switchToDir(se->state);
                    el->repaint();
                    el->setSwitched(true);
                    el = se->elemPtr2;
                    
                    if (el != NULL) {
                        el->switchToDir(se->state);
                        el->repaint();
                    }
                    
                    //fprintf(stderr, "counter: %d  turnouts: %d\n",
                    //        tocounter, turnouts);
                    if (tocounter < turnouts) {
                        QTimer::singleShot(250, this, SLOT(switchTurnouts()));
                    }
                }
                // no blink animation but command sending necessary
                else if (pref.sendstate)
                    el->sendSrcpState();
        }
    }
    // if there is no single turnout go ahead anyway
    else
        showRoutePath();
}


void Route::showRoutePath()
{
    // clean up
    if (triggerto != NULL) {
        disconnect(triggerto, SIGNAL(turnoutIsSwitched()),
                this, SLOT(showRoutePath()));
        triggerto = NULL;
    }

    /*
     * 3) switch route path element LEDs to yellow,
     *    check for occupied elements if not shunting route
     */
    RouteSetAction rsa = krouteZfs;
    if (routeType == RRS || routeType == URS) {
        rsa = krouteRfs;
    }
    // send signal to gbs to change route path LEDs
    emit updateRoutePathLEDs(entrySignal, exitSignal, rsa);
    
    // Interrupt routing if "Zugfahrstrasse" meets occupied element.
    // Lock state should not only be boolean but something like "waiting
    // for lock". The real lock state should be reached when all
    // occupied segments changed to be unoccupied.
    // route states: rsUnlocked, rsLocked, rsWfLock, rsWfUnlock
    // occupied tracks go this ways:
    //   rsUnlocked -> rsWfLock -> rsLocked 
    //   rsLocked -> rsWfUnlock -> rsUnlocked 
    if (krouteReset == rsa) {
        routestate = rsWfLock;
        emit stateChanged(this, routestate);
        return;
    }

    /* 
     * FIXME:
     * 4) lock all switchable elements; in original SpDr this is done
     * with step 3) but is too complicated to implement respecting
     * interruption by occupied elements for "Zugfahrstrassen" and the
     * necessary unlocking of allready locked elements
     */
    if (entrySignal.elemPtr != NULL)
        entrySignal.elemPtr->setLocked(true);

    if (entrySignal.elemPtr2 != NULL)
        entrySignal.elemPtr2->setLocked(true);

    /*exit signal does not need locking*/
    
    QPtrListIterator<stateElement> it(switchItems);
    stateElement* se;
    while ((se = it.current()) != 0) {
        ++it;
        element* el = se->elemPtr;
        if (el != NULL)
            el->setLocked(true);
        el = se->elemPtr2;
        if (el != NULL)
            el->setLocked(true);
    }

    /*
     * 5) activate "Fahrstrassenfestlegemelder" (FfM) at entry signal,
     *    shunting routes do not have an active FfM
     */
    if (entrySignal.elemPtr != NULL)
           entrySignal.elemPtr->activateFfM((routeType != RRS &&
                       routeType != URS));

    if (entrySignal.elemPtr2 != NULL)
           entrySignal.elemPtr2->activateFfM(routeType != RRS &&
                       routeType != URS);

    /*
     * 6) switch signals on route path to Sh1 (Siemens Type)
     * without changing lock state, also simple GA elements are switched
     * now
     */
    it.toFirst();
    while ((se = it.current()) != 0) {
        ++it;
        element* el = se->elemPtr;
        if (el != NULL)
            if (el->isSignal() || el->isSimpleGA()) {
                el->switchToDir(se->state);
                el->repaint();

                el = se->elemPtr2;
                if (el != NULL) {
                    el->switchToDir(se->state);
                    el->repaint();
                }
            }
    }

    /* 7) at last switch entry signal to Hp1/Sh1 etc.*/
    if (entrySignal.elemPtr != NULL)
        entrySignal.elemPtr->switchToDir(entrySignal.state);

    if (entrySignal.elemPtr2 != NULL)
        entrySignal.elemPtr2->switchToDir(entrySignal.state);

    routestate = rsLocked;
    emit stateChanged(this, routestate);
}


/**
 * Stop an active route. All signals are switched to red light,
 * turnouts keep current direction, a FfM is deactivated, locked
 * elements are unlocked.
 */
void Route::stopRouting()
{
    if (entrySignal.elemPtr != NULL) {
        entrySignal.elemPtr->activateFfM(false);
        entrySignal.elemPtr->switchToDir(0);
        //FIXME: temporary solution
        if (routestate == rsLocked)
            entrySignal.elemPtr->setLocked(false);
    }

    if (entrySignal.elemPtr2 != NULL) {
        entrySignal.elemPtr2->activateFfM(false);
        entrySignal.elemPtr2->switchToDir(0);
        entrySignal.elemPtr2->switchToDir(0);
        //FIXME: temporary solution
        if (routestate == rsLocked)
            entrySignal.elemPtr2->setLocked(false);
    }

    QPtrListIterator<stateElement> it(switchItems);
    stateElement* se;
    while ((se = it.current()) != 0) {
        ++it;
        element* el = se->elemPtr;
        if (el != NULL) {
            if (el->isSignal())
                el->switchToDir(0);
            //FIXME: temporary solution
            if (routestate == rsLocked)
                el->setLocked(false);
        }

        el = se->elemPtr2;
        if (el != NULL) {
            if (el->isSignal())
                el->switchToDir(0);
            //FIXME: temporary solution
            if (routestate == rsLocked)
                el->setLocked(false);
        }
    }

    /*
     * update route path element LEDs and
     * send signal to gbs to change route path LEDs
     */
    RouteSetAction rsa = krouteReset;
    emit updateRoutePathLEDs(entrySignal, exitSignal, rsa);

    routestate = rsUnlocked;
    emit stateChanged(this, routestate);
}


void Route::hideRoute()
{
    if (exitSignal.elemPtr != NULL)
           exitSignal.elemPtr->switchSelectionMode(ksmNormal);

    if (exitSignal.elemPtr2 != NULL)
           exitSignal.elemPtr2->switchSelectionMode(ksmNormal);

    if (entrySignal.elemPtr != NULL)
           entrySignal.elemPtr->switchSelectionMode(ksmNormal);

    if (entrySignal.elemPtr2 != NULL)
           entrySignal.elemPtr2->switchSelectionMode(ksmNormal);

    QPtrListIterator<stateElement> it(switchItems);
    stateElement* swe;

    while ((swe = it.current()) != 0) {
        ++it;
        if (swe->elemPtr != NULL)
            swe->elemPtr->switchSelectionMode(ksmNormal);
        if (swe->elemPtr2 != NULL)
            swe->elemPtr2->switchSelectionMode(ksmNormal);
    }
}


void Route::showRoute()
{
    if (exitSignal.elemPtr != NULL)
           exitSignal.elemPtr->showElementState(exitSignal.state,
                   ksmStopSig);

    if (exitSignal.elemPtr2 != NULL)
           exitSignal.elemPtr2->showElementState(exitSignal.state,
                   ksmStopSig);

    if (entrySignal.elemPtr != NULL)
           entrySignal.elemPtr->showElementState(entrySignal.state,
                   ksmStartSig);

    if (entrySignal.elemPtr2 != NULL)
           entrySignal.elemPtr2->showElementState(entrySignal.state,
                   ksmStartSig);

    QPtrListIterator<stateElement> it(switchItems);
    stateElement* swe;

    while ((swe = it.current()) != 0) {
        ++it;
        if (swe->elemPtr != NULL)
            swe->elemPtr->showElementState(swe->state, ksmSwitchEl);
        if (swe->elemPtr2 != NULL)
            swe->elemPtr2->showElementState(swe->state, ksmSwitchEl);
    }
}


void Route::setEntrySignal(element* el)
{
    el->getStateData(entrySignal);
    el->switchSelectionMode(ksmStartSig);
    updateRouteName();
    updateRouteType();
}


void Route::setExitSignal(element* el)
{
    el->getStateData(exitSignal);
    el->switchSelectionMode(ksmStopSig);
    updateRouteName();
    updateRouteType();
}


void Route::updateRouteName()
{
    if (hasEntrySignal()) {
        if (hasExitSignal()) 
            Name = QString("%1 - %2").arg(entrySignal.name).arg(exitSignal.name);
        else
            Name = QString(tr("New route from %1").arg(entrySignal.name));
    }
    else {
        if (hasExitSignal())
            Name = QString(tr("New route to %1").arg(exitSignal.name));
        else
            Name = tr("New route");
    }
}

/* 
 * In route edit mode try to guess what type of route is recorded,
 * but only respecting changes from default RZS to new RRS.
 */
void Route::updateRouteType()
{
    if (RZS != routeType)
        return;
    
    if (hasEntrySignal() &&
            entrySignal.elemPtr->hasShuntingRouteButtonOnly()) {
        routeType = RRS;
        return;
    }
    
    if (hasExitSignal() &&
            exitSignal.elemPtr->hasShuntingRouteButtonOnly())
        routeType = RRS;
}


void Route::addSwitchElement(element* el)
{
    stateElement* se = new stateElement;
    if (se != NULL) {
        el->getStateData(*se);
        switchItems.append(se);
        el->switchSelectionMode(ksmSwitchEl);
    }
    // else no memory available
}


void Route::removeElement(element* el)
{
    elemSelectionMode sm = el->getSelectionMode();
    switch (sm) {
        case (ksmStartSig):
            entrySignal.state = 0;
            entrySignal.address = 0;
            entrySignal.bus = 0;
            entrySignal.name = "";
            entrySignal.elemPtr = NULL;
            entrySignal.elemPtr2 = NULL;
            updateRouteName();
            break;
        case (ksmStopSig):
            exitSignal.state = 0;
            exitSignal.address = 0;
            exitSignal.bus = 0;
            exitSignal.name = "";
            exitSignal.elemPtr = NULL;
            exitSignal.elemPtr2 = NULL;
            updateRouteName();
            break;
        case (ksmSwitchEl):
            {
                bool found = false;
                QPtrListIterator<stateElement> it(switchItems);
                stateElement* swe;

                while ((swe = it.current()) != 0) {
                    ++it;
                    if (swe->elemPtr == el) {
                        found = true;
                        break;
                    }
                }
                if (found)
                        /*remove this item from list*/
                        switchItems.remove(swe);
                break;
            }
        case (ksmNormal):
            break;
        default:
            break;
    }
    el->switchSelectionMode(ksmNormal);
}


bool Route::hasEntrySignal()
{
    return entrySignal.elemPtr != NULL;
}


bool Route::hasExitSignal()
{
    return exitSignal.elemPtr != NULL;
}


bool Route::hasThisExitSignal(element* el)
{
    return (exitSignal.elemPtr == el || exitSignal.elemPtr2 == el);
}


bool Route::isLockedWithEntrySignal(element* el)
{
    return routestate != rsUnlocked &&
        (entrySignal.elemPtr == el || entrySignal.elemPtr2 == el);
}


bool Route::isUnlockedWithEntrySignalType(element* el, GbsButtonState cb,
        GbsButtonState sb)
{
    /* Function matrix

       route type    c-button       s-button
       ----------------------------------------
          RZS       kZfsClicked   kNoneClicked 
          UZS       kZfsClicked   kUfgtClicked
          ZHS       kZhsClicked   kNoneClicked
                    kZfsClicked   kNoneClicked
          RRS       kRfsClicked   kNoneClicked
          URS       kRfsClicked   kUfgtClicked
       ----------------------------------------
    */
    if (routestate == rsUnlocked && (entrySignal.elemPtr == el || 
                entrySignal.elemPtr2 == el)) {
        
        bool returnvalue = false;
        switch (routeType){
            case RZS:
                returnvalue = (kZfsClicked == cb && kNoneClicked == sb);
                break;
            case UZS:
                returnvalue = (kZfsClicked == cb && kUfgtClicked == sb);
                break;
            case ZHS:
                returnvalue = (kZhsClicked == cb || kZfsClicked == cb )
                    && kNoneClicked == sb;
                break;
            case RRS:
                returnvalue = (kRfsClicked == cb && kNoneClicked == sb);
                break;
            case URS:
                returnvalue = (kRfsClicked == cb && kUfgtClicked == sb);
                break;
            default:
                break;
        }
        return returnvalue;
    }
    else
        return false;
}


bool Route::isUnlockedType(element* fel, element* tel, GbsButtonState cb,
        GbsButtonState sb)
{
    /* Function matrix

       route type    c-button       s-button
       ----------------------------------------
          RZS       kZfsClicked   kNoneClicked 
          UZS       kZfsClicked   kUfgtClicked
          ZHS       kZhsClicked   kNoneClicked
                    kZfsClicked   kNoneClicked
          RRS       kRfsClicked   kNoneClicked
          URS       kRfsClicked   kUfgtClicked
       ----------------------------------------
    */
    if (routestate == rsUnlocked && (entrySignal.elemPtr == fel || 
                entrySignal.elemPtr2 == fel) && (exitSignal.elemPtr == tel || 
                    exitSignal.elemPtr2 == tel)) {
        
        bool returnvalue = false;
        switch (routeType){
            case RZS:
                returnvalue = (kZfsClicked == cb && kNoneClicked == sb);
                break;
            case UZS:
                returnvalue = (kZfsClicked == cb && kUfgtClicked == sb);
                break;
            case ZHS:
                returnvalue = (kZhsClicked == cb || kZfsClicked == cb )
                    && kNoneClicked == sb;
                break;
            case RRS:
                returnvalue = (kRfsClicked == cb && kNoneClicked == sb);
                break;
            case URS:
                returnvalue = (kRfsClicked == cb && kUfgtClicked == sb);
                break;
            default:
                break;
        }
        return returnvalue;
    }
    else
        return false;
}


/* 
  activate route action if conditions are given:

  used  switchtooff  ison  action
  -------------------------------
   1         0        0      0
   1         0        1      1
   1         1        0      1
   1         1        1      0
  -------------------------------
 */
bool Route::canActivateByFeedbackPort(unsigned int bus,
        unsigned int port, bool ison)
{
    return (routestate == rsUnlocked && acPort.used && acPort.bus == bus &&
        acPort.address == port && acPort.switchtooff != ison);
}


bool Route::canReleaseByFeedbackPort(unsigned int bus,
        unsigned int port, bool ison)
{
    return (routestate != rsUnlocked && rePort.used && rePort.bus == bus &&
        rePort.address == port && rePort.switchtooff != ison);
}


bool Route::runEditRouteDialog(QWidget* dlgparent)
{
    bool returnvalue = false;
    
    RouteDialog* rtDlg = new RouteDialog(dlgparent);

    if (rtDlg == NULL)
        return returnvalue;

    connect(rtDlg, SIGNAL(getElementByAddress(const int, const int,
                    element**)), this,
            SIGNAL(getElementByAddress(const int, const int,
                    element**)));
    rtDlg->setRouteName(Name);
    rtDlg->setEntrySignalData(entrySignal);
    rtDlg->setExitSignalData(exitSignal);
    rtDlg->setActivateData(acPort);
    rtDlg->setReleaseData(rePort);
    rtDlg->setRouteType(routeType, detourLevel);
    rtDlg->setRouteElements(switchItems);
    if (rtDlg->exec() == QDialog::Accepted) {
        // update gbs: 1) hide old route 2) show new route
        hideRoute();
        Name = rtDlg->getRouteName();
        rtDlg->getEntrySignalData(entrySignal);
        rtDlg->getExitSignalData(exitSignal);
        rtDlg->getActivateData(acPort);
        rtDlg->getReleaseData(rePort);
        routeType = (TypeOfRoute) rtDlg->getRouteType();
        detourLevel = rtDlg->getDetourLevel();
        switchItems.clear();
        rtDlg->getRouteElements(switchItems);
        showRoute();
        returnvalue = true;
    }
    disconnect(rtDlg, SIGNAL(getElementByAddress(const int,
                    const int, element**)), this,
            SIGNAL(getElementByAddress(const int, const int,
                    element**)));
    delete rtDlg;
    return returnvalue;
}

