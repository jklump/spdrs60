/***************************************************************************
                           routedialog.cpp
                           version 0.4.7
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : 2005-01-18
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

#include <stdlib.h>             // for system(), atoi()
#include <ctype.h>              // for isdigit()

#include "resources.h"
#include "routedialog.h"
#include "gbsarea.h"            //serd

/*button icons*/
#include "pixmaps/route_apply.xpm"
#include "pixmaps/route_record.xpm"
#include "pixmaps/route_view.xpm"
#include "pixmaps/route_clear.xpm"
#include "pixmaps/route_stop.xpm"
#include "pixmaps/route_start.xpm"
#include "pixmaps/route_editmode.xpm"
#include "pixmaps/route_new.xpm"
#include "pixmaps/route_copy.xpm"



extern bool SHOW_TOOLTIPS;
extern int FEEDBACK;



/* non modal window */
RouteDialog::RouteDialog(QWidget* pParent, QStrList* listOfLockedRoutes_)
:  QDialog(0, "RouteDialog", false)
{
    if (pParent)
        parent = pParent;

    listOfRoutes = new QStrList(true);
    Q_CHECK_PTR(listOfRoutes);

    // copy pointer to list with route status
    listOfLockedRoutes = listOfLockedRoutes_;

    readRoutingFile(NAMES);     // read the routing names
    iRouteNumber = -1;          // no routing selected so far
    iRouteTotal = listOfRoutes->count();

    setupRoutingTable();        // create left half = routing table
    setupProgArea();            // create right half = prog area

    this->move(this->x(), 0);   // move window in upper left corner
    enableProgArea(false, false);       // no route selected, disable all
    // unnecessary widgets in prog area
    slotResizeCommander(0);     // show only left half
    bHasChanged = false;        // no changes done so far
    bBlockSignals = false;      // no blocking of change signals
    bLockTable = false;         // no locking of route table
}


RouteDialog::~RouteDialog()
{
    delete listOfRoutes;
}


void RouteDialog::closeEvent(QCloseEvent * e)
{
    // if something has been changed without saving ask for doing so
    if (bHasChanged) {
        int choice = QMessageBox::warning(this,
                         tr("Warning"),
                         tr("Changes not saved!\nSave route?"),
                         tr("&Yes"), tr("&No"), 0, 1);
        // enter button no = button 0 = "Yes"
        // ecape button no = button 1 = "No"
        // do saving before closing window
        if (choice == 0)
            slotSave();
    }
    this->reject();
    e->accept();
}


void RouteDialog::setupRoutingTable()
{
    // dialog height should be same as parent?
    const int cGroupRouteHeigt = 520;   //serd
    groupRoutes = new QGroupBox("", this, tr("Routes"));
    groupRoutes->setGeometry(10, 12, 320, cGroupRouteHeigt);
    // serd: was unreadable in case of no routes found->fix

    // create and show routing table
    lbRouteTable = new QListBox(groupRoutes, "listBox");
    lbRouteTable->insertStrList(listOfRoutes, 0);

    lbRouteTable->setGeometry(10, 20, groupRoutes->width() - 40 - 20 - 10,
                              cGroupRouteHeigt - 30);
 
    connect(lbRouteTable, SIGNAL(highlighted(int)),
            this, SLOT(slotEnableRouteButton(int)));

    setRouteTableTitle();

    // setup a button to start routing a selected route
    QPixmap pix = QPixmap(route_start_xpm);
    buttStartRouting =
        new QPushButton(tr("Start routing"), groupRoutes, "");
    buttStartRouting->setPixmap(pix);
    buttStartRouting->setGeometry(10 + lbRouteTable->width() + 10,
                                  lbRouteTable->y(), 40, 40);
    if (SHOW_TOOLTIPS)
        QToolTip::add(buttStartRouting,
                      tr
                      ("Click this button to start the selected routing"));
    // disable start route button until a route has been highlighted
    buttStartRouting->setEnabled(false);

    connect(buttStartRouting, SIGNAL(clicked()),
            this, SLOT(slotSendRouteIndex()));

    // setup a button to reset a route
    pix = QPixmap(route_stop_xpm);
    buttStoppRouting =
        new QPushButton(tr("Stopp routing"), groupRoutes, "");
    buttStoppRouting->setPixmap(pix);
    buttStoppRouting->setGeometry(buttStartRouting->x(),
                                  buttStartRouting->y() +
                                  buttStartRouting->height() + 20, 40, 40);

    if (SHOW_TOOLTIPS)
        QToolTip::add(buttStoppRouting,
                      tr
                      ("Click this button to reset the selected routing"));
    // disable stop route button until a route has been highlighted
    buttStoppRouting->setEnabled(false);

    connect(buttStoppRouting, SIGNAL(clicked()),
            this, SLOT(slotSendRouteIndex()));

    // setup a button to show the programming area
    pix = QPixmap(route_editmode_xpm);
    buttSetup = new QPushButton(tr("Setup"), groupRoutes);

    buttSetup->setGeometry(buttStartRouting->x(),
                           buttStartRouting->y() +
                           2 * buttStartRouting->height() + 2 * 20, 40,
                           40);
    //serd: difficulties in placing at the right place down 
    // -> just below stopRouting

    buttSetup->setPixmap(pix);
    buttSetup->setToggleButton(true);
    if (SHOW_TOOLTIPS == true)
        QToolTip::add(buttSetup,
                      tr("Click this button to setup a routing"));

    buttSetup->setFocus();

    connect(buttSetup, SIGNAL(toggled(bool)),
            this, SLOT(slotResizeCommander(bool)));
}


