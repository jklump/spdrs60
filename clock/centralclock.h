#ifndef CENTRALCLOCK_H
#define CENTRALCLOCK_H

#include <qwidget.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qapplication.h>


class AnalogClock : public QWidget		// analog clock widget
{
    Q_OBJECT
public:
    AnalogClock(QWidget *parent=0, const char *name=0);

protected:
    void paintEvent(QPaintEvent *);

private slots:
    void timeout();

private:
    QTime time;
};

#endif // CENTRALCLOCK_H

