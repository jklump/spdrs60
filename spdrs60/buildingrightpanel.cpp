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

#include "buildingrightpanel.h"


BuildingRightPanel::BuildingRightPanel(QWidget* parent):
    GrayPanel(parent)
{
    classid = siciBur;
    setupElementIcon();
}


BuildingRightPanel::BuildingRightPanel(QTextStream& ts,
        QWidget* parent):
    GrayPanel(parent)
{
    classid = siciBur;
    readFileTextFromStream(ts);
    setupElementIcon();
}


void BuildingRightPanel::setupElementIcon()
{
    QPainter p(&background);

    // paint house
    p.setBrush(QColor(192, 0 ,0));
    p.drawRect(4, 5, 25, 25);
    p.drawLine(4, 5, 28, 29);
    p.drawLine(4, 29, 28, 5);
    p.drawRect(0, 9, 5, 17);
    setPaletteBackgroundPixmap(background);
    addTooltip();
}

