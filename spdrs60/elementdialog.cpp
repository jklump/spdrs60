/***************************************************************************
                           elementDialog.cpp
                           version 0.4.7
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : 2005-01-09
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 this file provides an user interface to change the properties of one element
 ***************************************************************************/

#include <ctype.h>              // for isdigit()

#include "elementdialog.h"

/*button icons*/
#include "pixmaps/viewfeedback.xpm"
#include "pixmaps/drehscheibe_st2.xpm"
#include "pixmaps/drehscheibe_st3.xpm"
#include "pixmaps/dkw_links_st2.xpm"
#include "pixmaps/dkw_links_st3.xpm"
#include "pixmaps/dkw_rechts_st2.xpm"
#include "pixmaps/dkw_rechts_st3.xpm"
#include "pixmaps/signal_vs_st1.xpm"
#include "pixmaps/signal_vs_st2.xpm"
#include "pixmaps/signal_vs_st3.xpm"
#include "pixmaps/signal_hs_st1.xpm"
#include "pixmaps/signal_hs_st2.xpm"
#include "pixmaps/signal_hs_st3.xpm"
#include "pixmaps/signal_hss_st1.xpm"
#include "pixmaps/signal_hss_st2.xpm"
#include "pixmaps/signal_hss_st3.xpm"
#include "pixmaps/entkoppler_st1.xpm"
#include "pixmaps/entkoppler_st2.xpm"
#include "pixmaps/entkoppler_st3.xpm"

extern int DEF_PROTOCOL;
extern int ACTIVE_TIME;
extern int FEEDBACK;
extern int FB_MODULES_[4];
extern bool SHOW_TOOLTIPS;

extern QString DEF_DECODER;


elementDialog::elementDialog(QWidget * parent, QStrList * listElementData_)
:  QDialog(0, "elementDialog", true)   // true, parent window not
{                               // usable until this closed
    if (parent);                // dummy command to avoid compiler warning
    bBlockMSignals = false;
    bBlockBSignals = false;
    listElementData = new QStrList(true);
    listElementData = listElementData_;

    setupDataFrame();
    setupLogicFrame();

    buttOK = new QPushButton(tr("&OK"), this);
    buttOK->setDefault(true);
    buttOK->move((2 * frLogic->width() + 30) / 3 - buttOK->width() / 2,
                 frLogic->y() + frLogic->height() + 10);
    connect(buttOK, SIGNAL(clicked()), this, SLOT(slotApplyPressed()));

    QPushButton *CancelButton = new QPushButton(tr("&Cancel"), this);
    CancelButton->move((2 * frLogic->width() + 30) * 2 / 3 -
                       CancelButton->width() / 2,
                       frLogic->y() + frLogic->height() + 10);
    connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    // now fill the widgets with data
    slotSymbolChanged(coboIcon->currentItem());

    this->setFixedWidth(2 * frData->width() + 3 * frData->x());
    this->setFixedHeight(buttOK->y() + buttOK->height() + 10);
    this->setTabOrder(leText, leAddress_1);
    this->setTabOrder(leAddress_1, leAddress_2);
}



