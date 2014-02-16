/*
  Copyright (c) 2010 Guido Scholz <gscholz@users.sourceforge.net>

  This file is part of spdrs60.

  spdrs60 is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License version 2,
  or (at your option) any later version as published by the Free
  Software Foundation.

  spdrs60 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with spdrs60.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <qapplication.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qpopupmenu.h>

#include "transfertablepanel.h"
#include "drivedialog.h"
#include "preferences.h"

#include "pixmaps/transfertable.xpm"


TransferTablePanel::TransferTablePanel(QWidget* parent,
        SpdrItemClassId cid):
    GrayPanel(parent, cid)
{
    commander = NULL;
    activetime = 200;
    bus = 1;
    xchangeport = 0;
    state = '|';

    init();
}


TransferTablePanel::TransferTablePanel(QTextStream& ts, QWidget* parent,
        SpdrItemClassId cid):
    GrayPanel(parent, cid)
{
    commander = NULL;
    activetime = 200;
    bus = 1;
    xchangeport = 0;
    state = '|';
    visualMode = kvmNormal;

    readFileTextFromStream(ts);
    init();
}


void TransferTablePanel::init()
{
    driveaction = new QAction(tr("&Drive..."), 0, this, "driveaction");
    connect(driveaction, SIGNAL(activated()),
            this, SLOT(showDriveDialog()));
    setupElementIcon();
}


void TransferTablePanel::setupElementIcon()
{
    QPainter p(&background);

    int w = background.width();
    int h = background.height();

    // paint track
    p.fillRect(0, h / 2 - 3, 4, 7, QBrush(Qt::black));
    p.fillRect(w - 4, h / 2 - 3, 4, 7,
            QBrush(Qt::black));

    // paint table icon
    p.drawPixmap(4, 4, QPixmap(transfertable_xpm));

    //paint label
    QFont f(QApplication::font());
    f.setPointSize(QApplication::font().pointSize() - 3);
    p.setFont(f);
    QFontMetrics fm(f);
    QRect br = fm.boundingRect('O');
    br.setWidth(br.width() + 4);
    br.setHeight(br.height() + 2);
    br.moveTopLeft(QPoint(w / 2 - br.width()/2, h / 2 - br.height()/2));
    p.fillRect(br, QBrush(Qt::white));
    p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
            Qt::DontClip, state);

    setPaletteBackgroundPixmap(background);
    addTooltip();
}


void TransferTablePanel::mousePressEvent(QMouseEvent* e)
{
    /*normal mode*/
    if ((visualMode == kvmNormal) && (e->button() == Qt::LeftButton)) {

        if (commander != NULL) {
            e->accept();
            return;
        }

        commander = new elementCommander(this, classid);
        connect(commander, SIGNAL(sendTtCommand(int, int)),
                this, SLOT(slotUpdateCommanderData(int, int)));

        commander->move(QCursor::pos());
        commander->exec();
        e->accept();
    }
    else
        e->ignore();
}


void TransferTablePanel::mouseReleaseEvent(QMouseEvent* e)
{
    if ((visualMode == kvmEditLayout) &&
            (e->button() == Qt::RightButton)) {

        runPropertyMenue();
        e->accept();
    }
    else
        e->ignore();
}


void TransferTablePanel::slotUpdateCommanderData(int keyno, int port)
{
    int address = baseaddress + keyno - 1;

    if (keyno == 1)
        if (port == 0)
            state = '<';
        else
            state = '>';
    else if (port == 1)
        state = '|';

    setupElementIcon();

    SrcpMessage sm = SrcpMessage(SrcpMessage::msgGaSet);
    sm.setGaData(protocol, bus, address, port ^ xchangeport,
            1, activetime);
    emit sendSrcpMessage(&sm);
    qApp->processEvents();
}


void TransferTablePanel::runPropertyMenue()
{
    QPopupMenu menu;

    driveaction->addTo(&menu);
    menu.exec(QCursor::pos());
}


