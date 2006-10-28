/***************************************************************************
                           elementdialog.cpp
                           version 0.4.8 $Revision: 1.18 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2006 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2006-10-28 07:06:42 $
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
 this file provides an user interface to change properties of an element
 ***************************************************************************/

#include <qhbox.h>
#include <qlayout.h>

#include "elementdialog.h"
#include "element.h"
#include "preferences.h"

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

#define MINMMPORT 0
#define MAXMMPORT 1
#define MINDCCPORT 0
#define MAXDCCPORT 1
#define MINSVPORT 0
#define MAXSVPORT 64535 // no limits
#define MINSXPORT 1
#define MAXSXPORT 8

elementDialog::elementDialog(QWidget* parent, int idx):
    QDialog(parent, "elementDialog", true)
{
    setCaption(tr("Properties of Element #%1").arg(idx));
    gaSubType = 0;
    gaDirection = 0;
    addresscount = 0;

    /*Layout to separate OK Cancel buttons from the upper rest*/
    QBoxLayout* baseLayout = new QVBoxLayout(this, 12, 12);

    /*Layout to separate left and right groupboxest*/
    QBoxLayout* leftRightLayout = new QHBoxLayout(baseLayout, 12);

    /*Layout to separate left column verticaly*/
    QBoxLayout* leftColumnLayout = new QVBoxLayout(leftRightLayout, 6);

    /*Layout to separate right column verticaly*/
    QBoxLayout* rightColumnLayout = new QVBoxLayout(leftRightLayout, 6);


    /*left column*/
    QGroupBox* frData = new QGroupBox(0, Horizontal, tr("Data"), this,
            "dataGroupBox");
    leftColumnLayout->addWidget(frData);
    QVBoxLayout* rfDataGBLayout = new QVBoxLayout(frData->layout(), 6);

    /*line with symbol group box*/
    QHBoxLayout* symbolLayout = new QHBoxLayout(rfDataGBLayout, 6);

    QLabel *label = new QLabel(tr("I&con:"), frData);
    symbolLayout->addWidget(label);
    QSpacerItem* spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    symbolLayout->addItem(spacer);

    /*container for icon names, unvisible */
    IconNameList = new QStrList(true);
    /*container for shown icons */
    IconComboBox = new QComboBox(false, frData);    // false = not editable
    symbolLayout->addWidget(IconComboBox);

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
    setupElement(SYM_BUE);
    setupElement(SYM_ADR);
    setupElement(SYM_BLD);
    setupElement(SYM_DRE);
    setupElement(SYM_SBN);
    setupElement(SYM_REL);
    setupElement(SYM_MDC);
    setupElement(SYM_HS1);
    setupElement(SYM_HS2);
    setupElement(SYM_SHO);
    setupElement(SYM_SHM);
    setupElement(SYM_SHU);
    
    /*external buttons*/
    setupElement(SYM_TAF);
    setupElement(SYM_TAU);
    setupElement(SYM_TAW);
    setupElement(SYM_TAS);

    /*external empty color panels*/
    setupElement(SYM_FEG);
    setupElement(SYM_FEB);
    setupElement(SYM_FER);
    setupElement(SYM_FEY);
    setupElement(SYM_FEE);
    setupElement(SYM_FEN);

    // only four elements visible in open combo Box
    IconComboBox->setSizeLimit(4);
    IconComboBox->setMinimumHeight(EL_HEIGHT + 5);
    connect(IconComboBox, SIGNAL(activated(int)), this,
            SLOT(slotSymbolChanged(int)));
    label->setBuddy(IconComboBox);

    /*line with text edit line*/
    QHBoxLayout* textLayout = new QHBoxLayout(rfDataGBLayout);
    labelText = new QLabel(tr("&Text:"), frData);
    textLayout->addWidget(labelText);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    textLayout->addItem(spacer);
    leText = new QLineEdit(frData, "text");
    leText->setMaxLength(10);
    leText->setMaximumWidth(IconComboBox->width() - 13);
    textLayout->addWidget(leText);
    labelText->setBuddy(leText);

    /* line with address to text checkbox*/
    cbAddrLabeling = new QCheckBox(tr("Address for la&beling"), frData,
            "addressLabelingCB");
    rfDataGBLayout->addWidget(cbAddrLabeling);
    connect(cbAddrLabeling, SIGNAL(toggled(bool)), this,
            SLOT(letteringChanged(bool))); 

    /* line with rotate checkbox*/
    cbRotate = new QCheckBox(tr("&Rotation"), frData, "rotateCB");
    rfDataGBLayout->addWidget(cbRotate);

    /* line with invert checkbox*/
    cbInvert = new QCheckBox(tr("&Inverted use"), frData, "invertCB");
    rfDataGBLayout->addWidget(cbInvert);

    /* spacer to shift lines above to top*/
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    rfDataGBLayout->addItem(spacer);

    /*group box for symbol variants*/
    QGroupBox* variantGB = new QGroupBox(0, Horizontal,
            tr("Symbol variants"), this, "variantsGB");
    leftColumnLayout->addWidget(variantGB);
    QVBoxLayout* variantGBLayout = new
        QVBoxLayout(variantGB->layout(), 6);

    /*group of three buttons*/
    bgSubType = new QButtonGroup(0, Horizontal, "", variantGB, "bgSubType");
    variantGBLayout->addWidget(bgSubType);
    QVBoxLayout* subtypeBGL = new QVBoxLayout(bgSubType->layout(), 6);
    bgSubType->setFrameStyle(QFrame::NoFrame);  // buttongroup not visible
    bgSubType->setExclusive(true);

    // on startup hide subtype buttons, they are only used by some elements
    for (int i = 0; i < 3; i++) {
        buttSubType[i] = new QPushButton(bgSubType, "buttSubType");
        buttSubType[i]->setPixmap(QPixmap(signal_hss_st1_xpm));
        subtypeBGL->addWidget(buttSubType[i]); 
        buttSubType[i]->setToggleButton(true);
        buttSubType[i]->hide();
    }

    connect(bgSubType, SIGNAL(clicked(int)),
            this, SLOT(slotSubTypeClicked(int)));

    /*groupbox with protocol data*/
    protocolBG = new QButtonGroup(4, Vertical,
                        tr("Protocol"), this, "protocolBG");
    //rightColumnLayout->addWidget(protocolBG);
    leftColumnLayout->addWidget(protocolBG);
    protocolBG->setExclusive(true);
    rbProtocol_MS = new QRadioButton("&Maerklin/Motorola", protocolBG);
    rbProtocol_NA = new QRadioButton("&NMRA/DCC", protocolBG);
    rbProtocol_PS = new QRadioButton("Protocol by Ser&ver", protocolBG);
    rbProtocol_SE = new QRadioButton("Selectri&x", protocolBG);
    connect(protocolBG, SIGNAL(clicked(int)),
            this, SLOT(slotProtocolChanged(int)));
    
    /*right side with logic data*/

    /*decoder data group box*/
    QGroupBox* decoderGB = new QGroupBox(0, Horizontal,
            tr("Decoder"), this, "decoderGB");
    rightColumnLayout->addWidget(decoderGB);
    QVBoxLayout* decoderGBL = new QVBoxLayout(decoderGB->layout(), 6);

    /*line with decoder combobox*/
    QHBoxLayout* decoderLayout = new QHBoxLayout(decoderGBL, 6);
    labelDecoder = new QLabel(tr("T&ype:"), decoderGB);
    decoderLayout->addWidget(labelDecoder);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    decoderLayout->addItem(spacer);

    coboDecoder = new QComboBox(false, decoderGB);
    coboDecoder->setGeometry(100, 75, 140, 22);
    coboDecoder->insertItem("Maerklin k83 WD (M)");
    coboDecoder->insertItem("Maerklin k84 SD (M)");
    coboDecoder->insertItem("Viessm. 5211 WD (M)");
    coboDecoder->insertItem("Viessm. 5213 SD (M)");
    // Signalbaustein, extra Code!
    // coboDecoder->insertItem("Viessmann 5210 (M)");
    coboDecoder->insertItem("Littf. QS-DEC-II WD (M)");
    coboDecoder->insertItem("Littf. S-DEC-4 WD (M)");
    coboDecoder->insertItem("Littf. SA-DEC-4 SD (M)");
    coboDecoder->insertItem("Littf. M-DEC-MM WD (M)");
    coboDecoder->insertItem("Märklin Drehscheiben (M)");
    // Signalbaustein, extra Code!
    // coboDecoder->insertItem("Littfinsky LS-DEC (M)");
    coboDecoder->insertItem("EDiTS WD (M)");
    coboDecoder->insertItem("EDiTS SD (M)");
    coboDecoder->insertItem("Littf. S-DEC-4 WD (D)");
    coboDecoder->insertItem("Littf. SA-DEC-4 SD (D)");
    coboDecoder->insertItem("Littf. M-DEC-DC WD (D)");
    // Signalbaustein, extra Code!
    // coboDecoder->insertItem("Littfinsky LS-DEC (D)");
    coboDecoder->insertItem("Lenz LS 100 WD (D)");
    coboDecoder->insertItem("Lenz LS 110 WD (D)");
    coboDecoder->insertItem("Lenz LS 130 SD (D)");
    coboDecoder->insertItem("Generic Decoder (P)");
    coboDecoder->insertItem("Generic Decoder (S)");
    coboDecoder->insertItem("-1");
    decoderLayout->addWidget(coboDecoder);

    labelDecoder->setBuddy(coboDecoder);

    connect(coboDecoder, SIGNAL(activated(int)),
            this, SLOT(slotDecoderChanged(int)));
    
    /*line with resettime */
    QHBoxLayout* resetLayout = new QHBoxLayout(decoderGBL, 6);
    labelTime = new QLabel(tr("Reset &after (ms):"), decoderGB);
    resetLayout->addWidget(labelTime);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    resetLayout->addItem(spacer);

    activeTimeSB = new QSpinBox(50, 2000, 50, decoderGB, "");
    activeTimeSB->setWrapping(true);
    resetLayout->addWidget(activeTimeSB);
    labelTime->setBuddy(activeTimeSB);

    QGridLayout* decdataLayout = new QGridLayout(decoderGBL, 6, 3, 10,
            "decdataLayout");
    
    /*line with srcp bus 1 */
    srcpBus1Label = new QLabel(tr("S&RCP-Bus 1:"), decoderGB);
    decdataLayout->addWidget(srcpBus1Label, 0, 0);
    srcpBus1LE = new QLineEdit(decoderGB, "srcpBus1LE");
    srcpBus1LE->setMaxLength(4);
    srcpBus1LE->setMaximumWidth(LEMAXWIDTH);
    decdataLayout->addWidget(srcpBus1LE, 0, 1);
    srcpBus1Label->setBuddy(srcpBus1LE);
    
    /*line with address 1 */
    labelAddress_1 = new QLabel(tr("Address &1:"), decoderGB);
    decdataLayout->addWidget(labelAddress_1, 1, 0);
    leAddress_1 = new QLineEdit(decoderGB, "address_1");
    decdataLayout->addWidget(leAddress_1, 1, 1);
    leAddress_1->setMaxLength(4);       // address length of NA protocol
    leAddress_1->setMaximumWidth(LEMAXWIDTH);
    connect(leAddress_1, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotAddress1Changed(const QString&)));
    a1Validator = new QIntValidator(-1, MAX_GADCC, this);
    leAddress_1->setValidator(a1Validator);
    labelAddress_1->setBuddy(leAddress_1);

    /*line with port 1 spinbox */
    port1Label = new QLabel(tr("&Port 1:"), decoderGB);
    decdataLayout->addWidget(port1Label, 2, 0);

    port1SB = new QSpinBox(0, 1, 1, decoderGB, "port1SB");
    port1SB->setWrapping(true);
    decdataLayout->addWidget(port1SB, 2, 1);
    port1Label->setBuddy(port1SB);

    xchConn1CB = new QCheckBox(tr("&Exch. conn."), decoderGB, "xch1");
    connect(xchConn1CB, SIGNAL(toggled(bool)), this,
            SLOT(xchConn1IsToggled(bool)));
    decdataLayout->addWidget(xchConn1CB, 2, 2);

    /*line with srcp bus 2 */
    srcpBus2Label = new QLabel(tr("SR&CP-Bus 2:"), decoderGB);
    decdataLayout->addWidget(srcpBus2Label, 3, 0);
    srcpBus2LE = new QLineEdit(decoderGB, "srcpBus2LE");
    srcpBus2LE->setMaxLength(4);
    srcpBus2LE->setMaximumWidth(LEMAXWIDTH);
    decdataLayout->addWidget(srcpBus2LE, 3, 1);
    srcpBus2Label->setBuddy(srcpBus2LE);
    
    /*line with address 2 */
    labelAddress_2 = new QLabel(tr("Address &2:"), decoderGB);
    decdataLayout->addWidget(labelAddress_2, 4, 0);
    leAddress_2 = new QLineEdit(decoderGB, "address_2");
    decdataLayout->addWidget(leAddress_2, 4, 1);
    leAddress_2->setMaxLength(4);
    leAddress_2->setMaximumWidth(LEMAXWIDTH);
    a2Validator = new QIntValidator(-1, MAX_GADCC, this);
    leAddress_2->setValidator(a2Validator);
    labelAddress_2->setBuddy(leAddress_2);

    /*line with port 1 spinbox */
    port2Label = new QLabel(tr("&Port 2:"), decoderGB);
    decdataLayout->addWidget(port2Label, 5, 0);

    port2SB = new QSpinBox(0, 1, 1, decoderGB, "port2SB");
    port2SB->setWrapping(true);
    decdataLayout->addWidget(port2SB, 5, 1);
    port2Label->setBuddy(port2SB);

    xchConn2CB = new QCheckBox(tr("E&xch. conn."), decoderGB, "xch2");
    connect(xchConn2CB, SIGNAL(toggled(bool)), this,
            SLOT(xchConn2IsToggled(bool)));
    decdataLayout->addWidget(xchConn2CB, 5, 2);

    /*spacer to push contents of box to top */
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    decoderGBL->addItem(spacer);

    /*feedback LED data group box*/
    QGroupBox* feedbackGB = new QGroupBox(0, Horizontal,
            tr("Feedback for track LEDs"), this, "feedbackGB");
    rightColumnLayout->addWidget(feedbackGB);
    QVBoxLayout* feedbackGBL = new QVBoxLayout(feedbackGB->layout(), 6);

    /* line with LED off checkbox*/
    cbLEDoff = new QCheckBox(tr("&LEDs off"), feedbackGB, "LEDsCB");
    feedbackGBL->addWidget(cbLEDoff);
    connect(cbLEDoff, SIGNAL(clicked()), this, SLOT(slotEnable_LED_FB()));

    /*line with bus*/
    QHBoxLayout* busLayout = new QHBoxLayout(feedbackGBL, 6);
    labelFBBus = new QLabel(tr("Bus (s&88/SRCP):"), feedbackGB);
    busLayout->addWidget(labelFBBus);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    busLayout->addItem(spacer);
    fbBusLE = new QLineEdit(feedbackGB, "fbBusLE");
    busLayout->addWidget(fbBusLE);
    fbBusLE->setMaximumWidth(LEMAXWIDTH);
    labelFBBus->setBuddy(fbBusLE);
    QValidator* busValidator = new QIntValidator(1, 999, this);
    fbBusLE->setValidator(busValidator);

    /*line with contact*/
    QHBoxLayout* feedbackLayout = new QHBoxLayout(feedbackGBL, 6);
    labelFBContact = new QLabel(tr("C&ontact (1 - 496):"), feedbackGB);
    feedbackLayout->addWidget(labelFBContact);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    feedbackLayout->addItem(spacer);
    contactSB = new QSpinBox(0, 496, 1, feedbackGB, "contactSB");
    labelFBContact->setBuddy(contactSB);
    feedbackLayout->addWidget(contactSB);
    connect(contactSB, SIGNAL(valueChanged(int)),
            this, SLOT(contactSBChanged(int)));

    /*line with module*/
    QHBoxLayout* moduleLayout = new QHBoxLayout(feedbackGBL, 6);
    labelFBmodule = new QLabel(tr("Module (1 - %1):")
            .arg(pref.fbfactor == 0 ? 31 : 62), feedbackGB);
    moduleLayout->addWidget(labelFBmodule);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    moduleLayout->addItem(spacer);
    moduleLE = new QLineEdit(feedbackGB, "moduleLE");
    moduleLE->setMaximumWidth(LEMAXWIDTH);
    moduleLE->setFocusPolicy(QWidget::NoFocus);
    moduleLayout->addWidget(moduleLE);
    
    /*line with port*/
    QHBoxLayout* portLayout = new QHBoxLayout(feedbackGBL, 6);
    labelFBport = new QLabel(tr("Port (1 - %1):")
            .arg(pref.fbfactor == 0 ? 16 : 8), feedbackGB);
    portLayout->addWidget(labelFBport);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    portLayout->addItem(spacer);
    portLE = new QLineEdit(feedbackGB, "portLE");
    portLE->setMaximumWidth(LEMAXWIDTH);
    portLE->setFocusPolicy(QWidget::NoFocus);
    portLayout->addWidget(portLE);
    
    /*this checkbox never is active, shows only state information*/
    QHBoxLayout* mbLayout = new QHBoxLayout(feedbackGBL, 6);
    cbAdrMod = new QCheckBox(tr("Address module"), feedbackGB,
                "AddressmoduleCB");
    mbLayout->addWidget(cbAdrMod);
    cbAdrMod->setEnabled(false);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    mbLayout->addItem(spacer);
    buttFBmodules = new QPushButton(tr("&FB"), feedbackGB);
    mbLayout->addWidget(buttFBmodules);
    buttFBmodules->setPixmap(QPixmap(viewfeedback_xpm));
    connect(buttFBmodules, SIGNAL(clicked()), this,
            SLOT(slotShowFBmodules()));
    if (pref.tooltips == true)
        QToolTip::add(buttFBmodules, tr("Show feedback module window"));

    /*spacer to push contents of box to top */
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    feedbackGBL->addItem(spacer);

    
    /*layout with OK and Cancel buttons*/
    QBoxLayout* buttonLayout = new QHBoxLayout(baseLayout, 6);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonLayout->addItem(spacer);

    /*button line at bottom*/
    buttOK = new QPushButton(tr("OK"), this);
    buttOK->setDefault(true);
    connect(buttOK, SIGNAL(clicked()), this, SLOT(accept()));
    buttonLayout->addWidget(buttOK);

    QPushButton *CancelButton = new QPushButton(tr("Cancel"), this);
    connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    buttonLayout->addWidget(CancelButton);
}


