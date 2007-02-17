/***************************************************************************
                           options.h
                           version 0.5.2 $Revision: 1.17 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2007 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-02-17 07:23:32 $
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
    optionsDialog(QWidget* parent = 0);

    void getPreferences(Preferences&);
    void setPreferences(const Preferences&);

private:
   void setupElementTab();
   void setupLayoutTab();              // creates the tab with layout specs
   void setupDigitalTab();             // creates the tab with data specs
   void setupFeedbackTab();            // creates the tab with data specs
   void setupFeedbackTypeTab();        // creates the tab with data specs
   bool valuesAreValid();              // check if any user entry is wrong

private slots:
   void slotGetAutofile();             // select a file to be auto-opened
   void slotAutoload(bool);            // activate autoload filename lineentry
   void slotDecoderChanged(int);       // selects new protocol if dec selected
   void slotProtChanged(int);          // selects new dec if protocol changed
   void slotLimitModules(int);         // limits no of fb moduls on each bus
   void fixFBBusNumbers(int);
   void selectFbModuleType(int);

private:
   QCheckBox* cbShowHp2;            // layout shows orange light for Hp2
   QCheckBox* cbShowBlinkingTurnouts;
   QCheckBox* allwaysSendState;     // send solenoid state on routing
   QCheckBox* cbGenBubble;          // show general bubble help
   QCheckBox* cbDataBubble;         // show element data as bubblehelp
   QCheckBox* cbAutoload;           // activate autoloader
   QCheckBox* cbAutosave;           // activate autosaver
   QCheckBox* cbAutoTTDir;          // auto-select turn dir of turntable
   QCheckBox* cbConvertTime;        // convert SRCP time string

   QRadioButton *rbShowAddr;           // show element's address or full text
   QRadioButton *rbShowTxt;
   QRadioButton *rb16inputs;           // user has feedback modules with
   QRadioButton *rb8inputs;            // 16 or 8 inputs
   QRadioButton *fixedBusesRB;
   QRadioButton *flexBusesRB;
   QRadioButton *rbProtMS;             // default protocol selector
   QRadioButton *rbProtNA;
   QRadioButton *rbProtPS;
   QRadioButton *rbProtSE;
   QRadioButton *rbSignalRed;          // init signals always red or as saved
   QRadioButton *rbSignalLay;

   QSpinBox     *sbActiveTime;         // default activation time for solenoids
   QSpinBox     *sbDefaultCols;        // no of default new columns
   QSpinBox     *sbDefaultRows;        // no of default new rows
   QSpinBox     *sbRoutingTime;        // default delay between to elements in
                                       // a route
   QSpinBox     *sbFBmod_1;
   QSpinBox     *sbFBmod_2;
   QSpinBox     *sbFBmod_3;
   QSpinBox     *sbFBmod_4;

   QComboBox    *coboEditor;           // name of file editor
   QComboBox    *coboDecoder;          // name of default decoder
   QComboBox    *coboBrowser;          // name of help browser

   QLineEdit    *leAutoload;           // name entry field for autoload file
   QLineEdit    *leTTRoundTime;        // time for a whole turntable turn
   QLineEdit*    bus1LE;
   QLineEdit*    bus2LE;
   QLineEdit*    bus3LE;
   QLineEdit*    bus4LE;

   QPushButton  *buttGetAutofile;      // button to select autoload file

   QButtonGroup* feedbackTypeGB;
   QGroupBox*    selectrixGB;
};

#endif    //OPTIONSDIALOG_H
