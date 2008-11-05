/***************************************************************************
 srcpport.cpp
 ------------
 Begin        : 17.08.2007
 Last modified: $Date: 2008-11-05 08:42:40 $
                $Revision: 1.8 $
 Copyright    : (C) 2007-2008 by Guido Scholz <guido.scholz@bayernline.de>
 Description  : Abstract class for network communication with SRCP server.
                Communication styles SRCP 0.7 and 0.8 are supported.
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "srcpport.h"

#include <qapplication.h>
#include <qstringlist.h>
#include <qdatetime.h>


SrcpPort::SrcpPort(QObject* parent, const char* name, const char* hostname,
        unsigned int portnumber, CommunicationStyle commstyle,
        bool translate): QObject(parent, name)
{
    // preset user preferences data
    host = hostname;
    port = portnumber;
    translateservertime = translate;
    commStyle = commstyle;
    otherProtocol = "";
    
    // reset connection dependend data
    clearConnectionData();
    reconnect = false;
    srcpSocket = new QSocket(this, "srcpSocket");
    Q_CHECK_PTR(srcpSocket);

    connect(srcpSocket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(srcpSocket, SIGNAL(hostFound()), this, SLOT(hostFound()));
    connect(srcpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(srcpSocket, SIGNAL(connectionClosed()), this, SLOT(socketClosed()));
    connect(srcpSocket, SIGNAL(error(int)), this, SLOT(socketError(int)));
}

/*
 * destructor to close socket connection and free socket memory
 */
SrcpPort::~SrcpPort()
{
    serverDisconnect();
    //delete srcpSocket;
}


/*
 * clear connection data
 */
void SrcpPort::clearConnectionData()
{
    sessionid = 0;
    srcpVersion = "";
    srcpOther = "";
    srcpServer = "";
    srcpState = sNone;
    currentStyle = csNone;
}

/*
 * Set hostname and portnumber to new values, close current connection
 * and reconnect if values change.
 */
void SrcpPort::setServer(const QString& hn, unsigned int prt)
{
    if ((host != hn) || (port != prt)) {

        host = hn;
        port = prt;

        //if (srcpSocket->isOpen()) {
        if (hasServerConnection()) {
            reconnect = true;
            serverDisconnect();
        }
    }
}

/*
 * Set prefered communication style, hostname and portnumber to new
 * values, close current connection and reconnect if values change.
 */
void SrcpPort::setServer(CommunicationStyle style,
        const QString& hn, unsigned int prt)
{
    commStyle= style;
    setServer(hn, prt);
}

/*
 * return hostname
 */
QString SrcpPort::getHostname() const
{
    return host;
}


/*
 * return port number
 */
unsigned int SrcpPort::getPortNumber() const
{
    return port;
}

/*
 * enable/disable translation of SRCP 0.8 server time value
 */
void SrcpPort::enableTimeTranslation(bool translate)
{
    translateservertime = translate;
}

/*
 * read if translation of SRCP 0.8 server time value is enabled
 */
bool SrcpPort::isTimeTranslationEnabled()
{
    return translateservertime;
}

/*
 * set other protocol for SRCP 0.8 SRCPOTHER value
 */
void SrcpPort::setPreferedProtocol(const QString& protocol)
{
    otherProtocol = protocol;
}

/*
 * set communication style, has no effect if a connection is running
 */
void SrcpPort::setCommunicationStyle(CommunicationStyle style)
{
    commStyle = style;
}

/*
 * initiate server connection and start handshake phase
 */
void SrcpPort::serverConnect()
{
    emit statusMessage(tr("%1: Try to connect host '%2' on port '%3'")
            .arg(getConnectionMode()).arg(host).arg(port));

    if (commStyle == csOld) {
        srcpState = sRun;
        currentStyle = csOld;
    }
    else
        srcpState = sLogin;

    srcpSocket->connectToHost(host, port);
}

/*
 * stop server connection and reset communication state
 */