elementDialog::~elementDialog()
{
    delete IconNameList;
}


/*
 * in this dialog we work with two different list: a QStrList which holds
 * all element names and a ComboBox which holds all elements graphically
 * thus it is not possible to determin the name of a QPixmap in a ComboBox
 * we use a trick: at the same index ID we have the icon in ComboBox and
 * it´s name in QStrList. To get a pixmap´s name we must recalculate the
 * name using the current item ID of the ComboBox
 */
void elementDialog::setupElement(const char *eName)
{
    IconNameList->append(eName);
    QString sPixmapName = RES_DIR_ELEM;
    sPixmapName += eName;
    sPixmapName += XPM_SUFFIX;
    IconComboBox->insertItem(QPixmap(sPixmapName));
}


void elementDialog::slotAddress1Changed(const QString&)
{
    if (cbAddrLabeling->isChecked())
        leText->setText(leAddress_1->text());
}


void elementDialog::letteringChanged(bool takeaddr)
{
    if (takeaddr) {
        leText->setFocusPolicy(QWidget::NoFocus);
        leText->setText(leAddress_1->text());
    }
    else
        leText->setFocusPolicy(QWidget::StrongFocus);
}


void elementDialog::updateValidators()
{
    /*
     *  id  protocol
     *  -------------
     *  -1  none
     *   0  MM
     *   1  DCC
     *   2  Server
     *   3  Selectrix
     *  -------------
     */
    
    QString icon = IconNameList->at(IconComboBox->currentItem());
    
    if (icon == SYM_NRB || icon == SYM_SRB) {
        a1Validator->setTop(MAX_RB);
        a2Validator->setTop(MAX_RB);
    }

    else { 
#if QT_VERSION >= 0x030300
        int prot = protocolBG->selectedId();
#else
        int prot = protocolBG->id(protocolBG->selected());
#endif
        switch (prot) {
            case 0:
                // MM
                a1Validator->setTop(MAX_GAMM);
                a2Validator->setTop(MAX_GAMM);
                
                port1Label->setEnabled(false);
                port2Label->setEnabled(false);
                port1SB->setEnabled(false);
                port2SB->setEnabled(false);
                // new range for port spinboxes
                port1SB->setMinValue(MINMMPORT);
                port1SB->setMaxValue(MAXMMPORT);
                port2SB->setMinValue(MINMMPORT);
                port2SB->setMaxValue(MAXMMPORT);

                xchConn1IsToggled(xchConn1CB->isChecked());
                xchConn2IsToggled(xchConn2CB->isChecked());

                switch (addresscount) {
                    case 0:
                        xchConn1CB->setEnabled(false);
                        xchConn2CB->setEnabled(false);
                        break;
                    case 1:
                        xchConn1CB->setEnabled(true);
                        xchConn2CB->setEnabled(false);
                        break;
                    case 2:
                        xchConn1CB->setEnabled(true);
                        xchConn2CB->setEnabled(true);
                        break;
                }
                
                if (leAddress_1->text().toInt() > MAX_GAMM)
                    leAddress_1->setText(QString::number(MAX_GAMM));

                if (leAddress_2->text().toInt() > MAX_GAMM)
                    leAddress_2->setText(QString::number(MAX_GAMM));
                break;

            case 1:
                //DCC
                a1Validator->setTop(MAX_GADCC);
                a2Validator->setTop(MAX_GADCC);
                
                port1Label->setEnabled(false);
                port2Label->setEnabled(false);
                port1SB->setEnabled(false);
                port2SB->setEnabled(false);
                // new range for port spinboxes
                port1SB->setMinValue(MINDCCPORT);
                port1SB->setMaxValue(MAXDCCPORT);
                port2SB->setMinValue(MINDCCPORT);
                port2SB->setMaxValue(MAXDCCPORT);

                xchConn1IsToggled(xchConn1CB->isChecked());
                xchConn2IsToggled(xchConn2CB->isChecked());
                
                switch (addresscount) {
                    case 0:
                        xchConn1CB->setEnabled(false);
                        xchConn2CB->setEnabled(false);
                        break;
                    case 1:
                        xchConn1CB->setEnabled(true);
                        xchConn2CB->setEnabled(false);
                        break;
                    case 2:
                        xchConn1CB->setEnabled(true);
                        xchConn2CB->setEnabled(true);
                        break;
                }
                
                break;
                
            case 2:
                // Server
                // MAGIC: Validator limits for Protocol by Server
                a1Validator->setTop(9999);
                a2Validator->setTop(9999);
                
                // new range for port spinboxes
                port1SB->setMinValue(MINSVPORT);
                port1SB->setMaxValue(MAXSVPORT);
                port2SB->setMinValue(MINSVPORT);
                port2SB->setMaxValue(MAXSVPORT);

                xchConn1CB->setEnabled(false);
                xchConn2CB->setEnabled(false);

                switch (addresscount) {
                    case 0:
                        port1Label->setEnabled(false);
                        port2Label->setEnabled(false);
                        port1SB->setEnabled(false);
                        port2SB->setEnabled(false);
                        break;
                    case 1:
                        port1Label->setEnabled(true);
                        port2Label->setEnabled(false);
                        port1SB->setEnabled(true);
                        port2SB->setEnabled(false);
                        break;
                    case 2:
                        port1Label->setEnabled(true);
                        port2Label->setEnabled(true);
                        port1SB->setEnabled(true);
                        port2SB->setEnabled(true);
                        break;
                }
                
                break;
                
            case 3:
                //Selectrix
                a1Validator->setTop(MAX_GASX);
                a2Validator->setTop(MAX_GASX);
                
                // new range for port spinboxes
                port1SB->setMinValue(MINSXPORT);
                port1SB->setMaxValue(MAXSXPORT);
                port2SB->setMinValue(MINSXPORT);
                port2SB->setMaxValue(MAXSXPORT);

                xchConn1CB->setEnabled(false);
                xchConn2CB->setEnabled(false);

                switch (addresscount) {
                    case 0:
                        port1Label->setEnabled(false);
                        port2Label->setEnabled(false);
                        port1SB->setEnabled(false);
                        port2SB->setEnabled(false);
                        break;
                    case 1:
                        port1Label->setEnabled(true);
                        port2Label->setEnabled(false);
                        port1SB->setEnabled(true);
                        port2SB->setEnabled(false);
                        break;
                    case 2:
                        port1Label->setEnabled(true);
                        port2Label->setEnabled(true);
                        port1SB->setEnabled(true);
                        port2SB->setEnabled(true);
                        break;
                }
                
                break;
                
            default:
                // undefined protocol
                break;
        }
    }
}


