/***************************************************************************
                           crcfmessage.h
                           version 0.5.3 $Revision: 1.1 $
                           -------------------------------
    copyright            : (C) 2008 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-03-27 21:54:28 $
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

#ifndef CRCFMESSAGE_H
#define CRCFMESSAGE_H

#include <qstring.h>

class CrcfMessage
{
    public:
        enum CrcfActor {acNone = 0, acRoute, acSection, acTrain};
       
        enum CrcfMethod {meNone = 0, meSet, meGet, meInfo};

        enum CrcfAttribute {atNone = 0, atState, atType, atTrain};

        CrcfMessage(CrcfActor, unsigned int, CrcfMethod,
                CrcfAttribute, unsigned int);
        virtual ~CrcfMessage();

        CrcfMessage::CrcfActor getActor() const;
        CrcfMessage::CrcfMethod getMethod() const;
        CrcfMessage::CrcfAttribute getAttribute() const;
        QString getActorStr() const;
        QString getMethodStr() const;
        QString getAttributeStr() const;
        unsigned int getActorId() const;
        unsigned int getAttValue() const;
        QString getMessage() const;
        static QString message(CrcfActor, unsigned int, CrcfMethod,
                CrcfAttribute, unsigned int);
        static CrcfMessage* parse(QString&);

    private:
        static QString actorStr(CrcfActor);
        static QString methodStr(CrcfMethod);
        static QString attributeStr(CrcfAttribute);
        CrcfActor actor;
        CrcfMethod method;
        CrcfAttribute attribute;
        unsigned int actor_id;
        unsigned int attvalue;
};
#endif

