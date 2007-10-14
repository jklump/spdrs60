/*
 * serverinfodialog.cpp
 * --------------------
 * Begin        : 2007-08-22
 * Last modified: $Date: 2007-10-14 10:49:40 $
 *                $Revision: 1.5 $
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Description  : Dialog window to display SRCP server information
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qgrid.h>

#include "serverinfodialog.h"


ServerInfoDialog::ServerInfoDialog(SrcpPort::CommunicationStyle style,
        QWidget* parent, const char* name) : QDialog(parent, name, true)
{
    setCaption(tr("SRCP server information"));
    QVBoxLayout* baseLayout = new QVBoxLayout(this, 10, 10);

    // command session groupbox
    QGroupBox* commandGB = new QGroupBox(0, Qt::Horizontal,
            tr("Command session"), this, "commandGB");
    baseLayout->addWidget(commandGB);

    // grid layout with 4 rows and 2 columns
    QGridLayout* cmdLayout = new QGridLayout(commandGB->layout(), 5, 2, 8,
            "cmdLayout");
    
    // 1. line
    cmdLayout->addWidget(new QLabel(tr("Server:"),
                commandGB), 0, 0);
    cmdServerLbl = new QLabel("", commandGB);
    cmdLayout->addWidget(cmdServerLbl, 0, 1);

    // 2. line
    cmdLayout->addWidget(new QLabel(tr("SRCP version:"),
                commandGB), 1, 0);
    cmdSrcpLbl = new QLabel("", commandGB);
    cmdLayout->addWidget(cmdSrcpLbl, 1, 1);

    QLabel* label;
    // 3. line
    label = new QLabel(tr("Other SRCP version:"), commandGB);
    cmdLayout->addWidget(label, 2, 0);
    label->setHidden(SrcpPort::csOld == style);
    cmdSrcpOtherLbl = new QLabel("", commandGB);
    cmdLayout->addWidget(cmdSrcpOtherLbl, 2, 1);
    cmdSrcpOtherLbl->setHidden(SrcpPort::csOld == style);

    // 4. line
    label = new QLabel(tr("SRCP session id:"), commandGB);
    cmdLayout->addWidget(label, 3, 0);
    label->setHidden(SrcpPort::csOld == style);
    cmdSessionIdLbl = new QLabel("", commandGB);
    cmdLayout->addWidget(cmdSessionIdLbl, 3, 1);
    cmdSessionIdLbl->setHidden(SrcpPort::csOld == style);

    // 5. line
    label = new QLabel(tr("Generic Messages:"), commandGB);
    cmdLayout->addWidget(label, 4, 0);
    label->setHidden(SrcpPort::csOld == style);
    cmdGmLbl = new QLabel("No", commandGB);
    cmdLayout->addWidget(cmdGmLbl, 4, 1);
    cmdGmLbl->setHidden(SrcpPort::csOld == style);


    // info session groupbox
    QGroupBox* infoGB = new QGroupBox(0, Qt::Horizontal,
            tr("Info session"), this, "infoGB");
    baseLayout->addWidget(infoGB);
    QGridLayout* infoLayout = new QGridLayout(infoGB->layout(), 4, 2, 8,
            "infoLayout");

    // 1. line
    infoLayout->addWidget(new QLabel(tr("Server:"),
                infoGB), 0, 0);
    infoServerLbl = new QLabel("", infoGB);
    infoLayout->addWidget(infoServerLbl, 0, 1);

    // 2. line
    infoLayout->addWidget(new QLabel(tr("SRCP version:"),
                infoGB) , 1, 0);
    infoSrcpLbl = new QLabel("", infoGB);
    infoLayout->addWidget(infoSrcpLbl, 1, 1);

    // 3. line
    infoLayout->addWidget(new QLabel(tr("Other SRCP version:"),
                infoGB), 2, 0);
    infoSrcpOtherLbl = new QLabel("", infoGB);
    infoLayout->addWidget(infoSrcpOtherLbl, 2, 1);

    // 4. line
    infoLayout->addWidget(new QLabel(tr("SRCP session id:"),
                infoGB) , 3, 0);
    infoSessionIdLbl = new QLabel("", infoGB);
    infoLayout->addWidget(infoSessionIdLbl, 3, 1);

    infoGB->setHidden(SrcpPort::csOld == style);


    // ports groupbox for old style information
    QGroupBox* portsGB = new QGroupBox(0, Qt::Horizontal,
            tr("Additional ports"), this, "portsGB");
    baseLayout->addWidget(portsGB);
    QGridLayout* portsLayout = new QGridLayout(portsGB->layout(), 2, 2, 8,
            "portsLayout");

    // 1. line
    portsLayout->addWidget(new QLabel(tr("Feedback port:"),
                portsGB), 0, 0);
    feedbackPortLbl = new QLabel("Not connected", portsGB);
    portsLayout->addWidget(feedbackPortLbl, 0, 1);

    // 2. line
    portsLayout->addWidget(new QLabel(tr("Info port:"),
                portsGB) , 1, 0);
    infoPortLbl = new QLabel("Not Connected", portsGB);
    portsLayout->addWidget(infoPortLbl, 1, 1);

    portsGB->setHidden(SrcpPort::csOld != style);


    // OK button
    QHBoxLayout* buttonLayout = new QHBoxLayout(0, 0, 6);
    baseLayout->addLayout(buttonLayout);
    buttonLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));

    QPushButton* okPB = new QPushButton(tr("OK"), this);
    buttonLayout->addWidget(okPB);
    okPB->setDefault(true);
    connect(okPB, SIGNAL(clicked()), this, SLOT(accept()));

    buttonLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));
}

/*
 * set data of command session
 */
void ServerInfoDialog::setCommandSessionData(const QString& server,
        const QString& srcp, const QString& srcpother,
        unsigned int sessionid, bool gm)
{
    cmdServerLbl->setText(server);
    cmdSrcpLbl->setText(srcp);
    cmdSrcpOtherLbl->setText(srcpother);
    cmdSessionIdLbl->setText(QString::number(sessionid));

    if (gm)
        cmdGmLbl->setText(tr("Yes"));
    else
        cmdGmLbl->setText(tr("No"));
}

/*
 * set data of info session
 */
void ServerInfoDialog::setInfoSessionData(const QString& server,
        const QString& srcp, const QString& srcpother,
        unsigned int sessionid)
{
    infoServerLbl->setText(server);
    infoSrcpLbl->setText(srcp);
    infoSrcpOtherLbl->setText(srcpother);
    infoSessionIdLbl->setText(QString::number(sessionid));
}


void ServerInfoDialog::setOldCommandData(const QString& server,
        const QString& srcp)
{
    cmdServerLbl->setText(server);
    cmdSrcpLbl->setText(srcp);
}


void ServerInfoDialog::setOldFeedbackConnected(bool connected)
{
    if (connected)
        feedbackPortLbl->setText(tr("Connected"));
    else
        feedbackPortLbl->setText(tr("Not connected"));
}


void ServerInfoDialog::setOldInfoConnected(bool connected)
{
    if (connected)
        infoPortLbl->setText(tr("Connected"));
    else
        infoPortLbl->setText(tr("Not connected"));
}

