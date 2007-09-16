/*
 * externalgrouppanels.cpp
 * -------------------------------
 * Copyright    : (C) 2007 Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-16
 * Last modified: $Date: 2007-09-16 21:02:24 $
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


#include "externalgrouppanels.h"

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
 QActionGroup(parent, name , true)
{
#if QT_VERSION >= 0x030200
    actionSpdrFeg = new QAction(QPixmap(spdritem_feg_xpm),
            tr("&Route panel"), 0, this, "routePanel");
#else
    actionSpdrFeg = new QAction("", QPixmap(spdritem_feg_xpm),
            tr("&Route panel"), 0, this, "routePanel");
#endif
    actionSpdrFeg->setToggleAction(true);


#if QT_VERSION >= 0x030200
    actionSpdrTaf = new QAction(QPixmap(spdritem_taf_xpm),
            tr("&FHT panel"), 0, this, "fhtPanel");
#else
    actionSpdrTaf = new QAction("", QPixmap(spdritem_taf_xpm),
            tr("&FHT panel"), 0, this, "fhtPanel");
#endif
    actionSpdrTaf->setToggleAction(true);


#if QT_VERSION >= 0x030200
    actionSpdrTau = new QAction(QPixmap(spdritem_tau_xpm),
            tr("&UfGT panel"), 0, this, "ufgtPanel");
#else
    actionSpdrTau = new QAction("", QPixmap(spdritem_tau_xpm),
            tr("&UfGT panel"), 0, this, "ufgtPanel");
#endif
    actionSpdrTau->setToggleAction(true);


#if QT_VERSION >= 0x030200
    actionSpdrFeb = new QAction(QPixmap(spdritem_feb_xpm),
            tr("&Turnout panel"), 0, this, "turnoutPanel");
#else
    actionSpdrFeb = new QAction("", QPixmap(spdritem_feb_xpm),
            tr("&Turnout panel"), 0, this, "turnoutPanel");
#endif
    actionSpdrFeb->setToggleAction(true);


#if QT_VERSION >= 0x030200
    actionSpdrTaw = new QAction(QPixmap(spdritem_taw_xpm),
            tr("&WGT panel"), 0, this, "wgtPanel");
#else
    actionSpdrTaw = new QAction("", QPixmap(spdritem_taw_xpm),
            tr("&WGT panel"), 0, this, "wgtPanel");
#endif
    actionSpdrTaw->setToggleAction(true);


#if QT_VERSION >= 0x030200
    actionSpdrFer = new QAction(QPixmap(spdritem_fer_xpm),
            tr("&Signal panel"), 0, this, "signalPanel");
#else
    actionSpdrFer = new QAction("", QPixmap(spdritem_fer_xpm),
            tr("&Signal panel"), 0, this, "signalPanel");
#endif
    actionSpdrFer->setToggleAction(true);


#if QT_VERSION >= 0x030200
    actionSpdrTas = new QAction(QPixmap(spdritem_tas_xpm),
            tr("&SGT panel"), 0, this, "sgtPanel");
#else
    actionSpdrTas = new QAction("", QPixmap(spdritem_tas_xpm),
            tr("&SGT panel"), 0, this, "sgtPanel");
#endif
    actionSpdrTas->setToggleAction(true);


#if QT_VERSION >= 0x030200
    actionSpdrFey = new QAction(QPixmap(spdritem_fey_xpm),
            tr("&Level crossing panel"), 0, this, "crossingPanel");
#else
    actionSpdrFey = new QAction("", QPixmap(spdritem_fey_xpm),
            tr("&Level crossing panel"), 0, this, "crossingPanel");
#endif
    actionSpdrFey->setToggleAction(true);


#if QT_VERSION >= 0x030200
    actionSpdrFen = new QAction(QPixmap(spdritem_fen_xpm),
            tr("&Axle counter panel"), 0, this, "axlePanel");
#else
    actionSpdrFen = new QAction("", QPixmap(spdritem_fen_xpm),
            tr("&Axle counter panel"), 0, this, "axlePanel");
#endif
    actionSpdrFen->setToggleAction(true);


#if QT_VERSION >= 0x030200
    actionSpdrFee = new QAction(QPixmap(spdritem_fee_xpm),
            tr("&Power supply panel"), 0, this, "powerPanel");
#else
    actionSpdrFee = new QAction("", QPixmap(spdritem_fee_xpm),
            tr("&Power supply panel"), 0, this, "powerPanel");
#endif
    actionSpdrFee->setToggleAction(true);


    connect(this, SIGNAL(selected(QAction*)),
            this, SLOT(spdrItemSelected(QAction*)));
}


/*
 *translate selected action to a public integer value
 */
void ExternalGroupPanels::spdrItemSelected(QAction* ac)
{
    element::SpdrItemClassId sici = element::siciFeg;

    if (ac == actionSpdrTaf)
        sici = element::siciTaf;
    else if (ac == actionSpdrTau)
        sici = element::siciTau;
    else if (ac == actionSpdrFeb)
        sici = element::siciFeb;
    else if (ac == actionSpdrTaw)
        sici = element::siciTaw;
    else if (ac == actionSpdrFer)
        sici = element::siciFer;
    else if (ac == actionSpdrTas)
        sici = element::siciTas;
    else if (ac == actionSpdrFey)
        sici = element::siciFey;
    else if (ac == actionSpdrFen)
        sici = element::siciFen;
    else if (ac == actionSpdrFee)
        sici = element::siciFee;

    emit paintItemChanged(sici);
}

/*
 * reset to select mode if layout edit mode is disabled
 */
void ExternalGroupPanels::enablePaintMode(bool enable)
{
    if (!enable)
       actionSpdrFeg->setOn(true);
}

