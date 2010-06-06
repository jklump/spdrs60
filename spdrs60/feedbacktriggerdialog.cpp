/***************************************************************************
                           feedbacktriggerdialog.cpp
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
#include <qtooltip.h>
#include <qvalidator.h>

#include "preferences.h"
#include "resources.h"
#include "feedbacktriggerdialog.h"

/*button icons*/
#include "pixmaps/viewfeedback.xpm"


FeedbackTriggerDialog::FeedbackTriggerDialog(QWidget* parent):
    QDialog(parent, "FeedbackTriggerDialog", true)
{
    /*Layout to separate OK Cancel buttons from the upper rest*/
    QBoxLayout* baseLayout = new QVBoxLayout(this, 12, 12);

    /* line with LED off (Gleismelder) checkbox*/
    QHBoxLayout* mbLayout = new QHBoxLayout(baseLayout, 6);
    enableTriggerCB = new QCheckBox(this, "triggerCB");
    mbLayout->addWidget(enableTriggerCB);
    mbLayout->addStretch();
    QPushButton* buttFBmodules = new QPushButton(tr("&FB"), this);
    mbLayout->addWidget(buttFBmodules);
    buttFBmodules->setPixmap(QPixmap(viewfeedback_xpm));
    connect(buttFBmodules, SIGNAL(clicked()), this,
            SLOT(slotShowFBmodules()));
    QToolTip::add(buttFBmodules, tr("Show feedback module window"));

    /*feedback LED data group box*/
    feedbackGB = new QGroupBox(0, Qt::Horizontal, tr("Feedback contact"),
            this, "feedbackGB");
    baseLayout->addWidget(feedbackGB);
    QVBoxLayout* feedbackGBL = new QVBoxLayout(feedbackGB->layout(), 6);
    connect(enableTriggerCB, SIGNAL(clicked()),
            this, SLOT(enableFeedbackGroupBox()));

    /*line with bus*/
    QHBoxLayout* busLayout = new QHBoxLayout(feedbackGBL, 6);
    QLabel* labelFBBus = new QLabel(tr("&Bus (s88/SRCP):"), feedbackGB);
    busLayout->addWidget(labelFBBus);
    busLayout->addStretch();
    fbBusLE = new QLineEdit(feedbackGB, "fbBusLE");
    busLayout->addWidget(fbBusLE);
    fbBusLE->setMaximumWidth(LEMAXWIDTH);
    labelFBBus->setBuddy(fbBusLE);
    QIntValidator* busValidator = new QIntValidator(1, 999, this);
    fbBusLE->setValidator(busValidator);

    /*line with contact*/
    QHBoxLayout* feedbackLayout = new QHBoxLayout(feedbackGBL, 6);
    QLabel* labelFBContact = new QLabel(tr("&Contact (1 - 496):"), feedbackGB);
    feedbackLayout->addWidget(labelFBContact);
    feedbackLayout->addStretch();
    contactSB = new QSpinBox(1, 496, 1, feedbackGB, "contactSB");
    labelFBContact->setBuddy(contactSB);
    feedbackLayout->addWidget(contactSB);

    /*line with module*/
    QHBoxLayout* moduleLayout = new QHBoxLayout(feedbackGBL, 6);
    QLabel* labelFBmodule = new QLabel(tr("Module (1 - %1):")
            .arg(pref.fbfactor == 0 ? 31 : 62), feedbackGB);
    moduleLayout->addWidget(labelFBmodule);
    moduleLayout->addStretch();
    moduleLE = new QLineEdit(feedbackGB, "moduleLE");
    moduleLE->setMaximumWidth(LEMAXWIDTH);
#if QT_VERSION >= 0x040000
    moduleLE->setFocusPolicy(Qt::NoFocus);
#else
    moduleLE->setFocusPolicy(QWidget::NoFocus);
#endif
    moduleLayout->addWidget(moduleLE);
    
    /*line with port*/
    QHBoxLayout* portLayout = new QHBoxLayout(feedbackGBL, 6);
    QLabel* labelFBport = new QLabel(tr("Port (1 - %1):")
            .arg(pref.fbfactor == 0 ? 16 : 8), feedbackGB);
    portLayout->addWidget(labelFBport);
    portLayout->addStretch();
    portLE = new QLineEdit(feedbackGB, "portLE");
    portLE->setMaximumWidth(LEMAXWIDTH);
#if QT_VERSION >= 0x040000
    portLE->setFocusPolicy(Qt::NoFocus);
#else
    portLE->setFocusPolicy(QWidget::NoFocus);
#endif
    portLayout->addWidget(portLE);

    connect(contactSB, SIGNAL(valueChanged(int)),
            this, SLOT(contactSBChanged(int)));

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

void FeedbackTriggerDialog::contactSBChanged(int contact)
{
    // FB_16 = 0, FB_8 = 1
    int inputs = 16 - (pref.fbfactor * 8);
    int module = (contact - 1) / inputs + 1;
    int port = contact - (module - 1) * inputs;
    moduleLE->setText(QString::number(module));
    portLE->setText(QString::number(port));
}


int FeedbackTriggerDialog::getFBBus()
{
    return fbBusLE->text().toInt();
}


void FeedbackTriggerDialog::setFBBus(int bus)
{
    fbBusLE->setText(QString::number(bus));
}


int FeedbackTriggerDialog::getFBContact()
{
    return contactSB->value();
}


void FeedbackTriggerDialog::setFBContact(int contact)
{
    contactSB->setValue(contact);
    contactSBChanged(contact);
}

bool FeedbackTriggerDialog::isTriggerEnabled()
{
    return enableTriggerCB->isChecked();
}


void FeedbackTriggerDialog::enableTrigger(bool enabled)
{
    enableTriggerCB->setChecked(enabled);
    enableFeedbackGroupBox();
}


void FeedbackTriggerDialog::enableFeedbackGroupBox()
{
    feedbackGB->setEnabled(enableTriggerCB->isChecked());
}


void FeedbackTriggerDialog::slotShowFBmodules()
{
    emit sigShowFBmodules();
}


void FeedbackTriggerDialog::setCheckBoxText(const QString& text)
{
   enableTriggerCB->setText(text);;
}

