/***************************************************************************
                           feedbackmodule.h
                           version 0.5.2 $Revision: 1.5 $
                           -------------------------------
    copyright            : (C) 2006-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-11-23 21:05:25 $
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


class FeedbackModule: public QListBoxItem
{
   // Q_OBJECT
        
public:
    enum ModuleType {fbm16 = 0, fbm8};
        
    FeedbackModule(QListBox* listbox = 0);
    //FeedbackModule(QListBox* listbox, QListBoxItem* after);
    void setId(unsigned int = 0);
    void setModuleType(ModuleType);
    bool changeFeedbackState(unsigned int, bool);
    //bool processSrcpMessage();
    
public slots:

protected:
    int height(const QListBox* lb) const;
    int width(const QListBox* lb) const;
    void paint(QPainter* p = 0);
    
signals:
    
private:
    unsigned int id;
    unsigned int ocstate;
    ModuleType moduleType;
    void paint16(QPainter* p = 0);
    void paint8(QPainter* p = 0);

};
#endif // FEEDBACKMODULE_H

