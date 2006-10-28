/***************************************************************************
                           feedbackviewer.cpp
                           version 0.5.0 $Revision: 1.2 $
                           -------------------------------
    copyright            : (C) 2006 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2006-10-28 18:46:28 $
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
  This code creates a dockable window with up to four lists of feedback
  modules. 
 ***************************************************************************/

#include <qlayout.h>

#include "feedbackviewer.h"
#include "feedbacklistbox.h"
#include "preferences.h"


#if QT_VERSION >= 0x030100
FeedbackViewer::FeedbackViewer(QWidget* parent,
        const char* name): QDockWindow(parent, name)
#else
FeedbackViewer::FeedbackViewer(QWidget* parent,
        const char* name): QDockWindow(QDockWindow::InDock, parent, name)
#endif
{
    setResizeEnabled(true);
    setCaption(tr("Feedback Modules"));
    setCloseMode(Always);
    setFixedExtentWidth(160);

    fbTW = new QTabWidget(this, "feedbackTW");
    boxLayout()->addWidget(fbTW);

    // first tab, this is the visible minimum
    addTab1();

    // second tab only if neccesary
    if (pref.fbbus2.modules > 0)
        addTab2();
    else
        fbLB2 = NULL;

    // third tab only if neccesary
    if (pref.fbbus3.modules > 0)
        addTab3();
    else
        fbLB3 = NULL;

    // fourth tab only if neccesary
    if (pref.fbbus4.modules > 0)
        addTab4();
    else
        fbLB4 = NULL;
}


void FeedbackViewer::updateBusAndModuleStructure()
{
    // first module list
    fbTW->setTabLabel(fbLB1, tr("Bus &%1").arg(pref.fbbus1.number));
    fbLB1->updateModuleSetup(pref.fbfactor, pref.fbbus1.modules);

    // second module list
    // create missing listbox or remove obsolete listbox
    if (fbLB2 == NULL) {
        if (pref.fbbus2.modules > 0)
            addTab2();
    }
    else {
        if (pref.fbbus2.modules == 0)
            removeTab2();
        else {
            // adjust label and number of modules
            fbTW->setTabLabel(fbLB2, tr("Bus &%1").arg(pref.fbbus2.number));
            fbLB2->updateModuleSetup(pref.fbfactor, pref.fbbus2.modules);
        }
    }

    // third module list
    // create missing listbox or remove obsolete listbox
    if (fbLB3 == NULL) {
        if (pref.fbbus3.modules > 0)
            addTab3();
    }
    else {
        if (pref.fbbus3.modules == 0)
            removeTab3();
        else {
            // adjust label and number of modules
            fbTW->setTabLabel(fbLB3, tr("Bus &%1").arg(pref.fbbus3.number));
            fbLB3->updateModuleSetup(pref.fbfactor, pref.fbbus3.modules);
        }
    }

    // fourth module list
    // create missing listbox or remove obsolete listbox
    if (fbLB4 == NULL) {
        if (pref.fbbus4.modules > 0)
            addTab4();
    }
    else {
        if (pref.fbbus4.modules == 0)
            removeTab4();
        else {
            // adjust label and number of modules
            fbTW->setTabLabel(fbLB4, tr("Bus &%1").arg(pref.fbbus4.number));
            fbLB4->updateModuleSetup(pref.fbfactor, pref.fbbus4.modules);
        }
    }
}

void FeedbackViewer::addTab1()
{
    fbLB1 = new FeedbackListBox(fbTW, "fbmodlist1");
    fbLB1->updateModuleSetup(pref.fbfactor, pref.fbbus1.modules);
    fbTW->addTab(fbLB1, tr("Bus &%1").arg(pref.fbbus1.number));
}

void FeedbackViewer::addTab2()
{
    fbLB2 = new FeedbackListBox(fbTW, "fbmodlist2");
    fbLB2->updateModuleSetup(pref.fbfactor, pref.fbbus2.modules);
    fbTW->addTab(fbLB2, tr("Bus &%1").arg(pref.fbbus2.number));
}

void FeedbackViewer::removeTab2()
{
    if (fbLB2 != NULL) {
        fbTW->removePage(fbLB2);
        fbLB2->clear();
        delete fbLB2;
        fbLB2 = NULL;
    }
}

void FeedbackViewer::addTab3()
{
    fbLB3 = new FeedbackListBox(fbTW, "fbmodlist3");
    fbLB3->updateModuleSetup(pref.fbfactor, pref.fbbus3.modules);
    fbTW->addTab(fbLB3, tr("Bus &%1").arg(pref.fbbus3.number));
}

void FeedbackViewer::removeTab3()
{
    if (fbLB3 != NULL) {
        fbTW->removePage(fbLB3);
        fbLB3->clear();
        delete fbLB3;
        fbLB3 = NULL;
    }
}

void FeedbackViewer::addTab4()
{
    fbLB4 = new FeedbackListBox(fbTW, "fbmodlist4");
    fbLB4->updateModuleSetup(pref.fbfactor, pref.fbbus4.modules);
    fbTW->addTab(fbLB4, tr("Bus &%1").arg(pref.fbbus4.number));
}

void FeedbackViewer::removeTab4()
{
    if (fbLB4 != NULL) {
        fbTW->removePage(fbLB4);
        fbLB4->clear();
        delete fbLB4;
        fbLB4 = NULL;
    }
}

void FeedbackViewer::feedbackPortChanged(unsigned int bus,
        unsigned int contact, bool state)
{
    if (bus == pref.fbbus1.number && fbLB1 != NULL)
        fbLB1->updateFeedbackPortState(contact, state);
    else if (bus == pref.fbbus2.number && fbLB2 != NULL)
        fbLB2->updateFeedbackPortState(contact, state);
    else if (bus == pref.fbbus3.number && fbLB3 != NULL)
        fbLB3->updateFeedbackPortState(contact, state);
    else if (bus == pref.fbbus4.number && fbLB4 != NULL)
        fbLB4->updateFeedbackPortState(contact, state);
}


/*
void FeedbackViewer::processSrcpMessage(message)
{
}
stBoxItem * firstItem () const*/

