/***************************************************************************
                           fbmodule.cpp
                           version 0.4.8 $Revision: 1.5 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                           (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2006-01-07 21:20:08 $
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
   this code shows all feedback modules with their actual port states
 ******************************************************************************/

#include <qapplication.h>
#include <qfont.h>
#include <qpainter.h>                       
#include <qpixmap.h>

#include "fbmodule.h"

/*module pixmaps*/
#include "pixmaps/fb_s44.xpm"
#include "pixmaps/fb_s88.xpm"

extern bool bFBport[MAX_FB];
extern int FEEDBACK;



fbModule::fbModule(QWidget* parent, unsigned int modid): QWidget(parent)
{
    // iModNr range: 0 - 30
    iModNr = modid;
    slotSetupModule(iModNr, 0, 0);
    // resize module due to module type
    this->resize(139 - FEEDBACK * 60, 90);
}


void fbModule::slotSetupModule(unsigned int module, unsigned int input,
        unsigned int state)
{
    if (module != iModNr)
        return;

    int i;
    QString s;
    QPixmap pixModule;

    // load bitmap according to number of ports per module if port
    // belongs to this module
    if (FEEDBACK == FB_16)
        pixModule = QPixmap(fb_s88_xpm);
    if (FEEDBACK == FB_8)
        pixModule = QPixmap(fb_s44_xpm);
    QPainter p;
    p.begin(&pixModule);

    QFont f("*");               // display module number
    f.setPointSize(QApplication::font().pointSize());
    f.setWeight(QFont::DemiBold);
    p.setFont(f);

    s.sprintf("%d", iModNr + 1);
    QRect br;
    QFontMetrics fm(f);
    br = fm.boundingRect(s.data());

    p.setPen(red);
    p.drawText(69 - FEEDBACK * 30 - br.width() / 2, 49, s.data());

    // display loco address if address fb module (beta)
    int iAdr = bFBport[iModNr * 8] + 2 * (bFBport[iModNr * 8 + 1]) +
        4 * (bFBport[iModNr * 8 + 2]) + 8 * (bFBport[iModNr * 8 + 3]) +
        16 * (bFBport[iModNr * 8 + 4]) + 32 * (bFBport[iModNr * 8 + 5]) +
        64 * (bFBport[iModNr * 8 + 6]) + 128 * (bFBport[iModNr * 8 + 7]);

    s.sprintf("%05d", iAdr);
    br = fm.boundingRect(s.data());
    p.setPen(blue);
    p.drawText(69 - FEEDBACK * 30 - br.width() / 2, 65, s.data());

    f.setPointSize(QApplication::font().pointSize() - 2);
    f.setWeight(QFont::Normal);
    p.setFont(f);
    p.setPen(black);

    // display port status for every single port
    for (i = 0; i < 8 - FEEDBACK * 4; i += 1) {
        s.sprintf("%2d", i + 1);
        p.drawText(115 - FEEDBACK * 60 - i * 15, 23, s.data());
        p.fillRect(118 - FEEDBACK * 60 - i * 15, 2, 8, 8,
                   QBrush(QColor
                          (bFBport[iModNr * (16 - FEEDBACK * 8) + i] ? red
                           : white), SolidPattern));
    }
    for (i = 8 - FEEDBACK * 4; i < 16 - FEEDBACK * 8; i += 1) {
        s.sprintf("%2d", i + 1);
        p.drawText(11 + (i - 8 + FEEDBACK * 4) * 15, 76, s.data());
        p.fillRect(13 + (i - 8 + FEEDBACK * 4) * 15, 80, 8, 8,
                   QBrush(QColor
                          (bFBport[iModNr * (16 - FEEDBACK * 8) + i] ? red
                           : white), SolidPattern));
    }

    p.end();                    // at least show module
    setBackgroundPixmap(pixModule);
}
