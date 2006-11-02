/***************************************************************************
                           routedialog.h
                           version 0.5.0 $Revision: 1.17 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2006 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2006-11-02 16:54:32 $
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
   this is the header file to routedialog.cpp
 ***************************************************************************/
#ifndef ROUTEDIALOG_H
#define ROUTEDIALOG_H

#include <qbuttongroup.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qstring.h>
#include <qspinbox.h>
#include <qtooltip.h>

#include "element.h"
#include "route.h"


enum RouteType {
    kNormal = 0,
    kDetour,
    kHelp,
    kShunting,
    kShuntingD};


class RouteDialog: public QDialog
{
    Q_OBJECT

public:
    RouteDialog(QWidget* parent = 0);
    void setRouteName(const QString&);
    void setRouteType(int, unsigned int);
    void setEntrySignalData(const stateElement&);
    void setExitSignalData(const stateElement&);
    void setActivateData(const PortState&);
    void setReleaseData(const PortState&);
    QString getRouteName();
    void getEntrySignalData(stateElement&);
    void getExitSignalData(stateElement&);
    void getActivateData(PortState&);
    void getReleaseData(PortState&);
    int getRouteType();
    unsigned int getDetourLevel();
    void setRouteElements(const QPtrList<stateElement>&);
    void getRouteElements(QPtrList<stateElement>&);
  
public slots:

private slots:
    void activateCBchanged(bool);
    void releaseCBchanged(bool);
    void activateContactSBChanged(int);
    void releaseContactSBChanged(int);
    void typeBGPressed(int);
    void elementsLVChanged(QListViewItem*);
    void removeElementFromList();
    void startSignalBusChanged(const QString&);
    void startSignalAddressChanged(const QString&);
    void stopSignalBusChanged(const QString&);
    void stopSignalAddressChanged(const QString&);
    void upListElement();
    void downListElement();
    void editListElement();
    void addElementToList();

signals:
    void showLogMessage(const QString&, int, int);
    void getElementByAddress(const int, const int, element**);

protected:

private:
   element* startSignalElPtr;
   element* stopSignalElPtr;
    
   QLineEdit*    routeNameLE;

   QLineEdit*    startSignalNameLE;
   QLineEdit*    startSignalSrcpBusLE;
   QLineEdit*    startSignalAddressLE;
   QSpinBox*     startSignalStateSB;
   
   QLineEdit*    stopSignalNameLE;
   QLineEdit*    stopSignalSrcpBusLE;
   QLineEdit*    stopSignalAddressLE;
   
   QCheckBox*    activatefbCB;
   QLabel*       activateSrcpBusLB;
   QLineEdit*    activateSrcpBusLE;
   QLabel*       activateContactLB;
   QSpinBox*     activateContactSB;
   QLabel*       activateModuleLB;
   QLineEdit*    activateModuleLE;
   QLabel*       activatePortLB;
   QLineEdit*    activatePortLE;

   QCheckBox*    releasefbCB;
   QLabel*       releaseSrcpBusLB;
   QLineEdit*    releaseSrcpBusLE;
   QLabel*       releaseContactLB;
   QSpinBox*     releaseContactSB;
   QLabel*       releaseModuleLB;
   QLineEdit*    releaseModuleLE;
   QLabel*       releasePortLB;
   QLineEdit*    releasePortLE;

   QButtonGroup* typeBG;
   QButtonGroup* activateRouteBG;
   QButtonGroup* releaseRouteBG;

   QListView*    elementsLV;

   QPushButton*  upPB;
   QPushButton*  downPB;
   QPushButton*  editPB;
   QPushButton*  addPB;
   QPushButton*  removePB;

   QSpinBox*     uzsLevelSB;
   QSpinBox*     ursLevelSB;
   
   void updateEntrySignalName(int, int);
   void updateExitSignalName(int, int);
   void updateListIndexNumbersFrom(QListViewItem*);
};

#endif    //ROUTEDIALOG_H
