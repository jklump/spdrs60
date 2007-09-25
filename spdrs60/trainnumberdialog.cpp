/*
 * trainnumberdialog.cpp
 * ---------------------
 * copyright            : (C) 2007 Guido Scholz
 * email                : guido.scholz@bayernline.de
 * last modified        : $Date: 2007-09-25 18:14:51 $
 *                        $Revision: 1.1 $
 *
 * this code shows a window with a manual trainnumberdialog to switch solenoids
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvalidator.h>

#include "trainnumberdialog.h"



TrainNumberDialog::TrainNumberDialog(QWidget* parent, const char* name)
: QDialog(parent, name)
{
    setCaption(tr("Change train number"));

    QBoxLayout* baseLayout = new QVBoxLayout(this, 10, 10);

    /*line with route label and edit line*/
    QBoxLayout* routeLayout = new QHBoxLayout(baseLayout, 6, "routeLayout");

    QLabel* label = new QLabel(tr("&Route-Id:"), this, "label");
    routeLayout->addWidget(label);

    routeLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));

    routeLE = new QLineEdit("1", this, "routeLE");
    QFontMetrics fm(routeLE->font());
    int LEwidth = fm.width("888888") + 10;
    routeLE->setMaxLength(5);
    routeLE->setMaximumWidth(LEwidth);
    QValidator* routeValidator = new QIntValidator(1, 9999, this);
    routeLE->setValidator(routeValidator);
    routeLayout->addWidget(routeLE);
    label->setBuddy(routeLE);
    QToolTip::add(routeLE, tr("Enter the route id for the train number"));

    /*line with train label and edit line*/
    QBoxLayout* trainLayout = new QHBoxLayout(baseLayout, 6,
            "trainLayout");

    label = new QLabel(tr("&Train number:"), this, "trainLbl");
    trainLayout->addWidget(label);

    trainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));

    trainLE = new QLineEdit("1", this, "trainLE");
    trainLE->setMaxLength(5);
    trainLE->setMaximumWidth(LEwidth);
    QValidator* trainValidator = new QIntValidator(0, 99999, this);
    trainLE->setValidator(trainValidator);
    trainLayout->addWidget(trainLE);
    label->setBuddy(trainLE);
    QToolTip::add(trainLE, tr("Enter the train number for the selected route"));
    
    /*line with buttons*/
    QBoxLayout* buttonLayout = new QHBoxLayout(0, 0, 6);
    baseLayout->addLayout(buttonLayout);

    buttonLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));

    QPushButton* redPB = new QPushButton("&Apply", this, "applyBtn");
    connect(redPB, SIGNAL(clicked()), this, SLOT(setTrainNumber()));
    buttonLayout->addWidget(redPB);
    redPB->setDefault(true);
    QToolTip::add(redPB, tr("Press this button to set train number"));
    
    buttonLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));
}


void TrainNumberDialog::setTrainNumber()
{
    unsigned int route = routeLE->text().toUInt();
    unsigned int train = trainLE->text().toUInt();
    emit trainNumberChanged(route, train);
}

