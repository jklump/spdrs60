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

#include <qaction.h>
#include <qcursor.h>
#include <qpopupmenu.h>
#include <qpainter.h>

#include "externalbuttonpanel.h"
#include "feedbacktriggerdialog.h"


ExternalButtonPanel::ExternalButtonPanel(QWidget* parent,
        SpdrItemClassId cid, GbsButtonState cb, const char* bt):
    SpdrPanel(parent, cid, kvmEditLayout),
    enable1fbtrigger(false),
    button1fbbus(1),
    button1fbcontact(1),
    ctrlButton(cb),
    buttontext(bt)
{
    setName("ExternalButtonPanel");
}


ExternalButtonPanel::ExternalButtonPanel(QTextStream& ts, QWidget*
        parent, SpdrItemClassId cid, GbsButtonState cb, const char* bt):
    SpdrPanel(parent, cid, kvmNormal),
    enable1fbtrigger(false),
    button1fbbus(1),
    button1fbcontact(1),
    ctrlButton(cb),
    buttontext(bt)
{
    setName("ExternalButtonPanel");
}


void ExternalButtonPanel::buttonTriggered()
{
    emit buttonClicked(ctrlButton);
}

/**
 * respond to mouse press events
 */
void ExternalButtonPanel::mousePressEvent(QMouseEvent* e)
{
    /*normal mode*/
    if (visualMode == kvmNormal) {
        if (e->button() == Qt::LeftButton) {
            buttonTriggered();
            e->accept();
        }
        else
            e->ignore();
    }
    else
        e->ignore();
}

/**
 * respond to mouse release events
 */
void ExternalButtonPanel::mouseReleaseEvent(QMouseEvent* e)
{
    /*layout edit mode*/
    if (visualMode == kvmEditLayout) {
        if (e->button() == Qt::RightButton) {
            runPropertyMenue(e->pos());
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
void ExternalButtonPanel::slotOccupyElement(unsigned int bus,
        unsigned int contact, bool ostate)
{
    /*shortcut if (occupation state = 0) => button release message*/
    if (!ostate)
        return;

    /*button 1 trigger only on*/
    if (enable1fbtrigger) {
        if ((bus == button1fbbus) && (contact == button1fbcontact))
            buttonTriggered();
    }
}

/* Read the layout item data from stream.
 * Lines starting with # are recognized as comments, lines starting
 * with % are recognized as end of dataset (new style file format)*/
void ExternalButtonPanel::readFileTextFromStream(QTextStream& ats)
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
                  break;
            }
            else if (key.compare(GF_BUTTON1FB) == 0) {
                enable1fbtrigger = value.toUInt();
                value = s.section(DS, 2, 2).stripWhiteSpace();
                button1fbbus = value.toUInt();
                value = s.section(DS, 3, 3).stripWhiteSpace();
                button1fbcontact = value.toUInt();
            }
        }
    }
}

void ExternalButtonPanel::writeFileTextToStream(QTextStream& ts)
{
    ts << GF_CLASSID << DS << classid << endl
        << GF_INDEX << DS << iSoldIndex<< endl;
    ts << GF_BUTTON1FB << DS << enable1fbtrigger << DS
        << button1fbbus << DS << button1fbcontact << endl;
    ts << '%' << endl;
    modified = false;
}


/*show dialog at click position, position is also used to check if left
 * or right button is edited*/
bool ExternalButtonPanel::showFeedbackTriggerDialog(const QPoint& p)
{
    bool returnvalue = false;

    FeedbackTriggerDialog* dlg = new FeedbackTriggerDialog(this);
    if (dlg == NULL)
        return false;

    /*move dialog to mouse click point*/
    dlg->move(QCursor::pos());
    dlg->setCaption(buttontext);
    dlg->setCheckBoxText(tr("&Enable feedback trigger"));
    dlg->enableTrigger(enable1fbtrigger);
    dlg->setFBBus(button1fbbus);
    dlg->setFBContact(button1fbcontact);

    connect(dlg, SIGNAL(sigShowFBmodules()),
            this, SIGNAL(sigShowFBmodules()));

    if (dlg->exec() == QDialog::Accepted) {
        enable1fbtrigger = dlg->isTriggerEnabled();
        button1fbbus = dlg->getFBBus();
        button1fbcontact = dlg->getFBContact();
        returnvalue = true;
        setupElementIcon();
    }

    disconnect(dlg, SIGNAL(sigShowFBmodules()),
            this, SIGNAL(sigShowFBmodules()));

    delete dlg;
    return returnvalue;
}

/*setup property menu and keep click position */
void ExternalButtonPanel::runPropertyMenue(const QPoint& p)
{
    QPopupMenu* propmenu = new QPopupMenu(this);
    propmenu->setName("propertyMenu");

    propmenu->insertItem(tr("Trigger &button..."), 1);

    int mitem = propmenu->exec(QCursor::pos());
    bool elchanged = false;

    switch(mitem) {
        case 1:
            elchanged = showFeedbackTriggerDialog(p);
            break;
        case -1: //fall through
        default:
            break;
    }
    if (elchanged)
        modified = true;
    delete propmenu; 
}
