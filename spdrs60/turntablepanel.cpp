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

#include "turntablepanel.h"
#include "drivedialog.h"
#include "variantdialog.h"
#include "preferences.h"


// Maerklin turntable addresses
enum {
   MMTT14 = 209,
   MMTT15 = 225
};


TurntablePanel::TurntablePanel(QWidget* parent, SpdrItemClassId cid):
    GrayPanel(parent, cid),
    iSoldSubType(0)
{
    ttComm = NULL;
    baseaddress = MMTT15;
    activetime = 200;
    bus = 1;
    xchangeport = 0;
    sSoldText= "";

    init();
}


TurntablePanel::TurntablePanel(QTextStream& ts, QWidget* parent,
        SpdrItemClassId cid):
    GrayPanel(parent, cid),
    iSoldSubType(0)
{
    ttComm = NULL;
    baseaddress = MMTT15;
    activetime = 200;
    bus = 1;
    xchangeport = 0;
    sSoldText= "";
    visualMode = kvmNormal;

    readFileTextFromStream(ts);
    init();
}


void TurntablePanel::init()
{
    variantaction = new QAction(tr("&Variant..."), 0, this, "variantaction");
    connect(variantaction, SIGNAL(activated()),
            this, SLOT(showVariantDialog()));

    driveaction = new QAction(tr("&Drive..."), 0, this, "driveaction");
    connect(driveaction, SIGNAL(activated()),
            this, SLOT(showDriveDialog()));
    setupElementIcon();
}


void TurntablePanel::setupElementIcon()
{
    QPainter p(&background);

    int w = background.width();
    int h = background.height();

    // translate origin to center of pixmap
    p.translate(w / 2, h / 2);

    // paint track s
    int tracklen = w / 4;
    int startx = -h / 2;
    p.fillRect(startx, -3, -tracklen, 7, QBrush(Qt::black));
    p.rotate(SANGLE);
    p.fillRect(startx, -3, -tracklen - 1, 7, QBrush(Qt::black));
    p.rotate(-2 * SANGLE);
    p.fillRect(startx, -3, -tracklen - 1, 7, QBrush(Qt::black));
    p.rotate(180.0);
    p.fillRect(startx, -3, -tracklen - 1, 7, QBrush(Qt::black));
    p.rotate(SANGLE);
    p.fillRect(startx, -3, -tracklen, 7, QBrush(Qt::black));
    p.rotate(SANGLE);
    p.fillRect(startx, -3, -tracklen - 1, 7, QBrush(Qt::black));

    // paint icon
    p.setBrush(Qt::darkGray);
    p.setPen(QPen(QColor(128, 0, 0), 2));
    p.drawEllipse(-h / 2 + 1, -h / 2 + 1, h - 2, h - 2);

    // turning track, may be animated later
    p.setBrush(Qt::white);
    p.rotate(-180.0 -SANGLE/2);
    p.drawRect(-h / 2 + 2, -3, h - 4, 7);
    p.drawLine(-h / 2 + 2, 0, h / 2 - 2, 0);
    p.drawRect(6, -6, 6, 3);

    //paint label
    p.rotate(-SANGLE/2);
    p.setPen(QPen(Qt::black));
    QFont f(QApplication::font());
    f.setPointSize(QApplication::font().pointSize() - 3);
    p.setFont(f);
    QFontMetrics fm(f);
    QString s;
    s.setNum(iSoldSubType);
    QRect br = fm.boundingRect(s);
    br.setWidth(br.width() + 4);
    br.setHeight(br.height() + 2);
    br.moveTopLeft(QPoint(-br.width()/2, 5));
    p.fillRect(br, QBrush(Qt::white));
    p.drawText(br, Qt::AlignCenter | Qt::SingleLine |
            Qt::DontClip, s);

    setPaletteBackgroundPixmap(background);
    addTooltip();
}


void TurntablePanel::mousePressEvent(QMouseEvent* e)
{
    /*normal mode*/
    if ((visualMode == kvmNormal) && (e->button() == Qt::LeftButton)) {

        if (ttComm != NULL) {
            e->accept();
            return;
        }

        ttComm = new turntableCommander(sSoldText, this, iSoldSubType);
        connect(ttComm, SIGNAL(sendTtCommand(int, int)),
                this, SLOT(slotUpdateTurntableData(int, int)));
        connect(ttComm, SIGNAL(trackPositionsChanged(const QString&)),
                this, SLOT(slotCopyAvailTracks(const QString&)));

        ttComm->move(QCursor::pos());
        ttComm->exec();
        e->accept();
    }
    else
        e->ignore();
}


