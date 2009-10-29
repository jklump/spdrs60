/***************************************************************************
                           elementdialog.cpp
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2009 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2009-10-29 20:15:02 $
                           $Revision: 1.81 $
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
 This file provides an user interface to change properties of an element
 ***************************************************************************/

#if QT_VERSION >= 0x040000
#include <q3hbox.h>
#include <q3hboxlayout>
#include <q3gridlayout>
#include <q3vboxlayout>
#else
#include <qhbox.h>
#endif

#include <qlayout.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qpixmap.h>

#include "elementdialog.h"
#include "preferences.h"
#include "resources.h"

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

enum {
    MINMMPORT = 0,
    MAXMMPORT = 1,
    MINDCCPORT = 0,
    MAXDCCPORT = 1,
    MINSVPORT = 0,
    MAXSVPORT = 64535, // no limits
    MINSXPORT = 1,
    MAXSXPORT = 8
};


ElementDialog::ElementDialog(QWidget* parent, int idx):
    QDialog(parent, "ElementDialog", true)
{
    setCaption(tr("Properties of Element #%1").arg(idx));
    gaSubType = 0;
    gaDirection = 0;
    addresscount = 0;
    classid = element::siciNone;

    /*Layout to separate OK Cancel buttons from the upper rest*/
    QBoxLayout* baseLayout = new QVBoxLayout(this, 12, 12);

    /*Layout to separate left and right groupboxest*/
    QBoxLayout* leftRightLayout = new QHBoxLayout(baseLayout, 12);

    /*Layout to separate left column verticaly*/
    QBoxLayout* leftColumnLayout = new QVBoxLayout(leftRightLayout, 6);

    /*Layout to separate right column verticaly*/
    QBoxLayout* rightColumnLayout = new QVBoxLayout(leftRightLayout, 6);


    /*left column*/
    QGroupBox* frData = new QGroupBox(0, Qt::Horizontal, tr("Data"), this,
            "dataGroupBox");
    leftColumnLayout->addWidget(frData);
    QVBoxLayout* rfDataGBLayout = new QVBoxLayout(frData->layout(), 6);


    /*line with text edit line*/
    QHBoxLayout* textLayout = new QHBoxLayout(rfDataGBLayout);
    labelText = new QLabel(tr("&Text:"), frData);
    textLayout->addWidget(labelText);
    textLayout->addStretch();
    leText = new QLineEdit(frData, "text");
    leText->setMaxLength(20);
    textLayout->addWidget(leText);
    labelText->setBuddy(leText);

    /* line with address to text checkbox*/
    cbAddrLabeling = new QCheckBox(tr("Address for la&beling"), frData,
            "addressLabelingCB");
    rfDataGBLayout->addWidget(cbAddrLabeling);
    connect(cbAddrLabeling, SIGNAL(toggled(bool)), this,
            SLOT(letteringChanged(bool))); 

    /* line with invert checkbox*/
    cbInvert = new QCheckBox(tr("&Inverted use"), frData, "invertCB");
    rfDataGBLayout->addWidget(cbInvert);
    connect(cbInvert, SIGNAL(toggled(bool)), this,
            SLOT(invertedChanged(bool))); 

    /* spacer to shift lines above to top*/
    rfDataGBLayout->addStretch();

    /*group of three buttons*/
    bgSubType = new QButtonGroup(3, Qt::Vertical, tr("Symbol variants"),
            this, "bgSubType");
    leftColumnLayout->addWidget(bgSubType);
    bgSubType->setExclusive(true);

    // on startup hide subtype buttons, they are only used by some elements
    for (int i = 0; i < 3; i++) {
        buttSubType[i] = new QPushButton(bgSubType, "buttSubType");
        buttSubType[i]->setPixmap(QPixmap(signal_hss_st1_xpm));
        buttSubType[i]->setToggleButton(true);
        buttSubType[i]->hide();
    }

    connect(bgSubType, SIGNAL(clicked(int)),
            this, SLOT(slotSubTypeClicked(int)));

    /*groupbox with protocol data*/
    protocolBG = new QButtonGroup(4, Qt::Vertical,
                        tr("Protocol"), this, "protocolBG");
    leftColumnLayout->addWidget(protocolBG);
    protocolBG->setExclusive(true);
    rbProtocol_MS = new QRadioButton(tr("&Maerklin/Motorola"), protocolBG);
    rbProtocol_NA = new QRadioButton(tr("&NMRA/DCC"), protocolBG);
    rbProtocol_PS = new QRadioButton(tr("Protocol by Ser&ver"), protocolBG);
    rbProtocol_SE = new QRadioButton(tr("Selectri&x"), protocolBG);
    connect(protocolBG, SIGNAL(clicked(int)),
            this, SLOT(slotProtocolChanged(int)));
    
    /*right side with logic data*/

    /*decoder data group box*/
    QGroupBox* decoderGB = new QGroupBox(0, Qt::Horizontal,
            tr("Decoder"), this, "decoderGB");
    rightColumnLayout->addWidget(decoderGB);
    QVBoxLayout* decoderGBL = new QVBoxLayout(decoderGB->layout(), 6);

    /*line with decoder combobox*/
    QHBoxLayout* decoderLayout = new QHBoxLayout(decoderGBL, 6);
    labelDecoder = new QLabel(tr("T&ype:"), decoderGB);
    decoderLayout->addWidget(labelDecoder);
    decoderLayout->addStretch();

    coboDecoder = new QComboBox(false, decoderGB);
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
    resetLayout->addStretch();

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
    address1Lbl = new QLabel(tr("Address &1:"), decoderGB);
    decdataLayout->addWidget(address1Lbl, 1, 0);
    address1LE = new QLineEdit(decoderGB, "address_1");
    decdataLayout->addWidget(address1LE, 1, 1);
    address1LE->setMaxLength(4);       // address length of NA protocol
    address1LE->setMaximumWidth(LEMAXWIDTH);
    connect(address1LE, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotAddress1Changed(const QString&)));
    addressVdt = new QIntValidator(0, MAX_GADCC, this);
    address1LE->setValidator(addressVdt);
    address1Lbl->setBuddy(address1LE);

    /*line with port 1 spinbox */
    port1Label = new QLabel(tr("&Port 1:"), decoderGB);
    decdataLayout->addWidget(port1Label, 2, 0);

    port1SB = new QSpinBox(0, 1, 1, decoderGB, "port1SB");
    port1SB->setWrapping(true);
    decdataLayout->addWidget(port1SB, 2, 1);
    port1Label->setBuddy(port1SB);

    xchConn1CB = new QCheckBox(tr("&Exch. conn."), decoderGB, "xch1");
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
    address2Lbl = new QLabel(tr("Address &2:"), decoderGB);
    decdataLayout->addWidget(address2Lbl, 4, 0);
    address2LE = new QLineEdit(decoderGB, "address_2");
    decdataLayout->addWidget(address2LE, 4, 1);
    address2LE->setMaxLength(4);
    address2LE->setMaximumWidth(LEMAXWIDTH);
    address2LE->setValidator(addressVdt);
    address2Lbl->setBuddy(address2LE);
    //update all address tooltips due to validator limit change
    updateAddressTooltips();

    /*line with port 1 spinbox */
    port2Label = new QLabel(tr("&Port 2:"), decoderGB);
    decdataLayout->addWidget(port2Label, 5, 0);

    port2SB = new QSpinBox(0, 1, 1, decoderGB, "port2SB");
    port2SB->setWrapping(true);
    decdataLayout->addWidget(port2SB, 5, 1);
    port2Label->setBuddy(port2SB);

    xchConn2CB = new QCheckBox(tr("E&xch. conn."), decoderGB, "xch2");
    //connect(xchConn2CB, SIGNAL(toggled(bool)), this,
    //        SLOT(xchConn2IsToggled(bool)));
    decdataLayout->addWidget(xchConn2CB, 5, 2);

    /*spacer to push contents of box to top */
    decoderGBL->addStretch();

    /*feedback LED data group box*/
    feedbackGB = new QGroupBox(0, Qt::Horizontal,
            tr("Feedback for track LEDs"), this, "feedbackGB");
    rightColumnLayout->addWidget(feedbackGB);
    QVBoxLayout* feedbackGBL = new QVBoxLayout(feedbackGB->layout(), 6);

    /* line with LED off (Gleismelder) checkbox*/
    cbLEDoff = new QCheckBox(tr("&LEDs off"), feedbackGB, "LEDsCB");
    feedbackGBL->addWidget(cbLEDoff);
    connect(cbLEDoff, SIGNAL(clicked()), this, SLOT(slotEnable_LED_FB()));

    /*line with bus*/
    QHBoxLayout* busLayout = new QHBoxLayout(feedbackGBL, 6);
    labelFBBus = new QLabel(tr("Bus (s&88/SRCP):"), feedbackGB);
    busLayout->addWidget(labelFBBus);
    busLayout->addStretch();
    fbBusLE = new QLineEdit(feedbackGB, "fbBusLE");
    busLayout->addWidget(fbBusLE);
    fbBusLE->setMaximumWidth(LEMAXWIDTH);
    labelFBBus->setBuddy(fbBusLE);
    QIntValidator* busValidator = new QIntValidator(1, 999, this);
    fbBusLE->setValidator(busValidator);

    /*line with contact*/
    QHBoxLayout* feedbackLayout = new QHBoxLayout(feedbackGBL, 6);
    labelFBContact = new QLabel(tr("C&ontact (1 - 496):"), feedbackGB);
    feedbackLayout->addWidget(labelFBContact);
    feedbackLayout->addStretch();
    contactSB = new QSpinBox(1, 496, 1, feedbackGB, "contactSB");
    labelFBContact->setBuddy(contactSB);
    feedbackLayout->addWidget(contactSB);
    connect(contactSB, SIGNAL(valueChanged(int)),
            this, SLOT(contactSBChanged(int)));

    /*line with module*/
    QHBoxLayout* moduleLayout = new QHBoxLayout(feedbackGBL, 6);
    labelFBmodule = new QLabel(tr("Module (1 - %1):")
            .arg(pref.fbfactor == 0 ? 31 : 62), feedbackGB);
    moduleLayout->addWidget(labelFBmodule);
    moduleLayout->addStretch();
    moduleLE = new QLineEdit(feedbackGB, "moduleLE");
    moduleLE->setMaximumWidth(LEMAXWIDTH);
