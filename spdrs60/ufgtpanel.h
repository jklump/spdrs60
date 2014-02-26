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


#ifndef UFGTPANEL_H
#define UFGTPANEL_H

#include "externaldualbuttonpanel.h"


class UfgtPanel: public ExternalDualButtonPanel
{
    Q_OBJECT

    public:
        UfgtPanel(QWidget* parent = NULL, SpdrItemClassId cid = siciTau);
        UfgtPanel(QTextStream&, QWidget* parent = NULL, SpdrItemClassId
                cid = siciTau);

    protected:
        void setupElementIcon();
};

#endif  //UFGTPANEL_H
