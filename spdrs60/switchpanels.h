/*
 * switchpanels.h
 * ---------------------------
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-21
 * Last modified: $Date: 2007-09-22 08:05:17 $
 *                $Revision: 1.1 $
 *
 * This is the header file to switchpanels.cpp
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#ifndef SWITCHPANELS_H
#define SWITCHPANELS_H

#include <panelactiongroup.h>


class SwitchPanels: public PanelActionGroup
{
    Q_OBJECT
        
public:
    SwitchPanels(QObject* parent, const char* name);

};

#endif //SWITCHPANELS_H
