/*
 * curvedtrackpanels.h
 * ---------------------------
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-16
 * Last modified: $Date: 2007-09-18 20:04:42 $
 *                $Revision: 1.1 $
 *
 * This is the header file to curvedtrackpanels.cpp
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#ifndef CURVEDTRACKPANELS_H
#define CURVEDTRACKPANELS_H

#include <panelactiongroup.h>


class CurvedTrackPanels: public PanelActionGroup
{
    Q_OBJECT
        
public:
    CurvedTrackPanels(QObject* parent, const char* name);

};

#endif //CURVEDTRACKPANELS_H
