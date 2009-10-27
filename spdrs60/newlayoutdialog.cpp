/***************************************************************************
                           newlayoutdialog.cpp
                           version 0.5.3 $Revision: 1.26 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                           (C) 2004-2008 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2009-10-27 20:29:24 $
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

#include "newlayoutdialog.h"
#include "preferences.h"
#include "resources.h"


newLayoutDialog::newLayoutDialog(QWidget* parent)
: QTabDialog(parent, "newLayoutDialog", true)
{
    setCaption(tr("Create new layout"));
    setupGeneralTab();
    setupCrcfTab();

    setOKButton();
    setCancelButton();
}

void newLayoutDialog::setupGeneralTab()
{
    QWidget *w = new QWidget(this, "tabPageOne");
    QVBoxLayout* tabL = new QVBoxLayout(w, 10);

    // layout dimensions group box
    QGroupBox* dimensionsGB = new QGroupBox(0, Qt::Horizontal,
            tr("Layout dimensions"), w, "dimensionsGB");
    tabL->addWidget(dimensionsGB);
    QVBoxLayout* boxL = new QVBoxLayout(dimensionsGB->layout(), 6);

    //line with column number
    QHBoxLayout* columnsLayout = new QHBoxLayout(boxL);
    QLabel* label = new QLabel(tr("&Columns:"), dimensionsGB);
    columnsLayout->addWidget(label);
    columnsLayout->addStretch();
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
    rowsLayout->addStretch();
    sbEnterRows = new QSpinBox(MIN_ROWS, MAX_ROWS, 1, dimensionsGB,
            "sbEnterCols");
    rowsLayout->addWidget(sbEnterRows);
    label->setBuddy(sbEnterRows);
    sbEnterRows->setWrapping(true);     // enables to spin "over" the limits
    QToolTip::add(sbEnterRows, tr(
                "Choose or enter the number\n"
                "of rows for your layout.\n"
                "Valid range is %1..%2.").arg(MIN_ROWS).arg(MAX_ROWS));

    // server group box
    QGroupBox *serverGB = new QGroupBox(0, Qt::Horizontal,
            tr("SRCP-Server"), w, "serverGB");
    tabL->addWidget(serverGB);
    QVBoxLayout* serverGBL = new QVBoxLayout(serverGB->layout(), 6);

    // line with host name
    QHBoxLayout* hostL = new QHBoxLayout(serverGBL);
    label = new QLabel(tr("&Hostname:"), serverGB);
    hostL->addWidget(label);
    hostL->addStretch();
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
    portL->addStretch();

    portLE = new QLineEdit(serverGB, "port");
    portL->addWidget(portLE);
    portLE->setMaximumWidth(100);
    label->setBuddy(portLE);
    portLE->setMaxLength(5);
    portValidator = new QIntValidator(1, 65535, serverGB);
    portLE->setValidator(portValidator);
    QToolTip::add(portLE, tr(
                "Enter the portnumber of your srcp service.\n"
                "Default value for SRCP 0.8 is 4303,\n"
                "for a SRCP 0.7 server choose 12345."));

    serverGBL->addStretch();

    // start options group box
    QButtonGroup *startBG = new QButtonGroup(3, Qt::Vertical,
            tr("Actions on file loading"), w);
    tabL->addWidget(startBG);
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

    // spacer to push group boxes to top
    tabL->addStretch();

    addTab(w, tr("&General"));
}


void newLayoutDialog::setupCrcfTab()
{
    QWidget *w = new QWidget(this, "crcfTabPage");
    QVBoxLayout* tabL = new QVBoxLayout(w, 10);

    // switch box identification group box
    QGroupBox *switchboxGB = new QGroupBox(0, Qt::Horizontal,
            tr("Switchbox Identification"), w, "switchboxGB");
    tabL->addWidget(switchboxGB);
    QVBoxLayout* switchboxGBL = new QVBoxLayout(
            switchboxGB->layout(), 6);

    // line with id
    QHBoxLayout* switchboxidL = new QHBoxLayout(switchboxGBL);
    QLabel* label = new QLabel(tr("&Id:"), switchboxGB);
    switchboxidL->addWidget(label);
    switchboxidL->addStretch();
    switchboxidLE = new QLineEdit(switchboxGB, "switchboxidLE");
    switchboxidL->addWidget(switchboxidLE);
    switchboxidLE->setMaximumWidth(100);
    switchboxidLE->setMaxLength(6);
    QValidator* idValidator = new QIntValidator(0, 999999, switchboxGB);
    switchboxidLE->setValidator(idValidator);
    label->setBuddy(switchboxidLE);
    QToolTip::add(switchboxidLE, tr(
                "Enter the CRCF-identification\n"
                "number of this switchbox.\n"
                "Valid range is 0..999999."));

    // line with layout name
    QHBoxLayout* switchboxnameL = new QHBoxLayout(switchboxGBL);
    label = new QLabel(tr("&Name:"), switchboxGB);
    switchboxnameL->addWidget(label);
    switchboxnameL->addStretch();

    switchboxnameLE = new QLineEdit(switchboxGB, "switchboxnameLE");
    switchboxnameL->addWidget(switchboxnameLE);
    switchboxnameLE->setMaximumWidth(100);
    label->setBuddy(switchboxnameLE);
    QToolTip::add(switchboxnameLE, tr(
                "Enter the CRCF-name of this switchbox."));

    switchboxGBL->addStretch();

    // layout identification group box
    QGroupBox *layoutGB = new QGroupBox(0, Qt::Horizontal,
            tr("Layout Identification"), w, "layoutGB");
    tabL->addWidget(layoutGB);
    QVBoxLayout* layoutGBL = new QVBoxLayout(
            layoutGB->layout(), 6);

    // line with id
    QHBoxLayout* layoutidL = new QHBoxLayout(layoutGBL);
    label = new QLabel(tr("I&d:"), layoutGB);
    layoutidL->addWidget(label);
    layoutidL->addStretch();
    layoutidLE = new QLineEdit(layoutGB, "layoutidLE");
    layoutidL->addWidget(layoutidLE);
    layoutidLE->setMaximumWidth(100);
    layoutidLE->setMaxLength(6);
    layoutidLE->setValidator(idValidator);
    label->setBuddy(layoutidLE);
    QToolTip::add(layoutidLE, tr(
                "Enter the CRCF-identification\n"
                "number of this layout.\n"
                "Valid range is 0..999999."));

    // line with layout name
    QHBoxLayout* layoutnameL = new QHBoxLayout(layoutGBL);
    label = new QLabel(tr("N&ame:"), layoutGB);
    layoutnameL->addWidget(label);
    layoutnameL->addStretch();

    layoutnameLE = new QLineEdit(layoutGB, "layoutnameLE");
    layoutnameL->addWidget(layoutnameLE);
    layoutnameLE->setMaximumWidth(100);
    label->setBuddy(layoutnameLE);
    QToolTip::add(layoutnameLE, tr(
                "Enter the CRCF-name of this layout."));

    layoutGBL->addStretch();

    // spacer to push group boxes to top
    tabL->addStretch();

    addTab(w, tr("&CRCF-Data"));
}


int newLayoutDialog::getColumns()
{
    return sbEnterCols->value();
}


int newLayoutDialog::getRows()
{
    return sbEnterRows->value();
}


unsigned int newLayoutDialog::getSwitchboxId()
{
    return switchboxidLE->text().toUInt();
}


QString newLayoutDialog::getSwitchboxName()
{
    return switchboxnameLE->text();
}


unsigned int newLayoutDialog::getLayoutId()
{
    return layoutidLE->text().toUInt();
}


QString newLayoutDialog::getLayoutName()
{
    return layoutnameLE->text();
}


void newLayoutDialog::setColumns(int cols)
{
    sbEnterCols->setValue(cols);
}


void newLayoutDialog::setRows(int rows)
{
    sbEnterRows->setValue(rows);
}


void newLayoutDialog::setSwitchboxId(unsigned int id)
{
    switchboxidLE->setText(QString::number(id));
}


void newLayoutDialog::setSwitchboxName(const QString& name)
{
    switchboxnameLE->setText(name);
}


void newLayoutDialog::setLayoutId(unsigned int id)
{
    layoutidLE->setText(QString::number(id));
}


void newLayoutDialog::setLayoutName(const QString& name)
{
    layoutnameLE->setText(name);
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
#else
    int pos = 0;
    QString value = portLE->text();
    if (portValidator->validate(value, pos) == QValidator::Acceptable)
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