void SrcpPort::serverDisconnect()
{
    if (srcpSocket->isOpen()) {
        clearConnectionData();
        srcpSocket->close();
        if (srcpSocket->state() == QSocket::Closing) {
            // We have a delayed close.
            connect(srcpSocket, SIGNAL(delayedCloseFinished()),
                    SLOT(socketDelayedClosed()));
        }
        // Socket immediately closed
        else {
            clearConnectionData();
            emit connectionStateChanged(false);
            emit statusMessage(tr("%1: Socket immediately closed.")
                    .arg(getConnectionMode()));

            if (reconnect) {
                reconnect = false;
                serverConnect();
            }
        }
    }
}


/*
 * report current connection state
 */
bool SrcpPort::hasServerConnection()
{
    return ((srcpSocket->state() != QSocket::Idle) &&
            (srcpSocket->state() != QSocket::Closing));
}


/*
 * Response to incomming SRCP messages. In old style mode there are only
 * two states (1. and 5.). In new style mode there is a handshake phase
 * with additional three or four states:
 *
 *  0. sNone: No connection
 *  1. sLogin: Start login and receive welcome message
 *  2. sProtocol: optional, select other protocol version
 *  3. sConnectionMode: select connection mode COMMAND or INFO
 *  4. sGo: Receive session number
 *  5. sRun: Receive SRCP messages and pass them through
 *
*/
void SrcpPort::readData()
{
    while (srcpSocket->canReadLine()) {
        QString line = srcpSocket->readLine();

        // cut off line break
        if (line.endsWith("\n"))
            line.truncate(line.length() - 1);

        // cut off line feed (not implemented yet)

        // normal operation mode, comes first to be faster
        // incoming messages are send als command messages
        if (srcpState == sRun) {
            //fprintf(stderr, "Info: %s\n", line.data());
            if ((currentStyle == csNew) && translateservertime)
                line = translateServerTime(line);
            emit messageReceived(line);
        }

        // handshake, only initial phase of a new connection
        // incoming messages are send als status messages
        else {
            //fprintf(stderr, "Login: %s\n", line.data());
            if ((currentStyle == csNew) && translateservertime)
                line = translateServerTime(line);
            emit statusMessage(line);
            QStringList tokens;
            QStringList wmt;
            QStringList::Iterator it;
            QString mode;

            // select login state
            switch(srcpState) {
                case sLogin:

                    // get welcome message and split to tokens
                    // we expect a list of three elements
                    tokens = QStringList::split(";", line);
                    it = tokens.begin();

                    // split tokens to get SRCP, SRCPOTHER
                    // and server values
                    while (it != tokens.end()) {
                        QString(*it).stripWhiteSpace();

                        // here we expect a list of exactly
                        // two elements
                        wmt = QStringList::split(" ", QString(*it));

                        if (wmt.count() == 2) {

                            if (wmt[0].lower() == "srcp")
                                srcpVersion = wmt[1];
                            else if (wmt[0].lower() == "srcpother")
                                srcpOther = wmt[1];
                            else {
                                if (!srcpServer.isEmpty()) {
                                    srcpServer.append(":");
                                    srcpServer.append(QString(*it));
                                }
                                else
                                    srcpServer = QString(*it);
                            }
                        }
                        else if (wmt.count() > 2)
                            emit statusMessage(
                                tr("%1: Parse error, parameter list too "
                                    "long '%2'.")
                                .arg(getConnectionMode()).arg(QString(*it)));
                        else
                            emit statusMessage(
                                tr("%1: Parse error, parameter list too "
                                    "short '%2'.")
                                .arg(getConnectionMode()).arg(QString(*it)));

                        wmt.clear();
                        ++it;
                    }

                    // check for valid SRCP version
                    // a) No SRCP version string found
                    if (srcpVersion.isEmpty()) {
                        emit statusMessage(
                                tr("%1: Communication error, no SRCP "
                                    "version found.").arg(getConnectionMode()));
                        serverDisconnect();
                    }

                    // b) SRCP 0.7 style connection
                    else if ((srcpVersion >= "0.7.0") &&
                            (srcpVersion < "0.8.0")) {
                        if (commStyle == csNew) {
                            emit statusMessage(
                                    tr("%1: Communication error, wrong SRCP "
                                        "version '%2'.")
                                    .arg(getConnectionMode()).arg(srcpVersion));
                            serverDisconnect();
                        }
                        else {
                            currentStyle = csOld;
                            // set next mode
                            srcpState = sRun;
                            emit connectionStateChanged(true);
                        }
                    }

                    // c) SRCP 0.8 style connection
                    else if ((srcpVersion >= "0.8.0") &&
                            (srcpVersion < "0.9.0")) {
                        if (commStyle == csOld) {
                            emit statusMessage(
                                    tr("%1: Communication error, wrong SRCP "
                                        "version '%2'.")
                                    .arg(getConnectionMode()).arg(srcpVersion));
                            serverDisconnect();
                        }
                        else {
                            currentStyle = csNew;
                            // option to select protocol is only used,
                            // if string value for protocol is given
                            if (!otherProtocol.isEmpty()) {
                                // set next mode
                                srcpState = sProtocol;
                                sendToServer(QString("SET PROTOCOL SRCP %1")
                                        .arg(otherProtocol));
                            }
                            else {
                                // set next mode
                                srcpState = sConnectionMode;
                                mode = getConnectionMode();
                                sendToServer(
                                        QString("SET CONNECTIONMODE SRCP %1")
                                        .arg(mode));
                            }
                        }
                    }

                    // d) No valid SRCP version string
                    else {
                        emit statusMessage(
                                tr("%1: Communication error, wrong SRCP "
                                    "version '%2'.")
                                .arg(getConnectionMode()).arg(srcpVersion));
                        serverDisconnect();
                    }

                    break;

                case sProtocol:
                    // set protocol version
                    // <time> 201 OK PROTOCOL
                    //   0     1  2     3
                    tokens = QStringList::split(" ", line);
                    if ((tokens.count() < 4) || (tokens[2] != "OK")) {
                        emit statusMessage(tr("%1: Communication error "
                                    "PROTOCOL '%2'")
                                .arg(getConnectionMode()).arg(line));
                        serverDisconnect();
                    }
                    else {
                        // set next mode
                        srcpState = sConnectionMode;
                        mode = getConnectionMode();
                        sendToServer(QString("SET CONNECTIONMODE SRCP %1")
                                .arg(mode));
                    }
                    break;

                case sConnectionMode:
                    // set connection mode
                    // <time> 202 OK CONNECTIONMODE
                    //   0     1  2        3
                    tokens = QStringList::split(" ", line);
                    if ((tokens.count() < 4) || (tokens[2] != "OK")) {
                        emit statusMessage(tr("%1: Communication error "
                                    "CONNECTIONMODE '%2'")
                                .arg(getConnectionMode()).arg(line));
                        serverDisconnect();
                    }
                    else {
                        // set next mode
                        srcpState = sGo;
                        sendToServer("GO");
                    }
                    break;

                case sGo:
                    // get session number
                    // <time> 200 OK GO <session-id>
                    //   0     1  2  3      4
                    tokens = QStringList::split(" ", line);
                    if ((tokens.count() < 5) || (tokens[2] != "OK")) {
                        emit statusMessage(
                                tr("%1: Communication error GO '%2'")
                                .arg(getConnectionMode()).arg(line));
                        serverDisconnect();
                    }
                    else {
                        sessionid = tokens[4].toUInt();

                        // set next mode
                        srcpState = sRun;
                        emit connectionStateChanged(true);
                    }
                    break;

                default:
                    // should never be reached
                    emit statusMessage(
                            tr("%1: Error, wrong SRCP state: %2")
                            .arg(getConnectionMode()).arg(srcpState));
                    break;
            }
        }
    }
}

