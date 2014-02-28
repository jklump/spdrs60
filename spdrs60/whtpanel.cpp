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

#include <qapplication.h>
#include <qpainter.h>

#include "whtpanel.h"

#include "pixmaps/label-wht.xpm"


WhtPanel::WhtPanel(QWidget* parent, SpdrItemClassId cid):
    ExternalCounterPanel(parent, cid, kWhtClicked, "WHT")
{
    setName("WhtPanel");
    setupElementIcon();
}


WhtPanel::WhtPanel(QTextStream& ts, QWidget* parent, SpdrItemClassId cid):
    ExternalCounterPanel(ts, parent, cid, kWhtClicked, "WHT")
{
    setName("WhtPanel");
    setupElementIcon();
}


void WhtPanel::setupElementIcon()
{
    QString countertext;
    background.fill(QColor(0, 0, 192));
    QPainter p(&background);

    int w = background.width();
    int h = background.height();

    // paint button
    p.setBrush(Qt::darkGray);
    p.drawEllipse(7, h / 2 - 4, 8, 8);

    // paint button label
    p.drawPixmap(2, 24, QPixmap(label_wht_xpm));

    // paint counter
    p.setBrush(Qt::white);
    p.drawRect(w / 2 , h / 2 - 5, 24, 11);
    countertext.sprintf("%04d", countervalue);
    QFont f(QApplication::font());
    f.setPointSize(7);
    p.setFont(f);
    QRect br = p.fontMetrics().boundingRect(countertext);
    br.moveTopRight(QPoint(w / 2 + 23, h / 2 - 4));
    p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
            Qt::DontClip, countertext);

    setPaletteBackgroundPixmap(background);
    addTooltip();
}

