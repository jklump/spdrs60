/***************************************************************************
                           gbsscrollview.h
                           version 0.4.3
                           -------------------------------
    copyright            : (C) 2004 by Guido Scholz
    email                : guido.scholz@ bayernline.de
    last modified        : 27.08.2004
***************************************************************************/
/**/

/*****************************************************************************
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by     *
 *   the Free Software Foundation; either version 2 of the License, or        *
 *   (at your option) any later version.                                      *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
   this code shows a window where user enters an address to be searched for
 ******************************************************************************/

#ifndef GBSSCROLLVIEW_H
#define GBSSCROLLVIEW_H
#ifndef QT_H
#include "qscrollview.h"
#endif // QT_H
class GBSScrollView: public QScrollView
{
    Q_OBJECT
public:
    GBSScrollView(QWidget *parent=0, const char *name=0, WFlags f=0);
private:
    void keyPressEvent(QKeyEvent *e);
};
#endif // GBSSCROLLVIEW_H

