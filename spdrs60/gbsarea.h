/**************************************************************************
                           gbsarea.h
                           version 0.5.3 $Revision: 1.54 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2008 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-11-05 08:42:40 $
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

#if QT_VERSION >= 0x040000
#include <QBitmap>
#endif

#include <qapplication.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qmap.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qptrvector.h>
#include <qtextstream.h>
#include <qtimer.h>

#include "element.h"
#include "route.h"

// maximum contacts per bus for SRCP 0.8
static const int MAXCONTACTS = 496;

static const char GF_GBSEXT[]     = ".spdrs60";
static const char GF_DIMENSIONS[] = "dimensions";
static const char GF_ID[]         = "identification";
static const char GF_TABLELIGHT[] = "tablelight";


class GBSArea: public QWidget
{
   Q_OBJECT
   Q_PROPERTY(bool modified READ isModified WRITE setModified DESIGNABLE false)

public:
   enum LayoutEditMode {lemSelect = 0, lemPaint, lemErase};

   GBSArea(QWidget* parent = 0, const char* name = 0);
   virtual ~GBSArea();

   bool isModified() const;
   void setModified(bool);
   QSize sizeHint() const;
   void writeFileTextToStream(QTextStream&);
   void readFileTextFromStream(QTextStream&);
   unsigned int getLayoutId();
   void setLayoutId(unsigned int);
   QString getLayoutName() const;
   void setLayoutName(const QString&);
   int getColumns();
   int getRows();
   void setLayoutSize(int, int);
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
   void processGenericMessage(unsigned int, unsigned int,
           const CrcfMessage*);
    
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
   QCursor     paintCursor;
   QCursor     eraseCursor;
   QTimer*     delayTimer;

   QPopupMenu* ctxNorm;
   
   QPtrVector<element> elements;

   int         cols;
   int         rows;
   unsigned int layoutid;
   QString      layoutname;

   bool        tablelight;
   bool        modified: 1;
   GbsButtonState  gkbState;
   elemVisualMode visualMode;
   LayoutEditMode lyeditMode;
   element::SpdrItemClassId paintItem;

   // for SRCP 0.8
   unsigned int SRCP08GA1InitWalker;
   unsigned int SRCP08GA2InitWalker;
   unsigned int SRCP08GABusCount;
   unsigned int SRCP08GABusWalker;
   unsigned int *pSRCP08GABusList;
   unsigned int SRCP08FBBusCount;
   unsigned int SRCP08FBBusWalker;
   unsigned int *pSRCP08FBBusList;

   void connectElement(element*);
   void externalButtonClicked(GbsButtonState);
   bool findElement(const QString&, int, int);
   void moveElementToIndexPos(element*, unsigned int);
   void updateSRCP08GABusList();
   void updateSRCP08FBBusList();
   void updateSRCP08BusLists();
   void sendGmCrcfMessage(unsigned int, unsigned int, const QString&);
   QString getCrcfInfoMessage(CrcfMessage::CrcfAttribute) const;
   void switchTableLight(bool);
   
public slots:
    void newFile(int, int, unsigned int, const QString&);
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
    void startRouteTimer(Route::RouteType);
    void updateRoutePathLEDs(const stateElement&, const stateElement&,
            Route::RouteSetAction&);
    void getElementByAddress(const int, const int, element**);
    void switchVisualMode(elemVisualMode);
    void changeLayoutEditMode(GBSArea::LayoutEditMode);
    void changeLayoutPaintItem(element::SpdrItemClassId);

protected:
    bool dragging;
    bool erasing;
    bool painting;
    unsigned int indexOf(int row, int col) const;
    unsigned int indexOf(QPoint) const;
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent*);
    void dragEnterEvent(QDragEnterEvent* e);
    void dropEvent(QDropEvent *);

signals:
    void statusMessage(const QString&);
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
