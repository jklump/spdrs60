/***************************************************************************
                           feedbacktriggerdialog.h
                           -------------------------------
    copyright            : (C) 2010 by Guido Scholz
    e-mail               : guido.scholz@bayernline.de
    last modified        : $Date: 2008/11/05 08:42:40 $
                           $Revision: 1.24 $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
   this is the header file to elementlabeldialog.cpp
 ***************************************************************************/

#ifndef FEEDBACKTRIGGERDIALOG_H
#define FEEDBACKTRIGGERDIALOG_H

#include <qcheckbox.h>
#include <qdialog.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qspinbox.h>


class FeedbackTriggerDialog: public QDialog
{
   Q_OBJECT

   QCheckBox* enableTriggerCB;
   QLineEdit* fbBusLE;
   QLineEdit* moduleLE;
   QLineEdit* portLE;
   QSpinBox* contactSB;
   QGroupBox* feedbackGB;

public:
   FeedbackTriggerDialog(QWidget* parent = 0);
   bool isTriggerEnabled();
   void enableTrigger(bool);
   int getFBBus();
   void setFBBus(int);
   int getFBContact();
   void setFBContact(int);
   void setCheckBoxText(const QString&);

private slots:
   void contactSBChanged(int);
   void enableFeedbackGroupBox();
   void slotShowFBmodules();

signals:
   void sigShowFBmodules();

};

#endif    //FEEDBACKTRIGGERDIALOG_H
