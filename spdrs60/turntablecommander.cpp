/***************************************************************************
                           turntablecommander.cpp
                           version 0.5.6 $Revision: 1.18 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2009 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2009-10-26 21:59:44 $
***************************************************************************/

/******************************************************************************
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by     *
 *   the Free Software Foundation; either version 2 of the License, or        *
 *   (at your option) any later version.                                      *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
    this code provides a GUI to control maerklin's digital turntable
 ******************************************************************************/

#include <stdlib.h>             // for abs()
#include <math.h>               // for nearbyint()
#include <qlayout.h>
#include <qstringlist.h>

#include "turntablecommander.h"
#include "preferences.h"

/*button icons*/
#include "pixmaps/tt_stop.xpm"
#include "pixmaps/tt_goto.xpm"
#include "pixmaps/tt_left.xpm"
#include "pixmaps/tt_left_step.xpm"
#include "pixmaps/tt_right.xpm"
#include "pixmaps/tt_right_step.xpm"
#include "pixmaps/tt_prog.xpm"
#include "pixmaps/tt_turn180.xpm"

// shortcuts for commands equal keys on maerklin keyboard
// key numbers from 1 to 24, 0 is red button, 1 is green button
#define KEY_LFT 3, 1
#define KEY_RGT 3, 0
#define KEY_END 1, 0
#define KEY_INP 1, 1
#define KEY_CLR 2, 0
#define KEY_TRN 2, 1

// turn modes for digital turntable
// and modes for digital turntable
enum {
    LEFT = 1,
    RIGHT = 0,
    USAGE = 1,
    PROG = 0
};


