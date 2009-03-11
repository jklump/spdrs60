/***************************************************************************
                           feedbackmodule.h
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
   header file for feedbackmodule.cpp 
 **************************************************************************/

#ifndef FEEDBACKMODULE_H
#define FEEDBACKMODULE_H

#include <qlistbox.h>
#include <qpainter.h>

#include "resources.h"


class FeedbackModule: public QListBoxItem
{
    // Q_OBJECT

    unsigned int id;
    unsigned int ocstate;
    FbContact fbc;
    void paint16(QPainter* p = 0);
    void paint8(QPainter* p = 0);
    bool isContactPosition16(const QPoint&);
    bool isContactPosition8(const QPoint&);
        
public:
    enum ModuleType {fbm16 = 0, fbm8};
        
    FeedbackModule(QListBox* listbox = 0, unsigned int bus = 1);
    //FeedbackModule(QListBox* listbox, QListBoxItem* after);
    void setId(unsigned int = 0);
    void setModuleType(ModuleType);
    bool changeFeedbackState(unsigned int, bool);
    bool isContactPosition(const QPoint&);
    bool getContactData(QByteArray&);
    //bool processSrcpMessage();
    
public slots:

protected:
    int height(const QListBox*) const;
    int width(const QListBox*) const;
    void paint(QPainter* p = 0);
    
private:
    ModuleType moduleType;
};
#endif // FEEDBACKMODULE_H

