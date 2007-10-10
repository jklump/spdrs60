/*
 * paintitemwindow.cpp
 * -------------------
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-26
 * Last modified: $Date: 2007-10-10 19:41:51 $
 *                $Revision: 1.6 $
 *
 * This code creates a dockable window with a set of paint items 
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qbuttongroup.h>

#include "paintitembutton.h"
#include "paintitemwindow.h"

/*straight track panels*/
#include "pixmaps/spdritem_trh.xpm"
#include "pixmaps/spdritem_trv.xpm"
#include "pixmaps/spdritem_dil.xpm"
#include "pixmaps/spdritem_dir.xpm"
#include "pixmaps/spdritem_tdr.xpm"
#include "pixmaps/spdritem_tdl.xpm"
#include "pixmaps/spdritem_tdb.xpm"
#include "pixmaps/spdritem_krh.xpm"
#include "pixmaps/spdritem_krr.xpm"
#include "pixmaps/spdritem_krl.xpm"

/*curved track panels*/
#include "pixmaps/spdritem_crb.xpm"
#include "pixmaps/spdritem_clt.xpm"
#include "pixmaps/spdritem_clb.xpm"
#include "pixmaps/spdritem_crt.xpm"
#include "pixmaps/spdritem_ttl.xpm"
#include "pixmaps/spdritem_ttr.xpm"
#include "pixmaps/spdritem_tbl.xpm"
#include "pixmaps/spdritem_tbr.xpm"

/*switch panels */
#include "pixmaps/spdritem_wel.xpm"
#include "pixmaps/spdritem_wer.xpm"
#include "pixmaps/spdritem_dwl.xpm"
#include "pixmaps/spdritem_dwr.xpm"
#include "pixmaps/spdritem_wey.xpm"
#include "pixmaps/spdritem_drw.xpm"
#include "pixmaps/spdritem_ekl.xpm"
#include "pixmaps/spdritem_ekr.xpm"
#include "pixmaps/spdritem_dkl.xpm"
#include "pixmaps/spdritem_dkr.xpm"

/*signal panels */
#include "pixmaps/spdritem_hsr.xpm"
#include "pixmaps/spdritem_hsl.xpm"
#include "pixmaps/spdritem_hssr.xpm"
#include "pixmaps/spdritem_hssl.xpm"
#include "pixmaps/spdritem_ssr.xpm"
#include "pixmaps/spdritem_ssl.xpm"
#include "pixmaps/spdritem_shr.xpm"
#include "pixmaps/spdritem_shl.xpm"
#include "pixmaps/spdritem_sdr.xpm"
#include "pixmaps/spdritem_sdl.xpm"
#include "pixmaps/spdritem_wsr.xpm"
#include "pixmaps/spdritem_wsl.xpm"
#include "pixmaps/spdritem_vsr.xpm"
#include "pixmaps/spdritem_vsl.xpm"
#include "pixmaps/spdritem_zpr.xpm"
#include "pixmaps/spdritem_zpl.xpm"
#include "pixmaps/spdritem_rbr.xpm"
#include "pixmaps/spdritem_rbl.xpm"
#include "pixmaps/spdritem_sbr.xpm"
#include "pixmaps/spdritem_sbl.xpm"

/*miscellanous panels*/
#include "pixmaps/spdritem_dco.xpm"
#include "pixmaps/spdritem_bld.xpm"
#include "pixmaps/spdritem_adr.xpm"
#include "pixmaps/spdritem_lcr.xpm"
#include "pixmaps/spdritem_rel.xpm"
#include "pixmaps/spdritem_mdc.xpm"
#include "pixmaps/spdritem_tnt.xpm"
#include "pixmaps/spdritem_trt.xpm"

