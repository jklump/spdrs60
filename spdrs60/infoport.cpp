/***************************************************************************
 infoport.cpp
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

#include "infoport.h"



InfoPort::InfoPort(QObject* parent, const char* name,
        const char* host, unsigned int port,
        CommunicationStyle commstyle, bool translate):
    SrcpPort(parent, name, host, port, commstyle, translate)
{
}


QString InfoPort::getConnectionMode()
{
    return QString("INFO");
}

/*
 * return the initial communication style, allowed csOld, csNew
 * SRCP 0.7 needs csOld to switch off server login for info and feedback
 * ports
 */
int InfoPort::getInitialStyle()
{
    return commStyle;
}

