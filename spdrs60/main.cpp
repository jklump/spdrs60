/***************************************************************************
                           main.cpp
                           version 0.5.2 $Revision: 1.17 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2007 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-05-19 21:19:40 $
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


#include <qapplication.h>
#include <qstring.h>
#include <qlocale.h>
#include <qtranslator.h>

#ifdef WIN32
#include "config_w32.h"
#else
#include "config.h"
#endif

#include "mainwindow.h"


int main(int argc, char* argv[])
{
   QApplication a(argc, argv);  // create a new Qt application

   if (QLocale::system().language() != QLocale::C) {

#ifdef QT_TRANSLATIONS_DIR
       QTranslator* qtTr = new QTranslator(0);
       Q_CHECK_PTR(qtTr);

       if (qtTr->load(QString("qt_") + QLocale::system().name(),
                   QT_TRANSLATIONS_DIR))
           a.installTranslator(qtTr);
       else {
           delete qtTr;
           qWarning("No Qt translation for locale '%s' found.",
                   QLocale::system().name().data());
       }
#endif

       QTranslator* spdrs60Tr = new QTranslator(0);
       Q_CHECK_PTR(spdrs60Tr);

       if (spdrs60Tr->load(QString("spdrs60_") + QLocale::system().name(),
                   RES_DIR))
           a.installTranslator(spdrs60Tr);
       else {
           delete spdrs60Tr;
           qWarning("No spdrs60 translation for locale '%s' in '%s' found.",
                   QLocale::system().name().data(), RES_DIR);
       }
   }

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

