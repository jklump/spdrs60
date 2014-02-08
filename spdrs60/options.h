/***************************************************************************
                           options.h
                           version 0.5.3 $Revision: 1.22 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2008 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-11-05 08:42:40 $
***************************************************************************/

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/

/**************************************************************************
   this is the header file to options.cpp
 **************************************************************************/

#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <qapplication.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qtabdialog.h>
#include <qtextstream.h>
  	
#include "preferences.h"


class optionsDialog: public QTabDialog
{
   Q_OBJECT

public:
    optionsDialog(QWidget* parent = NULL);

    void getPreferences(Preferences&);
    void setPreferences(const Preferences&);

private:
   void setupElementTab();
   void setupLayoutTab();
   void setupDigitalTab();
   void setupFeedbackTab();
   void setupFeedbackTypeTab();
   void setupGenericMessagesTab();
   bool valuesAreValid();

private slots:
   void slotGetAutofile();
   void slotAutoloadToggled(bool);
   void slotLimitModules(int);
   void fixFBBusNumbers(int);
   void selectFbModuleType(int);
   void globalBubbleHelpChanged(bool);

private:
   QCheckBox* cbShowHp2;
   QCheckBox* cbShowBlinkingTurnouts;
   QCheckBox* allwaysSendState;
   QCheckBox* cbGenBubble;
   QCheckBox* cbDataBubble;
   QCheckBox* cbAutoload;
   QCheckBox* cbFullScreen;
   QCheckBox* cbAutosave;
   QCheckBox* cbAutoTTDir;
   QCheckBox* cbConvertTime;
   QCheckBox* cbShowTime;

   QCheckBox* sroutestateCB;
   QCheckBox* rroutestateCB;
   QCheckBox* strainnumberCB;
   QCheckBox* rtrainnumberCB;
   QCheckBox* routetypeCB;
   QCheckBox* tracksectionCB;

   QRadioButton *rbShowAddr;
   QRadioButton *rbShowTxt;
   QRadioButton *rb16inputs;
   QRadioButton *rb8inputs;
   QRadioButton *fixedBusesRB;
   QRadioButton *flexBusesRB;
   QRadioButton *rbSignalRed;
   QRadioButton *rbSignalLay;

   QSpinBox *sbActiveTime;
   QSpinBox *sbDefaultCols;
   QSpinBox *sbDefaultRows;
   QSpinBox *sbRoutingTime;

   QSpinBox *sbFBmod_1;
   QSpinBox *sbFBmod_2;
   QSpinBox *sbFBmod_3;
   QSpinBox *sbFBmod_4;

   QComboBox *coboEditor;
   QComboBox *coboBrowser;

   QLineEdit* leAutoload;
   QLineEdit* leTTRoundTime;
   QLineEdit* bus1LE;
   QLineEdit* bus2LE;
   QLineEdit* bus3LE;
   QLineEdit* bus4LE;

   QPushButton* buttGetAutofile;
   QButtonGroup* protocolBG;
   QButtonGroup* feedbackTypeGB;
   QGroupBox* selectrixGB;
};

#endif    //OPTIONSDIALOG_H
