/***************************************************************************
 messagehistory.cpp
 ------------------
 Begin      : 2007-08-20
 Copyright  : (C) 2007 by Guido Scholz
 E-Mail     : guido.scholz@bayernline.de
 Description: Stacked set of Combo Boxex to collect different message types
              in separated lists.
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qdatetime.h>
#include <qfont.h>
#include <qlabel.h>

#include "messagehistory.h"

// max number of history lines
#define MAX_HISTORY 100


MessageHistory::MessageHistory(QWidget* parent): QHBox(parent,
        "MessageHistory", 0)
{
    setSpacing(2);

    lblStack = new QWidgetStack(this, "cbstack");
    lblStack->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
                QSizePolicy::Fixed, false));
    
    QLabel *HintLabel = new QLabel(lblStack, "Hint");
    HintLabel->setText(tr("Hints"));
    HintLabel->setIndent(3);
    lblStack->addWidget(HintLabel, 0);

    QLabel *CmdLabel = new QLabel(lblStack, "Cmd");
    CmdLabel->setText(tr("Commands"));
    CmdLabel->setIndent(3);
    lblStack->addWidget(CmdLabel, 1);

    QLabel *InfoLabel = new QLabel(lblStack, "Info");
    InfoLabel->setText(tr("Infoport"));
    InfoLabel->setIndent(3);
    lblStack->addWidget(InfoLabel, 2);

    QLabel *FeedBackLabel = new QLabel(lblStack, "Feedb");
    FeedBackLabel->setText(tr("Feedbacks"));
    FeedBackLabel->setIndent(3);
    lblStack->addWidget(FeedBackLabel, 3);

    cbStack = new QWidgetStack(this, "cbstack");
    cbStack->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                           QSizePolicy::Fixed, false));

    // set mono space font for message texts
    QFont f;
    f.setFamily("Courier");

    hintCB = new QComboBox(false, cbStack);
#if QT_VERSION >= 0x040000
    hintCB->setMaxVisibleItems(15);
#else
    hintCB->setSizeLimit(15);
#endif
    hintCB->setFont(f);
    cbStack->addWidget(hintCB, 0);

    commandCB = new QComboBox(false, cbStack);
#if QT_VERSION >= 0x040000
    commandCB->setMaxVisibleItems(15);
#else
    commandCB->setSizeLimit(15);
#endif
    commandCB->setFont(f);
    cbStack->addWidget(commandCB, 1);

    infoCB = new QComboBox(false, cbStack);
#if QT_VERSION >= 0x040000
    infoCB->setMaxVisibleItems(15);
#else
    infoCB->setSizeLimit(15);
#endif
    infoCB->setFont(f);
    cbStack->addWidget(infoCB, 2);

#if QT_VERSION < 0x030200
    // force painting of history lines and their labels
    cbStack->raiseWidget(0);
    lblStack->raiseWidget(0);
#endif

    visibleLine = vlHint;
    showTime = true;
}


/*
 * cyclic toggling between three history lines
 */
void MessageHistory::toggleLine()
{
    if (visibleLine == vlHint)
        visibleLine = vlCommand;
    else if (visibleLine == vlCommand)
        visibleLine = vlInfo;
    else if (visibleLine == vlInfo)
        visibleLine = vlHint;


    switch (visibleLine) {
        case (vlHint):
            cbStack->raiseWidget(0);
            lblStack->raiseWidget(0);
            break;
        case (vlCommand):
            cbStack->raiseWidget(1);
            lblStack->raiseWidget(1);
            break;
        case (vlInfo):
            cbStack->raiseWidget(2);
            lblStack->raiseWidget(2);
            break;
    }
}

/*
 * add a message to the hint combo box
 */
void MessageHistory::addHint(const QString& msg)
{
    if (hintCB->count() == MAX_HISTORY)
        hintCB->removeItem(0);

    if (showTime) {
        QString timemsg = getCurrentTime();
        timemsg.append(" ");
        timemsg.append(msg);
        hintCB->insertItem(timemsg);
    }
    else
        hintCB->insertItem(msg);
    hintCB->setCurrentItem(hintCB->count() - 1);
}

/*
 * add a message to the command combo box
 */
void MessageHistory::addCommand(const QString& msg)
{
    if (commandCB->count() == MAX_HISTORY)
        commandCB->removeItem(0);

    if (showTime) {
        QString timemsg = getCurrentTime();
        timemsg.append(" ");
        timemsg.append(msg);
        commandCB->insertItem(timemsg);
    }
    else
        commandCB->insertItem(msg);
    commandCB->setCurrentItem(commandCB->count() - 1);
}

/*
 * add a message to the info combo box
 */
void MessageHistory::addInfo(const QString& msg)
{
    if (infoCB->count() == MAX_HISTORY)
        infoCB->removeItem(0);

    if (showTime) {
        QString timemsg = getCurrentTime();
        timemsg.append(" ");
        timemsg.append(msg);
        infoCB->insertItem(timemsg);
    }
    else
        infoCB->insertItem(msg);
    infoCB->setCurrentItem(infoCB->count() - 1);
}

/*
 * get a formated string with current time
 */
QString MessageHistory::getCurrentTime()
{
    QTime ctime = QTime::currentTime();
    QString timestr = ctime.toString("hh:mm:ss.zzz");
    return timestr;
}


void MessageHistory::enableTime(bool enable)
{
    if (showTime != enable)
        showTime = enable;
}


bool MessageHistory::isTimeEnabled()
{
    return showTime;
}

