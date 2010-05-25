/***************************************************************************
                           virtualaddressdialog.h
                           -------------------------------
    copyright            : (C) 2010 by Guido Scholz
    e-mail               : guido.scholz@bayernline.de
    last modified        : $Date: 2008/11/05 08:42:40 $
                           $Revision: 1.24 $
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
   this is the header file to drivedialog.cpp
 ***************************************************************************/

#ifndef VIRTUALADDRESSDIALOG_H
#define VIRTUALADDRESSDIALOG_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qvalidator.h>

#include "element.h"


class VirtualAddressDialog: public QDialog
{
   Q_OBJECT

   element::SpdrItemClassId classid;
   QLineEdit* address1LE;
   QLineEdit* srcpBus1LE;
   QIntValidator* addressVdt;

   void updateValidators();
   void updateAddressTooltip();

public:
   VirtualAddressDialog(QWidget* parent = 0);
   void setClassId(element::SpdrItemClassId);
   int getSRCPBus1();
   void setSRCPBus1(int);
   int getAddress1();
   void setAddress1(int);

private slots:
   void validate();
};

#endif    //DRIVEDIALOG_H