void elementDialog::setupDataFrame()
{
    frData = new QGroupBox(tr("Data"), this, "dataGroupBox");
    frData->move(10, 10);
    frData->resize(260, 415);
    frData->show();


    listIcon = new QStrList(true);
    coboIcon = new QComboBox(false, frData);    // false = not editable
    coboIcon->show();
    setupElement(SYM_LEE);
    setupElement(SYM_GER);
    setupElement(SYM_KUL);
    setupElement(SYM_KUR);
    setupElement(SYM_DIL);
    setupElement(SYM_DIR);
    setupElement(SYM_WEL);
    setupElement(SYM_WER);
    setupElement(SYM_DWL);
    setupElement(SYM_DWR);
    setupElement(SYM_WEY);
    
    setupElement(SYM_HS);
    setupElement(SYM_HSS);
    setupElement(SYM_SS);
    setupElement(SYM_SSH);
    setupElement(SYM_SSS);
    setupElement(SYM_VS);
    setupElement(SYM_WS);
    setupElement(SYM_ZP);
    setupElement(SYM_NRB);
    setupElement(SYM_SRB);

    setupElement(SYM_EKL);
    setupElement(SYM_EKR);
    setupElement(SYM_DKL);
    setupElement(SYM_DKR);
    setupElement(SYM_DRW);
    setupElement(SYM_KRH);
    setupElement(SYM_KRL);
    setupElement(SYM_KRR);
    setupElement(SYM_ENK);
    setupElement(SYM_PRE);
    setupElement(SYM_RI1);
    setupElement(SYM_RI2);
    setupElement(SYM_DLT);
    setupElement(SYM_DRT);
    setupElement(SYM_GET);
    setupElement(SYM_REL);
    setupElement(SYM_MDC);
    setupElement(SYM_BUE);
    setupElement(SYM_DRE);
    setupElement(SYM_SBN);
    setupElement(SYM_HS1);
    setupElement(SYM_HS2);
    setupElement(SYM_SHO);
    setupElement(SYM_SHM);
    setupElement(SYM_SHU);
    setupElement(SYM_ADR);
    setupElement(SYM_BLD);
    /*external buttons*/
    setupElement(SYM_TAF);
    setupElement(SYM_TAU);
    setupElement(SYM_TAW);
    /*external empty color fields*/
    setupElement(SYM_FEG);
    setupElement(SYM_FEB);
    setupElement(SYM_FER);
    setupElement(SYM_FEY);
    setupElement(SYM_FEE);
    setupElement(SYM_FEN);

    coboIcon->setSizeLimit(4);  // only four elements visible in open combo Box
    coboIcon->setGeometry(100, 23, 80, EL_HEIGHT + 5);
    // show the right icon belonging to actual element
    coboIcon->setCurrentItem(listIcon->
                             find(listElementData->at(LIST_ID_ICON)));
    connect(coboIcon, SIGNAL(activated(int)), this,
            SLOT(slotSymbolChanged(int)));

    QLabel *label = new QLabel(coboIcon, tr("&Icon:"), frData);
    label->show();
    label->setGeometry(10, 33, 90, 15);
    //label->setBuddy( coboIcon );

    coboDecoder = new QComboBox(false, frData); // false = not editable
    coboDecoder->show();
    coboDecoder->setGeometry(100, 75, 140, 22);
    coboDecoder->insertItem("Maerklin k83 WD (M)");
    coboDecoder->insertItem("Maerklin k84 SD (M)");
    coboDecoder->insertItem("Viessm. 5211 WD (M)");
    coboDecoder->insertItem("Viessm. 5213 SD (M)");
    coboDecoder->insertItem("Viessmann 5210 (M)");      // Signalbaustein, extra Code!
    coboDecoder->insertItem("Littf. QS-DEC-II WD (M)");
    coboDecoder->insertItem("Littf. S-DEC-4 WD (M)");
    coboDecoder->insertItem("Littf. SA-DEC-4 SD (M)");
    coboDecoder->insertItem("Littf. M-DEC-MM WD (M)");
    coboDecoder->insertItem("Märklin Drehscheiben (M)");
    coboDecoder->insertItem("Littfinsky LS-DEC (M)");   // Signalbaustein, extra Code!
    coboDecoder->insertItem("EDiTS WD (M)");
    coboDecoder->insertItem("EDiTS SD (M)");
    coboDecoder->insertItem("Littf. S-DEC-4 WD (D)");
    coboDecoder->insertItem("Littf. SA-DEC-4 SD (D)");
    coboDecoder->insertItem("Littf. M-DEC-DC WD (D)");
    coboDecoder->insertItem("Littfinsky LS-DEC (D)");   // Signalbaustein, extra Code!
    coboDecoder->insertItem("Lenz LS 100 WD (D)");
    coboDecoder->insertItem("Lenz LS 110 WD (D)");
    coboDecoder->insertItem("Lenz LS 130 SD (D)");
    coboDecoder->insertItem("-1");
    coboDecoder->setFont((QFont) "Courier");    //serd

    labelDecoder = new QLabel(tr("&Decoder:"), frData);
    labelDecoder->show();
    labelDecoder->setGeometry(10, 78, 90, 15);
    labelDecoder->setBuddy(coboDecoder);

    connect(coboDecoder, SIGNAL(activated(int)),
            this, SLOT(slotDecoderChanged(int)));

    // setup the rotate widgets
    cbRotate = new QCheckBox(frData);
    cbRotate->show();
    cbRotate->move(100, 109);
    cbRotate->resize(25, 15);

    labelRotate = new QLabel(tr("&Rotation:"), frData);
    labelRotate->show();
    labelRotate->setGeometry(10, 108, 90, 15);
    labelRotate->setBuddy(cbRotate);

    // setup the LED off widgets
    cbLEDoff = new QCheckBox(frData);
    cbLEDoff->show();
    cbLEDoff->move(coboDecoder->x() + coboDecoder->width() - 15,
                   cbRotate->y());
    cbLEDoff->resize(25, 15);

    connect(cbLEDoff, SIGNAL(clicked()), this, SLOT(slotEnable_LED_FB()));

    labelLEDoff = new QLabel(tr("&LEDs off:"), frData);
    labelLEDoff->show();
    labelLEDoff->setGeometry(cbRotate->x() + cbRotate->width() + 35,
                             labelRotate->y(), 60, 15);
    labelLEDoff->setBuddy(cbLEDoff);

    // setup the invert widgets
    cbInvert = new QCheckBox(frData);
    cbInvert->show();
    cbInvert->move(100, 139);
    cbInvert->resize(25, 15);

    labelInvert = new QLabel(cbInvert, tr("&Inverted use:"), frData);
    labelInvert->setGeometry(10, 138, 90, 15);
    labelInvert->show();
    labelInvert->setBuddy(cbInvert);

    // setup the text widgets
    leText = new QLineEdit(frData, "text");
    leText->show();
    leText->setGeometry(100, 166, 80, 20);
    leText->setMaxLength(10);
    leText->setText(listElementData->at(LIST_ID_TEXT));

    labelText = new QLabel(tr("&Text:"), frData);
    labelText->show();
    labelText->setGeometry(10, 168, 90, 15);
    labelText->setBuddy(leText);

    QFrame *line = new QFrame(frData);
    line->show();
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    line->setGeometry(10, 190 /*leAddress_2->y()+24 */ ,
                      frData->width() - 20, 2);

    // setup the subtypes widgets
    bgSubType = new QButtonGroup("", frData);
    bgSubType->show();
    bgSubType->setGeometry(5, line->y() + 5, frData->width() - 10, 190);
    bgSubType->setFrameStyle(QFrame::NoFrame);  // buttongroup not visible
    bgSubType->setExclusive(true);

    labelSubTypeText = new QLabel("subType", bgSubType);
    labelSubTypeText->show();
    labelSubTypeText->setGeometry(5, 5, frData->width() - 20, 60);
    labelSubTypeText->setAlignment(AlignLeft | AlignTop | WordBreak);

    for (int i = 0; i < 3; i++) {
        buttSubType[i] = new QPushButton("", bgSubType);
        buttSubType[i]->move(frData->width() - 15 - 115,
                             labelSubTypeText->y() +
                             labelSubTypeText->height()
                             + i * 40 + 10);
        buttSubType[i]->resize(115, 30);
        buttSubType[i]->setToggleButton(true);  // on startup hide subtype butts
        buttSubType[i]->hide(); // only used by some elements
    }

    connect(bgSubType, SIGNAL(clicked(int)),
            this, SLOT(slotSubTypeClicked(int)));
}



