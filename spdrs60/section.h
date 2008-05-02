/*
 * section.h
 * ---------
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-10
 * Last modified: $Date: 2008-05-02 04:29:07 $
 *                $Revision: 1.3 $
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
    void setSectionName(const QString&);
    bool hasTrainNumberDisplay();
    void setTrainNumberDisplay(element*);
    
signals:

public slots:
    
protected:
    QString sectionName;
    unsigned int sectionid;
    unsigned int trainid;
    stateElement trainNumberDisplay;

    virtual void initVariables();
    void updateSectionName();
    void updateTrainNumberDisplay();
};
#endif // SECTION_H