void elementDialog::slotDecoderChanged(int index)
{
    QString sProt = coboDecoder->text(index).right(3);
    if (sProt == QString::null)
        return;

    // autoset protocol type after choosing a sProt
    if (sProt == "(M)")
        rbProtocol_MS->setChecked(true);
    else if (sProt == "(D)")
        rbProtocol_NA->setChecked(true);
    else if (sProt == "(P)")
        rbProtocol_PS->setChecked(true);
    else if (sProt == "(S)")
        rbProtocol_SE->setChecked(true);

    updateValidators();
}


void elementDialog::slotProtocolChanged(int)
{
    QString sProt;
    QString sText;
    
    if (rbProtocol_MS->isChecked())
      sProt = "(M)";
    else if (rbProtocol_NA->isChecked())
      sProt = "(D)";
    else if (rbProtocol_SE->isChecked())
      sProt = "(S)";
    else
      sProt = "(P)";

    // find first decoder which support the chosen protocol
    for (int i = 0; i < coboDecoder->count(); i++) {
        sText = coboDecoder->text(i);

        if (sText.right(3) == sProt) {
            coboDecoder->setCurrentItem(i);
            break;
        }
    }
    updateValidators();
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
    sSoldIcon = IconNameList->at(iCoboIconID);

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

    rbProtocol_MS->setEnabled(enabled || sSoldIcon == SYM_DRE);
    rbProtocol_NA->setEnabled(enabled);
    rbProtocol_PS->setEnabled(enabled);
    rbProtocol_SE->setEnabled(enabled);

    // show address_1 data, but take enabled value from above
    enabled = enabled && sSoldIcon != SYM_SBN && sSoldIcon != SYM_MDC;
    if (enabled) {
        // set direction to 0 if it was -1 before and address_1 is now enabled
        if (gaDirection == -1)
            gaDirection = 0;
        addresscount = 1;
    }
    else {
        leAddress_1->setText("-1");
        addresscount = 0;
    }

    srcpBus1Label->setEnabled(enabled);
    srcpBus1LE->setEnabled(enabled);
    leAddress_1->setEnabled(enabled);
    cbAddrLabeling->setEnabled(enabled);
    labelAddress_1->setEnabled(enabled);
    port1Label->setEnabled(rbProtocol_SE->isChecked());
    port1SB->setEnabled(rbProtocol_SE->isChecked());
    xchConn1CB->setEnabled(enabled);

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

    if (!enabled)
        leText->setText("-1");

    leText->setEnabled(enabled);
    labelText->setEnabled(enabled);

    // show address_2 data
    // Hp0+Hp1+Hp2
    enabled = sSoldIcon == SYM_HSS || sSoldIcon == SYM_DRW
        || sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR
        || sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR
        || sSoldIcon == SYM_MDC || sSoldIcon == SYM_DRE
        || sSoldIcon == SYM_SBN || (sSoldIcon == SYM_VS
                                    && gaSubType == 4)
        || (sSoldIcon == SYM_HS && gaSubType == 4);
    
    if (enabled) {
        /*
        if (sSoldIcon != SYM_DRE && sSoldIcon != SYM_SBN) {
            sListText = listElementData->at(LIST_ID_CHACONN_2);
            xchConn2CB->setChecked(sListText == "1");
        }*/
        addresscount = 2;
    }
    else
        leAddress_2->setText("-1");

    srcpBus2Label->setEnabled(enabled);
    srcpBus2LE->setEnabled(enabled);
    leAddress_2->setEnabled(enabled);
    labelAddress_2->setEnabled(enabled);
    port2Label->setEnabled(rbProtocol_SE->isChecked());
    port2SB->setEnabled(rbProtocol_SE->isChecked());

    xchConn2CB->setEnabled(enabled && sSoldIcon != SYM_DRE
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

    // show LEDoff data
    enabled = sSoldIcon == SYM_KUL || sSoldIcon == SYM_KUR ||
        sSoldIcon == SYM_DIL || sSoldIcon == SYM_DIR ||
        sSoldIcon == SYM_GER || sSoldIcon == SYM_RI1 ||
        sSoldIcon == SYM_NRB || sSoldIcon == SYM_SRB ||
        sSoldIcon == SYM_RI2 || sSoldIcon == SYM_KRH ||
        sSoldIcon == SYM_KRL || sSoldIcon == SYM_KRR ||
        sSoldIcon == SYM_SS || sSoldIcon == SYM_SSH ||
        sSoldIcon == SYM_SSS || sSoldIcon == SYM_WEL || 
        sSoldIcon == SYM_WER;

    cbLEDoff->setEnabled(enabled);

    // show invert data for empty elements only for colour
    enabled = sSoldIcon == SYM_WEL || sSoldIcon == SYM_WER
        || sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR
        || sSoldIcon == SYM_DWL || sSoldIcon == SYM_DWR
        || sSoldIcon == SYM_LEE;

    cbInvert->setEnabled(enabled);
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
          sSoldIcon == SYM_KRL || sSoldIcon == SYM_KRR) &&
         !cbLEDoff->isChecked());

    buttFBmodules->setEnabled(enabled);

    if (sSoldIcon == SYM_ADR) {
        cbAdrMod->setChecked(true);
        //contactSBChanged(1);
        contactSB->setEnabled(false);
    }
    else {
        cbAdrMod->setChecked(false);
        contactSB->setEnabled(true);
        //contactSBChanged(sListText.toInt() + 1);
    }

    fbBusLE->setEnabled(enabled);
    labelFBBus->setEnabled(enabled);
    contactSB->setEnabled(enabled);
    labelFBContact->setEnabled(enabled);
    moduleLE->setEnabled(enabled);
    labelFBmodule->setEnabled(enabled);
    portLE->setEnabled(enabled);
    labelFBport->setEnabled(enabled);

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

    activeTimeSB->setEnabled(enabled);
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
        sListText = lastDecoder;
        if (sListText == "-1")
            sListText = pref.decoder;

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
        gaSubType = -1;

    showSubTypes(enabled);
    updateValidators();
}


