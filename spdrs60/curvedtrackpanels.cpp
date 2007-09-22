/*
 * curvedtrackpanels.cpp
 * -------------------------------
 * Copyright    : (C) 2007 Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-16
 * Last modified: $Date: 2007-09-22 08:08:18 $
 *                $Revision: 1.2 $
 *
 * This file provides menu and toolbar action to switch the paint item
 * selection between different curved track panels.
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#include "curvedtrackpanels.h"
#include "panelaction.h"

#include "pixmaps/spdritem_kur.xpm"
#include "pixmaps/spdritem_kul.xpm"
#include "pixmaps/spdritem_ttl.xpm"
#include "pixmaps/spdritem_ttr.xpm"
#include "pixmaps/spdritem_tbl.xpm"
#include "pixmaps/spdritem_tbr.xpm"

    
CurvedTrackPanels::CurvedTrackPanels(QObject* parent, const char* name):
    PanelActionGroup(parent, name)
{
    QIconSet is;

    is.setPixmap(QPixmap(spdritem_kur_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Curve right panel"), 0,
            element::siciKur, this, "curverightPanel");

    is.setPixmap(QPixmap(spdritem_kul_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Curve left panel"), 0,
            element::siciKul, this, "curveleftPanel");

    is.setPixmap(QPixmap(spdritem_ttl_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Curve top vertical left"), 0,
            element::siciTtl, this, "curvetvlPanel");

    is.setPixmap(QPixmap(spdritem_ttr_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Curve top vertical right"), 0,
            element::siciTtr, this, "curvetvrPanel");

    is.setPixmap(QPixmap(spdritem_tbl_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Curve bottom vertical left"), 0,
            element::siciTbl, this, "curvebvlPanel");

    is.setPixmap(QPixmap(spdritem_tbr_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Curve bottom vertical right"), 0,
            element::siciTbr, this, "curvebvrPanel");
}

