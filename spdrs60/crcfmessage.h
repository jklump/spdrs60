/***************************************************************************
                           crcfmessage.h
                           version 0.5.3 $Revision: 1.3 $
                           -------------------------------
    copyright            : (C) 2008 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-04-08 20:09:14 $
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
    This is the headerfile for crcfmessage.cpp
 ***************************************************************************/

#ifndef CRCFMESSAGE_H
#define CRCFMESSAGE_H

#include <qstring.h>

class CrcfMessage
{
    public:
        enum CrcfActor {acNone = 0, acLayout, acRoute, acSection, acTrain};
       
        enum CrcfMethod {meNone = 0, meSet, meGet, meInfo};

        enum CrcfAttribute {atNone = 0, atId, atName, atState, atType,
            atTrain, atRows, atColumns};

        CrcfMessage(CrcfActor, unsigned int, CrcfMethod,
                CrcfAttribute, unsigned int);
        CrcfMessage(CrcfActor, unsigned int, CrcfMethod,
                CrcfAttribute, const QString);
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
        static QString message(CrcfActor, unsigned int, CrcfMethod,
                CrcfAttribute, const QString);
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
        QString attvaluestr;
};
#endif

