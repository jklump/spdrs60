/***************************************************************************
                           newLayoutDialog.cpp
                           version 0.4.3
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
    email                : stefan.preis@wdr.de
    last modified        : 2004-12-31
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
   this code provides an user interface to enter the number of new columns

 ******************************************************************************/
#include "newlayoutdialog.h"

extern bool SHOW_TOOLTIPS;
extern int DEF_COLS;


newLayoutDialog::newLayoutDialog(QWidget * parent)
:  QDialog(0, "newLayoutDialog", true)
{                               // true = parent window is locked
    if (parent);                // dummy command avoids compiler warning
    iNumberOfColumns = DEF_COLS;        // pre-save the new columns value

    QFrame *frame = new QFrame(this);   // paint a frame around text and spinbox
    frame->setGeometry(10, 10, 200, 100);
    frame->setFrameStyle(QFrame::Box | QFrame::Sunken);

    QLabel *label =
        new QLabel(tr("Number of columns\nfor a new layout:"), frame);
    label->resize(150, 50);
    label->move(10, 10);

    QSpinBox *sbEnterCols = new QSpinBox(5, MAX_COLS, 1, frame, "");
    sbEnterCols->resize(50, 25);
    sbEnterCols->move(frame->width() / 2 - sbEnterCols->width() / 2,
                      label->y() + label->height() + 5);
    sbEnterCols->setValue(DEF_COLS);
    sbEnterCols->setFocus();
    sbEnterCols->setWrapping(true);     // enables to spin "over" the limits
    if (SHOW_TOOLTIPS == true)
        QToolTip::add(sbEnterCols, tr("Choose or enter the number of\n"
                                      "columns for an empty layout"));
    connect(sbEnterCols, SIGNAL(valueChanged(int)),
            this, SLOT(slotSaveValue(int)));

    QPushButton *buttOK = new QPushButton(tr("&OK"), this);
    buttOK->move(10, frame->y() + frame->height() + 10);
    buttOK->setDefault(true);
    connect(buttOK, SIGNAL(clicked()), this, SLOT(accept()));

    QPushButton *buttCancel = new QPushButton(tr("&Cancel"), this);
    buttCancel->move(10 + frame->width() - buttCancel->width(),
                     frame->y() + frame->height() + 10);
    connect(buttCancel, SIGNAL(clicked()), this, SLOT(reject()));

    this->setFixedWidth(frame->width() + 2 * frame->x());
    this->setFixedHeight(buttOK->y() + buttOK->height() + 10);
}


void newLayoutDialog::slotSaveValue(int iNewValue)
{
    iNumberOfColumns = iNewValue;       // save the spinbox value
}
