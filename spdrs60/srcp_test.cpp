/***************************************************************************
                           srcp_test.cpp
                           version 0.5.5 $Revision: 1.1 $
                           -------------------------------
    copyright            :(C) 2008 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2008-12-25 18:23:51 $
***************************************************************************/

/**************************************************************************
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 **************************************************************************/

/**************************************************************************
   test program to check valid SRCP string translations by "make check"
 **************************************************************************/


#include <qapplication.h>
#include <qstring.h>

#include "srcpmessage.h"


bool checkOldSrcpString(const QString& nominal, SrcpMessage* sm)
{
    QString result = sm->getSrcpMessageStr(SrcpPort::csOld);
    bool isOK = (nominal == result);
    if (!isOK)
        qWarning("Failed: Nominal: \"%s\", Result: \"%s\"",
                nominal.data(), result.data());
    return isOK;
}


bool checkNewSrcpString(const QString& nominal, SrcpMessage* sm)
{
    QString result = sm->getSrcpMessageStr(SrcpPort::csNew);
    bool isOK = (nominal == result);
    if (!isOK)
        qWarning("Failed: Nominal: \"%s\", Result: \"%s\"",
                nominal.data(), result.data());
    return isOK;
}


bool runSrcp07Test()
{
    QString nominal, result;
    bool returnvalue = true;

    SrcpMessage* sm = new SrcpMessage();
    if (sm == NULL)
        return false;

    /*SRCP 0.7*/
    qWarning("Testing old SRCP messages...");

    /*Server*/
    sm->setMessage(SrcpMessage::msgServerReset);
    if (!checkOldSrcpString("RESET", sm))
        returnvalue = false;

    sm->setMessage(SrcpMessage::msgServerShutdown);
    if (!checkOldSrcpString("SHUTDOWN", sm))
        returnvalue =  false;

    /*Session*/
    sm->setMessage(SrcpMessage::msgSessionTerm);
    if (!checkOldSrcpString("LOGOUT", sm))
        returnvalue = false;

    /*Power*/
    sm->setMessage(SrcpMessage::msgPowerSet);
    sm->setPowerData(0, false);
    if (!checkOldSrcpString("SET POWER OFF", sm))
        returnvalue = false;

    // sm->setMessage(SrcpMessage::msgPowerSet);
    sm->setPowerData(0, true);
    if (!checkOldSrcpString("SET POWER ON", sm))
        returnvalue = false;

    sm->setMessage(SrcpMessage::msgPowerGet);
    if (!checkOldSrcpString("GET POWER", sm))
        returnvalue =  false;

    /*GA*/
    // MM protocol
    sm->setMessage(SrcpMessage::msgGaInit);
    sm->setGaData(SrcpMessage::proMM, 1, 2, 3, 4, 5);
    if (!checkOldSrcpString("", sm))
        returnvalue = false;

    sm->setMessage(SrcpMessage::msgGaSet);
    sm->setGaData(SrcpMessage::proMM, 1, 2, 3, 4, 5);
    if (!checkOldSrcpString("SET GA M 2 3 4 5", sm))
        returnvalue = false;

    sm->setMessage(SrcpMessage::msgGaGet);
    sm->setGaData(SrcpMessage::proMM, 1, 2, 3, 4, 5);
    if (!checkOldSrcpString("GET GA M 2 3", sm))
        returnvalue = false;

    sm->setMessage(SrcpMessage::msgGaInfo);
    sm->setGaData(SrcpMessage::proMM, 1, 2, 3, 4, 5);
    if (!checkOldSrcpString("INFO GA M 2 3 4", sm))
        returnvalue = false;

    // DCC protocol
    sm->setMessage(SrcpMessage::msgGaInit);
    sm->setGaData(SrcpMessage::proDCC, 1, 2, 3, 4, 5);
    if (!checkOldSrcpString("", sm))
        returnvalue = false;

    sm->setMessage(SrcpMessage::msgGaSet);
    sm->setGaData(SrcpMessage::proDCC, 1, 2, 3, 4, 5);
    if (!checkOldSrcpString("SET GA N 2 3 4 5", sm))
        returnvalue = false;

    sm->setMessage(SrcpMessage::msgGaGet);
    sm->setGaData(SrcpMessage::proDCC, 1, 2, 3, 4, 5);
    if (!checkOldSrcpString("GET GA N 2 3", sm))
        returnvalue = false;

    sm->setMessage(SrcpMessage::msgGaInfo);
    sm->setGaData(SrcpMessage::proDCC, 1, 2, 3, 4, 5);
    if (!checkOldSrcpString("INFO GA N 2 3 4", sm))
        returnvalue = false;

    // Server protocol
    sm->setMessage(SrcpMessage::msgGaInit);
    sm->setGaData(SrcpMessage::proServer, 1, 2, 3, 4, 5);
    if (!checkOldSrcpString("", sm))
        returnvalue = false;

    sm->setMessage(SrcpMessage::msgGaSet);
    sm->setGaData(SrcpMessage::proServer, 1, 2, 3, 4, 5);
    if (!checkOldSrcpString("SET GA P 2 3 4 5", sm))
        returnvalue = false;

    sm->setMessage(SrcpMessage::msgGaGet);
    sm->setGaData(SrcpMessage::proServer, 1, 2, 3, 4, 5);
    if (!checkOldSrcpString("GET GA P 2 3", sm))
        returnvalue = false;

    sm->setMessage(SrcpMessage::msgGaInfo);
    sm->setGaData(SrcpMessage::proServer, 1, 2, 3, 4, 5);
    if (!checkOldSrcpString("INFO GA P 2 3 4", sm))
        returnvalue = false;


    /*SRCP 0.8*/
    qWarning("Testing new SRCP messages...");

    /*Server*/
    sm->setMessage(SrcpMessage::msgServerReset);
    if (!checkNewSrcpString("RESET 0 SERVER", sm))
        returnvalue =  false;

    sm->setMessage(SrcpMessage::msgServerShutdown);
    if (!checkNewSrcpString("TERM 0 SERVER", sm))
        returnvalue =  false;

    /*Session*/
    sm->setMessage(SrcpMessage::msgSessionTerm);
    if (!checkNewSrcpString("TERM 0 SESSION", sm))
        returnvalue = false;

    /*Power*/
    sm->setMessage(SrcpMessage::msgPowerSet);
    sm->setPowerData(1, false);
    if (!checkNewSrcpString("SET 1 POWER OFF", sm))
        returnvalue = false;

    // sm->setMessage(SrcpMessage::msgPowerSet);
    sm->setPowerData(1, true);
    if (!checkNewSrcpString("SET 1 POWER ON", sm))
        returnvalue = false;

    sm->setMessage(SrcpMessage::msgPowerGet);
    sm->setPowerData(1, true);
    if (!checkNewSrcpString("GET 1 POWER", sm))
        returnvalue =  false;

    /*GA*/
    // TODO: loop over protocol
    sm->setMessage(SrcpMessage::msgGaInit);
    sm->setGaData(SrcpMessage::proMM, 1, 2, 3, 4, 5);
    if (!checkNewSrcpString("INIT 1 GA 2 M", sm))
        returnvalue = false;

    sm->setMessage(SrcpMessage::msgGaSet);
    sm->setGaData(SrcpMessage::proMM, 1, 2, 3, 4, 5);
    if (!checkNewSrcpString("SET 1 GA 2 3 4 5", sm))
        returnvalue = false;

    sm->setMessage(SrcpMessage::msgGaGet);
    sm->setGaData(SrcpMessage::proMM, 1, 2, 3, 4, 5);
    if (!checkNewSrcpString("GET 1 GA 2 3", sm))
        returnvalue = false;

    sm->setMessage(SrcpMessage::msgGaInfo);
    sm->setGaData(SrcpMessage::proMM, 1, 2, 3, 4, 5);
    if (!checkNewSrcpString("INFO 1 GA 2 3 4", sm))
        returnvalue = false;


    delete sm;
    return returnvalue;
}


/*create and run console application*/
int main(int argc, char* argv[])
{
   QApplication a(argc, argv, false);

   bool success = runSrcp07Test();

   return (success) ? 0 : 1;
}

