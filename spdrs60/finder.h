/***************************************************************************
                           Finder.h
                           version 0.4.3
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
    email                : stefan.preis@wdr.de
    last modified        : 2004-12-31
***************************************************************************/
/**/

/*****************************************************************************
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by     *
 *   the Free Software Foundation; either version 2 of the License, or        *
 *   (at your option) any later version.                                      *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
   this is the header file to Finder.cpp
 ******************************************************************************/
#ifndef FINDER_H
#define FINDER_H

#include <qdialog.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qtooltip.h>
#include <qpushbutton.h>
#include <qradiobutton.h>

#include "resources.h"


class Finder: public QDialog
{
   Q_OBJECT

public:
   Finder( QWidget *parent=0 );            // creator of Finder window

private:


signals:
   void sigFind( QString, int, bool );     // send search string to gbs

private slots:
   void slotActivateSearchButt(const QString&);// activates search butt if a char
                                           // has been entered
   void slotBeginSearch();                 // begins searching
   void slotSaveSearchType( int );         // saves search type button IDs
   void slotSaveMultiType( int );          // saves single/multi search type

private:
   QPushButton   *buttSearch;              // button to start searching
   QPushButton   *buttCancel;              // button to close dialog
   QLineEdit     *leSearch;                // line edit for search string

   int           iSearchType;              // search type button ID
   int           iMultiType;               // single/multi type button ID
};

#endif
