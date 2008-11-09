/***************************************************************************
                           newlayoutdialog.h
                           version 0.5.3 $Revision: 1.14 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                           (C) 2004-2008 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-11-09 20:54:53 $
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
   this is the header file to newlayoutdialog.cpp
 **************************************************************************/

#ifndef NEWLAYOUTDIALOG_H
#define NEWLAYOUTDIALOG_H

#include <qcheckbox.h>                     
#include <qtabdialog.h>                     
#include <qframe.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtooltip.h>
#include <qvalidator.h>


class newLayoutDialog: public QTabDialog
{
    Q_OBJECT

public:
    newLayoutDialog(QWidget* parent = 0);
    int getColumns();
    int getRows();
    unsigned int getSwitchboxId();
    QString getSwitchboxName();
    unsigned int getLayoutId();
    QString getLayoutName();
    QString getHost();
    unsigned int getPort();
    bool getAutoLogin();
    bool getAutoPower();
    bool getAutoSendAll();
    void setColumns(int);
    void setRows(int);
    void setSwitchboxId(unsigned int);
    void setSwitchboxName(const QString&);
    void setLayoutId(unsigned int);
    void setLayoutName(const QString&);
    void setHost(const QString&);
    void setPort(unsigned int);
    void setAutoLogin(bool);
    void setAutoPower(bool);
    void setAutoSendAll(bool);

private:
    void setupGeneralTab();
    void setupCrcfTab();

    QSpinBox* sbEnterCols;
    QSpinBox* sbEnterRows;
    QLineEdit* switchboxidLE;
    QLineEdit* switchboxnameLE;
    QLineEdit* layoutidLE;
    QLineEdit* layoutnameLE;
    QLineEdit* hostLE;
    QLineEdit* portLE;
    QCheckBox* autologinCB;
    QCheckBox* autopowerCB;
    QCheckBox* autosendallCB;
    QValidator* portValidator;

private slots:

};

#endif // NEWLAYOUTDIALOG_H
