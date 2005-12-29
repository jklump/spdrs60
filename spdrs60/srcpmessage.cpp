/***************************************************************************
                           srcpmessage.cpp
                           version 0.5.0 $Revision: 1.1 $
                           -------------------------------
    copyright            : (C) 2005 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2005-12-29 21:41:18 $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
    this code implements a class wrapping srcp commands
 ***************************************************************************/

#include "srcpmessage.h"



SrcpMessage::SrcpMessage(Command cmd)
{
   address = 0;
   cmdmode = true;
   command = cmd;
   delay = 0;
   port = 0;
   power = false;
   protocol = proMM;
   srcpbus = 0;
}


QString SrcpMessage::getSrcpMessageStr(unsigned int version)
{
    QString cmdStr = "";

    // SRCP 0.7
    if (7 == version)

        switch(command) {
            case cmdGetPower:
                cmdStr = "GET POWER";
                break;
            case cmdInitFb:
                cmdStr = "INIT FB S88";
                //TODO: I8255
                break;
            case cmdLogout:
                cmdStr = "LOGOUT";
                break;
            case cmdReset:
                cmdStr = "RESET";
                break;
            case cmdSetGa:
                cmdStr = QString("SET GA %1 %2 %3 1 %4")
                    .arg(protocol).arg(address).arg(port).arg(delay);
                break;
            case cmdShutdown:
                cmdStr = "SHUTDOWN";
                break;
            case cmdSetPower:
                cmdStr = QString("SET POWER %1").arg(power ? "ON" : "OFF");
                break;
            case cmdNoCmd:
            default:
                break;
        }

    // SRCP 0.8
    else
        switch(command) {
            case cmdGetPower:
                cmdStr = QString("GET %1 POWER").arg(srcpbus);
                break;
            case cmdInitFb:
                cmdStr = QString("INIT %1 FB").arg(srcpbus);
                break;
            case cmdReset:
                cmdStr = "RESET 0 SERVER";
                break;
            case cmdSetConnectionMode:
                cmdStr = QString("SET CONNECTIONMODE SRCP %1")
                    .arg(cmdmode ? "COMMAND" : "INFO");
                break;
            case cmdSetGa:
                cmdStr = QString("SET %1 GA %2 %3 1 %4")
                    .arg(srcpbus).arg(address).arg(port).arg(delay);
                break;
            case cmdShutdown:
                cmdStr = "TERM 0 SERVER";
                break;
            case cmdSetPower:
                cmdStr = QString("SET %1 POWER %2").arg(srcpbus)
                    .arg(power ? "ON" :"OFF");
                break;
            case cmdTermFb:
                cmdStr = QString("TERM %1 FB").arg(srcpbus);
                break;
            case cmdNoCmd:
            default:
                break;
        }

    return cmdStr;
}


void SrcpMessage::setAddress(unsigned int adr)
{
    address = adr;
}


void SrcpMessage::setBus(unsigned int bus)
{
    srcpbus = bus;
}


void SrcpMessage::setConnectionModeCmd(bool cmode)
{
    cmdmode = cmode;
}


void SrcpMessage::setCommand(Command cmd)
{
    command = cmd;
}


void SrcpMessage::setDelay(unsigned int dly)
{
    delay = dly;
}


void SrcpMessage::setPort(unsigned int prt)
{
    port = prt;
}


void SrcpMessage::setPower(bool pwr)
{
    power = pwr;
}


void SrcpMessage::setProtocol(Protocol pro)
{
    protocol = pro;
}

