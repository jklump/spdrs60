/*
 * section.h
 * ---------
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-10
 * Last modified: $Date: 2007-09-10 19:55:18 $
 *                $Revision: 1.1 $
 *
 * This is the header file for section.cpp.
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/

#ifndef SECTION_H
#define SECTION_H

//#include <qptrlist.h>
//#include <qtextstream.h>
//#include <qptrvector.h>

#include "element.h"


class Section: public QObject
{
    Q_OBJECT
        
public:
    enum TrainNumberTarget {tntRoute = 0, tntBlock};

    Section(const char* secname = NULL, unsigned int secid = 0,
            unsigned int trainid = 0,
            QObject* parent = NULL, const char* name = NULL);

    ~Section();
    
    virtual bool runEditDialog(QWidget*) = 0;

    void setId(unsigned int);
    unsigned int getId();
    void setTrain(unsigned int);
    unsigned int getTrain();
    void clearTrain();
    bool hasTrain();
    QString getSectionName() const;
    bool hasTrainNumberDisplay();
    void setTrainNumberDisplay(element*);
    bool forwardTrainNumber();
    bool forwardExternal();
    int forwardTargetId();
    int forwardTargetType();
    
signals:

public slots:
    
protected:
    QString sectionName;
    unsigned int sectionid;
    unsigned int trainid;
    unsigned int forwardtargetid;
    bool forwardnumber;
    bool forwardexternal;
    TrainNumberTarget forwardtargettype;
    stateElement trainNumberDisplay;

    virtual void initVariables();
    void updateSectionName();
    void updateTrainNumberDisplay();
};
#endif // SECTION_H

