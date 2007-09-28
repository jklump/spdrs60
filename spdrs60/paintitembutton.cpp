/*
 * paintitembutton.cpp
 * -------------------
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-26
 * Last modified: $Date: 2007-09-28 17:24:51 $
 *                $Revision: 1.1 $
 *
 * This code creates a dockable window with a set of paint items 
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qtooltip.h>

#include "paintitembutton.h"


PaintItemButton::PaintItemButton(const QIconSet& icon, const QString& text,
        element::SpdrItemClassId si, QWidget* parent, const char* name)
: QToolButton(parent, name)
{
    sici = si;
    setUsesBigPixmap(true);
    setIconSet(icon);
    setToggleButton(true);
    QToolTip::add(this, text);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}

/*
 * return associated sici value
 */
element::SpdrItemClassId PaintItemButton::Sici()
{
    return sici;
}
