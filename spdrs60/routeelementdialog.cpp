/***************************************************************************
                           routeelementdialog.cpp
                           version 0.4.8 $Revision: 1.1 $
                           -------------------------------
    copyright            : (C) 2005 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2005-10-22 05:43:44 $
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
   this file provides a dialog window to edit a single route element
 ***************************************************************************/

#include <qhbox.h>
#include <qlayout.h>

#include "routeelementdialog.h"

/*maximal length of an edit line*/
#define LEMAXWIDTH 55


/* non modal window */
RouteElementDialog::RouteElementDialog(QWidget* parent)
: QDialog(parent, "RouteElementDialog")
{
    rePtr1 = NULL;
    rePtr2 = NULL;
    
    setCaption(tr("Edit route element"));
    /*Layout to separate OK and Cancel buttons form the upper rest*/
    QBoxLayout* baseLayout = new QVBoxLayout(this, 12, 12);
    
    /*element data group box*/
    QGroupBox* elDataGB = new QGroupBox(0, Horizontal,
            tr("Route element"), this, "elDataGB");
    baseLayout->addWidget(elDataGB);
    QVBoxLayout* routeElGBLayout = new QVBoxLayout(elDataGB->layout(), 6);

    /*line with route element name*/
    QHBoxLayout* reLayout = new QHBoxLayout(routeElGBLayout, 6);
    QLabel* reNameLbl = new QLabel(tr("Name"), elDataGB);
    reLayout->addWidget(reNameLbl);
    QSpacerItem* spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    reLayout->addItem(spacer);
    reNameLE = new QLineEdit(elDataGB, "reNameLE");
    reNameLE->setReadOnly(true);
    reNameLE->setFocusPolicy(QWidget::NoFocus);
    reNameLE->setMaximumWidth(LEMAXWIDTH);
    reLayout->addWidget(reNameLE);

    /*line with route element SRCP bus*/
    QHBoxLayout* routeElSrcpBusLayout = new QHBoxLayout(routeElGBLayout, 6);
    QLabel* reSrcpBusLbl = new QLabel(tr("SRCP-&Bus"), elDataGB);
    routeElSrcpBusLayout->addWidget(reSrcpBusLbl);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    routeElSrcpBusLayout->addItem(spacer);
    reSrcpBusLE = new QLineEdit(elDataGB, "reSrcpBusLE");
    reSrcpBusLE->setMaximumWidth(LEMAXWIDTH);
    reSrcpBusLE->setMaxLength(4);
    reSrcpBusLbl->setBuddy(reSrcpBusLE);
    routeElSrcpBusLayout->addWidget(reSrcpBusLE);
    connect(reSrcpBusLE, SIGNAL(textChanged(const QString&)),
            this, SLOT(reBusChanged(const QString&)));

    /*line with route element adress*/
    QHBoxLayout* routeElAddrLayout = new QHBoxLayout(routeElGBLayout, 6);
    QLabel* reAddressLbl = new QLabel(tr("&Address"), elDataGB);
    routeElAddrLayout->addWidget(reAddressLbl);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    routeElAddrLayout->addItem(spacer);
    reAddressLE = new QLineEdit(elDataGB, "reAddressLE");
    reAddressLE->setMaximumWidth(LEMAXWIDTH);
    reAddressLE->setMaxLength(4);
    reAddressLbl->setBuddy(reAddressLE);
    routeElAddrLayout->addWidget(reAddressLE);
    connect(reAddressLE, SIGNAL(textChanged(const QString&)),
            this, SLOT(reAddressChanged(const QString&)));

    /*line with route element state*/
    QHBoxLayout* routeElStateLayout = new QHBoxLayout(routeElGBLayout, 6);
    QLabel* reStateLbl = new QLabel(tr("&State"), elDataGB);
    routeElStateLayout->addWidget(reStateLbl);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    routeElStateLayout->addItem(spacer);
    reStateSB = new QSpinBox(0, 3, 1 , elDataGB, "reStateSB");
    reStateLbl->setBuddy(reStateSB);
    routeElStateLayout->addWidget(reStateSB);


    /*separated line with OK and Cancel buttons*/
    QBoxLayout* buttonLayout = new QHBoxLayout(0, 0, 6);
    baseLayout->addLayout(buttonLayout);

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonLayout->addItem(spacer);

    QPushButton* okPB = new QPushButton(tr("OK"), this);
    connect(okPB, SIGNAL(clicked()), this, SLOT(accept()));
    buttonLayout->addWidget(okPB);
    okPB->setDefault(true);

    QPushButton* cancelPB = new QPushButton(tr("Cancel"), this);
    connect(cancelPB, SIGNAL(clicked()), this, SLOT(reject()));
    buttonLayout->addWidget(cancelPB);
}


void RouteElementDialog::setStateElementData(const stateElement* se)
{
    if (se == NULL)
        return;

    // name is set by "updateStartSignalName"
    reSrcpBusLE->setText(QString::number(se->bus));
    reAddressLE->setText(QString::number(se->address));
    reStateSB->setValue(se->state);
}


void RouteElementDialog::getStateElementData(stateElement* se)
{
    if (se == NULL)
        return;

    se->name = reNameLE->text();
    se->bus = reSrcpBusLE->text().toUInt();
    se->address = reAddressLE->text().toUInt();
    se->state = reStateSB->value();
    if (se->elemPtr != rePtr1) {
        se->elemPtr = rePtr2;
        se->elemPtr2 = NULL;
    }
}


void RouteElementDialog::reBusChanged(const QString& bstr)
{
    if (!bstr.isEmpty()) {
        int bus = bstr.toInt();
        int address = reAddressLE->text().toInt();
        updateRouteElementName(bus, address);
    }
}


void RouteElementDialog::reAddressChanged(const QString& astr)
{
    if (!astr.isEmpty()) {
        int address = astr.toInt();
        int bus = reSrcpBusLE->text().toInt();
        updateRouteElementName(bus, address);
    }
}


void RouteElementDialog::updateRouteElementName(int bus, int address)
{
    element* el = NULL;
    // send signal to routedialog, route, routingviewer, gbs
    emit getElementByAddress(bus, address, &el);

    rePtr1 = el;
    if (el == NULL)
        reNameLE->setText(tr("Error"));
    else {
        reNameLE->setText(el->getName());
        int ac = el->getAddressCount();
        if (ac == 1)
            reStateSB->setMaxValue(1);
        else
            reStateSB->setMaxValue(3);
    }
}

