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

#include "bufferstopbottompanel.h"


BufferStopBottomPanel::BufferStopBottomPanel(QWidget* parent):
    GrayPanel(parent)
{
    classid = siciBs4;
    setupElementIcon();
}


BufferStopBottomPanel::BufferStopBottomPanel(QTextStream& ts, QWidget* parent):
    GrayPanel(parent)
{
    classid = siciBs4;
    readFileTextFromStream(ts);
    setupElementIcon();
}


void BufferStopBottomPanel::setupElementIcon()
{
    QPainter p(&background);

    int w = background.width();
    int h = background.height();

    p.fillRect(w / 2 - 6, h - 5, 13, 5, QBrush(Qt::black));
    setPaletteBackgroundPixmap(background);
    addTooltip();
}

