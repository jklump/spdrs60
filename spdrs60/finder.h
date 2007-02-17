/***************************************************************************
                           finder.h
                           version 0.5.2 $Revision: 1.8 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2005-2007 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-02-17 07:23:32 $
***************************************************************************/

/**************************************************************************
 *                                                                        *
 *   This program is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation; either version 2 of the License, or    *
 *   (at your option) any later version.                                  *
 *                                                                        *
 **************************************************************************/

/**************************************************************************
   this is the header file to Finder.cpp
 **************************************************************************/

#ifndef FINDER_H
#define FINDER_H

#include <qdialog.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qtooltip.h>
#include <qpushbutton.h>
#include <qradiobutton.h>


class Finder: public QDialog
{
    Q_OBJECT

public:
    Finder(QWidget* parent = 0);
    QString getSearchText();
    int getDataType();
    int getMatchType();

private:
    QPushButton*  buttSearch;
    QLineEdit*    leSearch;
    QButtonGroup* dataBG;
    QButtonGroup* matchBG;
   
private slots:
    void slotActivateSearchButt(const QString&);

};

#endif
