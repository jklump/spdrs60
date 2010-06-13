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

#include "bufferstopleftpanel.h"


BufferStopLeftPanel::BufferStopLeftPanel(QWidget* parent):
    SpdrPanel(parent)
{
    classid = siciBs3;
    setupElementIcon();
}


BufferStopLeftPanel::BufferStopLeftPanel(QTextStream& ts, QWidget* parent):
    SpdrPanel(parent)
{
    classid = siciBs3;
    readFileTextFromStream(ts);
    setupElementIcon();
}


void BufferStopLeftPanel::setupElementIcon()
{
    background.fill(Qt::lightGray);
    QPainter p(&background);

    int h = background.height();

    p.fillRect(0, h / 2 - 6, 5, 13, QBrush(Qt::black));
    setPaletteBackgroundPixmap(background);
    addTooltip();
}

