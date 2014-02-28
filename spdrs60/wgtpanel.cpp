/*
  Copyright (c) 2014 Guido Scholz <gscholz@users.sourceforge.net>

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

#include "wgtpanel.h"

#include "pixmaps/label-wgt.xpm"


WgtPanel::WgtPanel(QWidget* parent, SpdrItemClassId cid):
    ExternalSingleButtonPanel(parent, cid, kWgtClicked, "WGT")
{
    setName("WgtPanel");
    setupElementIcon();
}


WgtPanel::WgtPanel(QTextStream& ts, QWidget* parent, SpdrItemClassId cid):
    ExternalSingleButtonPanel(ts, parent, cid, kWgtClicked, "WGT")
{
    setName("WgtPanel");
    setupElementIcon();
}


void WgtPanel::setupElementIcon()
{
    background.fill(QColor(0, 0, 192));
    QPainter p(&background);

    int w = background.width();
    int h = background.height();

    // paint red light
    p.setBrush(Qt::red);
    p.drawEllipse(w / 2 - 2, h / 4 - 3, 4, 4);

    // paint button
    p.setBrush(Qt::darkGray);
    p.drawEllipse(w / 2 - 4, h / 2 - 4, 8, 8);

    // paint button label
    p.drawPixmap(18, 24, QPixmap(label_wgt_xpm));

    setPaletteBackgroundPixmap(background);
    addTooltip();
}

