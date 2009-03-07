/***************************************************************************
                           route.cpp
                           version 0.5.5 $Revision: 1.79 $
                           -------------------------------
    copyright            : (C) 2004-2009 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2009-03-07 12:49:36 $
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

#include "element.h"
#include "preferences.h"
#include "route.h"
#include "routedialog.h"

/*some magic strings for reading and writing route data to files*/
static const char RF_ID[]           = "id";
static const char RF_NAME[]         = "name";
static const char RF_TRAIN[]        = "train";
static const char RF_ENTRYSIGNAL[]  = "from signal";
static const char RF_EXITSIGNAL[]   = "to signal";
static const char RF_TNDISPLAY[]    = "trainnumberdisplay";
static const char RF_SWITCHXTOY[]   = "switch x to y";
static const char RF_ACTIVATEPORT[] = "activate port";
static const char RF_RELEASEPORT[]  = "release port";
static const char RF_TYPE[]         = "type";


Route::Route(unsigned int anid,
        RouteType arouteType,
        const QString& aName,
        const stateElement& aexitSignal,
        const stateElement& aentrySignal,
        const stateElement& tnDisplay,
        const PortState& arePort,
        const PortState& aacPort,
        const QPtrList<stateElement>& swis,
        QObject* parent, const char* name): Section(aName, anid, 0, parent, name)
{
    initVariables();

    routeType = arouteType;

    exitSignal.name = aexitSignal.name;
    exitSignal.bus = aexitSignal.bus;
    exitSignal.address = aexitSignal.address;
    exitSignal.state = aexitSignal.state;
    exitSignal.elemPtr = aexitSignal.elemPtr;
    
    entrySignal.name = aentrySignal.name;
    entrySignal.bus = aentrySignal.bus;
    entrySignal.address = aentrySignal.address;
    entrySignal.state = aentrySignal.state;
    entrySignal.elemPtr = aentrySignal.elemPtr;
    
    trainNumberDisplay.name = tnDisplay.name;
    trainNumberDisplay.bus = tnDisplay.bus;
    trainNumberDisplay.address = tnDisplay.address;
    trainNumberDisplay.state = tnDisplay.state;
    trainNumberDisplay.elemPtr = tnDisplay.elemPtr;
    
    rePort.used = arePort.used;
    rePort.switchtooff = arePort.switchtooff;
    rePort.bus = arePort.bus;
    rePort.address = arePort.address;
    
    acPort.used = aacPort.used;
    acPort.switchtooff = aacPort.switchtooff;
    acPort.bus = aacPort.bus;
    acPort.address = aacPort.address;
    
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

            switchItems.append(el);
        }
        //else no memory available
    }
}

Route::Route(element* startEl, QObject* parent, const char* name)
: Section(tr("New route"), 0, 0, parent, name)
{
    initVariables();

    exitSignal.state = 0;
    exitSignal.address = 0;
    exitSignal.bus = 0;
    exitSignal.name = "";
    exitSignal.elemPtr = NULL;
    
    trainNumberDisplay.state = 0;
    trainNumberDisplay.address = 0;
    trainNumberDisplay.bus = 0;
    trainNumberDisplay.name = "";
    trainNumberDisplay.elemPtr = NULL;
    
    rePort.used = false;
    rePort.switchtooff = false;
    rePort.bus = 1;
    rePort.address = 0;
    
    acPort.used = false;
    acPort.switchtooff = false;
    acPort.bus = 1;
    acPort.address = 0;
    
    if (startEl != NULL) {
        sectionName.append(startEl->getLabelText());
        startEl->getStateData(entrySignal);
        //select route type element name dependent
        if (startEl->hasShuntingRouteButtonOnly())
            routeType = rtRRS;
    }
    else {
        entrySignal.state = 0;
        entrySignal.address = 0;
        entrySignal.bus = 0;
        entrySignal.name = "";
        entrySignal.elemPtr = NULL;
    }
}

