/*
 * miscpanels.h
 * ---------------------------
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-22
 * Last modified: $Date: 2007-09-22 15:27:32 $
 *                $Revision: 1.1 $
 *
 * This is the header file to miscpanels.cpp
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#ifndef MISCPANELS_H
#define MISCPANELS_H

#include <panelactiongroup.h>


class MiscPanels: public PanelActionGroup
{
    Q_OBJECT
        
public:
    MiscPanels(QObject* parent, const char* name);

};

#endif //MISCPANELS_H
