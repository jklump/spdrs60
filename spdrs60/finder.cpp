/***************************************************************************
                           finder.cpp
                           version 0.4.3 $Revision: 1.3 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-05-14 20:13:43 $
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
#include <ctype.h>              // for isdigit()
#include <stdlib.h>             // for atoi()
#include <unistd.h>             // for write()
#include "finder.h"
extern int SHOW_TOOLTIPS;


Finder::Finder(QWidget* parent): QDialog(parent, "Finder", false)
{
//    if (parent);                // dummy command to avoid compiler warning

    this->setCaption(tr("Element locator"));
    this->setFixedWidth(270);
    iSearchType = SRCH_TX;      // default search in text fields
    iMultiType = MULTI;         // default search for first element

    QLabel *title =
        new QLabel(tr("Search for this element data:"), this, "");
    title->resize(title->sizeHint());
    title->move(10, 10);

    leSearch = new QLineEdit(this, "");
    leSearch->resize(100, 20);
    leSearch->move(10, title->y() + 30);
    leSearch->setMaxLength(10); // max chars is 10 as in text field of element
    leSearch->setFocus();

    // if at least one char has been entered highlight search button
    connect(leSearch, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotActivateSearchButt(const QString&)));
    // even return key starts searching
    connect(leSearch, SIGNAL(returnPressed()),
            this, SLOT(slotBeginSearch()));

    QLabel *label = new QLabel("in:", this, "");
    label->resize(label->sizeHint());
    label->move(10, leSearch->y() + leSearch->height() + 20);

    QButtonGroup *grpBox1 = new QButtonGroup("", this);
    grpBox1->setGeometry(10, label->y() + 20, this->width() - 20, 100);
    grpBox1->setFrameStyle(QFrame::NoFrame);

    // different types of fields to search in for the entered string
    QRadioButton *rbSearchText =
        new QRadioButton(tr("text fields"), grpBox1);
    rbSearchText->setGeometry(10, 10, grpBox1->width() - 20, 20);
    QRadioButton *rbSearchAdr1 =
        new QRadioButton(tr("address 1 fields (main addresses)"), grpBox1);
    rbSearchAdr1->setGeometry(10, 40, grpBox1->width() - 10, 20);
    QRadioButton *rbSearchAdr2 =
        new QRadioButton(tr("address 2 fields (extra addresses)"),
                         grpBox1);
    rbSearchAdr2->setGeometry(10, 70, grpBox1->width() - 10, 20);
    rbSearchText->setChecked(true);

    connect(grpBox1, SIGNAL(clicked(int)),
            this, SLOT(slotSaveSearchType(int)));

    label = new QLabel(tr("Find:"), this, "");
    label->resize(label->sizeHint());
    label->move(10, grpBox1->y() + grpBox1->height() + 20);

    QButtonGroup *grpBox2 = new QButtonGroup("", this);
    grpBox2->setGeometry(10, label->y() + 20, this->width() - 20, 70);
    grpBox2->setFrameStyle(QFrame::NoFrame);

    // different types of fields to search in for the entered string
    QRadioButton *rbSearchSingle =
        new QRadioButton(tr("first/only one match"), grpBox2);
    rbSearchSingle->setGeometry(10, 10, grpBox2->width() - 20, 20);
    QRadioButton *rbSearchMulti =
        new QRadioButton(tr("all matches"), grpBox2);
    rbSearchMulti->setGeometry(10, 40, grpBox2->width() - 20, 20);
    rbSearchSingle->setChecked(true);

    connect(grpBox2, SIGNAL(clicked(int)),
            this, SLOT(slotSaveMultiType(int)));

    buttSearch = new QPushButton(tr("Search"), this, "Search");
    buttSearch->resize(buttSearch->sizeHint());
    buttSearch->move(this->width() / 2 - buttSearch->width() - 30,
                     grpBox2->y() + grpBox2->height() + 10);
    buttSearch->setEnabled(false);

    buttCancel = new QPushButton(tr("Cancel"), this, "Cancel");
    buttCancel->resize(buttCancel->sizeHint());
    buttCancel->move(this->width() - 20 - buttCancel->width(),
                     grpBox2->y() + grpBox2->height() + 10);

    connect(buttSearch, SIGNAL(clicked()), this, SLOT(slotBeginSearch()));
    connect(buttCancel, SIGNAL(clicked()), SLOT(reject()));
    if (SHOW_TOOLTIPS) {
        QToolTip::add(buttSearch,
                      tr("Press this button to begin searching"));
        QToolTip::add(buttCancel,
                      tr("Press this button to close this window"));
    }
    this->setFixedHeight(buttSearch->y() + buttSearch->height() + 10);
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
    QString sSearch = leSearch->text();
    buttSearch->setEnabled(!sSearch.isEmpty());
}
