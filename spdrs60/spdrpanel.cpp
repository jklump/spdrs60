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

#include "spdrpanel.h"


SpdrPanel::SpdrPanel(QWidget* parent):
    QWidget(parent, "spdrpanel"),
    classid(siciNone),
    modified(false)
{
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

