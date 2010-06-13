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
#include <qtooltip.h>

#include "spdrpanel.h"


SpdrPanel::SpdrPanel(QWidget* parent):
    QWidget(parent, "spdrpanel"),
    classid(siciNone),
    selectionMode(ksmNormal),
    visualMode(kvmEditLayout),
    iSoldIndex(0),
    modified(false)
{
    setFixedSize(QSize(EL_WIDTH, EL_HEIGHT));
    background = QPixmap(size());
}


void SpdrPanel::setIndexNo(unsigned int idx)
{
    iSoldIndex = idx;
}


unsigned int SpdrPanel::getIndexNo()
{
    return iSoldIndex;
}


elemSelectionMode SpdrPanel::getSelectionMode()
{
    return selectionMode;
}


void SpdrPanel::switchSelectionMode(elemSelectionMode sm)
{
    if (selectionMode != sm) {
        selectionMode = sm;
        update();
    }
}


void SpdrPanel::switchVisualMode(elemVisualMode vm)
{
    if (visualMode != vm) {
        visualMode = vm;

        if (selectionMode != ksmNormal)
            switchSelectionMode(ksmNormal);
    }
}


void SpdrPanel::setDropTargetView(bool on)
{
    if (on)
        switchSelectionMode(ksmDropTarget);
    else
        switchSelectionMode(ksmNormal);
}


bool SpdrPanel::isRoutable()
{
    return false;
}


bool SpdrPanel::isModified()
{
    return modified;
}


SpdrPanel::SpdrItemClassId SpdrPanel::classId()
{
    return classid;
}


void SpdrPanel::writeFileTextToStream(QTextStream& ts)
{
    ts << GF_CLASSID << DS << classid << endl
        << GF_INDEX << DS << iSoldIndex << endl
        << '%' << endl;
    modified = false;
}


void SpdrPanel::readFileTextFromStream(QTextStream& ats)
{
    QString s, key, value, oldname;

    while (!ats.eof()) {
        s = ats.readLine();
        if (!s.startsWith("#")) {
            key = s.section(DS, 0, 0);
            value = s.section(DS, 1, 1).stripWhiteSpace();

            /* key/value pairs are read sequence independent */
            if (key.compare(GF_INDEX) == 0) {
                  iSoldIndex = value.stripWhiteSpace().toUInt();
            }
            else if (key.startsWith("%")) {
                /*end of dataset, exit while loop*/
                  break;
            }
        }
    }
}

/*this draws only foreground lines on background pixmap*/
void SpdrPanel::paintEvent(QPaintEvent*)
{
    /* paint optional selection rectangle*/
    if (selectionMode != ksmNormal) {
        QPainter p(this);
        QColor c;

        switch (selectionMode) {
            case ksmStopSig:
                // red if in show route mode, stop signal
                c = QColor(Qt::red);
                break;
            case ksmStartSig:
                // green if in show route mode, start signal
                c = QColor(Qt::green);
                break;
            case ksmDisplay:
                // magenta if in show route mode, train number display
                c = QColor(Qt::magenta);
                break;
            case ksmSwitchEl:
                // yellow if clicked element in record route mode
                c = QColor(251, 251, 0);
                break;
            case ksmFoundEl:
                // found: orange
                c = QColor("DarkOrange");
                break;
            case ksmDropTarget:
                c = QColor(Qt::white);
                break;
            default:
                c = QColor(Qt::black);
                break;
        }

        int h = height();
        int w = width();

        p.setPen(QPen(c, 2, Qt::SolidLine));
        p.drawLine(0, h - 1, w, h - 1);
        p.drawLine(w - 1, h - 1, w - 1, 0);
        p.drawLine(w - 1, 1, 0, 0);
        p.drawLine(1, 1, 1, h - 1);
    }
}

/* 
 * update tooltip visability and
 * redraw backgroud pixmap with the opposite of text/address labels
 */
void SpdrPanel::slotRepaintLayout()
{
    QToolTip::remove(this);
    setupElementIcon();
}

