/***************************************************************************
                           main.cpp
                           version 0.4.8 $Revision: 1.4 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2006-01-07 21:20:08 $
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
   this file is the main file and handles all basic setting up of a Qt-
   application and stores the command line arguments
 **************************************************************************/
#include <qapplication.h>

#include <qfont.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qtranslator.h>

#include <qdir.h>

#include <stdlib.h>
#include "config.h"

#include "mainwindow.h"

/* configuration variables with default setup */
bool bFBport[MAX_FB];            // status of each feedback port, only
                                 // used for module window
bool SHOW_HP2= true;             // show signals with orange HP2 light?
bool SHOW_TOOLTIPS= true;        // show tooltips?
bool SHOW_DATA_TOOLTIPS= false;  // show element's data in a tooltip?
bool LOAD_DEF_LAYOUT= false;     // autoloads a layout
bool SHOW_TXT_ADR= true;         // show text or decoder address in solenoids
bool INIT_SIGNALS= true;         // how are the signal initialized
bool AUTO_ZP9= false;            // autostart power on layout?
bool AUTO_TT_DIR= false;         // autoselect rotating direction at digital tt?
bool SERVERLOGIN= false;         // auto serverlogin on program start
int  SERVER= SRCP;               // send data to which server?

int  DEF_COLS= 12;               // number of default layout columns
int  DEF_ROWS= 12;               // number of default layout rows
int  DEF_PROTOCOL= 1;            // default protocol name
int  ACTIVE_TIME= 50;            // solenoids' activation time
int  ROUTING_TIME= 100;          // delay time while routing between elements
int  FEEDBACK= FB_16;            // type of feedback modules (to be replaced!)
int  FB_MODULES_[4] = {8,0,0,0}; // number of feedback modules on each bus
int  PORT= 12345;                // TCP/IP port for connection to erddcd host
double TT_ROUND_TIME= 20.0;      // time in secs for a whole turntable turn

QString DEF_LAYOUT ="-1";        // name of autoload layout
QString EDITOR= "kwrite";        // name of extern editor prog
QString BROWSER= "konqueror";    // name of extern browser prog for docs
QString DEF_DECODER;         // name of default decoder type
QString HOST= "localhost";        // hostname where erddcd runs
                             // data (name does NOT contain .dat.gbs etc)
QString COMX;                // Name of serial interface
QString BAUD;                // Baudrate of a serial interface controlling
QString DATAB;               // Databits of this serial interface
QString STOPB;               // Stopbits for this serial interface
QString PARI;                // Parity of this serial interface



int main(int argc, char* argv[])
{
   QApplication a(argc, argv);  // create a new Qt application

   QString qtenv = getenv("QTDIR");
   // translation file for QT
   QTranslator qt(0);
   qt.load(QString("qt_") + QTextCodec::locale(), qtenv + "/translations");
   a.installTranslator(&qt);

   // translation file for application strings
   QTranslator spdrs60Tr(0);
   spdrs60Tr.load(QString("spdrs60_") + QTextCodec::locale(), RES_DIR);
   a.installTranslator(&spdrs60Tr);

   MainWindow* spdrs60Window = new MainWindow();
   Q_CHECK_PTR(spdrs60Window);
   spdrs60Window->setCaption(QString(APP_NAME));
   spdrs60Window->resize(720, 480);

   //necessary for "-geometry" command-line option:
   a.setMainWidget(spdrs60Window);
   spdrs60Window->show();

   if (qApp->argc() == 1) {
       /*only the first application window autoloads a layoutfile*/
       if (LOAD_DEF_LAYOUT)
           spdrs60Window->readAutoloadFile();
   }
   else
       spdrs60Window->openFile(qApp->argv()[1]);
   
   return a.exec();                                     
}

