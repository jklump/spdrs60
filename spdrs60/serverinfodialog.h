/*
 * serverinfodialog.h
 * --------------------
 * Begin        : 2007-08-22
 * Last modified: $Date: 2007-09-04 19:27:47 $
 *                $Revision: 1.2 $
 * Copyright    : (C) 2007 by Guido Scholz
 * E-Mail       : guido.scholz@bayernline.de
 * Description  : Dialog window to display SRCP server information
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SERVERINFODIALOG_H
#define SERVERINFODIALOG_H

#include <qdialog.h>
#include <qlabel.h>

#include "srcpport.h"


class ServerInfoDialog: public QDialog {
    Q_OBJECT
   
public:
    ServerInfoDialog(SrcpPort::CommunicationStyle style = SrcpPort::csNew,
            QWidget* parent = NULL, const char* name = NULL);

    // for new style
    void setCommandSessionData(const QString&, const QString&,
                       const QString&, unsigned int);
    void setInfoSessionData(const QString&, const QString&,
                       const QString&, unsigned int);
    // for old style
    void setOldCommandData(const QString&, const QString&);
    void setOldFeedbackConnected(bool);
    void setOldInfoConnected(bool);

private:
    QLabel* cmdServerLbl;
    QLabel* cmdSrcpLbl;
    QLabel* cmdSrcpOtherLbl;
    QLabel* cmdSessionIdLbl;

    QLabel* infoServerLbl;
    QLabel* infoSrcpLbl;
    QLabel* infoSrcpOtherLbl;
    QLabel* infoSessionIdLbl;

    QLabel* feedbackPortLbl;
    QLabel* infoPortLbl;
};

#endif    //SERVERINFODIALOG_H
