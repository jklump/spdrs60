/***************************************************************************
                           keyboard.cpp
                           version 0.5.0 $Revision: 1.14 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2006-02-11 20:38:10 $
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
   this code shows a window with a manual keyboard to switch solenoids
 ***************************************************************************/

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvalidator.h>

#include "resources.h"
#include "keyboard.h"
#include "preferences.h"



keyboard::keyboard(QWidget* parent, unsigned int srcpv): QDialog(parent,
        "keyboard")
{
    setCaption(tr("Keyboard"));
    srcpVersion = srcpv;

    QBoxLayout* baseLayout = new QVBoxLayout(this, 10, 10);

    /*line with SRCP-bus label and edit line*/
    QBoxLayout* busLayout = new QHBoxLayout(baseLayout, 6, "busLayout");

    QLabel *busLbl = new QLabel(tr("SRCP-&Bus:"), this, "busLbl");
    if (pref.tooltips)
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
    QValidator* busValidator = new QIntValidator(1, 999, this);
    busLE->setValidator(busValidator);
    busLayout->addWidget(busLE);
    busLbl->setBuddy(busLE);

    // hide SRCP bus line if server provides SRCP 0.7.x
    if (7 == srcpv) {
        busLbl->hide();
        busLE->hide();
    }
    
    /*line with address label and edit line*/
    QBoxLayout* addressLayout = new QHBoxLayout(baseLayout, 6,
            "addressLayout");

    QLabel *labelAddress = new QLabel(tr("&Address:"), this, "addressLbl");
    if (pref.tooltips)
        QToolTip::add(labelAddress,
                      tr("Please enter the address to be switched"));
    addressLayout->addWidget(labelAddress);

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    addressLayout->addItem(spacer);

    addressLE = new QLineEdit(this, "addressLE");
    addressLE->setMaxLength(4);
    addressLE->setMaximumWidth(LEwidth);
    QValidator* addressValidator = new QIntValidator(1, MAX_GADCC, this);
    addressLE->setValidator(addressValidator);
    addressLayout->addWidget(addressLE);
    labelAddress->setBuddy(addressLE);
    
    /*line with red and green buttons*/
    QBoxLayout* buttonLayout = new QHBoxLayout(0, 0, 6);
    baseLayout->addLayout(buttonLayout);

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonLayout->addItem(spacer);

    QPushButton* redPB = new QPushButton("&0", this, "redBtn");
    redPB->setMaximumWidth(LEwidth);
    redPB->setPaletteBackgroundColor(QColor(255, 0, 0));
    connect(redPB, SIGNAL(clicked()), this, SLOT(slotActivateRed()));
    buttonLayout->addWidget(redPB);
    if (pref.tooltips)
        QToolTip::add(redPB,
                      tr("Press this button to activate red connector"));

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonLayout->addItem(spacer);

    QPushButton* greenPB = new QPushButton("&1", this, "greenBtn");
    greenPB->setMaximumWidth(LEwidth);
    greenPB->setPaletteBackgroundColor(QColor(0, 255, 0));
    connect(greenPB, SIGNAL(clicked()), this, SLOT(slotActivateGrn()));
    buttonLayout->addWidget(greenPB);
    greenPB->setDefault(true);
    if (pref.tooltips)
        QToolTip::add(greenPB,
                      tr("Press this button to activate green connector"));

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonLayout->addItem(spacer);
}


// send command for both protocols with a basic limit check,
// MAX_GADCC is the higher limit of DCC protocol
void keyboard::slotActivateRed()
{
    unsigned int adr = addressLE->text().toUInt();
    unsigned int bus = busLE->text().toUInt();

    if (srcpVersion == 7) {
        /* SET GA <protocol> <addr> <port> <action> <delay> */
        QString cs = QString("SET GA N %1 0 1 50").arg(adr);
        emit sendCommand(cs);
        if (adr <= MAX_GAMM) {
            cs = QString("SET GA M %1 0 1 50").arg(adr);
            emit sendCommand(cs);
        }
    }
    else {
        /* SET <bus> GA <addr> <port> <value> <delay> */
        QString cs = QString("SET %1 GA %2 0 1 50").arg(bus).arg(adr);
        emit sendCommand(cs);
    }
}


void keyboard::slotActivateGrn()
{
    unsigned int adr = addressLE->text().toUInt();
    unsigned int bus = busLE->text().toUInt();
    
    if (srcpVersion == 7) {
        /* SET GA <protocol> <addr> <port> <action> <delay> */
        QString cs = QString("SET GA N %1 1 1 50").arg(adr);
        emit sendCommand(cs);
        if (adr <= MAX_GAMM) {
            cs = QString("SET GA M %1 1 1 50").arg(adr);
            emit sendCommand(cs);
        }
    }
    else {
        /* SET <bus> GA <addr> <port> <value> <delay> */
        QString cs = QString("SET %1 GA %2 1 1 50").arg(bus).arg(adr);
        emit sendCommand(cs);
    }
}

