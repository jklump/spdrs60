/*
 * externalgrouppanels.cpp
 * -------------------------------
 * Copyright    : (C) 2007 Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-16
 * Last modified: $Date: 2007-09-18 17:07:16 $
 *                $Revision: 1.3 $
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


#include "externalgrouppanels.h"
#include "panelaction.h"

#include "pixmaps/spdritem_feg.xpm"
#include "pixmaps/spdritem_taf.xpm"
#include "pixmaps/spdritem_tau.xpm"
#include "pixmaps/spdritem_feb.xpm"
#include "pixmaps/spdritem_taw.xpm"
#include "pixmaps/spdritem_fer.xpm"
#include "pixmaps/spdritem_tas.xpm"
#include "pixmaps/spdritem_fey.xpm"
#include "pixmaps/spdritem_fen.xpm"
#include "pixmaps/spdritem_fee.xpm"

    
ExternalGroupPanels::ExternalGroupPanels(QObject* parent, const char* name):
    PanelActionGroup(parent, name)
{
    new PanelAction(QPixmap(spdritem_feg_xpm), tr("&Route panel"), 0,
            element::siciFeg, this, "routePanel");

    new PanelAction(QPixmap(spdritem_taf_xpm), tr("&FHT panel"), 0,
            element::siciTaf, this, "fhtPanel");

    new PanelAction(QPixmap(spdritem_tau_xpm), tr("&UfGT and MGT panel"), 0,
            element::siciTau, this, "ufgtPanel");

    new PanelAction(QPixmap(spdritem_feb_xpm), tr("&Turnout panel"), 0,
            element::siciFeb, this, "turnoutPanel");

    new PanelAction(QPixmap(spdritem_taw_xpm), tr("&WGT panel"), 0,
            element::siciTaw, this, "wgtPanel");

    new PanelAction(QPixmap(spdritem_fer_xpm), tr("&Signal panel"), 0,
            element::siciFer, this, "signalPanel");

    new PanelAction(QPixmap(spdritem_tas_xpm), tr("&SGT and HaGT panel"), 0,
            element::siciTas, this, "sgtPanel");

    new PanelAction(QPixmap(spdritem_fey_xpm), tr("&Level crossing panel"), 0,
            element::siciFey, this, "crossingPanel");

    new PanelAction(QPixmap(spdritem_fen_xpm), tr("&Axle counter panel"), 0,
            element::siciFen, this, "axlePanel");

    new PanelAction(QPixmap(spdritem_fee_xpm), tr("&Power supply panel"), 0,
            element::siciFee, this, "powerPanel");
}

