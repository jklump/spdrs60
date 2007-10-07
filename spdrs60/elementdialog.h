/***************************************************************************
                           elementdialog.h
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-10-07 17:31:03 $
                           $Revision: 1.21 $
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
   this is the header file to elementdialog.cpp
 ***************************************************************************/

#ifndef ELEMENTDIALOG_H
#define ELEMENTDIALOG_H

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qvalidator.h>

#include "element.h"


class ElementDialog: public QDialog
{
   Q_OBJECT

public:
   ElementDialog(QWidget* parent = 0, int idx = 0);

   int getGASubType();
   void setGASubType(int);
   int getSRCPBus1();
   void setSRCPBus1(int);
   int getSRCPBus2();
   void setSRCPBus2(int);
   void setClassId(element::SpdrItemClassId);
   int getRotated();
   void setRotated(int);
   int getInverted();
   void setInverted(int);
   int getLEDsAreOff();
   void setLEDsAreOff(int);
   QString getDecoder();
   void setDecoder(const QString&);
   int getProtocol();
   void setProtocol(int);
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
   QLineEdit    *leText;
   QLineEdit    *address1LE;
   QLineEdit    *address2LE;
   QLineEdit    *srcpBus1LE;
   QLineEdit    *srcpBus2LE;
   QLineEdit    *fbBusLE;
   QLineEdit    *moduleLE;
   QLineEdit    *portLE;

   QGroupBox*   feedbackGB;

   QCheckBox    *cbAddrLabeling;
   QCheckBox    *cbRotate;
   QCheckBox    *cbInvert;
   QCheckBox    *cbLEDoff;
   QCheckBox    *xchConn1CB;
   QCheckBox    *xchConn2CB;
   QCheckBox    *cbAdrMod;

   QRadioButton *rbProtocol_MS;
   QRadioButton *rbProtocol_NA;
   QRadioButton *rbProtocol_PS;
   QRadioButton *rbProtocol_SE;

   QComboBox    *coboDecoder;

   QLabel       *srcpBus1Label;
   QLabel       *srcpBus2Label;
   QLabel       *address1Lbl;
   QLabel       *address2Lbl;
   QLabel       *port1Label;
   QLabel       *port2Label;
   QLabel       *labelDecoder;
   QLabel       *labelText;
   QLabel       *labelTime;
   QLabel       *labelFBBus;
   QLabel       *labelFBContact;
   QLabel       *labelFBmodule;
   QLabel       *labelFBport;

   QPushButton  *buttOK;
   QPushButton  *buttSubType[3];
   QPushButton  *buttFBmodules;
   QButtonGroup* bgSubType;
   QButtonGroup* protocolBG;

   QSpinBox     *activeTimeSB;
   QSpinBox     *contactSB;
   QSpinBox     *port1SB;
   QSpinBox     *port2SB;

   QIntValidator* a1Validator;
   QIntValidator* a2Validator;

   int          gaSubType;
   int          gaDirection;
   int          addresscount;
   QString      lastDecoder;
   element::SpdrItemClassId classid;

   void showSubTypes(int);
   void updateValidators();

private slots:
   void slotAddress1Changed(const QString&);
   void slotSymbolChanged();
   void slotSubTypeClicked(int);
   void slotDecoderChanged(int);
   void slotProtocolChanged(int);
   void slotShowFBmodules();
   void slotEnable_LED_FB();
   void contactSBChanged(int);
   void letteringChanged(bool);
   void invertedChanged(bool);

signals:
   void sigShowFBmodules();
};

#endif    //ELEMENTDIALOG_H
