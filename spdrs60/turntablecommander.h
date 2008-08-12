/***************************************************************************
                           turntablecommander.h
                           version 0.5.2 $Revision: 1.8 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2007 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-08-12 17:58:27 $
***************************************************************************/

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/

/**************************************************************************
   this is the header file to turntablecommander.cpp
 **************************************************************************/

#ifndef TURNTABLECOMMANDER_H
#define TURNTABLECOMMANDER_H

#include <qbuttongroup.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qtooltip.h>

// shortcuts for commands equal keys on maerklin keyboard
// key numbers from 1 to 24, 0 is red button, 1 is green button
#define KEY_LFT 3, 1
#define KEY_RGT 3, 0
#define KEY_END 1, 0
#define KEY_INP 1, 1
#define KEY_CLR 2, 0
#define KEY_TRN 2, 1


class turntableCommander: public QDialog
{
   Q_OBJECT

public:
   turntableCommander(QWidget* parent=0, int iActiveTrack_=0,
                      QString sTracks_="" ); // creator of commander gui

private:
   void activateUsageButtons(bool);         // (de)activates normal buttons
                                            // while programming
   void setupProgArea();                    // sets up the programming area
   void startTrackTimer();                  // starts track displaying timer
   void buildCommand(int, int);             // build command and sends it to
                                            // element
   void sendTracks();                       // sends new programmed tracks to
                                            // element
   void displayTracks();                    // displays all tracks

private slots:
   void slotSaveNewTrack(const QString&);   // saves selected new track
   void slotGoToTrack();                    // goes to selected track
   void slotTurn180();                      // turns bridge 180 degrees
   void slotChooseDir(int);                 // changes rotating direction
   void slotLeftStep();                     //
   void slotRightStep();                    //
   void slotStopCont();                     // stop rotating
   void slotResizeCommander(bool);          // resizes window
   void slotProgrammer(int);                //
   void slotTrackReached();                 // called by timer if a new track is
                                            // reached
signals:
   void applyPressed(QPoint&);               // send command to element
   void sendAvailTracks(const QString&);    // send track string to element

private:
   QButtonGroup* bgChooseDir;          // button group for prog buttons
   QPushButton*  buttLeftStep;         // button to step one track to the left
   QPushButton*  buttChooseLeft;       // button to chose anti-clockwise rotat.
   QPushButton*  buttRightStep;        // button to step one track to the right
   QPushButton*  buttChooseRight;      // button to chose anti-clockwise rotat.
   QPushButton*  buttGoToTrack;        // button to go to a certain track
   QPushButton*  buttStopCont;         // button to stop rotating
   QPushButton*  buttTurn180;          // button to turn 180 degrees
   QPushButton*  buttSetup;            // button to start setup mode
   QPushButton*  buttInput;            // button to start programming
   QPushButton*  buttSave;             // button to save position #1
   QPushButton*  buttAddPos;           // button to add a position
   QPushButton*  buttEnd;              // button to end programming
   QLabel*       labelTracks[24];      //
   QListBox*     listTracks;           // drop-down list of available tracks
   QFrame*       line;                 // just a separator line
   QPixmap       pixButton;            // pixmap for different buttons
   QTimer*       tTrackReached;        // timer for a virtual correct track
                                       // position display
   int           iNewTrack;            // number of selected new track
   int           iActiveTrack;         // number of active track
   int           iTracks[24];          // list of available tracks ( == 1 )
   int           iTotalProgTracks;     // total number of available tracks
   bool          bStepMode;            //
   bool          bStep;                //
   bool          bDir;                 // saves rotating direction
};

#endif