#if QT_VERSION >= 0x040000
    moduleLE->setFocusPolicy(Qt::NoFocus);
#else
    moduleLE->setFocusPolicy(QWidget::NoFocus);
#endif
    moduleLayout->addWidget(moduleLE);
    
    /*line with port*/
    QHBoxLayout* portLayout = new QHBoxLayout(feedbackGBL, 6);
    labelFBport = new QLabel(tr("Port (1 - %1):")
            .arg(pref.fbfactor == 0 ? 16 : 8), feedbackGB);
    portLayout->addWidget(labelFBport);
    portLayout->addStretch();
    portLE = new QLineEdit(feedbackGB, "portLE");
    portLE->setMaximumWidth(LEMAXWIDTH);
#if QT_VERSION >= 0x040000
    portLE->setFocusPolicy(Qt::NoFocus);
#else
    portLE->setFocusPolicy(QWidget::NoFocus);
#endif
    portLayout->addWidget(portLE);
    
    /*this checkbox never is active, shows only state information*/
    QHBoxLayout* mbLayout = new QHBoxLayout(feedbackGBL, 6);
    cbAdrMod = new QCheckBox(tr("Address module"), feedbackGB,
                "AddressmoduleCB");
    mbLayout->addWidget(cbAdrMod);
    cbAdrMod->setEnabled(false);
    mbLayout->addStretch();
    buttFBmodules = new QPushButton(tr("&FB"), feedbackGB);
    mbLayout->addWidget(buttFBmodules);
    buttFBmodules->setPixmap(QPixmap(viewfeedback_xpm));
    connect(buttFBmodules, SIGNAL(clicked()), this,
            SLOT(slotShowFBmodules()));
    QToolTip::add(buttFBmodules, tr("Show feedback module window"));

    /*spacer to push contents of box to top */
    feedbackGBL->addStretch();

    
    /*layout with OK and Cancel buttons*/
    QBoxLayout* buttonLayout = new QHBoxLayout(baseLayout, 6);
    buttonLayout->addStretch();

    /*button line at bottom*/
    buttOK = new QPushButton(tr("OK"), this);
    buttOK->setDefault(true);
    connect(buttOK, SIGNAL(clicked()), this, SLOT(validate()));
    //connect(buttOK, SIGNAL(clicked()), this, SLOT(accept()));
    buttonLayout->addWidget(buttOK);

    QPushButton *CancelButton = new QPushButton(tr("Cancel"), this);
    connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    buttonLayout->addWidget(CancelButton);
}


