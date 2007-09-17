/*
 * externalgrouppanels.h
 * ---------------------------
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mmail      : guido.scholz@bayernline.de
 * Begin        : 2007-09-16
 * Last modified: $Date: 2007-09-17 18:05:45 $
 *                $Revision: 1.2 $
 *
 * This is the header file to externalgrouppanels.cpp
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#ifndef EXTERNALGROUPPANELS_H
#define EXTERNALGROUPPANELS_H

#include <qaction.h>

#include "element.h"
#include "gbsarea.h"


class ExternalGroupPanels: public QActionGroup
{
    Q_OBJECT
        
public:
    ExternalGroupPanels(QObject* parent, const char* name);

signals:
    void paintItemChanged(element::SpdrItemClassId);

public slots:

private slots:
    void spdrItemSelected(QAction*);
    void enablePaintItems(GBSArea::LayoutEditMode);
    //void deselectPaintItem(element::SpdrItemClassId);

private:
    QAction* actionSpdrFeg;
    QAction* actionSpdrTaf;
    QAction* actionSpdrTau;
    QAction* actionSpdrFeb;
    QAction* actionSpdrTaw;
    QAction* actionSpdrFer;
    QAction* actionSpdrTas;
    QAction* actionSpdrFey;
    QAction* actionSpdrFen;
    QAction* actionSpdrFee;

protected:

};

#endif //EXTERNALGROUPPANELS_H
