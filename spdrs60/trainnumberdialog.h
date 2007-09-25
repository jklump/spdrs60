/*
 * trainnumberdialog.h
 * -------------------
 * Copyright    : (C) 2007 Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-24
 * Last modified: $Date: 2007-09-25 18:14:51 $
 *                $Revision: 1.1 $
 *
 * this is the header file to trainnumberdialog.cpp
 */

/*************************************************************************
 *                                                                       *
 *  This program is free software; you can redistribute it and/or modify *
 *  it under the terms of the GNU General Public License as published by *
 *  the Free Software Foundation; either version 2 of the License, or    *
 *  (at your option) any later version.                                  *
 *                                                                       *
 *************************************************************************/


#ifndef TRAINNUMBERDIALOG_H
#define TRAINNUMBERDIALOG_H

#include <qdialog.h>                    
#include <qlineedit.h>


class TrainNumberDialog: public QDialog
{
   Q_OBJECT

public:
   TrainNumberDialog(QWidget* parent = 0, const char* name = NULL);

private:

signals:
   void trainNumberChanged(unsigned int, unsigned int);

private slots:
   void setTrainNumber();

private:
   QLineEdit* routeLE;
   QLineEdit* trainLE;
};

#endif
