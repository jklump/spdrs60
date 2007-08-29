/***************************************************************************
 aboutdialog.h
 -----------------
 Begin      : 2007-08-26
 Copyright  : (C) 2007 by Guido Scholz
 E-Mail     : guido.scholz@bayernline.de
 Description: Dialog window to display program information
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <qdialog.h>


class AboutDialog: public QDialog {
    Q_OBJECT
   
public:
    AboutDialog(QWidget* parent = 0);

};

#endif    //ABOUTDIALOG_H
