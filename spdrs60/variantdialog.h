/***************************************************************************
                           variantdialog.h
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
   this is the header file to variantdialog.cpp
 ***************************************************************************/

#ifndef VARIANTDIALOG_H
#define VARIANTDIALOG_H

#include <qdialog.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qradiobutton.h>
#include <qvaluelist.h>


class VariantDialog: public QDialog
{
   Q_OBJECT

   QButtonGroup* variantBG;
   QValueList<QPixmap> pmList;
   QLabel* imageLabel;


public:
   VariantDialog(QWidget* parent = 0);
   int getChoice();
   void setChoice(int);
   void addVariant(const QString&);
   void addVariant(const QString&, const QPixmap&);
   void showImage(bool show);

private slots:
   void selectionChanged(int);
};

#endif    //VARIANTDIALOG_H
