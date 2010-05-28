/***************************************************************************
                           variantdialog.cpp
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
 This file provides an user interface to select an element variant
 ***************************************************************************/

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include "variantdialog.h"


VariantDialog::VariantDialog(QWidget* parent):
    QDialog(parent, "VariantDialog", true)
{
    setCaption(tr("Edit variant"));

    /*Layout to separate OK Cancel buttons from the upper rest*/
    QBoxLayout* baseLayout = new QVBoxLayout(this, 12, 12);

    /*variant display*/
    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignHCenter);
    baseLayout->addWidget(imageLabel);

    /*variant selection*/
    variantBG = new QButtonGroup(3, Qt::Vertical,
            tr("Variant"), this, "variantBG");
    baseLayout->addWidget(variantBG);
    variantBG->setRadioButtonExclusive(true);

    connect(variantBG, SIGNAL(clicked(int)), this,
            SLOT(selectionChanged(int)));

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


void VariantDialog::setChoice(int choice)
{
    variantBG->setButton(choice);
    selectionChanged(choice);
}


int VariantDialog::getChoice()
{
    return variantBG->selectedId();
}


void VariantDialog::addVariant(const QString& text)
{
    new QRadioButton(text, variantBG);
}


void VariantDialog::addVariant(const QString& text, const QPixmap& pm)
{
    new QRadioButton(text, variantBG);
    pmList.append(pm);
}


void VariantDialog::selectionChanged(int index)
{
    if (index >= 0 && (size_t)index < pmList.count()) {
        imageLabel->setPixmap(pmList[index]);
    }
}
