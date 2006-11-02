/***************************************************************************
                           keyboard.h
                           version 0.5.0 $Revision: 1.7 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2006 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2006-11-02 16:54:32 $
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
#include <qlineedit.h>


class keyboard: public QDialog
{
   Q_OBJECT

public:
   keyboard(QWidget* parent = 0, unsigned int srcpv = 7);

private:
   unsigned int srcpVersion;

signals:
   void sendCommand(const QString&);

private slots:
   void slotActivateRed();
   void slotActivateGrn();

private:
   QLineEdit*   addressLE;    // lineedit for address to switch
   QLineEdit*   busLE;        // lineedit for SRCP-bus
};

#endif
