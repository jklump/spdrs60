/***************************************************************************
                           feedback.cpp
                           version 0.4.8 $Revision: 1.6 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                           (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2006-01-24 20:38:33 $
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
   this code shows a window with the feedback modules and port stati
 ******************************************************************************/
#include "feedback.h"
#include "preferences.h"

/*module pixmaps*/
#include "pixmaps/fb_nextpage.xpm"
#include "pixmaps/fb_prevpage.xpm"



feedback::feedback(QWidget* parent): QDialog(parent, "feedbackDlg", false)
{
    iPage = 0;                  // == s88-busnumber 1 on window-startup
    iMdCnt = 0;                 // no modules shown yet
    this->setCaption(tr("Overview over feedback modules"));

    lPageInfo = new QLabel("", this, "labelPageInfo");
    lPageInfo->setFont(QFont("*", 20, QFont::Bold));
    lPageInfo->setAlignment(AlignCenter);
    lPageInfo->setFrameStyle(QFrame::Box | QFrame::Raised);

    // create "next page" button
    QPixmap pix = QPixmap(fb_nextpage_xpm);
    buttNextPage = new QPushButton(tr("Next Page"), this, "");
    buttNextPage->setPixmap(pix);
    if (pref.tooltips)
        QToolTip::add(buttNextPage, tr("Show next page"));

    connect(buttNextPage, SIGNAL(clicked()), this, SLOT(slotNextPage()));

    // create "prev page" button
    pix = QPixmap(fb_prevpage_xpm);
    buttPrevPage = new QPushButton(tr("Prev Page"), this, "");
    buttPrevPage->setPixmap(pix);
    if (pref.tooltips)
        QToolTip::add(buttPrevPage, tr("Show next page"));

    connect(buttPrevPage, SIGNAL(clicked()), this, SLOT(slotPrevPage()));

    if (((pref.fbbus1.modules > 0) + (pref.fbbus2.modules > 0) +
         (pref.fbbus3.modules > 0) + (pref.fbbus4.modules > 0)) <= 1) {
        // disable page buttons if only one
        // bus has feedback modules connected
        buttNextPage->setEnabled(false);
        buttPrevPage->setEnabled(false);

        if (pref.fbbus1.modules != 0)
            iPage = 0;
        else if (pref.fbbus2.modules != 0)
            iPage = 1;
        else if (pref.fbbus3.modules != 0)
            iPage = 2;
        else if (pref.fbbus4.modules != 0)
            iPage = 3;
    }

    showModules();              // now display all modules
    buttNextPage->setGeometry(850, this->height() - 65, 40, 40);
    buttPrevPage->setGeometry(650, this->height() - 65, 40, 40);
}


void feedback::slotNextPage()
{
    // calculate new page number == new busnumber
    if (iPage < 3)
        ++iPage;
    else
        iPage = 0;

    if ((iPage == 0 && pref.fbbus1.modules == 0)
        || (iPage == 1 && pref.fbbus2.modules == 0)
        || (iPage == 2 && pref.fbbus3.modules == 0)
        || (iPage == 3 && pref.fbbus4.modules == 0))
        // skip next page if no modules present
        slotNextPage();
    else
        showModules();
}


void feedback::slotPrevPage()
{
    // calculate new page number == new busnumber
    if (iPage > 0)
        --iPage;
    else
        iPage = 3;

    if ((iPage == 0 && pref.fbbus1.modules == 0)
        || (iPage == 1 && pref.fbbus2.modules == 0)
        || (iPage == 2 && pref.fbbus3.modules == 0)
        || (iPage == 3 && pref.fbbus4.modules == 0))
        // skip prev page if no modules present
        slotPrevPage();
    else
        showModules();
}


void feedback::showModules()
{
    unsigned int mods = 0;
    if (iPage == 0)
        mods = pref.fbbus1.modules;
    else if (iPage == 1)
        mods = pref.fbbus2.modules;
    else if (iPage == 2)
        mods = pref.fbbus3.modules;
    else if (iPage == 3)
        mods = pref.fbbus4.modules;
    
    // delete any modules if new page is displayed
    if (iMdCnt != 0)
        for (int i = 0; i < iMdCnt; i++)
            delete module[i];

   // calculate right window size
    this->setFixedWidth(991 - pref.fbfactor * 10);
    this->setFixedHeight(470 + pref.fbfactor * 95);

    // show bus number
    QString sText;
    sText.sprintf("Bus #%1d", iPage + 1);
    lPageInfo->setGeometry(700, this->height() - 65, 140, 40);
    lPageInfo->setText(sText);

    // create and show modules
    int j = 0;
    for (unsigned int i = 0; i < mods; i++) {
        module[j] = new fbModule(this, i + iPage * 31 * (pref.fbfactor + 1));
        if (pref.fbfactor == 0)
            module[j]->move((j % 7) * 142, (j / 7) * 95);
        else
            module[j]->move((j % 12) * 82, (j / 12) * 95);

        connect(this, SIGNAL(updateModule(unsigned int, unsigned int,
                        unsigned int)),
                module[j], SLOT(slotSetupModule(unsigned int,
                        unsigned int, unsigned int)));
        module[j]->show();
        j++;
        iMdCnt = j;
    }
}


void feedback::slotUpdateModules(unsigned int bus, unsigned int contact,
        unsigned int state)
{
    unsigned int module, input;

    // number range: 0 - 30
    // input range: 1 - 16 or 1 - 8
    module = (contact - 1) / (16 - pref.fbfactor * 8); // + 1
    input = (contact - 1) % (16 - pref.fbfactor * 8) + 1;
    
    // send signals to __ALL__ modules, but only the one with equal
    // module ID will do the update in port colours
    // TODO: adjust number counting
    emit updateModule(module, input, state);
}
