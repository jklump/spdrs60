/***************************************************************************
                           elementdialog.h
                           version 0.4.8 $Revision: 1.9 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2006-05-13 05:54:59 $
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
#include <qtooltip.h>
#include <qvalidator.h>
  	

class elementDialog: public QDialog
{
   Q_OBJECT

public:
   elementDialog(QWidget* parent = 0, int idx = 0);
   virtual ~elementDialog();

   int getGASubType();
   void setGASubType(int);
   int getSRCPBus1();
   void setSRCPBus1(int);
   int getSRCPBus2();
   void setSRCPBus2(int);
   QString getSymbolName();
   void setSymbolName(const QString&);
   int getRotated();
   void setRotated(int);
   int getInverted();
   void setInverted(int);
   int getLEDsAreOff();
   void setLEDsAreOff(int);
   QString getDecoder();
   void setDecoder(const QString&);
   QString getProtocol();
   void setProtocol(const QString&);
   int getAddress1();
   void setAddress1(int);
   int getAddress2();
   void setAddress2(int);
   int getXChangeConn1();
   void setXChangeConn1(int);
   int getXChangeConn2();
   void setXChangeConn2(int);
   int getDirection();
   void setDirection(int);
   QString getSymbolText();
   void setSymbolText(const QString&);
   int getActiveTime();
   void setActiveTime(int);
   int getFBBus();
   void setFBBus(int);
   int getFBContact();
   void setFBContact(int);

private:
   void setupElement(const char*);
   void showSubTypes(int);
   void updateValidators();


private slots:
   void slotAddress1Changed(const QString&);
   void slotSymbolChanged(int);
   void slotSubTypeClicked(int);
   void slotDecoderChanged(int);
   void slotProtChanged(int);
   void slotShowFBmodules();
   void slotEnable_LED_FB();
   void contactSBChanged(int);
   void letteringChanged(bool);

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

   QCheckBox    *cbAddrLabeling;
   QCheckBox    *cbRotate;
   QCheckBox    *cbInvert;
   QCheckBox    *cbLEDoff;
   QCheckBox    *cbChaConn1;
   QCheckBox    *cbChaConn2;
   QCheckBox    *cbAdrMod;

   QRadioButton *rbProtocol_MS;
   QRadioButton *rbProtocol_NA;
   QRadioButton *rbProtocol_PS;

   QComboBox    *IconComboBox;
   QComboBox    *coboDecoder;

   QStrList     *IconNameList;

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
   int          gaDirection;
   QString      lastDecoder;
};

#endif    //ELEMENTDIALOG_H
