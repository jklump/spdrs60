/***************************************************************************
                           gbsscrollview.cpp
                           version 0.5.0 $Revision: 1.3 $
                           -------------------------------
    copyright            : (C) 2004-2006 by Guido Scholz
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
 ***************************************************************************/
/***************************************************************************
   this code controls gbsarea key events
 ***************************************************************************/

#include "gbsscrollview.h"
GBSScrollView::GBSScrollView(QWidget* parent, const char* name,
                             WFlags f): QScrollView(parent, name, f)
{
    setFocusPolicy(QWidget::StrongFocus);
}


void GBSScrollView::keyPressEvent(QKeyEvent* e)
{
    switch (e->key()) {
        case Key_Down:
            setContentsPos(contentsX(), contentsY() + 10);
            break;
        case Key_Up:
            setContentsPos(contentsX(), contentsY() - 10);
            //e->accept();
            break;
        case Key_Left:
            setContentsPos(contentsX() - 10, contentsY());
            //e->accept();
            break;
        case Key_Right:
            setContentsPos(contentsX() + 10, contentsY());
            //e->accept();
            break;
        case Key_PageUp:
            setContentsPos(contentsX(), contentsY() - visibleHeight());
            //e->accept();
            break;
        case Key_PageDown:
            setContentsPos(contentsX(), contentsY() + visibleHeight());
            //e->accept();
            break;
        case Key_Home:
            setContentsPos(contentsX(), 0);
            //e->accept();
            break;
        case Key_End:
            setContentsPos(contentsX(), contentsHeight() - visibleHeight());
            //e->accept();
            break;
        default:
            QScrollView::keyPressEvent(e);
    }
}
