/***************************************************************************
                           newlayoutdialog.cpp
                           version 0.4.8 $Revision: 1.2 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                           (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-05-07 12:22:43 $
****************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
   this code provides an user interface to enter the number of new columns
 ***************************************************************************/

#include <qgroupbox.h>
#include <qhbox.h>
#include <qlayout.h>

#include "newlayoutdialog.h"

extern bool SHOW_TOOLTIPS;
extern int DEF_COLS;
extern int DEF_ROWS;


newLayoutDialog::newLayoutDialog(QWidget* parent)
: QDialog(parent, "newLayoutDialog", true)
{
    setCaption(tr("Create new layout"));
    QVBoxLayout* newlayoutDlgLayout = new QVBoxLayout(this, 10, 6);

    QGroupBox* dimensionsGB = new QGroupBox(0, Horizontal,
            tr("Layout dimensions"), this, "dimensionsGB");
    newlayoutDlgLayout->addWidget(dimensionsGB);

    QVBoxLayout* box = new QVBoxLayout(dimensionsGB->layout(), 6);
    
    QHBoxLayout* columnsLayout = new QHBoxLayout(box);

    QLabel* columnsLabel = new QLabel(tr("&Columns:"), dimensionsGB);
    columnsLayout->addWidget(columnsLabel);
    
    QSpacerItem* spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    columnsLayout->addItem(spacer);
    
    sbEnterCols = new QSpinBox(MIN_COLS, MAX_COLS, 1, dimensionsGB,
            "sbEnterCols");
    columnsLayout->addWidget(sbEnterCols);
    sbEnterCols->setValue(DEF_COLS);
    //sbEnterCols->setFocus();
    sbEnterCols->setWrapping(true);     // enables to spin "over" the limits
    if (SHOW_TOOLTIPS == true)
        QToolTip::add(sbEnterCols, tr("Choose or enter the number of\n"
                                      "columns for an empty layout"));

    columnsLabel->setBuddy(sbEnterCols);


    QHBoxLayout* rowsLayout = new QHBoxLayout(box);

    QLabel* rowsLabel = new QLabel(tr("&Rows:"), dimensionsGB);
    rowsLayout->addWidget(rowsLabel);
    
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    rowsLayout->addItem(spacer);
    
    sbEnterRows = new QSpinBox(MIN_ROWS, MAX_ROWS, 1, dimensionsGB,
            "sbEnterCols");
    rowsLayout->addWidget(sbEnterRows);
    sbEnterRows->setValue(DEF_ROWS);
    sbEnterRows->setWrapping(true);     // enables to spin "over" the limits
    if (SHOW_TOOLTIPS == true)
        QToolTip::add(sbEnterRows, tr("Choose or enter the number of\n"
                                      "rows for an empty layout"));

    rowsLabel->setBuddy(sbEnterRows);

    QHBoxLayout* buttonLayout = new QHBoxLayout(0, 0, 6);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonLayout->addItem(spacer);

    QPushButton* buttOK = new QPushButton(tr("OK"), this);
    buttonLayout->addWidget(buttOK);
    buttOK->setDefault(true);
    connect(buttOK, SIGNAL(clicked()), this, SLOT(accept()));

    QPushButton* buttCancel = new QPushButton(tr("Cancel"), this);
    buttonLayout->addWidget(buttCancel);
    connect(buttCancel, SIGNAL(clicked()), this, SLOT(reject()));

    newlayoutDlgLayout->addLayout(buttonLayout);
}


int newLayoutDialog::getColumns()
{
    return sbEnterCols->value();
}


int newLayoutDialog::getRows()
{
    return sbEnterRows->value();
}


void newLayoutDialog::setColumns(int cols)
{
    setCaption(tr("Change layout size"));
    sbEnterCols->setValue(cols);
}


void newLayoutDialog::setRows(int rows)
{
    sbEnterRows->setValue(rows);
}
