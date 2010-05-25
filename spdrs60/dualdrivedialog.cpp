/***************************************************************************
                           dualdrivedialog.cpp
                           -------------------------------
    copyright            : (C) 2010 Guido Scholz
    e-mail               : guido.scholz@bayernline.de
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
 This file provides an user interface to change drive confuguration
 ***************************************************************************/

#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>

#include "dualdrivedialog.h"
#include "preferences.h"
#include "resources.h"
#include "srcpmessage.h"

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



DualDriveDialog::DualDriveDialog(QWidget* parent):
    QDialog(parent, "DualDriveDialog", true)
{
    setCaption(tr("Edit drives"));

    /*Layout to separate OK Cancel buttons from the upper rest*/
    QBoxLayout* baseLayout = new QVBoxLayout(this, 10, 10);

    /* 1. decoder*/
    /*Layout to separate left and right groupboxes*/
    QBoxLayout* leftRight1Layout = new QHBoxLayout(baseLayout, 10);

    /*left groupbox with protocol data*/
    protocol1BG = new QButtonGroup(4, Qt::Vertical,
                        tr("Protocol 1"), this, "protocol1BG");
    leftRight1Layout->addWidget(protocol1BG);
    protocol1BG->setExclusive(true);
    rbProtocol1_MS = new QRadioButton(tr("&Maerklin/Motorola"), protocol1BG);
    rbProtocol1_NA = new QRadioButton(tr("&NMRA/DCC"), protocol1BG);
    rbProtocol1_SE = new QRadioButton(tr("Selectri&x"), protocol1BG);
    rbProtocol1_PS = new QRadioButton(tr("Protocol by Ser&ver"), protocol1BG);
    connect(protocol1BG, SIGNAL(clicked(int)),
            this, SLOT(slotProtocol1Changed(int)));

    /*right decoder data group box*/
    QGroupBox* decoder1GB = new QGroupBox(0, Qt::Horizontal,
            tr("Decoder 1"), this, "decoder1GB");
    leftRight1Layout->addWidget(decoder1GB);
    QVBoxLayout* decoder1GBL = new QVBoxLayout(decoder1GB->layout(), 6);
    
    /*line with resettime */
    QHBoxLayout* reset1Layout = new QHBoxLayout(decoder1GBL, 6);
    QLabel* labelTime = new QLabel(tr("Reset &after (ms):"), decoder1GB);
    reset1Layout->addWidget(labelTime);
    reset1Layout->addStretch();

    activeTime1SB = new QSpinBox(50, 2000, 50, decoder1GB, "");
    activeTime1SB->setWrapping(true);
    reset1Layout->addWidget(activeTime1SB);
    labelTime->setBuddy(activeTime1SB);

    QGridLayout* dec1dataLayout = new QGridLayout(decoder1GBL, 6, 3, 10,
            "dec1dataLayout");
    
    /*line with srcp bus 1 */
    QLabel* srcpBus1Label = new QLabel(tr("S&RCP-Bus:"), decoder1GB);
    dec1dataLayout->addWidget(srcpBus1Label, 0, 0);
    srcpBus1LE = new QLineEdit(decoder1GB, "srcpBus1LE");
    srcpBus1LE->setMaxLength(4);
    srcpBus1LE->setMaximumWidth(LEMAXWIDTH);
    dec1dataLayout->addWidget(srcpBus1LE, 0, 1);
    srcpBus1Label->setBuddy(srcpBus1LE);
    
    /*line with address 1 */
    QLabel* address1Lbl = new QLabel(tr("A&ddress:"), decoder1GB);
    dec1dataLayout->addWidget(address1Lbl, 1, 0);
    address1LE = new QLineEdit(decoder1GB, "address_1");
    dec1dataLayout->addWidget(address1LE, 1, 1);
    address1LE->setMaxLength(4);       // address length of NA protocol
    address1LE->setMaximumWidth(LEMAXWIDTH);
    addressVdt1 = new QIntValidator(0, MAX_GADCC, this);
    address1LE->setValidator(addressVdt1);
    address1Lbl->setBuddy(address1LE);

    /*line with port 1 spinbox */
    port1Label = new QLabel(tr("&Port:"), decoder1GB);
    dec1dataLayout->addWidget(port1Label, 2, 0);

    port1SB = new QSpinBox(0, 1, 1, decoder1GB, "port1SB");
    port1SB->setWrapping(true);
    dec1dataLayout->addWidget(port1SB, 2, 1);
    port1Label->setBuddy(port1SB);

    xchConn1CB = new QCheckBox(tr("&Exch. conn."), decoder1GB, "xch1");
    dec1dataLayout->addWidget(xchConn1CB, 2, 2);

    /*spacer to push contents of box to top */
    decoder1GBL->addStretch();


    /* 2. decoder*/
    /*Layout to separate left and right groupboxes*/
    QBoxLayout* leftRight2Layout = new QHBoxLayout(baseLayout, 10);

    /*left groupbox with protocol data*/
    protocol2BG = new QButtonGroup(4, Qt::Vertical,
                        tr("Protocol 2"), this, "protocol2BG");
    leftRight2Layout->addWidget(protocol2BG);
    protocol2BG->setExclusive(true);
    rbProtocol2_MS = new QRadioButton(tr("Maer&klin/Motorola"), protocol2BG);
    rbProtocol2_NA = new QRadioButton(tr("NMRA/D&CC"), protocol2BG);
    rbProtocol2_SE = new QRadioButton(tr("Selec&trix"), protocol2BG);
    rbProtocol2_PS = new QRadioButton(tr("Protocol b&y Server"), protocol2BG);
    connect(protocol2BG, SIGNAL(clicked(int)),
            this, SLOT(slotProtocol2Changed(int)));

    /*right decoder data group box*/
    QGroupBox* decoder2GB = new QGroupBox(0, Qt::Horizontal,
            tr("Decoder 2"), this, "decoder2GB");
    leftRight2Layout->addWidget(decoder2GB);
    QVBoxLayout* decoder2GBL = new QVBoxLayout(decoder2GB->layout(), 6);
    
    /*line with resettime */
    QHBoxLayout* reset2Layout = new QHBoxLayout(decoder2GBL, 6);
    labelTime = new QLabel(tr("Reset a&fter (ms):"), decoder2GB);
    reset2Layout->addWidget(labelTime);
    reset2Layout->addStretch();

    activeTime2SB = new QSpinBox(50, 2000, 50, decoder2GB, "");
    activeTime2SB->setWrapping(true);
    reset2Layout->addWidget(activeTime2SB);
    labelTime->setBuddy(activeTime2SB);

    QGridLayout* dec2dataLayout = new QGridLayout(decoder2GBL, 6, 3, 10,
            "dec2dataLayout");
    
    /*line with srcp bus 2 */
    QLabel* srcpBus2Label = new QLabel(tr("SRCP-&Bus:"), decoder2GB);
    dec2dataLayout->addWidget(srcpBus2Label, 0, 0);
    srcpBus2LE = new QLineEdit(decoder2GB, "srcpBus2LE");
    srcpBus2LE->setMaxLength(4);
    srcpBus2LE->setMaximumWidth(LEMAXWIDTH);
    dec2dataLayout->addWidget(srcpBus2LE, 0, 1);
    srcpBus2Label->setBuddy(srcpBus2LE);
    
    /*line with address 2 */
    QLabel* address2Lbl = new QLabel(tr("Addre&ss:"), decoder2GB);
    dec2dataLayout->addWidget(address2Lbl, 1, 0);
    address2LE = new QLineEdit(decoder2GB, "address_2");
    dec2dataLayout->addWidget(address2LE, 1, 1);
    address2LE->setMaxLength(4);       // address length of NA protocol
    address2LE->setMaximumWidth(LEMAXWIDTH);
    addressVdt2 = new QIntValidator(0, MAX_GADCC, this);
    address2LE->setValidator(addressVdt2);
    address2Lbl->setBuddy(address2LE);

    /*line with port 2 spinbox */
    port2Label = new QLabel(tr("P&ort:"), decoder2GB);
    dec2dataLayout->addWidget(port2Label, 2, 0);

    port2SB = new QSpinBox(0, 1, 1, decoder2GB, "port2SB");
    port2SB->setWrapping(true);
    dec2dataLayout->addWidget(port2SB, 2, 1);
    port2Label->setBuddy(port2SB);

    xchConn2CB = new QCheckBox(tr("E&xch. conn."), decoder2GB, "xch2");
    dec2dataLayout->addWidget(xchConn2CB, 2, 2);

    /*spacer to push contents of box to top */
    decoder2GBL->addStretch();


    /*layout with OK and Cancel buttons*/
    QBoxLayout* buttonLayout = new QHBoxLayout(baseLayout, 6);
    buttonLayout->addStretch();

    /*button line at bottom*/
    QPushButton* okButton = new QPushButton(tr("OK"), this);
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked()), this, SLOT(validate()));
    buttonLayout->addWidget(okButton);

    QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    buttonLayout->addWidget(cancelButton);
}


