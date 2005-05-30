/***************************************************************************
                           route.cpp
                           version 0.4.8 $Revision: 1.2 $
                           -------------------------------
    copyright            : (C) 2004-2005 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2005-05-30 15:12:52 $
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

#include "route.h"


Route::Route(TypeOfRoute arouteType,
        const QString& aName,
        const stateElement& atoSignal,
        const stateElement& afromSignal,
        const Port& arePort,
        const Port& aacPort,
        const Loco& aacLoco,
        unsigned int adetourLevel,
        const QPtrList<stateElement>& swis)
{
    locked = false;
    switchItems.setAutoDelete(true);
    allGBSItemsList.setAutoDelete(false);

    routeType = arouteType;
    Name = aName;
    toSignal.name = atoSignal.name;
    toSignal.bus = atoSignal.bus;
    toSignal.address = atoSignal.address;
    toSignal.state = atoSignal.state;
    toSignal.elemPtr = atoSignal.elemPtr;
    toSignal.elemPtr2 = atoSignal.elemPtr2;
    fromSignal.bus = afromSignal.bus;
    fromSignal.address = afromSignal.address;
    fromSignal.state = afromSignal.state;
    fromSignal.elemPtr = afromSignal.elemPtr;
    fromSignal.elemPtr2 = afromSignal.elemPtr2;
    fromSignal.name = afromSignal.name;
    rePort.bus = arePort.bus;
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
        el->name = swElement->name;
        el->bus = swElement->bus;
        el->address = swElement->address;
        el->state = swElement->state;
        el->elemPtr = swElement->elemPtr;
        el->elemPtr2 = swElement->elemPtr2;

        switchItems.append(el);
    }
}

Route::Route(element* startEl)
{
    locked = false;
    switchItems.setAutoDelete(true);
    allGBSItemsList.setAutoDelete(false);

    toSignal.state = 0;
    toSignal.address = 0;
    toSignal.bus = 0;
    toSignal.name = "";
    toSignal.elemPtr = NULL;
    toSignal.elemPtr2 = NULL;
    routeType = RZS; //TODO: element name dependent
    rePort.bus = 0;
    acPort.address = 0;
    acLoco.bus = 0;
    acLoco.address = 0;
    detourLevel = 0;

    Name = tr("New Route from ");
    if (startEl != NULL) {
        Name.append(startEl->getName());
        startEl->getStateData(fromSignal);
    }
    else {
        fromSignal.state = 0;
        fromSignal.address = 0;
        fromSignal.bus = 0;
        fromSignal.name = "";
        fromSignal.elemPtr = NULL;
        fromSignal.elemPtr2 = NULL;
    }
}

Route::Route(QTextStream& ts, bool isNewFormat)
{
    locked = false;
    switchItems.setAutoDelete(true);
    allGBSItemsList.setAutoDelete(false);

    /*stop signals are red by default*/
    toSignal.state = 0;
    toSignal.name = "";
    toSignal.elemPtr = NULL;
    toSignal.elemPtr2 = NULL;
    fromSignal.name = "";
    fromSignal.elemPtr = NULL;
    fromSignal.elemPtr2 = NULL;

    if (isNewFormat)
        readFileTextFromStream(ts);
    else
        readOldFileTextFromStream(ts);
}


Route::Route(const QString& aName)
{
    locked = false;
    switchItems.setAutoDelete(true);
    allGBSItemsList.setAutoDelete(false);

    routeType = RZS;
    Name = aName;

    toSignal.name = "";
    toSignal.bus = 1;
    toSignal.address = 0;
    toSignal.state = 0;
    toSignal.elemPtr = NULL;
    toSignal.elemPtr2 = NULL;
    fromSignal.bus = 1;
    fromSignal.address = 0;
    fromSignal.state = 0;
    fromSignal.elemPtr = NULL;
    fromSignal.elemPtr2 = NULL;
    fromSignal.name = "";
    rePort.bus = 1;
    acPort.address = 0;
    acLoco.bus = 1;
    acLoco.address = 0;
    detourLevel = 0;
}


Route::~Route()
{
    switchItems.clear();
    allGBSItemsList.clear();
}


