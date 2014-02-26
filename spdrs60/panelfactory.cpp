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
#include "axlecountergrouppanel.h"
#include "bufferstopbottompanel.h"
#include "bufferstopleftpanel.h"
#include "bufferstoprightpanel.h"
#include "bufferstoptoppanel.h"
#include "buildinghorizontalpanel.h"
#include "buildingleftpanel.h"
#include "buildingrightpanel.h"
#include "fhtpanel.h"
#include "levelcrossinggrouppanel.h"
#include "locoshedeastpanel.h"
#include "locoshedwestpanel.h"
#include "locoshednepanel.h"
#include "locoshednwpanel.h"
#include "locoshedsepanel.h"
#include "locoshedswpanel.h"
#include "powersupplygrouppanel.h"
#include "routegrouppanel.h"
#include "signalgrouppanel.h"
#include "transfertablepanel.h"
#include "turnoutgrouppanel.h"
#include "turntablepanel.h"
#include "wgtpanel.h"
#include "whtpanel.h"


PanelFactory::PanelFactory()
{
}

/*create new panels in edit mode*/
SpdrPanel* PanelFactory::createPanel(QWidget* parent,
        SpdrPanel::SpdrItemClassId type)
{
    switch (type) {
        case SpdrPanel::siciBs1:
            return new BufferStopRightPanel(parent, type);
        case SpdrPanel::siciBs2:
            return new BufferStopTopPanel(parent, type);
        case SpdrPanel::siciBs3:
            return new BufferStopLeftPanel(parent, type);
        case SpdrPanel::siciBs4:
            return new BufferStopBottomPanel(parent, type);
        case SpdrPanel::siciBuc:
            return new BuildingHorizontalPanel(parent, type);
        case SpdrPanel::siciBul:
            return new BuildingLeftPanel(parent, type);
        case SpdrPanel::siciBur:
            return new BuildingRightPanel(parent, type);
        case SpdrPanel::siciLs1:
            return new LocoShedEastPanel(parent, type);
        case SpdrPanel::siciLs3:
            return new LocoShedWestPanel(parent, type);
        case SpdrPanel::siciLt1:
            return new LocoShedNEPanel(parent, type);
        case SpdrPanel::siciLt3:
            return new LocoShedNWPanel(parent, type);
        case SpdrPanel::siciLb1:
            return new LocoShedSEPanel(parent, type);
        case SpdrPanel::siciLb3:
            return new LocoShedSWPanel(parent, type);

        case SpdrPanel::siciFeb:
            return new TurnoutGroupPanel(parent, type);
        case SpdrPanel::siciFee:
            return new PowerSupplyGroupPanel(parent, type);
        case SpdrPanel::siciFeg:
            return new RouteGroupPanel(parent, type);
        case SpdrPanel::siciFen:
            return new AxleCounterGroupPanel(parent, type);
        case SpdrPanel::siciFer:
            return new SignalGroupPanel(parent, type);
        case SpdrPanel::siciFey:
            return new LevelCrossingGroupPanel(parent, type);

        case SpdrPanel::siciDre:
            return new TurntablePanel(parent, type);
        case SpdrPanel::siciSbn:
            return new TransferTablePanel(parent, type);
        case SpdrPanel::siciTaw:
            return new WgtPanel(parent, type);
        case SpdrPanel::siciTwh:
            return new WhtPanel(parent, type);
        case SpdrPanel::siciTaf:
            return new FhtPanel(parent, type);

        default:
            return new element(parent, type);
    }
}

/*create new panels by file stream in normal mode*/
SpdrPanel* PanelFactory::createPanelFromStream(QTextStream& ts,
        QWidget* parent, SpdrPanel::SpdrItemClassId type)
{
    switch (type) {
        case SpdrPanel::siciBs1:
            return new BufferStopRightPanel(ts, parent, type);
        case SpdrPanel::siciBs2:
            return new BufferStopTopPanel(ts, parent, type);
        case SpdrPanel::siciBs3:
            return new BufferStopLeftPanel(ts, parent, type);
        case SpdrPanel::siciBs4:
            return new BufferStopBottomPanel(ts, parent, type);
        case SpdrPanel::siciBuc:
            return new BuildingHorizontalPanel(ts, parent, type);
        case SpdrPanel::siciBul:
            return new BuildingLeftPanel(ts, parent, type);
        case SpdrPanel::siciBur:
            return new BuildingRightPanel(ts, parent, type);
        case SpdrPanel::siciLs1:
            return new LocoShedEastPanel(ts, parent, type);
        case SpdrPanel::siciLs3:
            return new LocoShedWestPanel(ts, parent, type);
        case SpdrPanel::siciLt1:
            return new LocoShedNEPanel(ts, parent, type);
        case SpdrPanel::siciLt3:
            return new LocoShedNWPanel(ts, parent, type);
        case SpdrPanel::siciLb1:
            return new LocoShedSEPanel(ts, parent, type);
        case SpdrPanel::siciLb3:
            return new LocoShedSWPanel(ts, parent, type);

        case SpdrPanel::siciFeb:
            return new TurnoutGroupPanel(ts, parent, type);
        case SpdrPanel::siciFee:
            return new PowerSupplyGroupPanel(ts, parent, type);
        case SpdrPanel::siciFeg:
            return new RouteGroupPanel(ts, parent, type);
        case SpdrPanel::siciFen:
            return new AxleCounterGroupPanel(ts, parent, type);
        case SpdrPanel::siciFer:
            return new SignalGroupPanel(ts, parent, type);
        case SpdrPanel::siciFey:
            return new LevelCrossingGroupPanel(ts, parent, type);

        case SpdrPanel::siciDre:
            return new TurntablePanel(ts, parent, type);
        case SpdrPanel::siciSbn:
            return new TransferTablePanel(ts, parent, type);
        case SpdrPanel::siciTaw:
            return new WgtPanel(ts, parent, type);
        case SpdrPanel::siciTwh:
            return new WhtPanel(ts, parent, type);
        case SpdrPanel::siciTaf:
            return new FhtPanel(ts, parent, type);

        default:
            return new element(ts, parent, type);
    }
}