/*decorative panels*/
#include "pixmaps/spdritem_bsl.xpm"
#include "pixmaps/spdritem_bsr.xpm"
#include "pixmaps/spdritem_tug.xpm"
#include "pixmaps/spdritem_dlt.xpm"
#include "pixmaps/spdritem_drt.xpm"
#include "pixmaps/spdritem_lee.xpm"
#include "pixmaps/spdritem_hs1.xpm"
#include "pixmaps/spdritem_bul.xpm"
#include "pixmaps/spdritem_bur.xpm"
#include "pixmaps/spdritem_sht.xpm"
#include "pixmaps/spdritem_shm.xpm"
#include "pixmaps/spdritem_shb.xpm"

/*external group panels*/
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


PaintItemWindow::PaintItemWindow(QWidget* parent, const char* name)
: QDockWindow(QDockWindow::InDock, parent, name)
{
    setResizeEnabled(true);
    setCaption(tr("Paint items"));
    setCloseMode(Always);

    paintItemBG = new QButtonGroup(6, Qt::Horizontal, this, "paintItemBG");
    Q_CHECK_PTR(paintItemBG);
    paintItemBG->setExclusive(true);
    paintItemBG->setInsideSpacing(0);
    paintItemBG->setInsideMargin(0);
    paintItemBG->setEnabled(false);
    setWidget(paintItemBG);
    connect(paintItemBG, SIGNAL(pressed(int)),
            this, SLOT(paintItemPressed(int)));

    /*straight track panels*/
    PaintItemButton* pib;
    pib = new PaintItemButton(QPixmap(spdritem_trh_xpm),
            tr("Horizontal track"),
            element::siciGer, paintItemBG, "horizontaltrackPanel");
    pib->setOn(true);

    new PaintItemButton(QPixmap(spdritem_trv_xpm), tr("Vertical track"),
            element::siciTrv, paintItemBG, "verticaltrackPanel");

    new PaintItemButton(QPixmap(spdritem_dil_xpm), tr("Left diagonal track"),
            element::siciDil, paintItemBG, "leftdiaPanel");

    new PaintItemButton(QPixmap(spdritem_dir_xpm), tr("Right diagonal track"),
            element::siciDir, paintItemBG, "rightdiaPanel");

    new PaintItemButton(QPixmap(spdritem_krr_xpm), tr("Right crossing"),
            element::siciKrr, paintItemBG, "rightcrossingPanel");

    new PaintItemButton(QPixmap(spdritem_krl_xpm), tr("Left crossing"),
            element::siciKrl, paintItemBG, "leftcrossingPanel");

    new PaintItemButton(QPixmap(spdritem_krh_xpm), tr("Diagonal crossing"),
            element::siciKrh, paintItemBG, "crossingPanel");

    new PaintItemButton(QPixmap(spdritem_tdb_xpm),
            tr("Double direction track"),
            element::siciTdb, paintItemBG, "bothdirectionPanel");

    new PaintItemButton(QPixmap(spdritem_tdr_xpm),
            tr("Single direction track right"),
            element::siciTdr, paintItemBG, "rightdirectionPanel");

    new PaintItemButton(QPixmap(spdritem_tdl_xpm),
            tr("Single direction track left"),
            element::siciTdl, paintItemBG, "leftdirectionPanel");

    /*curved track panels*/
    new PaintItemButton(QPixmap(spdritem_crb_xpm),
            tr("Curve right bottom"),
            element::siciCrb, paintItemBG, "curverbotPanel");

    new PaintItemButton(QPixmap(spdritem_clt_xpm),
            tr("Curve left top"),
            element::siciClt, paintItemBG, "curveltopPanel");

    new PaintItemButton(QPixmap(spdritem_crt_xpm),
            tr("Curve right top"),
            element::siciCrt, paintItemBG, "curvertopPanel");

    new PaintItemButton(QPixmap(spdritem_clb_xpm),
            tr("Curve left bottom"),
            element::siciClb, paintItemBG, "curvelbotPanel");

    new PaintItemButton(QPixmap(spdritem_ttl_xpm),
            tr("Curve top vertical left"),
            element::siciTtl, paintItemBG, "curvetvlPanel");

    new PaintItemButton(QPixmap(spdritem_ttr_xpm),
            tr("Curve top vertical right"),
            element::siciTtr, paintItemBG, "curvetvrPanel");

    new PaintItemButton(QPixmap(spdritem_tbl_xpm),
            tr("Curve bottom vertical left"),
            element::siciTbl, paintItemBG, "curvebvlPanel");

    new PaintItemButton(QPixmap(spdritem_tbr_xpm),
            tr("Curve bottom vertical right"),
            element::siciTbr, paintItemBG, "curvebvrPanel");

    /*switch panels */
    new PaintItemButton(QPixmap(spdritem_wel_xpm),
            tr("Switch left"),
            element::siciWel, paintItemBG, "switchleftPanel");

    new PaintItemButton(QPixmap(spdritem_wer_xpm),
            tr("Switch right"),
            element::siciWer, paintItemBG, "switchrightPanel");

    new PaintItemButton(QPixmap(spdritem_dwl_xpm),
            tr("Diagonal switch left"),
            element::siciDwl, paintItemBG, "diaswitchleftPanel");

    new PaintItemButton(QPixmap(spdritem_dwr_xpm),
            tr("Diagonal switch right"),
            element::siciDwr, paintItemBG, "diaswitchrightPanel");

    new PaintItemButton(QPixmap(spdritem_wey_xpm),
            tr("Y-Switch"),
            element::siciWey, paintItemBG, "yswitchPanel");

    new PaintItemButton(QPixmap(spdritem_drw_xpm),
            tr("Three way turnout"),
            element::siciDrw, paintItemBG, "waitsignalPanel");

    new PaintItemButton(QPixmap(spdritem_ekl_xpm),
            tr("Single-slip switch left"),
            element::siciEkl, paintItemBG, "singleleftPanel");

    new PaintItemButton(QPixmap(spdritem_ekr_xpm),
            tr("Single-slip switch right"),
            element::siciEkr, paintItemBG, "singlerightPanel");

    new PaintItemButton(QPixmap(spdritem_dkl_xpm),
            tr("Double-slip switch left"),
            element::siciDkl, paintItemBG, "doubleleftPanel");

    new PaintItemButton(QPixmap(spdritem_dkr_xpm),
            tr("Double-slip switch right"),
            element::siciDkr, paintItemBG, "doublerightPanel");

    /*signal panels */
    new PaintItemButton(QPixmap(spdritem_hsl_xpm),
            tr("Main signal left panel"),
            element::siciHsl, paintItemBG, "mainsignalleftPanel");

    new PaintItemButton(QPixmap(spdritem_hsr_xpm),
            tr("Main signal right panel"),
            element::siciHsr, paintItemBG, "mainsignalrightPanel");

    new PaintItemButton(QPixmap(spdritem_hssl_xpm),
            tr("Left main four state signal panel"),
            element::siciHssl, paintItemBG, "leftmainfourstatesignalPanel");

    new PaintItemButton(QPixmap(spdritem_hssr_xpm),
            tr("Right main four state signal panel"),
            element::siciHssr, paintItemBG, "rightmainfourstatesignalPanel");

    new PaintItemButton(QPixmap(spdritem_ssl_xpm),
            tr("Left shunt signal panel"),
            element::siciSsl, paintItemBG, "leftshuntsignalPanel");

    new PaintItemButton(QPixmap(spdritem_ssr_xpm),
            tr("Right shunt signal panel"),
            element::siciSsr, paintItemBG, "rightshuntsignalPanel");

    new PaintItemButton(QPixmap(spdritem_shl_xpm),
            tr("Left help route signal panel"),
            element::siciShl, paintItemBG, "helproutePanel");

    new PaintItemButton(QPixmap(spdritem_shr_xpm),
            tr("Right help route signal panel"),
            element::siciShr, paintItemBG, "helproutePanel");

    new PaintItemButton(QPixmap(spdritem_sdl_xpm),
            tr("Left double button shunt signal panel"),
            element::siciSdl, paintItemBG, "lefttwobtnshuntsignalPanel");

    new PaintItemButton(QPixmap(spdritem_sdr_xpm),
            tr("Right doouble button shunt signal panel"),
            element::siciSdr, paintItemBG, "righttwobtnshuntsignalPanel");

    new PaintItemButton(QPixmap(spdritem_wsl_xpm),
            tr("Left wait signal panel"),
            element::siciWsl, paintItemBG, "leftwaitsignalPanel");

    new PaintItemButton(QPixmap(spdritem_wsr_xpm),
            tr("Right wait signal panel"),
            element::siciWsr, paintItemBG, "RightwaitsignalPanel");

    new PaintItemButton(QPixmap(spdritem_rbl_xpm),
            tr("Left route button panel"),
            element::siciRbl, paintItemBG, "leftroutebuttonPanel");

    new PaintItemButton(QPixmap(spdritem_rbr_xpm),
            tr("Right route button panel"),
            element::siciRbr, paintItemBG, "rightroutebuttonPanel");

    new PaintItemButton(QPixmap(spdritem_sbl_xpm),
            tr("Left shunt button panel"),
            element::siciSbl, paintItemBG, "leftshuntbuttonPanel");

    new PaintItemButton(QPixmap(spdritem_sbr_xpm),
            tr("Right shunt button panel"),
            element::siciSbr, paintItemBG, "rightshuntbuttonPanel");

    new PaintItemButton(QPixmap(spdritem_vsl_xpm),
            tr("Left distant signal panel"),
            element::siciVsl, paintItemBG, "leftdistantsignalPanel");

    new PaintItemButton(QPixmap(spdritem_vsr_xpm),
            tr("Right distant signal panel"),
            element::siciVsr, paintItemBG, "rightdistantsignalPanel");

    new PaintItemButton(QPixmap(spdritem_zpl_xpm),
            tr("Left ZP signal panel"),
            element::siciZpl, paintItemBG, "leftzpsignalPanel");

    new PaintItemButton(QPixmap(spdritem_zpr_xpm),
            tr("Right ZP signal panel"),
            element::siciZpr, paintItemBG, "rightzpsignalPanel");

    /*miscellanous panels*/
    new PaintItemButton(QPixmap(spdritem_dco_xpm), tr("Decoupler"),
            element::siciEnk, paintItemBG, "decouplerPanel");

    new PaintItemButton(QPixmap(spdritem_bld_xpm), tr("Blind element"),
            element::siciBld, paintItemBG, "blindPanel");

    new PaintItemButton(QPixmap(spdritem_adr_xpm), tr("Address indicator"),
            element::siciAdr, paintItemBG, "addressPanel");

    new PaintItemButton(QPixmap(spdritem_lcr_xpm), tr("Level crossing"),
            element::siciBue, paintItemBG, "levelcrossingPanel");

    new PaintItemButton(QPixmap(spdritem_rel_xpm), tr("Relais"),
            element::siciRel, paintItemBG, "relaisPanel");

    new PaintItemButton(QPixmap(spdritem_mdc_xpm), tr("DC motor"),
            element::siciMdc, paintItemBG, "dcmotorPanel");

    new PaintItemButton(QPixmap(spdritem_tnt_xpm), tr("Turntable"),
            element::siciDre, paintItemBG, "turntablePanel");

    new PaintItemButton(QPixmap(spdritem_trt_xpm), tr("Transfer table"),
            element::siciSbn, paintItemBG, "transfertablePanel");

    /*decorative panels*/
    new PaintItemButton(QPixmap(spdritem_bsl_xpm), tr("Left buffer stop"),
            element::siciBsl, paintItemBG, "leftbufferstopPanel");

    new PaintItemButton(QPixmap(spdritem_bsr_xpm), tr("Right buffer stop"),
            element::siciBsr, paintItemBG, "rightbufferstopPanel");

    new PaintItemButton(QPixmap(spdritem_tug_xpm),tr("Straight tunnel"),
            element::siciGet, paintItemBG, "tunnelstraightPanel");

    new PaintItemButton(QPixmap(spdritem_dlt_xpm), tr("Tunnel left"),
            element::siciDlt, paintItemBG, "tunnelleftPanel");

    new PaintItemButton(QPixmap(spdritem_drt_xpm), tr("Tunnel right"),
            element::siciDrt, paintItemBG, "tunnelrightPanel");

    new PaintItemButton(QPixmap(spdritem_lee_xpm), tr("Text field panel"),
            element::siciLee, paintItemBG, "textfieldPanel");

    new PaintItemButton(QPixmap(spdritem_hs1_xpm), tr("House center wing"),
            element::siciHs1, paintItemBG, "centerwingPanel");

    new PaintItemButton(QPixmap(spdritem_bul_xpm), tr("Building left wing"),
            element::siciBul, paintItemBG, "buildingleftwingPanel");

    new PaintItemButton(QPixmap(spdritem_bur_xpm), tr("Building right wing"),
            element::siciBur, paintItemBG, "buildingrightwingPanel");

    new PaintItemButton(QPixmap(spdritem_sht_xpm), tr("Top loco shed"),
            element::siciSho, paintItemBG, "locoshedtPanel");

    new PaintItemButton(QPixmap(spdritem_shm_xpm), tr("Middle loco shed"),
            element::siciShm, paintItemBG, "locoshedmPanel");

    new PaintItemButton(QPixmap(spdritem_shb_xpm), tr("Bottom loco shed"),
            element::siciShu, paintItemBG, "locoshedbPanel");

    /*external group panels*/
    new PaintItemButton(QPixmap(spdritem_feg_xpm),
            tr("Route group panel"),
            element::siciFeg, paintItemBG, "routePanel");

    new PaintItemButton(QPixmap(spdritem_taf_xpm), tr("FHT panel"),
            element::siciTaf, paintItemBG, "fhtPanel");

    new PaintItemButton(QPixmap(spdritem_tau_xpm),
            tr("UfGT and MGT panel"),
            element::siciTau, paintItemBG, "ufgtPanel");

    new PaintItemButton(QPixmap(spdritem_feb_xpm),
            tr("Turnout group panel"),
            element::siciFeb, paintItemBG, "turnoutPanel");

    new PaintItemButton(QPixmap(spdritem_taw_xpm), tr("WGT panel"),
            element::siciTaw, paintItemBG, "wgtPanel");

    new PaintItemButton(QPixmap(spdritem_fer_xpm),
            tr("Signal group panel"),
            element::siciFer, paintItemBG, "signalPanel");

    new PaintItemButton(QPixmap(spdritem_tas_xpm),
            tr("SGT and HaGT panel"),
            element::siciTas, paintItemBG, "sgtPanel");

    new PaintItemButton(QPixmap(spdritem_fey_xpm),
            tr("Level crossing group panel"),
            element::siciFey, paintItemBG, "crossingPanel");

    new PaintItemButton(QPixmap(spdritem_fen_xpm),
            tr("Axle counter group panel"),
            element::siciFen, paintItemBG, "axlePanel");

    new PaintItemButton(QPixmap(spdritem_fee_xpm),
            tr("Power supply group panel"),
            element::siciFee, paintItemBG, "powerPanel");

}

/*
 * translate selected button id to a public paint item value
 */
void PaintItemWindow::paintItemPressed(int bi)
{
#if QT_VERSION >= 0x040000
    QAbstractButton* pib = paintItemBG->find(bi);
#else
    QButton* pib = paintItemBG->find(bi);
#endif

    if (pib == NULL)
        return;

    emit paintItemChanged(static_cast<PaintItemButton*>(pib)->Sici());
}

/*
 * disable/enable paint items
 */
void PaintItemWindow::changeLayoutEditMode(GBSArea::LayoutEditMode mode)
{
    paintItemBG->setEnabled(mode == GBSArea::lemPaint);
}
