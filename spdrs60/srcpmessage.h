/***************************************************************************
                           srcpmessage.h
                           version 0.5.0 $Revision: 1.3 $
                           -------------------------------
    copyright            : (C) 2005 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2006-01-01 21:29:59 $
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


class SrcpMessage
{
    public:
        enum Message {msgNoMsg = 0,
            msgServerLogin, msgServerReset,
            msgServerShutdown, msgServerLogout,
            msgPowerInit, msgPowerSet, msgPowerGet, msgPowerTerm,
            msgPowerInfo,
            msgFbInit, msgFbGet, msgFbTerm, msgFbInfo,
            msgGaInit, msgGaSet, msgGaGet, msgGaInfo,
            msgGlInit, msgGlSet, msgGlGet, msgGlTerm, msgGlInfo};
        
        enum Protocol {proMM = 0, proDCC, proFsm, proLoco, proStx,
            proServ, proZimo};

        enum Feedback {fbS88 = 0, fbI8255, fbM6051, fbPS};

        SrcpMessage(Message = msgNoMsg);
        QString getSrcpMessageStr(unsigned int version = 7) const;
        int getMessage();
        void setConnectionModeCmd(bool);
        void setFbData(unsigned int, Feedback, unsigned int);
        void setGaData(Protocol, unsigned int, unsigned int,
                unsigned int, unsigned int);
        //TODO: void setGlData();
        void setPowerData(unsigned int, bool);

    private:
        Message message;
        Protocol protocol;
        Feedback fbtype;
        bool cmdmode;
        bool power;
        unsigned int address;
        unsigned int delay;
        unsigned int fbport;
        unsigned int port;
        unsigned int srcpbus;
};
#endif

