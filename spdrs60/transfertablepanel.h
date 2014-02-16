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


#ifndef TRANSFERTABLEPANEL_H
#define TRANSFERTABLEPANEL_H

#include <qaction.h>

#include "graypanel.h"
#include "srcpmessage.h"
#include "elementcommander.h"


class TransferTablePanel: public GrayPanel
{
    Q_OBJECT

    SrcpMessage::Protocol protocol;
    int baseaddress;
    int activetime;
    int bus;
    int xchangeport;
    QChar state;
    QAction* driveaction;
    elementCommander* commander;

    void runPropertyMenue();
    void init();

  protected:
    void setupElementIcon();
    void addTooltip();
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

  public:
    TransferTablePanel(QWidget* parent = NULL,
            SpdrItemClassId cid = siciSbn);
    TransferTablePanel(QTextStream&, QWidget* parent = NULL,
            SpdrItemClassId cid = siciSbn);
    void writeFileTextToStream(QTextStream&);
    void readFileTextFromStream(QTextStream&);

  private slots:
    void slotUpdateCommanderData(int, int);
    void showDriveDialog();

  signals:
    void sendSrcpMessage(SrcpMessage*);
};

#endif  //TRANSFERTABLEPANEL_H
