/***************************************************************************
                           fbmodule.h
                           version 0.4.3
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
    email                : stefan.preis@wdr.de
    last modified        : 2004-12-31
***************************************************************************/

/******************************************************************************
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by     *
 *   the Free Software Foundation; either version 2 of the License, or        *
 *   (at your option) any later version.                                      *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
   this is the header file to fbmodule.cpp
 ******************************************************************************/
#ifndef FBMODULE_H
#define FBMODULE_H

#include <qfont.h>
#include <qpainter.h>                       
#include <qpixmap.h>
#include <qwidget.h>

#include "resources.h"


class fbModule : public QWidget
{
   Q_OBJECT

public:
   fbModule( QWidget *parent=0, int iModNr_=0 ); // creator of a module

private slots:
   void slotSetupModule( int );   // gets update event through feedback.cpp

private:
   int iModNr;                    // number of module
};

#endif
