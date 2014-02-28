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

#include "einpanel.h"

#include "pixmaps/label-ein-on.xpm"
#include "pixmaps/label-ein-off.xpm"
#include "pixmaps/label-aus-on.xpm"
#include "pixmaps/label-aus-off.xpm"


EinPanel::EinPanel(QWidget* parent, SpdrItemClassId cid):
    ExternalDualButtonPanel(parent, cid, kEinClicked, kAusClicked,
            "Ein", "Aus"), TableLight(true)
{
    setName("EinPanel");
    setupElementIcon();
}


EinPanel::EinPanel(QTextStream& ts, QWidget* parent, SpdrItemClassId cid):
    ExternalDualButtonPanel(ts, parent, cid, kEinClicked, kAusClicked,
            "Ein", "Aus"), TableLight(true)
{
    setName("EinPanel");
    setupElementIcon();
}

/*implementation of abstract function*/
void EinPanel::setTableLight(bool ison)
{
    if (tablelight != ison) {
        tablelight = ison;
        setupElementIcon();
    }
}


void EinPanel::setupElementIcon()
{
    background.fill(Qt::darkGray);
    QPainter p(&background);

    int w = background.width();
    int h = background.height();

    // paint buttons
    p.setBrush(Qt::darkGray);
    p.drawEllipse(7, h / 2 - 4, 8, 8);
    p.drawEllipse(w - 16, h / 2 - 4, 8, 8);

    // paint button labels
    if (tablelight) {
        p.drawPixmap(2, 24, QPixmap(label_ein_on_xpm));
        p.drawPixmap(34, 24, QPixmap(label_aus_off_xpm));
    }
    else {
        p.drawPixmap(2, 24, QPixmap(label_ein_off_xpm));
        p.drawPixmap(34, 24, QPixmap(label_aus_on_xpm));
    }

    setPaletteBackgroundPixmap(background);
    addTooltip();
}