void elementDialog::setupLogicFrame()
{
    frLogic = new QGroupBox(tr("Logic"), this, "logicGroupBox");
    frLogic->move(frData->x() + frData->width() + 10, frData->y());
    frLogic->resize(frData->width(), frData->height());
    frLogic->show();

    QButtonGroup *bgLogic = new QButtonGroup("", frLogic);
    bgLogic->setFrameStyle(QFrame::NoFrame);

    bgLogic->move(5, 20);
    bgLogic->resize(frData->width() - 10, 80);

    rbProtocol_MS = new QRadioButton("&Maerklin/Motorola", bgLogic);
    rbProtocol_MS->move(5, 15);
    rbProtocol_MS->resize(bgLogic->width() - 10, 20);
    rbProtocol_MS->setFocusPolicy(NoFocus);

    rbProtocol_NA = new QRadioButton("&NMRA/DCC", bgLogic);
    rbProtocol_NA->move(5, 35);
    rbProtocol_NA->resize(bgLogic->width() - 10, 20);
    rbProtocol_NA->setFocusPolicy(NoFocus);

    connect(bgLogic, SIGNAL(clicked(int)),
            this, SLOT(slotProtChanged(int)));

    QFrame *line = new QFrame(frLogic);
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    line->setGeometry(10, 118, frLogic->width() - 20, 2);

    // setup the address_1 widgets
    leAddress_1 = new QLineEdit(frLogic, "address_1");
    leAddress_1->setGeometry(100, line->y() + 8, 50, 20);
    leAddress_1->setMaxLength(4);       // address length of NA protocol
    leAddress_1->setText(listElementData->at(LIST_ID_ADDRESS_1));
    connect(leAddress_1, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotAddressChanged(const QString &)));

    labelAddress_1 = new QLabel(tr("Address &1:"), frLogic);
    labelAddress_1->setGeometry(10, line->y() + 10, 90, 15);
    labelAddress_1->setBuddy(leAddress_1);

    // setup the address_2 widgets
    leAddress_2 = new QLineEdit(frLogic, "address_2");
    leAddress_2->setGeometry(100, line->y() + 38, 50, 20);
    leAddress_2->setMaxLength(4);
    leAddress_2->setText(listElementData->at(LIST_ID_ADDRESS_2));
    connect(leAddress_2, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotAddressChanged(const QString &)));

    labelAddress_2 = new QLabel(tr("Address &2:"), frLogic);
    labelAddress_2->setGeometry(10, line->y() + 40, 90, 15);
    labelAddress_2->setBuddy(leAddress_2);

    cbChaConn1 = new QCheckBox(frLogic);
    cbChaConn1->move(230, line->y() + 10);
    cbChaConn1->resize(25, 15);

    labelChangeConn1 = new QLabel(tr("xch conn.:"), frLogic);
    labelChangeConn1->move(160, line->y() + 10);
    labelChangeConn1->resize(labelChangeConn1->sizeHint());
    labelChangeConn1->setBuddy(cbChaConn1);

    cbChaConn2 = new QCheckBox(frLogic);
    cbChaConn2->move(230, line->y() + 40);
    cbChaConn2->resize(25, 15);

    labelChangeConn2 = new QLabel(tr("xch conn.:"), frLogic);
    labelChangeConn2->move(160, line->y() + 40);
    labelChangeConn2->resize(labelChangeConn2->sizeHint());
    labelChangeConn2->setBuddy(cbChaConn2);

    line = new QFrame(frLogic);
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    line->setGeometry(10, leAddress_2->y() + 24, frLogic->width() - 20, 2);

    labelTime = new QLabel(tr("Reset after (ms):"), frLogic);
    labelTime->move(10, line->y() + 10);
    labelTime->resize(labelTime->sizeHint());

    sbActiveTime = new QSpinBox(50, 2000, 50, frLogic, "");     // 20 ms steps
    sbActiveTime->resize(50, 20);
    sbActiveTime->move(194, line->y() + 8);
    sbActiveTime->setWrapping(true);    // enables to spin "over" the limits

    line = new QFrame(frLogic);
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    line->setGeometry(10, sbActiveTime->y() + 24, frLogic->width() - 20,
                      2);

    labelFB = new QLabel(tr("Feedback (track LEDs only):"), frLogic);
    labelFB->move(10, line->y() + 10);
    labelFB->resize(labelFB->sizeHint());

    labelFBport = new QLabel(tr("Port:"), frLogic);
    labelFBport->move(20, line->y() + 35);
    labelFBport->resize(labelFBport->sizeHint());
    sbPort = new QSpinBox(0, 16 - FEEDBACK * 8, 1, frLogic, "");
    sbPort->resize(50, 20);
    sbPort->move(100, line->y() + 33);
    sbPort->setWrapping(true);  // enables to spin "over" the limits

    labelFBmodule = new QLabel(tr("Module:"), frLogic);
    labelFBmodule->move(20, line->y() + 65);
    labelFBmodule->resize(labelFBmodule->sizeHint());
    sbModule = new QSpinBox(1, 4 * (31 + FEEDBACK * 31), 1, frLogic, "");
    sbModule->resize(50, 20);
    sbModule->move(100, line->y() + 63);
    sbModule->setWrapping(true);        // enables to spin "over" the limits
    iPrevModNo = 1;
    connect(sbModule, SIGNAL(valueChanged(int)),
            this, SLOT(slotModuleChanged(int)));

    buttFBmodules = new QPushButton(tr("&FB"), frLogic);
    buttFBmodules->setPixmap(QPixmap(viewfeedback_xpm));
    buttFBmodules->resize(20, 20);
    buttFBmodules->move(sbModule->x() + sbModule->width() + 30,
                        sbModule->y());
    connect(buttFBmodules, SIGNAL(clicked()), this,
            SLOT(slotShowFBmodules()));
    if (SHOW_TOOLTIPS == true)
        QToolTip::add(buttFBmodules, tr("Show feedback module window"));

    labelBus = new QLabel(tr("Bus:"), frLogic);
    labelBus->setGeometry(20, line->y() + 95, 220, 15);

    sbBus = new QSpinBox(1, 4, 1, frLogic, "");
    sbBus->resize(50, 20);
    sbBus->move(100, line->y() + 93);
    sbBus->setWrapping(true);   // enables to spin "over" the limits
    iPrevBusNo = 1;
    connect(sbBus, SIGNAL(valueChanged(int)),
            this, SLOT(slotBusChanged(int)));

    QString sBusN_A = tr("(# ");
    for (int i = 0; i < 4; i++) {
        if (FB_MODULES_[i] == 0) {
            if (sBusN_A.length() != 3)
                sBusN_A.append(", ");
            QString s;
            sBusN_A.append(s.setNum(i + 1));
        }
    }
    if (sBusN_A.length() != 3)  // something has been added
    {
        sBusN_A.append(tr(" not available)"));
    }
    labelBus2 = new QLabel(sBusN_A, frLogic);
    labelBus2->move(20 + 30, line->y() + 125);
    labelBus2->resize(labelBus2->sizeHint());

    labelAdrMod = new QLabel(tr("Addr. module:"), frLogic);
    labelAdrMod->setGeometry(20, line->y() + 155, 110, 15);

    cbAdrMod = new QCheckBox(frLogic);
    cbAdrMod->setGeometry(137, line->y() + 156, 25, 15);
    cbAdrMod->setEnabled(false);
}



void elementDialog::setupElement(const char *cElementName_)
{
    // in this dialog we work with two different list: a QStrList which holds
    // all element names and a ComboBox which holds all elements graphically
    // thus it is not possible to determin the name of a QPixmap in a ComboBox
    // we use a trick: at the same index ID we have the icon in ComboBox and
    // it´s name in QStrList. To get a pixmap´s name we must recalculate the
    // name using the current item ID of the ComboBox
    listIcon->append(cElementName_);
    QString sPixmapName = RES_DIR_ELEM;
    sPixmapName += cElementName_;
    sPixmapName += XPM_SUFFIX;
    coboIcon->insertItem(QPixmap(sPixmapName));
}



void elementDialog::slotDecoderChanged(int)
{
    QString sText = coboDecoder->currentText();
    // autoset protocol type after choosing a decoder
    sText.right(3) ==
        "(M)" ? rbProtocol_MS->setChecked(true) : rbProtocol_NA->
        setChecked(true);
}