int DualDriveDialog::getProtocol1()
{
    if (!rbProtocol1_MS->isEnabled())
        return SrcpMessage::proNone;
    else if (rbProtocol1_MS->isChecked())
        return SrcpMessage::proMM;
    else if (rbProtocol1_NA->isChecked())
        return SrcpMessage::proDCC;
    else if (rbProtocol1_SE->isChecked())
        return SrcpMessage::proSelectrix;
    else
        return SrcpMessage::proServer;
}


int DualDriveDialog::getProtocol2()
{
    if (!rbProtocol2_MS->isEnabled())
        return SrcpMessage::proNone;
    else if (rbProtocol2_MS->isChecked())
        return SrcpMessage::proMM;
    else if (rbProtocol2_NA->isChecked())
        return SrcpMessage::proDCC;
    else if (rbProtocol2_SE->isChecked())
        return SrcpMessage::proSelectrix;
    else
        return SrcpMessage::proServer;
}


void DualDriveDialog::setProtocol1(int protocol)
{
    if (protocol == SrcpMessage::proNone) {
        rbProtocol1_MS->setEnabled(false);
        rbProtocol1_NA->setEnabled(false);
        rbProtocol1_PS->setEnabled(false);
        rbProtocol1_SE->setEnabled(false);
    }
    else {
        if (protocol == SrcpMessage::proMM)
            rbProtocol1_MS->setChecked(true);
        else if (protocol == SrcpMessage::proDCC)
            rbProtocol1_NA->setChecked(true);
        else if (protocol == SrcpMessage::proSelectrix)
            rbProtocol1_SE->setChecked(true);
        else
            rbProtocol1_PS->setChecked(true);
    }
    updateValidator1();
}


