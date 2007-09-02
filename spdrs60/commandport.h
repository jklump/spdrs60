/***************************************************************************
 commandport.h
 ------------
 Begin        : 17.08.2007
 Last modified: $Date: 2007-09-02 17:21:42 $
                $Revision: 1.1 $
 Copyright    : (C) 2007 by Guido Scholz <guido.scholz@bayernline.de>
 Description  : Class for command port network communication with a
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

#ifndef COMMANDPORT_H
#define COMMANDPORT_H

#include "srcpport.h"


class CommandPort: public SrcpPort {
  Q_OBJECT
 
public:
    CommandPort(QObject* parent = 0, const char * name = 0,
            const char* host = "localhost", unsigned int port = 4303,
            CommunicationStyle commstyle = csBoth, bool translate = false);

protected:
    QString getConnectionMode();
};

#endif   //COMMANDPORT_H
