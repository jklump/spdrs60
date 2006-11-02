/***************************************************************************
                           newLayoutDialog.h
                           version 0.5.0 $Revision: 1.5 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                           (C) 2004-2006 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2006-11-02 16:54:32 $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 **************************************************************************/

/***************************************************************************
   this is the header file to newDialog.cpp
 **************************************************************************/

#ifndef NEWLAYOUTDIALOG_H
#define NEWLAYOUTDIALOG_H

#include <qcheckbox.h>                     
#include <qdialog.h>                     
#include <qframe.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtooltip.h>

#include "resources.h"


class newLayoutDialog: public QDialog
{
    Q_OBJECT

public:
    newLayoutDialog(QWidget* parent=0);
    int getColumns();
    int getRows();
    QString getHost();
    int getPort();
    bool getAutoLogin();
    bool getAutoPower();
    bool getAutoSendAll();
    void setColumns(int);
    void setRows(int);
    void setHost(const QString&);
    void setPort(int);
    void setAutoLogin(bool);
    void setAutoPower(bool);
    void setAutoSendAll(bool);

private:
    QSpinBox* sbEnterCols;
    QSpinBox* sbEnterRows;
    QLineEdit* hostLE;
    QLineEdit* portLE;
    QCheckBox* autologinCB;
    QCheckBox* autopowerCB;
    QCheckBox* autosendallCB;
};

#endif
