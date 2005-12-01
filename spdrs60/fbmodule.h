/***************************************************************************
                           fbmodule.h
                           version 0.4.8 $Revision: 1.3 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                           (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-12-01 20:37:04 $
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

#include <qwidget.h>

#include "resources.h" // TODO: check this


class fbModule: public QWidget
{
   Q_OBJECT

public:
   fbModule(QWidget* parent = 0, unsigned int modid = 0);

private slots:
   void slotSetupModule(unsigned int, unsigned int, unsigned int);

private:
   unsigned int iModNr;
};

#endif
