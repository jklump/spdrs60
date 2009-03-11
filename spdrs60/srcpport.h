/***************************************************************************
 srcpport.h
 ----------
 Begin        : 17.08.2007
 Last modified: $Date: 2009-03-11 19:36:38 $
                $Revision: 1.4 $
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
    void setServer(CommunicationStyle, const QString&, unsigned int);
    void setCommunicationStyle(CommunicationStyle);
    void setPreferedProtocol(const QString&);
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
    bool isTimeTranslationEnabled();

protected:
    enum SrcpState{sNone, sLogin, sProtocol, sConnectionMode, sGo, sRun};
    SrcpState srcpState;
    virtual void setPresetState();

    CommunicationStyle commStyle;
    CommunicationStyle currentStyle;
    virtual QString getConnectionMode() = 0;
    
private:
    QSocket* srcpSocket;
    QString host;
    unsigned int port;
    QString otherProtocol;
    bool translateservertime;
    unsigned int sessionid;
    QString srcpVersion;
    QString srcpOther;
    QString srcpServer;
    bool reconnect;

    void clearConnectionData();
    QString getSocketErrorString(int);
    QString translateServerTime(const QString&);

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
