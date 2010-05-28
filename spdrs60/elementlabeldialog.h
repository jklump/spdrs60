/***************************************************************************
                           elementlabeldialog.h
                           -------------------------------
    copyright            : (C) 2010 by Guido Scholz
    e-mail               : guido.scholz@bayernline.de
    last modified        : $Date: 2008/11/05 08:42:40 $
                           $Revision: 1.24 $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
   this is the header file to elementlabeldialog.cpp
 ***************************************************************************/

#ifndef ELEMENTLABELDIALOG_H
#define ELEMENTLABELDIALOG_H

#include <qdialog.h>
#include <qlineedit.h>

class ElementLabelDialog: public QDialog
{
   Q_OBJECT

   QLineEdit* leText;

public:
   ElementLabelDialog(QWidget* parent = 0);
   QString getSymbolText();
   void setSymbolText(const QString&);
};

#endif    //ELEMENTLABELDIALOG_H
