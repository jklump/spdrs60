/***************************************************************************
                           newLayoutDialog.h
                           version 0.4.3 $Revision: 1.2 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                           (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-05-07 12:22:43 $
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

#include <qdialog.h>                     
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtooltip.h>

#include "resources.h"


class newLayoutDialog: public QDialog
{
    Q_OBJECT

public:
    newLayoutDialog(QWidget* parent=0); // creator of new layout dialog
    int getColumns();
    int getRows();
    void setColumns(int);
    void setRows(int);

private:
    QSpinBox* sbEnterCols;
    QSpinBox* sbEnterRows;
};

#endif
