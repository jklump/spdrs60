/***************************************************************************
                           feedback.cpp
                           version 0.4.8 $Revision: 1.4 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                           (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-12-01 20:37:04 $
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

/*module pixmaps*/
#include "pixmaps/fb_nextpage.xpm"
#include "pixmaps/fb_prevpage.xpm"

extern int FB_MODULES_[4];
extern int FEEDBACK;
extern int SHOW_TOOLTIPS;


feedback::feedback(QWidget* parent): QDialog(parent, "feedback", false)
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
    if (SHOW_TOOLTIPS)
        QToolTip::add(buttNextPage, tr("Show next page"));

    connect(buttNextPage, SIGNAL(clicked()), this, SLOT(slotNextPage()));

    // create "prev page" button
    pix = QPixmap(fb_prevpage_xpm);
    buttPrevPage = new QPushButton(tr("Prev Page"), this, "");
    buttPrevPage->setPixmap(pix);
    if (SHOW_TOOLTIPS)
        QToolTip::add(buttPrevPage, tr("Show next page"));

    connect(buttPrevPage, SIGNAL(clicked()), this, SLOT(slotPrevPage()));

    if (((FB_MODULES_[0] > 0) + (FB_MODULES_[1] > 0) +
         (FB_MODULES_[2] > 0) + (FB_MODULES_[3] > 0)) <= 1) {
        buttNextPage->setEnabled(false);  // disable page buttons if only one
        buttPrevPage->setEnabled(false);  // bus has feedback modules connected
        for (int i = 0; i < 4; i++) {
            if (FB_MODULES_[i] != 0)
                iPage = i;
        }
    }

    showModules();              // now display all modules
    buttNextPage->setGeometry(850, this->height() - 65, 40, 40);
    buttPrevPage->setGeometry(650, this->height() - 65, 40, 40);
}


void feedback::slotNextPage()
{
    if (iPage < 3)              // calculate new page number == new busnumber
        iPage += 1;
    else
        iPage = 0;
    if (FB_MODULES_[iPage] == 0)
        slotNextPage();         // skip next page if no modules present
    else
        showModules();
}


void feedback::slotPrevPage()
{
    if (iPage > 0)              // calculate new page number == new busnumber
        iPage -= 1;
    else
        iPage = 3;
    if (FB_MODULES_[iPage] == 0)
        slotPrevPage();         // skip prev page if no modules present
    else
        showModules();
}


void feedback::showModules()
{
    // delete any modules if new page is displayed
    if (iMdCnt != 0)
        for (int i = 0; i < iMdCnt; i++)
            delete module[i];

   // calculate right window size
    this->setFixedWidth(991 - FEEDBACK * 10);
    this->setFixedHeight(470 + FEEDBACK * 95);

    // show bus number
    QString sText;
    sText.sprintf("Bus #%1d", iPage + 1);
    lPageInfo->setGeometry(700, this->height() - 65, 140, 40);
    lPageInfo->setText(sText);

    // create and show modules
    int j = 0;
    for (unsigned int i = iPage * 31 * (FEEDBACK + 1);
         i < iPage * 31 * (FEEDBACK + 1) + FB_MODULES_[iPage]; i++) {
        module[j] = new fbModule(this, i);
        if (FEEDBACK == FB_16)
            module[j]->move((j % 7) * 142, (j / 7) * 95);
        if (FEEDBACK == FB_8)
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
    module = (contact - 1) / (16 - FEEDBACK * 8); // + 1
    input = (contact - 1) % (16 - FEEDBACK * 8) + 1;
    
    // send signals to __ALL__ modules, but only the one with equal
    // module ID will do the update in port colours
    // TODO: adjust number counting
    emit updateModule(module, input, state);
}
