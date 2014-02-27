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


#ifndef EXTERNALBUTTONPANEL_H
#define EXTERNALBUTTONPANEL_H

#include "spdrpanel.h"
#include "gbsbuttonstate.h"


class ExternalButtonPanel: public SpdrPanel
{
    Q_OBJECT

    public:
        ExternalButtonPanel(QWidget* parent = NULL,
                SpdrItemClassId cid = siciNone,
                GbsButtonState cb = kNoneClicked,
                const char* bt = "Unknown");
        ExternalButtonPanel(QTextStream&, QWidget* parent = NULL,
                SpdrItemClassId cid = siciNone,
                GbsButtonState cb = kNoneClicked,
                const char* bt = "Unknown");

        void readFileTextFromStream(QTextStream&);
        void writeFileTextToStream(QTextStream&);
        virtual bool showFeedbackTriggerDialog(const QPoint&);

    protected:
        /*button 1 trigger*/
        bool     enable1fbtrigger;
        unsigned int button1fbbus;
        unsigned int button1fbcontact;

        GbsButtonState ctrlButton;
        QString buttontext;
        void runPropertyMenue(const QPoint&);
        void mousePressEvent(QMouseEvent*);
        void mouseReleaseEvent(QMouseEvent*);
        virtual void buttonTriggered();

    public slots:
        void slotOccupyElement(unsigned int, unsigned int, bool);

    signals:
        void buttonClicked(GbsButtonState);
        void sigShowFBmodules();
};

#endif  //EXTERNALBUTTONPANEL_H
