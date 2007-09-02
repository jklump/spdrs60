/***************************************************************************
 srcpport.h
 ----------
 Begin        : 17.08.2007
 Last modified: $Date: 2007-09-02 17:18:25 $
                $Revision: 1.1 $
 Copyright    : (C) 2007 by Guido Scholz <guido.scholz@bayernline.de>
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

#ifndef SRCPPORT_H
#define SRCPPORT_H

#include <qstring.h>
#include <qsocket.h>


class SrcpPort: public QObject {
  Q_OBJECT
   
public:
    enum CommunicationStyle{csNone, csOld, csNew, csBoth};

    SrcpPort(QObject* parent = 0, const char * name = 0,
            const char* host = "localhost", unsigned int port = 4303,
            CommunicationStyle commstyle = csBoth, bool translate = false);
    ~SrcpPort();

    void setServer(const QString&, unsigned int);
    QString getHostname() const;
    unsigned int getPortNumber() const;
    bool hasServerConnection();
    void serverConnect();
    void serverDisconnect();
    void sendToServer(const QString&);
    unsigned int getSessionId();
    QString getSrcpVersion();
    QString getSrcpOther();
    QString getSrcpServer();
    int getCurrentStyle();
    void enableTimeTranslation(bool);
    void setPreferedProtocol(const QString&);
    bool isTimeTranslationEnabled();

private:
    enum SrcpState{sNone, sLogin, sProtocol, sConnectionMode, sGo, sRun};

    QSocket* srcpSocket;
    QString host;
    unsigned int port;
    QString otherProtocol;
    CommunicationStyle commStyle;
    CommunicationStyle currentStyle;
    bool translateservertime;
    unsigned int sessionid;
    QString srcpVersion;
    QString srcpOther;
    QString srcpServer;
    SrcpState srcpState;
    bool reconnect;

    void clearConnectionData();
    QString getSocketErrorString(int);
    QString translateServerTime(const QString&);

protected:
    virtual QString getConnectionMode() = 0;
    
private slots:
    void readData();
    void hostFound();
    void socketConnected();
    void socketClosed();
    void socketDelayedClosed();
    void socketError(int);

signals:
    void connectionStateChanged(bool);
    void messageReceived(const QString&);
    void messageSend(const QString&);
    void statusMessage(const QString&);
};

#endif   //SRCPPORT_H
