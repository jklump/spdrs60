/***************************************************************************
                           drivedialog.h
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

#ifndef DRIVEDIALOG_H
#define DRIVEDIALOG_H

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qdialog.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qvalidator.h>


class DriveDialog: public QDialog
{
   Q_OBJECT

   QButtonGroup* protocolBG;
   QSpinBox* activeTimeSB;
   QSpinBox* port1SB;
   QLabel* port1Label;
   QLineEdit* address1LE;
   QLineEdit* srcpBus1LE;
   QCheckBox* xchConn1CB;
   QIntValidator* addressVdt;

   void updateValidators();
   void updateAddressTooltip();

public:
   DriveDialog(QWidget* parent = 0);
   int getProtocol();
   void setProtocol(int);
   int getActiveTime();
   void setActiveTime(int);
   int getSRCPBus1();
   void setSRCPBus1(int);
   int getAddress1();
   void setAddress1(int);
   int getPort1();
   void setPort1(int);
   int getXChangeConn1();
   void setXChangeConn1(int);

private slots:
   void slotProtocolChanged(int);
   void validate();

};

#endif    //DRIVEDIALOG_H
