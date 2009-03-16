/*
 * paintitemwindow.cpp
 * -------------------
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-26
 * Last modified: $Date: 2009-03-16 17:38:04 $
 *                $Revision: 1.19 $
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
#include "pixmaps/spdritem_dil.xpm"
#include "pixmaps/spdritem_dir.xpm"
#include "pixmaps/spdritem_krh.xpm"
#include "pixmaps/spdritem_krl.xpm"
#include "pixmaps/spdritem_krr.xpm"
#include "pixmaps/spdritem_tdb.xpm"
#include "pixmaps/spdritem_tdl.xpm"
#include "pixmaps/spdritem_tdr.xpm"
#include "pixmaps/spdritem_trh.xpm"
#include "pixmaps/spdritem_trv.xpm"
#include "pixmaps/spdritem_tuh.xpm" //tug
#include "pixmaps/spdritem_tuv.xpm"
#include "pixmaps/spdritem_tul.xpm"
#include "pixmaps/spdritem_tur.xpm"

/*curved track panels*/
#include "pixmaps/spdritem_cl3.xpm"
#include "pixmaps/spdritem_cl1.xpm"
#include "pixmaps/spdritem_cr1.xpm"
#include "pixmaps/spdritem_cr3.xpm"
#include "pixmaps/spdritem_cl4.xpm"
#include "pixmaps/spdritem_cr4.xpm"
#include "pixmaps/spdritem_cl2.xpm"
#include "pixmaps/spdritem_cr2.xpm"

/*switch panels */
#include "pixmaps/spdritem_tl1.xpm" //wel
#include "pixmaps/spdritem_tl3.xpm"
#include "pixmaps/spdritem_tr1.xpm" //wer
#include "pixmaps/spdritem_tr3.xpm"
#include "pixmaps/spdritem_dbl.xpm" //dwl
#include "pixmaps/spdritem_dbr.xpm"
#include "pixmaps/spdritem_dtr.xpm" //dwr
#include "pixmaps/spdritem_dtl.xpm"
#include "pixmaps/spdritem_syr.xpm" //wey
#include "pixmaps/spdritem_syl.xpm"
#include "pixmaps/spdritem_twr.xpm" //drw
#include "pixmaps/spdritem_twl.xpm"
#include "pixmaps/spdritem_sl1.xpm" //ekl
#include "pixmaps/spdritem_sl3.xpm"
#include "pixmaps/spdritem_sr1.xpm" //ekr
#include "pixmaps/spdritem_sr3.xpm"
#include "pixmaps/spdritem_dkl.xpm"
#include "pixmaps/spdritem_dkr.xpm"

/*signal panels */
#include "pixmaps/spdritem_hsl.xpm"
#include "pixmaps/spdritem_hsr.xpm"
#include "pixmaps/spdritem_hssl.xpm"
#include "pixmaps/spdritem_hssr.xpm"
#include "pixmaps/spdritem_rbl.xpm"
#include "pixmaps/spdritem_rbr.xpm"
#include "pixmaps/spdritem_sbl.xpm"
#include "pixmaps/spdritem_sbr.xpm"
#include "pixmaps/spdritem_sdl.xpm"
#include "pixmaps/spdritem_sdr.xpm"
#include "pixmaps/spdritem_shl.xpm"
#include "pixmaps/spdritem_shr.xpm"
#include "pixmaps/spdritem_ssl.xpm"
#include "pixmaps/spdritem_ssr.xpm"
#include "pixmaps/spdritem_vsl.xpm"
#include "pixmaps/spdritem_vsr.xpm"
#include "pixmaps/spdritem_wsl.xpm"
#include "pixmaps/spdritem_wsr.xpm"
#include "pixmaps/spdritem_zpl.xpm"
#include "pixmaps/spdritem_zpr.xpm"

/*miscellanous panels*/
#include "pixmaps/spdritem_adr.xpm"
#include "pixmaps/spdritem_bld.xpm"
#include "pixmaps/spdritem_dco.xpm"
#include "pixmaps/spdritem_lcr.xpm"
#include "pixmaps/spdritem_mdc.xpm"
#include "pixmaps/spdritem_rel.xpm"
#include "pixmaps/spdritem_tnt.xpm"
#include "pixmaps/spdritem_trt.xpm"

/*decorative panels*/
#include "pixmaps/spdritem_bs1.xpm"
#include "pixmaps/spdritem_bs2.xpm"
#include "pixmaps/spdritem_bs3.xpm"
#include "pixmaps/spdritem_bs4.xpm"
#include "pixmaps/spdritem_buc.xpm"
#include "pixmaps/spdritem_bul.xpm"
#include "pixmaps/spdritem_bur.xpm"
#include "pixmaps/spdritem_txt.xpm"
#include "pixmaps/spdritem_ltr.xpm" //sht
#include "pixmaps/spdritem_ltl.xpm"
#include "pixmaps/spdritem_lbr.xpm" //shb
#include "pixmaps/spdritem_lbl.xpm"
#include "pixmaps/spdritem_lsl.xpm"
#include "pixmaps/spdritem_lsr.xpm"

