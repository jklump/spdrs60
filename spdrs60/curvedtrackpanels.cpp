/*
 * curvedtrackpanels.cpp
 * -------------------------------
 * Copyright    : (C) 2007 Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-16
 * Last modified: $Date: 2007-09-24 20:57:03 $
 *                $Revision: 1.3 $
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

#include "pixmaps/spdritem_crb.xpm"
#include "pixmaps/spdritem_clt.xpm"
#include "pixmaps/spdritem_clb.xpm"
#include "pixmaps/spdritem_crt.xpm"
#include "pixmaps/spdritem_ttl.xpm"
#include "pixmaps/spdritem_ttr.xpm"
#include "pixmaps/spdritem_tbl.xpm"
#include "pixmaps/spdritem_tbr.xpm"

    
CurvedTrackPanels::CurvedTrackPanels(QObject* parent, const char* name):
    PanelActionGroup(parent, name)
{
    QIconSet is;

    is.setPixmap(QPixmap(spdritem_crb_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Curve right bottom"), 0,
            element::siciCrb, this, "curverbotPanel");

    is.setPixmap(QPixmap(spdritem_clt_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Curve left top"), 0,
            element::siciClt, this, "curveltopPanel");

    is.setPixmap(QPixmap(spdritem_clb_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Curve left bottom"), 0,
            element::siciClb, this, "curvelbotPanel");

    is.setPixmap(QPixmap(spdritem_crt_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Curve right top"), 0,
            element::siciCrt, this, "curvertopPanel");

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