void DualDriveDialog::setProtocol2(int protocol)
{
    if (protocol == SrcpMessage::proNone) {
        rbProtocol2_MS->setEnabled(false);
        rbProtocol2_NA->setEnabled(false);
        rbProtocol2_PS->setEnabled(false);
        rbProtocol2_SE->setEnabled(false);
    }
    else {
        if (protocol == SrcpMessage::proMM)
            rbProtocol2_MS->setChecked(true);
        else if (protocol == SrcpMessage::proDCC)
            rbProtocol2_NA->setChecked(true);
        else if (protocol == SrcpMessage::proSelectrix)
            rbProtocol2_SE->setChecked(true);
        else
            rbProtocol2_PS->setChecked(true);
    }
    updateValidator2();
}


void DualDriveDialog::setActiveTime1(int atime)
{
    if (atime != -1)
        activeTime1SB->setValue(atime);
    else
        activeTime1SB->setValue(pref.activetime);
}


void DualDriveDialog::setActiveTime2(int atime)
{
    if (atime != -1)
        activeTime2SB->setValue(atime);
    else
        activeTime2SB->setValue(pref.activetime);
}


int DualDriveDialog::getActiveTime1()
{
    return activeTime1SB->value();
}


int DualDriveDialog::getActiveTime2()
{
    return activeTime2SB->value();
}


int DualDriveDialog::getSRCPBus1()
{
    return srcpBus1LE->text().toInt();
}


int DualDriveDialog::getSRCPBus2()
{
    return srcpBus2LE->text().toInt();
}


