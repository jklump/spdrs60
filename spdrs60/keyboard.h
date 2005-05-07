/***************************************************************************
                           keyboard.h
                           version 0.4.3 $Revision: 1.2 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-05-07 12:22:43 $
***************************************************************************/

/**************************************************************************
 *                                                                        *
 *   This program is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation; either version 2 of the License, or    *
 *   (at your option) any later version.                                  *
 *                                                                        *
 **************************************************************************/

/**************************************************************************
   this is the header file to keyboard.cpp
 **************************************************************************/

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <qdialog.h>                    
#include <qlabel.h>
#include <qlineedit.h>
#include <qtooltip.h>
#include <qpixmap.h>
#include <qpushbutton.h>

#include "resources.h"


class keyboard: public QDialog
{
   Q_OBJECT

public:
   keyboard(QWidget* parent=0); // creator of keyboard window

private:
   void switchIt(int);

signals:
/*   void cmdToDebug(const QString&); // send command to debug
 *   window*/
   void sendCommand(const char* cSocketCommand_);

private slots:
   void slotActivateRed();        // call switchIt with right direction
   void slotActivateGrn();        // call switchIt with right direction
   void slotAddressChanged(const QString&); // check for number inputs

private:
   QLineEdit*   leAddress;        // lineedit for address to switch
   QPushButton* buttRed;          // pushbutton for red connector
   QPushButton* buttGrn;          // pushbutton for green connector
};

#endif
