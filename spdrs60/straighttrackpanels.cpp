/*
 * straighttrackpanels.cpp
 * -----------------------
 * Copyright    : (C) 2007 Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-16
 * Last modified: $Date: 2007-09-18 18:48:02 $
 *                $Revision: 1.1 $
 *
 * This file provides menu and toolbar action to switch the paint item
 * selection between different external group panels.
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#include "straighttrackpanels.h"
#include "panelaction.h"

#include "pixmaps/spdritem_trh.xpm"
#include "pixmaps/spdritem_trv.xpm"
#include "pixmaps/spdritem_dil.xpm"
#include "pixmaps/spdritem_dir.xpm"
#include "pixmaps/spdritem_ri1.xpm"
#include "pixmaps/spdritem_ri2.xpm"
#include "pixmaps/spdritem_krh.xpm"
#include "pixmaps/spdritem_krr.xpm"
#include "pixmaps/spdritem_krl.xpm"

    
StraightTrackPanels::StraightTrackPanels(QObject* parent, const char* name):
    PanelActionGroup(parent, name)
{
    new PanelAction(QPixmap(spdritem_trh_xpm),
            tr("&Horizontal track panel"), 0,
            element::siciGer, this, "horizontaltrackPanel");

    new PanelAction(QPixmap(spdritem_trv_xpm),
            tr("&Vertical track panel"), 0,
            element::siciTrv, this, "verticaltrackPanel");

    new PanelAction(QPixmap(spdritem_dil_xpm),
            tr("&Left diagonal track panel"), 0,
            element::siciDil, this, "leftdiaPanel");

    new PanelAction(QPixmap(spdritem_dir_xpm),
            tr("&Right diagonal track panel"), 0,
            element::siciDir, this, "rightdiaPanel");

    new PanelAction(QPixmap(spdritem_ri1_xpm),
            tr("&Single direction panel"), 0,
            element::siciRi1, this, "singledirectionPanel");

    new PanelAction(QPixmap(spdritem_ri2_xpm),
            tr("&Two direction panel"), 0,
            element::siciRi2, this, "twodirectionPanel");

    new PanelAction(QPixmap(spdritem_krh_xpm),
            tr("&Crossing panel"), 0,
            element::siciKrh, this, "crossingPanel");

    new PanelAction(QPixmap(spdritem_krr_xpm),
            tr("&Right crossing panel"), 0,
            element::siciKrr, this, "rightcrossingPanel");

    new PanelAction(QPixmap(spdritem_krl_xpm),
            tr("&Left crossing panel"), 0,
            element::siciKrl, this, "leftcrossingPanel");
}

