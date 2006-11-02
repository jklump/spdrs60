/***************************************************************************
                           newlayoutdialog.cpp
                           version 0.5.0 $Revision: 1.9 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                           (C) 2004-2006 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2006-11-02 16:54:32 $
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



newLayoutDialog::newLayoutDialog(QWidget* parent)
: QDialog(parent, "newLayoutDialog", true)
{
    setCaption(tr("Create new layout"));
    QVBoxLayout* baseLayout = new QVBoxLayout(this, 10, 6);

    // layout dimensions group box
    QGroupBox* dimensionsGB = new QGroupBox(0, Horizontal,
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
    if (pref.tooltips)
        QToolTip::add(sbEnterCols, tr("Choose or enter the number of\n"
                                      "columns for an empty layout"));

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
    if (pref.tooltips)
        QToolTip::add(sbEnterRows, tr("Choose or enter the number of\n"
                                      "rows for an empty layout"));

    // server group box
    QGroupBox *serverGB = new QGroupBox(0, Horizontal, "SRCP-Server",
            this, "serverGB");
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
    QValidator* portValidator = new QIntValidator(10000, 65535, serverGB);
    portLE->setValidator(portValidator);

    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    serverGBL->addItem(spacer);

    // start options group box
    QButtonGroup *startBG = new QButtonGroup(3, Vertical,
            tr("Start options"), this);
    baseLayout->addWidget(startBG);
    autologinCB = new QCheckBox(tr("&Server login on startup"),
            startBG, "autologinCB");
    autopowerCB = new QCheckBox(tr("&Autostart voltage on layout"),
            startBG, "autopowerCB");
    autosendallCB = new QCheckBox(tr("Send all s&olenoid states after"
                " power on"),
            startBG, "autosendallCB");

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


void newLayoutDialog::setColumns(int cols)
{
    sbEnterCols->setValue(cols);
}


void newLayoutDialog::setRows(int rows)
{
    sbEnterRows->setValue(rows);
}


QString newLayoutDialog::getHost()
{ 
    if (hostLE->text().isEmpty())
        return "localhost"; //FIXME
    else
        return hostLE->text();
}


int newLayoutDialog::getPort()
{
#if QT_VERSION >= 0x030200
    if (portLE->hasAcceptableInput())
#endif
        return portLE->text().toInt();
#if QT_VERSION >= 0x030200
    else
        return 12345;  //FIXME
#endif
}


void newLayoutDialog::setHost(const QString& host)
{
    hostLE->setText(host);
}


void newLayoutDialog::setPort(int port)
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

