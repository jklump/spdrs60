/*
 * curvedtrackpanels.cpp
 * -------------------------------
 * Copyright    : (C) 2007 Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-16
 * Last modified: $Date: 2007-09-18 20:04:42 $
 *                $Revision: 1.1 $
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
    new PanelAction(QPixmap(spdritem_kur_xpm),
            tr("&Curve right panel"), 0,
            element::siciKur, this, "curverightPanel");

    new PanelAction(QPixmap(spdritem_kul_xpm),
            tr("&Curve left panel"), 0,
            element::siciKul, this, "curveleftPanel");

    new PanelAction(QPixmap(spdritem_ttl_xpm),
            tr("&Curve top vertical left"), 0,
            element::siciTtl, this, "curvetvlPanel");

    new PanelAction(QPixmap(spdritem_ttr_xpm),
            tr("&Curve top vertical right"), 0,
            element::siciTtr, this, "curvetvrPanel");

    new PanelAction(QPixmap(spdritem_tbl_xpm),
            tr("&Curve bottom vertical left"), 0,
            element::siciTbl, this, "curvebvlPanel");

    new PanelAction(QPixmap(spdritem_tbr_xpm),
            tr("&Curve bottom vertical right"), 0,
            element::siciTbr, this, "curvebvrPanel");
}

