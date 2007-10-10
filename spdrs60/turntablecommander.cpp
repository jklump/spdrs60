/***************************************************************************
                           turntablecommander.cpp
                           version 0.5.2 $Revision: 1.13 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2007 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-10-10 19:41:51 $
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

// turn modes for digital turntable
#define   LEFT             1
#define   RIGHT            0

// modes for digital turntable
#define   USAGE            1
#define   PROG             0


turntableCommander::turntableCommander(QWidget * parent, int iActiveTrack_,
                                       QString tracks)
:QDialog(0, "turntableCommander", false)
{
    // parent window always usable dummy to avoid compiler warning
    if (parent);
    // mode of step buttons is normal use, not programming
    bStepMode = USAGE;
    
    // no tracks available on startup
    for (int i = 0; i < 24; i++)
        iTracks[i] = 0;
    
    // get active track from element data
    iActiveTrack = iActiveTrack_;
    // no new track selected
    iNewTrack = 0;
    setCaption(tr("Digital turntable commander"));

    // save all available tracks from element
    // data in combined format with ;'s

    int trackcount = tracks.contains(";") ? 1 : 0;
    if (tracks.length() > 0)
        ++trackcount;
   
    unsigned int idx = 0;
    
    for (int i = 0; i < trackcount; ++i) {
        idx = tracks.section(";", i, i).toInt() - 1;
        if (idx < 24)
            iTracks[idx] = 1;
    }

    // create a button to step one track to the left
    pixButton = QPixmap(tt_left_step_xpm);
    buttLeftStep = new QPushButton("<", this);
    buttLeftStep->move(10, 5);
    buttLeftStep->resize(20, 20);
    buttLeftStep->setPixmap(pixButton);
    connect(buttLeftStep, SIGNAL(clicked()), this, SLOT(slotLeftStep()));

    // create a button to choose the turning direction
    bgChooseDir = new QButtonGroup(this, "");
    bgChooseDir->move(35, 5);
    bgChooseDir->resize(85, 20);
    bgChooseDir->setExclusive(true);
    bgChooseDir->setFrameStyle(QFrame::NoFrame);
    connect(bgChooseDir, SIGNAL(clicked(int)),
            this, SLOT(slotChooseDir(int)));

    // create a button to turn the table anti-clockwise
    pixButton = QPixmap(tt_left_xpm);
    buttChooseLeft = new QPushButton("<<", bgChooseDir);
    buttChooseLeft->move(0, 0);
    buttChooseLeft->resize(20, 20);
    buttChooseLeft->setToggleButton(true);
    buttChooseLeft->setPixmap(pixButton);

    // create a button to turn the table clockwise
    pixButton = QPixmap(tt_right_xpm);
    buttChooseRight = new QPushButton(">>", bgChooseDir);
    buttChooseRight->move(65, 0);
    buttChooseRight->resize(20, 20);
    buttChooseRight->setPixmap(pixButton);
    buttChooseRight->setToggleButton(true);

    // create a button to step one track to the right
    pixButton = QPixmap(tt_right_step_xpm);
    buttRightStep = new QPushButton(">", this);
    buttRightStep->move(125, 5);
    buttRightStep->resize(20, 20);
    buttRightStep->setPixmap(pixButton);
    connect(buttRightStep, SIGNAL(clicked()), this, SLOT(slotRightStep()));

    // create a button to go to a certain track
    pixButton = QPixmap(tt_goto_xpm);
    buttGoToTrack = new QPushButton("->°", this);
    buttGoToTrack->move(160, 5);
    buttGoToTrack->resize(20, 20);
    buttGoToTrack->setPixmap(pixButton);
    connect(buttGoToTrack, SIGNAL(clicked()), this, SLOT(slotGoToTrack()));

    // create a button to turn 180 degrees
    pixButton = QPixmap(tt_turn180_xpm);
    buttTurn180 = new QPushButton("<->", this);
    buttTurn180->move(185, 5);
    buttTurn180->resize(20, 20);
    buttTurn180->setPixmap(pixButton);
    connect(buttTurn180, SIGNAL(clicked()), this, SLOT(slotTurn180()));

    // create a button to stop rotating
    pixButton = QPixmap(tt_stop_xpm);
    buttStopCont = new QPushButton("o", this);
    buttStopCont->move(220, 5);
    buttStopCont->resize(20, 20);
    buttStopCont->setPixmap(pixButton);
    buttStopCont->setEnabled(false);
    connect(buttStopCont, SIGNAL(clicked()), this, SLOT(slotStopCont()));

    // create a button to get to the programming area
    pixButton = QPixmap(tt_prog_xpm);
    buttSetup = new QPushButton(tr("&Setup"), this);
    buttSetup->move(260, 5);
    buttSetup->resize(20, 20);
    buttSetup->setPixmap(pixButton);
    buttSetup->setToggleButton(true);
    connect(buttSetup, SIGNAL(toggled(bool)),
            this, SLOT(slotResizeCommander(bool)));


    buttChooseLeft->setOn(true);        // setup button states
    buttChooseLeft->setEnabled(!pref.autottdir);
    buttChooseRight->setEnabled(!pref.autottdir);

     // show available tracks
    for (int i = 0; i < 24; i++) {
        labelTracks[i] = new QLabel("", this, 0, 0);
        labelTracks[i]->setGeometry(0, 0, 0, 0);
        labelTracks[i]->setAlignment(Qt::AlignCenter);
    }

    // create a dropdown list with all available tracks
    
    listTracks = new QListBox(this, "trackLB", 0);
    for (int i = 0; i < 24; i++)
        if (iTracks[i] == 1) {
            listTracks->insertItem(QString::number(i + 1), -1);
        }

    listTracks->resize(40, 20);
    listTracks->move(60, 5);
    connect(listTracks, SIGNAL(highlighted(const QString&)),
            this, SLOT(slotSaveNewTrack(const QString&)));

    slotResizeCommander(0);     // minimize commander window
    setupProgArea();            // sets up ethe programming area
    displayTracks();            // displays the active position
    slotChooseDir(0);           // default: bridge turns anti-clockwise
    buttGoToTrack->setFocus();  // default button focus
}


void turntableCommander::setupProgArea()
{
    // create a separator line between usage and programming area
    line = new QFrame(this);
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    line->setGeometry(10, 80, this->width() - 20, 2);

    QLabel *label = new QLabel(tr("Digital turntable programmer:"), this);
    label->move(10, line->y() + 10);
    label->resize(label->sizeHint());

    // create a buttongroup for all programming buttons
    QButtonGroup *bgProg = new QButtonGroup("", this);
    bgProg->setFrameStyle(QFrame::NoFrame);
    bgProg->resize(line->width(), 75);
    bgProg->move(10, label->y() + label->height() + 10);

    // create a button to start programming
    buttInput = new QPushButton(tr("&Start programming"), bgProg);
    buttInput->resize(120, 30);
    buttInput->move(0, 0);

    // create a button to save bridge's starting position
    buttSave = new QPushButton(tr("Save &position #1"), bgProg);
    buttSave->resize(buttInput->width(), buttInput->height());
    buttSave->move(line->width() - buttInput->width(), 0);
    buttSave->setEnabled(false);

    // create a button to add new positions
    buttAddPos = new QPushButton(tr("Pos. #1 is saved!"), bgProg);
    buttAddPos->resize(buttInput->width(), buttInput->height());
    buttAddPos->move(0, buttInput->y() + buttInput->height() + 10);
    buttAddPos->setEnabled(false);

    // create a button to end programming
    buttEnd = new QPushButton(tr("&Done"), bgProg);
    buttEnd->resize(buttInput->width(), buttInput->height());
    buttEnd->move(line->width() - buttInput->width(),
                  buttInput->y() + buttInput->height() + 10);
    buttEnd->setEnabled(false);

    connect(bgProg, SIGNAL(released(int)),
            this, SLOT(slotProgrammer(int)));

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
}


void turntableCommander::slotResizeCommander(bool bShowProgArea)
{
    // set a constant width but a height dep. on mode
    this->setFixedWidth(285);
    this->setFixedHeight(80 + bShowProgArea * 110);
    // in prog mode set focus to the first programming button
    if (bShowProgArea == 1)
        buttInput->setFocus();
}


void turntableCommander::slotProgrammer(int iButtID)
{
    QString sText;
    switch (iButtID) {
    case 0:
        buttInput->setEnabled(false);
        buttSave->setEnabled(true);
        activateUsageButtons(false);
        bStepMode = PROG;
        iTotalProgTracks = 0;
        for (int i = 0; i < 24; i++)
            iTracks[i] = 0;
        iActiveTrack = 1;
        displayTracks();
        buildCommand(KEY_INP);  // "INPUT" key
        buttSave->setFocus();
        break;
    case 1:
        buttSave->setEnabled(false);
        buttAddPos->setEnabled(true);
        buttEnd->setEnabled(true);

        iTracks[0] = 1;
        iTotalProgTracks += 1;
        buttAddPos->setText(tr("Pos. #1 saved!"));
        buildCommand(KEY_CLR);  // "CLEAR" key
        buttAddPos->setFocus();
        break;
    case 2:
        if (iTracks[iActiveTrack - 1] == 0) {
            iTracks[iActiveTrack - 1] = 1;
            iTotalProgTracks += 1;
            sText.sprintf(tr("Pos. #%d saved!"), iActiveTrack);
            buttAddPos->setText(sText);
            buildCommand(KEY_INP);      // "INPUT" key
        }
        if (iTotalProgTracks < 24)
            break;
    case 3:
        buttInput->setEnabled(true);
        buttAddPos->setEnabled(false);
        buttEnd->setEnabled(false);
        activateUsageButtons(true);
        bStepMode = USAGE;

        buildCommand(KEY_END);  // "END" key

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
    buildCommand((iNewTrack + 9) / 2, !(iNewTrack % 2));
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
        labelTracks[i]->setGeometry(5 + 23 * i - 276 * (i >= 12),
                                    35 + 20 * (i >= 12), 20, 17);

        // ... normal style if not available
        if (iTracks[i] == 0 || bStepMode == PROG) {
            labelTracks[i]->setFont(QFont("Helvetica", 10, QFont::Normal));
            labelTracks[i]->setBackgroundColor(Qt::lightGray);
        }
        // ... bold style and green background if available
        else {
            labelTracks[i]->setFont(QFont("Helvetica", 12, QFont::Black));
            labelTracks[i]->setBackgroundColor(QColor(80, 255, 80));
        }

        // draw a yellow frame if active track
        if (iActiveTrack == i + 1) {
            labelTracks[i]->setFrameStyle(QFrame::Panel | QFrame::Raised);
            labelTracks[i]->setBackgroundColor(QColor(255, 255, 0));
        }
        else
            labelTracks[i]->setFrameStyle(QFrame::NoFrame);

        // show tracknumber
        labelTracks[i]->setText(QString::number(i + 1));
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

    buildCommand(KEY_TRN);      // build command and start timer for track
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

        buildCommand(KEY_LFT);  // "<" key
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

        buildCommand(KEY_RGT);  // ">" key
    }
    else {
        bStep = true;
        i = iActiveTrack;
        // berechnen nur für Anzeige nötig
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
    buildCommand(4, !iDir_);
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

    buildCommand(KEY_END);      // build stopping command
}


void turntableCommander::buildCommand(int keyno, int keycolor)
{
    /* 
     * KEY constants are translated into a QPoint variable and send to
     * element -> to member function sendCommand which sends it to daemon
     * 0 == red key, 1 == green key, key no from 1 (end, input) to 24 (24)
     */
    emit applyPressed(QPoint(keyno, keycolor));
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
                s.append(QString::number(i + 1));
                isfirst = false;
            }
            else
                s.append(";" + QString::number(i + 1));
        }
    }
    emit sendAvailTracks(s);    // send track string to element
}

