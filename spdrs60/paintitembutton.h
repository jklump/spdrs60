/*
 * paintitembutton.h
 * ---------------------------
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mmail      : guido.scholz@bayernline.de
 * Begin        : 2007-09-26
 * Last modified: $Date: 2007-09-28 17:24:51 $
 *                $Revision: 1.1 $
 *
 * This is the header file to paintitembutton.cpp
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#ifndef PAINTITEMBUTTON_H
#define PAINTITEMBUTTON_H

#include <qtoolbutton.h>

#include "spdrpanel.h"


class PaintItemButton: public QToolButton
{
    Q_OBJECT
        
public:
    PaintItemButton(const QIconSet&, const QString&,
            SpdrPanel::SpdrItemClassId si = SpdrPanel::siciNone,
            QWidget* parent = NULL, const char* name = 0);

    SpdrPanel::SpdrItemClassId Sici();
    
private:
    SpdrPanel::SpdrItemClassId sici;

};

#endif // PAINTITEMBUTTON_H

