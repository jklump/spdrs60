/***************************************************************************
                           virtualaddressdialog.cpp
                           -------------------------------
    copyright            : (C) 2010 Guido Scholz
    e-mail               : guido.scholz@bayernline.de
    last modified        : $Date: 2009/11/01 20:40:38 $
                           $Revision: 1.82 $
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
 This file provides an user interface to change a virtual address
 ***************************************************************************/

#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>

#include "virtualaddressdialog.h"
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



VirtualAddressDialog::VirtualAddressDialog(QWidget* parent):
    QDialog(parent, "VirtualAddressDialog", true)
{
    setCaption(tr("Edit address"));
    classid = element::siciNone;

    /*Layout to separate OK Cancel buttons from the upper rest*/
    QBoxLayout* baseLayout = new QVBoxLayout(this, 12, 12);

    /*right decoder data group box*/
    QGroupBox* decoderGB = new QGroupBox(0, Qt::Horizontal,
            tr("Virtual address"), this, "decoderGB");
    baseLayout->addWidget(decoderGB);
    QVBoxLayout* decoderGBL = new QVBoxLayout(decoderGB->layout(), 6);
    
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


int VirtualAddressDialog::getSRCPBus1()
{
    return srcpBus1LE->text().toInt();
}


void VirtualAddressDialog::setSRCPBus1(int bus)
{
    srcpBus1LE->setText(QString::number(bus));
}


int VirtualAddressDialog::getAddress1()
{
    return address1LE->text().toInt();
}


void VirtualAddressDialog::setAddress1(int addr)
{
    address1LE->setText(QString::number(addr));
}


void VirtualAddressDialog::updateValidators()
{
    /*virtual address ranges*/    
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
}

void VirtualAddressDialog::updateAddressTooltip()
{
    QToolTip::add(address1LE, tr(
                "Enter address of decoder 1.\n"
                "Valid range is %1..%2.")
            .arg(addressVdt->bottom())
            .arg(addressVdt->top()));
}

void VirtualAddressDialog::setClassId(element::SpdrItemClassId ci) 
{
    classid = ci;
    updateValidators();
    updateAddressTooltip();
}


void VirtualAddressDialog::validate()
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
                tr("Value of address is not valid.\n"
                    "Valid range is %1..%2.")
                .arg(addressVdt->bottom())
                .arg(addressVdt->top())
        , tr("OK"));
        return;
    }
    accept();
}