/*external group panels*/
#include "pixmaps/spdritem_feb.xpm"
#include "pixmaps/spdritem_fee.xpm"
#include "pixmaps/spdritem_feg.xpm"
#include "pixmaps/spdritem_fen.xpm"
#include "pixmaps/spdritem_fer.xpm"
#include "pixmaps/spdritem_fey.xpm"
#include "pixmaps/spdritem_taf.xpm"
#include "pixmaps/spdritem_tas.xpm"
#include "pixmaps/spdritem_tau.xpm"
#include "pixmaps/spdritem_taw.xpm"
#include "pixmaps/spdritem_twh.xpm"
#include "pixmaps/spdritem_tal.xpm"


PaintItemWindow::PaintItemWindow(QWidget* parent, const char* name)
: QDockWindow(QDockWindow::InDock, parent, name)
{
    setResizeEnabled(true);
    setCaption(tr("Paint items"));
    setCloseMode(Always);

    paintItemBG = new QButtonGroup(8, Qt::Horizontal, this, "paintItemBG");
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
            element::siciSt1, paintItemBG, "horizontaltrackPanel");
    pib->setOn(true);

    new PaintItemButton(QPixmap(spdritem_trv_xpm), tr("Vertical track"),
            element::siciSt2, paintItemBG, "verticaltrackPanel");

    new PaintItemButton(QPixmap(spdritem_dil_xpm), tr("Left diagonal track"),
            element::siciSt4, paintItemBG, "leftdiaPanel");

    new PaintItemButton(QPixmap(spdritem_dir_xpm), tr("Right diagonal track"),
            element::siciSt3, paintItemBG, "rightdiaPanel");

    new PaintItemButton(QPixmap(spdritem_krr_xpm), tr("Right crossing"),
            element::siciKr1, paintItemBG, "rightcrossingPanel");

    new PaintItemButton(QPixmap(spdritem_krl_xpm), tr("Left crossing"),
            element::siciKl1, paintItemBG, "leftcrossingPanel");

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

    new PaintItemButton(QPixmap(spdritem_tuv_xpm),tr("Vertical Tunnel"),
            element::siciTuv, paintItemBG, "tunnelverticalPanel");

    new PaintItemButton(QPixmap(spdritem_tuh_xpm),tr("Horizontal Tunnel"),
            element::siciTuh, paintItemBG, "tunnelhorizontalPanel");

    new PaintItemButton(QPixmap(spdritem_tul_xpm), tr("Diagonal tunnel left"),
            element::siciTul, paintItemBG, "diagonaltunnelleftPanel");

    new PaintItemButton(QPixmap(spdritem_tur_xpm), tr("Diagonal tunnel right"),
            element::siciTur, paintItemBG, "diagonaltunnelrightPanel");

    /*curved track panels*/
    new PaintItemButton(QPixmap(spdritem_cr1_xpm),
            tr("Curve right bottom"),
            element::siciCr1, paintItemBG, "curveright1Panel");

    new PaintItemButton(QPixmap(spdritem_cl1_xpm),
            tr("Curve left top"),
            element::siciCl1, paintItemBG, "curveleft1Panel");

    new PaintItemButton(QPixmap(spdritem_cr3_xpm),
            tr("Curve right top"),
            element::siciCr3, paintItemBG, "curveright3Panel");

    new PaintItemButton(QPixmap(spdritem_cl3_xpm),
            tr("Curve left bottom"),
            element::siciCl3, paintItemBG, "curveleft3Panel");

    new PaintItemButton(QPixmap(spdritem_cl2_xpm),
            tr("Curve top vertical left"),
            element::siciCl2, paintItemBG, "curveleft2Panel");

    new PaintItemButton(QPixmap(spdritem_cr2_xpm),
            tr("Curve top vertical right"),
            element::siciCr2, paintItemBG, "curveright2Panel");

    new PaintItemButton(QPixmap(spdritem_cr4_xpm),
            tr("Curve bottom vertical right"),
            element::siciCr4, paintItemBG, "curveright4Panel");

    new PaintItemButton(QPixmap(spdritem_cl4_xpm),
            tr("Curve bottom vertical left"),
            element::siciCl4, paintItemBG, "curveleft4Panel");

    /*switch panels */
    new PaintItemButton(QPixmap(spdritem_tl1_xpm),
            tr("Switch top left"),
            element::siciTl1, paintItemBG, "turnoutleft1Panel");

    new PaintItemButton(QPixmap(spdritem_tr1_xpm),
            tr("Switch bottom right"),
            element::siciTr1, paintItemBG, "turnoutright1Panel");

    new PaintItemButton(QPixmap(spdritem_tr3_xpm),
            tr("Switch top right"),
            element::siciTr3, paintItemBG, "turnoutright3Panel");

    new PaintItemButton(QPixmap(spdritem_tl3_xpm),
            tr("Switch bottom left"),
            element::siciTl3, paintItemBG, "turnoutleft3Panel");

    new PaintItemButton(QPixmap(spdritem_dtr_xpm),
            tr("Diagonal switch top right"),
            element::siciIr1, paintItemBG, "diaswitchtoprightPanel");

    new PaintItemButton(QPixmap(spdritem_dbl_xpm),
            tr("Diagonal switch bottom left"),
            element::siciIl1, paintItemBG, "diagswitchbotleftPanel");

    new PaintItemButton(QPixmap(spdritem_dtl_xpm),
            tr("Diagonal switch top left"),
            element::siciIl3, paintItemBG, "diagswitchtopleftPanel");

    new PaintItemButton(QPixmap(spdritem_dbr_xpm),
            tr("Diagonal switch bottom right"),
            element::siciIr3, paintItemBG, "diagswitchbotrightPanel");

    new PaintItemButton(QPixmap(spdritem_syr_xpm),
            tr("Y-Switch right"),
            element::siciSy1, paintItemBG, "yswitchrightPanel");

    new PaintItemButton(QPixmap(spdritem_syl_xpm),
            tr("Y-Switch left"),
            element::siciSy3, paintItemBG, "yswitchleftPanel");

    new PaintItemButton(QPixmap(spdritem_twr_xpm),
            tr("Three way turnout right"),
            element::siciTw1, paintItemBG, "treewayrightPanel");

    new PaintItemButton(QPixmap(spdritem_twl_xpm),
            tr("Three way turnout left"),
            element::siciTw3, paintItemBG, "threewayleftPanel");

    new PaintItemButton(QPixmap(spdritem_sl1_xpm),
            tr("Single-slip switch top left"),
            element::siciSl1, paintItemBG, "singleslipleft1Panel");

    new PaintItemButton(QPixmap(spdritem_sl3_xpm),
            tr("Single-slip switch bottom left"),
            element::siciSl3, paintItemBG, "singleslipleft3Panel");

    new PaintItemButton(QPixmap(spdritem_sr1_xpm),
            tr("Single-slip switch top right"),
            element::siciSr1, paintItemBG, "singleslipright1Panel");

    new PaintItemButton(QPixmap(spdritem_sr3_xpm),
            tr("Single-slip switch bottom right"),
            element::siciSr3, paintItemBG, "singleslipright3Panel");

    new PaintItemButton(QPixmap(spdritem_dkl_xpm),
            tr("Double-slip switch left"),
            element::siciDl1, paintItemBG, "doubleleftPanel");

    new PaintItemButton(QPixmap(spdritem_dkr_xpm),
            tr("Double-slip switch right"),
            element::siciDr1, paintItemBG, "doublerightPanel");

    /*signal panels */
    new PaintItemButton(QPixmap(spdritem_hsl_xpm),
            tr("Main signal left panel"),
            element::siciHs3, paintItemBG, "mainsignalleftPanel");

    new PaintItemButton(QPixmap(spdritem_hsr_xpm),
            tr("Main signal right panel"),
            element::siciHs1, paintItemBG, "mainsignalrightPanel");

    new PaintItemButton(QPixmap(spdritem_hssl_xpm),
            tr("Left main four state signal panel"),
            element::siciHss3, paintItemBG, "leftmainfourstatesignalPanel");

    new PaintItemButton(QPixmap(spdritem_hssr_xpm),
            tr("Right main four state signal panel"),
            element::siciHss1, paintItemBG, "rightmainfourstatesignalPanel");

    new PaintItemButton(QPixmap(spdritem_ssl_xpm),
            tr("Left shunt signal panel"),
            element::siciSs3, paintItemBG, "leftshuntsignalPanel");

    new PaintItemButton(QPixmap(spdritem_ssr_xpm),
            tr("Right shunt signal panel"),
            element::siciSs1, paintItemBG, "rightshuntsignalPanel");

    new PaintItemButton(QPixmap(spdritem_shl_xpm),
            tr("Left help route signal panel"),
            element::siciSh3, paintItemBG, "helproutePanel");

    new PaintItemButton(QPixmap(spdritem_shr_xpm),
            tr("Right help route signal panel"),
            element::siciSh1, paintItemBG, "helproutePanel");

    new PaintItemButton(QPixmap(spdritem_sdl_xpm),
            tr("Left double button shunt signal panel"),
            element::siciSd3, paintItemBG, "lefttwobtnshuntsignalPanel");

    new PaintItemButton(QPixmap(spdritem_sdr_xpm),
            tr("Right doouble button shunt signal panel"),
            element::siciSd1, paintItemBG, "righttwobtnshuntsignalPanel");

    new PaintItemButton(QPixmap(spdritem_wsl_xpm),
            tr("Left wait signal panel"),
            element::siciWs3, paintItemBG, "leftwaitsignalPanel");

    new PaintItemButton(QPixmap(spdritem_wsr_xpm),
            tr("Right wait signal panel"),
            element::siciWs1, paintItemBG, "RightwaitsignalPanel");

    new PaintItemButton(QPixmap(spdritem_rbl_xpm),
            tr("Left route button panel"),
            element::siciZt3, paintItemBG, "leftroutebuttonPanel");

    new PaintItemButton(QPixmap(spdritem_rbr_xpm),
            tr("Right route button panel"),
            element::siciZt1, paintItemBG, "rightroutebuttonPanel");

    new PaintItemButton(QPixmap(spdritem_sbl_xpm),
            tr("Left shunt button panel"),
            element::siciRt3, paintItemBG, "leftshuntbuttonPanel");

    new PaintItemButton(QPixmap(spdritem_sbr_xpm),
            tr("Right shunt button panel"),
            element::siciRt1, paintItemBG, "rightshuntbuttonPanel");

    new PaintItemButton(QPixmap(spdritem_vsl_xpm),
            tr("Left distant signal panel"),
            element::siciVs3, paintItemBG, "leftdistantsignalPanel");

    new PaintItemButton(QPixmap(spdritem_vsr_xpm),
            tr("Right distant signal panel"),
            element::siciVs1, paintItemBG, "rightdistantsignalPanel");

    new PaintItemButton(QPixmap(spdritem_zpl_xpm),
            tr("Left ZP signal panel"),
            element::siciZp3, paintItemBG, "leftzpsignalPanel");

    new PaintItemButton(QPixmap(spdritem_zpr_xpm),
            tr("Right ZP signal panel"),
            element::siciZp1, paintItemBG, "rightzpsignalPanel");

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
    new PaintItemButton(QPixmap(spdritem_bs1_xpm), tr("Right buffer stop"),
            element::siciBs1, paintItemBG, "bufferstop1Panel");

    new PaintItemButton(QPixmap(spdritem_bs2_xpm), tr("Top buffer stop"),
            element::siciBs2, paintItemBG, "bufferstop2Panel");

    new PaintItemButton(QPixmap(spdritem_bs3_xpm), tr("Left buffer stop"),
            element::siciBs3, paintItemBG, "bufferstop3Panel");

    new PaintItemButton(QPixmap(spdritem_bs4_xpm), tr("Bottom buffer stop"),
            element::siciBs4, paintItemBG, "bufferstop4Panel");

    new PaintItemButton(QPixmap(spdritem_txt_xpm), tr("Text field panel"),
            element::siciTxt, paintItemBG, "textfieldPanel");

    new PaintItemButton(QPixmap(spdritem_buc_xpm), tr("House center wing"),
            element::siciBuc, paintItemBG, "centerwingPanel");

    new PaintItemButton(QPixmap(spdritem_bul_xpm), tr("Building left wing"),
            element::siciBul, paintItemBG, "buildingleftwingPanel");

    new PaintItemButton(QPixmap(spdritem_bur_xpm), tr("Building right wing"),
            element::siciBur, paintItemBG, "buildingrightwingPanel");

    new PaintItemButton(QPixmap(spdritem_ltr_xpm), tr("Top right loco shed"),
            element::siciLt1, paintItemBG, "locoshedtrPanel");

    new PaintItemButton(QPixmap(spdritem_lbr_xpm), tr("Bottom right loco shed"),
            element::siciLb1, paintItemBG, "locoshedbrPanel");

    new PaintItemButton(QPixmap(spdritem_lsr_xpm), tr("Middle loco shed right"),
            element::siciLs1, paintItemBG, "locoshedmrightPanel");

    new PaintItemButton(QPixmap(spdritem_lsl_xpm), tr("Middle loco shed left"),
            element::siciLs3, paintItemBG, "locoshedmleftPanel");

    new PaintItemButton(QPixmap(spdritem_ltl_xpm), tr("Top left loco shed"),
            element::siciLt3, paintItemBG, "locoshedtlPanel");

    new PaintItemButton(QPixmap(spdritem_lbl_xpm), tr("Bottom left loco shed"),
            element::siciLb3, paintItemBG, "locoshedblPanel");

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

    new PaintItemButton(QPixmap(spdritem_twh_xpm), tr("WHT panel"),
            element::siciTwh, paintItemBG, "whtPanel");

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

    new PaintItemButton(QPixmap(spdritem_tal_xpm),
            tr("Table light panel"),
            element::siciTal, paintItemBG, "lightPanel");
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
