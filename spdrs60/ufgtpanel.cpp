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

#include "ufgtpanel.h"

#include "pixmaps/label-ufgt.xpm"
#include "pixmaps/label-mgt.xpm"


UfgtPanel::UfgtPanel(QWidget* parent, SpdrItemClassId cid):
    ExternalDualButtonPanel(parent, cid, kUfgtClicked, kMgtClicked,
            "UfGT", "MGT")
{
    setName("UfgtPanel");
    setupElementIcon();
}


UfgtPanel::UfgtPanel(QTextStream& ts, QWidget* parent, SpdrItemClassId cid):
    ExternalDualButtonPanel(ts, parent, cid, kUfgtClicked, kMgtClicked,
            "UfGT", "MGT")
{
    setName("UfgtPanel");
    setupElementIcon();
}


void UfgtPanel::setupElementIcon()
{
    background.fill(QColor(0, 160, 0));
    QPainter p(&background);

    int w = background.width();
    int h = background.height();

    // paint buttons
    p.setBrush(Qt::darkGray);
    p.drawEllipse(7, h / 2 - 4, 8, 8);
    p.drawEllipse(w - 16, h / 2 - 4, 8, 8);

    // paint button labels
    p.drawPixmap(2, 24, QPixmap(label_ufgt_xpm));
    p.drawPixmap(34, 24, QPixmap(label_mgt_xpm));

    setPaletteBackgroundPixmap(background);
    addTooltip();
}

