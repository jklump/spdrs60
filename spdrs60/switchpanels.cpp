/*
 * switchpanels.cpp
 * ----------------
 * Copyright    : (C) 2007 Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-21
 * Last modified: $Date: 2007-09-22 14:25:35 $
 *                $Revision: 1.2 $
 *
 * This file provides menu and toolbar action to switch the paint item
 * selection between different switch panels.
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#include "switchpanels.h"
#include "panelaction.h"

#include "pixmaps/spdritem_wel.xpm"
#include "pixmaps/spdritem_wer.xpm"
#include "pixmaps/spdritem_dwl.xpm"
#include "pixmaps/spdritem_dwr.xpm"
#include "pixmaps/spdritem_wey.xpm"
#include "pixmaps/spdritem_drw.xpm"
#include "pixmaps/spdritem_ekl.xpm"
#include "pixmaps/spdritem_ekr.xpm"
#include "pixmaps/spdritem_dkl.xpm"
#include "pixmaps/spdritem_dkr.xpm"

    
SwitchPanels::SwitchPanels(QObject* parent, const char* name):
    PanelActionGroup(parent, name)
{

    QIconSet is;

    is.setPixmap(QPixmap(spdritem_wel_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Switch left"), 0,
            element::siciWel, this, "switchleftPanel");

    is.setPixmap(QPixmap(spdritem_wer_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Switch right"), 0,
            element::siciWer, this, "switchrightPanel");

    is.setPixmap(QPixmap(spdritem_dwl_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Diagonal switch left"), 0,
            element::siciDwl, this, "diaswitchleftPanel");

    is.setPixmap(QPixmap(spdritem_dwr_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Diagonal switch right"), 0,
            element::siciDwr, this, "diaswitchrightPanel");

    is.setPixmap(QPixmap(spdritem_wey_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Y-Switch"), 0,
            element::siciWey, this, "yswitchPanel");

    is.setPixmap(QPixmap(spdritem_drw_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Three way turnout"), 0,
            element::siciDrw, this, "waitsignalPanel");

    is.setPixmap(QPixmap(spdritem_ekl_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Single-slip switch left"), 0,
            element::siciEkl, this, "singleleftPanel");

    is.setPixmap(QPixmap(spdritem_ekr_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Single-slip switch right"), 0,
            element::siciEkr, this, "singlerightPanel");

    is.setPixmap(QPixmap(spdritem_dkl_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Double-slip switch left"), 0,
            element::siciDkl, this, "doubleleftPanel");

    is.setPixmap(QPixmap(spdritem_dkr_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Double-slip switch right"), 0,
            element::siciDkr, this, "doublerightPanel");
}