void DualDriveDialog::setSRCPBus1(int bus)
{
    srcpBus1LE->setText(QString::number(bus));
}


void DualDriveDialog::setSRCPBus2(int bus)
{
    srcpBus2LE->setText(QString::number(bus));
}


int DualDriveDialog::getAddress1()
{
    return address1LE->text().toInt();
}


int DualDriveDialog::getAddress2()
{
    return address2LE->text().toInt();
}


void DualDriveDialog::setAddress1(int addr)
{
    address1LE->setText(QString::number(addr));
}


void DualDriveDialog::setAddress2(int addr)
{
    address2LE->setText(QString::number(addr));
}


void DualDriveDialog::setPort1(int aport)
{
    port1SB->setValue(aport);
}


void DualDriveDialog::setPort2(int aport)
{
    port2SB->setValue(aport);
}


int DualDriveDialog::getPort1()
{
    return port1SB->value();
}


int DualDriveDialog::getPort2()
{
    return port2SB->value();
}


int DualDriveDialog::getXChangeConn1()
{
    return xchConn1CB->isEnabled() ?
        (xchConn1CB->isChecked() ? 1 : 0) : -1;
}


int DualDriveDialog::getXChangeConn2()
{
    return xchConn2CB->isEnabled() ?
        (xchConn2CB->isChecked() ? 1 : 0) : -1;
}


void DualDriveDialog::setXChangeConn1(int xch)
{
    if (xch == -1)
        xchConn1CB->setChecked(false);
    else
        xchConn1CB->setChecked(xch);
}

void DualDriveDialog::setXChangeConn2(int xch)
{
    if (xch == -1)
        xchConn2CB->setChecked(false);
    else
        xchConn2CB->setChecked(xch);
}

void DualDriveDialog::slotProtocol1Changed(int)
{
    updateValidator1();
}


void DualDriveDialog::slotProtocol2Changed(int)
{
    updateValidator2();
}


void DualDriveDialog::updateValidator1()
{
    /*
     *  id  protocol
     *  -------------
     *  -1  none
     *   0  MM
     *   1  DCC
     *   2  Selectrix
     *   3  Server
     *  -------------
     */

#if QT_VERSION >= 0x030300
    int prot = protocol1BG->selectedId();
#else
    int prot = protocol1BG->id(protocol1BG->selected());
#endif

    switch (prot) {
        case 0:
            // MM
            addressVdt1->setTop(MAX_GAMM);
            updateAddressTooltip1();

            port1Label->setEnabled(false);
            port1SB->setEnabled(false);
            // new range for port spinboxes
            port1SB->setMinValue(MINMMPORT);
            port1SB->setMaxValue(MAXMMPORT);

            xchConn1CB->setEnabled(true);

            if (address1LE->text().toInt() > MAX_GAMM)
                address1LE->setText(QString::number(MAX_GAMM));

            break;

        case 1:
            //DCC
            addressVdt1->setTop(MAX_GADCC);
            updateAddressTooltip1();

            port1Label->setEnabled(false);
            port1SB->setEnabled(false);
            // new range for port spinboxes
            port1SB->setMinValue(MINDCCPORT);
            port1SB->setMaxValue(MAXDCCPORT);

            xchConn1CB->setEnabled(true);

            break;

        case 2:
            //Selectrix
            addressVdt1->setTop(MAX_GASX);
            updateAddressTooltip1();

            // new range for port spinboxes
            port1SB->setMinValue(MINSXPORT);
            port1SB->setMaxValue(MAXSXPORT);

            //xchConn1CB->setEnabled(false);

            port1Label->setEnabled(true);
            port1SB->setEnabled(true);

            break;

        case 3:
            // Server
            // MAGIC: Validator limits for Protocol by Server
            addressVdt1->setTop(9999);
            updateAddressTooltip1();

            // new range for port spinboxes
            port1SB->setMinValue(MINSVPORT);
            port1SB->setMaxValue(MAXSVPORT);

            //xchConn1CB->setEnabled(false);

            port1Label->setEnabled(true);
            port1SB->setEnabled(true);

            break;

        default:
            // undefined protocol
            break;
    }
}

