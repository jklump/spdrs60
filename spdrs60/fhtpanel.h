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


#ifndef FHTPANEL_H
#define FHTPANEL_H

#include "externalcounterpanel.h"


class FhtPanel: public ExternalCounterPanel
{
    Q_OBJECT

    public:
        FhtPanel(QWidget* parent = NULL, SpdrItemClassId cid = siciTaf);
        FhtPanel(QTextStream&, QWidget* parent = NULL, SpdrItemClassId
                cid = siciTaf);

    protected:
        void setupElementIcon();

//    public slots:
//        void slotRepaintLayout();
};

#endif  //FHTPANEL_H
