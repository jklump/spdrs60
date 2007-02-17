/***************************************************************************
                           feedbacklistbox.h
                           version 0.5.2 $Revision: 1.5 $
                           -------------------------------
    copyright            : (C) 2006-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-02-17 07:23:32 $
***************************************************************************/

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/

/**************************************************************************
   header file for feedbacklistbox.cpp 
 **************************************************************************/

#ifndef FEEDBACKLISTBOX_H
#define FEEDBACKLISTBOX_H

#include <qlistbox.h>

#include "feedbackmodule.h"


class FeedbackListBox: public QListBox
{
    Q_OBJECT
        
public:
    FeedbackListBox(QWidget* parent=0, const char* name=0);
    void updateModuleSetup(int, unsigned int);
    
public slots:
    void updateFeedbackPortState(unsigned int, bool);
    //void processSrcpMessage();

signals:
    
private:
    FeedbackModule::ModuleType moduleType;
    void updateModuleNumber(unsigned int);
    void updateModulesType();

};
#endif // FEEDBACKLISTBOX_H

