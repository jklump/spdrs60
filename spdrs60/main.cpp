/***************************************************************************
                           main.cpp
                           version 0.5.0 $Revision: 1.7 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2006 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2006-08-21 16:21:56 $
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
   this file is the main file and handles all basic setting of a Qt-
   application and stores the command line arguments
 **************************************************************************/

#include <stdlib.h>

#include <qapplication.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qtranslator.h>

#include "config.h"
#include "mainwindow.h"


int main(int argc, char* argv[])
{
   QApplication a(argc, argv);  // create a new Qt application

   QString qtenv = getenv("QTDIR");
   // translation file for Qt
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

   //necessary for "-geometry" command-line option
   a.setMainWidget(spdrs60Window);
   spdrs60Window->show();

   /*only the first application window autoloads a layoutfile*/
   if (qApp->argc() == 1) {
           spdrs60Window->readAutoloadFile();
   }
   else
       //TODO: loop over all arguments -> open more application windows
       spdrs60Window->openFile(qApp->argv()[1]);
   
   return a.exec();                                     
}

