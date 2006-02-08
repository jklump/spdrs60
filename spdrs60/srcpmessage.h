/***************************************************************************
                           srcpmessage.h
                           version 0.5.0 $Revision: 1.6 $
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
       
        // only GA protocols
        enum Protocol {proMM = 0, proDCC, proServ};

        enum Feedback {fbS88 = 0, fbI8255, fbM6051, fbPS};

        SrcpMessage(Message = msgNoMsg);
        virtual QString getSrcpMessageStr(unsigned int version = 7) const;
        int getMessage();
        void setBus(unsigned int);
        void setFbData(unsigned int, Feedback, unsigned int);
        void setGaData(Protocol, unsigned int, unsigned int,
                unsigned int, int);
        //TODO: void setGlData();
        void setPowerData(unsigned int, bool);

    private:
        Message message;
        Protocol protocol;
        Feedback fbtype;
        bool power;
        int delay;
        unsigned int address;
        unsigned int fbport;
        unsigned int port;
        unsigned int srcpbus;
        QString getProtocolStr(Protocol pro = proMM) const;
};
#endif

