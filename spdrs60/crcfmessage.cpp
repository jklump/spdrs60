/***************************************************************************
                           crcfmessage.cpp
                           version 0.5.3 $Revision: 1.10 $
                           -------------------------------
    copyright            : (C) 2008 by Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2009-03-08 08:24:06 $
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
    This code implements a class for CRCF messages which can be
    exchanged via SRCP Generic Messages (GM).
    It implements methods to parse a CRCF message string to binary message
    content and the other way around.
 ***************************************************************************/

#include "crcfmessage.h"

#include <qstringlist.h>
#include <qurl.h>

/*constructor with integer attribute value*/
CrcfMessage::CrcfMessage(CrcfActor cac, unsigned int aid, CrcfMethod cme,
        CrcfAttribute cat, unsigned int value)
{
    actor = cac;
    actor_id = aid;
    method = cme;
    attribute = cat;
    attvalue = value;
    attvaluestr = "";
}


/*constructor with string attribute value*/
CrcfMessage::CrcfMessage(CrcfActor cac, unsigned int aid, CrcfMethod cme,
        CrcfAttribute cat, const QString& valuestr)
{
    actor = cac;
    actor_id = aid;
    method = cme;
    attribute = cat;
    attvalue = 0;
    attvaluestr = valuestr;
    QUrl::decode(attvaluestr);
}


// just to avoid compiler warnings
CrcfMessage::~CrcfMessage()
{           
}


CrcfMessage::CrcfActor CrcfMessage::getActor() const
{
    return actor;
}


QString CrcfMessage::getActorStr() const
{
    return actorStr(actor);
}


QString CrcfMessage::actorStr(CrcfActor ac)
{
    QString acStr = "";
    
    switch (ac) {
        case acRoute:
            acStr = "ROUTE";
            break;
        case acSection:
            acStr = "SECTION";
            break;
        case acTrain:
            acStr = "TRAIN";
            break;
        case acLayout:
            acStr = "LAYOUT";
            break;
        case acRwcc:
            acStr = "RWCC";
            break;
        default:
            break;
    }
    return acStr;
}


unsigned int CrcfMessage::getActorId() const
{
    return actor_id;
}


CrcfMessage::CrcfMethod CrcfMessage::getMethod() const
{
    return method;
}


QString CrcfMessage::getMethodStr() const
{
    return methodStr(method);
}


QString CrcfMessage::methodStr(CrcfMethod me)
{
    QString meStr = "";
    
    switch (me) {
        case meInfo:
            meStr = "INFO";
            break;
        case meSet:
            meStr = "SET";
            break;
        case meGet:
            meStr = "GET";
            break;
        default:
            break;
    }
    return meStr;
}


CrcfMessage::CrcfAttribute CrcfMessage::getAttribute() const
{
    return attribute;
}


QString CrcfMessage::getAttributeStr() const
{
    return attributeStr(attribute);
}


QString CrcfMessage::attributeStr(CrcfAttribute at)
{
    QString atStr = "";
    
    switch (at) {
        case atId:
            atStr = "ID";
            break;
        case atName:
            atStr = "NAME";
            break;
        case atState:
            atStr = "STATE";
            break;
        case atType:
            atStr = "TYPE";
            break;
        case atTrain:
            atStr = "TRAIN";
            break;
        case atRows:
            atStr = "ROWS";
            break;
        case atColumns:
            atStr = "COLUMNS";
            break;
        case atMode:
            atStr = "MODE";
            break;
        case atTableLight:
            atStr = "TABLELIGHT";
            break;
        default:
            break;
    }
    return atStr;
}


unsigned int CrcfMessage::getAttValue() const
{
    return attvalue;
}


/* return value string; if string contains white spaces escape it using
 * the character '"'*/
QString CrcfMessage::getAttValueStr() const
{
    return attvaluestr;
}

/* 
 * Parse CRCF message and return pointer to new message instance, if
 * message was valid.
 * <actor> <actor_id> <method> <attribute> [<value>]
 *    1       2          3         4          5
 */
