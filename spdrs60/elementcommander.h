/***************************************************************************
                           elementcommander.h
                           version 0.4.3
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
    email                : stefan.preis@wdr.de
    last modified        : 2004-12-31
***************************************************************************/

/******************************************************************************
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by     *
 *   the Free Software Foundation; either version 2 of the License, or        *
 *   (at your option) any later version.                                      *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
   this is the header file to elementcommander.cpp
 ******************************************************************************/
#ifndef ELEMENTCOMMANDER_H
#define ELEMENTCOMMANDER_H

#include <qbuttongroup.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qtooltip.h>

#include "resources.h"


class elementCommander : public QDialog
{
   Q_OBJECT

public:
   elementCommander( QWidget *parent=0, QString sType_="" ); // creator of  gui

private:
   void buildCommand( int, int );     // creates and sends the "keys"
   void setupBridge();                // sets up the bridge buttons
   void setupMotor();                 // sets up the motor buttons

private slots:
   void slotMoveUp();                 // moves bridge up
   void slotMoveDown();               // moves bridge down
   void slotRotateLeft();             // rotates motor left
   void slotRotateRight();            // rotates motor right
   void slotStop();                   // stop any movements

signals:
   void applyPressed( QPoint );       // sends keys to element

private:
   QPushButton*  buttMoveUp;          // button for moving a bridge up
   QPushButton*  buttMoveDown;        // button for moving a bridge down
   QPushButton*  buttRotateLeft;      // button for rotating a motor cw
   QPushButton*  buttRotateRight;     // button for rotating a motor acw
   QPushButton*  buttStop;            // button for stop any movements
   QPixmap       pixButton;           // the pixmap for each button
   QButtonGroup* bgButton;            // a button group for moving/rotating butt

   QString       sSoldIcon;           // save the icon relevant for commander
};

#endif
