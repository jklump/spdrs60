/*
 * signalpanels.cpp
 * ----------------
 * Copyright    : (C) 2007 Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-19
 * Last modified: $Date: 2007-09-22 08:05:17 $
 *                $Revision: 1.1 $
 *
 * This file provides menu and toolbar action to switch the paint item
 * selection between different signal panels.
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#include "signalpanels.h"
#include "panelaction.h"

#include "pixmaps/spdritem_hs.xpm"
#include "pixmaps/spdritem_hss.xpm"
#include "pixmaps/spdritem_ss.xpm"
#include "pixmaps/spdritem_ssh.xpm"
#include "pixmaps/spdritem_sss.xpm"
#include "pixmaps/spdritem_ws.xpm"
#include "pixmaps/spdritem_vs.xpm"
#include "pixmaps/spdritem_zp.xpm"
#include "pixmaps/spdritem_nrb.xpm"
#include "pixmaps/spdritem_srb.xpm"

    
SignalPanels::SignalPanels(QObject* parent, const char* name):
    PanelActionGroup(parent, name)
{

    QIconSet is;

    is.setPixmap(QPixmap(spdritem_hs_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Main signal panel"), 0,
            element::siciHs, this, "mainsignalPanel");

    is.setPixmap(QPixmap(spdritem_hss_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Main four state signal panel"), 0,
            element::siciHss, this, "mainfourstatesignalPanel");

    is.setPixmap(QPixmap(spdritem_ss_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Shunt signal panel"), 0,
            element::siciSs, this, "shuntsignalPanel");

    is.setPixmap(QPixmap(spdritem_ssh_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Help route signal panel"), 0,
            element::siciSsh, this, "helproutePanel");

    is.setPixmap(QPixmap(spdritem_sss_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Two button shunt signal panel"), 0,
            element::siciSss, this, "twobtnshuntsignalPanel");

    is.setPixmap(QPixmap(spdritem_ws_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Wait signal panel"), 0,
            element::siciWs, this, "waitsignalPanel");

    is.setPixmap(QPixmap(spdritem_vs_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Distant signal panel"), 0,
            element::siciVs, this, "distantsignalPanel");

    is.setPixmap(QPixmap(spdritem_zp_xpm), QIconSet::Small);
    new PanelAction(is, tr("&ZP signal panel"), 0,
            element::siciZp, this, "zpsignalPanel");

    is.setPixmap(QPixmap(spdritem_nrb_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Route button panel"), 0,
            element::siciNrb, this, "routebuttonPanel");

    is.setPixmap(QPixmap(spdritem_srb_xpm), QIconSet::Small);
    new PanelAction(is, tr("&Shunt button panel"), 0,
            element::siciSrb, this, "shuntbuttonPanel");
}

