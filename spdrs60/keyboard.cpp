/***************************************************************************
                           keyboard.cpp
                           version 0.4.7
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : 2005-01-06
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
   this code shows a window with a manual keyboard to switch solenoids
 ******************************************************************************/

#include <ctype.h>              // for isdigit()
#include <stdlib.h>             // for atoi()
#include <unistd.h>             // for write()
#include "keyboard.h"

/*button icons*/
#include "pixmaps/keyb_green.xpm"
#include "pixmaps/keyb_red.xpm"

extern int SHOW_TOOLTIPS;


keyboard::keyboard(QWidget * parent):QDialog(parent, "keyboard", false)
{                               // false == parent window is still usable
    //if (parent);                // dummy command to avoid compiler warning

    setCaption(tr("Keyboard"));
    setMaximumWidth(150);
    setMinimumWidth(150);
    setMaximumHeight(100);
    setMinimumHeight(100);

    QLabel *title = new QLabel(this, "");
    title->setText(tr("Basic switch keyboard"));
    title->resize(title->sizeHint());
    title->move(this->width() / 2 - title->width() / 2, 5);

    leAddress = new QLineEdit(this, "");
    leAddress->resize(50, 20);
    leAddress->move(this->width() / 2 - leAddress->width() / 2,
                    title->y() + title->height() + 5);
    leAddress->setFocus();
    connect(leAddress, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotAddressChanged(const QString &)));

    QLabel *labelAddress = new QLabel(this, "");
    labelAddress->resize(100, 20);
    labelAddress->move(leAddress->x() - 10 - labelAddress->width(), 20);
    if (SHOW_TOOLTIPS)
        QToolTip::add(labelAddress,
                      tr("Please enter the address to be switched"));

    buttRed = new QPushButton(this, tr("Red"));
    buttGrn = new QPushButton(this, tr("Green"));
    buttRed->resize(35, 35);
    buttGrn->resize(35, 35);
    buttRed->move(20, leAddress->y() + 30);
    buttGrn->move(95, leAddress->y() + 30);

    QPixmap pix = QPixmap(keyb_red_xpm);
    buttRed->setPixmap(pix);
    pix = QPixmap(keyb_green_xpm);
    buttGrn->setPixmap(pix);

    connect(buttRed, SIGNAL(clicked()), this, SLOT(slotActivateRed()));
    connect(buttGrn, SIGNAL(clicked()), this, SLOT(slotActivateGrn()));
    if (SHOW_TOOLTIPS) {
        QToolTip::add(buttRed,
                      tr("Press this button to activate red connector"));
        QToolTip::add(buttGrn,
                      tr("Press this button to activate green connector"));
    }
}


void keyboard::slotActivateRed()
{
    switchIt(0);                // switch red connector
}


void keyboard::slotActivateGrn()
{
    switchIt(1);                // switch green connector
}


void keyboard::switchIt(int iDir_)
{
    char buf[30];
    QString s;
    int iSolenoid = 0;

    iSolenoid = atoi(leAddress->text());        // read address to switch ...
    leAddress->setText("");     // ... and clear lineedit
    leAddress->setFocus();

    if (iSolenoid == 0 || iSolenoid > 4096)
        // basic limit check (4096 is the
        // higher limit of DCC protocol)
        return;

    for (int i = 77; i <= 78; i++)      // 77 = "M", 78 = "N"
    {                           // send command for both protocols
        s.sprintf("SET GA %c %04d %1d 1 50\n", i, iSolenoid, iDir_);
        bzero(buf, 30);
        strcpy(buf, s.data());

        /* MainWindow procedure takes the action */
        emit sendCommand(s);
    }
}


void keyboard::slotAddressChanged(const QString & cNewAddress_)
{
    QString sCorrection = cNewAddress_;

    // a zero length is okay
    if (sCorrection.length() == 0)
        return;

    // now reject character input if it was not a digit
    for (uint i = 0; i < sCorrection.length(); i++) {
        if (isdigit(cNewAddress_[i]) == false) {
            sCorrection.replace(i, 1, '\0');    // correction code replaces
            leAddress->setText(sCorrection);    // the wrong user entry in
            leAddress->setCursorPosition(i);    // line edits
            break;
        }
    }
}