void RouteDialog::readRoutingFile(bool bReadType_)
{
    QString s;
    int iRouteNoRead = 0;
    int iSwitchedElem = 0;

    // if also data must be read first clear all data fields
    if (bReadType_ == DATA) 
        fillProgAreaEmpty(NOTITLE); 

    QFile file(((GBSArea *) parent)->FILENAME + RTS_FILE_SUFFIX);
    // open the routing file
    if (file.open(IO_ReadOnly) == false) {
        // rts file does not exist or can't be opened
        QMessageBox::information(this, tr("Missing routing file"),
                                 tr
                                 ("There's no routing file for this layout.\n"
                                  "An empty routing file has been created."));
        createRoutingFile();
        return;
    }
    QTextStream ts(&file);

    // read routing file
    while (!ts.eof()) {
        s = ts.readLine();

        // only read names of routes
        if (bReadType_ == NAMES) {
            // build list of routing names
            if (s.contains("name:", 0) == true) {
                listOfRoutes->append(s.remove(0, 16));
            }
        }
        // read data of one special route
        else {
            // finds beginning of a section
            if (s.contains("ROUTE -------->", 0) == true) { 
                iRouteNoRead += 1;
                s = ts.readLine();
            }
            if (iRouteNoRead == iRouteNumber + 1) {
                if (s.contains("to signal:", 0) == true) {
                    s = s.remove(0, 16);
                    sRouteStopp = s;    // ready
                }
                else if (s.contains("switch x to y:", 0) == true) {
                    s = s.remove(0, 16);
                    sRouteElem[iSwitchedElem] = s.left(s.find(" ", 0, 0));
                    sRouteElemStat[iSwitchedElem] = s.right(1); // ready
                    if (iSwitchedElem < MAX_SW_ELEM)
                        iSwitchedElem += 1;
                }
                else if (s.contains("from signal:", 0) == true) {
                    s = s.remove(0, 16);
                    sRouteStart = s.left(s.find(" ", 0, 0));    // ready
                    sRouteStartStat = s.right(1);
                }
                else if (s.contains("release port:", 0) == true) {
                    s = s.remove(0, 16);
                    iRouteRelPort = s.toInt();  // ready               
                }
                else if (s.contains("activate port:", 0) == true) {
                    s = s.remove(0, 16);
                    iRouteActPort = s.toInt();  // ready
                }
                else if (s.contains("active by loco:", 0) == true) {
                    s = s.remove(0, 16);
                    sRouteLoco = s;     // TO DO
                }
                else if (s.contains("type:", 0) == true) {
                    s = s.remove(0, 16);
                    tRouteType = (RouteType)s.toInt();     // ready
                }
                else if (s.contains("level:", 0) == true) {
                    s = s.remove(0, 16);
                    iRouteLevel = s.toInt();    // TO DO
                }
            }
        }
    }
    file.close();
}


void RouteDialog::createRoutingFile()
{
    QDateTime dt = QDateTime::currentDateTime();
    QFile file(((GBSArea *) parent)->FILENAME + RTS_FILE_SUFFIX);
    file.open(IO_WriteOnly);
    QTextStream ts(&file);

    // read/write the header section
    ts << "Routes total #: " << "0" << endl;
    ts << "Last modified:  " << dt.toString() << endl;
    ts << "version:        " << VERSION << endl;
    ts << "-------------------------------------" << endl;
    file.close();
}


void RouteDialog::setRouteTableTitle()
{
    QString title;
    int routes = lbRouteTable->count();

    if (routes == 0)
        title = tr("No routes are available!");
    else if (routes == 1)
        title = tr("Routingtable (1 route)");
    else 
        title.sprintf(tr("Routingtable (%d routes)"),
                lbRouteTable->count());

    groupRoutes->setTitle(title);
}


void RouteDialog::slotSendRouteIndex()
{
    // Take the value at the ID of the currently selected route to switch
    // because of SET=0, RESET=1 und LOCKED=1, UNLOCKED=0 we have a wunder-
    // full combination without the need to use a negation.
    // Example: if a route has been SET=0, it gets the LOCKED=1 state. When we
    // want to RESET=1 it, then we just send the actual state value (which is of
    // course LOCKED). Afterwards it is UNLOCKED=0, then we can SET=0 it
    // and so on...
    int lockState = QString(listOfLockedRoutes->at(
                lbRouteTable->currentItem())).toInt();
    // number of route and state of route
    emit sendRouteIndex(lbRouteTable->currentItem(), lockState);
}


void RouteDialog::slotEnableRouteButton(int iRouteTableID_)
{
    // a route has been choosen, so enable the button which represents
    // the LOCKED state of this route and set it to the default button
    // don't do anything here if table is locked
    if (bLockTable)
        return;
/*
    // before showing a new route and the old
    // one hasn't been saved yet, ask for saving
    if (bHasChanged) {
        bLockTable = true;
        lbRouteTable->setCurrentItem(iRouteNumber);

        int choice = QMessageBox::warning(this,
                         tr("Warning"),
                         tr("Changes not saved!\nSave route?"),
                         tr("&Yes"), tr("&No"), 0, 1);
        // enter button no = button 0 = "Yes"
        // ecape button no = button 1 = "No"
        // and save it if yes-button pressed
        if (choice == 0)
            slotSave();

        else {
            bHasChanged = false;
            buttApply->setEnabled(false);
            buttDel->setEnabled(true);
            buttNew->setEnabled(true);
            buttCopy->setEnabled(true);
            buttShow->setEnabled(true);
        }

        lbRouteTable->setCurrentItem(iRouteTableID_);   // now show new selected
        bLockTable = false;     // route at all
    }
*/
    // save number of selected route and reset counter
    iRouteNumber = iRouteTableID_;
    iRouteTotal = lbRouteTable->count(); 
    buttApply->setEnabled(false);
    buttRecord->setEnabled(false);
    buttNew->setEnabled(true);
    //bHasChanged = false;

    int lockState = QString(listOfLockedRoutes->at(iRouteTableID_)).toInt();

    buttStartRouting->setEnabled(lockState == UNLOCKED);
    buttStartRouting->setDefault(lockState == UNLOCKED);

    buttStoppRouting->setEnabled(lockState == LOCKED);
    buttStoppRouting->setDefault(lockState == LOCKED);

    // disable signals from all lineedits, spin
    // boxes etc. for any changes while filling
    // afterwards re-enable signals via variable
    // and if a route is active, adjust availability of some buttons
    bBlockSignals = true;
    fillProgArea(lockState);
    bBlockSignals = false;
    
    if (lockState == UNLOCKED) {
        buttDel->setEnabled(true);
        buttCopy->setEnabled(true);
        buttShow->setEnabled(true);
    }
    else {
        buttDel->setEnabled(false);
        buttCopy->setEnabled(false);
        buttShow->setEnabled(false);
    }
}


void RouteDialog::slotUpdateRouteWindow()
{
    // a route switched by layout clicks renews
    // the state of routing button in table
    slotEnableRouteButton(lbRouteTable->currentItem());
}


void RouteDialog::slotResizeCommander(bool bShowProgArea)
{
    this->setFixedWidth(groupRoutes->width() + 20 +
                        bShowProgArea * (groupActivate->width() + 30 +
                                         groupStartStop->width()));
    this->setFixedHeight(groupRoutes->y() + groupRoutes->height() + 10);
}


