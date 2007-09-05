/***************************************************************************
                           keyboard.h
                           version 0.5.2 $Revision: 1.11 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2007 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-09-05 17:39:48 $
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

#include <qcombobox.h>                    
#include <qdialog.h>                    
#include <qlineedit.h>

#include "srcpmessage.h"
#include "srcpport.h"


class keyboard: public QDialog
{
   Q_OBJECT

public:
   keyboard(SrcpPort::CommunicationStyle sctyle = SrcpPort::csOld,
           int protocol = 1, QWidget* parent = 0, const char* name = NULL);

private:
   SrcpPort::CommunicationStyle srcpStyle;

signals:
   void sendSrcpMessage(SrcpMessage*);
   void protocolSelected(int);

private slots:
   void slotActivateRed();
   void slotActivateGreen();

private:
   QLineEdit* addressLE;
   QLineEdit* busLE;
   QComboBox* protocolCB;
};

#endif
