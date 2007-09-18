/*
 * panelactiongroup.h
 * ---------------------------
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-18
 * Last modified: $Date: 2007-09-18 17:07:16 $
 *                $Revision: 1.1 $
 *
 * This is the header file to panelactiongroup.cpp
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#ifndef PANELACTIONGROUP_H
#define PANELACTIONGROUP_H

#include <qaction.h>

#include "element.h"
#include "gbsarea.h"


class PanelActionGroup: public QActionGroup
{
    Q_OBJECT
        
public:
    PanelActionGroup(QObject* parent, const char* name);

signals:
    void paintItemChanged(element::SpdrItemClassId);

public slots:

private slots:
    void spdrItemSelected(QAction*);
    void enablePaintItems(GBSArea::LayoutEditMode);
    //void deselectPaintItem(element::SpdrItemClassId);

private:

protected:

};

#endif //PANELACTIONGROUP_H