void Route::setupElementLists(QPtrVector<element>* elements)
{
    if (elements == NULL)
        return;

    for (unsigned int i = 0; i < elements->size(); i++) {
        element* gbse = elements->at(i);

        /*first add switchable elements "between" start and stop signals*/
        QPtrListIterator<stateElement> it(switchItems);
        stateElement* swElement;
        while ((swElement = it.current()) != 0) {
            ++it;
        
            if ((gbse != 0) && gbse->hasSameAddress(swElement->address)) {
                /*TODO: find elements between switchable elements*/
                allGBSItemsList.append(gbse);
                if (swElement->elemPtr == NULL)
                    swElement->elemPtr = gbse;
                else {
                    /*may be this element is twice on layout*/
                    swElement->elemPtr2 = gbse;
                    break;
                }
            }
        }

        /*add stop signal*/
        if ((gbse != 0) && gbse->hasSameAddress(toSignal.address)) {
            allGBSItemsList.append(gbse);
            toSignal.name = gbse->getName();
            if (toSignal.elemPtr == NULL)
                toSignal.elemPtr = gbse;
            else {
                /*may be this signal is twice on layout*/
                toSignal.elemPtr2 = gbse;
            }
        }

        /*add start signal*/
        if ((gbse != 0) && gbse->hasSameAddress(fromSignal.address)) {
            allGBSItemsList.append(gbse);
            fromSignal.name = gbse->getName();
            if (fromSignal.elemPtr == NULL)
                fromSignal.elemPtr = gbse;
            else {
                /*may be this signal is twice on layout*/
                fromSignal.elemPtr2 = gbse;
            }
        }
    }
    //fprintf(stderr, "Route: %s  allEl: %d\n", Name.data(),
    //        allGBSItemsList.count());
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
                toSignal.bus = s.section(DS, 1, 1).toUInt();
                toSignal.address = s.section(DS, 2, 2).toUInt();
            }
            else if (key.compare(RF_SWITCHXTOY) == 0){
                stateElement *switchElement = new stateElement;
                
                switchElement->bus = s.section(DS, 1, 1).toUInt();
                switchElement->address = s.section(DS, 2, 2).toUInt();
                switchElement->state = s.section(DS, 3, 3).toUInt();
                switchElement->elemPtr = NULL;
                switchElement->elemPtr2 = NULL;
                
                switchItems.append(switchElement);
            }
            else if (key.compare(RF_FROMSIGNAL) == 0){
                fromSignal.bus = s.section(DS, 1, 1).toUInt();
                fromSignal.address = s.section(DS, 2, 2).toUInt();
                fromSignal.state = s.section(DS, 3, 3).toUInt();
            }
            else if (key.compare(RF_RELEASEPORT) == 0){
                rePort.bus = s.section(DS, 1, 1).toUInt();
                rePort.address = s.section(DS, 2, 2).toUInt();
            }
            else if (key.compare(RF_ACTIVATEPORT) == 0){
                acPort.bus = s.section(DS, 1, 1).toUInt();
                acPort.address = s.section(DS, 2, 2).toUInt();
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
                toSignal.bus = 1;
                toSignal.address = value.toUInt();
            }
            else if (key.compare(RF_SWITCHXTOY) == 0){
                stateElement* switchElement = new stateElement;
                
                switchElement->bus = 1;
                switchElement->address = value.section(" ", 0, 0).toUInt();
                switchElement->state = value.section(" ", 1, 1).toUInt();
                switchElement->elemPtr = NULL;
                switchElement->elemPtr2 = NULL;
                
                switchItems.append(switchElement);
            }
            else if (key.compare(RF_FROMSIGNAL) == 0){
                fromSignal.bus = 1;
                fromSignal.address = value.section(" ", 0, 0).toUInt();
                fromSignal.state = value.section(" ", 1, 1).toUInt();
            }
            else if (key.compare(RF_RELEASEPORT) == 0){
                rePort.bus = 1;
                intvalue = value.toInt();
                if (intvalue == -1)
                    rePort.address = 0;
                else
                    rePort.address = (unsigned int)intvalue;
            }
            else if (key.compare(RF_ACTIVATEPORT) == 0){
                acPort.bus = 1;
                intvalue = value.toInt();
                if (intvalue == -1)
                    acPort.address = 0;
                else
                    acPort.address = (unsigned int)intvalue;
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
        << RF_TOSIGNAL << DS << toSignal.bus << DS << toSignal.address << endl
        << RF_FROMSIGNAL << DS << fromSignal.bus << DS
        << fromSignal.address << DS << fromSignal.state << endl 
        << RF_RELEASEPORT << DS << rePort.bus << DS << rePort.address << endl
        << RF_ACTIVATEPORT << DS << acPort.bus << DS << acPort.address << endl
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
    return new Route(routeType, Name, toSignal, fromSignal,
            rePort, acPort, acLoco, detourLevel, switchItems);
}


