/***************************************************************************
                           newlayoutdialog.cpp
                           version 0.5.3 $Revision: 1.22 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                           (C) 2004-2008 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-05-04 18:44:44 $
****************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
   this code provides an user interface to enter the number of new columns
 ***************************************************************************/

#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qvalidator.h>

#include "newlayoutdialog.h"
#include "preferences.h"
#include "resources.h"


newLayoutDialog::newLayoutDialog(QWidget* parent)
: QDialog(parent, "newLayoutDialog", true)
{
    setCaption(tr("Create new layout"));
    QVBoxLayout* baseLayout = new QVBoxLayout(this, 10, 6);

    // layout dimensions group box
    QGroupBox* dimensionsGB = new QGroupBox(0, Qt::Horizontal,
            tr("Layout dimensions"), this, "dimensionsGB");
    baseLayout->addWidget(dimensionsGB);
    QVBoxLayout* boxL = new QVBoxLayout(dimensionsGB->layout(), 6);

    //line with column number
    QHBoxLayout* columnsLayout = new QHBoxLayout(boxL);
    QLabel* label = new QLabel(tr("&Columns:"), dimensionsGB);
    columnsLayout->addWidget(label);
    QSpacerItem* spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    columnsLayout->addItem(spacer);
    sbEnterCols = new QSpinBox(MIN_COLS, MAX_COLS, 1, dimensionsGB,
            "sbEnterCols");
    columnsLayout->addWidget(sbEnterCols);
    label->setBuddy(sbEnterCols);
    sbEnterCols->setWrapping(true);     // enables to spin "over" the limits
    QToolTip::add(sbEnterCols, tr(
                "Choose or enter the number of\n"
                "columns for your layout.\n"
                "Valid range is %1..%2.").arg(MIN_COLS).arg(MAX_COLS));

    //line with row number
    QHBoxLayout* rowsLayout = new QHBoxLayout(boxL);
    label = new QLabel(tr("&Rows:"), dimensionsGB);
    rowsLayout->addWidget(label);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    rowsLayout->addItem(spacer);
    sbEnterRows = new QSpinBox(MIN_ROWS, MAX_ROWS, 1, dimensionsGB,
            "sbEnterCols");
    rowsLayout->addWidget(sbEnterRows);
    label->setBuddy(sbEnterRows);
    sbEnterRows->setWrapping(true);     // enables to spin "over" the limits
    QToolTip::add(sbEnterRows, tr(
                "Choose or enter the number\n"
                "of rows for your layout.\n"
                "Valid range is %1..%2.").arg(MIN_ROWS).arg(MAX_ROWS));

    // identification group box
    QGroupBox *identificationGB = new QGroupBox(0, Qt::Horizontal,
            tr("CRCF Identification"), this, "identificationGB");
    baseLayout->addWidget(identificationGB);
    QVBoxLayout* identificationGBL = new QVBoxLayout(
            identificationGB->layout(), 6);

    // line with id
    QHBoxLayout* idL = new QHBoxLayout(identificationGBL);
    label = new QLabel(tr("&Id:"), identificationGB);
    idL->addWidget(label);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    idL->addItem(spacer);
    idLE = new QLineEdit(identificationGB, "idLE");
    idL->addWidget(idLE);
    idLE->setMaximumWidth(100);
    idLE->setMaxLength(6);
    QValidator* idValidator = new QIntValidator(0, 999999, identificationGB);
    idLE->setValidator(idValidator);
    label->setBuddy(idLE);
    QToolTip::add(idLE, tr(
                "Enter the identification\n"
                "number of this layout.\n"
                "Valid range is 0..999999."));

    // line with layout name
    QHBoxLayout* nameL = new QHBoxLayout(identificationGBL);
    label = new QLabel(tr("&Name:"), identificationGB);
    nameL->addWidget(label);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    nameL->addItem(spacer);

    nameLE = new QLineEdit(identificationGB, "nameLE");
    nameL->addWidget(nameLE);
    nameLE->setMaximumWidth(100);
    label->setBuddy(nameLE);
    QToolTip::add(nameLE, tr(
                "Enter the name of this layout.\n"
                "This data is currently only used\n"
                "for CRCF/Generic Message purposes."));

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    identificationGBL->addItem(spacer);

    // server group box
    QGroupBox *serverGB = new QGroupBox(0, Qt::Horizontal,
            tr("SRCP-Server"), this, "serverGB");
    baseLayout->addWidget(serverGB);
    QVBoxLayout* serverGBL = new QVBoxLayout(serverGB->layout(), 6);

    // line with host name
    QHBoxLayout* hostL = new QHBoxLayout(serverGBL);
    label = new QLabel(tr("&Hostname:"), serverGB);
    hostL->addWidget(label);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    hostL->addItem(spacer);
    hostLE = new QLineEdit(serverGB, "host");
    hostL->addWidget(hostLE);
    hostLE->setMaximumWidth(100);
    label->setBuddy(hostLE);
    QToolTip::add(hostLE, tr(
                "Enter the hostname or IP address\n"
                "of your SRCP server."));

    // line with port number
    QHBoxLayout* portL = new QHBoxLayout(serverGBL);
    label = new QLabel(tr("&Portnumber:"), serverGB);
    portL->addWidget(label);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    portL->addItem(spacer);

    portLE = new QLineEdit(serverGB, "port");
    portL->addWidget(portLE);
    portLE->setMaximumWidth(100);
    label->setBuddy(portLE);
    portLE->setMaxLength(5);
    QValidator* portValidator = new QIntValidator(1, 65535, serverGB);
    portLE->setValidator(portValidator);
    QToolTip::add(portLE, tr(
                "Enter the portnumber of your srcp service.\n"
                "Default value for SRCP 0.8 is 4303,\n"
                "for a SRCP 0.7 server choose 12345."));

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    serverGBL->addItem(spacer);

    // start options group box
    QButtonGroup *startBG = new QButtonGroup(3, Qt::Vertical,
            tr("Actions on file loading"), this);
    baseLayout->addWidget(startBG);
    autologinCB = new QCheckBox(tr("Autoconnect to &server"),
            startBG, "autologinCB");
    QToolTip::add(autologinCB, tr(
                "The SRCP server will be automatically\n"
                "connected when this file is loaded.\n"));

    autopowerCB = new QCheckBox(tr("Autostart layout &voltage"),
            startBG, "autopowerCB");
    QToolTip::add(autopowerCB, tr(
                "Power of your layout will be automatically\n"
                "switched on after server connect.\n"));

    autosendallCB = new QCheckBox(tr("Send all &solenoid states after"
                " power on"),
            startBG, "autosendallCB");
    QToolTip::add(autosendallCB, tr(
                "The configured states of all solenoids will\n"
                "be automatically send to the SRCP server\n"
                "after layout power is switched on.\n"));

    // line with OK/Cancel buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout(0, 0, 6);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonLayout->addItem(spacer);

    QPushButton* buttOK = new QPushButton(tr("OK"), this);
    buttonLayout->addWidget(buttOK);
    buttOK->setDefault(true);
    connect(buttOK, SIGNAL(clicked()), this, SLOT(accept()));

    QPushButton* buttCancel = new QPushButton(tr("Cancel"), this);
    buttonLayout->addWidget(buttCancel);
    connect(buttCancel, SIGNAL(clicked()), this, SLOT(reject()));

    baseLayout->addLayout(buttonLayout);
}


