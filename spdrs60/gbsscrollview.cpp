/***************************************************************************
                           gbsscrollview.cpp
                           version 0.5.1 $Revision: 1.5 $
                           -------------------------------
    copyright            : (C) 2004-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-02-13 19:45:11 $
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
   this code controls gbsarea key events
 ***************************************************************************/

#include "gbsscrollview.h"
GBSScrollView::GBSScrollView(QWidget* parent, const char* name,
                             Qt::WFlags f): QScrollView(parent, name, f)
{
    setFocusPolicy(QWidget::StrongFocus);
}


void GBSScrollView::keyPressEvent(QKeyEvent* e)
{
    switch (e->key()) {
        case Qt::Key_Down:
            setContentsPos(contentsX(), contentsY() + 10);
            break;
        case Qt::Key_Up:
            setContentsPos(contentsX(), contentsY() - 10);
            break;
        case Qt::Key_Left:
            setContentsPos(contentsX() - 10, contentsY());
            break;
        case Qt::Key_Right:
            setContentsPos(contentsX() + 10, contentsY());
            break;
        case Qt::Key_PageUp:
            setContentsPos(contentsX(), contentsY() - visibleHeight());
            break;
        case Qt::Key_PageDown:
            setContentsPos(contentsX(), contentsY() + visibleHeight());
            break;
        case Qt::Key_Home:
            setContentsPos(contentsX(), 0);
            break;
        case Qt::Key_End:
            setContentsPos(contentsX(), contentsHeight() - visibleHeight());
            break;
        default:
            QScrollView::keyPressEvent(e);
    }
}
