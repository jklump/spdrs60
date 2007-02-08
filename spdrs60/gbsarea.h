/**************************************************************************
                           gbsarea.h
                           version 0.5.1 $Revision: 1.39 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-02-08 18:59:57 $
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
#include <qpopupmenu.h>
#include <qptrvector.h>
#include <qtextstream.h>
#include <qtimer.h>

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
   void setLayoutSize(int, int);
   int getColumns();
   int getRows();
   void removeRowElements(int row);
   void removeColumnElements(int col);
   element* item(int row, int col) const;
   QPtrVector<element>* getGbsElementListPtr();
   void sendInfoPortMessage(unsigned int bus,
        unsigned int addr, unsigned int port, unsigned int value);
   bool sendSRCP08BusMessage(SrcpMessage::Message);
   bool setSRCP08BusPower(bool);
   //bool switchSRCP08FBBusState(bool);
   bool runSRCP08GAInitSequence();
   bool hasSrcp08GaBus(unsigned int);
    
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
   QTimer*     delayTimer;

   QPopupMenu* ctxNorm;
   QPopupMenu* ctxEdit;
   QString     lastElementName;
   
   QPtrVector<element> elements;

   int         cols;
   int         rows;

   bool        modified: 1;
   GbsButtonState  gkbState;
   elemVisualMode visualMode;

   // for SRCP 0.8
   int         SRCP08GA1InitWalker;
   int         SRCP08GA2InitWalker;
   int         SRCP08GABusCount;
   int         SRCP08GABusWalker;
   int         *pSRCP08GABusList;
   int         SRCP08FBBusCount;
   int         SRCP08FBBusWalker;
   int         *pSRCP08FBBusList;

   void connectElement(element*);
   void deleteElements();
   void externalButtonClicked(GbsButtonState);
   bool findElement(const QString&, int, int);
   void moveElementToIndexPos(element*, int);
   void updateSRCP08GABusList();
   void updateSRCP08FBBusList();
   void updateSRCP08BusLists();
   
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
    void slotEditFind(const QString&, int, int);
    void startRouteTimer(TypeOfRoute);
    void updateRoutePathLEDs(const stateElement&, const stateElement&,
            RouteSetAction&);
    void getElementByAddress(const int, const int, element**);
    void switchVisualMode(elemVisualMode);

protected:
    int indexOf(int row, int col) const;
    int indexOf(QPoint) const;
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent*);

signals:
    void showLogMessage(const QString&, int, int);
    void switchedVisualMode(elemVisualMode);
    void feedbackPortChanged(unsigned int, unsigned int, bool);
    void sendSrcpMessage(SrcpMessage*);
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
    void processInfoPortMessage(unsigned int bus,
            unsigned int addr, unsigned int port, unsigned int value);
};

#endif  //GBSAREA_H
