/*
 * layouteditmodeagrp.h
 * ---------------------------
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mmail      : guido.scholz@bayernline.de
 * Begin        : 2007-09-12
 * Last modified: $Date: 2007-09-15 10:54:47 $
 *                $Revision: 1.2 $
 *
 * This is the header file to layouteditmodeagrp.cpp
 */

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/


#ifndef LAYOUTEDITMODEAGRP_H
#define LAYOUTEDITMODEAGRP_H

#include <qaction.h>

#include "gbsarea.h"


class LayoutEditModeAgrp: public QActionGroup
{
    Q_OBJECT
        
public:
    LayoutEditModeAgrp(QObject* parent, const char* name);

signals:
    void modeChanged(GBSArea::LayoutEditMode);

public slots:

private slots:
    void modeSelected(QAction*);
    void enableEditMode(bool);

    
private:
    QAction* actionSelectMode;
    QAction* actionPaintMode;
    QAction* actionEraseMode;

protected:

};

#endif //LAYOUTEDITMODEAGRP_H
