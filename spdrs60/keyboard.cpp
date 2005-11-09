/***************************************************************************
                           keyboard.cpp
                           version 0.4.7 $Revision: 1.3 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-11-09 20:54:50 $
***************************************************************************/

/*****************************************************************************
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by     *
 *   the Free Software Foundation; either version 2 of the License, or        *
 *   (at your option) any later version.                                      *
 *                                                                            *
 ******************************************************************************/
/******************************************************************************
   this code shows a window with a manual keyboard to switch solenoids
 ******************************************************************************/

#include <ctype.h>              // for isdigit()
#include <stdlib.h>             // for atoi()
#include <unistd.h>             // for write()
#include <qlayout.h>

#include "keyboard.h"

/*button icons*/
#include "pixmaps/keyb_green.xpm"
#include "pixmaps/keyb_red.xpm"

extern int SHOW_TOOLTIPS;


keyboard::keyboard(QWidget* parent): QDialog(parent, "keyboard", false)
{
    setCaption(tr("Keyboard"));

    QBoxLayout* baseLayout = new QVBoxLayout(this, 10, 10);

    /*line with SRCP-bus label and edit line*/
    QBoxLayout* busLayout = new QHBoxLayout(0, 0, 6);
    baseLayout->addLayout(busLayout);

    QLabel *busLbl = new QLabel(tr("SRCP-Bus:"), this, "busLbl");
    if (SHOW_TOOLTIPS)
        QToolTip::add(busLbl, tr("Please enter the SRCB-bus for the address"));
    busLayout->addWidget(busLbl);

    QSpacerItem* spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    busLayout->addItem(spacer);

    busLE = new QLineEdit("1", this, "busLE");
    QFontMetrics fm(busLE->font());
    int LEwidth = fm.width("8888") + 10;
    busLE->setMaxLength(4);
    busLE->setMaximumWidth(LEwidth);
    //connect(busLE, SIGNAL(textChanged(const QString &)),
    //        this, SLOT(slotAddressChanged(const QString &)));
    busLayout->addWidget(busLE);
    busLbl->setBuddy(busLE);
    
    /*line with address label and edit line*/
    QBoxLayout* addressLayout = new QHBoxLayout(0, 0, 6);
    baseLayout->addLayout(addressLayout);

    QLabel *labelAddress = new QLabel(tr("Address:"), this, "addressLbl");
    if (SHOW_TOOLTIPS)
        QToolTip::add(labelAddress,
                      tr("Please enter the address to be switched"));
    addressLayout->addWidget(labelAddress);

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    addressLayout->addItem(spacer);

    addressLE = new QLineEdit(this, "addressLE");
    addressLE->setMaxLength(4);
    addressLE->setMaximumWidth(LEwidth);
    connect(addressLE, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotAddressChanged(const QString &)));
    addressLayout->addWidget(addressLE);
    labelAddress->setBuddy(addressLE);
    
    /*line with red and green buttons*/
    QBoxLayout* buttonLayout = new QHBoxLayout(0, 0, 6);
    baseLayout->addLayout(buttonLayout);

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonLayout->addItem(spacer);

    QPushButton* greenPB = new QPushButton("", this, "greenBtn");
    greenPB->setMaximumWidth(LEwidth);
    greenPB->setPaletteBackgroundColor(QColor(0, 255, 0));
    connect(greenPB, SIGNAL(clicked()), this, SLOT(slotActivateGrn()));
    buttonLayout->addWidget(greenPB);
    greenPB->setDefault(true);
    if (SHOW_TOOLTIPS)
        QToolTip::add(greenPB,
                      tr("Press this button to activate green connector"));

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonLayout->addItem(spacer);

    QPushButton* redPB = new QPushButton("", this, "redBtn");
    redPB->setMaximumWidth(LEwidth);
    redPB->setPaletteBackgroundColor(QColor(255, 0, 0));
    connect(redPB, SIGNAL(clicked()), this, SLOT(slotActivateRed()));
    buttonLayout->addWidget(redPB);
    if (SHOW_TOOLTIPS)
        QToolTip::add(redPB,
                      tr("Press this button to activate red connector"));

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonLayout->addItem(spacer);
}


// send command for both protocols with a basic 
// limit check (4096 is the higher limit of DCC protocol)
void keyboard::slotActivateRed()
{
    unsigned int adr = addressLE->text().toUInt();
    if (adr == 0 || adr > 4096)
        // TODO show error message
        return;
    QString cs = QString("SET GA M %1 0 1 50").arg(adr);
    emit sendCommand(cs);
    cs = QString("SET GA N %1 0 1 50").arg(adr);
    emit sendCommand(cs);
}


void keyboard::slotActivateGrn()
{
    unsigned int adr = addressLE->text().toUInt();
    if (adr == 0 || adr > 4096)
        // TODO show error message
        return;
    QString cs = QString("SET GA M %1 1 1 50").arg(adr);
    emit sendCommand(cs);
    cs = QString("SET GA N %1 1 1 50").arg(adr);
    emit sendCommand(cs);
}


void keyboard::slotAddressChanged(const QString & cNewAddress_)
{
    QString sCorrection = cNewAddress_;

    // a zero length is okay
    if (sCorrection.length() == 0)
        return;

    // now reject character input if it was not a digit
    for (uint i = 0; i < sCorrection.length(); i++) {
        if (isdigit(cNewAddress_[i]) == false) {
            sCorrection.replace(i, 1, '\0');    // correction code replaces
            addressLE->setText(sCorrection);    // the wrong user entry in
            addressLE->setCursorPosition(i);    // line edits
            break;
        }
    }
}
