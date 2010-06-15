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


#ifndef TURNTABLEPANEL_H
#define TURNTABLEPANEL_H

#include <qaction.h>

#include "graypanel.h"
#include "srcpmessage.h"
#include "turntablecommander.h"


class TurntablePanel: public GrayPanel
{
    Q_OBJECT

    int iSoldSubType;
    int baseaddress;
    int activetime;
    int bus;
    int xchangeport;
    QString sSoldText;
    QAction* variantaction;
    QAction* driveaction;
    turntableCommander* ttComm;

    void runPropertyMenue();
    void init();

  protected:
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void setupElementIcon();
    void addTooltip();

  public:
    TurntablePanel(QWidget* parent = NULL);
    TurntablePanel(QTextStream&, QWidget* parent = NULL);
    void writeFileTextToStream(QTextStream&);
    void readFileTextFromStream(QTextStream&);

  private slots:
    void slotUpdateTurntableData(int, int);
    void showVariantDialog();
    void showDriveDialog();
    void slotCopyAvailTracks(const QString&);

  signals:
    void sendSrcpMessage(SrcpMessage*);
};

#endif  //TURNTABLEPANEL_H
