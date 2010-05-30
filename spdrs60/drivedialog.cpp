/***************************************************************************
                           drivedialog.cpp
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
 This file provides an user interface to change drive configuration
 ***************************************************************************/

#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>

#include "drivedialog.h"
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



DriveDialog::DriveDialog(QWidget* parent):
    QDialog(parent, "DriveDialog", true)
{
    setCaption(tr("Edit drive"));

    /*Layout to separate OK Cancel buttons from the upper rest*/
    QBoxLayout* baseLayout = new QVBoxLayout(this, 12, 12);

    /*Layout to separate left and right groupboxes*/
    QBoxLayout* leftRightLayout = new QHBoxLayout(baseLayout, 12);

    /*left groupbox with protocol data*/
    protocolBG = new QButtonGroup(4, Qt::Vertical,
                        tr("Protocol"), this, "protocolBG");
    leftRightLayout->addWidget(protocolBG);
    protocolBG->setExclusive(true);
    new QRadioButton(tr("&Maerklin/Motorola"), protocolBG);
    new QRadioButton(tr("&NMRA/DCC"), protocolBG);
    new QRadioButton(tr("Selectri&x"), protocolBG);
    new QRadioButton(tr("Protocol by Ser&ver"), protocolBG);
    connect(protocolBG, SIGNAL(clicked(int)),
            this, SLOT(slotProtocolChanged(int)));

    /*right decoder data group box*/
    QGroupBox* decoderGB = new QGroupBox(0, Qt::Horizontal,
            tr("Decoder"), this, "decoderGB");
    leftRightLayout->addWidget(decoderGB);
    QVBoxLayout* decoderGBL = new QVBoxLayout(decoderGB->layout(), 6);
    
    /*line with resettime */
    QHBoxLayout* resetLayout = new QHBoxLayout(decoderGBL, 6);
    QLabel* labelTime = new QLabel(tr("Reset &after (ms):"), decoderGB);
    resetLayout->addWidget(labelTime);
    resetLayout->addStretch();

    activeTimeSB = new QSpinBox(50, 2000, 50, decoderGB, "");
    activeTimeSB->setWrapping(true);
    resetLayout->addWidget(activeTimeSB);
    labelTime->setBuddy(activeTimeSB);

    QGridLayout* decdataLayout = new QGridLayout(decoderGBL, 6, 3, 10,
            "decdataLayout");
    
    /*line with srcp bus 1 */
    QLabel* srcpBus1Label = new QLabel(tr("S&RCP-Bus:"), decoderGB);
    decdataLayout->addWidget(srcpBus1Label, 0, 0);
    srcpBus1LE = new QLineEdit(decoderGB, "srcpBus1LE");
    srcpBus1LE->setMaxLength(4);
    srcpBus1LE->setMaximumWidth(LEMAXWIDTH);
    decdataLayout->addWidget(srcpBus1LE, 0, 1);
    srcpBus1Label->setBuddy(srcpBus1LE);
    
    /*line with address 1 */
    QLabel* address1Lbl = new QLabel(tr("A&ddress:"), decoderGB);
    decdataLayout->addWidget(address1Lbl, 1, 0);
    address1LE = new QLineEdit(decoderGB, "address_1");
    decdataLayout->addWidget(address1LE, 1, 1);
    address1LE->setMaxLength(4);       // address length of NA protocol
    address1LE->setMaximumWidth(LEMAXWIDTH);
    addressVdt = new QIntValidator(0, MAX_GADCC, this);
    address1LE->setValidator(addressVdt);
    address1Lbl->setBuddy(address1LE);

    /*line with port 1 spinbox */
    port1Label = new QLabel(tr("&Port:"), decoderGB);
    decdataLayout->addWidget(port1Label, 2, 0);

    port1SB = new QSpinBox(0, 1, 1, decoderGB, "port1SB");
    port1SB->setWrapping(true);
    decdataLayout->addWidget(port1SB, 2, 1);
    port1Label->setBuddy(port1SB);

    xchConn1CB = new QCheckBox(tr("&Exch. conn."), decoderGB, "xch1");
    decdataLayout->addWidget(xchConn1CB, 2, 2);

    /*spacer to push contents of box to top */
    decoderGBL->addStretch();


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


int DriveDialog::getProtocol()
{
#if QT_VERSION >= 0x030300
    return protocolBG->selectedId();
#else
    return protocolBG->id(protocolBG->selected());
#endif
}


void DriveDialog::setProtocol(int protocol)
{
    if (protocol == SrcpMessage::proNone)
        protocolBG->setButton(pref.protocol);
    else 
        protocolBG->setButton(protocol);

    updateValidators();
}


void DriveDialog::setActiveTime(int atime)
{
    if (atime != -1)
        activeTimeSB->setValue(atime);
    else
        activeTimeSB->setValue(pref.activetime);
}


int DriveDialog::getActiveTime()
{
    return activeTimeSB->value();
}


int DriveDialog::getSRCPBus1()
{
    return srcpBus1LE->text().toInt();
}


void DriveDialog::setSRCPBus1(int bus)
{
    srcpBus1LE->setText(QString::number(bus));
}


int DriveDialog::getAddress1()
{
    return address1LE->text().toInt();
}


void DriveDialog::setAddress1(int addr)
{
    address1LE->setText(QString::number(addr));
}


void DriveDialog::setPort1(int aport)
{
    port1SB->setValue(aport);
}


int DriveDialog::getPort1()
{
    return port1SB->value();
}


int DriveDialog::getXChangeConn1()
{
    return xchConn1CB->isEnabled() ?
        (xchConn1CB->isChecked() ? 1 : 0) : -1;
}


void DriveDialog::setXChangeConn1(int xch)
{
    if (xch == -1)
        xchConn1CB->setChecked(false);
    else
        xchConn1CB->setChecked(xch);
}

void DriveDialog::slotProtocolChanged(int)
{
    updateValidators();
}


void DriveDialog::updateValidators()
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

#if QT_VERSION >= 0x030300
    int prot = protocolBG->selectedId();
#else
    int prot = protocolBG->id(protocolBG->selected());
#endif

    switch (prot) {
        case 0:
            // MM
            addressVdt->setTop(MAX_GAMM);
            updateAddressTooltip();

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
            addressVdt->setTop(MAX_GADCC);
            updateAddressTooltip();

            port1Label->setEnabled(false);
            port1SB->setEnabled(false);
            // new range for port spinboxes
            port1SB->setMinValue(MINDCCPORT);
            port1SB->setMaxValue(MAXDCCPORT);

            xchConn1CB->setEnabled(true);

            break;

        case 2:
            //Selectrix
            addressVdt->setTop(MAX_GASX);
            updateAddressTooltip();

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
            addressVdt->setTop(9999);
            updateAddressTooltip();

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

void DriveDialog::updateAddressTooltip()
{
    QToolTip::add(address1LE, tr(
                "Enter decoder address.\n"
                "Valid range is %1..%2.")
            .arg(addressVdt->bottom())
            .arg(addressVdt->top()));
}

void DriveDialog::validate()
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
                tr("Value of decoder address is not valid.\n"
                    "Valid range is %1..%2.")
                .arg(addressVdt->bottom())
                .arg(addressVdt->top())
        , tr("OK"));
        return;
    }
    accept();
}

