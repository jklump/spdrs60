/*
 * paintitemwindow.h
 * ---------------------------
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mmail      : guido.scholz@bayernline.de
 * Begin        : 2007-09-26
 * Last modified: $Date: 2007-09-28 17:24:51 $
 *                $Revision: 1.1 $
 *
 * This is the header file to paintitemwindow.cpp
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#ifndef PAINTITEMWINDOW_H
#define PAINTITEMWINDOW_H

#include <qdockwindow.h>

#include "element.h"
#include "gbsarea.h"


class PaintItemWindow: public QDockWindow
{
    Q_OBJECT
        
public:
    PaintItemWindow(QWidget* parent = NULL, const char* name = 0);
    
public slots:
    void changeLayoutEditMode(GBSArea::LayoutEditMode);

private slots:
    void paintItemPressed(int);

signals:
    void paintItemChanged(element::SpdrItemClassId);
    
private:
    QButtonGroup* paintItemBG;

};

#endif // PAINTITEMWINDOW_H

