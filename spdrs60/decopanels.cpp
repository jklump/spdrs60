/*
 * decopanels.cpp
 * ----------------
 * Copyright    : (C) 2007 Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-22
 * Last modified: $Date: 2007-09-22 16:11:00 $
 *                $Revision: 1.1 $
 *
 * This file provides menu and toolbar action to switch the paint item
 * selection between different deco panels.
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#include "decopanels.h"
#include "panelaction.h"

#include "pixmaps/spdritem_pre.xpm"
#include "pixmaps/spdritem_tug.xpm"
#include "pixmaps/spdritem_dlt.xpm"
#include "pixmaps/spdritem_drt.xpm"
#include "pixmaps/spdritem_lee.xpm"
#include "pixmaps/spdritem_hs1.xpm"
#include "pixmaps/spdritem_hs2.xpm"
#include "pixmaps/spdritem_sht.xpm"
#include "pixmaps/spdritem_shm.xpm"
#include "pixmaps/spdritem_shb.xpm"

    
DecoPanels::DecoPanels(QObject* parent, const char* name):
    PanelActionGroup(parent, name)
{

    QIconSet is;

    is.setPixmap(QPixmap(spdritem_pre_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Buffer stop"), 0,
            element::siciPre, this, "bufferstopPanel");

    is.setPixmap(QPixmap(spdritem_tug_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Straight tunnel"), 0,
            element::siciGet, this, "tunnelstraightPanel");

    is.setPixmap(QPixmap(spdritem_dlt_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Tunnel left"), 0,
            element::siciDlt, this, "tunnelleftPanel");

    is.setPixmap(QPixmap(spdritem_drt_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Tunnel right"), 0,
            element::siciDrt, this, "tunnelrightPanel");

    is.setPixmap(QPixmap(spdritem_lee_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Text field panel"), 0,
            element::siciLee, this, "textfieldPanel");

    is.setPixmap(QPixmap(spdritem_hs1_xpm), QIconSet::Small);
    new PanelAction(is, tr("&House center wing"), 0,
            element::siciHs1, this, "centerwingPanel");

    is.setPixmap(QPixmap(spdritem_hs2_xpm), QIconSet::Small);
    new PanelAction(is, tr("&House side wing"), 0,
            element::siciHs2, this, "sidewingPanel");

    is.setPixmap(QPixmap(spdritem_sht_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Top loco shed"), 0,
            element::siciSho, this, "locoshedtPanel");

    is.setPixmap(QPixmap(spdritem_shm_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Middle loco shed"), 0,
            element::siciShm, this, "locoshedmPanel");

    is.setPixmap(QPixmap(spdritem_shb_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Bottom loco shed"), 0,
            element::siciShu, this, "locoshedbPanel");
}

