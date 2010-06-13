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


#ifndef SPDRPANEL_H
#define SPDRPANEL_H

#include <qwidget.h>

/*element selection modes, shown as an inner rectangle*/
enum elemSelectionMode {
    ksmNormal = 0,
    ksmSelected,
    ksmStopSig,
    ksmStartSig,
    ksmDisplay,
    ksmSwitchEl,
    ksmFoundEl,
    ksmDropTarget
};

/*element visual modes, shown as colored right and bottom line*/
enum elemVisualMode {
    kvmNormal = 0,
    kvmEditLayout,
    kvmEditRoute,
    kvmEditClearance
};

// TODO: adjust width to 55 (56 has no center)
enum {
    EL_WIDTH  = 56, // width of an element in pixels (orig: 54 mm)
    EL_HEIGHT = 35  // height of an element in pixels (orig: 34 mm)
};                  // 8 * H = 5 * W = 280
                    // diagonal: 65.513 pixels (63.812)
                    // alpha: 31.264° (32.196°)
                    // beta: 58.736°  (57.804°)

/*some magic strings for reading and writing layout files*/
static const char GF_INDEX[]     = "index";
static const char GF_CLASSID[]   = "classid";
static const char DS[]  = ";";   // data separator in spdrs60 files


class SpdrPanel: public QWidget
{
    Q_OBJECT

public:
    enum SpdrItemClassId {
        siciNone = 0,
        siciSt1 = 100, siciSt2, siciSt3, siciSt4, // straight track
        siciCr1 = 120, siciCr2, siciCr3, siciCr4, // right curves
        siciCl1 = 130, siciCl2, siciCl3, siciCl4, // left curves
        siciKrh = 150, siciKr1, siciKr2, siciKl1, siciKl2, //Kr2, Kl2 not used
        siciTuh = 160, siciTuv, siciTul, siciTur, // bridges
        siciTdr = 170, siciTdl, siciTdb,          // arrows
        siciZt1 = 200, siciZt2, siciZt3, siciZt4, //Train rt btn 2, 4 not used
        siciRt1 = 210, siciRt2, siciRt3, siciRt4, //Shanting rt btn2, 4 not used
        siciZp1 = 250, siciZp2, siciZp3, siciZp4, //2, 4 not used
        siciHs1 = 300, siciHs2, siciHs3, siciHs4, //Main signal 2, 4 not used
        siciHv1 = 310, siciHv2, siciHv3, siciHv4, //Hs + Vs not used
        siciHss1 = 320, siciHss2, siciHss3, siciHss4,//Hss; 2 + 4 not used
        siciHvs1 = 330, siciHvs2, siciHvs3, siciHvs4,//Hss + Vs ext. not used
        siciSs1 = 340, siciSs2, siciSs3, siciSs4, //Sh signal; 2, 4 not used
        siciSh1 = 350, siciSh2, siciSh3, siciSh4, //Sh help route; 2, 4 not used
        siciSd1 = 360, siciSd2, siciSd3, siciSd4, //Sh two buttons; 2, 4 not used
        siciWs1 = 370, siciWs2, siciWs3, siciWs4, //2, 4 not used
        siciVs1 = 380, siciVs2, siciVs3, siciVs4, //2, 4 not used
        siciVx1 = 390, siciVx2, siciVx3, siciVx4, // Vs extension for Hss n. u.
        siciSb1 = 400, siciSb2, siciSb3, siciSb4, // Selbstblocksignal
        siciZb1 = 410, siciZb2, siciZb3, siciZb4, // Zentralblocksignal
        siciZv1 = 420, siciZv2, siciZv3, siciZv4, // Zentralblocksignal + Vs
        siciTr1 = 500, siciTr2, siciTr3, siciTr4, // right turnouts
        siciTl1 = 510, siciTl2, siciTl3, siciTl4, // left turnouts
        siciIr1 = 520, siciIr2, siciIr3, siciIr4, //diagonal to. 2, 4 not used
        siciIl1 = 530, siciIl2, siciIl3, siciIl4, //diagonal to. 2, 4 not used
        siciSy1 = 550, siciSy2, siciSy3, siciSy4, //y turnout 2, 4 not used
        siciSr1 = 600, siciSr2, siciSr3, siciSr4, // Single slip switch right
        siciSl1 = 610, siciSl2, siciSl3, siciSl4, // Single slip switch left
        siciDr1 = 620, siciDr2, siciDl1, siciDl2, //Double slip switch 2 n. u.
        siciTw1 = 650, siciTw2, siciTw3, siciTw4, //2, 4 not used
        siciDre = 700, siciSbn, // turntable, shiftbridge
        siciRel = 710, //relais
        siciMdc = 720, //DC motor
        siciBs1 = 800, siciBs2, siciBs3, siciBs4,
        siciLs1 = 820, siciLs2, siciLs3, siciLs4, //2, 4 not used
        siciLt1 = 830, siciLt2, siciLt3, siciLt4, //2, 4 not used
        siciLb1 = 840, siciLb2, siciLb3, siciLb4, //2, 4 not used
        siciBuc = 850, siciBul, siciBur,
        siciAdr = 900, // address indicator
        siciEnk = 930, // decoupler
        siciBld = 940, // blind element
        siciBue = 950, // level crossing
        siciTxt = 1000, // text only
        siciFeg = 1100, siciTaf, siciTau, // Green, FHT, UfGT
        siciFeb = 1200, siciTaw, siciTwh, // Blue, WGT, WHT
        siciFer = 1300, siciTas, // Red, SGT
        siciFey = 1400, // Yellow
        siciFen = 1500, // Brown
        siciFee = 1600, siciTal // Grey, Ein
    };

    protected:
        SpdrItemClassId classid;
        elemSelectionMode selectionMode;
        elemVisualMode visualMode;
        unsigned int iSoldIndex;
        bool modified;

    public:
        SpdrPanel(QWidget* parent = NULL);
        virtual void readFileTextFromStream(QTextStream&) = 0;
        virtual void writeFileTextToStream(QTextStream&);
        void setIndexNo(unsigned int);
        unsigned int getIndexNo();
        elemSelectionMode getSelectionMode();
        void setDropTargetView(bool);
        bool isRoutable();
        bool isModified();
        SpdrPanel::SpdrItemClassId classId();

    public slots:
        void switchSelectionMode(elemSelectionMode);
        void switchVisualMode(elemVisualMode);
};

#endif  //SPDRPANEL_H