void RouteDialog::setupProgArea()
{
    lRouteTitle = new QLabel("", this);
    if (iRouteNumber == -1)
        lRouteTitle->setText(tr("Enter &route title:"));

    lRouteTitle->resize(lRouteTitle->sizeHint());
    lRouteTitle->move(groupRoutes->width() + 30, groupRoutes->y() + 3);

    leRouteName = new QLineEdit(this, "");
    leRouteName->setMaxLength(40);
    leRouteName->resize(300, 20);
    leRouteName->move(groupRoutes->width() + 30 + 250, lRouteTitle->y());

    lRouteTitle->setBuddy(leRouteName);

    connect(leRouteName, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotTextChanged(const QString &)));


    // elements to activate or deactivate a route
    groupActivate =
        new QGroupBox(tr("(De-) Activation"), this, "Activation");

    /*TODO: add Contact and Bus for SRCP 0.8*/
    lMod = new QLabel(tr("Mod #"), groupActivate);
    lMod->move(70, 20);
    lMod->resize(lMod->sizeHint());

    lPort = new QLabel(tr("Port #"), groupActivate);
    lPort->move(140, 20);
    lPort->resize(lPort->sizeHint());

    lRel = new QLabel(tr("Release:"), groupActivate);
    lRel->move(10, lPort->y() + lPort->height() + 6);
    lRel->resize(lRel->sizeHint());

    lAct = new QLabel(tr("Activate:"), groupActivate);
    lAct->move(10, lRel->y() + lRel->height() + 10);
    lAct->resize(lAct->sizeHint());
    lAct->setEnabled(false);

    lAdd = new QLabel(tr("or loco #:"), groupActivate);
    lAdd->move(190, lRel->y() + lRel->height() + 10);
    lAdd->resize(lAdd->sizeHint());
    lAdd->setEnabled(false);

    sbRelMod =
        new QSpinBox(0, 4 * (31 + (FEEDBACK * 31)), 1, groupActivate, "");
    sbRelMod->resize(60, 20);
    sbRelMod->move(lMod->x(), lRel->y());
    sbRelMod->setWrapping(true);
    connect(sbRelMod, SIGNAL(valueChanged(int)),
            this, SLOT(slotValueChanged(int)));
    connect(sbRelMod, SIGNAL(valueChanged(int)),
            this, SLOT(slotDisableRelPort(int)));

    sbRelPort = new QSpinBox(1, 16 - (FEEDBACK * 8), 1, groupActivate, "");
    sbRelPort->resize(40, 20);
    sbRelPort->move(lPort->x(), lRel->y());
    sbRelPort->setWrapping(true);
    sbRelPort->setEnabled(false);
    connect(sbRelPort, SIGNAL(valueChanged(int)),
            this, SLOT(slotValueChanged(int)));

    sbActMod =
        new QSpinBox(0, 4 * (31 + (FEEDBACK * 31)), 1, groupActivate, "");
    sbActMod->resize(60, 20);
    sbActMod->move(lMod->x(), lAct->y());
    sbActMod->setWrapping(true);
    connect(sbActMod, SIGNAL(valueChanged(int)),
            this, SLOT(slotValueChanged(int)));
    connect(sbActMod, SIGNAL(valueChanged(int)),
            this, SLOT(slotDisableActPort(int)));
    sbActMod->setEnabled(false);

    sbActPort = new QSpinBox(1, 16 - (FEEDBACK * 8), 1, groupActivate, "");
    sbActPort->resize(40, 20);
    sbActPort->move(lPort->x(), lAct->y());
    sbActPort->setWrapping(true);
    sbActPort->setEnabled(false);
    connect(sbActPort, SIGNAL(valueChanged(int)),
            this, SLOT(slotValueChanged(int)));

    //lAdd->move(sbActPort->x()+sbActPort->width()+5, lAct->y());

    leLocoAddr = new QLineEdit(groupActivate, "");
    leLocoAddr->resize(30, 20);
    leLocoAddr->move(lAdd->x() + lAdd->width() + 10, sbActPort->y());
    leLocoAddr->setMaxLength(4);
    connect(leLocoAddr, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotTextChanged(const QString &)));
    leLocoAddr->setEnabled(false);

    groupActivate->setGeometry(lRouteTitle->x(),
                               leRouteName->y() + leRouteName->height() +
                               15, 300,
                               sbActMod->y() + sbActMod->height() + 10);


    // elements to set the type of a route
    bgRouteType = new QButtonGroup(this, "Type");
    bgRouteType->setTitle(tr("Type"));
    bgRouteType->move(groupActivate->x(),
                      groupActivate->y() + groupActivate->height() + 10);
    bgRouteType->setExclusive(true);
    bgRouteType->resize(groupActivate->width(), 173);
    connect(bgRouteType, SIGNAL(clicked(int)),
            this, SLOT(slotSaveRouteType(int)));

    rbRouteNormal = new QRadioButton(tr("&Normal route"), bgRouteType);
    rbRouteNormal->move(10, 20);
    rbRouteNormal->resize(rbRouteNormal->sizeHint());
    connect(rbRouteNormal, SIGNAL(clicked()),
            this, SLOT(slotSomethingChanged()));

    rbRouteDetour = new QRadioButton(tr("&Detour route"), bgRouteType);
    rbRouteDetour->move(10, rbRouteNormal->y()+30);
    rbRouteDetour->resize(rbRouteDetour->sizeHint());
    connect(rbRouteDetour, SIGNAL(clicked()),
            this, SLOT(slotSomethingChanged()));

    rbRouteHelp = new QRadioButton(tr("&Help route"), bgRouteType);
    rbRouteHelp->move(10, rbRouteDetour->y()+30);
    rbRouteHelp->resize(rbRouteHelp->sizeHint());
    connect(rbRouteHelp, SIGNAL(clicked()),
            this, SLOT(slotSomethingChanged()));

    rbRouteShuntg = new QRadioButton(tr("Normal &shunting"), bgRouteType);
    rbRouteShuntg->move(10, rbRouteHelp->y()+30);
    rbRouteShuntg->resize(rbRouteShuntg->sizeHint());
    connect(rbRouteShuntg, SIGNAL(clicked()),
            this, SLOT(slotSomethingChanged()));

    rbRouteDShuntg = new QRadioButton(tr("Detour sh&unting"), bgRouteType);
    rbRouteDShuntg->move(10, rbRouteShuntg->y()+30);
    rbRouteDShuntg->resize(rbRouteDShuntg->sizeHint());
    connect(rbRouteDShuntg, SIGNAL(clicked()),
            this, SLOT(slotSomethingChanged()));

    QLabel *label = new QLabel(tr("&Level:"), bgRouteType);
    label->move(178, rbRouteDetour->y()+2);
    label->resize(label->sizeHint());

    sbDetourLevel = new QSpinBox(1, 9, 1, bgRouteType, "DetourSB");
    sbDetourLevel->resize(40, 20);
    sbDetourLevel->setWrapping(false);  // enables to spin "over" the limits
    sbDetourLevel->move(label->x()+label->width() + 6, label->y());
    connect(sbDetourLevel, SIGNAL(valueChanged(int)),
            this, SLOT(slotValueChanged(int)));

    label->setBuddy(sbDetourLevel);

    label = new QLabel(tr("L&evel:"), bgRouteType);
    label->move(178, rbRouteDShuntg->y()+2);
    label->resize(label->sizeHint());

    sbShDetourLevel = new QSpinBox(1, 9, 1, bgRouteType, "ShDetourSB");
    sbShDetourLevel->resize(40, 20);
    sbShDetourLevel->setWrapping(false);  // enables to spin "over" the limits
    sbShDetourLevel->move(sbDetourLevel->x(), label->y());
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
    lName->move(58, 20);
    lName->resize(lName->sizeHint());

    lAddr = new QLabel(tr("Address"), groupStartStop);
    lAddr->move(131, 20);
    lAddr->resize(lAddr->sizeHint());

    lStat = new QLabel(tr("State"), groupStartStop);
    lStat->move(190, 20);
    lStat->resize(lStat->sizeHint());

    lStart = new QLabel(tr("Start:"), groupStartStop);
    lStart->move(10, lStat->y() + lStat->height());
    lStopp = new QLabel(tr("Stop: "), groupStartStop);
    lStopp->move(10, lStart->y() + lStart->height());

    lStartName = new QLabel(groupStartStop, "");
    lStartName->setGeometry(lName->x() + 2, lStart->y() + 6, 60, 20);
    lStartName->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    leStartAddr = new QLineEdit(groupStartStop, "");
    leStartAddr->setGeometry(lAddr->x(), lStart->y() + 5, 45, 20);
    leStartAddr->setMaxLength(4);
    connect(leStartAddr, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotTextChanged(const QString&)));
    connect(leStartAddr, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotAddressChanged(const QString&)));

    leStartStat = new QLineEdit(groupStartStop, "");
    leStartStat->setGeometry(lStat->x(), lStart->y() + 5, 35, 20);
    leStartStat->setMaxLength(1);
    connect(leStartStat, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotTextChanged(const QString&)));
    connect(leStartStat, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotStatusChanged(const QString&)));

    lStoppName = new QLabel(groupStartStop, "");
    lStoppName->setGeometry(lName->x() + 2, lStopp->y() + 6, 60, 20);
    lStoppName->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    leStoppAddr = new QLineEdit(groupStartStop, "");
    leStoppAddr->setGeometry(lAddr->x(), lStopp->y() + 5, 45, 20);
    leStoppAddr->setMaxLength(4);
    connect(leStoppAddr, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotTextChanged(const QString&)));
    connect(leStoppAddr, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotAddressChanged(const QString&)));


    // the elements which are switched in a route
    groupElements = new QGroupBox(tr("Routed elements"), this, "Elements");
    groupElements->setGeometry(groupStartStop->x(),
                               bgRouteType->y(),
                               groupStartStop->width(),
                               366);


    lNo = new QLabel(tr("No"), groupElements);
    lNo->resize(lNo->sizeHint());
    lNo->move(40 - lNo->width(), 20);
    //lNo->setAlignment(Qt::AlignRight);

    lName2 = new QLabel(tr("Name"), groupElements);
    lName2->move(lName->x(), 20);
    lName2->resize(lName2->sizeHint());

    lAddr2 = new QLabel(tr("Address"), groupElements);
    lAddr2->move(lAddr->x(), 20);
    lAddr2->resize(lAddr2->sizeHint());

    lStat2 = new QLabel(tr("State"), groupElements);
    lStat2->move(lStat->x(), 20);
    lStat2->resize(lStat2->sizeHint());

    QString s;
    for (int j = 0; j < MAX_SW_ELEM; j++) {
        s.sprintf("%d", j + 1);
        lElem[j] = new QLabel(s, groupElements, "noLabel");
        lElem[j]->text().setNum(j+1);
        lElem[j]->resize(lElem[j]->sizeHint());
        lElem[j]->move(lNo->x()+ lNo->width()- lElem[j]->width(),
                lName->y() + lName->height() + 5 + 21 * j);
        lElem[j]->setAlignment(Qt::AlignRight);

        lElemName[j] = new QLabel(groupElements, "elemName");
        lElemName[j]->setGeometry(lName2->x() + 2, lElem[j]->y() - 1, 60,
                                  20);
        lElemName[j]->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        lElemName[j]->setFocusPolicy(QWidget::NoFocus);

        leElemAddr[j] = new QLineEdit(groupElements, "elemAddress");
        leElemAddr[j]->setGeometry(lAddr2->x(), lElem[j]->y() - 1, 45, 20);
        leElemAddr[j]->setMaxLength(4);
        leElemAddr[j]->setFocusPolicy(QWidget::StrongFocus);
        connect(leElemAddr[j], SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(leElemAddr[j], SIGNAL(textChanged(const QString &)),
                this, SLOT(slotAddressChanged(const QString &)));

        leElemStat[j] = new QLineEdit(groupElements, "elemState");
        leElemStat[j]->setGeometry(lStat2->x(), lElem[j]->y() - 1, 35, 20);
        leElemStat[j]->setMaxLength(1);
        leElemStat[j]->setFocusPolicy(QWidget::StrongFocus);
        connect(leElemStat[j], SIGNAL(textChanged(const QString &)),
                this, SLOT(slotTextChanged(const QString &)));
        connect(leElemStat[j], SIGNAL(textChanged(const QString &)),
                this, SLOT(slotStatusChanged(const QString &)));
    }


    // all buttons for the actions to be done with all the routings
    buttApply = new QPushButton(tr("Apply"), this, "applyBtn");
    QPixmap pix = QPixmap(route_apply_xpm);
    buttApply->setPixmap(pix);
    buttApply->resize(30, 30);
    buttApply->move(groupActivate->x(),
                    groupRoutes->y() + groupRoutes->height() -
                    buttApply->height());
    if (SHOW_TOOLTIPS == true)
        QToolTip::add(buttApply, tr("Apply route changes (CTRL+S)"));
    connect(buttApply, SIGNAL(clicked()), this, SLOT(slotSave()));

    buttDel = new QPushButton(tr("Delete"), this, "delBtn");
    pix = QPixmap(route_clear_xpm);
    buttDel->setPixmap(pix);
    buttDel->resize(30, 30);
    buttDel->move(buttApply->x() + buttApply->width() + 10,
                  buttApply->y());
    if (SHOW_TOOLTIPS == true)
        QToolTip::add(buttDel, tr("Delete this route (CTRL+D)"));
    connect(buttDel, SIGNAL(clicked()), this, SLOT(slotDelRoute()));

    buttNew = new QPushButton(tr("New"), this, "newBtn");
    pix = QPixmap(route_new_xpm);
    buttNew->setPixmap(pix);
    buttNew->resize(30, 30);
    buttNew->move(buttDel->x() + buttDel->width() + 10, buttDel->y());
    if (SHOW_TOOLTIPS == true)
        QToolTip::add(buttNew, tr("Add a new route (CTRL+A)"));
    connect(buttNew, SIGNAL(clicked()), this, SLOT(slotNewRoute()));

    buttCopy = new QPushButton(tr("Copy"), this, "copyBtn");
    pix = QPixmap(route_copy_xpm);
    buttCopy->setPixmap(pix);
    buttCopy->resize(30, 30);
    buttCopy->move(buttNew->x() + buttNew->width() + 10, buttNew->y());
    if (SHOW_TOOLTIPS == true)
        QToolTip::add(buttCopy, tr("Copy this route (CTRL+C)"));
    connect(buttCopy, SIGNAL(clicked()), this, SLOT(slotCopyRoute()));

    buttRecord = new QPushButton(tr("Record"), this, "recordBtn");
    pix = QPixmap(route_record_xpm);
    buttRecord->setPixmap(pix);
    buttRecord->resize(30, 30);
    buttRecord->move(buttCopy->x() + buttCopy->width() + 40,
                     buttCopy->y());
    if (SHOW_TOOLTIPS == true)
        QToolTip::add(buttRecord, tr("Record new route (CTRL+R)"));
    connect(buttRecord, SIGNAL(clicked()), this, SLOT(slotRecordRoute()));

    buttShow = new QPushButton(tr("Show"), this, "showBtn");
    pix = QPixmap(route_view_xpm);
    buttShow->setPixmap(pix);
    buttShow->resize(30, 30);
    buttShow->move(buttRecord->x() + buttRecord->width() + 40,
                   buttRecord->y());
    if (SHOW_TOOLTIPS == true)
        QToolTip::add(buttShow, tr("Show this route in layout"));
    connect(buttShow, SIGNAL(clicked()), this, SLOT(slotShowRoute()));

    QAccel *c = new QAccel(this);
    c->connectItem(c->insertItem(CTRL + Key_C), this,
                   SLOT(slotCopyRoute()));

    QAccel *ss = new QAccel(this);
    ss->connectItem(ss->insertItem(CTRL + Key_S), this, SLOT(slotSave()));

    QAccel *n = new QAccel(this);
    n->connectItem(n->insertItem(CTRL + Key_A), this,
                   SLOT(slotNewRoute()));

    QAccel *d = new QAccel(this);
    d->connectItem(d->insertItem(CTRL + Key_D), this,
                   SLOT(slotDelRoute()));

    QAccel *r = new QAccel(this);
    r->connectItem(r->insertItem(CTRL + Key_R), this,
                   SLOT(slotRecordRoute()));



    // on setup no route is selected -> disable most buttons
    buttApply->setEnabled(false);
    buttDel->setEnabled(false);
    buttCopy->setEnabled(false);
    buttRecord->setEnabled(false);
    buttShow->setEnabled(false);
    buttNew->setEnabled(true);  // just for understanding!
}


