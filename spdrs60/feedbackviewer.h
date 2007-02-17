/***************************************************************************
                           feedbackviewer.h
                           version 0.5.2 $Revision: 1.4 $
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
   header file for feedbackviewer.cpp 
 **************************************************************************/

#ifndef FEEDBACKVIEWER_H
#define FEEDBACKVIEWER_H

#include <qdockwindow.h>
#include <qtabwidget.h>

#include "feedbacklistbox.h"

class FeedbackViewer: public QDockWindow
{
    Q_OBJECT
        
public:
    FeedbackViewer(QWidget* parent=0, const char* name=0);
    void updateBusAndModuleStructure();
    
public slots:
    //void processSrcpMessage();
    void feedbackPortChanged(unsigned int, unsigned int, bool);

signals:
    
private:
    QTabWidget* fbTW;
    FeedbackListBox* fbLB1;
    FeedbackListBox* fbLB2;
    FeedbackListBox* fbLB3;
    FeedbackListBox* fbLB4;

    void addTab1();
    void addTab2();
    void addTab3();
    void addTab4();
    void removeTab2();
    void removeTab3();
    void removeTab4();
};
#endif // FEEDBACKVIEWER_H

