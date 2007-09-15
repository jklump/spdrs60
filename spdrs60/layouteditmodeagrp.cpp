/*
 * layouteditmodeagrp.cpp
 * -------------------------------
 * Copyright    : (C) 2007 Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-12
 * Last modified: $Date: 2007-09-15 10:54:47 $
 *                $Revision: 1.2 $
 *
 * This file provides menu and toolbar action to switch layout edit mode
 * between three different working modes:
 *   - Select: Select and move a layout item
 *   - Paint:  Paint new layout items
 *   - Erase:  Erase items
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#include "layouteditmodeagrp.h"

#include "pixmaps/layoutitemselectmode.xpm"
#include "pixmaps/layoutitempaintmode.xpm"
#include "pixmaps/layoutitemerasemode.xpm"

    
LayoutEditModeAgrp::LayoutEditModeAgrp(QObject* parent, const char* name):
 QActionGroup(parent, name , true)
{
#if QT_VERSION >= 0x030200
    actionSelectMode = new QAction(QPixmap(layoutitemselectmode_xpm),
            tr("&Select mode"), 0, this, "selectMode");
#else
    actionSelectMode = new QAction("", QPixmap(layoutitemselectmode_xpm),
            tr("&Select mode"), 0, this, "selectMode");
#endif
    actionSelectMode->setToggleAction(true);
    actionSelectMode->setOn(true);

    
#if QT_VERSION >= 0x030200
    actionPaintMode = new QAction(QPixmap(layoutitempaintmode_xpm),
            tr("&Paint mode"), 0, this, "paintMode");
#else
    actionPaintMode = new QAction("", QPixmap(layoutitempaintmode_xpm),
            tr("&Paint mode"), 0, this, "paintMode");
#endif
    actionPaintMode->setToggleAction(true);

    
#if QT_VERSION >= 0x030200
    actionEraseMode = new QAction(QPixmap(layoutitemerasemode_xpm),
            tr("&Erase mode"), 0, this, "eraseMode");
#else
    actionEraseMode = new QAction("", QPixmap(layoutitemerasemode_xpm),
            tr("&Erase mode"), 0, this, "eraseMode");
#endif
    actionEraseMode->setToggleAction(true);

    connect(this, SIGNAL(selected(QAction*)),
            this, SLOT(modeSelected(QAction*)));
}


/*
 *translate selected action to a public integer value
 */
void LayoutEditModeAgrp::modeSelected(QAction* ac)
{
    GBSArea::LayoutEditMode mode = GBSArea::lemSelect;

    if (ac == actionPaintMode)
        mode = GBSArea::lemPaint;
    else if (ac == actionEraseMode)
        mode = GBSArea::lemErase;

    emit modeChanged(mode);
}

/*
 * reset to select mode if layout edit mode is disabled
 */
void LayoutEditModeAgrp::enableEditMode(bool enable)
{
    if (!enable)
       actionSelectMode->setOn(true);
}

