/***************************************************************************
                           elementdialog.cpp
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2007 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-03-22 16:09:16 $
                           $Revision: 1.62 $
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

#define MINMMPORT 0
#define MAXMMPORT 1
#define MINDCCPORT 0
#define MAXDCCPORT 1
#define MINSVPORT 0
#define MAXSVPORT 64535 // no limits
#define MINSXPORT 1
#define MAXSXPORT 8

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
    textLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));
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

    /* line with rotate checkbox*/
    cbRotate = new QCheckBox(tr("&Rotation"), frData, "rotateCB");
    rfDataGBLayout->addWidget(cbRotate);

    /* line with invert checkbox*/
    cbInvert = new QCheckBox(tr("&Inverted use"), frData, "invertCB");
    rfDataGBLayout->addWidget(cbInvert);
    connect(cbInvert, SIGNAL(toggled(bool)), this,
            SLOT(invertedChanged(bool))); 

    /* spacer to shift lines above to top*/
    rfDataGBLayout->addItem(new QSpacerItem(0, 0,
                QSizePolicy::Expanding, QSizePolicy::Minimum));

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
    rbProtocol_MS = new QRadioButton("&Maerklin/Motorola", protocolBG);
    rbProtocol_NA = new QRadioButton("&NMRA/DCC", protocolBG);
    rbProtocol_PS = new QRadioButton("Protocol by Ser&ver", protocolBG);
    rbProtocol_SE = new QRadioButton("Selectri&x", protocolBG);
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
    decoderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));

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
    resetLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));

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
    a1Validator = new QIntValidator(-1, MAX_GADCC, this);
    address1LE->setValidator(a1Validator);
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
    a2Validator = new QIntValidator(-1, MAX_GADCC, this);
    address2LE->setValidator(a2Validator);
    address2Lbl->setBuddy(address2LE);

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
    decoderGBL->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));

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
    busLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));
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
    feedbackLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));
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
    moduleLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));
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
    portLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));
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
    mbLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));
    buttFBmodules = new QPushButton(tr("&FB"), feedbackGB);
    mbLayout->addWidget(buttFBmodules);
    buttFBmodules->setPixmap(QPixmap(viewfeedback_xpm));
    connect(buttFBmodules, SIGNAL(clicked()), this,
            SLOT(slotShowFBmodules()));
    QToolTip::add(buttFBmodules, tr("Show feedback module window"));

    /*spacer to push contents of box to top */
    feedbackGBL->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));

    
    /*layout with OK and Cancel buttons*/
    QBoxLayout* buttonLayout = new QHBoxLayout(baseLayout, 6);
    buttonLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));

    /*button line at bottom*/
    buttOK = new QPushButton(tr("OK"), this);
    buttOK->setDefault(true);
    connect(buttOK, SIGNAL(clicked()), this, SLOT(accept()));
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
    
    if (classid == element::siciRbr || classid == element::siciRbl ||
            classid == element::siciSbr || classid == element::siciSbl) {
        a1Validator->setTop(MAX_RB);
        a2Validator->setTop(MAX_RB);
    }
    else if (classid == element::siciAdr) {
        a1Validator->setTop(MAX_DISP);
        a2Validator->setTop(MAX_DISP);
        //a1Validator->setBottom(MAX_RB);
        //a2Validator->setBottom(MAX_RB);
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
                a1Validator->setTop(MAX_GASX);
                a2Validator->setTop(MAX_GASX);
                
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
    QString sListText;
    
    // show protocol data => element->hasAddress() or isSwitchable()
    enabled =
        classid == element::siciHsr || classid == element::siciHsl ||
        classid == element::siciHssr || classid == element::siciHssl ||
        classid == element::siciSsr || classid == element::siciSsl ||
        classid == element::siciWsr || classid == element::siciWsl ||
        classid == element::siciShr || classid == element::siciShl ||
        classid == element::siciSdr || classid == element::siciSdl ||
        classid == element::siciSrt || classid == element::siciSrb ||
        classid == element::siciSlt || classid == element::siciSlb ||
        classid == element::siciDbl || classid == element::siciDbr ||
        classid == element::siciDtl || classid == element::siciDtr ||
        classid == element::siciEkl || classid == element::siciEkr ||
        classid == element::siciDkl || classid == element::siciDkr ||
        classid == element::siciTwr || classid == element::siciTwl ||
        classid == element::siciEnk || classid == element::siciRel ||
        classid == element::siciMdc ||
        classid == element::siciSyr || classid == element::siciSyl ||
        classid == element::siciSbn || classid == element::siciBld ||
        classid == element::siciRbr || classid == element::siciRbl ||
        classid == element::siciSbr || classid == element::siciSbl ||
        classid == element::siciZpr || classid == element::siciZpl ||
        classid == element::siciVsr || classid == element::siciVsl;

    rbProtocol_MS->setEnabled(enabled || classid == element::siciDre);
    rbProtocol_NA->setEnabled(enabled);
    rbProtocol_PS->setEnabled(enabled);
    rbProtocol_SE->setEnabled(enabled);

    // show address_1 data, but take enabled value from above
    // TODO: include items with virtual addresses
    // exclude turntable and transfer table
    enabled = enabled && classid != element::siciSbn &&
        classid != element::siciMdc;

    if (enabled || classid == element::siciAdr) {
        // set direction to 0 if it was -1 before and address_1 is now enabled
        if (gaDirection == -1)
            gaDirection = 0;
        addresscount = 1;
    }
    else {
        address1LE->setText("-1");
        addresscount = 0;
    }

    srcpBus1Label->setEnabled(enabled || classid == element::siciAdr);
    srcpBus1LE->setEnabled(enabled || classid == element::siciAdr);
    address1LE->setEnabled(enabled || classid == element::siciAdr);
    cbAddrLabeling->setEnabled(enabled);
    address1Lbl->setEnabled(enabled || classid == element::siciAdr);
    port1Label->setEnabled(rbProtocol_SE->isChecked());
    port1SB->setEnabled(rbProtocol_SE->isChecked());
    xchConn1CB->setEnabled(enabled);

    // show text data
    enabled = 
        classid == element::siciHsr || classid == element::siciHsl ||
        classid == element::siciHssr || classid == element::siciHssl ||
        classid == element::siciWsr || classid == element::siciWsl ||
        classid == element::siciVsr || classid == element::siciVsl ||
        classid == element::siciSrt || classid == element::siciSrb ||
        classid == element::siciSlt || classid == element::siciSlb ||
        classid == element::siciDbl || classid == element::siciDbr ||
        classid == element::siciDtl || classid == element::siciDtr ||
        classid == element::siciEkl || classid == element::siciEkr ||
        classid == element::siciDkl || classid == element::siciDkr ||
        classid == element::siciTwr || classid == element::siciTwl ||
        classid == element::siciSsr || classid == element::siciSsl ||
        classid == element::siciShr || classid == element::siciShl ||
        classid == element::siciSdr || classid == element::siciSdl ||
        classid == element::siciEnk || classid == element::siciRel ||
        classid == element::siciGer || classid == element::siciLee ||
        classid == element::siciRbr || classid == element::siciRbl ||
        classid == element::siciSbr || classid == element::siciSbl ||
        classid == element::siciMdc ||
        classid == element::siciSyr || classid == element::siciSyl ||
        classid == element::siciSbn || classid == element::siciAdr ||
        classid == element::siciTdr || classid == element::siciTdl ||
        classid == element::siciTdb || classid == element::siciDre ||
        classid == element::siciZpr || classid == element::siciZpl ||
        classid == element::siciBld;

    if (!enabled)
        leText->setText("-1");

    leText->setEnabled(enabled);
    labelText->setEnabled(enabled);

    // show address_2 data
    // Hp0+Hp1+Hp2
    enabled =
        classid == element::siciHssr || classid == element::siciHssl ||
        classid == element::siciTwr || classid == element::siciTwl ||
        classid == element::siciDkl || classid == element::siciDkr || 
        classid == element::siciEkl || classid == element::siciEkr || 
        classid == element::siciMdc || classid == element::siciDre || 
        classid == element::siciSbn || 
        (classid == element::siciVsr && gaSubType == 4) || 
        (classid == element::siciVsl && gaSubType == 4) || 
        (classid == element::siciHsr && gaSubType == 4) || 
        (classid == element::siciHsl && gaSubType == 4);
    
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

    // show rotate data
    // element::siciGer: only for text placement
    enabled =
        classid == element::siciEkl || classid == element::siciEkr ||
        classid == element::siciGer ||
        classid == element::siciTul || classid == element::siciTur ||
        classid == element::siciSho || classid == element::siciShu;

    cbRotate->setEnabled(enabled);

    // show LEDoff data (Gleismelder)
    enabled =
        classid == element::siciClt || classid == element::siciCrb ||
        classid == element::siciCrt || classid == element::siciClb ||
        classid == element::siciDil || classid == element::siciDir ||
        classid == element::siciGer || classid == element::siciTrv ||
        classid == element::siciTdr || classid == element::siciTdl ||
        classid == element::siciTdb ||
        classid == element::siciRbr || classid == element::siciRbl ||
        classid == element::siciSbr || classid == element::siciSbl ||
        classid == element::siciKrh ||
        classid == element::siciKrl || classid == element::siciKrr ||
        classid == element::siciSsr || classid == element::siciSsl ||
        classid == element::siciShr || classid == element::siciShl ||
        classid == element::siciSdr || classid == element::siciSdl ||
        classid == element::siciTtl || classid == element::siciTtr ||
        classid == element::siciTbl || classid == element::siciTbr ||
        classid == element::siciSrt || classid == element::siciSrb ||
        classid == element::siciSlt || classid == element::siciSlb ||
        classid == element::siciSyr || classid == element::siciSyl ||
        classid == element::siciTwr || classid == element::siciTwl ||
        classid == element::siciDbl || classid == element::siciDbr ||
        classid == element::siciDtl || classid == element::siciDtr ||
        classid == element::siciEkl || classid == element::siciEkr ||
        classid == element::siciDkl || classid == element::siciDkr ||
        classid == element::siciEnk || classid == element::siciBld;

    cbLEDoff->setEnabled(enabled);

    // show invert data for empty elements only for colour
    enabled =
        classid == element::siciSrt || classid == element::siciSrb ||
        classid == element::siciSlt || classid == element::siciSlb ||
        classid == element::siciEkl || classid == element::siciEkr ||
        classid == element::siciDbl || classid == element::siciDbr ||
        classid == element::siciDtl || classid == element::siciDtr ||
        classid == element::siciLee || classid == element::siciAdr;

    cbInvert->setEnabled(enabled);
    invertedChanged(cbInvert->isChecked());

    // element gets feedback messages
    enabled =
        classid == element::siciHsr || classid == element::siciHsl ||
        classid == element::siciHssr || classid == element::siciHssl ||
        classid == element::siciShr || classid == element::siciShl ||
        classid == element::siciSdr || classid == element::siciSdl ||
        classid == element::siciBue || classid == element::siciAdr ||
        classid == element::siciVsr || classid == element::siciVsl ||
        classid == element::siciWsr || classid == element::siciWsl ||
        classid == element::siciSsr || classid == element::siciSsl || 
        classid == element::siciZpr || classid == element::siciZpl ||
        ((classid == element::siciClt || classid == element::siciCrb ||
          classid == element::siciCrt || classid == element::siciClb || 
          classid == element::siciEnk || classid == element::siciBld || 
          classid == element::siciEkl || classid == element::siciEkr ||
          classid == element::siciDkl || classid == element::siciDkr ||
          classid == element::siciTwr || classid == element::siciTwl ||
          classid == element::siciSyr || classid == element::siciSyl ||
          classid == element::siciSrt || classid == element::siciSrb ||
          classid == element::siciSlt || classid == element::siciSlb ||
          classid == element::siciDbl || classid == element::siciDbr ||
          classid == element::siciDtl || classid == element::siciDtr ||
          classid == element::siciDil || classid == element::siciDir ||
          classid == element::siciGer || classid == element::siciTdr ||
          classid == element::siciTdl || classid == element::siciTrv ||
          classid == element::siciTtl || classid == element::siciTtr ||
          classid == element::siciTbl || classid == element::siciTbr ||
          classid == element::siciRbr || classid == element::siciRbl ||
          classid == element::siciSbr || classid == element::siciSbl ||
          classid == element::siciTdb || classid == element::siciKrh ||
          classid == element::siciKrl || classid == element::siciKrr) &&
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
        if (classid == element::siciRbr || classid == element::siciRbl ||
                classid == element::siciSbr || classid == element::siciSbl)
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
        classid == element::siciHsr || classid == element::siciHsl ||
        classid == element::siciHssr || classid == element::siciHssl ||
        classid == element::siciVsr || classid == element::siciVsl ||
        classid == element::siciWsr || classid == element::siciWsl ||
        classid == element::siciSrt || classid == element::siciSrb ||
        classid == element::siciSlt || classid == element::siciSlb ||
        classid == element::siciDbl || classid == element::siciDbr ||
        classid == element::siciDtl || classid == element::siciDtr ||
        classid == element::siciEkl || classid == element::siciEkr ||
        classid == element::siciDkl || classid == element::siciDkr ||
        classid == element::siciTwr || classid == element::siciTwl ||
        classid == element::siciSsr || classid == element::siciSsl ||
        classid == element::siciShr || classid == element::siciShl ||
        classid == element::siciSdr || classid == element::siciSdl ||
        classid == element::siciEnk || classid == element::siciRel ||
        classid == element::siciDre ||
        classid == element::siciSyr || classid == element::siciSyl ||
        classid == element::siciSbn || classid == element::siciMdc ||
        classid == element::siciBld ||
        classid == element::siciZpr || classid == element::siciZpl;

    activeTimeSB->setEnabled(enabled);
    labelTime->setEnabled(enabled);

    // show decoder data
    enabled =
        classid == element::siciHsr || classid == element::siciHsl ||
        classid == element::siciHssr || classid == element::siciHssl ||
        classid == element::siciVsr || classid == element::siciVsl ||
        classid == element::siciWsr || classid == element::siciWsl ||
        classid == element::siciSrt || classid == element::siciSrb ||
        classid == element::siciSlt || classid == element::siciSlb ||
        classid == element::siciDbl || classid == element::siciDbr ||
        classid == element::siciDtl || classid == element::siciDtr ||
        classid == element::siciEkl || classid == element::siciEkr ||
        classid == element::siciDkl || classid == element::siciDkr ||
        classid == element::siciTwr || classid == element::siciTwl ||
        classid == element::siciSsr || classid == element::siciSsl ||
        classid == element::siciShr || classid == element::siciShl ||
        classid == element::siciSdr || classid == element::siciSdl ||
        classid == element::siciEnk || classid == element::siciRel ||
        classid == element::siciSyr || classid == element::siciSyl ||
        classid == element::siciSbn || classid == element::siciMdc ||
        classid == element::siciBld ||
        classid == element::siciZpr || classid == element::siciZpl;

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
        classid == element::siciHsr || classid == element::siciHsl ||
        classid == element::siciHssr || classid == element::siciHssl ||
        classid == element::siciDkl || classid == element::siciDkr ||
        classid == element::siciEnk || classid == element::siciDre ||
        classid == element::siciVsr || classid == element::siciVsl;

    if (!enabled)
        gaSubType = -1;

    showSubTypes(enabled);
    updateValidators();
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
    if (classid == element::siciHsr || classid == element::siciHsl ||
            classid == element::siciHssr || classid == element::siciHssl ||
            classid == element::siciVsr || classid == element::siciVsl) {
        if (classid == element::siciHsr || classid == element::siciHsl){
            buttSubType[0]->setPixmap(QPixmap(signal_hs_st1_xpm));
            buttSubType[1]->setPixmap(QPixmap(signal_hs_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(signal_hs_st3_xpm));
        }
        else if (classid == element::siciHssr || classid == element::siciHssl){
            buttSubType[0]->setPixmap(QPixmap(signal_hss_st1_xpm));
            buttSubType[1]->setPixmap(QPixmap(signal_hss_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(signal_hss_st3_xpm));
        }
        else if (classid == element::siciVsr || classid == element::siciVsl){
            buttSubType[0]->setPixmap(QPixmap(signal_vs_st1_xpm));
            buttSubType[1]->setPixmap(QPixmap(signal_vs_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(signal_vs_st3_xpm));
        }
    }
    
    else if (classid == element::siciDkl || classid == element::siciDkr) {
        if (classid == element::siciDkl){
            buttSubType[1]->setPixmap(QPixmap(dkw_links_st2_xpm));
            buttSubType[2]->setPixmap(QPixmap(dkw_links_st3_xpm));
        }
        else if (classid == element::siciDkr){
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


    // activate the subtype dependant button
    if (classid == element::siciHsr || classid == element::siciHsl ||
            classid == element::siciHssr || classid == element::siciHssl ||
            classid == element::siciVsr || classid == element::siciVsl) {
        if (classid == element::siciHsr || classid == element::siciHsl) {
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
        if (classid == element::siciHssr || classid == element::siciHssl) {
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
        if (classid == element::siciVsr || classid == element::siciVsl) {
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

    if (classid == element::siciDkl || classid == element::siciDkr) {
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
            if (classid == element::siciHsr || classid == element::siciHsl ||
                    classid == element::siciVsr || classid == element::siciVsl) {
                srcpBus2Label->setEnabled(false);
                srcpBus2LE->setEnabled(false);
                address2LE->setEnabled(false);
                address2Lbl->setEnabled(false);
                port2Label->setEnabled(false);
                port2SB->setEnabled(false);
                xchConn2CB->setEnabled(false);

                address2LE->setText("-1");
                gaSubType = 0;
            }

            else if (classid == element::siciHssr ||
                    classid == element::siciHssl) {
                gaSubType = 1;
            }

            else if (classid == element::siciEnk) {
                srcpBus2Label->setEnabled(false);
                srcpBus2LE->setEnabled(false);
                address2LE->setEnabled(false);
                address2Lbl->setEnabled(false);
                port2Label->setEnabled(false);
                port2SB->setEnabled(false);
                xchConn2CB->setEnabled(false);

                gaSubType = -1;
            }
            break;

        case 1:                    // == subType 2
            if (classid == element::siciHsr || classid == element::siciHsl ||
                    classid == element::siciVsr || classid == element::siciVsl) {
                srcpBus2Label->setEnabled(false);
                srcpBus2LE->setEnabled(false);
                address2LE->setEnabled(false);
                address2Lbl->setEnabled(false);
                port2Label->setEnabled(false);
                port2SB->setEnabled(false);
                xchConn2CB->setEnabled(false);
                address2LE->setText("-1");
                gaSubType = 6;
            }

            else if (classid == element::siciDkl || classid == element::siciDkr) {
                srcpBus2Label->setEnabled(false);
                srcpBus2LE->setEnabled(false);
                address2LE->setEnabled(false);
                address2Lbl->setEnabled(false);
                port2Label->setEnabled(false);
                port2SB->setEnabled(false);
                xchConn2CB->setEnabled(false);
                gaSubType = 0;
            }

            else if (classid == element::siciHssr ||
                    classid == element::siciHssl)
                gaSubType = 7;

            else if (classid == element::siciEnk)
                gaSubType = 0;

            else if (classid == element::siciDre)
                address2LE->setText("240");

            break;

        case 2:                    // == subType 3
            if (classid == element::siciHsr || classid == element::siciHsl ||
                    classid == element::siciVsr || classid == element::siciVsl) {
                srcpBus2Label->setEnabled(true);
                srcpBus2LE->setEnabled(true);
                address2LE->setEnabled(true);
                address2Lbl->setEnabled(true);
                port2Label->setEnabled(true);
                port2SB->setEnabled(true);
                xchConn2CB->setEnabled(true);
                gaSubType = 4;
            }

            else if (classid == element::siciDkl || classid == element::siciDkr) {
                srcpBus2Label->setEnabled(true);
                srcpBus2LE->setEnabled(true);
                address2LE->setEnabled(true);
                address2Lbl->setEnabled(true);
                port2Label->setEnabled(true);
                port2SB->setEnabled(true);
                xchConn2CB->setEnabled(true);
                gaSubType = 1;
            }

            else if (classid == element::siciHssr ||
                    classid == element::siciHssl)
                gaSubType = 5;

            else if (classid == element::siciEnk)
                gaSubType = 1;

            else if (classid == element::siciDre)
                address2LE->setText("224");

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
    else if (classid == element::siciHssr || classid == element::siciHssl) {
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
    else if (classid == element::siciHsr || classid == element::siciHsl ||
            classid == element::siciVsr || classid == element::siciVsl) {
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
     * for element::siciDkl and element::siciDkr:
     *
     * gaSubType   pressed button
     * --------------------------
     *     n.d.         0
     *     0            1
     *     1            2
     * --------------------------
    */
    else if (classid == element::siciDkl || classid == element::siciDkr) {
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


int ElementDialog::getRotated()
{
    return cbRotate->isEnabled() ? (cbRotate->isChecked()? 1 : 0) : -1;
}


void ElementDialog::setRotated(int rotated)
{
    if (rotated == -1)
        cbRotate->setChecked(false);
    else
        cbRotate->setChecked(rotated);
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
    }
}
