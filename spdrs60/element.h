/***************************************************************************
                           element.h
                           version 0.4.7
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
    email                : stefan.preis@wdr.de
    last modified        : 2005-01-17
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
   this is the header file to element.cpp
 ***************************************************************************/

#ifndef ELEMENT_H
#define ELEMENT_H

#include <qfont.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qpopupmenu.h>
#include <qrect.h>
#include <qstring.h>
#include <qtooltip.h>
#include <qwmatrix.h>

#include "resources.h"
#include "elementdialog.h"
#include "elementcommander.h"
#include "turntablecommander.h"


/* Click states of layout internal buttons (group key block, signals,
   turnouts) for every new key, add here a corresponding name */
enum GbsButtonState {
    kNoneClicked = 0,
    kTurnoutClicked,
    kZfsClicked,
    kZhsClicked,
    kRfsClicked,
    kFhtClicked,
    kWgtClicked,
    kUfgtClicked,
    kMgtClicked,
    kFrtClicked};


class element : public QWidget
{
   Q_OBJECT

public:
/*   element(QWidget *parent=0);*/
   element(QStrList *elementData_=0, QWidget *parent=0);

   int  iSoldAddress_1;
   int  iSoldAddress_2;
   QString  sSoldText;
   QString  sSoldDecoder;
   QString  sSoldIcon;
   QString  sRepeatIcon;
   QString  sSoldProtocol;
   QString  sSoldData_2;
   QString  sSoldData_3;
   int      iSoldDirection;
   int      iSoldSubType;
   int      iSoldRotate;
   int      iSoldLocked;
   int      iSoldInvert;
   int      iSoldIndex;
   int      iSoldChangeConn[2];
   int      iSoldFBport;
   int      iSoldActiveTime;
   int      iSoldLEDstate;
   int      iSoldLEDoff;

   QSize sizeHint() const;
   void writeFileTextToStream(QTextStream &);
   int  routeElement(int, int, int);
   void sendState();
   void locateMe();
   QString getName() const;
   bool hasSameAddress(int);
   bool isLocked();

private:
   elementDialog*      elementPropertyDlg;
   elementCommander*   turntableProperties;
   turntableCommander* ttComm;

   QPopupMenu*       ctxNorm;
   QPopupMenu*       ctxEdit;
   int               iSoldRoutingActive;
   int               iEditMode;
   bool              bRecStaStoTimeout;
   QString           sSaveReplaceIcon;
   QTimer           *tRecStaSto;
   QTimer           *locateTimer;

   void addTooltip();
   void clear();
   void copyData(QStrList *);
   void createPopupMenus();
   void makeCommand();
   void openProperties();
   void rotate();
   void setupElementIcon(int, QString);

public slots:
   void slotSwitchIt(int, int);
   void slotToggle();
   void slotOccupyElement(unsigned int);
   void slotEditMode(int);
   void slotRecordMode(int);
   void slotRepeatIcon(QString);
   void slotShowElement(int, int, int);
   void slotRepaintLayout();

private slots:
   void slotLocateTimerTimeout();
   void slotUpdateData();
   void slotUpdateTurntableData(QPoint);
   void slotCopyAvailTracks(QString);
   void slotCtxEdit(int);
   void slotRecStaStoTimeout();

signals:
   void cmdToDebug(const QString&);
   void elementClicked(int, GbsButtonState);
   void sendCommand(const QString&);
   void setRepeatIcon(QString);
   void sigElementClickedRecord(int, int);
   void sigShowFBmodules();

protected:
   virtual void mousePressEvent(QMouseEvent*);
};

#endif  //ELEMENT_H
