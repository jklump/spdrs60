/***************************************************************************
                           routedialog.cpp
                           version 0.4.8 $Revision: 1.7 $
                           -------------------------------
    copyright            : (C) 2005 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2005-06-15 20:13:04 $
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
   this file provides a small user interface to select a certain route
   to be activated
 ***************************************************************************/

#include <qhbox.h>
#include <qlayout.h>

#include "routedialog.h"

extern bool SHOW_TOOLTIPS;
extern int FEEDBACK;


/* non modal window */
RouteDialog::RouteDialog(QWidget* parent)
: QDialog(parent, "EditRouteDialog")
{
    
    setCaption(tr("Edit route"));
    /*Layout to separate OK Cancel Button form the upper rest*/
    QBoxLayout* baseLayout = new QVBoxLayout(this, 6, 6);
    
    /*Layout to separate left and right groupboxest*/
    QBoxLayout* leftRightLayout = new QHBoxLayout(0, 0, 0);
    baseLayout->addLayout(leftRightLayout);
    
    /*line with OK and Cancel buttons*/
    QBoxLayout* buttonLayout = new QHBoxLayout(0, 0, 6);
    baseLayout->addLayout(buttonLayout);

    QSpacerItem* spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonLayout->addItem(spacer);

    QPushButton* okPB = new QPushButton(tr("OK"), this);
    connect(okPB, SIGNAL(clicked()), this, SLOT(accept()));
    buttonLayout->addWidget(okPB);
    okPB->setDefault(true);
    QPushButton* cancelPB = new QPushButton(tr("Cancel"), this);
    connect(cancelPB, SIGNAL(clicked()), this, SLOT(reject()));
    buttonLayout->addWidget(cancelPB);

    /*Layout to separate left column verticaly*/
    QBoxLayout* leftColumnLayout = new QVBoxLayout(0, 6, 6);
    leftRightLayout->addLayout(leftColumnLayout);
    

    /*Layout to separate right column verticaly*/
    QBoxLayout* rightColumnLayout = new QVBoxLayout(0, 6, 6);
    leftRightLayout->addLayout(rightColumnLayout);
    

    /*left column*/
    /*line with route name*/
    QHBoxLayout* nameLayout = new QHBoxLayout(leftColumnLayout);
    QLabel* lblRouteName = new QLabel(tr("&Name"), this);
    nameLayout->addWidget(lblRouteName);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    nameLayout->addItem(spacer);
    routeNameLE = new QLineEdit(this, "routeNameLE");
    lblRouteName->setBuddy(routeNameLE);
    nameLayout->addWidget(routeNameLE);

    /*start signal group box*/
    QGroupBox* startsignalGB = new QGroupBox(0, Horizontal,
            tr("Start signal"), this, "startsignalGB");
    leftColumnLayout->addWidget(startsignalGB);
    QVBoxLayout* ssgbL = new QVBoxLayout(startsignalGB->layout(), 6);

    /*line with start signal name*/
    QHBoxLayout* startSignalLayout = new QHBoxLayout(ssgbL, 6);
    QLabel* startSignalNameLB = new QLabel(tr("Name"), startsignalGB);
    startSignalLayout->addWidget(startSignalNameLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    startSignalLayout->addItem(spacer);
    startSignalNameLE = new QLineEdit(startsignalGB, "startSignalNameLE");
    startSignalNameLE->setReadOnly(true);
    startSignalLayout->addWidget(startSignalNameLE);

    /*line with start signal adress*/
    QHBoxLayout* startSigAddrLayout = new QHBoxLayout(ssgbL, 6);
    QLabel* startSignalAddressLB = new QLabel(tr("&Address"), startsignalGB);
    startSigAddrLayout->addWidget(startSignalAddressLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    startSigAddrLayout->addItem(spacer);
    startSignalAddressLE = new QLineEdit(startsignalGB, "startSignalAddressLE");
    startSignalAddressLB->setBuddy(startSignalAddressLE);
    startSigAddrLayout->addWidget(startSignalAddressLE);

    /*line with start signal state*/
    QHBoxLayout* startSigStateLayout = new QHBoxLayout(ssgbL, 6);
    QLabel* startSignalStateLB = new QLabel(tr("&State"), startsignalGB);
    startSigStateLayout->addWidget(startSignalStateLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    startSigStateLayout->addItem(spacer);
    startSignalStateLE = new QLineEdit(startsignalGB, "startSignalStateLE");
    startSignalStateLB->setBuddy(startSignalStateLE);
    startSigStateLayout->addWidget(startSignalStateLE);



    /*stop signal group box*/
    QGroupBox* stopsignalGB = new QGroupBox(0, Horizontal,
            tr("Stop signal"), this, "stopsignalGB");
    leftColumnLayout->addWidget(stopsignalGB);
    QVBoxLayout* sogbL = new QVBoxLayout(stopsignalGB->layout(), 6);

    /*line with stop signal name*/
    QHBoxLayout* stopSignalLayout = new QHBoxLayout(sogbL, 6);
    QLabel* stopSignalNameLB = new QLabel(tr("Name"), stopsignalGB);
    stopSignalLayout->addWidget(stopSignalNameLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    stopSignalLayout->addItem(spacer);
    stopSignalNameLE = new QLineEdit(stopsignalGB, "stopSignalNameLE");
    stopSignalNameLE->setReadOnly(true);
    stopSignalLayout->addWidget(stopSignalNameLE);

    /*line with stop signal adress*/
    QHBoxLayout* stopSigAddrLayout = new QHBoxLayout(sogbL, 6);
    QLabel* stopSignalAddressLB = new QLabel(tr("A&ddress"), stopsignalGB);
    stopSigAddrLayout->addWidget(stopSignalAddressLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    stopSigAddrLayout->addItem(spacer);
    stopSignalAddressLE = new QLineEdit(stopsignalGB, "stopSignalAddressLE");
    stopSignalAddressLB->setBuddy(stopSignalAddressLE);
    stopSigAddrLayout->addWidget(stopSignalAddressLE);

    /*route type group box*/
    typeGB = new QButtonGroup(0, Horizontal,
            tr("Type"), this, "typeGB");
    leftColumnLayout->addWidget(typeGB);
    QVBoxLayout* typeL = new QVBoxLayout(typeGB->layout(), 6);
    typeGB->setExclusive(true);

    QRadioButton* normalRouteRB = new QRadioButton(tr("&Normal route"),
            typeGB);
    typeL->addWidget(normalRouteRB);

    QRadioButton* detourRouteRB = new QRadioButton(tr("&Detour route"),
            typeGB);
    typeL->addWidget(detourRouteRB);

    QRadioButton* helpRouteRB = new QRadioButton(tr("&Help route"),
            typeGB);
    typeL->addWidget(helpRouteRB);

    QRadioButton* normalShuntingRB = new QRadioButton(tr("Normal &shunting"),
            typeGB);
    typeL->addWidget(normalShuntingRB);

    QRadioButton* detourShuntingRB = new QRadioButton(tr("Detour sh&unting"),
            typeGB);
    typeL->addWidget(detourShuntingRB);


    /*activate route group box*/
    QGroupBox* activateGB = new QGroupBox(0, Horizontal,
            tr("Activate route"), this, "activateGB");
    leftColumnLayout->addWidget(activateGB);
    QVBoxLayout* actibvateL = new QVBoxLayout(activateGB->layout(), 6);

    /*release route group box*/
    QGroupBox* releaseGB = new QGroupBox(0, Horizontal,
            tr("Release route"), this, "releaseGB");
    leftColumnLayout->addWidget(releaseGB);
    QVBoxLayout* releaseL = new QVBoxLayout(releaseGB->layout(), 6);

    
    /*right column*/
    /*start signal group box*/
    QGroupBox* routeElementsGB = new QGroupBox(0, Horizontal,
            tr("Route elements"), this, "routeElementsGB");
    rightColumnLayout->addWidget(routeElementsGB);
    QVBoxLayout* routeElL = new QVBoxLayout(routeElementsGB->layout(), 6);
    // TODO: table, Add-, Delete-buttons

/*
    // elements to activate or deactivate a route
    groupActivate =
        new QGroupBox(tr("(De-) Activation"), this, "Activation");

    lMod = new QLabel(tr("Mod #"), groupActivate);
    lPort = new QLabel(tr("Port #"), groupActivate);
    lRel = new QLabel(tr("Release:"), groupActivate);
    lAct = new QLabel(tr("Activate:"), groupActivate);
    lAct->setEnabled(false);
    lAdd = new QLabel(tr("or loco #:"), groupActivate);
    lAdd->setEnabled(false);

    sbRelMod =
        new QSpinBox(0, 4 * (31 + (FEEDBACK * 31)), 1, groupActivate, "");
    sbRelMod->setWrapping(true);
    connect(sbRelMod, SIGNAL(valueChanged(int)),
            this, SLOT(slotValueChanged(int)));
    connect(sbRelMod, SIGNAL(valueChanged(int)),
            this, SLOT(slotDisableRelPort(int)));

    sbRelPort = new QSpinBox(1, 16 - (FEEDBACK * 8), 1, groupActivate, "");
    sbRelPort->setWrapping(true);
    sbRelPort->setEnabled(false);
    connect(sbRelPort, SIGNAL(valueChanged(int)),
            this, SLOT(slotValueChanged(int)));

    sbActMod =
        new QSpinBox(0, 4 * (31 + (FEEDBACK * 31)), 1, groupActivate, "");
    sbActMod->setWrapping(true);
    connect(sbActMod, SIGNAL(valueChanged(int)),
            this, SLOT(slotValueChanged(int)));
    connect(sbActMod, SIGNAL(valueChanged(int)),
            this, SLOT(slotDisableActPort(int)));
    sbActMod->setEnabled(false);

    sbActPort = new QSpinBox(1, 16 - (FEEDBACK * 8), 1, groupActivate, "");
    sbActPort->setWrapping(true);
    sbActPort->setEnabled(false);
    connect(sbActPort, SIGNAL(valueChanged(int)),
            this, SLOT(slotValueChanged(int)));

    //lAdd->move(sbActPort->x()+sbActPort->width()+5, lAct->y());

    leLocoAddr = new QLineEdit(groupActivate, "");
    leLocoAddr->setMaxLength(4);
    connect(leLocoAddr, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotTextChanged(const QString&)));
    leLocoAddr->setEnabled(false);

    groupActivate->setGeometry(lblRouteName->x(),
                               routeNameLE->y() + routeNameLE->height() +
                               15, 300,
                               sbActMod->y() + sbActMod->height() + 10);

    QLabel *label = new QLabel(tr("&Level:"), bgRouteType);

    sbDetourLevel = new QSpinBox(1, 9, 1, bgRouteType, "DetourSB");
    sbDetourLevel->setWrapping(false);  // enables to spin "over" the limits
    connect(sbDetourLevel, SIGNAL(valueChanged(int)),
            this, SLOT(slotValueChanged(int)));

    label->setBuddy(sbDetourLevel);

    label = new QLabel(tr("L&evel:"), bgRouteType);

    sbShDetourLevel = new QSpinBox(1, 9, 1, bgRouteType, "ShDetourSB");
    sbShDetourLevel->setWrapping(false);  // enables to spin "over" the limits
    connect(sbShDetourLevel, SIGNAL(valueChanged(int)),
            this, SLOT(slotValueChanged(int)));
    label->setBuddy(sbShDetourLevel);

    // all about start and stop signals
    groupStartStop =
        new QGroupBox(tr("Start and stop signals"), this, "Signals");
    groupStartStop->setGeometry(groupActivate->x() +
                                groupActivate->width() + 10,
                                groupActivate->y(), 240,
                                groupActivate->height());

    lName = new QLabel(tr("Name"), groupStartStop);
    lAddr = new QLabel(tr("Address"), groupStartStop);
    lStat = new QLabel(tr("State"), groupStartStop);
    lStart = new QLabel(tr("Start:"), groupStartStop);
    lStopp = new QLabel(tr("Stop: "), groupStartStop);
    lStartName = new QLabel(groupStartStop, "");
    lStartName->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    leStartAddr = new QLineEdit(groupStartStop, "");
    leStartAddr->setMaxLength(4);
    connect(leStartAddr, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotTextChanged(const QString&)));
    connect(leStartAddr, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotAddressChanged(const QString&)));

    leStartStat = new QLineEdit(groupStartStop, "");
    leStartStat->setMaxLength(1);
    connect(leStartStat, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotTextChanged(const QString&)));
    connect(leStartStat, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotStatusChanged(const QString&)));

    lStoppName = new QLabel(groupStartStop, "");
    lStoppName->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    leStoppAddr = new QLineEdit(groupStartStop, "");
    leStoppAddr->setMaxLength(4);
    connect(leStoppAddr, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotTextChanged(const QString&)));
    connect(leStoppAddr, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotAddressChanged(const QString&)));


    // the elements which are switched in a route
    groupElements = new QGroupBox(tr("Routed elements"), this, "Elements");

    lNo = new QLabel(tr("No"), groupElements);
    lName2 = new QLabel(tr("Name"), groupElements);
    lAddr2 = new QLabel(tr("Address"), groupElements);
    lStat2 = new QLabel(tr("State"), groupElements);

    QString s;
    for (int j = 0; j < MAX_SW_ELEM; j++) {
        s.sprintf("%d", j + 1);
        lElem[j] = new QLabel(s, groupElements, "noLabel");
        lElem[j]->text().setNum(j+1);
        lElem[j]->setAlignment(Qt::AlignRight);

        lElemName[j] = new QLabel(groupElements, "elemName");
        lElemName[j]->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        lElemName[j]->setFocusPolicy(QWidget::NoFocus);

        leElemAddr[j] = new QLineEdit(groupElements, "elemAddress");
        leElemAddr[j]->setMaxLength(4);
        leElemAddr[j]->setFocusPolicy(QWidget::StrongFocus);
        connect(leElemAddr[j], SIGNAL(textChanged(const QString&)),
                this, SLOT(slotTextChanged(const QString&)));
        connect(leElemAddr[j], SIGNAL(textChanged(const QString&)),
                this, SLOT(slotAddressChanged(const QString&)));

        leElemStat[j] = new QLineEdit(groupElements, "elemState");
        leElemStat[j]->setMaxLength(1);
        leElemStat[j]->setFocusPolicy(QWidget::StrongFocus);
        connect(leElemStat[j], SIGNAL(textChanged(const QString&)),
                this, SLOT(slotTextChanged(const QString&)));
        connect(leElemStat[j], SIGNAL(textChanged(const QString&)),
                this, SLOT(slotStatusChanged(const QString&)));
    }
    */
}


QString RouteDialog::addZeros(const QString& sNZString_) // N on  Z ero  String
{
    QString s = sNZString_;     // string maybe without leading zeros
    while (s.length() < 4)
        s.prepend("0");

    return s;                   // string with leading zeros
}

void RouteDialog::setRouteName(const QString& rname)
{
    routeNameLE->setText(rname);
}


QString RouteDialog::getRouteName()
{
    return routeNameLE->text();
}


void RouteDialog::setRouteType(int type)
{
    typeGB->setButton(type);
}


int RouteDialog::getRouteType()
{
    return typeGB->selectedId();
}