//TODO: struct {drivedata}
void TransferTablePanel::showDriveDialog()
{
    DriveDialog* dlg = new DriveDialog(this);
    if (dlg == NULL)
        return;

    /*move dialog to mouse click point*/
    dlg->move(QCursor::pos());

    dlg->setProtocol((int) SrcpMessage::proMM);
    dlg->setActiveTime(activetime);
    dlg->setSRCPBus1(bus);
    dlg->setAddress1(baseaddress);
    dlg->setXChangeConn1(xchangeport);
    dlg->setPort1(0);

    if (dlg->exec() == QDialog::Accepted) {

        activetime = dlg->getActiveTime();
        bus = dlg->getSRCPBus1();
        baseaddress = dlg->getAddress1();
        xchangeport = dlg->getXChangeConn1();

        modified = true;
        setupElementIcon();
    }

    delete dlg;
}


void TransferTablePanel::addTooltip()
{
    QString tip;

    if (!pref.datatooltips)
        return;

    QToolTip::remove(this);
    tip.sprintf("Item No: %d\n"
            "ClassId: %d\n"
            "Protocol: %s\n"
            "Base address: %d\n"
            "Active time: %d ms\n"
            "Exchange ports: %s\n"
            "State: %c",
            iSoldIndex,
            classid,
            protocol == SrcpMessage::proNone ? "N/A (=-1)"
            : (protocol == SrcpMessage::proMM ? "Motorola"
                : (protocol == SrcpMessage::proDCC ? "NMRA/DCC"
                    : (protocol == SrcpMessage::proSelectrix ?
                        "Selectrix" : "Server"))),
            baseaddress,
            activetime,
            (xchangeport == 0) ? "No" : "Yes",
            state.latin1());
    QToolTip::add(this, tip);
}


void TransferTablePanel::writeFileTextToStream(QTextStream& ts)
{
    ts << GF_CLASSID << DS << classid << endl
        << GF_INDEX << DS << iSoldIndex << endl
        << GF_PROTOCOL2  << DS << 
        ((protocol == SrcpMessage::proMM) ? "M" :
         (protocol == SrcpMessage::proDCC) ? "N" :
         (protocol == SrcpMessage::proServer) ? "P" :
         (protocol == SrcpMessage::proSelectrix) ? "S" : "-1") << endl
        << GF_ADDRESS2 << DS << bus << DS << baseaddress << endl
        << GF_ACTTIME2 << DS << activetime << endl
        << GF_XCHCONN2 << DS << xchangeport << endl
        << '%' << endl;
    modified = false;
}


void TransferTablePanel::readFileTextFromStream(QTextStream& ats)
{
    QString s, key, value;

    while (!ats.eof()) {
        s = ats.readLine();
        if (!s.startsWith("#")) {
            key = s.section(DS, 0, 0);
            value = s.section(DS, 1, 1).stripWhiteSpace();

            /* key/value pairs are read sequence independent */
            if (key.compare(GF_CLASSID) == 0) {
                  classid = (SpdrItemClassId)value.toInt();
            }
            else if (key.compare(GF_INDEX) == 0) {
                  iSoldIndex = value.stripWhiteSpace().toUInt();
            }
            else if (key.startsWith("%")) {
                /*end of dataset, exit while loop*/
                  break;
            }
            else if (key.compare(GF_PROTOCOL2) == 0) {
                if (value == "M")
                    protocol = SrcpMessage::proMM;
                else if (value == "N")
                    protocol = SrcpMessage::proDCC;
                else if (value == "P")
                    protocol = SrcpMessage::proServer;
                else if (value == "S")
                    protocol = SrcpMessage::proSelectrix;
                else
                    protocol = SrcpMessage::proNone;
            }
            else if (key.compare(GF_ADDRESS2) == 0) {
                bus = value.toInt();
                value = s.section(DS, 2, 2).stripWhiteSpace();
                baseaddress = value.toInt();
            }
            else if (key.compare(GF_XCHCONN2) == 0) {
                xchangeport = value.toInt();
            }
            else if (key.compare(GF_ACTTIME2) == 0) {
                activetime = value.toInt();
            }
        }
    }
}
