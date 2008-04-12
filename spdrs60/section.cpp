/*
 * section.cpp
 * -----------
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-10
 * Last modified: $Date: 2008-04-12 14:22:14 $
 *                $Revision: 1.2 $
 *
 * This code implements the section class to handle train ids and train
 * locations. This is a base class for routes and blocks.
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#include "section.h"


Section::Section(const char* secname, unsigned int secid, unsigned int trid,
        QObject* parent, const char* name): QObject(parent, name)
{
    initVariables();

    sectionName = secname;
    sectionid = secid;
    trainid = trid;

    trainNumberDisplay.name = "";
    trainNumberDisplay.bus = 1;
    trainNumberDisplay.address = 0;
    trainNumberDisplay.state = 0;
    trainNumberDisplay.elemPtr = NULL;
}


Section::~Section()
{
}

/*
 * set all variables to init values
 */
void Section::initVariables()
{
    // user selectable data
    forwardnumber = false;
    forwardexternal = false;
    forwardtargetid = 0;
    forwardtargettype = tntRoute;
}


unsigned int Section::getId()
{
    return sectionid;
}


void Section::setId(unsigned int id)
{
    sectionid = id;
}


QString Section::getSectionName() const
{
    return sectionName;
}


void Section::setSectionName(const QString& sn)
{
    sectionName = sn;
}


void Section::setTrainNumberDisplay(element* el)
{
    el->getStateData(trainNumberDisplay);
    el->switchSelectionMode(ksmDisplay);
    updateTrainNumberDisplay();
}


bool Section::hasTrainNumberDisplay()
{
    return trainNumberDisplay.elemPtr != NULL;
}

/*
 * return true, if a train number is available
 */
bool Section::hasTrain()
{
    return (0 != trainid);
}

/*
 * clear stored train number
 */
void Section::clearTrain()
{
    trainid = 0;
    updateTrainNumberDisplay();
}

/*
 * set new value for train number
 */
void Section::setTrain(unsigned int tr)
{
    if (tr != trainid) {
        trainid = tr;
        updateTrainNumberDisplay();
    }
}

/*
 * return availabel train number
 */
unsigned int Section::getTrain()
{
    return trainid;
}

/*
 * search train number display and update content
 */
void Section::updateTrainNumberDisplay()
{
    if (trainNumberDisplay.elemPtr != NULL)
        trainNumberDisplay.elemPtr->updateTrainNumber(trainid);
}

/*
 * return true if train number forwarding is enabled
 */
bool Section::forwardTrainNumber()
{
    return forwardnumber;
}

/*
 * return true if train number forwarding is external
 */
bool Section::forwardExternal()
{
    return forwardexternal;
}

/*
 * return type of train number forwarding target
 */
int Section::forwardTargetId()
{
    return forwardtargetid;
}

/*
 * return type of train number forwarding target
 */
int Section::forwardTargetType()
{
    return forwardtargettype;
}

