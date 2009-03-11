/***************************************************************************
                           feedbacklistbox.h
                           version 0.5.2 $Revision: 1.6 $
                           -------------------------------
    copyright            : (C) 2006-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2009-03-11 19:36:38 $
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
    FeedbackListBox(QWidget* parent = 0, const char* name = 0,
            unsigned int b = 1);
    void updateModuleSetup(int, unsigned int);
    
public slots:
    void updateFeedbackPortState(unsigned int, bool);
    //void processSrcpMessage();

signals:
    
private:
    unsigned int bus;
    QPoint presspos;
    bool mousePressed;
    FeedbackModule::ModuleType moduleType;

    void updateModuleNumber(unsigned int);
    void updateModulesType();

protected:
    void contentsMousePressEvent(QMouseEvent*);
    void contentsMouseReleaseEvent(QMouseEvent*);
    void contentsMouseMoveEvent(QMouseEvent*);
};
#endif // FEEDBACKLISTBOX_H

