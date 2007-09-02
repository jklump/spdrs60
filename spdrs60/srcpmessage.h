/***************************************************************************
                           srcpmessage.h
                           version 0.5.2 $Revision: 1.15 $
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
    This is the headerfile for srcpmessage.cpp
 ***************************************************************************/

#ifndef SRCPMESSAGE_H
#define SRCPMESSAGE_H

#include <qstring.h>

#include "srcpport.h"

class SrcpMessage
{
    public:
        enum Message {msgNoMsg = 0,
            msgServerLogin, msgServerReset,
            msgServerShutdown, msgServerLogout,
            msgPowerInit, msgPowerSet, msgPowerGet, msgPowerTerm,
            msgPowerInfo,
            msgLockSet, msgLockGet, msgLockTerm, msgLockInfo,
            msgFbInit, msgFbGet, msgFbTerm, msgFbInfo,
            msgGaInit, msgGaSet, msgGaGet, msgGaInfo,
            msgGlInit, msgGlSet, msgGlGet, msgGlTerm, msgGlInfo};
       
        enum DeviceGroup {dgGA = 0, dgGL, dgFB, dgSM, dgTime, dgPower,
            dgServer, dgSession, dgLock, dgDescription};

        enum Action {acInit = 0, acSet, acGet, acCheck, acTerm, acWait,
            acReset, acVerify};

        // only GA protocols, FIXME: proNone is a temporary solution
        enum Protocol {proMM = 0, proDCC, proServer, proSelectrix, proNone};

        enum Feedback {fbS88 = 0, fbI8255, fbM6051, fbPS, fbSelectrix};

        SrcpMessage(Message = msgNoMsg);
        virtual ~SrcpMessage();
        //SrcpMessage(DeviceGroup = dgServer, Action = dgInit,
        //unsigned int bus = 0);
        // TODO: this should be: ConnectionStyle {csOld = 0, csNew}
        virtual QString getSrcpMessageStr(
                SrcpPort::CommunicationStyle style = SrcpPort::csOld) const;
        int getMessage();
        //int getDeviceGroup();
        //int getAction();
        void setBus(unsigned int);
        void setFbData(unsigned int, Feedback, unsigned int);
        void setGaData(Protocol, unsigned int, unsigned int,
                unsigned int, unsigned int, int);
        //TODO: void setGlData();
        void setLockData(unsigned int, DeviceGroup, unsigned int);
        void setPowerData(unsigned int, bool);

    private:
        Message message;
        DeviceGroup devicegroup;
        Action action;
        Protocol protocol;
        Feedback fbtype;
        bool power;
        int delay;
        unsigned int address;
        unsigned int fbport;
        unsigned int port;
        unsigned int value;
        unsigned int srcpbus;
        QString getProtocolStr(Protocol pro = proMM) const;
        QString getDeviceGroupStr(DeviceGroup dg = dgGA) const;
        QString getActionStr(Action ac = acInit) const;
};
#endif