void RouteDialog::slotSaveRouteType(int tRouteType_)
{
   // save button ID of route type selector

    tRouteType = (RouteType)tRouteType_;
    if (tRouteType != kDetour)
        sbDetourLevel->setEnabled(false);
    else
        sbDetourLevel->setEnabled(true);
    if (tRouteType != kShuntingD)
        sbShDetourLevel->setEnabled(false);
    else
        sbShDetourLevel->setEnabled(true);
}


void RouteDialog::fillProgArea(int iRouteState_)
{
    QString s;                  // set route title
    s.sprintf(tr("Route #%d"), iRouteNumber + 1);
    lRouteTitle->setText(s);
    lRouteTitle->resize(lRouteTitle->sizeHint());

    readRoutingFile(DATA);

    sRouteName = lbRouteTable->text(iRouteNumber);
    leRouteName->setText(sRouteName);   // fill in everything else

    leStartStat->setText(sRouteStartStat);
    leStartAddr->setText(sRouteStart);
    sReadElemName = "";
    emit sigReadElemName(sRouteStart);  // get name of start signal
    do {
    } while (sReadElemName == "");      // when reading route from
    lStartName->setText(sReadElemName); // file

    leStoppAddr->setText(sRouteStopp);
    sReadElemName = "";
    emit sigReadElemName(sRouteStopp);  // same for stop signal
    do {
    } while (sReadElemName == "");
    lStoppName->setText(sReadElemName);

    if (iRouteRelPort != -1) {
        sbRelMod->setValue(iRouteRelPort / (16 - FEEDBACK * 8) + 1);
        sbRelPort->setValue(iRouteRelPort % (16 - FEEDBACK * 8) + 1);
    }
    else {
        sbRelMod->setValue(0);
        sbRelPort->setValue(1);
        sbRelPort->setEnabled(false);
    }

    if (iRouteActPort != -1) {
        sbActMod->setValue(iRouteActPort / (16 - FEEDBACK * 8) + 1);
        sbActPort->setValue(iRouteActPort % (16 - FEEDBACK * 8) + 1);
    }
    else {
        sbActMod->setValue(0);
        sbActPort->setValue(1);
        sbActPort->setEnabled(false);
    }

    bgRouteType->setButton(tRouteType);
    //leLocoAddr->setText( sRouteLoco );
    
    if (tRouteType == kDetour)
        sbDetourLevel->setValue(iRouteLevel);
    else if (tRouteType == kShuntingD)
        sbShDetourLevel->setValue(iRouteLevel);

    // display all switched elems
    for (int i = 0; i < MAX_SW_ELEM; i++) {
        leElemStat[i]->setText(sRouteElemStat[i]);
        leElemAddr[i]->setText(sRouteElem[i]);

        sReadElemName = "";
        if (sRouteElemStat[i] != "" && sRouteElem[i] != "") {
            // same for all switched elems
            emit sigReadElemName(sRouteElem[i]);
            do {
            } while (sReadElemName == "");
            lElemName[i]->setText(sReadElemName);
        }
    }

    if (iRouteState_ == LOCKED)
        enableProgArea(true, false);    // show Labels, but no data
    else
        enableProgArea(true, true);     // show labels AND data
}


