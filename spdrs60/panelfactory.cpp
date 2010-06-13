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

#include "panelfactory.h"
#include "levelcrossinggrouppanel.h"
#include "powersupplygrouppanel.h"
#include "routegrouppanel.h"
#include "signalgrouppanel.h"
#include "turnoutgrouppanel.h"


PanelFactory::PanelFactory()
{
}

/*create new panels in edit mode*/
SpdrPanel* PanelFactory::createPanel(QWidget* parent,
        SpdrPanel::SpdrItemClassId type)
{
    switch (type) {
        case SpdrPanel::siciFeb:
            return new TurnoutGroupPanel(parent);
        case SpdrPanel::siciFee:
            return new PowerSupplyGroupPanel(parent);
        case SpdrPanel::siciFeg:
            return new RouteGroupPanel(parent);
        case SpdrPanel::siciFer:
            return new SignalGroupPanel(parent);
        case SpdrPanel::siciFey:
            return new LevelCrossingGroupPanel(parent);
        default:
            return new element(parent, type);
    }
}

/*create new panels by file stream in normal mode*/
SpdrPanel* PanelFactory::createPanelFromStream(QTextStream& ts,
        QWidget* parent, SpdrPanel::SpdrItemClassId type)
{
    switch (type) {
        case SpdrPanel::siciFeb:
            return new TurnoutGroupPanel(ts, parent);
        case SpdrPanel::siciFee:
            return new PowerSupplyGroupPanel(ts, parent);
        case SpdrPanel::siciFeg:
            return new RouteGroupPanel(ts, parent);
        case SpdrPanel::siciFer:
            return new SignalGroupPanel(ts, parent);
        case SpdrPanel::siciFey:
            return new LevelCrossingGroupPanel(ts, parent);
        default:
            return new element(ts, parent, type);
    }
}