Route::Route(QTextStream& ts, QObject* parent, const char* name)
: Section(tr("noname"), 0, 0, parent, name)
{
    initVariables();

    /*exit signals are red by default*/
    exitSignal.state = 0;
    exitSignal.name = "";
    exitSignal.elemPtr = NULL;

    entrySignal.name = "";
    entrySignal.elemPtr = NULL;
    
    trainNumberDisplay.state = 0;
    trainNumberDisplay.address = 0;
    trainNumberDisplay.bus = 0;
    trainNumberDisplay.name = "";
    trainNumberDisplay.elemPtr = NULL;

    readFileTextFromStream(ts);
}


Route::Route(const QString& aName, QObject* parent, const char* name)
    : Section(aName, 0, 0, parent, name)
{
    initVariables();

    exitSignal.name = "";
    exitSignal.bus = 1;
    exitSignal.address = 0;
    exitSignal.state = 0;
    exitSignal.elemPtr = NULL;
    
    entrySignal.name = "";
    entrySignal.bus = 1;
    entrySignal.address = 0;
    entrySignal.state = 0;
    entrySignal.elemPtr = NULL;
    
    trainNumberDisplay.name = "";
    trainNumberDisplay.bus = 1;
    trainNumberDisplay.address = 0;
    trainNumberDisplay.state = 0;
    trainNumberDisplay.elemPtr = NULL;
    
    rePort.used = false;
    rePort.switchtooff = false;
    rePort.bus = 1;
    rePort.address = 0;
    
    acPort.used = false;
    acPort.switchtooff = false;
    acPort.bus = 1;
    acPort.address = 0;
}


Route::~Route()
{
    switchItems.clear();
}

/*
 * set all variables to init values
 */
void Route::initVariables()
{
    Section::initVariables();

    // dynamic data
    routestate = rsUnlocked;
    turnouts = 0;
    tocounter = 0;
    triggerto = NULL;
    switchItems.setAutoDelete(true);

    // user selectable data
    routeType = rtRZS;
    replyid = 0;
}
/* 
 * Update route element names and pointers when layout was edited
 */
void Route::updateElementList(QPtrVector<element>* elements)
{
    if (elements == NULL)
        return;

    // first clear all element names and pointers
    entrySignal.name = tr("Error");
    entrySignal.elemPtr = NULL;

    exitSignal.name = tr("Error");
    exitSignal.elemPtr = NULL;
    
    trainNumberDisplay.name = tr("Error");
    trainNumberDisplay.elemPtr = NULL;

    QPtrListIterator<stateElement> it(switchItems);
    stateElement* swElement;
    while ((swElement = it.current()) != 0) {
        ++it;

        swElement->name = tr("Error");
        swElement->elemPtr = NULL;
    }

    // now assign the right values
    setupElementList(elements);
}


void Route::setupElementList(QPtrVector<element>* elements)
{
    if (elements == NULL)
        return;

    for (unsigned int i = 0; i < elements->size(); i++) {
        element* el = elements->at(i);

        if (el == NULL)
            continue;

        /*first add switchable elements "between" entry and exit signals*/
        QPtrListIterator<stateElement> it(switchItems);
        stateElement* swElement;
        while ((swElement = it.current()) != 0) {
            ++it;
        
            if (el->hasSameAddress(swElement->bus,
                        swElement->address)) {
                swElement->name = el->getLabelText();
                swElement->elemPtr = el;
                break;
            }
        }

        /*add exit signal*/
        // FIXME: this finds also Vr-types with same address, check also
        // route start/ end point
        if (el->isRouteMark() && el->hasSameAddress(exitSignal.bus,
                    exitSignal.address)) {
            exitSignal.name = el->getLabelText();
            exitSignal.elemPtr = el;
        }

        /*add entry signal*/
        // FIXME: this finds also Vr-types with same address
        if (el->isRouteMark() && el->hasSameAddress(entrySignal.bus,
                    entrySignal.address)) {
            entrySignal.name = el->getLabelText();
            entrySignal.elemPtr = el;
        }

        /*add train number dislay*/
        if (el->hasSameAddress(trainNumberDisplay.bus,
                    trainNumberDisplay.address)) {
            /*
            fprintf(stderr, "Route: %s, Train: %d, Display: %d %d\n",
                    sectionName.data(), trainid, trainNumberDisplay.bus,
                    trainNumberDisplay.address);
                    */
            trainNumberDisplay.elemPtr = el;
        }
    }

    // update only displays of utilized routes
    if (trainid != 0)
        updateTrainNumberDisplay();
}


