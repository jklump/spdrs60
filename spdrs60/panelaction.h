/*
 * panelaction.h
 * ---------------------------
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Begin        : 2007-09-18
 * Last modified: $Date: 2007-09-18 17:07:16 $
 *                $Revision: 1.1 $
 *
 * This is the header file to panelaction.cpp
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#ifndef PANELACTION_H
#define PANELACTION_H

#include <qaction.h>

#include "element.h"


class PanelAction: public QAction
{
    Q_OBJECT
        
public:
    PanelAction(const QIconSet& icon, const QString&, QKeySequence,
            element::SpdrItemClassId si = element::siciNone,
            QObject* parent = NULL, const char * name = 0);
    element::SpdrItemClassId Sici();

signals:

public slots:

private slots:

private:
    element::SpdrItemClassId sici;

protected:

};

#endif //PANELACTION_H
