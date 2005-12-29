/***************************************************************************
                           srcpmessage.h
                           version 0.5.0 $Revision: 1.1 $
                           -------------------------------
    copyright            : (C) 2005 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2005-12-29 21:41:18 $
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
   this is the headerfile for srcpmessage.cpp
 ***************************************************************************/

#ifndef SRCPMESSAGE_H
#define SRCPMESSAGE_H

#include <qstring.h>


class SrcpMessage
{
    public:
        enum Command {cmdNoCmd = 0, cmdGetPower, cmdGo, cmdInitFb,
            cmdLogout, cmdReset, cmdShutdown, cmdSetConnectionMode,
            cmdSetGa, cmdSetPower, cmdTermFb};
        enum Protocol {proMM = 0, proDCC, proSel, proServ};

        SrcpMessage(Command = cmdNoCmd);
        QString getSrcpMessageStr(unsigned int version = 7);
        void setAddress(unsigned int);
        void setBus(unsigned int);
        void setCommand(Command);
        void setConnectionModeCmd(bool);
        void setDelay(unsigned int);
        void setPort(unsigned int);
        void setPower(bool);
        void setProtocol(Protocol);

    private:
        Command command;
        Protocol protocol;
        bool cmdmode;
        bool power;
        unsigned int address;
        unsigned int delay;
        unsigned int port;
        unsigned int srcpbus;
};
#endif

