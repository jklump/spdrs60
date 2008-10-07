/***************************************************************************
                           feedbackmodule.cpp
                           version 0.5.2 $Revision: 1.7 $
                           -------------------------------
    copyright            : (C) 2006-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-10-07 17:34:43 $
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
   This code paints a single feedback module for the feedback modules list
 ***************************************************************************/

#include <qapplication.h>
#include <qfont.h>

#include "feedbackmodule.h"

enum {
 FBM16WIDTH = 140,
 FBM8WIDTH = 80,
 FBMHEIGHT = 80
};


FeedbackModule::FeedbackModule(QListBox* listbox): QListBoxItem(listbox)
{
    moduleType = fbm16;
    id = 0;
    ocstate = 0;
    setSelectable(false);
}

int FeedbackModule::height(const QListBox*) const
{
    return FBMHEIGHT;
}

int FeedbackModule::width(const QListBox*) const
{
    if (moduleType == fbm16)
        return FBM16WIDTH;
    else
        return FBM8WIDTH;
}

void FeedbackModule::paint(QPainter* p)
{
    if (moduleType == fbm16)
        paint16(p);
    else
        paint8(p);
}

/**
 * paint module with sixteen inputs
 */
void FeedbackModule::paint16(QPainter* p)
{
    int i;
    QString s;

    p->setPen(QPen(Qt::black, 1, Qt::SolidLine));
    p->drawRoundRect(4, 4, FBM16WIDTH - 8, FBMHEIGHT - 8, 8, 8);
    
    QFont f;
    QRect br;
    f.setPointSize(QApplication::font().pointSize() - 2);
    p->setFont(f);
    QFontMetrics fm(f);
    unsigned int bit = 0;
    
    for (i = 0; i < 8; i++) {
        bit = 1u << i;
        if ((bit & ocstate) != 0u)
            p->fillRect(14 + i * 15, 5, 6, 6, QBrush(QColor(Qt::red)));
        else
            p->fillRect(14 + i * 15, 5, 6, 6, QBrush(QColor(Qt::white)));
        p->drawRect(13 + i * 15, 4, 8, 8);
        s.setNum(i + 1);
        br = fm.boundingRect(s);
        p->drawText(17 + i * 15 - br.width()/2, 20 + br.height()/2, s);
    }
    
    for (i = 0; i < 8; i++) {
        bit = 1u << (i + 8);
        if ((bit & ocstate) != 0u)
            p->fillRect(14 + i * 15, FBMHEIGHT - 11, 6, 6,
                    QBrush(QColor(Qt::red)));
        else
            p->fillRect(14 + i * 15, FBMHEIGHT - 11, 6, 6,
                    QBrush(QColor(Qt::white)));
        p->drawRect(13 + i * 15, FBMHEIGHT - 12, 8, 8);
        s.setNum(i + 9);
        br = fm.boundingRect(s);
        p->drawText(17 + i * 15 - br.width()/2,
                FBMHEIGHT - 12 - br.height()/2, s);
    }
    
    s.setNum(id);
    
    f.setPointSize(QApplication::font().pointSize() + 1);
    f.setWeight(QFont::DemiBold);
    p->setFont(f);
    br = fm.boundingRect(s);
    p->drawText(FBM16WIDTH/2 - br.width()/2, FBMHEIGHT/2 + br.height()/2, s);
}

/**
 * paint module with eight inputs
 */
void FeedbackModule::paint8(QPainter* p)
{
    int i;
    QString s;

    p->setPen(QPen(Qt::black, 1, Qt::SolidLine));
    p->drawRoundRect(4, 4, FBM8WIDTH - 8, FBMHEIGHT - 8, 8, 8);

    QFont f;
    QRect br;
    f.setPointSize(QApplication::font().pointSize() - 2);
    p->setFont(f);
    QFontMetrics fm(f);
    unsigned int bit = 0;
    
    for (i = 0; i < 4; i++) {
        bit = 1u << i;
        if ((bit & ocstate) != 0u)
            p->fillRect(14 + i * 15, 5, 6, 6, QBrush(QColor(Qt::red)));
        else
            p->fillRect(14 + i * 15, 5, 6, 6, QBrush(QColor(Qt::white)));
        p->drawRect(13 + i * 15, 4, 8, 8);
        s.setNum(i + 1);
        br = fm.boundingRect(s);
        p->drawText(17 + i * 15 - br.width()/2, 20 + br.height()/2, s);
    }
    
    for (i = 0; i < 4; i++) {
        bit = 1u << (i + 4);
        if ((bit & ocstate) != 0u)
            p->fillRect(14 + i * 15, FBMHEIGHT - 11, 6, 6,
                    QBrush(QColor(Qt::red)));
        else
            p->fillRect(14 + i * 15, FBMHEIGHT - 11, 6, 6,
                    QBrush(QColor(Qt::white)));
        p->drawRect(13 + i * 15, FBMHEIGHT - 12, 8, 8);
        s.setNum(i + 5);
        br = fm.boundingRect(s);
        p->drawText(17 + i * 15 - br.width()/2,
                FBMHEIGHT - 12 - br.height()/2, s);
    }
    
    s.setNum(id);
    
    f.setPointSize(QApplication::font().pointSize() + 1);
    f.setWeight(QFont::DemiBold);
    p->setFont(f);
    br = fm.boundingRect(s);
    p->drawText(FBM8WIDTH/2 - br.width()/2, FBMHEIGHT/2 + br.height()/2, s);
}

void FeedbackModule::setId(unsigned int mid)
{
    // range: 0..30
    id = mid;
}

/**
 * set module type and clear occcupation state when type is changed
 **/
void FeedbackModule::setModuleType(ModuleType mdt)
{
    if (mdt != moduleType)
        ocstate = 0;

    moduleType = mdt;
}

/**
 * update occupation state if module address matches, return match state
 */
bool FeedbackModule::changeFeedbackState(unsigned int contact, bool state)
{
    unsigned int targetmod = 0;
    unsigned int address = 0;
    
    if (moduleType == fbm16)
        targetmod = (contact - 1) / 16 + 1;
    else
        targetmod = (contact - 1) / 8 + 1;
    
    if (targetmod != id)
        return false;

    if (moduleType == fbm16)
        address = (contact - 1) & 15u;
    else
        address = (contact - 1) & 7u;

    unsigned int bit = 1u << address;

    if (state)
        ocstate = ocstate | bit;
    else
        ocstate = ocstate & ~bit;

    return true;
}

/*
void FeedbackModule::processSrcpMessage(message)
{
}
*/

