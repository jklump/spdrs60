/***************************************************************************
                           turntablecommander.h
                           version 0.5.6 $Revision: 1.12 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2009 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2009-10-27 18:50:31 $
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


class turntableCommander: public QDialog
{
   Q_OBJECT

   bool          bStepMode;            //
   int           currenttrack;         // number of active track
   int           targettrack;            // number of selected new track
   int           iTracks[24];          // list of available tracks ( == 1 )
   int           iTotalProgTracks;     // total number of available tracks
   bool          bStep;                //
   bool          bDir;                 // saves rotating direction

   QButtonGroup *bgProg;
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

   void activateUsageButtons(bool);    // (de)activates normal buttons
                                       // while programming
   void startTrackTimer();             // starts track displaying timer
   void storeTrackPositions();         // sends new programmed tracks to
                                       // element
   void displayTracks();               // displays all tracks

public:
   turntableCommander(const QString&, QWidget* parent=0, int activetrack = 0);

private slots:
   void slotSaveNewTrack(const QString&);   // saves selected new track
   void slotGoToTrack();                    // goes to selected track
   void slotTurn180();                      // turns bridge 180 degrees
   void slotChooseDir(int);                 // changes rotating direction
   void slotLeftStep();                     //
   void slotRightStep();                    //
   void slotStopCont();                     // stop rotating
   void enableProgramming(bool);            // resizes window
   void slotProgrammer(int);                //
   void slotTrackReached();                 // called by timer if a new track is
                                            // reached
signals:
   void sendTtCommand(int, int);            // send turn table control command
   void trackPositionsChanged(const QString&);    // send track string to element
};

#endif
