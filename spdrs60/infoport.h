/***************************************************************************
 infoport.h
 ------------
 Begin        : 17.08.2007
 Last modified: $Date: 2007-09-02 20:51:35 $
                $Revision: 1.2 $
 Copyright    : (C) 2007 by Guido Scholz <guido.scholz@bayernline.de>
 Description  : Class for info port network communication with a
                SRCP 0.8 server.
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INFOPORT_H
#define INFOPORT_H

#include "srcpport.h"


class InfoPort: public SrcpPort {
  Q_OBJECT
   
public:
    InfoPort(QObject* parent = 0, const char * name = 0,
            const char* host = "localhost", unsigned int port = 4303,
            CommunicationStyle commstyle = csBoth, bool translate = false);

protected:
    QString getConnectionMode();
    int getInitialStyle();
};

#endif                          //INFOPORT_H