void ElementDialog::slotAddress1Changed(const QString&)
{
    if (cbAddrLabeling->isChecked())
        leText->setText(address1LE->text());
}


void ElementDialog::letteringChanged(bool takeaddr)
{
    if (takeaddr) {
#if QT_VERSION >= 0x040000
        leText->setFocusPolicy(Qt::NoFocus);
#else
        leText->setFocusPolicy(QWidget::NoFocus);
#endif
        leText->setText(address1LE->text());
    }
    else
#if QT_VERSION >= 0x040000
        leText->setFocusPolicy(Qt::StrongFocus);
#else
        leText->setFocusPolicy(QWidget::StrongFocus);
#endif
}

void ElementDialog::updateAddress1Tooltip()
{
    if (address1LE->isEnabled())
        QToolTip::add(address1LE, tr(
                    "Enter address of decoder 1.\n"
                    "Valid range is %1..%2.")
                .arg(addressVdt->bottom())
                .arg(addressVdt->top()));
    else
        QToolTip::remove(address1LE);
}

void ElementDialog::updateAddress2Tooltip()
{
    if (address2LE->isEnabled())
        QToolTip::add(address2LE, tr(
                    "Enter address of decoder 2.\n"
                    "Valid range is %1..%2.")
                .arg(addressVdt->bottom())
                .arg(addressVdt->top()));
    else
        QToolTip::remove(address2LE);
}

void ElementDialog::updateAddressTooltips()
{
    updateAddress1Tooltip();
    updateAddress2Tooltip();
}