void RouteDialog::fillProgAreaEmpty(bool bShowNewRouteName_)
{
    QString s;
    s.sprintf(tr("&Route #%d:"), iRouteNumber + 1);
    lRouteTitle->setText(s);
    lRouteTitle->resize(lRouteTitle->sizeHint());

    if (bShowNewRouteName_ == TITLE) {
        leRouteName->setText(tr("Enter name of new route (max 40 chars)"));
        leRouteName->selectAll();
        leRouteName->setFocus();
    }
    // clear all existent fields
    sbRelMod->setValue(0);
    sbRelPort->setValue(0);
    sbActMod->setValue(0);
    sbActPort->setValue(0);
    leLocoAddr->setText("");

    rbRouteNormal->setChecked(true);
    slotSaveRouteType(0);

    lStartName->setText("");
    leStartAddr->setText("");
    leStartStat->setText("");
    lStoppName->setText("");
    leStoppAddr->setText("");

    for (int i = 0; i < MAX_SW_ELEM; i++) {
        lElemName[i]->setText("");
        leElemAddr[i]->setText("");
        leElemStat[i]->setText("");
        sRouteElem[i] = "";
        sRouteElemStat[i] = "";
    }
}


void RouteDialog::slotSave()
{
    slotSaveRoute(ADDCHANGE);
}