CrcfMessage* CrcfMessage::parse(QString& msg)
{
    CrcfActor actor;
    CrcfMethod cm;
    CrcfAttribute cat;
    unsigned int aid = 0;
    unsigned int value = 0;
    QStringList tokens;

    tokens = QStringList::split(' ', msg);
    if (tokens.count() < 4)
        // error message to short
        return NULL;

    // token 1: actor
    if ("ROUTE" == tokens[0]) {
        actor = acRoute;
    }
    else if ("LAYOUT" == tokens[0]) {
        actor = acLayout;
    }
    else if ("SECTION" == tokens[0]) {
        actor = acSection;
    }
    else if ("TRAIN" == tokens[0]) {
        actor = acTrain;
    }
    else if ("RWCC" == tokens[0]) {
        actor = acRwcc;
    }
    else {
        //error unsupported CRCF actor
        return NULL;
    }

    // token 2: actor id
    aid = tokens[1].toUInt();

    // token 3: method
    if ("SET" == tokens[2]) {
        cm = meSet;
    }
    else if ("GET" == tokens[2]) {
        cm = meGet;
    }
    else if ("INFO" == tokens[2]) {
        cm = meInfo;
    }
    else {
        //error unsupported CRCF method
        return NULL;
    }

    // token 4: attribute (no context analysis)
    if ("STATE" == tokens[3]) {
        cat = atState;
    }
    else if ("ID" == tokens[3]) {
        cat = atId;
    }
    else if ("NAME" == tokens[3]) {
        cat = atName;
    }
    else if ("TYPE" == tokens[3]) {
        cat = atType;
    }
    else if ("TRAIN" == tokens[3]) {
        cat = atTrain;
    }
    else if ("ROWS" == tokens[3]) {
        cat = atRows;
    }
    else if ("COLUMNS" == tokens[3]) {
        cat = atColumns;
    }
    else if ("MODE" == tokens[3]) {
        cat = atMode;
    }
    else if ("TABLELIGHT" == tokens[3]) {
        cat = atTableLight;
    }
    else {
        //error unsupported CRCF attribute
        return NULL;
    }

    // token 5: attribute value
    if (meSet == cm || meInfo == cm) {
        if (tokens.count() == 5) {
            if (cat == atName)
                return new CrcfMessage(actor, aid, cm, cat, tokens[4]);
            value = tokens[4].toUInt();
        }
        else {
            //error unsupported CRCF operation
            return NULL;
        }
    }

    return new CrcfMessage(actor, aid, cm, cat, value);
}

/*assemble CRCF message string*/
QString CrcfMessage::getMessage() const
{
    if (atName != attribute)
        return message(actor, actor_id, method, attribute, attvalue);
    
    return message(actor, actor_id, method, attribute, attvaluestr);
}

/*static assemble CRCF message string, integer attribute value*/
QString CrcfMessage::message(CrcfActor cac, unsigned int aid, CrcfMethod cme,
        CrcfAttribute cat, unsigned int value)
{
    QString result;

    if (meGet == cme)
        result = QString("%1 %2 %3 %4").arg(actorStr(cac)).arg(aid)
            .arg(methodStr(cme)).arg(attributeStr(cat));
    else
        result = QString("%1 %2 %3 %4 %5").arg(actorStr(cac)).arg(aid)
            .arg(methodStr(cme)).arg(attributeStr(cat)).arg(value);

    return result;
}

/* Static assemble CRCF message string, string attribute value.
 * The value string gets URL encoded.*/
QString CrcfMessage::message(CrcfActor cac, unsigned int aid, CrcfMethod cme,
        CrcfAttribute cat, const QString& valuestr)
{
    QString result;
    QString crcfurl;

    if (meGet == cme)
        result = QString("%1 %2 %3 %4").arg(actorStr(cac)).arg(aid)
            .arg(methodStr(cme)).arg(attributeStr(cat));
    else {
        crcfurl = valuestr;
        QUrl::encode(crcfurl);
        result = QString("%1 %2 %3 %4 %5").arg(actorStr(cac)).arg(aid)
            .arg(methodStr(cme)).arg(attributeStr(cat)).arg(crcfurl);
    }

    return result;
}

