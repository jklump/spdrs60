/***************************************************************************
                           srcpmessage.cpp
                           version 0.5.0 $Revision: 1.8 $
                           -------------------------------
    copyright            : (C) 2005 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2006-02-08 20:22:40 $
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
    This code implements a class for SRCP messages which can be
    exchanged between a SRCP-data containing class and the SRCP protocol
    class. It implements a method to translate binary data to SRCP-
    protocol dependend command strings. 
 ***************************************************************************/

#include "srcpmessage.h"



SrcpMessage::SrcpMessage(Message msg)
{
   address = 0;
   message = msg;
   delay = 0;
   fbport = 0;
   fbtype = fbS88;
   port = 0;
   power = false;
   protocol = proMM;
   srcpbus = 0;
}


QString SrcpMessage::getSrcpMessageStr(unsigned int version) const
{
    QString cmdStr = "";

    // SRCP 0.7
    if (7 == version)

        switch(message) {
            case msgFbGet:
                if (fbport == 0)
                    cmdStr = "GET FB *";
                else
                    cmdStr = QString("GET FB %1")
                        .arg((srcpbus - 1) * 496 + fbport);
                break;
            case msgFbInit:
                switch(fbtype) {
                    case fbS88:
                        cmdStr = "INIT FB S88";
                        break;
                    case fbI8255:
                        cmdStr = "INIT FB I8255";
                        break;
                    case fbM6051:
                        cmdStr = "INIT FB M6051";
                        break;
                    case fbPS:
                        cmdStr = "INIT FB PS";
                        break;
                }
                break;
            case msgGaGet:
                cmdStr = QString("GET GA %1 %2 %3")
                    .arg(getProtocolStr(protocol)).arg(address).arg(port);
                break;
            case msgGaSet:
                cmdStr = QString("SET GA %1 %2 %3 1 %4")
                    .arg(getProtocolStr(protocol)).arg(address)
                    .arg(port).arg(delay);
                break;
                //TODO: msgGl...
            case msgPowerGet:
                cmdStr = "GET POWER";
                break;
            case msgPowerSet:
                cmdStr = QString("SET POWER %1").arg(power ? "ON" : "OFF");
                break;
            case msgServerLogout:
                cmdStr = "LOGOUT";
                break;
            case msgServerReset:
                cmdStr = "RESET";
                break;
            case msgServerShutdown:
                cmdStr = "SHUTDOWN";
                break;
            case msgNoMsg:
            default:
                break;
        }

    // SRCP 0.8
    else
        switch(message) {
            case msgFbGet:
                cmdStr = QString("GET %1 FB %2").arg(srcpbus).arg(fbport);
                break;
            case msgFbInit:
                cmdStr = QString("INIT %1 FB").arg(srcpbus);
                break;
            case msgFbTerm:
                cmdStr = QString("TERM %1 FB").arg(srcpbus);
                break;
            case msgGaGet:
                cmdStr = QString("GET %1 GA %2 %3")
                    .arg(srcpbus).arg(address).arg(port);
                break;
            case msgGaInit:
                cmdStr = QString("INIT %1 GA %2 %3")
                    .arg(srcpbus).arg(address).arg(getProtocolStr(protocol));
                break;
            case msgGaSet:
                cmdStr = QString("SET %1 GA %2 %3 1 %4")
                    .arg(srcpbus).arg(address).arg(port).arg(delay);
                break;
                //TODO: msgGl...
            case msgPowerInit:
                cmdStr = QString("INIT %1 POWER").arg(srcpbus);
                break;
            case msgPowerGet:
                cmdStr = QString("GET %1 POWER").arg(srcpbus);
                break;
            case msgPowerSet:
                cmdStr = QString("SET %1 POWER %2").arg(srcpbus)
                    .arg(power ? "ON" :"OFF");
                break;
            case msgServerReset:
                cmdStr = "RESET 0 SERVER";
                break;
            case msgServerShutdown:
                cmdStr = "TERM 0 SERVER";
                break;
            case msgNoMsg:
            default:
                break;
        }

    return cmdStr;
}


int SrcpMessage::getMessage()
{
    return (int) message;
}


QString SrcpMessage::getProtocolStr(Protocol pro) const
{
    QString proStr = "";

    switch (pro) {
        case proMM:
            proStr = "M";
            break;
        case proDCC:
            proStr = "N";
            break;
        case proServ:
            proStr = "P";
            break;
    }
    return proStr;
}


void SrcpMessage::setBus(unsigned int bus)
{
    srcpbus = bus;
}


void SrcpMessage::setFbData(unsigned int bus, Feedback fbt, unsigned int prt)
{
    srcpbus = bus;
    fbtype = fbt;
    fbport = prt;
}


void SrcpMessage::setGaData(Protocol pro, unsigned int bus,
        unsigned int adr, unsigned int prt, int dly)
{
    protocol = pro;
    srcpbus = bus;
    address = adr;
    port = prt;
    delay = dly;
}

//TODO: void SrcpMessage::setGlData()

void SrcpMessage::setPowerData(unsigned int bus, bool pwr)
{
    srcpbus = bus;
    power = pwr;
}