int newLayoutDialog::getColumns()
{
    return sbEnterCols->value();
}


int newLayoutDialog::getRows()
{
    return sbEnterRows->value();
}


unsigned int newLayoutDialog::getLayoutId()
{
    return idLE->text().toUInt();
}


QString newLayoutDialog::getLayoutName()
{
    return nameLE->text();
}


void newLayoutDialog::setColumns(int cols)
{
    sbEnterCols->setValue(cols);
}


void newLayoutDialog::setRows(int rows)
{
    sbEnterRows->setValue(rows);
}


void newLayoutDialog::setLayoutId(unsigned int id)
{
    return idLE->setText(QString::number(id));
}


void newLayoutDialog::setLayoutName(const QString& name)
{
    return nameLE->setText(name);
}


QString newLayoutDialog::getHost()
{ 
    if (hostLE->text().isEmpty())
        return "localhost"; //FIXME
    else
        return hostLE->text();
}


unsigned int newLayoutDialog::getPort()
{
#if QT_VERSION >= 0x030200
    if (portLE->hasAcceptableInput())
#endif
        return portLE->text().toUInt();
#if QT_VERSION >= 0x030200
    else
        return 4303;  //FIXME
#endif
}


void newLayoutDialog::setHost(const QString& host)
{
    hostLE->setText(host);
}


void newLayoutDialog::setPort(unsigned int port)
{
    portLE->setText(QString::number(port));
}


bool newLayoutDialog::getAutoLogin()
{
    return autologinCB->isChecked();
    
}


bool newLayoutDialog::getAutoPower()
{
    return autopowerCB->isChecked();
}


bool newLayoutDialog::getAutoSendAll()
{
    return autosendallCB->isChecked();
}


void newLayoutDialog::setAutoLogin(bool login)
{
    autologinCB->setChecked(login);
}


void newLayoutDialog::setAutoPower(bool power)
{
    autopowerCB->setChecked(power);
}


void newLayoutDialog::setAutoSendAll(bool power)
{
    autosendallCB->setChecked(power);
}

