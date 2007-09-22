/*
 * straighttrackpanels.cpp
 * -----------------------
 * Copyright    : (C) 2007 Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-16
 * Last modified: $Date: 2007-09-22 08:08:18 $
 *                $Revision: 1.2 $
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
    QIconSet is;

    is.setPixmap(QPixmap(spdritem_trh_xpm), QIconSet::Small);
    selectedAction = new PanelAction(is,
            tr("&Horizontal track panel"), 0,
            element::siciGer, this, "horizontaltrackPanel");
    selectedAction->setOn(true);

    is.setPixmap(QPixmap(spdritem_trv_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Vertical track panel"), 0,
            element::siciTrv, this, "verticaltrackPanel");

    is.setPixmap(QPixmap(spdritem_dil_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Left diagonal track panel"), 0,
            element::siciDil, this, "leftdiaPanel");

    is.setPixmap(QPixmap(spdritem_dir_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Right diagonal track panel"), 0,
            element::siciDir, this, "rightdiaPanel");

    is.setPixmap(QPixmap(spdritem_ri1_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Single direction panel"), 0,
            element::siciRi1, this, "singledirectionPanel");

    is.setPixmap(QPixmap(spdritem_ri2_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Two direction panel"), 0,
            element::siciRi2, this, "twodirectionPanel");

    is.setPixmap(QPixmap(spdritem_krh_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Crossing panel"), 0,
            element::siciKrh, this, "crossingPanel");

    is.setPixmap(QPixmap(spdritem_krr_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Right crossing panel"), 0,
            element::siciKrr, this, "rightcrossingPanel");

    is.setPixmap(QPixmap(spdritem_krl_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Left crossing panel"), 0,
            element::siciKrl, this, "leftcrossingPanel");
}

