/*
  Copyright (c) 2010 Guido Scholz <gscholz@users.sourceforge.net>

  This file is part of spdrs60.

  spdrs60 is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License version 2,
  or (at your option) any later version as published by the Free
  Software Foundation.

  spdrs60 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with spdrs60.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <qpainter.h>

#include "locoshedeastpanel.h"


LocoShedEastPanel::LocoShedEastPanel(QWidget* parent):
    GrayPanel(parent)
{
    classid = siciLs1;
    setupElementIcon();
}


LocoShedEastPanel::LocoShedEastPanel(QTextStream& ts,
        QWidget* parent):
    GrayPanel(parent)
{
    classid = siciLs1;
    readFileTextFromStream(ts);
    setupElementIcon();
}


void LocoShedEastPanel::setupElementIcon()
{
    QPainter p(&background);

    int w = background.width();
    int h = background.height();

    // translate origin to center of pixmap
    p.translate(w / 2, h / 2);

    int tracklen = w / 2;
    int shedwidth = h;
    int shedxoffset = 0;
    int shedyoffset = 0;

    // paint track
    p.fillRect(0, -3, -tracklen, 7, QBrush(Qt::black));

    // paint schuppen
    p.setBrush(QColor(192, 0 ,0));
    p.drawRect(0 - shedxoffset, -shedwidth / 2 + shedyoffset,
            w / 2, shedwidth);
    p.drawLine(w / 4 - shedxoffset, -shedwidth / 2 + shedyoffset,
            w / 4 - shedxoffset, shedwidth / 2 + shedyoffset);

    setPaletteBackgroundPixmap(background);
    addTooltip();
}

