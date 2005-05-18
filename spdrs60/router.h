/***************************************************************************
                           router.h
                           version 0.4.8 $Revision: 1.1 $
                           -------------------------------
    copyright            : (C) 2004-2005 by Guido Scholz
    email                : guido.scholz@ bayernline.de
    last modified        : $Date: 2005-05-18 21:15:02 $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
  This code implements the router object. This object cares for the routing
  list, loads it from file, save it to file, shows routing edit window.
 ***************************************************************************/

#ifndef ROUTER_H
#define ROUTER_H

#include <qptrlist.h>
#include <qptrvector.h>

#include "route.h"
#include "element.h"

#define RF_OLDROUTEEXT ".dat.rts"
#define RF_ROUTEEXT ".routes"


class Router: public QObject
{
    Q_OBJECT
        
public:
    Router(QObject* parent=0, const char* name=0);
    Router(QObject* parent=0, QPtrVector<element>* elPtr=0, const char* name=0);
    ~Router();
    void importFile(const QString&);
    void readFileTextFromStream(QTextStream&);
    void writeFileTextToStream(QTextStream&);
    unsigned int getRouteCount();
    unsigned int addNewRoute();
    Route* getRouteAt(unsigned int);
    void deleteRouteAt(unsigned int);
    void copyRouteAt(unsigned int);
    bool isModified();
    void clear();
    void setElementListPtr(QPtrVector<element>*);
    void selectedRouteChanged(int, int);
    void showRouteAt(int);
    void setViewRouteMode(bool);
    void startRecordModeAt(unsigned int);
    void stopRecordMode();

public slots:
    void clearRoutes();
    void recordElement(element*, elemRecordType);
    
private:
    QPtrVector<element>* gbsElements;
    QPtrList<Route> routeList;
    Route* recRoute;
    bool modified;
    bool routeviewmode;
    bool recording;
    void setupRouteElements();
    
signals:
    void updateRoutingViewer();
    void updateRoutingViewerAt(int);

};
#endif // ROUTER_H