void elementDialog::slotEnable_LED_FB()
{
    fbBusLE->setEnabled(!cbLEDoff->isChecked());
    labelFBBus->setEnabled(!cbLEDoff->isChecked());

    contactSB->setEnabled(!cbLEDoff->isChecked());
    labelFBContact->setEnabled(!cbLEDoff->isChecked());

    moduleLE->setEnabled(!cbLEDoff->isChecked());
    labelFBmodule->setEnabled(!cbLEDoff->isChecked());

    portLE->setEnabled(!cbLEDoff->isChecked());
    labelFBport->setEnabled(!cbLEDoff->isChecked());

    //labelAdrMod->setEnabled(!cbLEDoff->isChecked());
    buttFBmodules->setEnabled(!cbLEDoff->isChecked());
}


void elementDialog::showSubTypes(int iShow_)
{
    int i;

    // remove text field and hide buttons
    if (!iShow_) {
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
        if (sSoldIcon == SYM_HS){
            buttSubType[0]->setPixmap(QPixmap(signal_hs_st1_xpm));
            buttSubType[1]->setPixmap(QPixmap(signal_hs_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(signal_hs_st3_xpm));
        }
        else if (sSoldIcon == SYM_HSS){
            buttSubType[0]->setPixmap(QPixmap(signal_hss_st1_xpm));
            buttSubType[1]->setPixmap(QPixmap(signal_hss_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(signal_hss_st3_xpm));
        }
        else if (sSoldIcon == SYM_VS){
            buttSubType[0]->setPixmap(QPixmap(signal_vs_st1_xpm));
            buttSubType[1]->setPixmap(QPixmap(signal_vs_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(signal_vs_st3_xpm));
        }
    }
    
    else if (sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR) {
        if (sSoldIcon == SYM_DKL){
            buttSubType[1]->setPixmap(QPixmap(dkw_links_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(dkw_links_st3_xpm));
        }
        else if (sSoldIcon == SYM_DKR){
            buttSubType[1]->setPixmap(QPixmap(dkw_rechts_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(dkw_rechts_st3_xpm));
        }
        buttSubType[0]->hide();
    }
    else if (sSoldIcon == SYM_ENK) {
        buttSubType[0]->setPixmap(QPixmap(entkoppler_st1_xpm));
        buttSubType[1]->setPixmap(QPixmap(entkoppler_st2_xpm));
        buttSubType[2]->setPixmap(QPixmap(entkoppler_st3_xpm));
    }
    else if (sSoldIcon == SYM_DRE) {
        buttSubType[0]->hide();
        buttSubType[1]->setPixmap(QPixmap(drehscheibe_st2_xpm));
        buttSubType[2]->setPixmap(QPixmap(drehscheibe_st3_xpm));
    }


    // activate the subtype dependant button
    if (sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS || sSoldIcon == SYM_VS) {
        if (sSoldIcon == SYM_HS && pref.tooltips) {
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
        if (sSoldIcon == SYM_HSS && pref.tooltips) {
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
        if (sSoldIcon == SYM_VS && pref.tooltips) {
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

        switch (gaSubType) {
            default:               // on new creation
            case 0:                // Hp0+Hp1
            case 1:                // Hp0+Hp1+Sh1
                buttSubType[0]->setOn(true);
                slotSubTypeClicked(0);
                break;
            case 4:                // Hp0+Hp1+Hp2
            case 5:                // Hp0+Hp1+Hp2+Sh1
                buttSubType[2]->setOn(true);
                slotSubTypeClicked(2);
                break;
            case 6:                // Hp0+Hp2
            case 7:                // Hp0+Hp2+Sh1
                buttSubType[1]->setOn(true);
                slotSubTypeClicked(1);
                break;
        }
    }

    if (sSoldIcon == SYM_ENK) {
        if (pref.tooltips) {
            QToolTip::add(buttSubType[0],
                          tr("Allows to use a:\nbistable coupler"));
            QToolTip::add(buttSubType[1], tr("Allows to use a:\n"
                                             "momentary coupler\n"
                                             "on left connector"));
            QToolTip::add(buttSubType[2], tr("Allows to use a:\n"
                                             "momentary coupler\n"
                                             "on right connector"));
        }
        switch (gaSubType) {
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
        if (pref.tooltips) {
            QToolTip::add(buttSubType[1],
                          tr("Allows to use a:\n"
                             "2 state double turnout\n(f.e. Maerklin 2264)"
                             "\nDOES NOT WORK YET!"));
            QToolTip::add(buttSubType[2],
                          tr("Allows to use a:\n"
                             "4 state double turnout\n"
                             "(f.e. Maerklin 2275,\nall Roco´s)"));
        }
        switch (gaSubType) {
            case 0:
                buttSubType[1]->setOn(true);   // 2 states possible/Maerklin
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

        if (pref.tooltips) {
            QToolTip::add(buttSubType[1], tr("Default turntable:\n"
                                             "Controlled via keyboard #15"));
            QToolTip::add(buttSubType[2], tr("Extra turntable:\n"
                                             "Controlled via keyboard #14"));
        }

        int a2 = leAddress_2->text().toInt();
        switch (a2 / 16) {
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


void elementDialog::slotSubTypeClicked(int stBtn)
{
    switch (stBtn) {
        case 0:                    // == subType 1
            if (sSoldIcon == SYM_HS || sSoldIcon == SYM_VS) {
                srcpBus2Label->setEnabled(false);
                srcpBus2LE->setEnabled(false);
                leAddress_2->setEnabled(false);
                labelAddress_2->setEnabled(false);
                port2Label->setEnabled(false);
                port2SB->setEnabled(false);
                xchConn2CB->setEnabled(false);

                leAddress_2->setText("-1");
                gaSubType = 0;
            }

            else if (sSoldIcon == SYM_HSS) {
                gaSubType = 1;
            }

            else if (sSoldIcon == SYM_ENK) {
                srcpBus2Label->setEnabled(false);
                srcpBus2LE->setEnabled(false);
                leAddress_2->setEnabled(false);
                labelAddress_2->setEnabled(false);
                port2Label->setEnabled(false);
                port2SB->setEnabled(false);
                xchConn2CB->setEnabled(false);

                gaSubType = -1;
            }
            break;

        case 1:                    // == subType 2
            if (sSoldIcon == SYM_HS || sSoldIcon == SYM_VS) {
                srcpBus2Label->setEnabled(false);
                srcpBus2LE->setEnabled(false);
                leAddress_2->setEnabled(false);
                labelAddress_2->setEnabled(false);
                port2Label->setEnabled(false);
                port2SB->setEnabled(false);
                xchConn2CB->setEnabled(false);
                leAddress_2->setText("-1");
                gaSubType = 6;
            }

            else if (sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR) {
                srcpBus2Label->setEnabled(false);
                srcpBus2LE->setEnabled(false);
                leAddress_2->setEnabled(false);
                labelAddress_2->setEnabled(false);
                port2Label->setEnabled(false);
                port2SB->setEnabled(false);
                xchConn2CB->setEnabled(false);
                gaSubType = 0;
            }

            else if (sSoldIcon == SYM_HSS)
                gaSubType = 7;

            else if (sSoldIcon == SYM_ENK)
                gaSubType = 0;

            else if (sSoldIcon == SYM_DRE)
                leAddress_2->setText("240");

            break;

        case 2:                    // == subType 3
            if (sSoldIcon == SYM_HS || sSoldIcon == SYM_VS) {
                srcpBus2Label->setEnabled(true);
                srcpBus2LE->setEnabled(true);
                leAddress_2->setEnabled(true);
                labelAddress_2->setEnabled(true);
                port2Label->setEnabled(true);
                port2SB->setEnabled(true);
                xchConn2CB->setEnabled(true);
                gaSubType = 4;
            }

            else if (sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR) {
                srcpBus2Label->setEnabled(true);
                srcpBus2LE->setEnabled(true);
                leAddress_2->setEnabled(true);
                labelAddress_2->setEnabled(true);
                port2Label->setEnabled(true);
                port2SB->setEnabled(true);
                xchConn2CB->setEnabled(true);
                gaSubType = 1;
            }

            else if (sSoldIcon == SYM_HSS)
                gaSubType = 5;

            else if (sSoldIcon == SYM_ENK)
                gaSubType = 1;

            else if (sSoldIcon == SYM_DRE)
                leAddress_2->setText("224");

            break;
    }
}


void elementDialog::contactSBChanged(int contact)
{
    // FB_16 = 0, FB_8 = 1
    int inputs = 16 - (pref.fbfactor * 8);
    int module = (contact - 1) / inputs + 1;
    int port = contact - (module - 1) * inputs;
    moduleLE->setText(QString::number(module));
    portLE->setText(QString::number(port));
}


int elementDialog::getGASubType()
{
    return gaSubType;
};


void elementDialog::setGASubType(int sType)
{
    int btn = -1;
    gaSubType = sType;
    QString icon = IconNameList->at(IconComboBox->currentItem());

    /*
     * for SYM_ENK:
     *
     * gaSubType   pressed button
     * --------------------------
     *    -1            0
     *     0            1
     *     1            2
     * --------------------------
    */
    if (icon == SYM_ENK)
        btn = gaSubType + 1;

    /*
     * for SYM_HSS:
     *
     * gaSubType   pressed button
     * --------------------------
     *     1            0
     *     7            1
     *     5            2
     * --------------------------
    */
    else if (icon == SYM_HSS) {
        switch (gaSubType) {
            case 1:
                btn = 0;
                break;
            case 5:
                btn = 2;
                break;
            case 7:
                btn = 1;
                break;
            default:
                btn = 0;
                break;
        }
    }
    
    /*
     * for SYM_HS and SYM_VS:
     *
     * gaSubType   pressed button
     * --------------------------
     *     0            0
     *     6            1
     *     4            2
     * --------------------------
    */
    else if (icon == SYM_HS || icon == SYM_VS) {
        switch (gaSubType) {
            case 0:
                btn = 0;
                break;
            case 4:
                btn = 2;
                break;
            case 6:
                btn = 1;
                break;
            default:
                btn = 0;
                break;
        }
    }
    
    /*
     * for SYM_DKL and SYM_DKR:
     *
     * gaSubType   pressed button
     * --------------------------
     *     n.d.         0
     *     0            1
     *     1            2
     * --------------------------
    */
    else if (icon == SYM_DKL || icon == SYM_DKR) {
        switch (gaSubType) {
            case 0:
                btn = 1;
                break;
            case 1:
                btn = 2;
                break;
            default:
                btn = 1;
                break;
        }
    }
    if (btn != -1) {
        bgSubType->setButton(btn);
        slotSubTypeClicked(btn);
    }
}


int elementDialog::getSRCPBus1()
{
    return srcpBus1LE->text().toInt();
};


void elementDialog::setSRCPBus1(int bus)
{
    srcpBus1LE->setText(QString::number(bus));
}


int elementDialog::getSRCPBus2()
{
    return srcpBus2LE->text().toInt();
};


void elementDialog::setSRCPBus2(int bus)
{
    srcpBus2LE->setText(QString::number(bus));
}


QString elementDialog::getSymbolName()
{
    return IconNameList->at(IconComboBox->currentItem());
};


void elementDialog::setSymbolName(const QString& sname) 
{
    IconComboBox->setCurrentItem(IconNameList->find(sname));
    slotSymbolChanged(IconComboBox->currentItem());
}


int elementDialog::getRotated()
{
    return cbRotate->isEnabled() ? (cbRotate->isChecked()? 1 : 0) : -1;
};


void elementDialog::setRotated(int rotated)
{
    if (rotated == -1)
        cbRotate->setChecked(false);
    else
        cbRotate->setChecked(rotated);
}


int elementDialog::getInverted()
{
    return cbInvert->isEnabled() ? (cbInvert->isChecked()? 1 : 0) : -1;
};


void elementDialog::setInverted(int inverted)
{
    if (inverted == -1)
        cbInvert->setChecked(false);
    else
        cbInvert->setChecked(inverted);
}


int elementDialog::getLEDsAreOff()
{
    return cbLEDoff->isEnabled() ? (cbLEDoff->isChecked()? 1 : 0) : -1;
};


void elementDialog::setLEDsAreOff(int off)
{
    if (off == -1)
        cbLEDoff->setEnabled(false);
    else
        cbLEDoff->setChecked(off);
}


QString elementDialog::getDecoder()
{
    return coboDecoder->currentText();
};


void elementDialog::setDecoder(const QString& decoder)
{
    QString sDec;
    lastDecoder = decoder;

    if (decoder == "-1")
        sDec = pref.decoder;
    else
        sDec = decoder;

    for (int i = 0; i < coboDecoder->count(); i++) {
        if (sDec == coboDecoder->text(i)) {
            coboDecoder->setCurrentItem(i);
            break;
        }
    }
}   


int elementDialog::getProtocol()
{
    if (!rbProtocol_MS->isEnabled())
        return SrcpMessage::proNone;
    else if (rbProtocol_MS->isChecked())
        return SrcpMessage::proMM;
    else if (rbProtocol_NA->isChecked())
        return SrcpMessage::proDCC;
    else if (rbProtocol_SE->isChecked())
        return SrcpMessage::proSelectrix;
    else
        return SrcpMessage::proServer;
};


void elementDialog::setProtocol(int protocol)
{
    if (protocol == SrcpMessage::proNone) {
        rbProtocol_MS->setEnabled(false);
        rbProtocol_NA->setEnabled(false);
        rbProtocol_PS->setEnabled(false);
        rbProtocol_SE->setEnabled(false);
    }
    else {
        if (protocol == SrcpMessage::proMM)
            rbProtocol_MS->setChecked(true);
        else if (protocol == SrcpMessage::proDCC)
            rbProtocol_NA->setChecked(true);
        else if (protocol == SrcpMessage::proSelectrix)
            rbProtocol_SE->setChecked(true);
        else
            rbProtocol_PS->setChecked(true);
    }
}


int elementDialog::getAddress1()
{
    return leAddress_1->text().toInt();
};


void elementDialog::setAddress1(int addr)
{
    leAddress_1->setText(QString::number(addr));
}


int elementDialog::getAddress2()
{
    return leAddress_2->text().toInt();
};


void elementDialog::setAddress2(int addr)
{
    leAddress_2->setText(QString::number(addr));
}


void elementDialog::setPort1(int aport)
{
    port1SB->setValue(aport);
}


int elementDialog::getPort1()
{
    port1SB->value();
}


void elementDialog::setPort2(int aport)
{
    port2SB->setValue(aport);
}


int elementDialog::getPort2()
{
    port2SB->value();
}


int elementDialog::getXChangeConn1()
{
    return xchConn1CB->isEnabled() ?
        (xchConn1CB->isChecked() ? 1 : 0) : -1;
};


void elementDialog::setXChangeConn1(int xch)
{
    if (xch == -1)
        xchConn1CB->setChecked(false);
    else
        xchConn1CB->setChecked(xch);
}


int elementDialog::getXChangeConn2()
{
    return xchConn2CB->isEnabled() ?
        (xchConn2CB->isChecked() ? 1 : 0) : -1;
};


void elementDialog::setXChangeConn2(int xch)
{
    if (xch == -1)
        xchConn2CB->setChecked(false);
    else
        xchConn2CB->setChecked(xch);
}


int elementDialog::getDirection()
{
    return leAddress_1->isEnabled() ? gaDirection : -1;
};


void elementDialog::setDirection(int dir)
{
    gaDirection = dir;
};


QString elementDialog::getSymbolText()
{
    return (leText->text() == "-1") ?
        leAddress_1->text() : leText->text();
};


void elementDialog::setSymbolText(const QString& text)
{
    leText->setText(text);
}


int elementDialog::getActiveTime()
{
    return activeTimeSB->isEnabled() ?
        activeTimeSB->value() : -1;
};


void elementDialog::setActiveTime(int atime)
{
    if (atime != -1)
        activeTimeSB->setValue(atime);
    else
        activeTimeSB->setValue(pref.activetime);
}



int elementDialog::getFBBus()
{
    return fbBusLE->text().toInt();
};


void elementDialog::setFBBus(int bus)
{
    fbBusLE->setText(QString::number(bus));
};


int elementDialog::getFBContact()
{
    return contactSB->value();
};


void elementDialog::setFBContact(int contact)
{
    contactSB->setValue(contact);
    contactSBChanged(contact);
};


/* port connector exchange is only used for MM and DCC */
void elementDialog::xchConn1IsToggled(bool ison)
{
    if (ison)
        port1SB->setValue(1);
    else 
        port1SB->setValue(0);
};


/* port connector exchange is only used for MM and DCC */
void elementDialog::xchConn2IsToggled(bool ison)
{
    if (ison)
        port2SB->setValue(1);
    else 
        port2SB->setValue(0);
};
