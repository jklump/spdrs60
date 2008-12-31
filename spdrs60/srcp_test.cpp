/***************************************************************************
 srcp_test.cpp
 -------------
 Copyright    : (C) 2008 Guido Scholz
 E-Mail       : guido.scholz@bayernline.de
 Begin        : 16.12.2008
 Last modified: $Date: 2008-12-31 07:39:01 $
                $Revision: 1.2 $
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


bool checkSrcp07String(const QString& nominal, SrcpMessage* sm)
{
    QString result = sm->getSrcpMessageStr(SrcpPort::csOld);
    bool isOK = (nominal == result);
    if (!isOK)
        qWarning("Failed: Nominal: \"%s\", Result: \"%s\"",
                nominal.data(), result.data());
    return isOK;
}


bool checkSrcp08String(const QString& nominal, SrcpMessage* sm)
{
    QString result = sm->getSrcpMessageStr(SrcpPort::csNew);
    bool isOK = (nominal == result);
    if (!isOK)
        qWarning("Failed: Nominal: \"%s\", Result: \"%s\"",
                nominal.data(), result.data());
    return isOK;
}


bool runSrcpTest()
{
    QString nominal, result;
    bool returnvalue = true;

    SrcpMessage* sm = new SrcpMessage();
    if (sm == NULL)
        return false;


    /*SRCP Server*/
    qWarning("Testing SRCP Server messages...");

    /*Reset*/
    sm->setMessage(SrcpMessage::msgServerReset);
    if (!checkSrcp07String("RESET", sm))
        returnvalue = false;

    if (!checkSrcp08String("RESET 0 SERVER", sm))
        returnvalue =  false;

    /*Term*/
    sm->setMessage(SrcpMessage::msgServerShutdown);
    if (!checkSrcp07String("SHUTDOWN", sm))
        returnvalue =  false;

    if (!checkSrcp08String("TERM 0 SERVER", sm))
        returnvalue =  false;


    /*SRCP Session*/
    qWarning("Testing SRCP Session messages...");

    /*Term*/
    sm->setMessage(SrcpMessage::msgSessionTerm);
    if (!checkSrcp07String("LOGOUT", sm))
        returnvalue = false;

    if (!checkSrcp08String("TERM 0 SESSION", sm))
        returnvalue = false;


    /*SRCP Power*/
    qWarning("Testing SRCP Power messages...");

    /*  SET OFF*/
    sm->setMessage(SrcpMessage::msgPowerSet);
    sm->setPowerData(1, false);
    if (!checkSrcp07String("SET POWER OFF", sm))
        returnvalue = false;

    if (!checkSrcp08String("SET 1 POWER OFF", sm))
        returnvalue = false;

    /*  SET ON*/
    sm->setPowerData(0, true);
    if (!checkSrcp07String("SET POWER ON", sm))
        returnvalue = false;

    sm->setPowerData(1, true);
    if (!checkSrcp08String("SET 1 POWER ON", sm))
        returnvalue = false;

    /*  GET*/
    sm->setMessage(SrcpMessage::msgPowerGet);
    sm->setPowerData(1, true);
    if (!checkSrcp07String("GET POWER", sm))
        returnvalue =  false;

    if (!checkSrcp08String("GET 1 POWER", sm))
        returnvalue =  false;


    /*SRCP Ga*/
    qWarning("Testing SRCP Ga messages...");

    /*MM protocol*/
    /*  GA INIT*/
    sm->setMessage(SrcpMessage::msgGaInit);
    sm->setGaData(SrcpMessage::proMM, 1, 2, 3, 4, 5);
    if (!checkSrcp07String("", sm))
        returnvalue = false;

    if (!checkSrcp08String("INIT 1 GA 2 M", sm))
        returnvalue = false;

    /*  GA SET*/
    sm->setMessage(SrcpMessage::msgGaSet);
    sm->setGaData(SrcpMessage::proMM, 1, 2, 3, 4, 5);
    if (!checkSrcp07String("SET GA M 2 3 4 5", sm))
        returnvalue = false;

    if (!checkSrcp08String("SET 1 GA 2 3 4 5", sm))
        returnvalue = false;

    /*  GA GET*/
    sm->setMessage(SrcpMessage::msgGaGet);
    sm->setGaData(SrcpMessage::proMM, 1, 2, 3, 4, 5);
    if (!checkSrcp07String("GET GA M 2 3", sm))
        returnvalue = false;

    sm->setGaData(SrcpMessage::proMM, 1, 2, 3, 4, 5);
    if (!checkSrcp08String("GET 1 GA 2 3", sm))
        returnvalue = false;

    /*  GA INFO*/
    sm->setMessage(SrcpMessage::msgGaInfo);
    sm->setGaData(SrcpMessage::proMM, 1, 2, 3, 4, 5);
    if (!checkSrcp07String("INFO GA M 2 3 4", sm))
        returnvalue = false;

    if (!checkSrcp08String("100 INFO 1 GA 2 3 4", sm))
        returnvalue = false;


    /* DCC protocol*/
    /*  GA INIT*/
    sm->setMessage(SrcpMessage::msgGaInit);
    sm->setGaData(SrcpMessage::proDCC, 1, 2, 3, 4, 5);
    if (!checkSrcp07String("", sm))
        returnvalue = false;

    if (!checkSrcp08String("INIT 1 GA 2 N", sm))
        returnvalue = false;

    /*  GA SET*/
    sm->setMessage(SrcpMessage::msgGaSet);
    sm->setGaData(SrcpMessage::proDCC, 1, 2, 3, 4, 5);
    if (!checkSrcp07String("SET GA N 2 3 4 5", sm))
        returnvalue = false;

    /*  GA GET*/
    sm->setMessage(SrcpMessage::msgGaGet);
    sm->setGaData(SrcpMessage::proDCC, 1, 2, 3, 4, 5);
    if (!checkSrcp07String("GET GA N 2 3", sm))
        returnvalue = false;

    /*  GA INFO*/
    sm->setMessage(SrcpMessage::msgGaInfo);
    sm->setGaData(SrcpMessage::proDCC, 1, 2, 3, 4, 5);
    if (!checkSrcp07String("INFO GA N 2 3 4", sm))
        returnvalue = false;

    /*Server protocol*/
    /*  GA INIT*/
    sm->setMessage(SrcpMessage::msgGaInit);
    sm->setGaData(SrcpMessage::proServer, 1, 2, 3, 4, 5);
    if (!checkSrcp07String("", sm))
        returnvalue = false;

    if (!checkSrcp08String("INIT 1 GA 2 P", sm))
        returnvalue = false;

    /*  GA SET*/
    sm->setMessage(SrcpMessage::msgGaSet);
    sm->setGaData(SrcpMessage::proServer, 1, 2, 3, 4, 5);
    if (!checkSrcp07String("SET GA P 2 3 4 5", sm))
        returnvalue = false;

    /*  GA GET*/
    sm->setMessage(SrcpMessage::msgGaGet);
    sm->setGaData(SrcpMessage::proServer, 1, 2, 3, 4, 5);
    if (!checkSrcp07String("GET GA P 2 3", sm))
        returnvalue = false;

    /*  GA INFO*/
    sm->setMessage(SrcpMessage::msgGaInfo);
    sm->setGaData(SrcpMessage::proServer, 1, 2, 3, 4, 5);
    if (!checkSrcp07String("INFO GA P 2 3 4", sm))
        returnvalue = false;

    /*SRCP Gl*/
    qWarning("Testing SRCP Gl messages...");
    qWarning("Testing SRCP Fb messages...");
    qWarning("Testing SRCP Lock messages...");
    qWarning("Testing SRCP Time messages...");
    qWarning("Testing SRCP Sm messages...");
    qWarning("Testing SRCP Description messages...");


    delete sm;
    return returnvalue;
}


/*create and run console application*/
int main(int argc, char* argv[])
{
   QApplication testApp(argc, argv, false);

   bool success = runSrcpTest();

   return (success) ? 0 : 1;
}

