/***************************************************************************
                           feedback.h
                           version 0.4.3
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
    email                : stefan.preis@wdr.de
    last modified        : 2004-12-31
***************************************************************************/
/***/

/******************************************************************************
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by     *
 *   the Free Software Foundation; either version 2 of the License, or        *
 *   (at your option) any later version.                                      *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
   this is the header file to feedback.cpp
 ******************************************************************************/
#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <qdialog.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qtooltip.h>

#include "resources.h"
#include "fbmodule.h"


class feedback: public QDialog
{
   Q_OBJECT

public:
   feedback( QWidget *parent=0 ); // creator of this window

private:
   void showModules();            // shows all modules on a bus

public slots:
   void slotUpdateModules(unsigned int);
   // gets update event through GBSArea.cpp

private slots:
   void slotNextPage();           // shows next page/bus
   void slotPrevPage();           // shows previous page/bus

signals:
   void updateModule( int );      // sends update to fbModule.cpp

private:
   fbModule    *module[62];       // a series of s88 modules
   int         iPage;             // number of page = busnumber - 1
   int         iMdCnt;            // shown modules on a page
   QPushButton *buttNextPage;     // button to display next page/bus
   QPushButton *buttPrevPage;     // button to display previous page/bus
   QLabel      *lPageInfo;        // label with no of s88 bus    
};

#endif
