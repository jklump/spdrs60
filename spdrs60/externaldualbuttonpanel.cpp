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

#include <qcursor.h>

#include "externaldualbuttonpanel.h"
#include "feedbacktriggerdialog.h"


ExternalDualButtonPanel::ExternalDualButtonPanel(QWidget* parent,
        SpdrItemClassId cid, GbsButtonState cb, GbsButtonState cb2,
        const char* bt, const char* bt2):
    ExternalButtonPanel(parent, cid, cb, bt),
    enable2fbtrigger(false),
    button2fbbus(1),
    button2fbcontact(1),
    ctrlButton2(cb2),
    buttontext2(bt2)
{
    setName("ExternalDualButtonPanel");
}


ExternalDualButtonPanel::ExternalDualButtonPanel(QTextStream& ts, QWidget*
        parent, SpdrItemClassId cid, GbsButtonState cb,
        GbsButtonState cb2, const char* bt, const char* bt2):
    ExternalButtonPanel(ts, parent, cid, cb, bt),
    enable2fbtrigger(false),
    button2fbbus(1),
    button2fbcontact(1),
    ctrlButton2(cb2),
    buttontext2(bt2)
{
    setName("ExternalDualButtonPanel");
    readFileTextFromStream(ts);
}


void ExternalDualButtonPanel::button2Triggered()
{
    emit buttonClicked(ctrlButton2);
}

/**
 * respond to mouse press events
 */
void ExternalDualButtonPanel::mousePressEvent(QMouseEvent* e)
{
    /*normal mode*/
    if (visualMode == kvmNormal) {
        if (e->button() == Qt::LeftButton) {
            QPoint CursorPos = mapFromGlobal(QCursor::pos());

            if (CursorPos.x() < (width() >> 1))
                /*left button*/
                buttonTriggered();
            else
                /*right button*/
                button2Triggered();
            e->accept();
        }
        else
            e->ignore();
    }
    else
        e->ignore();
}

/**
 * This slot is always called if a feedback port toggles.
 */
void ExternalDualButtonPanel::slotOccupyElement(unsigned int bus,
        unsigned int contact, bool ostate)
{
    /*shortcut if (occupation state = 0) => button release message*/
    if (!ostate)
        return;

    /*button 1 trigger only on*/
    if (enable2fbtrigger) {
        if ((bus == button2fbbus) && (contact == button2fbcontact))
            button2Triggered();
    }

    ExternalButtonPanel::slotOccupyElement(bus, contact, ostate);
}

/* Read the layout item data from stream.
 * Lines starting with # are recognized as comments, lines starting
 * with % are recognized as end of dataset (new style file format)*/
void ExternalDualButtonPanel::readFileTextFromStream(QTextStream& ats)
{
    QString s, key, value;

    while (!ats.atEnd()) {
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
                  return;
            }
            else if (key.compare(GF_BUTTON1FB) == 0) {
                enable1fbtrigger = value.toUInt();
                value = s.section(DS, 2, 2).stripWhiteSpace();
                button1fbbus = value.toUInt();
                value = s.section(DS, 3, 3).stripWhiteSpace();
                button1fbcontact = value.toUInt();
            }
            else if (key.compare(GF_BUTTON2FB) == 0) {
                enable2fbtrigger = value.toUInt();
                value = s.section(DS, 2, 2).stripWhiteSpace();
                button2fbbus = value.toUInt();
                value = s.section(DS, 3, 3).stripWhiteSpace();
                button2fbcontact = value.toUInt();
            }
        }
    }
}

void ExternalDualButtonPanel::writeFileTextToStream(QTextStream& ts)
{
    ts << GF_CLASSID << DS << classid << endl
        << GF_INDEX << DS << iSoldIndex<< endl;
    ts << GF_BUTTON1FB << DS << enable1fbtrigger << DS
        << button1fbbus << DS << button1fbcontact << endl;
    ts << GF_BUTTON2FB << DS << enable2fbtrigger << DS
        << button2fbbus << DS << button2fbcontact << endl;
    ts << '%' << endl;
    modified = false;
}


/*show dialog at click position, position is also used to check if left
 * or right button is edited*/
bool ExternalDualButtonPanel::showFeedbackTriggerDialog(const QPoint& p)
{
    bool returnvalue = false;
    
    // select Button names; FHT WGT, WHT UfGT,...
    bool left = p.x() < (width() / 2);

    if (left)
        return ExternalButtonPanel::showFeedbackTriggerDialog(p);

    FeedbackTriggerDialog* dlg = new FeedbackTriggerDialog(this);
    if (dlg == NULL)
        return false;

    /*move dialog to mouse click point*/
    dlg->move(QCursor::pos());
    dlg->setCaption(buttontext2);
    dlg->setCheckBoxText(tr("&Enable feedback trigger"));
    dlg->enableTrigger(enable2fbtrigger);
    dlg->setFBBus(button2fbbus);
    dlg->setFBContact(button2fbcontact);

    connect(dlg, SIGNAL(sigShowFBmodules()),
            this, SIGNAL(sigShowFBmodules()));

    if (dlg->exec() == QDialog::Accepted) {
        enable2fbtrigger = dlg->isTriggerEnabled();
        button2fbbus = dlg->getFBBus();
        button2fbcontact = dlg->getFBContact();
        returnvalue = true;
        setupElementIcon();
    }

    disconnect(dlg, SIGNAL(sigShowFBmodules()),
            this, SIGNAL(sigShowFBmodules()));

    delete dlg;
    return returnvalue;
}