void RouteDialog::slotSaveRoute(bool bMode_)
{
    if (readProgFields(bMode_) == INVALID)
        return;

    // write routing file
    QDateTime dt = QDateTime::currentDateTime();
    // open old routing file for reading only
    QFile file1(((GBSArea *) parent)->FILENAME + RTS_FILE_SUFFIX);
    file1.open(IO_ReadOnly);

    // open temp file = new routing file for writing only
    QFile file2(((GBSArea *) parent)->FILENAME + RTS_FILE_SUFFIX + ".tmp");
    file2.open(IO_WriteOnly);

    QTextStream ts1(&file1);
    QTextStream ts2(&file2);
    int bRouteDeleted = false;

    // read/write the header section
    QString s1;
    QString s2;
    int iRouteNoRead = 0;
    for (int i = 0; i < 4; i++)
        s1 = ts1.readLine();    // read the old header (and forget it)
    ts2 << "Routes total #: " << iRouteTotal << endl;
    ts2 << "Last modified:  " << dt.toString() << endl;
    ts2 << "version:        " << VERSION << endl;
    ts2 << "-------------------------------------" << endl;

    do {
        s1 = ts1.readLine();    // read line with route number information
        if (s1.contains("ROUTE -------->", 0) == true)
            iRouteNoRead += 1;  // count routes in routing files

        // if a route is changed or added do this
        // just copy the routes with different number
        if ((iRouteNoRead != iRouteNumber + 1) && (bMode_ == ADDCHANGE))
            ts2 << s1 << endl;

        // if a route is deleted do this
        // lines BEFORE deleted route are copied
        else if ((iRouteNoRead != iRouteNumber + 1) && (bMode_ == DELETE)) {
            if (bRouteDeleted == false)
                ts2 << s1 << endl;
             // AFTER adjust the route number ...
             // .. and copy the rest infos of a route
            else  {
                if (s1.contains("ROUTE -------->", 0) == true)
                    ts2 << "ROUTE --------> " << iRouteNoRead - 1 << endl;
                else
                    ts2 << s1 << endl;
            }
        }

        if (iRouteNoRead == iRouteNumber + 1 ||
            (ts1.eof() && (iRouteNoRead < iRouteNumber + 1))) {

            if (bMode_ == ADDCHANGE) {

                ts2 << "ROUTE --------> " << iRouteNumber + 1 << endl;
                ts2 << "name:           " << sRouteName << endl;
                ts2 << "to signal:      " << addZeros(sRouteStopp) << endl;
                for (int i = 0; i < MAX_SW_ELEM; i++)
                    if (sRouteElem[i] != "" && sRouteElemStat[i] != "")
                        ts2 << "switch x to y:  " <<
                            addZeros(sRouteElem[i]) << " " <<
                            sRouteElemStat[i] << endl;
                ts2 << "from signal:    " << addZeros(sRouteStart) << " "
                    << sRouteStartStat << endl;
                ts2 << "release port:   " << iRouteRelPort << endl;
                ts2 << "activate port:  " << iRouteActPort << endl;
                ts2 << "active by loco: " << "-1" /*sRouteLoco */  << endl;
                ts2 << "type:           " << tRouteType << endl;
                ts2 << "level:          " << iRouteLevel << endl;
                /*ts2 << "data1:          " << "" << endl;
                ts2 << "data2:          " << "" << endl;
                ts2 << "data3:          " << "" << endl;*/
                ts2 << "-------------------------------------" << endl;
            }
            else if (bMode_ == DELETE) {
                do
                    s1 = ts1.readLine();
                while (s1 != "-------------------------------------");
                bRouteDeleted = true;
            }
            if (!ts1.eof() && (bMode_ == ADDCHANGE))
                do
                    s1 = ts1.readLine();
                while (s1 != "-------------------------------------");
        }
    }
    while (!ts1.eof());
    file1.close();
    file2.close();

    QString s;
    s.sprintf(tr(">Writing routes: %s%s"),
              ((GBSArea *) parent)->FILENAME.data(), RTS_FILE_SUFFIX);
    emit cmdToDebug(s);

    // copy temp-file to original file
    s.sprintf("mv %s%s.tmp %s%s",
              ((GBSArea *) parent)->FILENAME.data(), RTS_FILE_SUFFIX,
              ((GBSArea *) parent)->FILENAME.data(), RTS_FILE_SUFFIX);
    system(s);

    if (bMode_ == ADDCHANGE) {
        // insert new route
        if (iRouteNumber == (int) lbRouteTable->count()) {
            lbRouteTable->insertItem(sRouteName, -1);
            setRouteTableTitle();
        }
        // or update name
        else {
            //lbRouteTable->changeItem(sRouteName, lbRouteTable->currentItem());
            lbRouteTable->changeItem(sRouteName, iRouteNumber);
        }
    }
    lbRouteTable->setCurrentItem(iRouteNumber);

    buttNew->setEnabled(true);
    buttCopy->setEnabled(true);
    buttDel->setEnabled(true);
    buttApply->setEnabled(false);
    buttRecord->setEnabled(false);
    buttStartRouting->setFocus();
    bHasChanged = false;

    /* send update signal to gbsarea */
    emit sendReloadRoutes();
}


QString RouteDialog::addZeros(QString sNZString_)       // N on  Z ero  String
{
    QString s = sNZString_;     // string maybe without leading zeros
    while (s.length() < 4)
        s.prepend("0");

    return s;                   // string with leading zeros
}


