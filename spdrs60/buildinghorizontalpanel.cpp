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

#include "buildinghorizontalpanel.h"


BuildingHorizontalPanel::BuildingHorizontalPanel(QWidget* parent,
        SpdrItemClassId cid): GrayPanel(parent, cid)
{
    setupElementIcon();
}


BuildingHorizontalPanel::BuildingHorizontalPanel(QTextStream& ts,
        QWidget* parent,  SpdrItemClassId cid):
    GrayPanel(parent, cid)
{
    readFileTextFromStream(ts);
    setupElementIcon();
}


void BuildingHorizontalPanel::setupElementIcon()
{
    QPainter p(&background);

    int w = background.width();
    int h = background.height();

    // paint house
    p.setBrush(QColor(192, 0 ,0));
    p.drawRect(0, 9, w, 17);
    p.drawLine(0, h / 2, w, h / 2);
    setPaletteBackgroundPixmap(background);
    addTooltip();
}

