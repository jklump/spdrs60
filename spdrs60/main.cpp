/***************************************************************************
                           main.cpp
                           version 0.5.3 $Revision: 1.20 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2008 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-09-10 18:34:15 $
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
   This file is the main file and handles all basic setting of a Qt-
   application and stores the command line arguments
 **************************************************************************/


#include <qapplication.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qtranslator.h>

#ifdef WIN32
#include "config_w32.h"
#else
#include "config.h"
#endif

#include "mainwindow.h"


int main(int argc, char* argv[])
{
   QApplication a(argc, argv);

#ifdef QT_TRANSLATIONS_DIR

   // translator for Qt library strings
   QTranslator qtTr(0);

   if (qtTr.load(QString("qt_") + QTextCodec::locale(), QT_TRANSLATIONS_DIR))
       a.installTranslator(&qtTr);
   else
       qWarning("No Qt translation for locale '%s' found.",
               QTextCodec::locale());
#endif
   
   // translator for application strings
   QTranslator spdrs60Tr(0);

   if (spdrs60Tr.load(QString("spdrs60_") + QTextCodec::locale(),
               TRANSLATIONSDIR))
       a.installTranslator(&spdrs60Tr);
   else
       qWarning("No spdrs60 translation for locale '%s' in '%s' found.",
               QTextCodec::locale(), TRANSLATIONSDIR);

   MainWindow* spdrs60Window = new MainWindow();
   Q_CHECK_PTR(spdrs60Window);
   spdrs60Window->setCaption(PACKAGE);
   spdrs60Window->resize(720, 480);

   //necessary for "-geometry" command-line option
   a.setMainWidget(spdrs60Window);
   spdrs60Window->show();

   /*only the first application window autoloads a layoutfile*/
   int ac = qApp->argc();

   if (ac == 1)
       spdrs60Window->readAutoloadFile();
   else {
       spdrs60Window->openFile(qApp->argv()[1]);
       // loop over all arguments -> open more application windows
       if (ac > 2)
           for (int i = 2; i < ac; i++)
               spdrs60Window->openFileWindow(qApp->argv()[i]);
   }
   
   return a.exec();                                     
}

