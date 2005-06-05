/***************************************************************************
                           options.h
                           version 0.4.3 $Revision: 1.3 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-06-05 13:04:19 $
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
  	
#include "resources.h"

#define  TABHEIGHT 550                 // constant height of pref dialog



class optionsDialog: public QTabDialog
{
   Q_OBJECT

public:
   optionsDialog(QWidget* parent=0);

private:
   void setupTabLayout();              // creates the tab with layout specs
   void setupTabData();                // creates the tab with data specs
   void setupTabInterface();           // creates the tab with interface specs
   void fillWithData();                // fills all tabs with actual data
   void readData();                    // reads data from init file
   int  checkForWarnings();            // check if any user entry is wrong

private slots:
   void slotGetAutofile();             // select a file to be auto-opened
   void slotAutoload(bool);            // activate autoload filename lineentry
   void slotDecoderChanged(int);       // selects new protocol if dec selected
   void slotChangeServer(int);         // de/activate server dependant fields
   void slotProtChanged(int);          // selects new dec if protocol changed
   void slotLimitModules(int);         // limits no of fb moduls on each bus
   void slotPortChanged(const QString&);// check for valid port number entries
   void slotSetRepaint();              // saves a necessary layout repaint

signals:
    void repaintLayout();              // send a repaint to all elements
    void refreshConfigData();          // send a re-read of init file to MainWin
    void showLogMessage(const QString&, int, int);

protected:
   virtual void done( int );           // what to do when window is closed
                                       // (save all or reject)
private:
   QCheckBox    *cbShowHp2;            // layout shows orange light for Hp2
   QCheckBox    *cbGenBubble;          // show general bubble help
   QCheckBox    *cbDataBubble;         // show element data as bubblehelp
   QCheckBox    *cbAutoload;           // activate autoloader
   QCheckBox    *cbAutoZP9;            // auto-start layout voltage
   QCheckBox    *cbAutoTTDir;          // auto-select turn dir of turntable
   QCheckBox    *cbAutologin;          // auto-login to server on start

   QRadioButton *rbShowAddr;           // show element's address or full text
   QRadioButton *rbShowTxt;
   QRadioButton *rbS88_16;             // user has 16 or 8 port feedback mods
   QRadioButton *rbS88_8;
   QRadioButton *rbProtMS;             // default protocol selector
   QRadioButton *rbProtNA;
   QRadioButton *rbSignalRed;          // init signals always red or as saved
   QRadioButton *rbSignalLay;
   QRadioButton *rbServer;             // SpDrS60 works with a SRCP server ...
   QRadioButton *rbEditsP;             // ... or with EDiTS Pro
   QRadioButton *rb6051;               // ... or with an original Märklin IF
   QRadioButton *rbIntelli;            // ... or with Intellibox

   QSpinBox     *sbActiveTime;         // default activation time for solenoids
   QSpinBox     *sbDefaultCols;        // no of default new columns
   QSpinBox     *sbRoutingTime;        // default delay between to elements in
                                       // a route
   QSpinBox     *sbFBmod_1;            // no of fb mods on bus 1 ... 4
   QSpinBox     *sbFBmod_2;
   QSpinBox     *sbFBmod_3;
   QSpinBox     *sbFBmod_4;

   QComboBox    *coboEditor;           // name of file editor
   QComboBox    *coboDecoder;          // name of default decoder
   QComboBox    *coboBrowser;          // name of help browser
   QComboBox    *coboCom;              // name of com interface
   QComboBox    *coboBaud;             // baudrate for that serial IF
   QComboBox    *coboData;             // no of databits
   QComboBox    *coboStop;             // no of stopbits
   QComboBox    *coboPari;             // type of parity

   QLineEdit    *leAutoload;           // name entry field for autoload file
   QLineEdit    *leHost;               // name of SRCP server
   QLineEdit    *lePort;               // port number to SRCP server
   QLineEdit    *leTTRoundTime;        // time for a whole turntable turn

   QFrame       *line;                 // various separator lines
   QLabel       *label;                // various text labels
   QLabel       *lServerIP;            // label for SRCP server name
   QLabel       *lServerPort;          // label for SRCP port number
   QLabel       *lCom;                 // labels for serial IF specs
   QLabel       *lBaud;
   QLabel       *lData;
   QLabel       *lStop;
   QLabel       *lPari;

   QPushButton  *buttGetAutofile;      // button to select autoload file
   bool         bRepaintNecessary;     // save a necessary layout repaint
   int          iSelectedServer;       // button ID for server or interface
};

#endif    //OPTIONSDIALOG_H