turntableCommander::turntableCommander(const QString& tracks,
        QWidget * parent, int activetrack):
    QDialog(NULL, "turntableCommander", false)
{
    // mode of step buttons is normal use, not programming
    bStepMode = USAGE;
    
    // no tracks available on startup
    for (int i = 0; i < 24; i++)
        iTracks[i] = 0;
    
    // get active track from element data
    iActiveTrack = activetrack;
    // no new track selected
    iNewTrack = 0;
    setCaption(tr("Digital turntable commander"));

    // save all available tracks from element
    // data in combined format with ;'s

    QStringList list = QStringList::split(';', tracks);
    if (list.count() > 0) {
        unsigned int pos = 0;
        for (QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
            pos = (*it).toUInt();
            if ((pos != 0) && (pos < 24)) {
                iTracks[pos - 1] = 1;
            }
        }
    }

    QVBoxLayout* mainLayout = new QVBoxLayout(this, 4, 8, "mainLayout");
    QHBoxLayout* buttonLayout = new QHBoxLayout(mainLayout, 4);

    // create a button to step one track to the left
    pixButton = QPixmap(tt_left_step_xpm);
    buttLeftStep = new QPushButton("<", this);
    buttonLayout->addWidget(buttLeftStep);
    buttLeftStep->setPixmap(pixButton);
    connect(buttLeftStep, SIGNAL(clicked()), this, SLOT(slotLeftStep()));

    // create a button to choose the turning direction
    bgChooseDir = new QButtonGroup(1, Qt::Vertical, this, "turnButtonGrp");
    bgChooseDir->setExclusive(true);
    bgChooseDir->setMargin(0);
    bgChooseDir->setInsideMargin(0);
    bgChooseDir->setFrameStyle(QFrame::NoFrame);
    buttonLayout->addWidget(bgChooseDir);
    connect(bgChooseDir, SIGNAL(clicked(int)),
            this, SLOT(slotChooseDir(int)));

    // create a button to turn the table anti-clockwise
    pixButton = QPixmap(tt_left_xpm);
    buttChooseLeft = new QPushButton("<<", bgChooseDir);
    buttChooseLeft->setToggleButton(true);
    buttChooseLeft->setPixmap(pixButton);

    // create a button to turn the table clockwise
    pixButton = QPixmap(tt_right_xpm);
    buttChooseRight = new QPushButton(">>", bgChooseDir);
    buttChooseRight->setPixmap(pixButton);
    buttChooseRight->setToggleButton(true);

    // create a button to step one track to the right
    pixButton = QPixmap(tt_right_step_xpm);
    buttRightStep = new QPushButton(">", this);
    buttonLayout->addWidget(buttRightStep);
    buttRightStep->setPixmap(pixButton);
    connect(buttRightStep, SIGNAL(clicked()), this, SLOT(slotRightStep()));

    // create a dropdown list with all available tracks
    listTracks = new QListBox(this, "trackLB", 0);
    listTracks->setFixedHeight(24);
    listTracks->setFixedWidth(44);
    buttonLayout->addWidget(listTracks);
    for (int i = 0; i < 24; i++)
        if (iTracks[i] == 1) {
            listTracks->insertItem(QString::number(i + 1), -1);
        }

    connect(listTracks, SIGNAL(highlighted(const QString&)),
            this, SLOT(slotSaveNewTrack(const QString&)));

    // create a button to go to a certain track
    pixButton = QPixmap(tt_goto_xpm);
    buttGoToTrack = new QPushButton("->°", this);
    buttonLayout->addWidget(buttGoToTrack);
    buttGoToTrack->setPixmap(pixButton);
    connect(buttGoToTrack, SIGNAL(clicked()), this, SLOT(slotGoToTrack()));

    // create a button to turn 180 degrees
    pixButton = QPixmap(tt_turn180_xpm);
    buttTurn180 = new QPushButton("<->", this);
    buttonLayout->addWidget(buttTurn180);
    buttTurn180->setPixmap(pixButton);
    connect(buttTurn180, SIGNAL(clicked()), this, SLOT(slotTurn180()));

    // create a button to stop rotating
    pixButton = QPixmap(tt_stop_xpm);
    buttStopCont = new QPushButton("o", this);
    buttonLayout->addWidget(buttStopCont);
    buttStopCont->setPixmap(pixButton);
    buttStopCont->setEnabled(false);
    connect(buttStopCont, SIGNAL(clicked()), this, SLOT(slotStopCont()));

    // create a button to get to the programming area
    pixButton = QPixmap(tt_prog_xpm);
    buttSetup = new QPushButton(tr("&Setup"), this);
    buttonLayout->addWidget(buttSetup);
    buttSetup->setPixmap(pixButton);
    buttSetup->setToggleButton(true);
    connect(buttSetup, SIGNAL(toggled(bool)),
            this, SLOT(slotResizeCommander(bool)));

    buttonLayout->addStretch();

    buttChooseLeft->setOn(true);        // setup button states
    buttChooseLeft->setEnabled(!pref.autottdir);
    buttChooseRight->setEnabled(!pref.autottdir);


    /*grid with 24 position labels*/
    QHBoxLayout* posBaseLayout = new QHBoxLayout(mainLayout);
    posBaseLayout->addStretch();
    QGridLayout* grid = new QGridLayout(posBaseLayout, 2, 12, 4, "posGrid");
     // show available tracks
    for (int i = 0; i < 24; i++) {
        labelTracks[i] = new QLabel(QString::number(i + 1), this, 0, 0);
        labelTracks[i]->setAlignment(Qt::AlignCenter);
        grid->addWidget(labelTracks[i],
                (i < 12) ? 0 : 1, i % 12);
    }
    posBaseLayout->addStretch();

    
    // create a buttongroup for all programming buttons
    bgProg = new QButtonGroup(2, Qt::Vertical,
            tr("Digital turntable programmer"), this, "buttonGrpBox");
    mainLayout->addWidget(bgProg);

    // create a button to start programming
    buttInput = new QPushButton(tr("&Start programming"), bgProg);

    // create a button to save bridge's starting position
    buttSave = new QPushButton(tr("Save &position #1"), bgProg);
    buttSave->setEnabled(false);

    // create a button to add new positions
    buttAddPos = new QPushButton(tr("Pos. #1 is saved!"), bgProg);
    buttAddPos->setEnabled(false);

    // create a button to end programming
    buttEnd = new QPushButton(tr("&Done"), bgProg);
    buttEnd->setEnabled(false);

    connect(bgProg, SIGNAL(released(int)),
            this, SLOT(slotProgrammer(int)));

    bgProg->hide();


    QToolTip::add(buttLeftStep,
            tr("Step to next available track\n"
                "counter-clockwise"));
    QToolTip::add(buttRightStep,
            tr("Step to next available track\nclockwise"));
    QToolTip::add(buttChooseLeft,
            tr("Select counter-\nclockwise rotation"));
    QToolTip::add(buttChooseRight, tr("Select clockwise\nrotation"));
    QToolTip::add(buttStopCont, tr("Stop rotating"));
    QToolTip::add(listTracks,
            tr("Choose the new track the turntable\n"
                "shall go to"));
    QToolTip::add(buttGoToTrack, tr("Go to selected track"));
    QToolTip::add(buttTurn180, tr("Turn bridge at 180°"));
    QToolTip::add(buttSetup, tr("Setup turntable"));
    QToolTip::add(buttInput,
            tr("Press this button within 5 seconds after\n"
                "having switched on power to the whole\n"
                "layout (not just to your PC)."));
    QToolTip::add(buttSave,
            tr("Press this button to save the current\n"
                "bridge position as position #1."));
    QToolTip::add(buttAddPos,
            tr("Press this button to add an other position\n"
                "for a track."));
    QToolTip::add(buttEnd,
            tr("Press this button to end programming mode."));


    displayTracks();            // displays the active position
    slotChooseDir(0);           // default: bridge turns anti-clockwise
    buttGoToTrack->setFocus();  // default button focus
}


