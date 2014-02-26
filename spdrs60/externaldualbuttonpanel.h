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


#ifndef EXTERNALDUALBUTTONPANEL_H
#define EXTERNALDUALBUTTONPANEL_H

#include "externalbuttonpanel.h"


class ExternalDualButtonPanel: public ExternalButtonPanel
{
    Q_OBJECT

    public:
        ExternalDualButtonPanel(QWidget* parent = NULL,
                SpdrItemClassId cid = siciNone,
                GbsButtonState cb = kNoneClicked,
                GbsButtonState cb2 = kNoneClicked,
                const char* bt = "Unknown",
                const char* bt2 = "Unknown");
        ExternalDualButtonPanel(QTextStream&, QWidget* parent = NULL,
                SpdrItemClassId cid = siciNone,
                GbsButtonState cb = kNoneClicked,
                GbsButtonState cb2 = kNoneClicked,
                const char* bt = "Unknown",
                const char* bt2 = "Unknown");

        void readFileTextFromStream(QTextStream&);
        void writeFileTextToStream(QTextStream&);
        bool showFeedbackTriggerDialog(const QPoint&);

    protected:
        /*button 2 trigger*/
        bool     enable2fbtrigger;
        unsigned int button2fbbus;
        unsigned int button2fbcontact;

        GbsButtonState ctrlButton2;
        QString buttontext2;
        void mousePressEvent(QMouseEvent*);
        virtual void button2Triggered();

    public slots:
        void slotOccupyElement(unsigned int, unsigned int, bool);
};

#endif  //EXTERNALDUALBUTTONPANEL_H