void elementDialog::slotProtChanged(int)
{
    QString sProt = (rbProtocol_MS->isChecked() == 1) ? "(M)" : "(D)";
    QString sText;

    for (int i = 0; i < coboDecoder->count(); i++) {
        sText = coboDecoder->text(i);
        if (sText.right(3) == sProt)    // find first decoder which
        {                       // support the chosen protocol
            coboDecoder->setCurrentItem(i);
            break;
        }
    }
}



void elementDialog::slotModuleChanged(int iModuleNo_)
{
    int iBusNo = ((iModuleNo_ - 1) / (31 + FEEDBACK * 31)) + 1;

    if (!bBlockBSignals) {
        bBlockMSignals = true;
        if (FB_MODULES_[iBusNo - 1] != 0) {
            sbBus->setValue(iBusNo);
            iPrevModNo = iModuleNo_;
        }
        else {
            iPrevModNo <
                iModuleNo_ ? sbModule->stepUp() : sbModule->stepDown();
            iPrevModNo =
                (iPrevModNo <
                 iModuleNo_) ? iModuleNo_ + 1 : iModuleNo_ - 1;
        }
        iPrevBusNo = sbBus->value();
        bBlockMSignals = false;
    }
}



void elementDialog::slotBusChanged(int iBusNo_)
{
    if (!bBlockMSignals) {
        bBlockBSignals = true;
        if (FB_MODULES_[iBusNo_ - 1] != 0) {
            sbModule->setValue((iBusNo_ - 1) * (31 + FEEDBACK * 31) + 1);
            iPrevBusNo = iBusNo_;
        }
        else {
            iPrevBusNo < iBusNo_ ? sbBus->stepUp() : sbBus->stepDown();
            iPrevBusNo =
                (iPrevBusNo < iBusNo_) ? iBusNo_ + 1 : iBusNo_ - 1;
        }
        iPrevBusNo = iBusNo_;
        bBlockBSignals = false;
    }
}



void elementDialog::slotShowFBmodules()
{
    emit sigShowFBmodules();
}



