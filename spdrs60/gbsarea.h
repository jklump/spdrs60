/**************************************************************************
                           gbsarea.h
                           version 0.4.8 $Revision: 1.8 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-05-31 19:54:48 $
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
   this file is the header file to gbsarea.cpp
 **************************************************************************/

#ifndef GBSAREA_H
#define GBSAREA_H

#include <qapplication.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qptrvector.h>
#include <qtextstream.h>
#include <qtimer.h>

#include "resources.h"
#include "element.h"
#include "routedialog.h"

#define NOLOCK  0  // reads routing file without renewing the locked list
#define LOCK    1  // reads routing file and renews the list of locked routes

#define MAXCONTACTS 496  // maximum contacts per bus for SRCP 0.8

#define GF_OLDGBSEXT   ".dat.gbs"
#define GF_GBSEXT      ".spdrs60"
#define GF_DIMENSIONS  "dimensions"
#define GF_CMDHOST     "cmdhost"
#define GF_FBHOST      "fbhost"

class GBSArea: public QWidget
{
   Q_OBJECT
   Q_PROPERTY(bool modified READ isModified WRITE setModified DESIGNABLE false)

public:
   GBSArea(QWidget* parent = 0, const char* name = 0);
   virtual ~GBSArea();

   QString routeFileName; //should be private, is used in routedialog
   
   bool isModified() const;
   virtual void setModified(bool m);
   QSize sizeHint() const;
   void writeFileTextToStream(QTextStream& ts);
   void readFileTextFromStream(QTextStream& ts);
   void readOldFileTextFromStream(QTextStream& ts);
   void setLayoutSize(int, int);
   int getColumns();
   int getRows();
   void removeRowElements(int row);
   void removeColumnElements(int col);
   element* item(int row, int col) const;
   void setRouteFileName(const QString&);
   QString getRouteFileName();
   QPtrVector<element>* getGbsElementListPtr();
    
private:
   QCursor     FHTCursor;
   QCursor     HaGTCursor;
   QCursor     MGTCursor;
   QCursor     RRSCursor;
   QCursor     RZSCursor;
   QCursor     SGTCursor;
   QCursor     UfGTCursor;
   QCursor     URSCursor;
   QCursor     UZSCursor;
   QCursor     WGTCursor;
   QCursor     ZHSCursor;
   QStrList    *listOfActivatePorts;
   QStrList    *listOfFromSignals;
   QStrList    *listOfLockedRoutes;
   QStrList    *listOfReleasePorts;
   QStrList    *listOfRouteTypes;
   QStrList    *listOfToSignals;
   QTimer      *delayTimer;
   RouteDialog *routeWindow;

   QPtrVector<element> elements;

   int         cols;
   int         rows;
   int         iFromSignalIndex;
   int         iToSignalIndex;
   RouteType   searchedRoute;

   QString     cmdHost;
   QString     fbHost;
   int         cmdPort;
   int         fbPort;
   bool        cmdLogin;
   bool        fbLogin;

   int         iLastFoundID;
   GbsButtonState  gkbState;
   bool        bRouteWindowActive;
   bool        bRecord;
   int         iConvertCheck;
   bool        modified: 1;

   void loadRoutes(bool);
   void closeRouteWindow();
   void deleteElements();
   void setupElements();
   void setRoute(int, QStrList*, QStrList*);
   void showLEDs(int, int);
   int  locateIndex(const QString&, int, int);
   void externalButtonClicked(GbsButtonState);
   
/*
void savePixmaps(int ID)
{
 const QPixmap *saveIcon;
 saveIcon=GBSElement[ID]->backgroundPixmap();
 QString fn;
 fn.sprintf("/home/stefan/.AA/spdrs60/resources/save/%s_D%d_R%d.bmp",GBSElement[ID]->sSoldIcon.data(),GBSElement[ID]->iSoldDirection,GBSElement[ID]->iSoldRotate);
 //if(GBSElement[ID]->iSoldDirection != -1)
 saveIcon->save(fn, "BMP");
};
*/
public slots:
    int  newFile(int, int);
    void slotShowRoutings();
    void slotElementClickedTimeout();
    void slotFHTclicked();
    void slotFRTclicked();
    void slotHaGTclicked();
    void slotMGTclicked();
    void slotSGTclicked();
    void slotUfGTclicked();
    void slotWGTclicked();
    void slotUnlockRoutings();
    void slotToggleAll();
    void slotSendAll();
    void slotStartRouting(int, int);
    void slotElementClicked(int, GbsButtonState);
    void slotElementClicked(element*, GbsButtonState);
    void slotFBportChanged(unsigned int);
    void slotNotrot();
    void slotUpdateRouteLists();
    void slotReadElemName(const QString&);
    void slotEditFind(const QString&, int, bool);

protected:
    int indexOf(int row, int col) const;

signals:
    void cmdToDebug(const QString&);
    void switchVisualMode(elemVisualMode);
    void FBportChanged(unsigned int);
    void sendCommand(const QString&);
    void setRepeatIcon(const QString&);
    void setRoute(element*, GbsButtonState, GbsButtonState);
    void resetRoute(element*, GbsButtonState);
    void sigRecordElement(int, const QString&, int, int);
    void sigRecordMode(elemVisualMode);
    void sigRepaintLayout();
    void sigShowElement(int, int, elemSelectionMode);
    void sigShowFBmodules();
    void sigUpdateEditmenu();
    void updateRouteWindow();
    void updateRoutingViewer(const QString&);
    void clearRoutes();
    void recordElement(element*, elemRecordType);
};

#endif  //GBSAREA_H