void DualDriveDialog::updateValidator2()
{
    /*
     *  id  protocol
     *  -------------
     *  -1  none
     *   0  MM
     *   1  DCC
     *   2  Selectrix
     *   3  Server
     *  -------------
     */

#if QT_VERSION >= 0x030300
    int prot = protocol2BG->selectedId();
#else
    int prot = protocol2BG->id(protocol2BG->selected());
#endif

    switch (prot) {
        case 0:
            // MM
            addressVdt2->setTop(MAX_GAMM);
            updateAddressTooltip2();

            port2Label->setEnabled(false);
            port2SB->setEnabled(false);
            // new range for port spinboxes
            port2SB->setMinValue(MINMMPORT);
            port2SB->setMaxValue(MAXMMPORT);

            xchConn2CB->setEnabled(true);

            if (address2LE->text().toInt() > MAX_GAMM)
                address2LE->setText(QString::number(MAX_GAMM));

            break;

        case 1:
            //DCC
            addressVdt2->setTop(MAX_GADCC);
            updateAddressTooltip2();

            port2Label->setEnabled(false);
            port2SB->setEnabled(false);
            // new range for port spinboxes
            port2SB->setMinValue(MINDCCPORT);
            port2SB->setMaxValue(MAXDCCPORT);

            xchConn2CB->setEnabled(true);

            break;

        case 2:
            //Selectrix
            addressVdt2->setTop(MAX_GASX);
            updateAddressTooltip2();

            // new range for port spinboxes
            port2SB->setMinValue(MINSXPORT);
            port2SB->setMaxValue(MAXSXPORT);

            //xchConn2CB->setEnabled(false);

            port2Label->setEnabled(true);
            port2SB->setEnabled(true);

            break;

        case 3:
            // Server
            // MAGIC: Validator limits for Protocol by Server
            addressVdt2->setTop(9999);
            updateAddressTooltip2();

            // new range for port spinboxes
            port2SB->setMinValue(MINSVPORT);
            port2SB->setMaxValue(MAXSVPORT);

            //xchConn2CB->setEnabled(false);

            port2Label->setEnabled(true);
            port2SB->setEnabled(true);

            break;

        default:
            // undefined protocol
            break;
    }
}

void DualDriveDialog::updateAddressTooltip1()
{
    QToolTip::add(address1LE, tr(
                "Enter decoder address.\n"
                "Valid range is %1..%2.")
            .arg(addressVdt1->bottom())
            .arg(addressVdt1->top()));
}


void DualDriveDialog::updateAddressTooltip2()
{
    QToolTip::add(address2LE, tr(
                "Enter decoder address.\n"
                "Valid range is %1..%2.")
            .arg(addressVdt2->bottom())
            .arg(addressVdt2->top()));
}


void DualDriveDialog::validate()
{
#if QT_VERSION < 0x030200
    int pos = 0;
    QString value = address1LE->text();
#endif
    if (address1LE->isEnabled() &&
#if QT_VERSION >= 0x030200
            !address1LE->hasAcceptableInput()
#else
            (addressVdt1->validate(value, pos) == QValidator::Invalid)
#endif
            ) {
        address1LE->setFocus();
        address1LE->selectAll();
        QMessageBox::warning(this, tr("Unvalid address detected"),
                tr("Value of decoder address is not valid.\n"
                    "Valid range is %1..%2.")
                .arg(addressVdt1->bottom())
                .arg(addressVdt1->top())
        , tr("OK"));
        return;
    }

#if QT_VERSION < 0x030200
    int pos = 0;
    QString value = address2LE->text();
#endif
    if (address2LE->isEnabled() &&
#if QT_VERSION >= 0x030200
            !address2LE->hasAcceptableInput()
#else
            (addressVdt2->validate(value, pos) == QValidator::Invalid)
#endif
            ) {
        address2LE->setFocus();
        address2LE->selectAll();
        QMessageBox::warning(this, tr("Unvalid address detected"),
                tr("Value of decoder address is not valid.\n"
                    "Valid range is %1..%2.")
                .arg(addressVdt2->bottom())
                .arg(addressVdt2->top())
        , tr("OK"));
        return;
    }
    accept();
}

