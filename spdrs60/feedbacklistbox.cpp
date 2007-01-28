/***************************************************************************
                           feedbacklistbox.cpp
                           version 0.5.1 $Revision: 1.2 $
                           -------------------------------
    copyright            : (C) 2006-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-01-28 16:25:42 $
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


#include "feedbacklistbox.h"


FeedbackListBox::FeedbackListBox(QWidget* parent,
        const char* name): QListBox(parent, name)
{
    setRowMode(FitToHeight);
    setSelectionMode(NoSelection);
    setVariableHeight(false);
    moduleType = FeedbackModule::fbm16;
}

void FeedbackListBox::updateModuleSetup(unsigned int mt, unsigned int mc)
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
            fbm = new FeedbackModule(this);
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


/*
void FeedbackListBox::processSrcpMessage(message)
{
}
stBoxItem * firstItem () const*/

