/****************************************************************************
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

/* The original file is aclock.cpp from the aclock example
** modified by Stefan Preis <spdrs60@linux-modellbahn.de>
** and Guido Scholz <guido.scholz@bayernline.de> (second hand bug fixes)
** last update: 2004-12-31
*/

/*for roundf()*/
#ifndef _ISOC99_SOURCE
#define _ISOC99_SOURCE 1
#endif

#include <math.h>
#include "centralclock.h"



AnalogClock::AnalogClock(QWidget *parent, const char *name)
    : QWidget(parent, name)
{
    time = QTime::currentTime();		// get current time
    QTimer *secondTimer = new QTimer(this);	// create timer
    connect(secondTimer, SIGNAL(timeout()), SLOT(timeout()));
    secondTimer->start(100);		        // emit signal every second

    setBackgroundColor(Qt::white);
    setMinimumWidth (110);
    setMinimumHeight(110);
}


void AnalogClock::timeout()
{
    QTime new_time = QTime::currentTime();	// get the current time
    if (new_time.second() != time.second())	// second has changed
      update();
}


void AnalogClock::paintEvent(QPaintEvent *)	// paint clock
{
    if (!isVisible())				// is it invisible
	return;
    time = QTime::currentTime();		// save current time

    QPointArray pts;
    QPainter paint(this);
    paint.setBrush(Qt::black);	                // set color for tickmarks
    paint.setPen(Qt::black);	                // and hands

    QPoint cp = rect().center();		// widget center point
    int d = QMIN(width(), height());		// we want a circular clock

    QWMatrix matrix;				// setup transformation matrix
    matrix.translate(cp.x(), cp.y());		// origin at widget center
    matrix.scale(d / 1100.0F, d / 1100.0F);	// scale coordinate system

    for (int i = 0; i < 60; i++) {		// draw hour lines
	paint.setWorldMatrix(matrix);
	
	if(i % 5 == 0)
 	   paint.drawRect(380, -15, 120, 30);   // 5 minute ticks
	else
	   paint.drawRect(460, -10, 40, 20);    // minute ticks
	matrix.rotate(6);
    }

    // draw hour hand
    float h_angle = 30 * (time.hour() % 12 - 3) + time.minute() / 2;
    matrix.rotate(h_angle);			// rotate to draw hour hand
    paint.setWorldMatrix(matrix);
    pts.setPoints(5, -40, -15, 355, -15, 370, 0, 355, 15, -40, 15);
    paint.drawPolygon(pts);			
    matrix.rotate(-h_angle);			// rotate back to zero

    // draw minute hand
    float m_angle = (time.minute() - 15) * 6;
    matrix.rotate(m_angle);			// rotate to draw minute hand
    paint.setWorldMatrix(matrix);
    pts.setPoints(5, -40, -10, 435, -10, 450, 0, 435, 10, -40, 10);
    paint.drawPolygon(pts);			
    matrix.rotate(-m_angle);			// rotate back to zero

    // draw second hand
    paint.setBrush(Qt::red);
    paint.setPen(Qt::red);	        // fill with foreground color
    float s_angle = (time.second() - 15) * 6;
    matrix.rotate(s_angle);			// rotate to draw second hand
    paint.setWorldMatrix(matrix);
    pts.setPoints(4, -54, -4, 490, -2, 490, 2, -54, 4);
    paint.drawPolygon(pts);			// draw second hand

    // draw second hand axis + circle
    paint.drawEllipse(-10, -10, 20, 20);
    // draw second hand respecting overlapping with minute
    // _and_ hour hand (guido)
    if (time.second() != time.minute() &&
            time.second() != 5 * (time.hour() % 12) +
            roundf((float)time.minute() / 12.0))
        paint.setBrush(Qt::white);
    else
        paint.setBrush(Qt::black);
    paint.setPen(QPen(Qt::red, 2, Qt::SolidLine));
    paint.drawEllipse(300, -25, 50, 50);
    matrix.rotate(-s_angle);			// rotate back to zero
}