void turntableCommander::slotResizeCommander(bool bShowProgArea)
{
    if (bShowProgArea)
        bgProg->show();
    else {
        bgProg->hide();
        adjustSize();
    }

    // in prog mode set focus to the first programming button
    if (bShowProgArea == 1)
        buttInput->setFocus();
}


void turntableCommander::slotProgrammer(int iButtID)
{
    switch (iButtID) {
        case 0: //Start programming
            buttInput->setEnabled(false);
            buttSave->setEnabled(true);
            activateUsageButtons(false);
            bStepMode = PROG;
            iTotalProgTracks = 0;
            for (int i = 0; i < 24; i++)
                iTracks[i] = 0;
            iActiveTrack = 1;
            displayTracks();
            emit sendTtCommand(KEY_INP);  // "INPUT" key
            buttSave->setFocus();
            break;

        case 1: //Save &position #x
            buttSave->setEnabled(false);
            buttAddPos->setEnabled(true);
            buttEnd->setEnabled(true);

            iTracks[0] = 1;
            iTotalProgTracks += 1;
            buttAddPos->setText(tr("Pos. #1 saved!"));
            emit sendTtCommand(KEY_CLR);  // "CLEAR" key
            buttAddPos->setFocus();
            break;

        case 2: //Pos. #1 is saved!
            if (iTracks[iActiveTrack - 1] == 0) {
                iTracks[iActiveTrack - 1] = 1;
                iTotalProgTracks += 1;
                buttAddPos->setText(tr("Pos. #%1 saved!").arg(iActiveTrack));
                emit sendTtCommand(KEY_INP);      // "INPUT" key
            }
            if (iTotalProgTracks < 24)
                break;
            // else fall through

        case 3: //Done
            buttInput->setEnabled(true);
            buttAddPos->setEnabled(false);
            buttEnd->setEnabled(false);
            activateUsageButtons(true);
            bStepMode = USAGE;

            emit sendTtCommand(KEY_END);  // "END" key

            buttSetup->toggle();
            slotResizeCommander(0);
            buttGoToTrack->setFocus();
            sendTracks();
            displayTracks();
            iNewTrack = 1;
            slotGoToTrack();
            break;
    }
}


void turntableCommander::activateUsageButtons(bool bState)
{
    buttTurn180->setEnabled(bState);    // en/disable normal using buttons
    buttChooseLeft->setEnabled(bState); // while programming/rotating
    buttChooseRight->setEnabled(bState);
    buttStopCont->setEnabled(bState);
    buttGoToTrack->setEnabled(bState);
    buttSetup->setEnabled(bState);
    listTracks->setEnabled(bState);
}


void turntableCommander::slotSaveNewTrack(const QString & cNewTrack_)
{
    bool ok;

    iNewTrack = cNewTrack_.toInt(&ok, 10);
    // saves new track value selected in drop-down track-list
}


void turntableCommander::slotGoToTrack()
{
    int iTracksToMove = abs(iActiveTrack - iNewTrack);
    // do nothing if new track is old track
    if (iTracksToMove == 0 || iNewTrack == 0)
        return;

    // autoselect direction of rotating
    if (pref.autottdir || bStep) {
        bgChooseDir->
            setButton((iTracksToMove > 12) ^ (iNewTrack > iActiveTrack));
        slotChooseDir((iTracksToMove > 12) ^ (iNewTrack > iActiveTrack));
    }
    // send command and start timer
    emit sendTtCommand((iNewTrack + 9) / 2, !(iNewTrack % 2));
    startTrackTimer();
}


void turntableCommander::startTrackTimer()
{
    buttStopCont->setEnabled(true);     // en/disable buttons while rotating
    buttLeftStep->setEnabled(false);
    buttRightStep->setEnabled(false);
    buttChooseLeft->setEnabled(false);
    buttChooseRight->setEnabled(false);
    buttGoToTrack->setEnabled(false);
    buttTurn180->setEnabled(false);
    buttSetup->setEnabled(false);
    listTracks->setEnabled(false);

    tTrackReached = new QTimer();       // start timer
#if QT_VERSION >= 0x040000
    tTrackReached->start((int) 1000 * pref.ttroundtime / 24);
#else
    tTrackReached->start((int) nearbyint(1000 * pref.ttroundtime / 24));
#endif
    connect(tTrackReached, SIGNAL(timeout()),
            this, SLOT(slotTrackReached()));
}


