/***************************************************************************
                           elementdialog.h
                           version 0.4.8 $Revision: 1.6 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-11-27 14:05:40 $
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
   this is the header file to elementDialog.cpp
 ***************************************************************************/
#ifndef ELEMENTDIALOG_H
#define ELEMENTDIALOG_H

#include <qapplication.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qstrlist.h>
#include <qtooltip.h>
#include <qvalidator.h>
  	

class elementDialog: public QDialog
{
   Q_OBJECT

public:
   elementDialog(QWidget* parent = 0, QStrList* edList = 0);
   virtual ~elementDialog();

   int getGASubType();
   void setGASubType(int);
   int getSRCPBus1();
   void setSRCPBus1(int);
   int getSRCPBus2();
   void setSRCPBus2(int);
   QString getSymbolName();
   int getRotated();
   int getInverted();
   int getLEDsAreOff();
   QString getDecoder();
   QString getProtocol();
   int getAddress1();
   int getAddress2();
   int getXChangeConn1();
   int getXChangeConn2();
   int getDirection();
   QString getSymbolText();
   int getActiveTime();
   int getFBBus();
   void setFBBus(int);
   int getFBContact();
   void setFBContact(int);

private:
   void setupElement(const char*);
   void showSubTypes(int);

private slots:
   void slotSymbolChanged(int);
   void slotSubTypeClicked(int);
   void slotDecoderChanged(int);
   void slotProtChanged(int);
   void slotShowFBmodules();
   void slotEnable_LED_FB();
   void contactSBChanged(int);

signals:
   void sigShowFBmodules();

private:
   QLineEdit    *leAddress_1;
   QLineEdit    *leAddress_2;
   QLineEdit    *leText;
   QLineEdit    *srcpBus1LE;
   QLineEdit    *srcpBus2LE;
   QLineEdit    *fbBusLE;
   QLineEdit    *moduleLE;
   QLineEdit    *portLE;

   QCheckBox    *cbRotate;
   QCheckBox    *cbInvert;
   QCheckBox    *cbLEDoff;
   QCheckBox    *cbChaConn1;
   QCheckBox    *cbChaConn2;
   QCheckBox    *cbAdrMod;

   QRadioButton *rbProtocol_MS;
   QRadioButton *rbProtocol_NA;

   QComboBox    *IconComboBox;
   QComboBox    *coboDecoder;

   QStrList     *IconNameList;
   QStrList     *listElementData;

   QLabel       *srcpBus1Label;
   QLabel       *srcpBus2Label;
   QLabel       *labelAddress_1;
   QLabel       *labelAddress_2;
   QLabel       *subtypeLabel;
   QLabel       *labelText;
   QLabel       *labelDecoder;
   QLabel       *labelTime;
   QLabel       *labelFBmodule;
   QLabel       *labelFBport;
   QLabel       *labelFBBus;

   QPushButton  *buttOK;
   QPushButton  *buttSubType[3];
   QPushButton  *buttFBmodules;
   QButtonGroup *bgSubType;
   QGroupBox*    feedbackGB;

   QSpinBox     *activeTimeSB;
   QSpinBox     *contactSB;

   QString      sSoldIcon;
   QIntValidator* a1Validator;
   QIntValidator* a2Validator;

   int          gaSubType;
};

#endif    //ELEMENTDIALOG_H
