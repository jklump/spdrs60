/***************************************************************************
                           feedbacklistbox.cpp
                           version 0.5.2 $Revision: 1.5 $
                           -------------------------------
    copyright            : (C) 2006-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2009-03-11 19:36:38 $
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
   This code handles a list of feedback modules 
 ***************************************************************************/

#include <qapplication.h>
#include <qdragobject.h>

#include "feedbacklistbox.h"
#include "resources.h"

#include "pixmaps/feedback_red.xpm"
#include "pixmaps/feedback_white.xpm"


FeedbackListBox::FeedbackListBox(QWidget* parent,
        const char* name, unsigned int b): QListBox(parent, name),
    bus(b), mousePressed(false)
{
    setRowMode(FitToHeight);
    setSelectionMode(NoSelection);
    setVariableHeight(false);
    moduleType = FeedbackModule::fbm16;
}

void FeedbackListBox::updateModuleSetup(int mt, unsigned int mc)
{
    if (mt != moduleType) {
        moduleType = (FeedbackModule::ModuleType) mt;
        updateModulesType();
        triggerUpdate(true);
    }

    if (count() != mc)
        updateModuleNumber(mc);
}

void FeedbackListBox::updateModulesType()
{
    FeedbackModule* fbm;
    FeedbackModule* nextfbm;

    fbm = (FeedbackModule*) firstItem();
    while (fbm != NULL) {
        /*if matching module is found repaint module and exit loop*/
        fbm->setModuleType(moduleType);
        updateItem(fbm);
        nextfbm = (FeedbackModule*) fbm->next();
        fbm = nextfbm;
    }
}

void FeedbackListBox::updateModuleNumber(unsigned int mc)
{
    int delta = 0;
    int i = 0;
    FeedbackModule* fbm;
    
    if (count() > mc) {
        // delete some modules
        delta = count() - mc;
        for (i = 0; i < delta; i++) {
            removeItem(count() - 1);
        }
    }
    else if (count() < mc) {
        // add some modules
        delta = mc - count();
        for (i = 0; i < delta; i++) {
            fbm = new FeedbackModule(this, bus);
            fbm->setId(count());
            fbm->setModuleType(moduleType);
        }
    }
}

void FeedbackListBox::updateFeedbackPortState(unsigned int contact, bool state)
{
    FeedbackModule* fbm;
    FeedbackModule* nextfbm;

    fbm = (FeedbackModule*) firstItem();
    while (fbm != NULL) {
        /*if matching module is found repaint module and exit loop*/
        if (fbm->changeFeedbackState(contact, state)) {
            updateItem(fbm);
            break;
        }
        nextfbm = (FeedbackModule*) fbm->next();
        fbm = nextfbm;
    }
}


void FeedbackListBox::contentsMousePressEvent(QMouseEvent* e)
{
    QListBox::contentsMousePressEvent(e);
    QPoint p(contentsToViewport(e->pos()));
    QListBoxItem* i = itemAt(p);
    if (i != NULL) {
        QRect r = itemRect(i);
        if (dynamic_cast<FeedbackModule*>(i)->isContactPosition(
                    QPoint(p.x() - r.x(), p.y() - r.y()))) {
            presspos = e->pos();
            mousePressed = true;
        }
    }
}


void FeedbackListBox::contentsMouseReleaseEvent(QMouseEvent*)
{
    mousePressed = false;
}


void FeedbackListBox::contentsMouseMoveEvent(QMouseEvent* e)
{
    QByteArray data;
    bool occupied = false;

    QPoint p(contentsToViewport(e->pos())); //presspos?
    if (mousePressed && (presspos - e->pos()).manhattanLength()
            > QApplication::startDragDistance()) {
        mousePressed = false;
        QListBoxItem* i = itemAt(p);
        if (i != NULL) {
            occupied = dynamic_cast<FeedbackModule*>(i)->getContactData(data);
            if (data.size() > 0) {
                QStoredDrag* d = new QStoredDrag(MIME_FBC, this, "spdrs60-fbc");
                d->setEncodedData(data);
                if (occupied)
                    d->setPixmap(QPixmap(feedback_red_xpm));
                else
                    d->setPixmap(QPixmap(feedback_white_xpm));
                d->dragMove();
            }
        }
    }
}

