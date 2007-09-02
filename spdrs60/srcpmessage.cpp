/***************************************************************************
                           srcpmessage.cpp
                           version 0.5.2 $Revision: 1.17 $
                           -------------------------------
    copyright            : (C) 2005-2007 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-09-02 17:50:27 $
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


// just to avoid compiler warnings
SrcpMessage::~SrcpMessage()
{           
}


QString SrcpMessage::getSrcpMessageStr(SrcpPort::CommunicationStyle style) const
{
    QString cmdStr = "";

    // SRCP 0.7
    if (SrcpPort::csOld == style)

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
                    default:
                        cmdStr = "INIT FB";
                        break;
                }
                break;
            case msgGaGet:
                cmdStr = QString("GET GA %1 %2 %3")
                    .arg(getProtocolStr(protocol)).arg(address).arg(port);
                break;
            case msgGaSet:
                cmdStr = QString("SET GA %1 %2 %3 %4 %5")
                    .arg(getProtocolStr(protocol)).arg(address)
                    .arg(port).arg(value).arg(delay);
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
                cmdStr = QString("SET %1 GA %2 %3 %4 %5")
                    .arg(srcpbus).arg(address).arg(port).arg(value).arg(delay);
                break;
                //TODO: msgGl...
                //TODO: flexible lock duration
            case msgLockSet:
                cmdStr = QString("SET %1 LOCK %2 %3 0").arg(srcpbus)
                    .arg(getDeviceGroupStr(devicegroup)).arg(address);
                break;
            case msgLockGet:
                cmdStr = QString("GET %1 LOCK %2 %3").arg(srcpbus)
                    .arg(getDeviceGroupStr(devicegroup)).arg(address);
                break;
            case msgLockTerm:
                cmdStr = QString("TERM %1 LOCK %2 %3").arg(srcpbus)
                    .arg(getDeviceGroupStr(devicegroup)).arg(address);
                break;
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
            case msgServerLogout:
                cmdStr = "TERM 0 SESSION";
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
        case proServer:
            proStr = "P";
            break;
        case proSelectrix:
            proStr = "S";
            break;
        default:
            proStr = "";
            break;
    }
    return proStr;
}


QString SrcpMessage::getDeviceGroupStr(DeviceGroup dg) const
{
    QString dgStr = "";
    
    switch (dg) {
        case dgGA:
            dgStr = "GA";
            break;
        case dgGL:
            dgStr = "GL";
            break;
        case dgFB:
            dgStr = "FB";
            break;
        case dgSM:
            dgStr = "SM";
            break;
        case dgTime:
            dgStr = "TIME";
            break;
        case dgPower:
            dgStr = "POWER";
            break;
        case dgServer:
            dgStr = "SERVER";
            break;
        case dgSession:
            dgStr = "SESSION";
            break;
        case dgLock:
            dgStr = "LOCK";
            break;
        case dgDescription:
            dgStr = "DESCRIPTION";
            break;
    }
    return dgStr;
}


QString SrcpMessage::getActionStr(Action ac) const
{
    QString acStr = "";
    
    switch (ac) {
        case acInit:
            acStr = "INIT";
            break;
        case acSet:
            acStr = "SET";
            break;
        case acGet:
            acStr = "GET";
            break;
        case acCheck:
            acStr = "CHECK";
            break;
        case acTerm:
            acStr = "TERM";
            break;
        case acWait:
            acStr = "WAIT";
            break;
        case acReset:
            acStr = "RESET";
            break;
        case acVerify:
            acStr = "VERIFY";
            break;
    }
    return acStr;
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
        unsigned int adr, unsigned int prt, unsigned int val, int dly)
{
    protocol = pro;
    srcpbus = bus;
    address = adr;
    port = prt;
    value = val;
    delay = dly;
}

//TODO: void SrcpMessage::setGlData()

void SrcpMessage::setLockData(unsigned int bus, DeviceGroup dg,
        unsigned int adr)
{
    srcpbus = bus;
    devicegroup = dg;
    address = adr;
}


void SrcpMessage::setPowerData(unsigned int bus, bool pwr)
{
    srcpbus = bus;
    power = pwr;
}


