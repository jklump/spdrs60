/****************************************************************************
** $Id: main.cpp,v 1.1 2005-01-26 21:29:31 gscholz Exp $
**
** Copyright (C) 1992-1998 Troll Tech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/* this file is derived from the qt 'aclock' example */

#include "centralclock.h"
#include <qapplication.h>


int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    AnalogClock *clock = new AnalogClock;
    clock->resize(100, 100);
    a.setMainWidget(clock);
    clock->show();
    int result = a.exec();
    delete clock;
    return result;
}

