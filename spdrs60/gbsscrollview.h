/***************************************************************************
                           gbsscrollview.h
                           version 0.5.2 $Revision: 1.8 $
                           -------------------------------
    copyright            : (C) 2004-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-11-23 21:05:25 $
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
   header file for gbsscrollview.cpp
 ***************************************************************************/

#ifndef GBSSCROLLVIEW_H
#define GBSSCROLLVIEW_H

#include <qscrollview.h>

class GBSScrollView: public QScrollView
{
    Q_OBJECT
public:
    GBSScrollView(QWidget* parent=0, const char* name=0, Qt::WFlags f=0);

protected:
    void keyPressEvent(QKeyEvent *e);
};
#endif // GBSSCROLLVIEW_H

