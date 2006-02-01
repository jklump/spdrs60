/***************************************************************************
                           finder.cpp
                           version 0.5.0 $Revision: 1.5 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2006 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2006-02-01 16:40:46 $
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

#include "finder.h"
#include "preferences.h"


Finder::Finder(QWidget* parent): QDialog(parent, "Finder", false)
{
    iSearchType = SRCH_TX;      // default search in text fields
    iMultiType = MULTI;         // default search for first element

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
    // even return key starts searching
    connect(leSearch, SIGNAL(returnPressed()),
            this, SLOT(slotBeginSearch()));

    /*group box to choose data field*/
    QButtonGroup *grpBox1 = new QButtonGroup(3, Vertical,
            tr("Data fields"), this, "dataGB");
    baseLayout->addWidget(grpBox1);
    QRadioButton *rbSearchText =
        new QRadioButton(tr("&Text fields"), grpBox1);
    QRadioButton *rbSearchAdr1 =
        new QRadioButton(tr("Decoder address &1"), grpBox1);
    QRadioButton *rbSearchAdr2 =
        new QRadioButton(tr("Decoder address &2"),
                         grpBox1);
    rbSearchText->setChecked(true);

    connect(grpBox1, SIGNAL(clicked(int)),
            this, SLOT(slotSaveSearchType(int)));

    /*group box to choose find count*/
    QButtonGroup *grpBox2 = new QButtonGroup(2, Vertical,
            tr("Match counter"), this, "foundGB");
    baseLayout->addWidget(grpBox2);
    QRadioButton *rbSearchSingle =
        new QRadioButton(tr("&First/only one match"), grpBox2);
    QRadioButton *rbSearchMulti =
        new QRadioButton(tr("&All matches"), grpBox2);
    rbSearchSingle->setChecked(true);

    connect(grpBox2, SIGNAL(clicked(int)),
            this, SLOT(slotSaveMultiType(int)));

    /*separated line with Search and Cancel buttons*/
    QHBoxLayout* buttonLayout = new QHBoxLayout(baseLayout, 6);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonLayout->addItem(spacer);
    buttSearch = new QPushButton(tr("Search"), this, "searchBtn");
    buttonLayout->addWidget(buttSearch);
    buttSearch->setEnabled(false);
    buttCancel = new QPushButton(tr("Cancel"), this, "cancelBtn");
    buttonLayout->addWidget(buttCancel);
    connect(buttSearch, SIGNAL(clicked()), this, SLOT(slotBeginSearch()));
    connect(buttCancel, SIGNAL(clicked()), SLOT(reject()));
    if (pref.tooltips) {
        QToolTip::add(buttSearch,
                      tr("Press this button to begin searching"));
        QToolTip::add(buttCancel,
                      tr("Press this button to close this window"));
    }
}


void Finder::slotBeginSearch()
{
    QString sSearch = leSearch->text();
    // send string & type to gbs
    emit sigFind(sSearch, iSearchType, iMultiType);
    reject();                   // close this window
}


void Finder::slotSaveSearchType(int iButtID_)
{
    iSearchType = iButtID_;     // save search type button ID
}


void Finder::slotSaveMultiType(int iButtID_)
{
    iMultiType = iButtID_;      // save multi type button ID
}


void Finder::slotActivateSearchButt(const QString&)
{
    // only enable search button if at least one char has been entered
    buttSearch->setEnabled(!leSearch->text().isEmpty());
}
