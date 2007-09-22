/*
 * externalgrouppanels.cpp
 * -------------------------------
 * Copyright    : (C) 2007 Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-16
 * Last modified: $Date: 2007-09-22 08:08:18 $
 *                $Revision: 1.4 $
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
    QIconSet is;

    is.setPixmap(QPixmap(spdritem_feg_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Route group panel"), 0,
            element::siciFeg, this, "routePanel");

    is.setPixmap(QPixmap(spdritem_taf_xpm), QIconSet::Small);
    new PanelAction(is, tr("&FHT panel"), 0,
            element::siciTaf, this, "fhtPanel");

    is.setPixmap(QPixmap(spdritem_tau_xpm), QIconSet::Small);
    new PanelAction(is, tr("&UfGT and MGT panel"), 0,
            element::siciTau, this, "ufgtPanel");

    is.setPixmap(QPixmap(spdritem_feb_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Turnout group panel"), 0,
            element::siciFeb, this, "turnoutPanel");

    is.setPixmap(QPixmap(spdritem_taw_xpm), QIconSet::Small);
    new PanelAction(is, tr("&WGT panel"), 0,
            element::siciTaw, this, "wgtPanel");

    is.setPixmap(QPixmap(spdritem_fer_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Signal group panel"), 0,
            element::siciFer, this, "signalPanel");

    is.setPixmap(QPixmap(spdritem_tas_xpm), QIconSet::Small);
    new PanelAction(is, tr("&SGT and HaGT panel"), 0,
            element::siciTas, this, "sgtPanel");

    is.setPixmap(QPixmap(spdritem_fey_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Level crossing group panel"), 0,
            element::siciFey, this, "crossingPanel");

    is.setPixmap(QPixmap(spdritem_fen_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Axle counter group panel"), 0,
            element::siciFen, this, "axlePanel");

    is.setPixmap(QPixmap(spdritem_fee_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Power supply group panel"), 0,
            element::siciFee, this, "powerPanel");
}