void turntableCommander::displayTracks()
{
    // display track numbers with ...
    for (int i = 0; i < 24; i++) {

        // ... normal style if not available
        if (iTracks[i] == 0 || bStepMode == PROG)
            labelTracks[i]->setBackgroundColor(Qt::lightGray);
        else
            labelTracks[i]->setBackgroundColor(QColor(80, 255, 80));

        // draw a yellow frame if active track
        if (iActiveTrack == i + 1)
            labelTracks[i]->setBackgroundColor(QColor(255, 255, 0));
    }
}


void turntableCommander::slotTrackReached()
{
    // calculate new track ( right == bDir=0 )
    int iTrackID = ((bDir == RIGHT) ? iActiveTrack + 1 : iActiveTrack - 1);
    // correct trackID if "spin over"
    if (iTrackID == 0)
        iTrackID = 24;
    if (iTrackID == 25)
        iTrackID = 1;
    iActiveTrack = iTrackID;
    // display tracks
    displayTracks();

    // enable most buttons if track is reached
    if (iNewTrack == iTrackID) {
        buttStopCont->setEnabled(false);
        buttChooseLeft->setEnabled(!pref.autottdir);
        buttChooseRight->setEnabled(!pref.autottdir);
        buttLeftStep->setEnabled(true);
        buttRightStep->setEnabled(true);
        buttGoToTrack->setEnabled(true);
        buttTurn180->setEnabled(true);
        buttSetup->setEnabled(true);
        listTracks->setEnabled(true);
        delete tTrackReached;
    }
}


void turntableCommander::slotTurn180()
{
    iNewTrack = iActiveTrack + 12;
    if (iNewTrack > 24)         // turns bridge 180 degrees
        iNewTrack -= 24;

    emit sendTtCommand(KEY_TRN);      // build command and start timer for track
    startTrackTimer();          // display
}


void turntableCommander::slotLeftStep()
{
    int i;

    if (bStepMode == PROG) {
        if (iTracks[0] == 1) {
            if (iActiveTrack == 1)
                iActiveTrack = 24;
            else
                iActiveTrack -= 1;
        }
        QString sText;

        if (iTracks[iActiveTrack - 1] == 0)
            sText.sprintf(tr("&Add position #%d"), iActiveTrack);
        else
            sText.sprintf(tr("Pos. #%d saved!"), iActiveTrack);
        buttAddPos->setText(sText);

        emit sendTtCommand(KEY_LFT);  // "<" key
    }
    else {
        bStep = true;
        i = iActiveTrack;
        do {
            if (i == 1)
                i = 24;
            else
                i -= 1;
        }
        while (iTracks[i - 1] != 1);
        iNewTrack = i;
        slotGoToTrack();
        bStep = false;
    }
}


void turntableCommander::slotRightStep()
{
    int i;

    if (bStepMode == PROG) {
        if (iTracks[0] == 1) {
            if (iActiveTrack == 24)
                iActiveTrack = 1;
            else
                iActiveTrack += 1;
        }
        QString sText;

        if (iTracks[iActiveTrack - 1] == 0)
            sText.sprintf(tr("&Add position #%d"), iActiveTrack);
        else
            sText.sprintf(tr("Pos. #%d is saved!"), iActiveTrack);
        buttAddPos->setText(sText);

        emit sendTtCommand(KEY_RGT);  // ">" key
    }
    else {
        bStep = true;
        i = iActiveTrack;
        // calculation only for display
        do {
            if (i == 24)
                i = 1;
            else
                i += 1;
        }
        while (iTracks[i - 1] != 1);
        iNewTrack = i;
        slotGoToTrack();
        bStep = false;
    }
}


void turntableCommander::slotChooseDir(int iDir_)
{
    bDir = !iDir_;              // changes rotating direction
    emit sendTtCommand(4, !iDir_);
}


void turntableCommander::slotStopCont()
{
    buttStopCont->setEnabled(false);    // enable most buttons
    buttLeftStep->setEnabled(true);
    buttRightStep->setEnabled(true);
    buttChooseLeft->setEnabled(true);
    buttChooseRight->setEnabled(true);
    buttGoToTrack->setEnabled(true);
    buttTurn180->setEnabled(true);
    buttSetup->setEnabled(true);
    listTracks->setEnabled(true);

    buttGoToTrack->setFocus();
    delete tTrackReached;       // delete timer

    emit sendTtCommand(KEY_END);      // build stopping command
}


void turntableCommander::sendTracks()
{
    QString s = "";
    bool isfirst = true;

    // build a string with the numbers of all available tracks
    // separated by a ";"
    for (int i = 0; i < 24; i++) {
        if (iTracks[i] == 1) {
            if (isfirst) {
                s = QString::number(i + 1);
                isfirst = false;
            }
            else
                s.append(";%1").arg(i + 1);
        }
    }
    emit sendAvailTracks(s);    // send track string to element
}