int RouteDialog::readProgFields(bool bMode_)
{
    // read out all screen elements and save in vars
    if (bMode_ == ADDCHANGE) {
        // new locking entry for new route
        listOfLockedRoutes->append("0");
        // save all information in  routing variables
        sRouteName = leRouteName->text();

        if (sbRelMod->value() > 0)
            iRouteRelPort =
                (sbRelMod->value() - 1) * (16 - FEEDBACK * 8) +
                sbRelPort->value() - 1;
        else
            iRouteRelPort = -1;

        if (sbActMod->value() > 0)
            iRouteActPort =
                (sbActMod->value() - 1) * (16 - FEEDBACK * 8) +
                sbActPort->value() - 1;
        else
            iRouteActPort = -1;

        if (leLocoAddr->text() != "")
            sRouteLoco = leLocoAddr->text();
        else
            sRouteLoco = "-1";

        // tRouteType = saved by slot slotSaveRouteType( int )

        if (tRouteType == kDetour) // detour routing
            iRouteLevel = sbDetourLevel->value();
        else if (tRouteType == kShuntingD)
            iRouteLevel = sbShDetourLevel->value();
        else
            iRouteLevel = -1;

        sRouteStart = leStartAddr->text();
        sRouteStartStat = leStartStat->text();
        sRouteStopp = leStoppAddr->text();

        if (sRouteStart == "" || sRouteStartStat == ""
            || sRouteStopp == "") {
            QMessageBox::information(this, tr("Information"),
                             tr("You did not fill in one or more fields\n"
                                "concerning start and/or stop signals!\n\n"
                                "Please correct this before saving!"));
            return INVALID;
        }

        for (int i = 0; i < MAX_SW_ELEM; i++) {
            sRouteElem[i] = leElemAddr[i]->text();
            sRouteElemStat[i] = leElemStat[i]->text();
            if ((sRouteElem[i] == "" && sRouteElemStat[i] != "") ||
                (sRouteElem[i] != "" && sRouteElemStat[i] == "")) {
                QMessageBox::information(this, tr("Information"),
                                tr("You filled in an element's address\n"
                                   "but not it's state or vice versa!\n\n"
                                   "Please correct this before saving!"));
                return INVALID;
            }

        }
    }
    else {
        // remove locking entry for deleted route
        listOfLockedRoutes->remove(iRouteNumber);
    }
    return VALID;
}


void RouteDialog::slotDelRoute()
{
    int choice = QMessageBox::warning(this,
                                  tr("Warning"),
                                  tr("Do you want to delete this route?"),
                                  tr("&Yes"), tr("&No"), 0, 1);
    // enter button no = button 0 = "Yes"
    // ecape button no = button 1 = "No"
    if (choice == 1)
        return;

    iRouteTotal -= 1;
    if (iRouteTotal == 0)
        buttDel->setEnabled(false); // no more routes disable delete button
    setRouteTableTitle();

    slotSaveRoute(DELETE);      // first delete route in file
    lbRouteTable->removeItem(iRouteNumber);

    slotEnableRouteButton(iRouteNumber);
}


void RouteDialog::slotNewRoute()
{
    iRouteNumber = lbRouteTable->count();
    iRouteTotal += 1;

    enableProgArea(true, true);
    fillProgAreaEmpty(TITLE);   // clear all fields

    //slotSomethingChanged();   is automatically called within fillProgAreaEmpty
    // just for information; do NOT uncomment this!
    buttNew->setEnabled(false);
    buttDel->setEnabled(false);
    buttCopy->setEnabled(false);
    buttShow->setEnabled(false);
    buttRecord->setEnabled(true);
    // avoid being asked for changes because all is changed
    bHasChanged = false;
}


void RouteDialog::slotCopyRoute()
{
    iRouteNumber = lbRouteTable->count();
    iRouteTotal += 1;

    enableProgArea(true, true);

    // copy the old name and set focus to new name that user can direct
    // enter his new name
    sRouteName = tr("Copy of ") + sRouteName;
    leRouteName->setText(sRouteName);
    leRouteName->selectAll();
    leRouteName->setFocus();

    QString s;
    s.sprintf(tr("Route #%d:"), iRouteNumber + 1);
    lRouteTitle->setText(s);
    lRouteTitle->resize(lRouteTitle->sizeHint());

    slotSomethingChanged();
    buttNew->setEnabled(false);
    buttDel->setEnabled(false);
    buttCopy->setEnabled(false);
    buttShow->setEnabled(false);
}


void RouteDialog::slotRecordRoute()
{
    //fillProgAreaEmpty( NOTITLE );
    // clear all data fields  serd: not to be done here
    // serd: after changing must be able to save route
    buttApply->setEnabled(true);
    cmdToDebug(tr(">Recording new route ..."));
    emit sigRecord(REC_START);  // and activate recording of elems
    this->hide();               // while hiding route window
}


void RouteDialog::slotRecordElement(int iRecordAddress_,
                                    QString sRecordName_,
                                    int iRecordStatus_, int iRecordType_)
{
    QString sA, sS, s;
    sA.sprintf("%04d", iRecordAddress_);
    sS.sprintf("%1d", iRecordStatus_);

    // fill start/stop if never filled before
    if (iRecordType_ == REC_STASTO) {
        s = leStartAddr->text();
        // first STASTO click for start signal
        if (s.length() == 0) {
            leStartAddr->setText(sA);
            lStartName->setText(sRecordName_);
            leStartStat->setText(sS);
        }
        //if (leStoppAddr != 0     // other STASTO click for start signal
        else {
            s = leStoppAddr->text();
            if (s.length() == 0) {
                leStoppAddr->setText(sA);
                lStoppName->setText(sRecordName_);
            }
        }
    }

    // all others NORMAL are elements
    else if (iRecordType_ == REC_NORMAL) {
        int i = -1;
        // find empty line
        do {
            i += 1;
            s = leElemAddr[i]->text();
        }
        while (s.length() > 0);

        leElemAddr[i]->setText(sA);
        lElemName[i]->setText(sRecordName_);
        leElemStat[i]->setText(sS);

        if (i == MAX_SW_ELEM - 1)       // automatically stop recording if max
            stopRecord();       // switchable elements are recorded
    }

     // empty element clicked -> user thinks route is finished
    else 
        stopRecord();
}


void RouteDialog::stopRecord()
{
    emit sigRecord(REC_STOPP);  // send "stop record" mode to all
    this->show();               // and show routing window again
}


