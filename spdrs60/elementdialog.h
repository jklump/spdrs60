/***************************************************************************
                           elementDialog.h
                           version 0.4.3
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
    email                : stefan.preis@wdr.de
    last modified        : 2004-12-31
***************************************************************************/
/****************************************************
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
  	
#include "resources.h"


class elementDialog : public QDialog
{
   Q_OBJECT

public:
   elementDialog( QWidget *parent=0, QStrList *elementData_=0 );

private:
   void setupElement( const char* );
   int  checkAddressLimits( QString );
   void showSubTypes( int );
   void setupDataFrame();
   void setupLogicFrame();

private slots:
   void slotSymbolChanged( int );
   void slotApplyPressed();
   void slotAddressChanged(const QString&);
   void slotSubTypeClicked( int );
   void slotDecoderChanged( int );
   void slotProtChanged( int );
   void slotBusChanged( int );
   void slotModuleChanged( int );
   void slotShowFBmodules();
   void slotEnable_LED_FB();

signals:
   void ApplyPressed();
   void sigShowFBmodules();

public:
   QStrList     *listNewData;

private:
   QLineEdit    *leAddress_1;
   QLineEdit    *leAddress_2;
   QLineEdit    *leText;

   QCheckBox    *cbRotate;
   QCheckBox    *cbInvert;
   QCheckBox    *cbLEDoff;
   QCheckBox    *cbChaConn1;
   QCheckBox    *cbChaConn2;
   QCheckBox    *cbAdrMod;

   QRadioButton *rbProtocol_MS;
   QRadioButton *rbProtocol_NA;

   QComboBox    *coboIcon;
   QComboBox    *coboDecoder;

   QStrList     *listIcon;
   QStrList     *listElementData;

   QLabel       *labelAddress_1;
   QLabel       *labelAddress_2;
   QLabel       *labelColour;
   QLabel       *labelRotate;
   QLabel       *labelInvert;
   QLabel       *labelSubTypeText;
   QLabel       *labelText;
   QLabel       *labelDecoder;
   QLabel       *labelChangeConn1;
   QLabel       *labelChangeConn2;
   QLabel       *labelTime;
   QLabel       *labelFB;
   QLabel       *labelFBmodule;
   QLabel       *labelFBport;
   QLabel       *labelBus;
   QLabel       *labelBus2;
   QLabel       *labelAdrMod;
   QLabel       *labelLEDoff;

   QPushButton  *buttOK;
   QPushButton  *buttSubType[3];
   QPushButton  *buttFBmodules;
   QButtonGroup *bgSubType;

   QSpinBox     *sbActiveTime;
   QSpinBox     *sbModule;
   QSpinBox     *sbPort;
   QSpinBox     *sbBus;

   QString      sSoldIcon;
   QFrame       *frData;
   QFrame       *frLogic;

   bool         bBlockMSignals;
   bool         bBlockBSignals;
   int          iPrevBusNo;
   int          iPrevModNo;
};

#endif    //ELEMENTDIALOG_H