void TurntablePanel::mouseReleaseEvent(QMouseEvent* e)
{
    if ((visualMode == kvmEditLayout) &&
            (e->button() == Qt::RightButton)) {

        runPropertyMenue();
        e->accept();
    }
    else
        e->ignore();
}


void TurntablePanel::slotUpdateTurntableData(int keyno, int port)
{
    int address = baseaddress + keyno - 1;

    // save target track number in subtype if a track key was pressed
    if (keyno >= 4)
        iSoldSubType = keyno * 2 - 9 + port;

    setupElementIcon();

    SrcpMessage sm = SrcpMessage(SrcpMessage::msgGaSet);
    sm.setGaData(SrcpMessage::proMM, bus, address, port ^ xchangeport,
            1, activetime);
    emit sendSrcpMessage(&sm);
    qApp->processEvents();
}


void TurntablePanel::runPropertyMenue()
{
    QPopupMenu menu;

    variantaction->addTo(&menu);
    driveaction->addTo(&menu);
    menu.exec(QCursor::pos());
}


void TurntablePanel::showVariantDialog()
{
    QPixmap pm;

    VariantDialog* dlg = new VariantDialog(this);
    if (dlg == NULL)
        return;

    /*move dialog to mouse click point*/
    dlg->move(QCursor::pos());

    dlg->addVariant(tr("Typ 1&4 (base address 209)"));
    dlg->addVariant(tr("Typ 1&5 (base address 225)"));
    /*
     * index Address2
     * --------------
     *   0     209
     *   1     225
     * --------------
     */
    if (baseaddress == MMTT14)
        dlg->setChoice(0);
    else
        dlg->setChoice(1);

    if (dlg->exec() == QDialog::Accepted) {

        if (dlg->getChoice() == 0)
            baseaddress = MMTT14;
        else
            baseaddress = MMTT15;

        modified = true;
        setupElementIcon();
    }
    
    delete dlg;
}

//TODO: struct {drivedata}
void TurntablePanel::showDriveDialog()
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

/*
 * copy all available tracks at turntable into element's text
 * field, update tool tip
 */
void TurntablePanel::slotCopyAvailTracks(const QString& trackstr)
{
    sSoldText = trackstr;
    addTooltip();
}


void TurntablePanel::addTooltip()
{
    QString tip;

    if (!pref.datatooltips)
        return;

    QToolTip::remove(this);
    tip.sprintf("Item No: %d\n"
            "ClassId: %d\n"
            "Base address: %d\n"
            "Active time: %d ms\n"
            "Exchange ports: %s\n"
            "Text: %s",
            iSoldIndex,
            classid,
            baseaddress,
            activetime,
            (xchangeport == 0) ? "No" : "Yes",
            sSoldText.data());
    QToolTip::add(this, tip);
}


void TurntablePanel::writeFileTextToStream(QTextStream& ts)
{
    ts << GF_CLASSID << DS << classid << endl
        << GF_INDEX << DS << iSoldIndex << endl
        << GF_ADDRESS2 << DS << bus << DS << baseaddress << endl
        << GF_ACTTIME2 << DS << activetime << endl
        << GF_XCHCONN2 << DS << xchangeport << endl
        << GF_SUBTYPE << DS << iSoldSubType << endl
        << GF_TEXT << DS << sSoldText << endl
        << '%' << endl;
    modified = false;
}


void TurntablePanel::readFileTextFromStream(QTextStream& ats)
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
            else if (key.compare(GF_ADDRESS2) == 0) {
                bus = value.toInt();
                value = s.section(DS, 2, 2).stripWhiteSpace();
                baseaddress = value.toInt();
            }
            else if (key.compare(GF_XCHCONN2) == 0) {
                xchangeport = value.toInt();
            }
            else if (key.compare(GF_SUBTYPE) == 0) {
                iSoldSubType = value.toInt();
            }
            else if (key.compare(GF_TEXT) == 0) {
                sSoldText = s.section(DS, 1).stripWhiteSpace();
                // filter old flag for "no label text"
                if (sSoldText == "-1")
                    sSoldText = "";
            }
            else if (key.compare(GF_ACTTIME2) == 0) {
                activetime = value.toInt();
            }
        }
    }
}