void ElementDialog::updateValidators()
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
    
    if (classid == element::siciZt1 || classid == element::siciZt3 ||
            classid == element::siciRt1 || classid == element::siciRt3) {
        addressVdt->setTop(MAX_RB);
        addressVdt->setBottom(MIN_RB);
    }
    else if (classid == element::siciAdr) {
        addressVdt->setTop(MAX_DISP);
        addressVdt->setBottom(MIN_DISP);
    }

    else if (classid == element::siciKrh || classid == element::siciKr1
            || classid == element::siciKl1) {
        addressVdt->setTop(MAX_CROSS);
        addressVdt->setBottom(MIN_CROSS);
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
                addressVdt->setTop(MAX_GAMM);
                updateAddressTooltips();
                
                port1Label->setEnabled(false);
                port2Label->setEnabled(false);
                port1SB->setEnabled(false);
                port2SB->setEnabled(false);
                // new range for port spinboxes
                port1SB->setMinValue(MINMMPORT);
                port1SB->setMaxValue(MAXMMPORT);
                port2SB->setMinValue(MINMMPORT);
                port2SB->setMaxValue(MAXMMPORT);

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
                
                if (address1LE->text().toInt() > MAX_GAMM)
                    address1LE->setText(QString::number(MAX_GAMM));

                if (address2LE->text().toInt() > MAX_GAMM)
                    address2LE->setText(QString::number(MAX_GAMM));
                break;

            case 1:
                //DCC
                addressVdt->setTop(MAX_GADCC);
                updateAddressTooltips();
                
                port1Label->setEnabled(false);
                port2Label->setEnabled(false);
                port1SB->setEnabled(false);
                port2SB->setEnabled(false);
                // new range for port spinboxes
                port1SB->setMinValue(MINDCCPORT);
                port1SB->setMaxValue(MAXDCCPORT);
                port2SB->setMinValue(MINDCCPORT);
                port2SB->setMaxValue(MAXDCCPORT);

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
                addressVdt->setTop(9999);
                updateAddressTooltips();
                
                // new range for port spinboxes
                port1SB->setMinValue(MINSVPORT);
                port1SB->setMaxValue(MAXSVPORT);
                port2SB->setMinValue(MINSVPORT);
                port2SB->setMaxValue(MAXSVPORT);

                //xchConn1CB->setEnabled(false);
                //xchConn2CB->setEnabled(false);

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
                addressVdt->setTop(MAX_GASX);
                updateAddressTooltips();
                
                // new range for port spinboxes
                port1SB->setMinValue(MINSXPORT);
                port1SB->setMaxValue(MAXSXPORT);
                port2SB->setMinValue(MINSXPORT);
                port2SB->setMaxValue(MAXSXPORT);

                //xchConn1CB->setEnabled(false);
                //xchConn2CB->setEnabled(false);

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


void ElementDialog::slotDecoderChanged(int index)
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


void ElementDialog::slotProtocolChanged(int)
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


void ElementDialog::slotShowFBmodules()
{
    emit sigShowFBmodules();
}


void ElementDialog::slotSymbolChanged()
{
    int enabled;
    bool hasvirtualaddress;
    QString sListText;
    
    // elements with virtual addresses need an address, but no protokoll
    hasvirtualaddress =
        classid == element::siciZt1 || classid == element::siciZt3 ||
        classid == element::siciRt1 || classid == element::siciRt3 ||
        classid == element::siciKr1 || classid == element::siciKl1 ||
        classid == element::siciKrh || classid == element::siciAdr;

    // show protocol data => element->hasAddress() or isSwitchable()
    enabled =
        classid == element::siciHs1 || classid == element::siciHs3 ||
        classid == element::siciHss1 || classid == element::siciHss3 ||
        classid == element::siciSs1 || classid == element::siciSs3 ||
        classid == element::siciWs1 || classid == element::siciWs3 ||
        classid == element::siciSh1 || classid == element::siciSh3 ||
        classid == element::siciSd1 || classid == element::siciSd3 ||
        classid == element::siciTr3 || classid == element::siciTr1 ||
        classid == element::siciTl1 || classid == element::siciTl3 ||
        classid == element::siciIl1 || classid == element::siciIr3 ||
        classid == element::siciIl3 || classid == element::siciIr1 ||
        classid == element::siciSl1 || classid == element::siciSl3 ||
        classid == element::siciSr1 || classid == element::siciSr3 ||
        classid == element::siciDl1 || classid == element::siciDr1 ||
        classid == element::siciTw1 || classid == element::siciTw3 ||
        classid == element::siciEnk || classid == element::siciRel ||
        classid == element::siciMdc ||
        classid == element::siciSy1 || classid == element::siciSy3 ||
        classid == element::siciSbn || classid == element::siciBld ||
        classid == element::siciZp1 || classid == element::siciZp3 ||
        classid == element::siciVs1 || classid == element::siciVs3;

    rbProtocol_MS->setEnabled(enabled || classid == element::siciDre);
    rbProtocol_NA->setEnabled(enabled);
    rbProtocol_PS->setEnabled(enabled);
    rbProtocol_SE->setEnabled(enabled);

    // show address_1 data, but take enabled value from above
    // TODO: include items with virtual addresses
    // exclude turntable and transfer table
    enabled = enabled && classid != element::siciSbn &&
        classid != element::siciMdc;

    if (enabled || hasvirtualaddress) {
        // set direction to 0 if it was -1 before and address_1 is now enabled
        if (gaDirection == -1)
            gaDirection = 0;
        addresscount = 1;
    }
    else {
        address1LE->setText("-1");
        addresscount = 0;
    }

    srcpBus1Label->setEnabled(enabled || hasvirtualaddress);
    srcpBus1LE->setEnabled(enabled || hasvirtualaddress);
    address1Lbl->setEnabled(enabled || hasvirtualaddress);
    address1LE->setEnabled(enabled || hasvirtualaddress);
    updateAddress1Tooltip();
    cbAddrLabeling->setEnabled(classid != element::siciAdr &&
            (enabled || hasvirtualaddress));
    port1Label->setEnabled(rbProtocol_SE->isChecked());
    port1SB->setEnabled(rbProtocol_SE->isChecked());
    xchConn1CB->setEnabled(enabled);

    // show text data
    enabled = 
        classid == element::siciHs1 || classid == element::siciHs3 ||
        classid == element::siciHss1 || classid == element::siciHss3 ||
        classid == element::siciWs1 || classid == element::siciWs3 ||
        classid == element::siciVs1 || classid == element::siciVs3 ||
        classid == element::siciTr3 || classid == element::siciTr1 ||
        classid == element::siciTl1 || classid == element::siciTl3 ||
        classid == element::siciIl1 || classid == element::siciIr3 ||
        classid == element::siciIl3 || classid == element::siciIr1 ||
        classid == element::siciSl1 || classid == element::siciSl3 ||
        classid == element::siciSr1 || classid == element::siciSr3 ||
        classid == element::siciDl1 || classid == element::siciDr1 ||
        classid == element::siciTw1 || classid == element::siciTw3 ||
        classid == element::siciSs1 || classid == element::siciSs3 ||
        classid == element::siciSh1 || classid == element::siciSh3 ||
        classid == element::siciSd1 || classid == element::siciSd3 ||
        classid == element::siciEnk || classid == element::siciRel ||
        classid == element::siciSt1 || classid == element::siciTxt ||
        classid == element::siciZt1 || classid == element::siciZt3 ||
        classid == element::siciRt1 || classid == element::siciRt3 ||
        classid == element::siciMdc ||
        classid == element::siciSy1 || classid == element::siciSy3 ||
        classid == element::siciSbn || classid == element::siciAdr ||
        classid == element::siciTdr || classid == element::siciTdl ||
        classid == element::siciTdb || classid == element::siciDre ||
        classid == element::siciZp1 || classid == element::siciZp3 ||
        classid == element::siciBld;

    if (!enabled)
        leText->setText("-1");

    leText->setEnabled(enabled);
    labelText->setEnabled(enabled);

    // show address_2 data
    // Hp0+Hp1+Hp2
    enabled =
        classid == element::siciHss1 || classid == element::siciHss3 ||
        classid == element::siciTw1 || classid == element::siciTw3 ||
        classid == element::siciDl1 || classid == element::siciDr1 || 
        classid == element::siciSl1 || classid == element::siciSl3 ||
        classid == element::siciSr1 || classid == element::siciSr3 ||
        classid == element::siciMdc || classid == element::siciDre || 
        classid == element::siciSbn || 
        (classid == element::siciVs1 && gaSubType == 4) || 
        (classid == element::siciVs3 && gaSubType == 4) || 
        (classid == element::siciHs1 && gaSubType == 4) || 
        (classid == element::siciHs3 && gaSubType == 4);
    
    if (enabled) {
        /*
        if (classid != element::siciDre && classid != element::siciSbn) {
            sListText = listElementData->at(LIST_ID_CHACONN_2);
            xchConn2CB->setChecked(sListText == "1");
        }*/
        addresscount = 2;
    }
    else
        address2LE->setText("-1");

    srcpBus2Label->setEnabled(enabled);
    srcpBus2LE->setEnabled(enabled);
    address2LE->setEnabled(enabled);
    updateAddress2Tooltip();
    address2Lbl->setEnabled(enabled);
    port2Label->setEnabled(rbProtocol_SE->isChecked());
    port2SB->setEnabled(rbProtocol_SE->isChecked());

    xchConn2CB->setEnabled(enabled && classid != element::siciDre
                           && classid != element::siciSbn
                           && classid != element::siciMdc);
 
    if (classid == element::siciDre)
#if QT_VERSION >= 0x040000
        address2LE->setFocusPolicy(Qt::NoFocus);
    else
        address2LE->setFocusPolicy(Qt::StrongFocus);
#else
        address2LE->setFocusPolicy(QWidget::NoFocus);
    else
        address2LE->setFocusPolicy(QWidget::StrongFocus);
#endif

    // show LEDoff data (Gleismelder)
    enabled =
        classid == element::siciCl1 || classid == element::siciCr1 ||
        classid == element::siciCr3 || classid == element::siciCl3 ||
        classid == element::siciSt4 || classid == element::siciSt3 ||
        classid == element::siciSt1 || classid == element::siciSt2 ||
        classid == element::siciTdr || classid == element::siciTdl ||
        classid == element::siciTdb ||
        classid == element::siciZt1 || classid == element::siciZt3 ||
        classid == element::siciRt1 || classid == element::siciRt3 ||
        classid == element::siciKrh ||
        classid == element::siciKl1 || classid == element::siciKr1 ||
        classid == element::siciTuh || classid == element::siciTuv ||
        classid == element::siciTul || classid == element::siciTur ||
        classid == element::siciSs1 || classid == element::siciSs3 ||
        classid == element::siciSh1 || classid == element::siciSh3 ||
        classid == element::siciSd1 || classid == element::siciSd3 ||
        classid == element::siciCl2 || classid == element::siciCr2 ||
        classid == element::siciCl4 || classid == element::siciCr4 ||
        classid == element::siciTr3 || classid == element::siciTr1 ||
        classid == element::siciTl1 || classid == element::siciTl3 ||
        classid == element::siciSy1 || classid == element::siciSy3 ||
        classid == element::siciTw1 || classid == element::siciTw3 ||
        classid == element::siciIl1 || classid == element::siciIr3 ||
        classid == element::siciIl3 || classid == element::siciIr1 ||
        classid == element::siciSl1 || classid == element::siciSl3 ||
        classid == element::siciSr1 || classid == element::siciSr3 ||
        classid == element::siciDl1 || classid == element::siciDr1 ||
        classid == element::siciEnk || classid == element::siciBld;

    cbLEDoff->setEnabled(enabled);

    // show invert data for empty elements only for colour
    enabled =
        classid == element::siciTr3 || classid == element::siciTr1 ||
        classid == element::siciTl1 || classid == element::siciTl3 ||
        classid == element::siciSl1 || classid == element::siciSl3 ||
        classid == element::siciSr1 || classid == element::siciSr3 ||
        classid == element::siciIl1 || classid == element::siciIr3 ||
        classid == element::siciIl3 || classid == element::siciIr1 ||
        classid == element::siciTxt || classid == element::siciAdr;

    cbInvert->setEnabled(enabled);
    invertedChanged(cbInvert->isChecked());

    // element gets feedback messages
    enabled =
        classid == element::siciHs1 || classid == element::siciHs3 ||
        classid == element::siciHss1 || classid == element::siciHss3 ||
        classid == element::siciSh1 || classid == element::siciSh3 ||
        classid == element::siciSd1 || classid == element::siciSd3 ||
        classid == element::siciAdr ||
        classid == element::siciVs1 || classid == element::siciVs3 ||
        classid == element::siciWs1 || classid == element::siciWs3 ||
        classid == element::siciSs1 || classid == element::siciSs3 || 
        classid == element::siciZp1 || classid == element::siciZp3 ||
        ((classid == element::siciCl1 || classid == element::siciCr1 ||
          classid == element::siciCr3 || classid == element::siciCl3 || 
          classid == element::siciEnk || classid == element::siciBld || 
          classid == element::siciSl1 || classid == element::siciSl3 ||
          classid == element::siciSr1 || classid == element::siciSr3 ||
          classid == element::siciDl1 || classid == element::siciDr1 ||
          classid == element::siciTw1 || classid == element::siciTw3 ||
          classid == element::siciSy1 || classid == element::siciSy3 ||
          classid == element::siciTr3 || classid == element::siciTr1 ||
          classid == element::siciTl1 || classid == element::siciTl3 ||
          classid == element::siciIl1 || classid == element::siciIr3 ||
          classid == element::siciIl3 || classid == element::siciIr1 ||
          classid == element::siciSt4 || classid == element::siciSt3 ||
          classid == element::siciSt1 || classid == element::siciSt2 ||
          classid == element::siciTdl || classid == element::siciTdr ||
          classid == element::siciCl2 || classid == element::siciCr2 ||
          classid == element::siciCl4 || classid == element::siciCr4 ||
          classid == element::siciZt1 || classid == element::siciZt3 ||
          classid == element::siciRt1 || classid == element::siciRt3 ||
          classid == element::siciTdb || classid == element::siciKrh ||
          classid == element::siciKl1 || classid == element::siciKr1) &&
         !cbLEDoff->isChecked());

    buttFBmodules->setEnabled(enabled);

    if (classid == element::siciAdr) {
        cbAdrMod->setChecked(true);
        //contactSBChanged(1);
        contactSB->setEnabled(false);
        if (address1LE->text() == "-1")
            address1LE->setText(QString::number(MIN_DISP));
    }
    else {
        cbAdrMod->setChecked(false);
        contactSB->setEnabled(true);
        //contactSBChanged(sListText.toInt() + 1);
        if (classid == element::siciZt1 || classid == element::siciZt3 ||
                classid == element::siciRt1 || classid == element::siciRt3)
            if (address1LE->text() == "-1")
                address1LE->setText(QString::number(MIN_RB));
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
    enabled =
        classid == element::siciHs1 || classid == element::siciHs3 ||
        classid == element::siciHss1 || classid == element::siciHss3 ||
        classid == element::siciVs1 || classid == element::siciVs3 ||
        classid == element::siciWs1 || classid == element::siciWs3 ||
        classid == element::siciTr3 || classid == element::siciTr1 ||
        classid == element::siciTl1 || classid == element::siciTl3 ||
        classid == element::siciIl1 || classid == element::siciIr3 ||
        classid == element::siciIl3 || classid == element::siciIr1 ||
        classid == element::siciSl1 || classid == element::siciSl3 ||
        classid == element::siciSr1 || classid == element::siciSr3 ||
        classid == element::siciDl1 || classid == element::siciDr1 ||
        classid == element::siciTw1 || classid == element::siciTw3 ||
        classid == element::siciSs1 || classid == element::siciSs3 ||
        classid == element::siciSh1 || classid == element::siciSh3 ||
        classid == element::siciSd1 || classid == element::siciSd3 ||
        classid == element::siciEnk || classid == element::siciRel ||
        classid == element::siciDre ||
        classid == element::siciSy1 || classid == element::siciSy3 ||
        classid == element::siciSbn || classid == element::siciMdc ||
        classid == element::siciBld ||
        classid == element::siciZp1 || classid == element::siciZp3;

    activeTimeSB->setEnabled(enabled);
    labelTime->setEnabled(enabled);

    // show decoder data
    enabled =
        classid == element::siciHs1 || classid == element::siciHs3 ||
        classid == element::siciHss1 || classid == element::siciHss3 ||
        classid == element::siciVs1 || classid == element::siciVs3 ||
        classid == element::siciWs1 || classid == element::siciWs3 ||
        classid == element::siciTr3 || classid == element::siciTr1 ||
        classid == element::siciTl1 || classid == element::siciTl3 ||
        classid == element::siciIl1 || classid == element::siciIr3 ||
        classid == element::siciIl3 || classid == element::siciIr1 ||
        classid == element::siciSl1 || classid == element::siciSl3 ||
        classid == element::siciSr1 || classid == element::siciSr3 ||
        classid == element::siciDl1 || classid == element::siciDr1 ||
        classid == element::siciTw1 || classid == element::siciTw3 ||
        classid == element::siciSs1 || classid == element::siciSs3 ||
        classid == element::siciSh1 || classid == element::siciSh3 ||
        classid == element::siciSd1 || classid == element::siciSd3 ||
        classid == element::siciEnk || classid == element::siciRel ||
        classid == element::siciSy1 || classid == element::siciSy3 ||
        classid == element::siciSbn || classid == element::siciMdc ||
        classid == element::siciBld ||
        classid == element::siciZp1 || classid == element::siciZp3;

    if (!enabled && classid != element::siciDre)
        coboDecoder->setCurrentItem(coboDecoder->count() - 1);  // == -1
    // Maerklin special turntable decoder
    else if (!enabled && classid == element::siciDre)
        coboDecoder->setCurrentItem(8);
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
    enabled =
        classid == element::siciHs1 || classid == element::siciHs3 ||
        classid == element::siciHss1 || classid == element::siciHss3 ||
        classid == element::siciDl1 || classid == element::siciDr1 ||
        classid == element::siciEnk || classid == element::siciDre ||
        classid == element::siciVs1 || classid == element::siciVs3;

    if (!enabled)
        gaSubType = -1;

    showSubTypes(enabled);
    updateValidators();
    updateAddressTooltips();
}


void ElementDialog::slotEnable_LED_FB()
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


void ElementDialog::showSubTypes(int iShow_)
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
    if (classid == element::siciHs1 || classid == element::siciHs3 ||
            classid == element::siciHss1 || classid == element::siciHss3 ||
            classid == element::siciVs1 || classid == element::siciVs3) {

        if (classid == element::siciHs1 || classid == element::siciHs3){
            buttSubType[0]->setPixmap(QPixmap(signal_hs_st1_xpm));
            buttSubType[1]->setPixmap(QPixmap(signal_hs_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(signal_hs_st3_xpm));
        }
        
        else if (classid == element::siciHss1 || classid == element::siciHss3){
            buttSubType[0]->setPixmap(QPixmap(signal_hss_st1_xpm));
            buttSubType[1]->setPixmap(QPixmap(signal_hss_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(signal_hss_st3_xpm));
        }
        
        else if (classid == element::siciVs1 || classid == element::siciVs3){
            buttSubType[0]->setPixmap(QPixmap(signal_vs_st1_xpm));
            buttSubType[1]->setPixmap(QPixmap(signal_vs_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(signal_vs_st3_xpm));
        }
    }
    
    else if (classid == element::siciDl1 || classid == element::siciDr1) {
        if (classid == element::siciDl1){
            buttSubType[1]->setPixmap(QPixmap(dkw_links_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(dkw_links_st3_xpm));
        }
        else if (classid == element::siciDr1){
            buttSubType[1]->setPixmap(QPixmap(dkw_rechts_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(dkw_rechts_st3_xpm));
        }
        buttSubType[0]->hide();
    }
    else if (classid == element::siciEnk) {
        buttSubType[0]->setPixmap(QPixmap(entkoppler_st1_xpm));
        buttSubType[1]->setPixmap(QPixmap(entkoppler_st2_xpm));
        buttSubType[2]->setPixmap(QPixmap(entkoppler_st3_xpm));
    }
    else if (classid == element::siciDre) {
        buttSubType[0]->hide();
        buttSubType[1]->setPixmap(QPixmap(drehscheibe_st2_xpm));
        buttSubType[2]->setPixmap(QPixmap(drehscheibe_st3_xpm));
    }


    // activate the subtype dependent button
    if (classid == element::siciHs1 || classid == element::siciHs3 ||
            classid == element::siciHss1 || classid == element::siciHss3 ||
            classid == element::siciVs1 || classid == element::siciVs3) {
        if (classid == element::siciHs1 || classid == element::siciHs3) {
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
        if (classid == element::siciHss1 || classid == element::siciHss3) {
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
        if (classid == element::siciVs1 || classid == element::siciVs3) {
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

    if (classid == element::siciEnk) {
        QToolTip::add(buttSubType[0],
                tr("Allows to use a bistable coupler"));
        QToolTip::add(buttSubType[1], tr("Allows to use a:\n"
                    "momentary coupler on left connector"));
        QToolTip::add(buttSubType[2], tr("Allows to use a:\n"
                    "momentary coupler on right connector"));
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

    if (classid == element::siciDl1 || classid == element::siciDr1) {
        QToolTip::add(buttSubType[1],
                tr("Allows to use a:\n"
                    "2 state double turnout\n(f.e. Maerklin 2264)"
                    "\nDOES NOT WORK YET!"));
        QToolTip::add(buttSubType[2],
                tr("Allows to use a:\n"
                    "4 state double turnout\n"
                    "(f.e. Maerklin 2275,\nall Roco´s)"));
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

    if (classid == element::siciDre) {

        QToolTip::add(buttSubType[1], tr("Default turntable:\n"
                    "Controlled via keyboard #15"));
        QToolTip::add(buttSubType[2], tr("Extra turntable:\n"
                    "Controlled via keyboard #14"));

        int a2 = address2LE->text().toInt();
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


void ElementDialog::slotSubTypeClicked(int stBtn)
{
    switch (stBtn) {
        case 0:                    // == subType 1
            if (classid == element::siciHs1 || classid == element::siciHs3 ||
                    classid == element::siciVs1 || classid == element::siciVs3) {
                srcpBus2Label->setEnabled(false);
                srcpBus2LE->setEnabled(false);
                address2LE->setEnabled(false);
                updateAddress2Tooltip();
                address2Lbl->setEnabled(false);
                port2Label->setEnabled(false);
                port2SB->setEnabled(false);
                xchConn2CB->setEnabled(false);

                address2LE->setText("-1");
                gaSubType = 0;
            }

            else if (classid == element::siciHss1 ||
                    classid == element::siciHss3) {
                gaSubType = 1;
            }

            else if (classid == element::siciEnk) {
                srcpBus2Label->setEnabled(false);
                srcpBus2LE->setEnabled(false);
                address2LE->setEnabled(false);
                updateAddress2Tooltip();
                address2Lbl->setEnabled(false);
                port2Label->setEnabled(false);
                port2SB->setEnabled(false);
                xchConn2CB->setEnabled(false);

                gaSubType = -1;
            }
            break;

        case 1:                    // == subType 2
            if (classid == element::siciHs1 || classid == element::siciHs3 ||
                    classid == element::siciVs1 || classid == element::siciVs3) {
                srcpBus2Label->setEnabled(false);
                srcpBus2LE->setEnabled(false);
                address2LE->setEnabled(false);
                updateAddress2Tooltip();
                address2Lbl->setEnabled(false);
                port2Label->setEnabled(false);
                port2SB->setEnabled(false);
                xchConn2CB->setEnabled(false);
                address2LE->setText("-1");
                gaSubType = 6;
            }

            else if (classid == element::siciDl1 || classid == element::siciDr1) {
                srcpBus2Label->setEnabled(false);
                srcpBus2LE->setEnabled(false);
                address2LE->setEnabled(false);
                updateAddress2Tooltip();
                address2Lbl->setEnabled(false);
                port2Label->setEnabled(false);
                port2SB->setEnabled(false);
                xchConn2CB->setEnabled(false);
                gaSubType = 0;
            }

            else if (classid == element::siciHss1 ||
                    classid == element::siciHss3)
                gaSubType = 7;

            else if (classid == element::siciEnk)
                gaSubType = 0;

            else if (classid == element::siciDre)
                address2LE->setText("225");  // type 15

            break;

        case 2:                    // == subType 3
            if (classid == element::siciHs1 || classid == element::siciHs3 ||
                    classid == element::siciVs1 || classid == element::siciVs3) {
                srcpBus2Label->setEnabled(true);
                srcpBus2LE->setEnabled(true);
                address2LE->setEnabled(true);
                updateAddress2Tooltip();
                address2Lbl->setEnabled(true);
                port2Label->setEnabled(true);
                port2SB->setEnabled(true);
                xchConn2CB->setEnabled(true);
                gaSubType = 4;
            }

            else if (classid == element::siciDl1 || classid == element::siciDr1) {
                srcpBus2Label->setEnabled(true);
                srcpBus2LE->setEnabled(true);
                address2LE->setEnabled(true);
                updateAddress2Tooltip();
                address2Lbl->setEnabled(true);
                port2Label->setEnabled(true);
                port2SB->setEnabled(true);
                xchConn2CB->setEnabled(true);
                gaSubType = 1;
            }

            else if (classid == element::siciHss1 ||
                    classid == element::siciHss3)
                gaSubType = 5;

            else if (classid == element::siciEnk)
                gaSubType = 1;

            else if (classid == element::siciDre)
                address2LE->setText("209"); // type 14

            break;
    }
}


void ElementDialog::contactSBChanged(int contact)
{
    // FB_16 = 0, FB_8 = 1
    int inputs = 16 - (pref.fbfactor * 8);
    int module = (contact - 1) / inputs + 1;
    int port = contact - (module - 1) * inputs;
    moduleLE->setText(QString::number(module));
    portLE->setText(QString::number(port));
}


int ElementDialog::getGASubType()
{
    return gaSubType;
}


void ElementDialog::setGASubType(int sType)
{
    int btn = -1;
    gaSubType = sType;

    /*
     * for element::siciEnk:
     *
     * gaSubType   pressed button
     * --------------------------
     *    -1            0
     *     0            1
     *     1            2
     * --------------------------
    */
    if (classid == element::siciEnk)
        btn = gaSubType + 1;

    /*
     * for element::siciHss:
     *
     * gaSubType   pressed button
     * --------------------------
     *     1            0
     *     7            1
     *     5            2
     * --------------------------
    */
    else if (classid == element::siciHss1 || classid == element::siciHss3) {
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
     * for element::siciHS and element::siciVS:
     *
     * gaSubType   pressed button
     * --------------------------
     *     0            0
     *     6            1
     *     4            2
     * --------------------------
    */
    else if (classid == element::siciHs1 || classid == element::siciHs3 ||
            classid == element::siciVs1 || classid == element::siciVs3) {
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
     * for element::siciDl1 and element::siciDr1:
     *
     * gaSubType   pressed button
     * --------------------------
     *     n.d.         0
     *     0            1
     *     1            2
     * --------------------------
    */
    else if (classid == element::siciDl1 || classid == element::siciDr1) {
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


int ElementDialog::getSRCPBus1()
{
    return srcpBus1LE->text().toInt();
}


void ElementDialog::setSRCPBus1(int bus)
{
    srcpBus1LE->setText(QString::number(bus));
}


int ElementDialog::getSRCPBus2()
{
    return srcpBus2LE->text().toInt();
}


void ElementDialog::setSRCPBus2(int bus)
{
    srcpBus2LE->setText(QString::number(bus));
}


void ElementDialog::setClassId(element::SpdrItemClassId ci) 
{
    classid = ci;
    slotSymbolChanged();
}


int ElementDialog::getInverted()
{
    return cbInvert->isEnabled() ? (cbInvert->isChecked()? 1 : 0) : -1;
}


void ElementDialog::setInverted(int inverted)
{
    if (inverted == -1)
        cbInvert->setChecked(false);
    else
        cbInvert->setChecked(inverted);
}


int ElementDialog::getLEDsAreOff()
{
    return cbLEDoff->isEnabled() ? (cbLEDoff->isChecked()? 1 : 0) : -1;
}


void ElementDialog::setLEDsAreOff(int off)
{
    if (off == -1)
        cbLEDoff->setEnabled(false);
    else
        cbLEDoff->setChecked(off);

    slotEnable_LED_FB();
}


QString ElementDialog::getDecoder()
{
    return coboDecoder->currentText();
}


void ElementDialog::setDecoder(const QString& decoder)
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


int ElementDialog::getProtocol()
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
}


void ElementDialog::setProtocol(int protocol)
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


int ElementDialog::getAddress1()
{
    return address1LE->text().toInt();
}


void ElementDialog::setAddress1(int addr)
{
    address1LE->setText(QString::number(addr));
}


int ElementDialog::getAddress2()
{
    return address2LE->text().toInt();
}


void ElementDialog::setAddress2(int addr)
{
    address2LE->setText(QString::number(addr));
}


void ElementDialog::setPort1(int aport)
{
    port1SB->setValue(aport);
}


int ElementDialog::getPort1()
{
    return port1SB->value();
}


void ElementDialog::setPort2(int aport)
{
    port2SB->setValue(aport);
}


int ElementDialog::getPort2()
{
    return port2SB->value();
}


int ElementDialog::getXChangeConn1()
{
    return xchConn1CB->isEnabled() ?
        (xchConn1CB->isChecked() ? 1 : 0) : -1;
}


void ElementDialog::setXChangeConn1(int xch)
{
    if (xch == -1)
        xchConn1CB->setChecked(false);
    else
        xchConn1CB->setChecked(xch);
}


int ElementDialog::getXChangeConn2()
{
    return xchConn2CB->isEnabled() ?
        (xchConn2CB->isChecked() ? 1 : 0) : -1;
}


void ElementDialog::setXChangeConn2(int xch)
{
    if (xch == -1)
        xchConn2CB->setChecked(false);
    else
        xchConn2CB->setChecked(xch);
}


int ElementDialog::getDirection()
{
    return address1LE->isEnabled() ? gaDirection : -1;
}


void ElementDialog::setDirection(int dir)
{
    gaDirection = dir;
}


QString ElementDialog::getSymbolText()
{
    return (leText->text() == "-1") ?
        address1LE->text() : leText->text();
}


void ElementDialog::setSymbolText(const QString& text)
{
    leText->setText(text);
}


int ElementDialog::getActiveTime()
{
    return activeTimeSB->isEnabled() ?
        activeTimeSB->value() : -1;
}


void ElementDialog::setActiveTime(int atime)
{
    if (atime != -1)
        activeTimeSB->setValue(atime);
    else
        activeTimeSB->setValue(pref.activetime);
}



int ElementDialog::getFBBus()
{
    return fbBusLE->text().toInt();
}


void ElementDialog::setFBBus(int bus)
{
    fbBusLE->setText(QString::number(bus));
}


int ElementDialog::getFBContact()
{
    return contactSB->value();
}


void ElementDialog::setFBContact(int contact)
{
    contactSB->setValue(contact);
    contactSBChanged(contact);
}

/*
 * enabe/disable feedback editing depending on address symbol is EDiTs
 * or train number tracing type
 */
void ElementDialog::invertedChanged(bool inverted)
{
    if (classid == element::siciAdr) {
        leText->setEnabled(!inverted);
        if (inverted)
            leText->setText("-1");
        else
            leText->setText("0");
        feedbackGB->setEnabled(!inverted);
        srcpBus1Label->setEnabled(inverted);
        srcpBus1LE->setEnabled(inverted);
        address1Lbl->setEnabled(inverted);
        address1LE->setEnabled(inverted);
        updateAddress1Tooltip();
    }
}

void ElementDialog::validate()
{
#if QT_VERSION < 0x030200
    int pos = 0;
    QString value = address1LE->text();
#endif
    if (address1LE->isEnabled() &&
#if QT_VERSION >= 0x030200
            !address1LE->hasAcceptableInput()
#else
            (addressVdt->validate(value, pos) == QValidator::Invalid)
#endif
            ) {
        address1LE->setFocus();
        address1LE->selectAll();
        QMessageBox::warning(this, tr("Unvalid address detected"),
                tr("Value of address 1 is not valid.\n"
                    "Valid range is %1..%2.")
                .arg(addressVdt->bottom())
                .arg(addressVdt->top())
        , tr("OK"));
        return;
    }
    if (address2LE->isEnabled() &&
#if QT_VERSION >= 0x030200
            !address2LE->hasAcceptableInput()
#else
            (addressVdt->validate(value, pos) == QValidator::Invalid)
#endif
            ) {
        address2LE->setFocus();
        address2LE->selectAll();
        QMessageBox::warning(this, tr("Unvalid address detected"),
                tr("Value of address 2 is not valid.\n"
                    "Valid range is %1..%2.")
                .arg(addressVdt->bottom())
                .arg(addressVdt->top())
                , tr("OK"));
        return;
    }
    accept();
}
