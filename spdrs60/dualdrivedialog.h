/***************************************************************************
                           dualdrivedialog.h
                           -------------------------------
    copyright            : (C) 2010 by Guido Scholz
    e-mail               : guido.scholz@bayernline.de
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
   this is the header file to dualdrivedialog.cpp
 ***************************************************************************/

#ifndef DUALDRIVEDIALOG_H
#define DUALDRIVEDIALOG_H

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qdialog.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qvalidator.h>


class DualDriveDialog: public QDialog
{
   Q_OBJECT

   QButtonGroup* protocol1BG;
   QButtonGroup* protocol2BG;
   QSpinBox* activeTime1SB;
   QSpinBox* activeTime2SB;
   QSpinBox* port1SB;
   QSpinBox* port2SB;
   QLabel* port1Label;
   QLabel* port2Label;
   QLineEdit* address1LE;
   QLineEdit* address2LE;
   QLineEdit* srcpBus1LE;
   QLineEdit* srcpBus2LE;
   QCheckBox* xchConn1CB;
   QCheckBox* xchConn2CB;
   QRadioButton* rbProtocol1_MS;
   QRadioButton* rbProtocol1_NA;
   QRadioButton* rbProtocol1_PS;
   QRadioButton* rbProtocol1_SE;
   QRadioButton* rbProtocol2_MS;
   QRadioButton* rbProtocol2_NA;
   QRadioButton* rbProtocol2_PS;
   QRadioButton* rbProtocol2_SE;
   QIntValidator* addressVdt1;
   QIntValidator* addressVdt2;

   void updateValidator1();
   void updateValidator2();
   void updateAddressTooltip1();
   void updateAddressTooltip2();

public:
   DualDriveDialog(QWidget* parent = 0);
   int getProtocol1();
   void setProtocol1(int);
   int getProtocol2();
   void setProtocol2(int);
   int getActiveTime1();
   void setActiveTime1(int);
   int getActiveTime2();
   void setActiveTime2(int);
   int getSRCPBus1();
   void setSRCPBus1(int);
   int getSRCPBus2();
   void setSRCPBus2(int);
   int getAddress1();
   void setAddress1(int);
   int getAddress2();
   void setAddress2(int);
   int getPort1();
   void setPort1(int);
   int getPort2();
   void setPort2(int);
   int getXChangeConn1();
   void setXChangeConn1(int);
   int getXChangeConn2();
   void setXChangeConn2(int);

private slots:
   void slotProtocol1Changed(int);
   void slotProtocol2Changed(int);
   void validate();

};

#endif    //DUALDRIVEDIALOG_H