void elementDialog::slotSymbolChanged(int iCoboIconID)
{
    int enabled;
    QString sListText;

    // store the name of current pixmap in a special variable -> faster access
    sSoldIcon = listIcon->at(iCoboIconID);

    // show protocol data => element->hasAddress() or isSwitchable()
    enabled = sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS ||
        sSoldIcon == SYM_SS || sSoldIcon == SYM_WS ||
        sSoldIcon == SYM_SSH || sSoldIcon == SYM_SSS ||
        sSoldIcon == SYM_WEL || sSoldIcon == SYM_WER ||
        sSoldIcon == SYM_DWL || sSoldIcon == SYM_DWR ||
        sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR ||
        sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR ||
        sSoldIcon == SYM_DRW || 
        sSoldIcon == SYM_ENK || sSoldIcon == SYM_REL ||
        sSoldIcon == SYM_WEY || sSoldIcon == SYM_MDC ||
        sSoldIcon == SYM_SBN || sSoldIcon == SYM_BLD ||
        sSoldIcon == SYM_NRB || sSoldIcon == SYM_SRB ||
        sSoldIcon == SYM_ZP || sSoldIcon == SYM_VS;

    sListText = listElementData->at(LIST_ID_PROTOCOL);

    if (sListText == "M" || (DEF_PROTOCOL == PROT_MS && sListText != "N")
        || sSoldIcon == SYM_DRE)
        rbProtocol_MS->setChecked(true);
    if (sListText == "N" || (DEF_PROTOCOL == PROT_NA && sListText != "M"))
        rbProtocol_NA->setChecked(true);

    rbProtocol_MS->setEnabled(enabled || sSoldIcon == SYM_DRE);
    rbProtocol_NA->setEnabled(enabled);

    // show address_1 data, but take enabled value from above
    enabled = enabled && sSoldIcon != SYM_SBN && sSoldIcon != SYM_MDC;
    if (enabled) {
        sListText = listElementData->at(LIST_ID_DIRECTION);
        // set direction to 0 if it was -1 before and address_1 is now enabled
        if (sListText == "-1")
            listElementData->insert(LIST_ID_DIRECTION, "0");
        leAddress_1->setText(listElementData->at(LIST_ID_ADDRESS_1));
        sListText = listElementData->at(LIST_ID_CHACONN_1);
        cbChaConn1->setChecked(sListText == "1");
    }
    else
        leAddress_1->setText("-1");

    leAddress_1->setEnabled(enabled);
    labelAddress_1->setEnabled(enabled);
    cbChaConn1->setEnabled(enabled);
    labelChangeConn1->setEnabled(enabled);

    // show text data
    enabled = sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS ||
        sSoldIcon == SYM_WS || sSoldIcon == SYM_VS ||
        sSoldIcon == SYM_WEL || sSoldIcon == SYM_WER ||
        sSoldIcon == SYM_DWL || sSoldIcon == SYM_DWR ||
        sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR ||
        sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR ||
        sSoldIcon == SYM_DRW || sSoldIcon == SYM_SS ||
        sSoldIcon == SYM_SSH || sSoldIcon == SYM_SSS ||
        sSoldIcon == SYM_ENK || sSoldIcon == SYM_REL ||
        sSoldIcon == SYM_GER || sSoldIcon == SYM_LEE ||
        sSoldIcon == SYM_NRB || sSoldIcon == SYM_SRB ||
        sSoldIcon == SYM_WEY || sSoldIcon == SYM_MDC ||
        sSoldIcon == SYM_SBN || sSoldIcon == SYM_ADR ||
        sSoldIcon == SYM_BLD || sSoldIcon == SYM_ZP;

    if (enabled)
        leText->setText(listElementData->at(LIST_ID_TEXT));
    else
        leText->setText("-1");

    leText->setEnabled(enabled);
    labelText->setEnabled(enabled);

    // show address_2 data
    sListText = listElementData->at(LIST_ID_SUBTYPE);

    // Hp0+Hp1+Hp2
    enabled = sSoldIcon == SYM_HSS || sSoldIcon == SYM_DRW
        || sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR
        || sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR
        || sSoldIcon == SYM_MDC || sSoldIcon == SYM_DRE
        || sSoldIcon == SYM_SBN || (sSoldIcon == SYM_VS
                                    && sListText == "4")
        || (sSoldIcon == SYM_HS && sListText == "4");

    if (enabled) {
        leAddress_2->setText(listElementData->at(LIST_ID_ADDRESS_2));
        if (sSoldIcon != SYM_DRE && sSoldIcon != SYM_SBN) {
            sListText = listElementData->at(LIST_ID_CHACONN_2);
            cbChaConn2->setChecked(sListText == "1");
        }
    }
    else
        leAddress_2->setText("-1");

    leAddress_2->setEnabled(enabled);
    labelAddress_2->setEnabled(enabled);
    cbChaConn2->setEnabled(enabled && sSoldIcon != SYM_DRE
                           && sSoldIcon != SYM_SBN
                           && sSoldIcon != SYM_MDC);
    labelChangeConn2->setEnabled(enabled && sSoldIcon != SYM_DRE
                                 && sSoldIcon != SYM_SBN
                                 && sSoldIcon != SYM_MDC);

    if (sSoldIcon == SYM_DRE)
        leAddress_2->setFocusPolicy(NoFocus);
    else
        leAddress_2->setFocusPolicy(StrongFocus);

    // show rotate data
    enabled = sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS ||
        sSoldIcon == SYM_WS || sSoldIcon == SYM_VS ||
        sSoldIcon == SYM_WEL || sSoldIcon == SYM_WER ||
        sSoldIcon == SYM_DWL || sSoldIcon == SYM_DWR ||
        sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR ||
        sSoldIcon == SYM_KUL || sSoldIcon == SYM_KUR ||
        sSoldIcon == SYM_DRW || sSoldIcon == SYM_SS ||
        sSoldIcon == SYM_SSH || sSoldIcon == SYM_SSS ||
        sSoldIcon == SYM_PRE || sSoldIcon == SYM_RI1 ||
        sSoldIcon == SYM_GER || sSoldIcon == SYM_WEY ||
        sSoldIcon == SYM_NRB || sSoldIcon == SYM_SRB ||
        sSoldIcon == SYM_HS2 || sSoldIcon == SYM_DLT ||
        sSoldIcon == SYM_DRT || sSoldIcon == SYM_GET ||
        sSoldIcon == SYM_SHM || sSoldIcon == SYM_SHO ||
        sSoldIcon == SYM_SHU || sSoldIcon == SYM_ZP;
    // SYM_GER: only for text placement

    sListText = listElementData->at(LIST_ID_ROTATE);

    cbRotate->setChecked(sListText == "1");
    cbRotate->setEnabled(enabled);
    labelRotate->setEnabled(enabled);

    // show LEDoff data
    enabled = sSoldIcon == SYM_KUL || sSoldIcon == SYM_KUR ||
        sSoldIcon == SYM_DIL || sSoldIcon == SYM_DIR ||
        sSoldIcon == SYM_GER || sSoldIcon == SYM_RI1 ||
        sSoldIcon == SYM_NRB || sSoldIcon == SYM_SRB ||
        sSoldIcon == SYM_RI2 || sSoldIcon == SYM_KRH ||
        sSoldIcon == SYM_KRL || sSoldIcon == SYM_KRR;

    sListText = listElementData->at(LIST_ID_LEDOFF);

    cbLEDoff->setChecked(sListText == "1");
    cbLEDoff->setEnabled(enabled);
    labelLEDoff->setEnabled(enabled);

    // show invert data for empty elements only for colour
    enabled = sSoldIcon == SYM_WEL || sSoldIcon == SYM_WER
        || sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR
        || sSoldIcon == SYM_DWL || sSoldIcon == SYM_DWR
        || sSoldIcon == SYM_LEE;

    sListText = listElementData->at(LIST_ID_INVERT);

    cbInvert->setChecked(sListText == "1");
    cbInvert->setEnabled(enabled);
    labelInvert->setEnabled(enabled);
/*
    enabled = sSoldIcon == SYM_KUL || sSoldIcon == SYM_KUR ||
        sSoldIcon == SYM_DIL || sSoldIcon == SYM_DIR ||
        sSoldIcon == SYM_GER || sSoldIcon == SYM_RI1 ||
        sSoldIcon == SYM_RI2 || sSoldIcon == SYM_KRH ||
        sSoldIcon == SYM_KRL || sSoldIcon == SYM_KRR;
*/
    // show feedback data =>element->hasFBContact()
    enabled = sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS ||
        sSoldIcon == SYM_WS || sSoldIcon == SYM_VS ||
        sSoldIcon == SYM_WEL || sSoldIcon == SYM_WER ||
        sSoldIcon == SYM_DWL || sSoldIcon == SYM_DWR ||
        sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR ||
        sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR ||
        sSoldIcon == SYM_DRW || sSoldIcon == SYM_SS ||
        sSoldIcon == SYM_SSH || sSoldIcon == SYM_SSS ||
        sSoldIcon == SYM_ENK || sSoldIcon == SYM_WEY ||
        sSoldIcon == SYM_BUE || sSoldIcon == SYM_ADR ||
        sSoldIcon == SYM_BLD || sSoldIcon == SYM_ZP ||
        ((sSoldIcon == SYM_KUL || sSoldIcon == SYM_KUR ||
          sSoldIcon == SYM_DIL || sSoldIcon == SYM_DIR ||
          sSoldIcon == SYM_GER || sSoldIcon == SYM_RI1 ||
          sSoldIcon == SYM_NRB || sSoldIcon == SYM_SRB ||
          sSoldIcon == SYM_RI2 || sSoldIcon == SYM_KRH ||
          sSoldIcon == SYM_KRL || sSoldIcon == SYM_KRR)
         && !cbLEDoff->isChecked());

    sListText = listElementData->at(LIST_ID_FBPORT);
    labelFB->setEnabled(enabled);
    labelAdrMod->setEnabled(enabled);
    buttFBmodules->setEnabled(enabled);

    if (sSoldIcon == SYM_ADR) {
        cbAdrMod->setChecked(true);
        sbPort->setValue(1);
        sbPort->setEnabled(false);
    }
    else {
        cbAdrMod->setChecked(false);
        sbPort->setEnabled(true);
    }

    sbModule->setValue(sListText.toInt() / (16 - FEEDBACK * 8) + 1);
    sbModule->setEnabled(enabled);
    labelFBmodule->setEnabled(enabled);

    sbPort->setValue(sListText.toInt() % (16 - FEEDBACK * 8) + 1);
    sbPort->setEnabled(enabled);
    labelFBport->setEnabled(enabled);

    sbBus->setEnabled(enabled);
    labelBus->setEnabled(enabled);

    // show active time
    enabled = sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS ||
        sSoldIcon == SYM_WS || sSoldIcon == SYM_VS ||
        sSoldIcon == SYM_WEL || sSoldIcon == SYM_WER ||
        sSoldIcon == SYM_DWL || sSoldIcon == SYM_DWR ||
        sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR ||
        sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR ||
        sSoldIcon == SYM_DRW || sSoldIcon == SYM_SS ||
        sSoldIcon == SYM_SSH || sSoldIcon == SYM_SSS ||
        sSoldIcon == SYM_ENK || sSoldIcon == SYM_REL ||
        sSoldIcon == SYM_WEY || sSoldIcon == SYM_DRE ||
        sSoldIcon == SYM_SBN || sSoldIcon == SYM_MDC ||
        sSoldIcon == SYM_BLD || sSoldIcon == SYM_ZP;

    sListText = listElementData->at(LIST_ID_ACTTIME);
    if (enabled) {
        if (sListText != "-1")
            sbActiveTime->setValue(sListText.toInt());
        else
            sbActiveTime->setValue(ACTIVE_TIME);
    }
    sbActiveTime->setEnabled(enabled);
    labelTime->setEnabled(enabled);

    // show decoder data
    enabled = sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS ||
        sSoldIcon == SYM_WS || sSoldIcon == SYM_VS ||
        sSoldIcon == SYM_WEL || sSoldIcon == SYM_WER ||
        sSoldIcon == SYM_DWL || sSoldIcon == SYM_DWR ||
        sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR ||
        sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR ||
        sSoldIcon == SYM_DRW || sSoldIcon == SYM_SS ||
        sSoldIcon == SYM_SSH || sSoldIcon == SYM_SSS ||
        sSoldIcon == SYM_ENK || sSoldIcon == SYM_REL ||
        sSoldIcon == SYM_WEY ||
        sSoldIcon == SYM_SBN || sSoldIcon == SYM_MDC ||
        sSoldIcon == SYM_BLD || sSoldIcon == SYM_ZP;

    if (!enabled && sSoldIcon != SYM_DRE)
        coboDecoder->setCurrentItem(coboDecoder->count() - 1);  // == -1
    else if (!enabled && sSoldIcon == SYM_DRE)
        coboDecoder->setCurrentItem(8); // Maerklin special turntable decoder
    else {
        sListText = listElementData->at(LIST_ID_DECODER);
        if (sListText == "-1")
            sListText = DEF_DECODER;

        for (int i = 0; i < coboDecoder->count(); i++) {
            if (sListText == coboDecoder->text(i)) {
                coboDecoder->setCurrentItem(i);
                break;
            }
        }
    }
    coboDecoder->setEnabled(enabled);
    labelDecoder->setEnabled(enabled);

    // show subtype data
    enabled = sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS ||
        sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR ||
        sSoldIcon == SYM_ENK || sSoldIcon == SYM_DRE ||
        sSoldIcon == SYM_VS;

    if (!enabled)
        listElementData->insert(LIST_ID_SUBTYPE, "-1");
    showSubTypes(enabled);
}



