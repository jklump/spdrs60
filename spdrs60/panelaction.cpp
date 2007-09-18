/*
 * panelaction.cpp
 * ---------------
 * Copyright    : (C) 2007 Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-18
 * Last modified: $Date: 2007-09-18 17:07:16 $
 *                $Revision: 1.1 $
 *
 * This file provides a basic class for menu and toolbar actions 
 * to switch gbsarea paint items.
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#include "panelaction.h"

    
#if QT_VERSION >= 0x030200
PanelAction::PanelAction(const QIconSet& icon, const QString& ms,
        QKeySequence accel, element::SpdrItemClassId si,
        QObject* parent, const char* name)
    : QAction(icon, ms, accel, parent, name)
#else
PanelAction::PanelAction(const QIconSet& icon, const QString& ms,
        QKeySequence accel, element::SpdrItemClassId si,
        QObject* parent, const char* name)
    : QAction("", icon, ms, accel, parent, name)
#endif
{
    sici = si;
    setToggleAction(true);
}

/*
 * return associated sici value
 */
element::SpdrItemClassId PanelAction::Sici()
{
    return sici;
}