void Route::readFileTextFromStream(QTextStream& ts)
{
    QString s, key;

    while (!ts.eof()) {
        s = ts.readLine();
        if (!s.startsWith("#")) {
            key = s.section(DS, 0, 0);
            /* key/value pairs are read sequence independent */
            if (key.compare(RF_ID) == 0){
                sectionid = s.section(DS, 1, 1).toUInt();
            }
            else if (key.compare(RF_NAME) == 0){
                sectionName = s.section(DS, 1, 1);
            }
            else if (key.compare(RF_TRAIN) == 0){
                trainid = s.section(DS, 1, 1).toUInt();
            }
            else if (key.compare(RF_EXITSIGNAL) == 0){
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
                    switchElement->name = "";

                    switchItems.append(switchElement);
                }
                // else no memory available
            }
            else if (key.compare(RF_ENTRYSIGNAL) == 0){
                entrySignal.bus = s.section(DS, 1, 1).toUInt();
                entrySignal.address = s.section(DS, 2, 2).toUInt();
                entrySignal.state = s.section(DS, 3, 3).toUInt();
            }
            else if (key.compare(RF_TNDISPLAY) == 0){
                trainNumberDisplay.bus = s.section(DS, 1, 1).toUInt();
                trainNumberDisplay.address = s.section(DS, 2, 2).toUInt();
                trainNumberDisplay.state = s.section(DS, 3, 3).toUInt();
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
            else if (key.compare(RF_TYPE) == 0){
                routeType = (RouteType)s.section(DS, 1, 1).toUInt();
            }
            /*end of route data*/
            else if (s.startsWith("%%"))
                break;
        }
    }
}


void Route::writeFileTextToStream(QTextStream& ts)
{
    ts
        << RF_ID << DS << sectionid << endl
        << RF_NAME << DS << sectionName << endl
        << RF_TRAIN << DS << trainid << endl
        << RF_EXITSIGNAL << DS << exitSignal.bus << DS
        << exitSignal.address << endl
        << RF_ENTRYSIGNAL << DS << entrySignal.bus << DS
        << entrySignal.address << DS << entrySignal.state << endl 
        << RF_TNDISPLAY << DS << trainNumberDisplay.bus << DS
        << trainNumberDisplay.address << DS << trainNumberDisplay.state << endl 
        << RF_RELEASEPORT << DS << rePort.bus << DS << rePort.address
        << DS << rePort.used << DS << rePort.switchtooff << endl
        << RF_ACTIVATEPORT << DS << acPort.bus << DS << acPort.address
        << DS << acPort.used << DS << acPort.switchtooff << endl
        << RF_TYPE << DS << routeType << endl;

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
    return new Route(sectionid, routeType, sectionName, exitSignal,
            entrySignal, trainNumberDisplay, rePort, acPort, 
            switchItems, parent(), "clonedRoute");
}


/*function to update route state visualisation*/
void Route::setState(Route::RouteState state)
{
    if (routestate != state) {
        RouteSetAction rsa = rsaReset;
        routestate = state;

        if (routestate != rsUnlocked) {
            if (routeType == rtRRS || routeType == rtURS)
                rsa = rsaRfs;
            else
                rsa = rsaZfs;
        }

        emit updateRoutePathLEDs(entrySignal, exitSignal, rsa);
        emit stateChanged(this, routestate);
    }
}