/*
 * give response if a new socket connection is successfully established
 * for SRCP 07. info port and feedback port connection state change is
 * reported immediately (without login procedure)
 */
void SrcpPort::socketConnected()
{
    emit statusMessage(tr("%1: Socket connected to "
                "host '%2' on port '%3'").arg(getConnectionMode()).arg(host).arg(port));

    if (srcpState == sRun)
        emit connectionStateChanged(true);
}


/*
 * give response if host was found
 */
void SrcpPort::hostFound()
{
    emit statusMessage(tr("%1: Host '%2' found").arg(getConnectionMode()).arg(host));
}


/*
 * socket was closed by peer
 */
void SrcpPort::socketClosed()
{
    if (srcpSocket->isOpen())
        srcpSocket->close();

    clearConnectionData();
    emit connectionStateChanged(false);
    emit statusMessage(tr("%1: Socket closed by foreign host.").arg(getConnectionMode()));
}

/*
 * socket was actively closed;
 * reconnect if server name or port was changed
 */
void SrcpPort::socketDelayedClosed()
{
    clearConnectionData();
    emit connectionStateChanged(false);
    emit statusMessage(tr("%1: Socket delayed closed.").arg(getConnectionMode()));

    if (reconnect) {
        reconnect = false;
        serverConnect();
    }
}


