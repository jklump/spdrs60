/*
 * panelactiongroup.cpp
 * -------------------------------
 * Copyright    : (C) 2007 Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-18
 * Last modified: $Date: 2007-09-22 08:05:17 $
 *                $Revision: 1.2 $
 *
 * This file provides a basic class for menu and toolbar actions to switch
 * the gbsarea paint item.
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#include "panelactiongroup.h"
#include "panelaction.h"

    
PanelActionGroup::PanelActionGroup(QObject* parent, const char* name):
 QActionGroup(parent, name , true)
{
    selectedAction = NULL;
    connect(this, SIGNAL(selected(QAction*)),
            this, SLOT(spdrItemSelected(QAction*)));
    setEnabled(false);
}

/*
 *translate selected action to a public integer value
 */
void PanelActionGroup::spdrItemSelected(QAction* ac)
{
    if (ac != NULL) {
        selectedAction = ac;
        emit paintItemChanged(static_cast<PanelAction*>(ac)->Sici());
    }
}

/*
 * enable actions if layout edit mode is choosen
 */
void PanelActionGroup::enablePaintItems(GBSArea::LayoutEditMode mode)
{
    setEnabled(mode == GBSArea::lemPaint);
}

/*
 * deselect current selection, only possible if exclusive state is false
 */
void PanelActionGroup::deselectPaintItem(element::SpdrItemClassId)
{
    if (selectedAction != NULL) {
        setExclusive(false);
        selectedAction->setOn(false);
        setExclusive(true);
        selectedAction = NULL;
    }
}

