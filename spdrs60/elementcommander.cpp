/***************************************************************************
                           elementcommander.cpp
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2008 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-08-12 17:58:27 $ 
                           $Revision: 1.11 $
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
 this code provides a GUI to control analog turntables, shifting brigdes ...
****************************************************************************/

#include <qpixmap.h>

#if QT_VERSION >= 0x040000
#include <q3frame.h>
#endif

#include "elementcommander.h"
#include "preferences.h"
#include "element.h"

/*button icons*/
#include "pixmaps/tt_stop.xpm"
#include "pixmaps/tt_left.xpm"
#include "pixmaps/tt_left_step.xpm"
#include "pixmaps/tt_right.xpm"
#include "pixmaps/tt_right_step.xpm"



elementCommander::elementCommander(QWidget* parent, int cid)
    : QDialog(0, "elementCommander", false)
{
    // true, parent window not usable until this closed
    // dummy command to avoid compiler warning
    if (parent);
    classid = cid;

    // create a button group for standard buttons ...
    bgButton = new QButtonGroup(this, "commandBG");
    bgButton->move(0, 0);
    bgButton->resize(60, 30);
    bgButton->setExclusive(true);
    bgButton->setFrameStyle(QFrame::NoFrame);

    pixButton = QPixmap(tt_stop_xpm);
    buttStop = new QPushButton("o", this);      // ... and a stop button
    buttStop->move(150, 5);
    buttStop->resize(20, 20);
    buttStop->setPixmap(pixButton);
    buttStop->setEnabled(false);
    connect(buttStop, SIGNAL(clicked()), this, SLOT(slotStop()));

    if (classid == element::siciSbn)   // setup the remaining buttons
        setupBridge();
    else
        setupMotor();

    // fix the window's geometry
    setFixedWidth(285);
    setFixedHeight(30);
}


void elementCommander::setupBridge()
{
    pixButton = QPixmap(tt_left_step_xpm);
    buttMoveUp = new QPushButton("<", bgButton);
    buttMoveUp->move(10, 5);
    buttMoveUp->resize(20, 20);
    buttMoveUp->setPixmap(pixButton);
    buttMoveUp->setToggleButton(true);

    pixButton = QPixmap(tt_right_step_xpm);
    buttMoveDown = new QPushButton(">", bgButton);
    buttMoveDown->move(40, 5);
    buttMoveDown->resize(20, 20);
    buttMoveDown->setPixmap(pixButton);
    buttMoveDown->setToggleButton(true);

    setCaption(tr("Shifting bridge commander"));
    connect(buttMoveUp, SIGNAL(clicked()), this, SLOT(slotMoveUp()));
    connect(buttMoveDown, SIGNAL(clicked()), this, SLOT(slotMoveDown()));

    QToolTip::add(buttMoveUp, tr("Move bridge upwards"));
    QToolTip::add(buttMoveDown, tr("Move bridge downwards"));
    QToolTip::add(buttStop, tr("Stop moving bridge"));
}


void elementCommander::setupMotor()
{
    pixButton = QPixmap(tt_left_xpm);
    buttRotateLeft = new QPushButton("<<", bgButton);
    buttRotateLeft->move(10, 5);
    buttRotateLeft->resize(20, 20);
    buttRotateLeft->setToggleButton(true);
    buttRotateLeft->setPixmap(pixButton);

    pixButton = QPixmap(tt_right_xpm);
    buttRotateRight = new QPushButton(">>", bgButton);
    buttRotateRight->move(40, 5);
    buttRotateRight->resize(20, 20);
    buttRotateRight->setPixmap(pixButton);
    buttRotateRight->setToggleButton(true);

    this->setCaption(tr("DC-motor commander"));
    connect(buttRotateLeft, SIGNAL(clicked()),
            this, SLOT(slotRotateLeft()));
    connect(buttRotateRight, SIGNAL(clicked()),
            this, SLOT(slotRotateRight()));

    QToolTip::add(buttRotateLeft, tr("Move motor clockwise"));
    QToolTip::add(buttRotateRight, tr("Move motor anti-clockwise"));
    QToolTip::add(buttStop, tr("Stop motor"));
}


void elementCommander::slotMoveUp()
{
    buttStop->setEnabled(true);
    buttMoveUp->setEnabled(false);
    buttMoveDown->setEnabled(false);
    buildCommand(1, 0);         // select direction upwards
    buildCommand(2, 0);         // start moving bridge
}


void elementCommander::slotMoveDown()
{
    buttStop->setEnabled(true);
    buttMoveUp->setEnabled(false);
    buttMoveDown->setEnabled(false);
    buildCommand(1, 1);         // select direction downwards
    buildCommand(2, 0);         // start moving bridge
}


void elementCommander::slotRotateLeft()
{
    buttStop->setEnabled(true);
    buttRotateLeft->setEnabled(false);
    buttRotateRight->setEnabled(false);
    buildCommand(1, 0);         // select left rotating
    buildCommand(2, 0);
}


void elementCommander::slotRotateRight()
{
    buttStop->setEnabled(true);
    buttRotateLeft->setEnabled(false);
    buttRotateRight->setEnabled(false);
    buildCommand(1, 1);         // select right rotating
    buildCommand(2, 1);
}


void elementCommander::slotStop()
{
    buttStop->setEnabled(false);

    if (classid == element::siciSbn) {
        buttMoveUp->setEnabled(true);
        buttMoveDown->setEnabled(true);
        buttMoveUp->setOn(false);
        buttMoveDown->setOn(false);
        buildCommand(2, 1);     // stop moving
    }
    else if (classid == element::siciMdc) {
        buttRotateLeft->setEnabled(true);
        buttRotateRight->setEnabled(true);
        buttRotateLeft->setOn(false);
        buttRotateRight->setOn(false);
        buildCommand(1, 0);     // stop rotating with a red and a green button
        buildCommand(2, 1);
    }
}


void elementCommander::buildCommand(int iKeyNo_, int iKeyColor_)
{
    QPoint point = QPoint(iKeyNo_, iKeyColor_);
    emit applyPressed(point);
    // 0 == red key, 1 == green key
}
