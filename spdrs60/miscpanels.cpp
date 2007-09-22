/*
 * miscpanels.cpp
 * ----------------
 * Copyright    : (C) 2007 Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-22
 * Last modified: $Date: 2007-09-22 15:27:32 $
 *                $Revision: 1.1 $
 *
 * This file provides menu and toolbar action to switch the paint item
 * selection between miscellanous panels.
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#include "miscpanels.h"
#include "panelaction.h"

#include "pixmaps/spdritem_dco.xpm"
#include "pixmaps/spdritem_bld.xpm"
#include "pixmaps/spdritem_adr.xpm"
#include "pixmaps/spdritem_lcr.xpm"
#include "pixmaps/spdritem_rel.xpm"
#include "pixmaps/spdritem_mdc.xpm"
#include "pixmaps/spdritem_tnt.xpm"
#include "pixmaps/spdritem_trt.xpm"

    
MiscPanels::MiscPanels(QObject* parent, const char* name):
    PanelActionGroup(parent, name)
{

    QIconSet is;

    is.setPixmap(QPixmap(spdritem_dco_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Decoupler"), 0,
            element::siciEnk, this, "decouplerPanel");

    is.setPixmap(QPixmap(spdritem_bld_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Blind element"), 0,
            element::siciBld, this, "blindPanel");

    is.setPixmap(QPixmap(spdritem_adr_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Address indicator"), 0,
            element::siciAdr, this, "addressPanel");

    is.setPixmap(QPixmap(spdritem_lcr_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Level crossing"), 0,
            element::siciBue, this, "levelcrossingPanel");

    is.setPixmap(QPixmap(spdritem_rel_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Relais"), 0,
            element::siciRel, this, "relaisPanel");

    is.setPixmap(QPixmap(spdritem_mdc_xpm), QIconSet::Small);
    new PanelAction(is, tr("&DC motor"), 0,
            element::siciMdc, this, "dcmotorPanel");

    is.setPixmap(QPixmap(spdritem_tnt_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Turntable"), 0,
            element::siciDre, this, "turntablePanel");

    is.setPixmap(QPixmap(spdritem_trt_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Transfer table"), 0,
            element::siciSbn, this, "transfertablePanel");
}