/*
 * show error message text
 */
void SrcpPort::socketError(int error)
{
    QString errorMsg = getSocketErrorString(error);
    emit statusMessage(tr("%1: Socket error %2 occured (%3).")
            .arg(getConnectionMode()).arg(error).arg(errorMsg));
}


/*
 * send SRCP command to server, for handshake phase only
 */
void SrcpPort::sendToServer(const QString& cs)
{
    if (srcpSocket->isOpen()) {
        QCString cmd = cs.ascii();

        // check for line end
        if (!cs.endsWith("\n"))
            cmd.append("\n");

        //fprintf(stderr, "Send: %s", cmd.data());
        srcpSocket->writeBlock(cmd, (ulong) cmd.length());

        // give application some time to send message
        qApp->processEvents();

        if (srcpState == sRun)
            emit messageSend(cs);
        else
            emit statusMessage(cs);
    }
}

/*
 * get session id of current srcp connection
 */
unsigned int SrcpPort::getSessionId()
{
    return sessionid;
}

/*
 * get SRCP version of current srcp session
 */
QString SrcpPort::getSrcpVersion()
{
    return srcpVersion;
}

/*
 * get SRCPOTHER value of current connected SRCP server
 */
QString SrcpPort::getSrcpOther()
{
    return srcpOther;
}

/*
 * get server name and version of current connected SRCP server
 */
QString SrcpPort::getSrcpServer()
{
    return srcpServer;
}

/*
 * get communication style of current connected SRCP server,
 * SRCP 0.7: csOld, SRCP 0.8: csNew, no connection: csNone
 */
int SrcpPort::getCurrentStyle()
{
    return currentStyle;
}

/*
 * return a message string for every numerical error
 */
QString SrcpPort::getSocketErrorString(int e)
{
    QString ErrMessage = "";

    switch (e) {
        case (QSocket::ErrConnectionRefused):
            ErrMessage = tr("connection refused");
            break;
        case (QSocket::ErrHostNotFound):
            ErrMessage = tr("host not found");
            break;
        case (QSocket::ErrSocketRead):
            ErrMessage = tr("socket read error");
            break;
    }
    return ErrMessage;
}

/*
 * translate SRCP 0.8 server time value to a readable string
 */
QString SrcpPort::translateServerTime(const QString& msg)
{
    QString msgstr;
    QString timestr = msg.section(' ', 0, 0);
    if (!timestr.startsWith("0.")) {
        QDateTime srvtime = QDateTime();
        srvtime.setTime_t(timestr.section('.', 0 , 0).toUInt());
        QTime msgtime = srvtime.time();
        msgstr = msgtime.toString("[hh:mm:ss.");

        msgstr.append(timestr.section('.', 1 , 1));
        msgstr.append("] ");
    }
    else
        msgstr = "[--:--:--.---] ";

    msgstr.append(msg.section(' ', 1));
    return msgstr;
}