void elementDialog::slotEnable_LED_FB()
{
    labelFB->setEnabled(!cbLEDoff->isChecked());
    labelAdrMod->setEnabled(!cbLEDoff->isChecked());
    buttFBmodules->setEnabled(!cbLEDoff->isChecked());

    sbModule->setEnabled(!cbLEDoff->isChecked());
    labelFBmodule->setEnabled(!cbLEDoff->isChecked());

    sbPort->setEnabled(!cbLEDoff->isChecked());
    labelFBport->setEnabled(!cbLEDoff->isChecked());

    sbBus->setEnabled(!cbLEDoff->isChecked());
    labelBus->setEnabled(!cbLEDoff->isChecked());
}



void elementDialog::showSubTypes(int iShow_)
{
    int i;

    // remove text field and hide buttons
    if (!iShow_) {
        labelSubTypeText->hide();
        for (i = 0; i < 3; i++) {
            QToolTip::remove(buttSubType[i]);
            buttSubType[i]->hide();
        }
        return;
    }


    buttSubType[0]->show();
    buttSubType[1]->show();
    buttSubType[2]->show();
    // show appropriate text ...
    if (sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS || sSoldIcon ==
            SYM_VS) {
        labelSubTypeText->setText(tr
                ("Please choose the button which represents "
                 "the available signal states:"));
        if (sSoldIcon == SYM_HS){
            buttSubType[0]->setPixmap(QPixmap(signal_hs_st1_xpm));
            buttSubType[1]->setPixmap(QPixmap(signal_hs_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(signal_hs_st3_xpm));
        }
        if (sSoldIcon == SYM_HSS){
            buttSubType[0]->setPixmap(QPixmap(signal_hss_st1_xpm));
            buttSubType[1]->setPixmap(QPixmap(signal_hss_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(signal_hss_st3_xpm));
        }
        if (sSoldIcon == SYM_VS){
            buttSubType[0]->setPixmap(QPixmap(signal_vs_st1_xpm));
            buttSubType[1]->setPixmap(QPixmap(signal_vs_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(signal_vs_st3_xpm));
        }
    }
    
    else if (sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR) {
        labelSubTypeText->setText(tr
                ("Please choose the button which represents "
                 "the available crossing states:"));
        if (sSoldIcon == SYM_DKL){
            buttSubType[1]->setPixmap(QPixmap(dkw_links_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(dkw_links_st3_xpm));
        }
        if (sSoldIcon == SYM_DKR){
            buttSubType[1]->setPixmap(QPixmap(dkw_rechts_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(dkw_rechts_st3_xpm));
        }
        buttSubType[0]->hide();
    }
    else if (sSoldIcon == SYM_ENK) {
        labelSubTypeText->setText(tr
                ("Please choose the button which represents "
                 "the used coupler:"));
        buttSubType[0]->setPixmap(QPixmap(entkoppler_st1_xpm));
        buttSubType[1]->setPixmap(QPixmap(entkoppler_st2_xpm));
        buttSubType[2]->setPixmap(QPixmap(entkoppler_st3_xpm));
    }
    else if (sSoldIcon == SYM_DRE) {
        labelSubTypeText->setText(tr
                ("Please choose the button which represents "
                 "the turntable base address:"));
        buttSubType[0]->hide();
        buttSubType[1]->setPixmap(QPixmap(drehscheibe_st2_xpm));
        buttSubType[2]->setPixmap(QPixmap(drehscheibe_st3_xpm));
    }
    labelSubTypeText->show();



/*
    // ... and the appropriate icon on buttons
    for (i = 0; i < 3; i++) {
        QString sSuffix, sPixmapName;

        sPixmapName = RES_DIR_APPL + sSoldIcon;

        sSuffix.sprintf("_st%d%s", i + 1, XPM_SUFFIX);
        // Hier steckt ein Fehler, der bei DKL, DKR und DRE einen leeren
        // ersten Knopf erzeugt 
        buttSubType[i]->setPixmap(QPixmap(sPixmapName + sSuffix));
        if (i != 0 ||
            (sSoldIcon != SYM_DKL && sSoldIcon != SYM_DKR
             && sSoldIcon != SYM_DRE))
            buttSubType[i]->show();     // no button 0 for DKW and turntable
        // Schalter verstecken bei DKL, DKR und DRE 
        else
            buttSubType[i]->hide();
    }
*/

    // activate the subtype dependant button
    QString sListText = listElementData->at(LIST_ID_SUBTYPE);

    if (sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS || sSoldIcon == SYM_VS) {
        if (sSoldIcon == SYM_HS && SHOW_TOOLTIPS == true) {
            QToolTip::add(buttSubType[0],
                          tr("Allows to switch this signal to:\n"
                             "Hp0, Hp1"));
            QToolTip::add(buttSubType[1],
                          tr("Allows to switch this signal to:\n"
                             "Hp0 and Hp2"));
            QToolTip::add(buttSubType[2],
                          tr("Allows to switch this signal to:\n"
                             "Hp0, Hp1 and Hp2"));
        }
        if (sSoldIcon == SYM_HSS && SHOW_TOOLTIPS == true) {
            QToolTip::add(buttSubType[0],
                          tr("Allows to switch this signal to:\n"
                             "Hp0, Hp1 and Sh1"));
            QToolTip::add(buttSubType[1],
                          tr("Allows to switch this signal to:\n"
                             "Hp0, Hp2 and Sh1"));
            QToolTip::add(buttSubType[2],
                          tr("Allows to switch this signal to:\n"
                             "Hp0, Hp1, Hp2 and Sh1"));
        }
        if (sSoldIcon == SYM_VS && SHOW_TOOLTIPS == true) {
            QToolTip::add(buttSubType[0],
                          tr("Allows to switch this signal to:\n"
                             "Vr0, Vr1"));
            QToolTip::add(buttSubType[1],
                          tr("Allows to switch this signal to:\n"
                             "Vr0 and Vr2"));
            QToolTip::add(buttSubType[2],
                          tr("Allows to switch this signal to:\n"
                             "Vr0, Vr1 and Vr2"));
        }

        switch (sListText.toInt()) {
        default:               // on new creation
        case 0:                // Hp0+Hp1
        case 1:
            buttSubType[0]->setOn(true);        // Hp0+Hp1+Sh1
            slotSubTypeClicked(0);
            break;
        case 6:                // Hp0+Hp2
        case 7:
            buttSubType[1]->setOn(true);        // Hp0+Hp2+Sh1
            slotSubTypeClicked(1);
            break;
        case 4:                // Hp0+Hp1+Hp2
        case 5:
            buttSubType[2]->setOn(true);        // Hp0+Hp1+Hp2+Sh1
            slotSubTypeClicked(2);
            break;
        }
    }

    if (sSoldIcon == SYM_ENK) {
        if (SHOW_TOOLTIPS == true) {
            QToolTip::add(buttSubType[0],
                          tr("Allows to use a:\nbistable coupler"));
            QToolTip::add(buttSubType[1], tr("Allows to use a:\n"
                                             "momentary coupler\non left connector"));
            QToolTip::add(buttSubType[2], tr("Allows to use a:\n"
                                             "momentary coupler\non right connector"));
        }
        switch (sListText.toInt()) {
        default:               // on new creation
        case -1:
            buttSubType[0]->setOn(true);        // bistable coupler
            slotSubTypeClicked(0);
            break;
        case 0:
            buttSubType[1]->setOn(true);        // momentary coupler
            slotSubTypeClicked(1);      // left connector
            break;
        case 1:
            buttSubType[2]->setOn(true);        // momentary coupler
            slotSubTypeClicked(2);      // right connector
            break;
        }
    }

    if (sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR) {
        if (SHOW_TOOLTIPS == true) {
            QToolTip::add(buttSubType[1],
                          tr("Allows to use a:\n"
                             "2 state double turnout\n(f.e. Maerklin 2264)"
                             "\nDOES NOT WORK YET!"));
            QToolTip::add(buttSubType[2],
                          tr("Allows to use a:\n"
                             "4 state double turnout\n"
                             "(f.e. Maerklin 2275,\nall Roco´s)"));
        }
        switch (sListText.toInt()) {
        case 0:
            buttSubType[1]->setOn(true);        // 2 states possible/Maerklin
            slotSubTypeClicked(1);
            break;
        default:               // on new creation
        case 1:
            buttSubType[2]->setOn(true);        // 4 states possible
            slotSubTypeClicked(2);
            break;
        }
    }

    if (sSoldIcon == SYM_DRE) {
        if (SHOW_TOOLTIPS == true) {
            sListText = listElementData->at(LIST_ID_ADDRESS_2);
            QToolTip::add(buttSubType[1], tr("Default turntable:\n"
                                             "Controlled via keyboard #15"));
            QToolTip::add(buttSubType[2], tr("Extra turntable:\n"
                                             "Controlled via keyboard #14"));
        }
        switch (sListText.toInt() / 16) {
        default:               // default turntable
        case 15:
            buttSubType[1]->setOn(true);
            slotSubTypeClicked(1);
            break;              // extra or second turntable
        case 14:
            buttSubType[2]->setOn(true);
            slotSubTypeClicked(2);
            break;
        }
    }
}



void elementDialog::slotSubTypeClicked(int iSubTypeID_)
{
    switch (iSubTypeID_) {
    case 0:                    // == subType 1
        if (sSoldIcon == SYM_HS || sSoldIcon == SYM_VS) {
            leAddress_2->setEnabled(false);
            labelAddress_2->setEnabled(false);
            cbChaConn2->setEnabled(false);
            labelChangeConn2->setEnabled(false);

            leAddress_2->setText("-1");
            listElementData->insert(LIST_ID_SUBTYPE, "0");
        }

        if (sSoldIcon == SYM_HSS)
            listElementData->insert(LIST_ID_SUBTYPE, "1");

        if (sSoldIcon == SYM_ENK) {
            leAddress_2->setEnabled(false);
            labelAddress_2->setEnabled(false);
            cbChaConn2->setEnabled(false);
            labelChangeConn2->setEnabled(false);

            listElementData->insert(LIST_ID_SUBTYPE, "-1");
        }
        break;

    case 1:                    // == subType 2
        if (sSoldIcon == SYM_HS || sSoldIcon == SYM_VS) {
            leAddress_2->setEnabled(false);
            labelAddress_2->setEnabled(false);
            cbChaConn2->setEnabled(false);
            labelChangeConn2->setEnabled(false);
            leAddress_2->setText("-1");
            listElementData->insert(LIST_ID_SUBTYPE, "6");
        }

        if (sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR) {
            leAddress_2->setEnabled(false);
            labelAddress_2->setEnabled(false);
            cbChaConn2->setEnabled(false);
            labelChangeConn2->setEnabled(false);

            listElementData->insert(LIST_ID_SUBTYPE, "0");
        }

        if (sSoldIcon == SYM_HSS)
            listElementData->insert(LIST_ID_SUBTYPE, "7");

        if (sSoldIcon == SYM_ENK)
            listElementData->insert(LIST_ID_SUBTYPE, "0");

        if (sSoldIcon == SYM_DRE)
            leAddress_2->setText("240");

        break;

    case 2:                    // == subType 3
        if (sSoldIcon == SYM_HS || sSoldIcon == SYM_VS) {
            leAddress_2->setEnabled(true);
            labelAddress_2->setEnabled(true);
            cbChaConn2->setEnabled(true);
            labelChangeConn2->setEnabled(true);
            listElementData->insert(LIST_ID_SUBTYPE, "4");
        }

        if (sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR) {
            leAddress_2->setEnabled(true);
            labelAddress_2->setEnabled(true);
            cbChaConn2->setEnabled(true);
            labelChangeConn2->setEnabled(true);
            listElementData->insert(LIST_ID_SUBTYPE, "1");
        }

        if (sSoldIcon == SYM_HSS)
            listElementData->insert(LIST_ID_SUBTYPE, "5");

        if (sSoldIcon == SYM_ENK)
            listElementData->insert(LIST_ID_SUBTYPE, "1");

        if (sSoldIcon == SYM_DRE)
            leAddress_2->setText("224");

        break;
    }
}



void elementDialog::slotAddressChanged(const QString & cNewAddress_)
{
    QString sCorrection = cNewAddress_;

    // a zero length and "-1" value are o.k.
    if (sCorrection.length() == 0 || sCorrection == "-1")
        return;

    // now reject character input if it was not a digit
    for (uint i = 0; i < sCorrection.length(); i++) {

        if (isdigit(cNewAddress_[i]) == false) {
            sCorrection.replace(i, 1, '\0');    // correction code replaces
            if (leAddress_1->text() == cNewAddress_)    // the wrong user entry in
            {                   // line edits
                leAddress_1->setText(sCorrection);
                leAddress_1->setCursorPosition(i);
            }
            else {
                leAddress_2->setText(sCorrection);
                leAddress_2->setCursorPosition(i);
            }
            break;
        }
    }

    if (leAddress_1->text() == cNewAddress_)
        leText->setText(sCorrection);
    // if user enters new address but does not
    // change the text field (where the old
    // address is still visible) the element
    // shows the old address, and the user is
    // confused
}



void elementDialog::slotApplyPressed()
{
    // make sure that the address have leading "0"s if shorter than protocol
    // determined and check for protocol limits
    if (checkAddressLimits(leAddress_1->text()) == INVALID ||
        checkAddressLimits(leAddress_2->text()) == INVALID)
        return;

    // now read all widget data and store them in a new QStrList
    listNewData = new QStrList(true);

    listNewData->insert(LIST_ID_INDEX, listElementData->at(LIST_ID_INDEX));
    listNewData->insert(LIST_ID_ICON,
                        listIcon->at(coboIcon->currentItem()));

    listNewData->insert(LIST_ID_ROTATE, cbRotate->isEnabled()?
                        (cbRotate->isChecked()? "1" : "0") : "-1");

    listNewData->insert(LIST_ID_INVERT, cbInvert->isEnabled()?
                        (cbInvert->isChecked()? "1" : "0") : "-1");

    listNewData->insert(LIST_ID_DECODER, coboDecoder->currentText());

    listNewData->insert(LIST_ID_PROTOCOL, rbProtocol_MS->isEnabled()?
                        (rbProtocol_MS->isChecked()? "M" : "N") : "-1");

    listNewData->insert(LIST_ID_ADDRESS_1, leAddress_1->text());
    listNewData->insert(LIST_ID_ADDRESS_2, leAddress_2->text());

    listNewData->insert(LIST_ID_CHACONN_1, cbChaConn1->isEnabled()?
                        (cbChaConn1->isChecked()? "1" : "0") : "-1");
    listNewData->insert(LIST_ID_CHACONN_2, cbChaConn2->isEnabled()?
                        (cbChaConn2->isChecked()? "1" : "0") : "-1");

    listNewData->insert(LIST_ID_DIRECTION, leAddress_1->isEnabled()?
                        listElementData->at(LIST_ID_DIRECTION) : "-1");

    listNewData->insert(LIST_ID_SUBTYPE,
                        listElementData->at(LIST_ID_SUBTYPE));

    /* if field contains '-1' then show address*/
    listNewData->insert(LIST_ID_TEXT,
                        /*(leText->text() != "" && leText->text() != "-1")
                         ? leText->text() : leAddress_1->text());*/
                        (leText->text() == "-1")
                         ? leAddress_1->text() : leText->text());

    listNewData->insert(LIST_ID_ACTTIME, sbActiveTime->isEnabled()
            ? sbActiveTime->text() : (QString) "-1");

    QString sPort;
    listNewData->insert(LIST_ID_FBPORT, labelFB->isEnabled()?
                        (sPort.
                         setNum((sbModule->value() - 1) * (16 -
                                                           FEEDBACK * 8) +
                                (sbPort->value() - 1)).data()) : "-1");

    listNewData->insert(LIST_ID_LEDOFF, cbLEDoff->isEnabled()?
                        (cbLEDoff->isChecked()? "1" : "0") : "-1");

    //listNewData->insert( LIST_ID_DATA_2, "-1" );
    //listNewData->insert( LIST_ID_DATA_3, "-1" );

    emit ApplyPressed();        // send signal to element to copy new data
}



int elementDialog::checkAddressLimits(QString sAddressToCheck_)
{
    if (sAddressToCheck_ == "-1")
        return VALID;
    // function is also called if no address has been
    // entered other negative values than -1 are not
    // possible cause you can't enter the "-" sign!
    
    QString SymName = QString(listIcon->at(coboIcon->currentItem()));
    
    /* for this symbols also dummy numbers are allowed*/ 
    if (SymName == SYM_NRB || SymName == SYM_SRB)
        return VALID;
    
    int chkAddress = sAddressToCheck_.toInt();

    if ((chkAddress > 324 && rbProtocol_MS->isChecked()) ||
        (chkAddress > 4096 && rbProtocol_NA->isChecked())) {
        qApp->beep();
        QMessageBox::warning(this, tr("Wrong address(es)"),
                             tr("Entered address(es) exceed\n"
                                "the protocol limits.\n\n"
                                "Valid ranges are:\n"
                                "Maerklin/Motorola: 1 -  324\n"
                                "NMRA/DCC:          1 - 4096"),
                             tr("&OK"), 0, 0, 0, 0);
        // butt 1: OK, butt 2+3: not avail.
        // <ENTER> + <ESC> default to butt 0 = OK
        return INVALID;
    }

    return VALID;               // everything else is valid
}
