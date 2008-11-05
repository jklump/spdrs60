/***************************************************************************
                           preferences.h
                           version 0.5.3 $Revision: 1.12 $
                           -------------------------------
    copyright            : (C) 2006-2008 Guido Scholz
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
   this file defines the preferences struct for the main application
 **************************************************************************/

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <qstring.h>

struct BusModules {
    unsigned int number;
    unsigned int modules;
};

struct Preferences {
    unsigned int layoutcols;
    unsigned int layoutrows;
    bool hp2;
    bool blinkingturnouts;
    bool sendstate;
    bool datatooltips;
    bool addresslabeling;
    bool initsignalsred;
    bool converttime;
    bool showtime;
    bool autoload;
    QString autolayout;
    bool autosave;
    QString editor;
    QString browser;
    unsigned int protocol;
    QString decoder;
    bool autottdir;
    unsigned int activetime;
    unsigned int routingtime;
    double ttroundtime;
    unsigned int fbfactor;
    int fbmoduletype;
    bool fixedbusnum;
    BusModules fbbus1;
    BusModules fbbus2;
    BusModules fbbus3;
    BusModules fbbus4;
    int keyboardprot;
    bool gmbroadcastroutestate;
    bool gmbroadcasttrainnumber;
};

extern Preferences pref;

#endif    // PREFERENCES_H