int Route::getState()
{
    return routestate;
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


void Route::setType(RouteType t)
{
    if (routeType != t)
        routeType = t;
}


int Route::getType() const
{
    return routeType;
}


QString Route::getTypeStr() const
{
    QString typeStr;

    /*returns decoded route type */
    switch (routeType){
        case rtRZS:
            typeStr = QString(QObject::tr("NR")); // normal route
            break;
        case rtUZS:                               // detour route
            typeStr = QString(QObject::tr("DR"));
            break;
        case rtZHS:
            typeStr = QString(QObject::tr("HR")); // help route
            break;
        case rtRRS:
            typeStr = QString(QObject::tr("NS")); // normal shunting
            break;
        case rtURS:                               // detour shunting
            typeStr = QString(QObject::tr("DS"));
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

    if (exitSignal.elemPtr != NULL && exitSignal.elemPtr->isLocked() &&
                exitSignal.elemPtr->hasDifferentState(exitSignal.state))
               return 0;

    turnouts = 0;
    tocounter = 0;

    QPtrListIterator<stateElement> it(switchItems);
    stateElement* se;
    while ((se = it.current()) != 0) {
        ++it;
        element* el = se->elemPtr;
        if (el != NULL && el->isLocked() &&
                (el->hasDifferentState(se->state) ||
                 el->is2StateDKW() || !el->isLockable()))
            return 0;

        // count turnouts for timer activation
        if (el != NULL && el->isTurnout() &&
                el->hasDifferentState(se->state)) {
            ++turnouts;
            el->setSwitched(false);
            if (el->isOccupied())
                return -2;
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

    // start timer controlled turnout switching
    switchTurnouts();
    return 1;
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

            if (el != NULL && el->isTurnout() && !el->isSwitched()) {
                if (el->hasDifferentState(se->state)) {

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
    RouteSetAction rsa = rsaZfs;
    if (routeType == rtRRS || routeType == rtURS) {
        rsa = rsaRfs;
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
    if (rsaReset == rsa) {
        routestate = rsWfLock;
        emit stateChanged(this, routestate);
        return;
    }

    /* 
     * FIXME:
     * 4) lock all switchable elements; in original SpDr this is done
     * with step 3) but is too complicated to implement respecting
     * interruption by occupied elements for "Zugfahrstrassen" and the
     * necessary unlocking of already locked elements
     */
    if (entrySignal.elemPtr != NULL)
        entrySignal.elemPtr->setLocked(true);

    /*exit signal does not need locking*/
    
    QPtrListIterator<stateElement> it(switchItems);
    stateElement* se;
    while ((se = it.current()) != 0) {
        ++it;
        element* el = se->elemPtr;
        if (el != NULL)
            el->setLocked(true);
    }

    /*
     * 5) activate "Fahrstrassenfestlegemelder" (FfM) at entry signal,
     *    shunting routes do not have an active FfM
     */
    if (entrySignal.elemPtr != NULL)
           entrySignal.elemPtr->activateFfM((routeType != rtRRS &&
                       routeType != rtURS));

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
            }
    }

    /* 7) at last switch entry signal to Hp1/Sh1 etc.*/
    if (entrySignal.elemPtr != NULL)
        entrySignal.elemPtr->switchToDir(entrySignal.state);

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
    }

    /*
     * update route path element LEDs and
     * send signal to gbs to change route path LEDs
     */
    RouteSetAction rsa = rsaReset;
    emit updateRoutePathLEDs(entrySignal, exitSignal, rsa);

    routestate = rsUnlocked;
    emit stateChanged(this, routestate);
}


void Route::hideRoute()
{
    if (exitSignal.elemPtr != NULL)
        exitSignal.elemPtr->switchSelectionMode(ksmNormal);

    if (entrySignal.elemPtr != NULL) {
        entrySignal.elemPtr->switchToDir(0);
        entrySignal.elemPtr->switchSelectionMode(ksmNormal);
    }

    if (trainNumberDisplay.elemPtr != NULL) {
        trainNumberDisplay.elemPtr->updateTrainNumber(0);
        trainNumberDisplay.elemPtr->switchSelectionMode(ksmNormal);
    }

    QPtrListIterator<stateElement> it(switchItems);
    stateElement* swe;

    while ((swe = it.current()) != 0) {
        ++it;
        if (swe->elemPtr != NULL)
            swe->elemPtr->switchSelectionMode(ksmNormal);
    }
}


void Route::showRoute()
{
    if (exitSignal.elemPtr != NULL)
        exitSignal.elemPtr->showElementState(exitSignal.state,
                ksmStopSig);

    if (entrySignal.elemPtr != NULL)
        entrySignal.elemPtr->showElementState(entrySignal.state,
                ksmStartSig);

    if (trainNumberDisplay.elemPtr != NULL) {
        trainNumberDisplay.elemPtr->updateTrainNumber(trainid);
        trainNumberDisplay.elemPtr->showElementState(-1, ksmDisplay);
    }

    QPtrListIterator<stateElement> it(switchItems);
    stateElement* swe;

    while ((swe = it.current()) != 0) {
        ++it;
        if (swe->elemPtr != NULL)
            swe->elemPtr->showElementState(swe->state, ksmSwitchEl);
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
            sectionName = QString("%1 - %2").arg(entrySignal.name)
                .arg(exitSignal.name);
        else
            sectionName = QString(tr("New route from %1")
                    .arg(entrySignal.name));
    }
    else {
        if (hasExitSignal())
            sectionName = QString(tr("New route to %1").arg(exitSignal.name));
        else
            sectionName = tr("New route");
    }
}

/* 
 * In route edit mode try to guess what type of route is recorded,
 * but only respecting changes from default rtRZS to new rtRRS.
 */
void Route::updateRouteType()
{
    if (rtRZS != routeType)
        return;
    
    if (hasEntrySignal() &&
            entrySignal.elemPtr->hasShuntingRouteButtonOnly()) {
        routeType = rtRRS;
        return;
    }
    
    if (hasExitSignal() &&
            exitSignal.elemPtr->hasShuntingRouteButtonOnly())
        routeType = rtRRS;
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
            updateRouteName();
            break;

        case (ksmStopSig):
            exitSignal.state = 0;
            exitSignal.address = 0;
            exitSignal.bus = 0;
            exitSignal.name = "";
            exitSignal.elemPtr = NULL;
            updateRouteName();
            break;

        case (ksmDisplay):
            el->updateTrainNumber(0);
            trainNumberDisplay.state = 0;
            trainNumberDisplay.address = 0;
            trainNumberDisplay.bus = 0;
            trainNumberDisplay.name = "";
            trainNumberDisplay.elemPtr = NULL;
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
    return (exitSignal.elemPtr == el);
}


bool Route::isLockedWithEntrySignal(element* el)
{
    return routestate != rsUnlocked &&
        (entrySignal.elemPtr == el);
}


bool Route::isUnlockedWithEntrySignalType(element* el, GbsButtonState cb,
        GbsButtonState sb)
{
    /* Function matrix

       route type    c-button       s-button
       ---------------------------------------
         rtRZS      kZfsClicked   kNoneClicked 
         rtUZS      kZfsClicked   kUfgtClicked
         rtZHS      kZhsClicked   kNoneClicked
                    kZfsClicked   kNoneClicked
         rtRRS      kRfsClicked   kNoneClicked
         rtURS      kRfsClicked   kUfgtClicked
       ---------------------------------------
    */
    if (routestate == rsUnlocked && (entrySignal.elemPtr == el)) {
        
        bool returnvalue = false;
        switch (routeType){
            case rtRZS:
                returnvalue = (kZfsClicked == cb && kNoneClicked == sb);
                break;
            case rtUZS:
                returnvalue = (kZfsClicked == cb && kUfgtClicked == sb);
                break;
            case rtZHS:
                returnvalue = (kZhsClicked == cb || kZfsClicked == cb )
                    && kNoneClicked == sb;
                break;
            case rtRRS:
                returnvalue = (kRfsClicked == cb && kNoneClicked == sb);
                break;
            case rtURS:
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
       ---------------------------------------
         rtRZS      kZfsClicked   kNoneClicked 
         rtUZS      kZfsClicked   kUfgtClicked
         rtZHS      kZhsClicked   kNoneClicked
                    kZfsClicked   kNoneClicked
         rtRRS      kRfsClicked   kNoneClicked
         rtURS      kRfsClicked   kUfgtClicked
       ---------------------------------------
    */
    if (routestate == rsUnlocked && (entrySignal.elemPtr == fel) &&
            (exitSignal.elemPtr == tel)) {
        
        bool returnvalue = false;
        switch (routeType){
            case rtRZS:
                returnvalue = (kZfsClicked == cb && kNoneClicked == sb);
                break;
            case rtUZS:
                returnvalue = (kZfsClicked == cb && kUfgtClicked == sb);
                break;
            case rtZHS:
                returnvalue = (kZhsClicked == cb || kZfsClicked == cb )
                    && kNoneClicked == sb;
                break;
            case rtRRS:
                returnvalue = (kRfsClicked == cb && kNoneClicked == sb);
                break;
            case rtURS:
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


bool Route::runEditDialog(QWidget* dlgparent)
{
    bool returnvalue = false;
    
    RouteDialog* rtDlg = new RouteDialog(dlgparent);

    if (rtDlg == NULL)
        return returnvalue;

    connect(rtDlg, SIGNAL(getElementByAddress(const int, const int,
                    element**)), this,
            SIGNAL(getElementByAddress(const int, const int,
                    element**)));
    rtDlg->setRouteNumber(sectionid);
    rtDlg->setRouteTrain(trainid);
    rtDlg->setRouteName(sectionName);
    rtDlg->setEntrySignalData(entrySignal);
    rtDlg->setExitSignalData(exitSignal);
    rtDlg->setActivateData(acPort);
    rtDlg->setReleaseData(rePort);
    rtDlg->setRouteType(routeType);
    rtDlg->setRouteElements(switchItems);

    if (rtDlg->exec() == QDialog::Accepted) {
        // update gbs: 1) hide old route 2) show new route
        hideRoute();
        sectionid = rtDlg->getRouteNumber();
        setTrain(rtDlg->getRouteTrain());
        sectionName = rtDlg->getRouteName();
        rtDlg->getEntrySignalData(entrySignal);
        rtDlg->getExitSignalData(exitSignal);
        rtDlg->getActivateData(acPort);
        rtDlg->getReleaseData(rePort);
        routeType = (RouteType) rtDlg->getRouteType();
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

/*
 * return pointer to entry signal element
 */
element* Route::getEntrySignalElementPtr()
{
    return entrySignal.elemPtr;
}

/*assemble CRCF route info message string*/
QString Route::getCrcfInfoMessage(CrcfMessage::CrcfAttribute at) const
{
    unsigned int result = 0;

    switch (at) {
        case CrcfMessage::atId:
            result = sectionid;
            break;
        case CrcfMessage::atName:
            return CrcfMessage::message(CrcfMessage::acRoute, sectionid,
                    CrcfMessage::meInfo, at, sectionName);
            break;
        case CrcfMessage::atState:
            result = routestate == rsUnlocked ? 0 : 1;
            break;
        case CrcfMessage::atType:
            result = routeType;
            break;
        case CrcfMessage::atTrain:
            result = trainid;
            break;
        default:
            return "";
            break;
    }

    return CrcfMessage::message(CrcfMessage::acRoute, sectionid,
            CrcfMessage::meInfo, at, result);
}

void Route::setCrcfMessageReplyId(unsigned int id)
{
    replyid = id;
}

unsigned int Route::getCrcfMessageReplyId()
{
    return replyid;
}

bool Route::isCrcfMessageRequested()
{
    return (replyid != 0);
}

