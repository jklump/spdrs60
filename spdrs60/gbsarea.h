/**************************************************************************
                           gbsarea.h
                           version 0.4.8 $Revision: 1.16 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 by Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-06-13 20:46:55 $
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
#include "route.h"


#define MAXCONTACTS 496  // maximum contacts per bus for SRCP 0.8

#define GF_GBSEXT      ".spdrs60"
#define GF_DIMENSIONS  "dimensions"


class GBSArea: public QWidget
{
   Q_OBJECT
   Q_PROPERTY(bool modified READ isModified WRITE setModified DESIGNABLE false)

public:
   GBSArea(QWidget* parent = 0, const char* name = 0);
   virtual ~GBSArea();

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
   QPtrVector<element>* getGbsElementListPtr();
   void sendInfoPortMessage(QString prot, int addr, int port, int
           state);
    
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

   QPtrVector<element> elements;

   int         cols;
   int         rows;

   bool        modified: 1;
   GbsButtonState  gkbState;

   void deleteElements();
   void externalButtonClicked(GbsButtonState);
   int  locateIndex(const QString&, int, int);
   void setupElements();
   
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
    void newFile(int, int);
    void slotElementClickedTimeout();
    void slotFHTclicked();
    void slotFRTclicked();
    void slotHaGTclicked();
    void slotMGTclicked();
    void slotSGTclicked();
    void slotUfGTclicked();
    void slotWGTclicked();
    void slotToggleAll();
    void slotSendAll();
    void slotElementClicked(element*, GbsButtonState);
    void slotNotrot();
    void slotEditFind(const QString&, int, bool);
    void startRouteTimer(TypeOfRoute);
    void updateRoutePathLEDs(const stateElement&, const stateElement&,
            RouteSetAction&);

protected:
    int indexOf(int row, int col) const;

signals:
    void showLogMessage(const QString&, int, int);
    void switchVisualMode(elemVisualMode);
    void feedbackPortChanged(unsigned int);
    void sendCommand(const QString&);
    void setRepeatIcon(const QString&);
    void setRoute(element*, GbsButtonState, GbsButtonState);
    void resetRoute(element*, GbsButtonState);
    void resetSelectedSignal();
    void sigRecordElement(int, const QString&, int, int);
    void sigRepaintLayout();
    void sigShowElement(int, int, elemSelectionMode);
    void sigShowFBmodules();
    void updateRoutingViewer(const QString&);
    void clearRoutes();
    void recordElement(element*, elemRecordType);
    void processInfoPortMessage(QString prot, int addr, int port,
                    int state);
};

#endif  //GBSAREA_H