bool Route::isLocked()
{
    return locked;
}


QString Route::getName() const
{
    return Name;
}


QString Route::getFromSignalName() const
{
    if (fromSignal.name.isEmpty())
        return QString::number(fromSignal.address);
    else
        return fromSignal.name;
}


QString Route::getToSignalName() const
{
    if (toSignal.name.isEmpty())
        return QString::number(toSignal.address);
    else
        return toSignal.name;
}


unsigned int Route::getType()
{
    return routeType;
}


QString Route::getTypeStr() const
{
    QString typeStr;

    /*returns decoded type */
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


bool Route::startRouting()
{
    locked = true;
    /*TODO: routing code*/
    return locked;
}


bool Route::stopRouting()
{
    locked = false;
    /*TODO: routing code*/
    return !locked;
}


void Route::hideRoute()
{
    if (toSignal.elemPtr != NULL)
           toSignal.elemPtr->switchSelectionMode(ksmNormal);

    if (toSignal.elemPtr2 != NULL)
           toSignal.elemPtr2->switchSelectionMode(ksmNormal);

    if (fromSignal.elemPtr != NULL)
           fromSignal.elemPtr->switchSelectionMode(ksmNormal);

    if (fromSignal.elemPtr2 != NULL)
           fromSignal.elemPtr2->switchSelectionMode(ksmNormal);

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
    if (toSignal.elemPtr != NULL)
           toSignal.elemPtr->showElementState(toSignal.state, ksmStopSig);

    if (toSignal.elemPtr2 != NULL)
           toSignal.elemPtr2->showElementState(toSignal.state, ksmStopSig);

    if (fromSignal.elemPtr != NULL)
           fromSignal.elemPtr->showElementState(fromSignal.state, ksmStartSig);

    if (fromSignal.elemPtr2 != NULL)
           fromSignal.elemPtr2->showElementState(fromSignal.state, ksmStartSig);

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


void Route::setStartSignal(element* el)
{
    el->getStateData(fromSignal);
    Name = tr("New Route");
    
    if (hasStartSignal())
        Name.append(tr(" from %1").arg(fromSignal.name));
 
    if (hasStopSignal())
        Name.append(tr(" to %1").arg(toSignal.name));

    el->switchSelectionMode(ksmStartSig);
}


void Route::setStopSignal(element* el)
{
    el->getStateData(toSignal);
    Name = tr("New Route");

    if (hasStartSignal())
        Name.append(tr(" from %1").arg(fromSignal.name));
 
    if (hasStopSignal())
        Name.append(tr(" to %1").arg(toSignal.name));

    el->switchSelectionMode(ksmStopSig);
}


void Route::addSwitchElement(element* el)
{
    stateElement* se = new stateElement;
    el->getStateData(*se);
    switchItems.append(se);
    el->switchSelectionMode(ksmSwitchEl);
}


void Route::removeElement(element* el)
{
    elemSelectionMode sm = el->getSelectionMode();
    switch (sm) {
        case (ksmStartSig):
            fromSignal.elemPtr = NULL;
            fromSignal.elemPtr2 = NULL;
            break;
        case (ksmStopSig):
            toSignal.elemPtr = NULL;
            toSignal.elemPtr2 = NULL;
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


bool Route::hasStartSignal()
{
    return fromSignal.elemPtr != NULL;
}


bool Route::hasStopSignal()
{
    return toSignal.elemPtr != NULL;
}