void RouteDialog::slotShowRoute()
{
    QString sA, sS;
    emit sigRecord(REC_SHOW);

    sA = leStartAddr->text();   // show start signal with correct direction
    sS = leStartStat->text();
    emit sigShowElement(sA.toInt(), sS.toInt(), R_SHOW_STA);

    sA = leStoppAddr->text();   // same for stop signal
    emit sigShowElement(sA.toInt(), -1, R_SHOW_STO);
    // same for all elements to be switched
    for (int i = 0; i < MAX_SW_ELEM; i++) {
        sA = leElemAddr[i]->text();
        sS = leElemStat[i]->text();
        if (sA.length() != 0 && sS.length() != 0)
            emit sigShowElement(sA.toInt(), sS.toInt(), R_SHOW_ELM);
    }
    this->hide();               // hide routing window
}


void RouteDialog::slotDisableRelPort(int iRelModValue_)
{
    if (iRelModValue_ == 0)     // means no feedback function expected for
        sbRelPort->setEnabled(false);   // deactivation
    else
        sbRelPort->setEnabled(true);
}


void RouteDialog::slotDisableActPort(int iActModValue_)
{
    if (iActModValue_ == 0)     // means no feedback function expected for
        sbActPort->setEnabled(false);   // activation
    else
        sbActPort->setEnabled(true);
}


void RouteDialog::slotValueChanged(int iDummy_)
{
    if (iDummy_);               // a number field has changed
    if (bBlockSignals == false)
        slotSomethingChanged();
}


void RouteDialog::slotTextChanged(const QString & cDummy_)
{
    if (cDummy_);               // a text field has changed
    if (bBlockSignals == false)
        slotSomethingChanged();
}


void RouteDialog::slotSomethingChanged()
{
    // if some field has been changed disable most buttons so that user
    // must save this changes
    if (bBlockSignals == false) { 
        bHasChanged = true; 

        buttApply->setEnabled(true);
        buttDel->setEnabled(false);
        buttCopy->setEnabled(false);
        buttNew->setEnabled(false);
    }
}


void RouteDialog::enableProgArea(int iLabelEnable_, int iFieldsEnable_)
{
    // if a route is active, only show the labels clear, deactivate all fields
    // if a route is inactive, show all
    leRouteName->setEnabled(iFieldsEnable_);

    for (int j = 0; j < 15; j++) {
        lElem[j]->setEnabled(iLabelEnable_);
        lElemName[j]->setEnabled(iFieldsEnable_);
        leElemAddr[j]->setEnabled(iFieldsEnable_);
        leElemStat[j]->setEnabled(iFieldsEnable_);
    }

    lStart->setEnabled(iLabelEnable_);
    lStopp->setEnabled(iLabelEnable_);
    lMod->setEnabled(iLabelEnable_);
    lPort->setEnabled(iLabelEnable_);
    lRel->setEnabled(iLabelEnable_);
    lAct->setEnabled(iLabelEnable_);
    //lAdd->setEnabled( iLabelEnable_ );

    bgRouteType->setEnabled(iLabelEnable_);
    sbDetourLevel->setEnabled(rbRouteDetour->isChecked());
    sbShDetourLevel->setEnabled(rbRouteDShuntg->isChecked());
    rbRouteNormal->setEnabled(iFieldsEnable_);
    rbRouteDetour->setEnabled(iFieldsEnable_);
    rbRouteHelp->setEnabled(iFieldsEnable_);
    rbRouteShuntg->setEnabled(iFieldsEnable_);
    rbRouteDShuntg->setEnabled(iFieldsEnable_);

    lName->setEnabled(iLabelEnable_);
    lAddr->setEnabled(iLabelEnable_);
    lStat->setEnabled(iLabelEnable_);
    lStartName->setEnabled(iFieldsEnable_);
    lStoppName->setEnabled(iFieldsEnable_);
    leStartAddr->setEnabled(iFieldsEnable_);
    leStartStat->setEnabled(iFieldsEnable_);
    leStoppAddr->setEnabled(iFieldsEnable_);

    sbRelMod->setEnabled(iFieldsEnable_);
    sbRelPort->setEnabled(iFieldsEnable_);
    if (sbRelMod->value() == 0)
        sbRelPort->setEnabled(false);

    sbActMod->setEnabled(iFieldsEnable_);
    sbActPort->setEnabled(iFieldsEnable_);
    if (sbActMod->value() == 0)
        sbActPort->setEnabled(false);

    //leLocoAddr->setEnabled( iFieldsEnable_ );

    groupActivate->setEnabled(iLabelEnable_);
    groupStartStop->setEnabled(iLabelEnable_);
    groupElements->setEnabled(iLabelEnable_);

    lName2->setEnabled(iLabelEnable_);
    lAddr2->setEnabled(iLabelEnable_);
    lStat2->setEnabled(iLabelEnable_);
}


void RouteDialog::slotAddressChanged(const QString & cNewAddress_)
{
    QString sCorrection = cNewAddress_;

    // a zero length is o.k.
    if (sCorrection.length() == 0)
        return;

    // now reject character input if it was not a digit
    for (uint i = 0; i < sCorrection.length(); i++) {

        if (isdigit(cNewAddress_[i]) == false) {
            // correction code replaces the wrong user entry in
            // line edits "address"
            sCorrection.replace(i, 1, '\0');
            if (leStartAddr->text() == cNewAddress_) { 
                leStartAddr->setText(sCorrection);
                leStartAddr->setCursorPosition(i);
            }
            else if (leStoppAddr->text() == cNewAddress_) {
                leStoppAddr->setText(sCorrection);
                leStoppAddr->setCursorPosition(i);
            }
            else
                for (int j = 0; j < MAX_SW_ELEM; j++) {
                    if (leElemAddr[j]->text() == cNewAddress_) {
                        leElemAddr[j]->setText(sCorrection);
                        leElemAddr[j]->setCursorPosition(i);
                        break;
                    }
                }
            break;
        }
    }
}


void RouteDialog::slotStatusChanged(const QString & cNewStatus_)
{
    QString sCorrection = cNewStatus_;

    // a zero length is o.k.
    if (sCorrection.length() == 0)
        return;

    // now reject character input if it was not a digit
    for (uint i = 0; i < sCorrection.length(); i++) {

        if ((isdigit(cNewStatus_[i]) == false)
            || (sCorrection.toInt() >= 4)) {
            // correction code replaces the wrong user entry in
            // line edits "status"
            sCorrection.replace(i, 1, '\0');
            if (leStartStat->text() == cNewStatus_) {
                leStartStat->setText(sCorrection);
                leStartStat->setCursorPosition(i);
            }
            else
                for (int j = 0; j < MAX_SW_ELEM; j++) {
                    if (leElemStat[j]->text() == cNewStatus_) {
                        leElemStat[j]->setText(sCorrection);
                        leElemStat[j]->setCursorPosition(i);
                        break;
                    }
                }
            break;
        }
    }
}
