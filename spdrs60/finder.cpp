/***************************************************************************
                           finder.cpp
                           version 0.5.2 $Revision: 1.14 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-10-10 19:41:51 $
***************************************************************************/

/*****************************************************************************
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by     *
 *   the Free Software Foundation; either version 2 of the License, or        *
 *   (at your option) any later version.                                      *
 *                                                                            *
 ******************************************************************************/
/******************************************************************************
   this code shows a window where user enters an address to be searched for
 ******************************************************************************/

#include <qlayout.h>
#include <qlabel.h>

#include "finder.h"
#include "preferences.h"


Finder::Finder(QWidget* parent): QDialog(parent, "Finder", false)
{
    setCaption(tr("Element locator"));
    /*Layout to separate Search and Cancel buttons form the upper rest*/
    QVBoxLayout* baseLayout = new QVBoxLayout(this, 10, 10);
    
    /*line with search editline*/
    QHBoxLayout* searchLayout = new QHBoxLayout(baseLayout, 6);
    QLabel *title = new QLabel(tr("&Find:"), this, "searchLbl");
    searchLayout->addWidget(title);
    QSpacerItem* spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    searchLayout->addItem(spacer);
    leSearch = new QLineEdit(this, "searchLE");
    searchLayout->addWidget(leSearch);
    title->setBuddy(leSearch);
    leSearch->setMaxLength(10);
    // if at least one char has been entered highlight search button
    connect(leSearch, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotActivateSearchButt(const QString&)));

    /*group box to choose data field*/
    dataBG = new QButtonGroup(3, Qt::Vertical,
            tr("Data fields"), this, "dataBG");
    dataBG->setExclusive(true);
    baseLayout->addWidget(dataBG);
    QRadioButton *rbSearchText =
        new QRadioButton(tr("&Text fields"), dataBG);
    //QRadioButton *rbSearchAdr1 =
        new QRadioButton(tr("Decoder address &1"), dataBG);
    //QRadioButton *rbSearchAdr2 =
        new QRadioButton(tr("Decoder address &2"),
                         dataBG);
    rbSearchText->setChecked(true);

    /*group box to choose find count*/
    matchBG = new QButtonGroup(2, Qt::Vertical,
            tr("Match counter"), this, "foundGB");
    matchBG->setExclusive(true);
    baseLayout->addWidget(matchBG);
    QRadioButton *rbSearchSingle =
        new QRadioButton(tr("&First/only one match"), matchBG);
    //QRadioButton *rbSearchMulti =
        new QRadioButton(tr("&All matches"), matchBG);
    rbSearchSingle->setChecked(true);

    /*separated line with Search and Cancel buttons*/
    QHBoxLayout* buttonLayout = new QHBoxLayout(baseLayout, 6);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonLayout->addItem(spacer);
    buttSearch = new QPushButton(tr("Search"), this, "searchBtn");
    buttonLayout->addWidget(buttSearch);
    buttSearch->setDefault(true);
    buttSearch->setEnabled(false);
    connect(buttSearch, SIGNAL(clicked()), this, SLOT(accept()));
    QPushButton* buttCancel = new QPushButton(tr("Cancel"), this, "cancelBtn");
    buttonLayout->addWidget(buttCancel);
    connect(buttCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QToolTip::add(buttSearch,
            tr("Press this button to start search"));
    QToolTip::add(buttCancel,
            tr("Press this button to close this window"));
}


QString Finder::getSearchText()
{
    return leSearch->text();
}


int Finder::getDataType()
{
#if QT_VERSION >= 0x030300
    return dataBG->selectedId();
#else
    return dataBG->id(dataBG->selected());
#endif
}


int Finder::getMatchType()
{
#if QT_VERSION >= 0x030300
    return matchBG->selectedId();
#else
    return matchBG->id(matchBG->selected());
#endif
}


void Finder::slotActivateSearchButt(const QString&)
{
    // only enable search button if at least one char has been entered
    buttSearch->setEnabled(!leSearch->text().isEmpty());
}
