/***************************************************************************
                           elementlabeldialog.cpp
                           -------------------------------
    copyright            : (C) 2010 Guido Scholz
    e-mail               : guido.scholz@bayernline.de
    last modified        : $Date: 2009/11/01 20:40:38 $
                           $Revision: 1.82 $
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
 This file provides an user interface to change label text of an layout
 item
 ***************************************************************************/

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include "elementlabeldialog.h"


ElementLabelDialog::ElementLabelDialog(QWidget* parent):
    QDialog(parent, "ElementLabelDialog", true)
{
    setCaption(tr("Edit label"));

    /*Layout to separate OK Cancel buttons from the upper rest*/
    QBoxLayout* baseLayout = new QVBoxLayout(this, 12, 12);

    /*line with text edit line*/
    QHBoxLayout* textLayout = new QHBoxLayout(baseLayout);
    QLabel* labelText = new QLabel(tr("&Text:"), this);
    textLayout->addWidget(labelText);
    textLayout->addStretch();
    leText = new QLineEdit(this, "text");
    leText->setMaxLength(20);
    textLayout->addWidget(leText);
    labelText->setBuddy(leText);

    /*layout with OK and Cancel buttons*/
    QBoxLayout* buttonLayout = new QHBoxLayout(baseLayout, 6);
    buttonLayout->addStretch();

    /*button line at bottom*/
    QPushButton* okButton = new QPushButton(tr("OK"), this);
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    buttonLayout->addWidget(okButton);

    QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    buttonLayout->addWidget(cancelButton);
}


QString ElementLabelDialog::getSymbolText()
{
    return leText->text();
}


void ElementLabelDialog::setSymbolText(const QString& text)
{
    leText->setText(text);
}

