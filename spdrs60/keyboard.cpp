/***************************************************************************
                           keyboard.cpp
                           version 0.5.2 $Revision: 1.20 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2007 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-09-05 17:39:48 $
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

#include <qapplication.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvalidator.h>

#include "resources.h"
#include "keyboard.h"
#include "preferences.h"



keyboard::keyboard(SrcpPort::CommunicationStyle cstyle, int protocol,
        QWidget* parent, const char* name): QDialog(parent, name)
{
    setCaption(tr("Keyboard"));
    srcpStyle = cstyle;

    QBoxLayout* baseLayout = new QVBoxLayout(this, 10, 10);

    /*line with decoder combobox*/
    QHBoxLayout* protocolLayout = new QHBoxLayout(baseLayout, 6,
            "protocolLayout");
    QLabel* protocolLbl = new QLabel(tr("&Protocol:"), this);
    protocolLayout->addWidget(protocolLbl);
    protocolLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));

    protocolCB = new QComboBox(false, this);
    protocolLayout->addWidget(protocolCB);
    protocolCB->insertItem("MM");
    protocolCB->insertItem("DCC");
    protocolCB->insertItem("Slx");
    protocolCB->insertItem("Srv");
    protocolLbl->setBuddy(protocolCB);
    protocolCB->setCurrentItem(protocol);
    connect(protocolCB, SIGNAL(activated(int)),
            this, SIGNAL(protocolSelected(int)));
    QToolTip::add(protocolCB, tr("Select the decoder protocol\n"
                "MM: Maerklin/Motorola\n"
                "DCC: NMRA/DCC\n"
                "Slx: Selectrix\n"
                "Srv: Protocol by server\n"));

    /*line with SRCP-bus label and edit line*/
    QBoxLayout* busLayout = new QHBoxLayout(baseLayout, 6, "busLayout");

    QLabel *busLbl = new QLabel(tr("SRCP-&Bus:"), this, "busLbl");
    busLayout->addWidget(busLbl);

    busLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));

    busLE = new QLineEdit("1", this, "busLE");
    QFontMetrics fm(busLE->font());
    int LEwidth = fm.width("8888") + 10;
    busLE->setMaxLength(4);
    busLE->setMaximumWidth(LEwidth);
    QValidator* busValidator = new QIntValidator(1, 999, this);
    busLE->setValidator(busValidator);
    busLayout->addWidget(busLE);
    busLbl->setBuddy(busLE);
    QToolTip::add(busLE, tr("Enter the SRCP bus for the address"));

    // hide SRCP bus line if server provides SRCP 0.7.x
    if (SrcpPort::csOld == cstyle) {
        busLbl->hide();
        busLE->hide();
    }
    
    /*line with address label and edit line*/
    QBoxLayout* addressLayout = new QHBoxLayout(baseLayout, 6,
            "addressLayout");

    QLabel *labelAddress = new QLabel(tr("&Address:"), this, "addressLbl");
    addressLayout->addWidget(labelAddress);

    addressLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));

    addressLE = new QLineEdit("1", this, "addressLE");
    addressLE->setMaxLength(4);
    addressLE->setMaximumWidth(LEwidth);
    QValidator* addressValidator = new QIntValidator(1, MAX_GADCC, this);
    addressLE->setValidator(addressValidator);
    addressLayout->addWidget(addressLE);
    labelAddress->setBuddy(addressLE);
    QToolTip::add(addressLE, tr("Enter the address to be switched"));
    
    /*line with red and green buttons*/
    QBoxLayout* buttonLayout = new QHBoxLayout(0, 0, 6);
    baseLayout->addLayout(buttonLayout);

    buttonLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));

    QPushButton* redPB = new QPushButton("&0", this, "redBtn");
    redPB->setMaximumWidth(LEwidth);
    redPB->setPaletteBackgroundColor(QColor(255, 0, 0));
    connect(redPB, SIGNAL(clicked()), this, SLOT(slotActivateRed()));
    buttonLayout->addWidget(redPB);
    QToolTip::add(redPB, tr("Press this button to activate red connector"));

    buttonLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));

    QPushButton* greenPB = new QPushButton("&1", this, "greenBtn");
    greenPB->setMaximumWidth(LEwidth);
    greenPB->setPaletteBackgroundColor(QColor(0, 255, 0));
    connect(greenPB, SIGNAL(clicked()), this, SLOT(slotActivateGreen()));
    buttonLayout->addWidget(greenPB);
    greenPB->setDefault(true);
    QToolTip::add(greenPB,
            tr("Press this button to activate green connector"));

    buttonLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));
}


// send command for selected protocol
void keyboard::slotActivateRed()
{
    SrcpMessage::Protocol protocol;
    unsigned int adr = addressLE->text().toUInt();
    unsigned int bus = busLE->text().toUInt();

    switch(protocolCB->currentItem()) {
        case 0:
            protocol = SrcpMessage::proMM;
            break;
        case 1:
            protocol = SrcpMessage::proDCC;
            break;
        case 2:
            protocol = SrcpMessage::proSelectrix;
            break;
        case 3:
            protocol = SrcpMessage::proServer;
            break;
        default:
            protocol = SrcpMessage::proMM;
    }

    SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgGaSet);
    if (sm == NULL)
        return;

    sm->setGaData(protocol, bus, adr, 0, 1, 200);

    // send also init message if is new style
    if (SrcpPort::csNew == srcpStyle) {
        sm->setMessage(SrcpMessage::msgGaInit);
        emit sendSrcpMessage(sm);
        // give time to show effect
        qApp->processEvents();
        sm->setMessage(SrcpMessage::msgGaSet);
    }

    emit sendSrcpMessage(sm);
    delete sm;
}


void keyboard::slotActivateGreen()
{
    SrcpMessage::Protocol protocol;
    unsigned int adr = addressLE->text().toUInt();
    unsigned int bus = busLE->text().toUInt();

    switch(protocolCB->currentItem()) {
        case 0:
            protocol = SrcpMessage::proMM;
            break;
        case 1:
            protocol = SrcpMessage::proDCC;
            break;
        case 2:
            protocol = SrcpMessage::proSelectrix;
            break;
        case 3:
            protocol = SrcpMessage::proServer;
            break;
        default:
            protocol = SrcpMessage::proMM;
    }

    SrcpMessage* sm = new SrcpMessage(SrcpMessage::msgGaSet);
    if (sm == NULL)
        return;

    sm->setGaData(protocol, bus, adr, 1, 1, 200);

    // send also init message if is new style
    if (SrcpPort::csNew == srcpStyle) {
        sm->setMessage(SrcpMessage::msgGaInit);
        emit sendSrcpMessage(sm);
        // give time to show effect
        qApp->processEvents();
        sm->setMessage(SrcpMessage::msgGaSet);
    }

    emit sendSrcpMessage(sm);
    delete sm;
}

