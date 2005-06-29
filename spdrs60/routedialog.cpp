/***************************************************************************
                           routedialog.cpp
                           version 0.4.8 $Revision: 1.14 $
                           -------------------------------
    copyright            : (C) 2005 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2005-06-29 20:42:36 $
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

/*maximal length of an edit line*/
#define LEMAXWIDTH 55


extern bool SHOW_TOOLTIPS;
extern int FEEDBACK;


/* non modal window */
RouteDialog::RouteDialog(QWidget* parent)
: QDialog(parent, "EditRouteDialog")
{
    startSignalElPtr = NULL;
    stopSignalElPtr = NULL;
    
    setCaption(tr("Edit route"));
    /*Layout to separate OK Cancel Button form the upper rest*/
    QBoxLayout* baseLayout = new QVBoxLayout(this, 12, 12);
    
    /*Layout to separate left and right groupboxest*/
    QBoxLayout* leftRightLayout = new QHBoxLayout(0, 0, 12);
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
    QBoxLayout* leftColumnLayout = new QVBoxLayout(0, 0, 6);
    leftRightLayout->addLayout(leftColumnLayout);
    

    /*Layout to separate right column verticaly*/
    QBoxLayout* rightColumnLayout = new QVBoxLayout(0, 0, 6);
    leftRightLayout->addLayout(rightColumnLayout);
    

    /*left column*/
    /*line with route name*/
    QHBoxLayout* nameLayout = new QHBoxLayout(leftColumnLayout);
    QLabel* lblRouteName = new QLabel(tr("Na&me"), this);
    nameLayout->addWidget(lblRouteName);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    nameLayout->addItem(spacer);
    routeNameLE = new QLineEdit(this, "routeNameLE");
    lblRouteName->setBuddy(routeNameLE);
    nameLayout->addWidget(routeNameLE);

    /*horizontal layout for start and stop signal data group boxes*/
    QHBoxLayout* signalStaStoLayout = new QHBoxLayout(leftColumnLayout, 6);
    /*start signal group box*/
    QGroupBox* startsignalGB = new QGroupBox(0, Horizontal,
            tr("Start signal"), this, "startsignalGB");
    signalStaStoLayout->addWidget(startsignalGB);
    QVBoxLayout* startSigGBLayout = new QVBoxLayout(startsignalGB->layout(), 6);

    /*line with start signal name*/
    QHBoxLayout* startSignalLayout = new QHBoxLayout(startSigGBLayout, 6);
    QLabel* startSignalNameLB = new QLabel(tr("Name"), startsignalGB);
    startSignalLayout->addWidget(startSignalNameLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    startSignalLayout->addItem(spacer);
    startSignalNameLE = new QLineEdit(startsignalGB, "startSignalNameLE");
    startSignalNameLE->setReadOnly(true);
    startSignalNameLE->setFocusPolicy(QWidget::NoFocus);
    startSignalNameLE->setMaximumWidth(LEMAXWIDTH);
    startSignalLayout->addWidget(startSignalNameLE);

    /*line with start signal SRCP bus*/
    QHBoxLayout* startSigSrcpBusLayout = new QHBoxLayout(startSigGBLayout, 6);
    QLabel* startSignalSrcpBusLB = new QLabel(tr("SRC&P-Bus"), startsignalGB);
    startSigSrcpBusLayout->addWidget(startSignalSrcpBusLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    startSigSrcpBusLayout->addItem(spacer);
    startSignalSrcpBusLE = new QLineEdit(startsignalGB, "startSignalSrcpBusLE");
    startSignalSrcpBusLE->setMaximumWidth(LEMAXWIDTH);
    startSignalSrcpBusLE->setMaxLength(4);
    startSignalSrcpBusLB->setBuddy(startSignalSrcpBusLE);
    startSigSrcpBusLayout->addWidget(startSignalSrcpBusLE);
    connect(startSignalSrcpBusLE, SIGNAL(textChanged(const QString&)),
            this, SLOT(startSignalBusChanged(const QString&)));

    /*line with start signal adress*/
    QHBoxLayout* startSigAddrLayout = new QHBoxLayout(startSigGBLayout, 6);
    QLabel* startSignalAddressLB = new QLabel(tr("&Address"), startsignalGB);
    startSigAddrLayout->addWidget(startSignalAddressLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    startSigAddrLayout->addItem(spacer);
    startSignalAddressLE = new QLineEdit(startsignalGB, "startSignalAddressLE");
    startSignalAddressLE->setMaximumWidth(LEMAXWIDTH);
    startSignalAddressLE->setMaxLength(4);
    startSignalAddressLB->setBuddy(startSignalAddressLE);
    startSigAddrLayout->addWidget(startSignalAddressLE);
    connect(startSignalAddressLE, SIGNAL(textChanged(const QString&)),
            this, SLOT(startSignalAddressChanged(const QString&)));

    /*line with start signal state*/
    QHBoxLayout* startSigStateLayout = new QHBoxLayout(startSigGBLayout, 6);
    QLabel* startSignalStateLB = new QLabel(tr("S&tate"), startsignalGB);
    startSigStateLayout->addWidget(startSignalStateLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    startSigStateLayout->addItem(spacer);
    startSignalStateSB = new QSpinBox(0, 3, 1 , startsignalGB,
            "startSignalStateSB");
    startSignalStateLB->setBuddy(startSignalStateSB);
    startSigStateLayout->addWidget(startSignalStateSB);

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    startSigGBLayout->addItem(spacer);


    /*stop signal group box*/
    QGroupBox* stopSignalGB = new QGroupBox(0, Horizontal,
            tr("Stop signal"), this, "stopSignalGB");
    signalStaStoLayout->addWidget(stopSignalGB);
    QVBoxLayout* stopSigGBLayout = new QVBoxLayout(stopSignalGB->layout(), 6);

    /*line with stop signal name*/
    QHBoxLayout* stopSignalLayout = new QHBoxLayout(stopSigGBLayout, 6);
    QLabel* stopSignalNameLB = new QLabel(tr("Name"), stopSignalGB);
    stopSignalLayout->addWidget(stopSignalNameLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    stopSignalLayout->addItem(spacer);
    stopSignalNameLE = new QLineEdit(stopSignalGB, "stopSignalNameLE");
    stopSignalNameLE->setMaximumWidth(LEMAXWIDTH);
    stopSignalNameLE->setReadOnly(true);
    stopSignalNameLE->setFocusPolicy(QWidget::NoFocus);
    stopSignalLayout->addWidget(stopSignalNameLE);

    /*line with stop signal SRCP bus*/
    QHBoxLayout* stopSigSrcpBusLayout = new QHBoxLayout(stopSigGBLayout, 6);
    QLabel* stopSignalSrcpBusLB = new QLabel(tr("SRCP-&Bus"), stopSignalGB);
    stopSigSrcpBusLayout->addWidget(stopSignalSrcpBusLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    stopSigSrcpBusLayout->addItem(spacer);
    stopSignalSrcpBusLE = new QLineEdit(stopSignalGB, "stopSignalSrcpBusLE");
    stopSignalSrcpBusLE->setMaximumWidth(LEMAXWIDTH);
    stopSignalSrcpBusLE->setMaxLength(4);
    stopSignalSrcpBusLB->setBuddy(stopSignalSrcpBusLE);
    stopSigSrcpBusLayout->addWidget(stopSignalSrcpBusLE);
    connect(stopSignalSrcpBusLE, SIGNAL(textChanged(const QString&)),
            this, SLOT(stopSignalBusChanged(const QString&)));

    /*line with stop signal adress*/
    QHBoxLayout* stopSigAddrLayout = new QHBoxLayout(stopSigGBLayout, 6);
    QLabel* stopSignalAddressLB = new QLabel(tr("Add&ress"), stopSignalGB);
    stopSigAddrLayout->addWidget(stopSignalAddressLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    stopSigAddrLayout->addItem(spacer);
    stopSignalAddressLE = new QLineEdit(stopSignalGB, "stopSignalAddressLE");
    stopSignalAddressLE->setMaximumWidth(LEMAXWIDTH);
    stopSignalAddressLE->setMaxLength(4);
    stopSignalAddressLB->setBuddy(stopSignalAddressLE);
    stopSigAddrLayout->addWidget(stopSignalAddressLE);
    connect(stopSignalAddressLE, SIGNAL(textChanged(const QString&)),
            this, SLOT(stopSignalAddressChanged(const QString&)));

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    stopSigGBLayout->addItem(spacer);


    /*horizontal layout for route activation and release group boxes*/
    QHBoxLayout* routeAcReLayout = new QHBoxLayout(leftColumnLayout, 6);
    /*activate route group box*/
    QGroupBox* activateGB = new QGroupBox(0, Horizontal,
            tr("Activate route"), this, "activateGB");
    routeAcReLayout->addWidget(activateGB);
    QVBoxLayout* activateGBL = new QVBoxLayout(activateGB->layout(), 6);

    /*line with CheckBox to activate FB switching*/
    activatefbCB = new QCheckBox(tr("&Enable feedback activation"),
            activateGB, "activatefbCB");
    activateGBL->addWidget(activatefbCB);
    connect(activatefbCB, SIGNAL(toggled(bool)),
            this, SLOT(activateCBchanged(bool)));
    
    /*two lines with radio buttons to choose feedback signal direction*/
    activateRouteBG = new QButtonGroup(0, Horizontal,
            tr("Switch on feedback response"), activateGB, "activateRouteBG");
    activateGBL->addWidget(activateRouteBG);
    QVBoxLayout* activateFBL = new QVBoxLayout(activateRouteBG->layout(), 6);
    activateRouteBG->setExclusive(true);
    
    QRadioButton* activateOnActivationRB = new QRadioButton(
            tr("Ac&tivation"), activateRouteBG);
    activateFBL->addWidget(activateOnActivationRB);

    QRadioButton* activateOnDeactivationRB = new QRadioButton(
            tr("&Deactivation"), activateRouteBG);
    activateFBL->addWidget(activateOnDeactivationRB);

    /*line with SRCP bus for activation by feedback*/
    QHBoxLayout* activateSrcpBusLayout = new QHBoxLayout(activateGBL, 6);
    activateSrcpBusLB = new QLabel(tr("B&us (s88/SRCP)"), activateGB);
    activateSrcpBusLayout->addWidget(activateSrcpBusLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    activateSrcpBusLayout->addItem(spacer);
    activateSrcpBusLE = new QLineEdit(activateGB, "activateSrcpBusLE");
    activateSrcpBusLE->setMaximumWidth(LEMAXWIDTH);
    activateSrcpBusLE->setMaxLength(4);
    activateSrcpBusLB->setBuddy(activateSrcpBusLE);
    activateSrcpBusLayout->addWidget(activateSrcpBusLE);

    /*line with contact for activation by feedback*/
    QHBoxLayout* activateContactLayout = new QHBoxLayout(activateGBL, 6);
    activateContactLB = new QLabel(tr("&Contact (1 - 496)"),
            activateGB);
    activateContactLayout->addWidget(activateContactLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    activateContactLayout->addItem(spacer);
    activateContactSB = new QSpinBox(1, 496, 1, activateGB,
            "activateContactSB");
    activateContactLB->setBuddy(activateContactSB);
    activateContactLayout->addWidget(activateContactSB);
    connect(activateContactSB, SIGNAL(valueChanged(int)),
            this, SLOT(activateContactSBChanged(int)));

    /*line with module for activation by feedback*/
    QHBoxLayout* activateModuleLayout = new QHBoxLayout(activateGBL, 6);
    activateModuleLB = new QLabel(tr("Module (1 - %1)")
            .arg(FEEDBACK == FB_16 ? 31 : 62), activateGB);
    activateModuleLayout->addWidget(activateModuleLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    activateModuleLayout->addItem(spacer);
    activateModuleLE = new QLineEdit(activateGB, "activateModuleLE");
    activateModuleLE->setMaximumWidth(LEMAXWIDTH);
    activateModuleLE->setFocusPolicy(QWidget::NoFocus);
    //activateModuleLB->setBuddy(activateModuleLE);
    activateModuleLayout->addWidget(activateModuleLE);

    /*line with port for activation by feedback*/
    QHBoxLayout* activatePortLayout = new QHBoxLayout(activateGBL, 6);
    activatePortLB = new QLabel(tr("Port (1 - %1)")
            .arg(FEEDBACK == FB_16 ? 16 : 8), activateGB);
    activatePortLayout->addWidget(activatePortLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    activatePortLayout->addItem(spacer);
    activatePortLE = new QLineEdit(activateGB, "activatePortLE");
    activatePortLE->setMaximumWidth(LEMAXWIDTH);
    activatePortLE->setFocusPolicy(QWidget::NoFocus);
    //activatePortLB->setBuddy(activatePortLE);
    activatePortLayout->addWidget(activatePortLE);


    /*release route group box*/
    QGroupBox* releaseGB = new QGroupBox(0, Horizontal,
            tr("Release route"), this, "releaseGB");
    routeAcReLayout->addWidget(releaseGB);
    QVBoxLayout* releaseGBL = new QVBoxLayout(releaseGB->layout(), 6);

    /*line with CheckBox to release FB switching*/
    releasefbCB = new QCheckBox(tr("Enable &feedback release"),
            releaseGB, "releasefbCB");
    releaseGBL->addWidget(releasefbCB);
    connect(releasefbCB, SIGNAL(toggled(bool)),
            this, SLOT(releaseCBchanged(bool)));
    
    /*two lines with radio buttons to choose feedback signal direction*/
    releaseRouteBG = new QButtonGroup(0, Horizontal,
            tr("Switch on feedback response"), releaseGB, "releaseRouteBG");
    releaseGBL->addWidget(releaseRouteBG);
    QVBoxLayout* releaseFBL = new QVBoxLayout(releaseRouteBG->layout(), 6);
    releaseRouteBG->setExclusive(true);
    
    QRadioButton* releaseOnActivationRB = new QRadioButton(
            tr("Ac&tivation"), releaseRouteBG);
    releaseFBL->addWidget(releaseOnActivationRB);

    QRadioButton* releaseOnDeactivationRB = new QRadioButton(
            tr("&Deactivation"), releaseRouteBG);
    releaseFBL->addWidget(releaseOnDeactivationRB);

    /*line with SRCP bus for activation by feedback*/
    QHBoxLayout* releaseSrcpBusLayout = new QHBoxLayout(releaseGBL, 6);
    releaseSrcpBusLB = new QLabel(tr("Bus (s&88/SRCP)"), releaseGB);
    releaseSrcpBusLayout->addWidget(releaseSrcpBusLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    releaseSrcpBusLayout->addItem(spacer);
    releaseSrcpBusLE = new QLineEdit(releaseGB, "releaseSrcpBusLE");
    releaseSrcpBusLE->setMaximumWidth(LEMAXWIDTH);
    releaseSrcpBusLE->setMaxLength(4);
    releaseSrcpBusLB->setBuddy(releaseSrcpBusLE);
    releaseSrcpBusLayout->addWidget(releaseSrcpBusLE);

    /*line with contact for activation by feedback*/
    QHBoxLayout* releaseContactLayout = new QHBoxLayout(releaseGBL, 6);
    releaseContactLB = new QLabel(tr("&Contact (1 - 496)"), releaseGB);
    releaseContactLayout->addWidget(releaseContactLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    releaseContactLayout->addItem(spacer);
    releaseContactSB = new QSpinBox(1, 496, 1, releaseGB, "releaseContactSB");
    releaseContactLB->setBuddy(releaseContactSB);
    releaseContactLayout->addWidget(releaseContactSB);
    connect(releaseContactSB, SIGNAL(valueChanged(int)),
            this, SLOT(releaseContactSBChanged(int)));

    /*line with module for activation by feedback*/
    QHBoxLayout* releaseModuleLayout = new QHBoxLayout(releaseGBL, 6);
    releaseModuleLB = new QLabel(tr("Module (1 - %1)")
            .arg(FEEDBACK == FB_16 ? 31 : 62), releaseGB);
    releaseModuleLayout->addWidget(releaseModuleLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    releaseModuleLayout->addItem(spacer);
    releaseModuleLE = new QLineEdit(releaseGB, "releaseModuleLE");
    releaseModuleLE->setMaximumWidth(LEMAXWIDTH);
    releaseModuleLE->setFocusPolicy(QWidget::NoFocus);
    //releaseModuleLB->setBuddy(releaseModuleLE);
    releaseModuleLayout->addWidget(releaseModuleLE);

    /*line with port activation by feedback*/
    QHBoxLayout* releasePortLayout = new QHBoxLayout(releaseGBL, 6);
    releasePortLB = new QLabel(tr("Port (1 - %1)")
            .arg(FEEDBACK == FB_16 ? 16 : 8), releaseGB);
    releasePortLayout->addWidget(releasePortLB);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    releasePortLayout->addItem(spacer);
    releasePortLE = new QLineEdit(releaseGB, "releasePortLE");
    releasePortLE->setMaximumWidth(LEMAXWIDTH);
    releasePortLE->setFocusPolicy(QWidget::NoFocus);
    //releasePortLB->setBuddy(releasePortLE);
    releasePortLayout->addWidget(releasePortLE);


    /*right column*/
    /*route type group box*/
    typeBG = new QButtonGroup(0, Horizontal,
            tr("Type"), this, "typeBG");
    rightColumnLayout->addWidget(typeBG);
    QVBoxLayout* typeL = new QVBoxLayout(typeBG->layout(), 6);
    typeBG->setExclusive(true);

    QRadioButton* normalRouteRB = new QRadioButton(tr("&Normal route"),
            typeBG);
    typeL->addWidget(normalRouteRB);

    /*line with radio button and uzs detour level spinbox*/
    QBoxLayout* uzsLayout = new QHBoxLayout(0, 0, 16);
    typeL->addLayout(uzsLayout);
    
    QRadioButton* detourRouteRB = new QRadioButton(tr("&Detour route"),
            typeBG);
    uzsLayout->addWidget(detourRouteRB);

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    uzsLayout->addItem(spacer);
    
    /*sublayout for label ans spinbox*/
    QBoxLayout* uzsLevelLayout = new QHBoxLayout(0, 0, 6);
    uzsLayout->addLayout(uzsLevelLayout);
    
    QLabel* label = new QLabel(tr("&Level:"), typeBG);
    uzsLayout->addWidget(label);
    uzsLevelSB = new QSpinBox(0, 9, 1, typeBG, "uzsDetourLevelSB");
    uzsLevelSB->setWrapping(false);
    uzsLayout->addWidget(uzsLevelSB);
    label->setBuddy(uzsLevelSB);

    QRadioButton* helpRouteRB = new QRadioButton(tr("&Help route"),
            typeBG);
    typeL->addWidget(helpRouteRB);

    QRadioButton* normalShuntingRB = new QRadioButton(tr("Normal &shunting"),
            typeBG);
    typeL->addWidget(normalShuntingRB);

    /*line with radio button and urs detour level spinbox*/
    QBoxLayout* ursLayout = new QHBoxLayout(0, 0, 16);
    typeL->addLayout(ursLayout);
    
    QRadioButton* detourShuntingRB = new QRadioButton(tr("Detour shuntin&g"),
            typeBG);
    ursLayout->addWidget(detourShuntingRB);

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    ursLayout->addItem(spacer);
    
    /*sublayout for label ans spinbox*/
    QBoxLayout* ursLevelLayout = new QHBoxLayout(0, 0, 6);
    ursLayout->addLayout(ursLevelLayout);
    
    label = new QLabel(tr("Le&vel:"), typeBG);
    ursLayout->addWidget(label);
    ursLevelSB = new QSpinBox(0, 9, 1, typeBG, "ursDetourLevelSB");
    ursLevelSB->setWrapping(false);
    ursLayout->addWidget(ursLevelSB);
    label->setBuddy(ursLevelSB);
    connect(typeBG, SIGNAL(pressed(int)), this, SLOT(typeBGPressed(int)));


    /*route elements group box*/
    QGroupBox* routeElementsGB = new QGroupBox(0, Horizontal,
            tr("Route elements"), this, "routeElementsGB");
    rightColumnLayout->addWidget(routeElementsGB);
    QVBoxLayout* routeElL = new QVBoxLayout(routeElementsGB->layout(), 6);
    
    /* list with route elements */
    elementsLV = new QListView(routeElementsGB, "elementsLV");
    elementsLV->setSorting(-1);
    routeElL->addWidget(elementsLV);
    elementsLV->addColumn(tr("Name"));
    elementsLV->setColumnWidthMode(0, QListView::Maximum);
    elementsLV->addColumn(tr("SRCP-Bus"));
    elementsLV->addColumn(tr("Address"));
    elementsLV->addColumn(tr("State"));
    elementsLV->setColumnAlignment(1, Qt::AlignRight);
    elementsLV->setColumnAlignment(2, Qt::AlignRight);
    elementsLV->setColumnAlignment(3, Qt::AlignRight);
    connect(elementsLV, SIGNAL(currentChanged(QListViewItem*)),
            this, SLOT(elementsLVChanged(QListViewItem*)));
 
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    routeElL->addItem(spacer);

    /*line with Add and Remove buttons*/
    QBoxLayout* routeElBtnLayout = new QHBoxLayout(0, 0, 6);
    routeElL->addLayout(routeElBtnLayout);

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    routeElBtnLayout->addItem(spacer);

    editPB = new QPushButton(tr("&Edit"), routeElementsGB);
    //connect(editPB, SIGNAL(clicked()), this, SLOT(editListElement()));
    routeElBtnLayout->addWidget(editPB);
    editPB->setEnabled(false);

    addPB = new QPushButton(tr("&Add"), routeElementsGB);
    //connect(addPB, SIGNAL(clicked()), this, SLOT(addElementToList()));
    routeElBtnLayout->addWidget(addPB);

    removePB = new QPushButton(tr("&Remove"), routeElementsGB);
    connect(removePB, SIGNAL(clicked()), this, SLOT(removeElementFromList()));
    routeElBtnLayout->addWidget(removePB);
    removePB->setEnabled(false);
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
    typeBG->setButton(type);
    typeBGPressed(type);
}


int RouteDialog::getRouteType()
{
    return typeBG->selectedId();
}


void RouteDialog::setStartSignalData(const stateElement& signal)
{
    // name is set by "updateStartSignalName"
    startSignalSrcpBusLE->setText(QString::number(signal.bus));
    startSignalAddressLE->setText(QString::number(signal.address));
    startSignalStateSB->setValue(signal.state);
}


void RouteDialog::getStartSignalData(stateElement& signal)
{
    signal.name = startSignalNameLE->text();
    signal.bus = startSignalSrcpBusLE->text().toUInt();
    signal.address = startSignalAddressLE->text().toUInt();
    signal.state = startSignalStateSB->value();
    signal.elemPtr = startSignalElPtr;
    signal.elemPtr2 = NULL;
}


void RouteDialog::setStopSignalData(const stateElement& signal)
{
    // name is set by "updateStopSignalName"
    stopSignalSrcpBusLE->setText(QString::number(signal.bus));
    stopSignalAddressLE->setText(QString::number(signal.address));
}


void RouteDialog::getStopSignalData(stateElement& signal)
{
    signal.name = stopSignalNameLE->text();
    signal.bus = stopSignalSrcpBusLE->text().toUInt();
    signal.address = stopSignalAddressLE->text().toUInt();
    signal.elemPtr = stopSignalElPtr;
    signal.elemPtr2 = NULL;
}


void RouteDialog::setActivateData(const PortState& port)
{
    activatefbCB->setChecked(port.used);
    // this should normaly not be necessary:
    activateCBchanged(port.used);
    activateRouteBG->setButton((int)port.switchtooff);
    activateSrcpBusLE->setText(QString::number(port.bus));
    activateContactSB->setValue(port.address);
    activateContactSBChanged(port.address);
}


void RouteDialog::getActivateData(PortState& port)
{
    port.used = activatefbCB->isChecked();
    port.switchtooff = (activateRouteBG->selectedId() == 1);
    port.bus = activateSrcpBusLE->text().toUInt();
    port.address = activateContactSB->value();
}


void RouteDialog::setReleaseData(const PortState& port)
{
    releasefbCB->setChecked(port.used);
    // this should normaly not be necessary:
    releaseCBchanged(port.used);
    releaseRouteBG->setButton((int)port.switchtooff);
    releaseSrcpBusLE->setText(QString::number(port.bus));
    releaseContactSB->setValue(port.address);
    releaseContactSBChanged(port.address);
}


void RouteDialog::getReleaseData(PortState& port)
{
    port.used = releasefbCB->isChecked();
    port.switchtooff = (releaseRouteBG->selectedId() == 1);
    port.bus = releaseSrcpBusLE->text().toUInt();
    port.address = releaseContactSB->value();
}


void RouteDialog::setRouteElements(const QPtrList<stateElement>& items)
{
    QPtrListIterator<stateElement> it(items);
    stateElement* se;
    QListViewItem* element;
    while ((se = it.current()) != 0) {
        ++it;
        element = new QListViewItem(elementsLV, se->name,
            QString::number(se->bus), QString::number(se->address),
            QString::number(se->state));
    }
}


void RouteDialog::activateCBchanged(bool isChecked)
{
    activateRouteBG->setEnabled(isChecked);
    activateSrcpBusLB->setEnabled(isChecked);
    activateSrcpBusLE->setEnabled(isChecked);
    activateContactLB->setEnabled(isChecked);
    activateContactSB->setEnabled(isChecked);
    activateModuleLB->setEnabled(isChecked);
    activateModuleLE->setEnabled(isChecked);
    activatePortLB->setEnabled(isChecked);
    activatePortLE->setEnabled(isChecked);
}


void RouteDialog::releaseCBchanged(bool isChecked)
{
    releaseRouteBG->setEnabled(isChecked);
    releaseSrcpBusLB->setEnabled(isChecked);
    releaseSrcpBusLE->setEnabled(isChecked);
    releaseContactLB->setEnabled(isChecked);
    releaseContactSB->setEnabled(isChecked);
    releaseModuleLB->setEnabled(isChecked);
    releaseModuleLE->setEnabled(isChecked);
    releasePortLB->setEnabled(isChecked);
    releasePortLE->setEnabled(isChecked);
}


void RouteDialog::activateContactSBChanged(int contact)
{
    // FB_16 = 0, FB_8 = 1
    int inputs = 16 - (FEEDBACK * 8);
    int module = (contact - 1) / inputs + 1;
    int port = contact - (module - 1) * inputs;
    activateModuleLE->setText(QString::number(module));
    activatePortLE->setText(QString::number(port));
}


void RouteDialog::releaseContactSBChanged(int contact)
{
    int inputs = 16 - (FEEDBACK * 8);
    int module = (contact - 1) / inputs + 1;
    int port = contact - (module - 1) * inputs;
    releaseModuleLE->setText(QString::number(module));
    releasePortLE->setText(QString::number(port));
}


void RouteDialog::typeBGPressed(int btn)
{
    uzsLevelSB->setEnabled(btn == 1);
    ursLevelSB->setEnabled(btn == 4);
}


void RouteDialog::elementsLVChanged(QListViewItem* lvi)
{
    if (lvi == NULL) {
	editPB->setEnabled(false);
	removePB->setEnabled(false);
    }
    else {
	editPB->setEnabled(true);
	removePB->setEnabled(true);
    }
}


void RouteDialog::removeElementFromList()
{
    QListViewItem* lvi = elementsLV->currentItem();
    if (lvi != NULL) {
        elementsLV->takeItem(lvi);
        delete lvi;
    }
}


void RouteDialog::startSignalBusChanged(const QString& bstr)
{
    if (!bstr.isEmpty()) {
        int bus = bstr.toInt();
        int address = startSignalAddressLE->text().toInt();
        updateStartSignalName(bus, address);
    }
}


void RouteDialog::startSignalAddressChanged(const QString& astr)
{
    if (!astr.isEmpty()) {
        int address = astr.toInt();
        int bus = startSignalSrcpBusLE->text().toInt();
        updateStartSignalName(bus, address);
    }
}


void RouteDialog::updateStartSignalName(int bus, int address)
{
    element* el = NULL;
    // send signal to route, routingviewer, gbs
    emit getElementByAddress(bus, address, &el);

    startSignalElPtr = el;
    if (el == NULL)
        startSignalNameLE->setText(tr("Error"));
    else {
        startSignalNameLE->setText(el->getName());
        int ac = el->getAddressCount();
        if (ac == 1)
            startSignalStateSB->setMaxValue(1);
        else
            startSignalStateSB->setMaxValue(3);
    }
}


void RouteDialog::stopSignalBusChanged(const QString& bstr)
{
    if (!bstr.isEmpty()) {
        int bus = bstr.toInt();
        int address = stopSignalAddressLE->text().toInt();
        updateStopSignalName(bus, address);
    }
}


void RouteDialog::stopSignalAddressChanged(const QString& astr)
{
    if (!astr.isEmpty()) {
        int address = astr.toInt();
        int bus = stopSignalSrcpBusLE->text().toInt();
        updateStopSignalName(bus, address);
    }
}


void RouteDialog::updateStopSignalName(int bus, int address)
{
    element* el = NULL;
    // send signal to route, routingviewer, gbs
    emit getElementByAddress(bus, address, &el);

    stopSignalElPtr = el;
    if (el == NULL)
        stopSignalNameLE->setText(tr("Error"));
    else
        stopSignalNameLE->setText(el->getName());
}

