/*
 * decopanels.h
 * ---------------------------
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-22
 * Last modified: $Date: 2007-09-22 14:25:35 $
 *                $Revision: 1.1 $
 *
 * This is the header file to decopanels.cpp
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#ifndef DECOPANELS_H
#define DECOPANELS_H

#include <panelactiongroup.h>


class DecoPanels: public PanelActionGroup
{
    Q_OBJECT
        
public:
    DecoPanels(QObject* parent, const char* name);

};

#endif //DECOPANELS_H
