/***************************************************************************
                           element.cpp
                           version 0.4.8 $Revision: 1.4 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2005 Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2005-04-21 20:27:43 $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
   this file sets up the outfit of one element only and handles the sending
   of the switch command to the erddcd daemon
 ***************************************************************************/

#include <unistd.h>             // for write()
#include <stdio.h>              // for sprintf()

#include "element.h"

/*menu icons*/
#include "pixmaps/ctx_rota.xpm"
#include "pixmaps/ctx_straight.xpm"
#include "pixmaps/ctx_clear.xpm"
#include "pixmaps/ctx_l_curve.xpm"
#include "pixmaps/ctx_l_diag.xpm"
#include "pixmaps/ctx_l_turn.xpm"
#include "pixmaps/ctx_r_curve.xpm"
#include "pixmaps/ctx_r_diag.xpm"
#include "pixmaps/ctx_r_turn.xpm"

extern bool SHOW_HP2;
extern bool SHOW_DATA_TOOLTIPS;
extern bool INIT_SIGNALS;
extern bool SHOW_TXT_ADR;
extern bool bFBport[MAX_FB];
extern int ACTIVE_TIME;
extern int FEEDBACK;


/*constructor for element setup by QStrList*/
element::element(QStrList* elementData_, QWidget* parent): QWidget(parent)
{
    setMaximumSize(sizeHint());
    setMinimumSize(sizeHint());
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed,
                false));
    copyData(elementData_);     // copy the parameter string list
    iSoldLEDstate = 2 * bFBport[iSoldFBport];   // LED_OFF=0 or LED_RED=2*1=2
    iSoldRoutingActive = 0;     // set global vars for this ...
    iEditMode = NOEDIT;         // ... element
    bRecStaStoTimeout = false;
    sSaveReplaceIcon = "";
    sRepeatIcon = SYM_LEE;
    iSoldLocked = UNLOCKED;

    elementPropertyDlg = NULL;
    turntableProperties = NULL;
    ttComm = NULL;

    createPopupMenus();
    // init signals as they were saved in layout
    if ((sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS || sSoldIcon == SYM_SS ||
         sSoldIcon == SYM_SSH || sSoldIcon == SYM_SSS ||
         sSoldIcon == SYM_REL || sSoldIcon == SYM_WS
         || sSoldIcon == SYM_ZP || sSoldIcon == SYM_BLD
         || sSoldIcon == SYM_VS) && (INIT_SIGNALS == RED))
        iSoldDirection = 0;     // file or with red state

    if (sSoldIcon == SYM_ENK)   // couplers get the non-active direction
        iSoldDirection = 0;     // on setup

    setupElementIcon(iSoldLEDstate, sSaveReplaceIcon);  // paint element

    if (!(sSoldIcon == SYM_ENK && iSoldSubType != -1)
        && sSoldIcon != SYM_DRE && sSoldIcon != SYM_SBN
        && sSoldIcon != SYM_MDC)
        // write to socket to ensure that the solenoid
        // switch everything but momentary couplers
        // get the saved state
        makeCommand();
}


void element::createPopupMenus()
{
    /* every single element gets his own toggle popupmenu (!?) */
    // context menu with "toggle" for normal mode
    ctxNorm = new QPopupMenu(this, "");
    ctxNorm->insertItem(tr("&Toggle"), this, SLOT(slotToggle()),
                        0, CTX_ID_TOGGLE);

    QPixmap p;
    /* every single element gets his own edit popupmenu (!?) */
    // context menu with entries for edit mode
    ctxEdit = new QPopupMenu(this, "");
    ctxEdit->insertItem(tr("&Repeat"), CTX_ID_REP);
    ctxEdit->insertSeparator();

    p = QPixmap(ctx_rota_xpm);
    ctxEdit->insertItem(p, tr("R&otate"), CTX_ID_ROTATE);
    p = QPixmap(ctx_clear_xpm);
    ctxEdit->insertItem(p, tr("&Clear"), CTX_ID_CLEAR);
    ctxEdit->insertSeparator();

    p = QPixmap(ctx_straight_xpm);
    ctxEdit->insertItem(p, SYM_GER);
    p = QPixmap(ctx_l_curve_xpm);
    ctxEdit->insertItem(p, SYM_KUL);
    p = QPixmap(ctx_r_curve_xpm);
    ctxEdit->insertItem(p, SYM_KUR);
    p = QPixmap(ctx_l_diag_xpm);
    ctxEdit->insertItem(p, SYM_DIL);
    p = QPixmap(ctx_r_diag_xpm);
    ctxEdit->insertItem(p, SYM_DIR);
    p = QPixmap(ctx_l_turn_xpm);
    ctxEdit->insertItem(p, SYM_WEL);
    p = QPixmap(ctx_r_turn_xpm);
    ctxEdit->insertItem(p, SYM_WER);

    connect(ctxEdit, SIGNAL(activated(int)), this, SLOT(slotCtxEdit(int)));
}


void element::copyData(QStrList * copyData)
{
    QString data; 

    // copy a QStrList into different member variables
    data = copyData->at(LIST_ID_INDEX);
    iSoldIndex = data.toInt();

    sSoldIcon = copyData->at(LIST_ID_ICON);

    data = copyData->at(LIST_ID_ROTATE);
    iSoldRotate = data.toInt();

    data = copyData->at(LIST_ID_INVERT);
    iSoldInvert = data.toInt();

    sSoldDecoder = copyData->at(LIST_ID_DECODER);
    sSoldProtocol = copyData->at(LIST_ID_PROTOCOL);
    data = copyData->at(LIST_ID_ADDRESS_1);
    iSoldAddress_1 = data.toInt();
    data = copyData->at(LIST_ID_ADDRESS_2);
    iSoldAddress_2 = data.toInt();

    data = copyData->at(LIST_ID_CHACONN_1);
    iSoldChangeConn[0] = data.toInt();

    data = copyData->at(LIST_ID_CHACONN_2);
    iSoldChangeConn[1] = data.toInt();

    data = copyData->at(LIST_ID_DIRECTION);
    iSoldDirection = data.toInt();

    data = copyData->at(LIST_ID_SUBTYPE);
    iSoldSubType = data.toInt();

    sSoldText = copyData->at(LIST_ID_TEXT);

    data = copyData->at(LIST_ID_ACTTIME);
    iSoldActiveTime = data.toInt();

    data = copyData->at(LIST_ID_FBPORT);
    iSoldFBport = data.toInt();

    data = copyData->at(LIST_ID_LEDOFF);
    iSoldLEDoff = data.toInt();

    //sSoldData_2    = copyData->at(LIST_ID_DATA_2);
    //sSoldData_3    = copyData->at(LIST_ID_DATA_3);
}


void element::slotSwitchIt(int iNewDirection, int iLocked)
{
    // quit if element contains no solenoid, otherwise
    // "toggle all" would toggle them, too
    if (iSoldDirection == -1)
        return;

    // add another locked state cause a solenoid can belong to more than 1 route
    iSoldLocked += iLocked;

    // only save new direction and switch it if the new direction differs from
    // the old one;
    // momentary couplers are exceptional: they only use one connector e.g. one
    // direction which is activated or deactivated, these are treated the same
    if (iNewDirection != iSoldDirection || sSoldIcon == SYM_ENK) {
        iSoldDirection = iNewDirection;
        setupElementIcon(iSoldLEDstate, "");
        makeCommand();
    }
    // if the new direction equals the old one just setup the element to ensure
    // that the contextmenu and the lock variable are correct set
    // exception: 2-state-DKW, they would show a momentary false LED state
    else if (!((sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR)
               && iSoldSubType == 0))
        setupElementIcon(iSoldLEDstate, "");
}


void element::mousePressEvent(QMouseEvent * event)
{
    // normal modus -> context menu
    if ((event->button() == RightButton) &&
        (iEditMode == NOEDIT || iEditMode == R_EDIT))
        ctxNorm->exec(QCursor::pos());


    // layout edit modus -> context menu or element properties window
    if ((event->button() == RightButton) && (iEditMode == L_EDIT))
        ctxEdit->exec(QCursor::pos());
    if ((event->button() == LeftButton) && (iEditMode == L_EDIT))
        openProperties();


    // normal modus, action dependant of element type
    if ((event->button() == LeftButton) && (iEditMode == NOEDIT)) {

        GbsButtonState ctrlButton = kNoneClicked;
        QPoint CursorPos = mapFromGlobal(QCursor::pos());

        if (sSoldIcon == SYM_DRE) {
            ttComm = new turntableCommander(this, iSoldSubType, sSoldText);
            connect(ttComm, SIGNAL(applyPressed(QPoint)),
                    this, SLOT(slotUpdateTurntableData(QPoint)));
            connect(ttComm, SIGNAL(sendAvailTracks(QString)),
                    this, SLOT(slotCopyAvailTracks(QString)));

            ttComm->exec();     // parent window NOT usable
            ttComm->move(QCursor::pos());
        }

        else if (sSoldIcon == SYM_SBN || sSoldIcon == SYM_MDC) {
            turntableProperties = new elementCommander(this, sSoldIcon);
            connect(turntableProperties, SIGNAL(applyPressed(QPoint)),
                    this, SLOT(slotUpdateTurntableData(QPoint)));

            turntableProperties->exec();        // parent window NOT usable
            turntableProperties->move(QCursor::pos());
        }


        /* determine what type of button was pressed an send the
         * correspondig value to GBSArea to change cursor shape etc.*/
        // if element contains a solenoi
        else if (iSoldAddress_1 != -1){

            if (sSoldIcon == SYM_HS || sSoldIcon == SYM_NRB)
                ctrlButton = kZfsClicked;
            
            else if (sSoldIcon == SYM_SS || sSoldIcon == SYM_SSS ||
                     sSoldIcon == SYM_WS || sSoldIcon == SYM_SRB)
                ctrlButton = kRfsClicked;

            else if (sSoldIcon == SYM_HSS){
                /* two different buttons on this panel */
                if (CursorPos.x() > (EL_WIDTH >> 1) ^ (bool)iSoldRotate)
                    ctrlButton = kZfsClicked; 
                else
                    ctrlButton = kRfsClicked;
            }

            else if (sSoldIcon == SYM_SSH)
                    ctrlButton = kZhsClicked;
            
            else
                ctrlButton = kTurnoutClicked;
            
            emit elementClicked(iSoldIndex, ctrlButton);
        }

        /*add here new external button functions*/
        /*TODO: change to switch SYM_ID_ */
        else if (sSoldIcon.startsWith("taste")){
            if (sSoldIcon == SYM_TAW)
                ctrlButton = kWgtClicked;
            
            else if (sSoldIcon == SYM_TAF)
                ctrlButton = kFhtClicked;
            
            else if (sSoldIcon == SYM_TAU) {
                if (CursorPos.x() < (EL_WIDTH >> 1))
                    ctrlButton = kUfgtClicked; 
                else
                    ctrlButton = kMgtClicked;
            }
            
            else if (sSoldIcon == SYM_TAS) {
                if (CursorPos.x() < (EL_WIDTH >> 1))
                    ctrlButton = kSgtClicked; 
                else
                    ctrlButton = kHagtClicked;
            }
            emit elementClicked(iSoldIndex, ctrlButton);
        }
    }


    // routing record modus
    // left mouse-click in R_EDIT mode sends normal element index
    if ((event->button() == LeftButton) && (iEditMode == R_EDIT)) {
        
        if ((sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS ||
             sSoldIcon == SYM_SS || sSoldIcon == SYM_NRB ||
             sSoldIcon == SYM_SSH || sSoldIcon == SYM_SSS ||
             sSoldIcon == SYM_SRB)){
                
            if (bRecStaStoTimeout) {
                delete tRecStaSto;
                iEditMode = R_EDIT_CLICKED;
                emit sigElementClickedRecord(iSoldIndex, REC_STASTO);
                setupElementIcon(iSoldLEDstate, "");
                bRecStaStoTimeout = false;
            }
            else {
                bRecStaStoTimeout = true;
                // timer is for a double-click on a signal
                // only saves this as start signal
                tRecStaSto = new QTimer();
                tRecStaSto->start(500, true);
                connect(tRecStaSto, SIGNAL(timeout()),
                        this, SLOT(slotRecStaStoTimeout()));
            }
        }

        else if (sSoldIcon == SYM_WEL || sSoldIcon == SYM_WER
                 || sSoldIcon == SYM_DWL || sSoldIcon == SYM_DWR
                 || sSoldIcon == SYM_WEY || sSoldIcon == SYM_EKR
                 || sSoldIcon == SYM_EKL || sSoldIcon == SYM_DKR
                 || sSoldIcon == SYM_DKL || sSoldIcon == SYM_DRW
                 || sSoldIcon == SYM_REL || sSoldIcon == SYM_ZP
                 || sSoldIcon == SYM_BLD) {
            iEditMode = R_EDIT_CLICKED;
            emit sigElementClickedRecord(iSoldIndex, REC_NORMAL);
            setupElementIcon(iSoldLEDstate, "");
        }

        else if (sSoldIcon == SYM_LEE) {
            emit sigElementClickedRecord(0, REC_FINISH);
        }
    }

    // routing show modus
    // left button click if element is in Routing Show modus and
    // clicked element is an empty element
    if ((event->button() == LeftButton) && (iEditMode == R_SHOW)
        && (sSoldIcon == SYM_LEE))
        emit sigElementClickedRecord(0, REC_FINISH);
}


void element::slotRecStaStoTimeout()
{
    bRecStaStoTimeout = false;

    iEditMode = R_EDIT_CLICKED; // element has been clicked and a yellow frame
    emit sigElementClickedRecord(iSoldIndex, REC_NORMAL);
    setupElementIcon(iSoldLEDstate, "");
}


void element::slotShowElement(int iShowElemAddr_, int iShowElemStat_,
                              int iShowType_)
{
    if (iShowElemAddr_ == iSoldAddress_1) {
        iEditMode = iShowType_; //R_EDIT_CLICKED;
        if (iShowElemStat_ != -1)
            iSoldDirection = iShowElemStat_;
        setupElementIcon(iSoldLEDstate, "");
    }
}


void element::locateMe()
{
    iEditMode = L_EDIT;         // activate edit mode for LOCATE_TIMER secs
    setupElementIcon(iSoldLEDstate, "");        // if this is the searched element

    locateTimer = new QTimer();
    locateTimer->start(LOCATE_TIMER, true);
    connect(locateTimer, SIGNAL(timeout()),
            this, SLOT(slotLocateTimerTimeout()));
}


void element::slotLocateTimerTimeout()
{
    delete locateTimer;
    iEditMode = NOEDIT;         // show element in normal mode
    setupElementIcon(iSoldLEDstate, "");
}


void element::slotEditMode(int iEditMode_)
{
    iEditMode = iEditMode_;     // either EDIT = 1 or NOEDIT = 0
    setupElementIcon(iSoldLEDstate, "");
}


void element::slotRecordMode(int iRecMode_)
{                               // or R_SHOW = 4
    iEditMode = iRecMode_;
    // routing record = 2 = R_EDIT, no routing record = 0 = NOEDIT
    setupElementIcon(iSoldLEDstate, "");
}


void element::makeCommand()
{
    /* do not send anything for rail buttons without signals */
    if (sSoldIcon == SYM_SRB || sSoldIcon == SYM_NRB)
        return;
        
    int iRealDirection = iSoldDirection;
    // default copy of direction + address
    int iRealAddress = iSoldAddress_1;

    bool bSwitchSecondAddress = false;
  DO_AGAIN:;
    int sendRepeatCounter = 1;

    // element contains a momentarily activated coupler
    if (sSoldIcon == SYM_ENK && iSoldSubType != -1)
        iRealDirection = iSoldSubType;  // copy subtype as realDirection

    // element contains a main signal with direction >= 2
    else if ((sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS
              || sSoldIcon == SYM_VS) && iSoldDirection >= 2) {
        sendRepeatCounter = cNumRepeatCommands;

        // Hp0+Hp1 not considered, is done by default copy
        switch (iSoldSubType) {
        case 6:                // Hp0+Hp2,     --> iSoldDirection = 2
        case 7:                // Hp0+Hp2+Sh1, --> iSoldDirection = 2 or 3
            iRealDirection = 1;
            if (iSoldDirection == 3)    // Sh1, case 7, HSS
                iRealAddress = iSoldAddress_2;
            break;
        case 4:                // Hp0+Hp1+Hp2, --> iSoldDirection = 2
            iRealDirection = 0;
            iRealAddress = iSoldAddress_2;
            break;
        case 1:                // Hp0+Hp1+Sh1, --> iSoldDirection = 3
        case 5:                // Hp0+Hp1+Hp2+Sh1, --> iSoldDirection = 2 or 3
            iRealDirection = iSoldDirection - 2;
            iRealAddress = iSoldAddress_2;
            break;
        }
    }

    // element contains a 3-way-turnout
    else if (sSoldIcon == SYM_DRW) {
        switch (bSwitchSecondAddress) {
        case false:            // send first address data
            iRealDirection = iSoldDirection % 2;
            break;
        case true:             // send second address data
            iRealDirection = iSoldDirection > 1;
            iRealAddress = iSoldAddress_2;
            break;
        }
    }

    // element contains a 4-state-DKW or EKW
    else if ((sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR) ||
             ((sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR)
              && iSoldSubType == 1)) {
        switch (bSwitchSecondAddress) {
        case false:            // send first address data
            iRealDirection = iSoldDirection >= 2;
            break;
        case true:             // send second address data
            iRealDirection = (iSoldDirection == 1 || iSoldDirection == 2);
            iRealAddress = iSoldAddress_2;
            break;
        }
    }

    if (sSoldProtocol != "-1") {
        // holds the erddcd command for writing to socket
        QString sSocketCommand;

        if (sSoldIcon != SYM_DRE) {
            // the calculated real direction to be sent is modified again if you
            // electronically changed your decoder outputs of one address
            if (iRealAddress == iSoldAddress_1)
                iRealDirection = iRealDirection ^ iSoldChangeConn[0];
            if (iRealAddress == iSoldAddress_2)
                iRealDirection = iRealDirection ^ iSoldChangeConn[1];
        }

        sSocketCommand.sprintf("SET GA %s %04d %1d 1 %d\n",
                               sSoldProtocol.data(), iRealAddress,
                               (sSoldProtocol ==
                                "M") ? iRealDirection : !iRealDirection,
                               iSoldActiveTime);

        //serd: Viessman Formsignale often need several attempts
        while (sendRepeatCounter) {
            emit sendCommand(sSocketCommand);   // switch solenoid
            sendRepeatCounter--;
            if (sendRepeatCounter)
                //serd: wait 500ms only if repeating command for signal
                usleep(500 * 1000);
        }

        // return to copy direction and address for second switch
        if ((sSoldIcon == SYM_DRW || sSoldIcon == SYM_EKL
             || sSoldIcon == SYM_EKR
             || ((sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR)
                 && iSoldSubType == 1)) && !bSwitchSecondAddress) {
            bSwitchSecondAddress = true;
            goto DO_AGAIN;
        }
        else if (sSoldIcon == SYM_ENK && iSoldSubType != -1) {
            // if momentary coupler: activate it,
            // wait for a short time and deactivate it graphically
            setupElementIcon(iSoldLEDstate, ""); 
            usleep(1000 * iSoldActiveTime);
            iSoldDirection = !iSoldDirection;
            setupElementIcon(iSoldLEDstate, "");
        }
    }
}


void element::openProperties()
{
    if (elementPropertyDlg != NULL) {
        // Wenn der Dialog schon geöffnet ist, das Fenster nach vorne
        // bringen
        elementPropertyDlg->setActiveWindow();
        elementPropertyDlg->raise();
    }
    // neuen Dialog erzeugen
    else {

        QString title;
        QString insert;
        QStrList *elementData = new QStrList(true);

        // cause we copied the QStrList into member variables, we now
        // must re-copy them into a new QStrList to pass the data to
        // the properties window it did not work in this class to hold
        // the data permanently in a QStrList
        elementData->insert(LIST_ID_INDEX, insert.setNum(iSoldIndex));
        elementData->insert(LIST_ID_ICON, sSoldIcon);
        elementData->insert(LIST_ID_ROTATE, insert.setNum(iSoldRotate));
        elementData->insert(LIST_ID_INVERT, insert.setNum(iSoldInvert));
        elementData->insert(LIST_ID_DECODER, sSoldDecoder);
        elementData->insert(LIST_ID_PROTOCOL, sSoldProtocol);
        elementData->insert(LIST_ID_ADDRESS_1,
                insert.setNum(iSoldAddress_1));
        elementData->insert(LIST_ID_ADDRESS_2,
                insert.setNum(iSoldAddress_2));
        elementData->insert(LIST_ID_CHACONN_1,
                            insert.setNum(iSoldChangeConn[0]));
        elementData->insert(LIST_ID_CHACONN_2,
                            insert.setNum(iSoldChangeConn[1]));
        elementData->insert(LIST_ID_DIRECTION,
                            insert.setNum(iSoldDirection));
        elementData->insert(LIST_ID_SUBTYPE, insert.setNum(iSoldSubType));
        elementData->insert(LIST_ID_TEXT, sSoldText);
        elementData->insert(LIST_ID_ACTTIME,
                            insert.setNum(iSoldActiveTime));
        elementData->insert(LIST_ID_FBPORT, insert.setNum(iSoldFBport));
        elementData->insert(LIST_ID_LEDOFF, insert.setNum(iSoldLEDoff));
        //elementData->insert(LIST_ID_DATA_2,    sSoldData_2);
        //elementData->insert(LIST_ID_DATA_3,    sSoldData_3);

        elementPropertyDlg = new elementDialog(this, elementData);
        title.sprintf(tr("Properties of Element #%d"), iSoldIndex);
        elementPropertyDlg->setCaption(title);
        connect(elementPropertyDlg, SIGNAL(ApplyPressed()),
                this, SLOT(slotUpdateData()));
        connect(elementPropertyDlg, SIGNAL(sigShowFBmodules()),
                this, SIGNAL(sigShowFBmodules()));
        elementPropertyDlg->exec();      // parent window NOT usable
        // nach Beenden Zeiger wieder zurücksetzen:
        //delete elementPropertyDlg;
        elementPropertyDlg = NULL;
        //delete elementData;
    }
}


void element::slotUpdateData()
{
    copyData(elementPropertyDlg->listNewData);
    elementPropertyDlg->close(true);
    // copy data from properties window,
    // either LED_OFF = 0 or LED_RED = 2*1=2
    iSoldLEDstate = 2 * bFBport[iSoldFBport];
    sRepeatIcon = sSoldIcon;
    emit setRepeatIcon(sRepeatIcon);
    setupElementIcon(iSoldLEDstate, "");
}


void element::slotRepeatIcon(QString sRepeat_)
{
    QString s;

    sRepeatIcon = sRepeat_;
    s.sprintf(tr("&Repeat: %s"), sRepeatIcon.data());
    ctxEdit->changeItem(s, CTX_ID_REP);
}


void element::slotToggle()
{                               // toggles cyclic for 3-state-solenoids
    if (sSoldIcon == SYM_DRW ||
        sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR) {
        if (iSoldDirection < 2)
            slotSwitchIt(iSoldDirection + 1, UNLOCKED);
        else
            slotSwitchIt(0, UNLOCKED);
    }

    else if (sSoldIcon == SYM_HS || sSoldIcon == SYM_VS) {
        switch (iSoldDirection) {
            case 0:
                if (iSoldSubType != 6)
                    slotSwitchIt(1, UNLOCKED);
                else 
                    slotSwitchIt(2, UNLOCKED);
                break;
            case 1:
                if (iSoldSubType == 0)
                    slotSwitchIt(0, UNLOCKED);
                else
                    slotSwitchIt(2, UNLOCKED);
                break;
            case 2:
                slotSwitchIt(0, UNLOCKED);
                break;
        } 
    }

    else if (sSoldIcon == SYM_HSS) {
        switch (iSoldDirection) {
            case 0:
                if (iSoldSubType < 6)
                    slotSwitchIt(1, UNLOCKED);
                else 
                    slotSwitchIt(2, UNLOCKED);
                break;
            case 1:
                if (iSoldSubType == 1)
                    slotSwitchIt(3, UNLOCKED);
                else
                    slotSwitchIt(2, UNLOCKED);
                break;
            case 2:
                slotSwitchIt(3, UNLOCKED);
                break;
            case 3:
                slotSwitchIt(0, UNLOCKED);
                break;
        } 
    }

    // toggles cyclic for 4-state-solenoids
    else if ((sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR)
             && iSoldSubType == 1) {
        if (iSoldDirection < 3)
            slotSwitchIt(iSoldDirection + 1, UNLOCKED);
        else
            slotSwitchIt(0, UNLOCKED);
    }

    else                        // toggles cyclic for 2-state-solenoids
        slotSwitchIt(!iSoldDirection, UNLOCKED);
}


void element::slotCtxEdit(int iID_)
{
    switch (iID_) {
    case CTX_ID_REP:
        clear();
        sSoldIcon = sRepeatIcon;
        break;
        //case 1: Trennlinie
    case CTX_ID_ROTATE:
        rotate();
        break;
    case CTX_ID_CLEAR:
        clear();
        break;
        //case 4: Trennlinie
    case 5:
        clear();
        sSoldIcon = SYM_GER;    // straight
        iSoldRotate = 0;        // all symbols are rotatable
        break;
    case 6:
        clear();
        sSoldIcon = SYM_KUL;    // left curve
        iSoldRotate = 0;        // all symbols are rotatable
        break;
    case 7:
        clear();
        sSoldIcon = SYM_KUR;    // right "
        iSoldRotate = 0;        // all symbols are rotatable
        break;
    case 8:
        clear();
        sSoldIcon = SYM_DIL;    // left diagonal
        break;
    case 9:
        clear();
        sSoldIcon = SYM_DIR;    // right "
        break;
    case 10:
        clear();
        sSoldIcon = SYM_WEL;    // left turnout
        iSoldRotate = 0;        // all symbols are rotatable
        break;
    case 11:
        clear();
        sSoldIcon = SYM_WER;    // right "
        iSoldRotate = 0;        // all symbols are rotatable
        break;
    }
    iSoldFBport = 0;
    iSoldLEDoff = 0;
    iSoldLEDstate = 2 * bFBport[iSoldFBport];
    setupElementIcon(iSoldLEDstate, "");
    sRepeatIcon = sSoldIcon;
    emit setRepeatIcon(sRepeatIcon);
}


void element::rotate()
{
    iSoldRotate = !iSoldRotate; // rotates the icon
}


void element::clear()
{
    iSoldActiveTime = -1;
    iSoldChangeConn[0] = -1;
    iSoldChangeConn[1] = -1;
    iSoldDirection = -1;
    iSoldFBport = -1;
    iSoldInvert = -1;
    iSoldLEDoff = -1;
    iSoldLEDstate = LED_OFF;
    iSoldLocked = 0;
    iSoldRotate = -1;
    iSoldSubType = -1;
    iSoldAddress_1 = -1;
    iSoldAddress_2 = -1;
    sSoldDecoder = "-1";
    sSoldIcon = SYM_LEE;        // reset all solenoid data to empty
    sSoldProtocol = "-1";
    sSoldText = "-1";
    //sSoldData_2        = "-1";
    //sSoldData_3        = "-1";
}


void element::sendState()
{
    makeCommand();
}


void element::setupElementIcon(int iLEDstate_, QString sReplaceIcon)
{
    // update the contextmenu
    ctxEdit->setItemEnabled(CTX_ID_CLEAR, iEditMode == L_EDIT);
    ctxEdit->setItemEnabled(CTX_ID_ROTATE,
                            iSoldRotate != -1 && iEditMode == L_EDIT);
    ctxNorm->setItemEnabled(CTX_ID_TOGGLE,
                            (iSoldAddress_1 != -1) &&
                            (iSoldLocked == false) &&
                            (sSoldIcon != SYM_DRE) &&
                            (sSoldIcon != SYM_MDC) &&
                            (iLEDstate_ != LED_RED) &&
                            (iEditMode == NOEDIT || iEditMode == R_EDIT));

    // translate icon name and direction into a binary-coded integers
    int iIconByte = 0;
    int iDirByte = 0;

    //if (sSoldIcon == SYM_PRE)   iIconByte =  2;
    if (sSoldIcon == SYM_DIL || sReplaceIcon == SYM_DIL ||
        (sSoldIcon == SYM_KRH && sReplaceIcon == "")) {
        iIconByte = 36;
        sReplaceIcon = SYM_DIL;
    }
    else if (sSoldIcon == SYM_DIR || sReplaceIcon == SYM_DIR)
        iIconByte = 9;
    else if (sSoldIcon == SYM_KUL || sReplaceIcon == SYM_KUL)
        iIconByte = 20;
    else if (sSoldIcon == SYM_KULR || sReplaceIcon == SYM_KULR)
        iIconByte = 34;
    else if (sSoldIcon == SYM_KUR || sReplaceIcon == SYM_KUR)
        iIconByte = 17;
    else if (sSoldIcon == SYM_KURR || sReplaceIcon == SYM_KURR)
        iIconByte = 10;
    else if (sSoldIcon == SYM_GER || sSoldIcon == SYM_ENK
             || sSoldIcon == SYM_NRB || sSoldIcon == SYM_SRB
             || sSoldIcon == SYM_BUE || sSoldIcon == SYM_HS
             || sSoldIcon == SYM_HSS || sSoldIcon == SYM_SS
             || sSoldIcon == SYM_SSH || sSoldIcon == SYM_SSS
             || sSoldIcon == SYM_WS || sSoldIcon == SYM_ZP
             || sSoldIcon == SYM_BLD || sSoldIcon == SYM_RI1
             || sSoldIcon == SYM_RI2 || sReplaceIcon == SYM_GER
             || sSoldIcon == SYM_VS
             || ((sSoldIcon == SYM_KRL || sSoldIcon == SYM_KRR)
                 && sReplaceIcon == "")) {
        iIconByte = 18;
        sReplaceIcon = SYM_GER;
    }

    else if (sSoldIcon == SYM_WEL || sSoldIcon == SYM_WER) {
        iIconByte = 22 - 3 * (sSoldIcon == SYM_WER);    // 22 or 19
        iDirByte =
            (2 << iSoldDirection) >>
            ((sSoldIcon == SYM_WER && iSoldDirection != 0) * 2);
    }

    else if (sSoldIcon == SYM_DWL || sSoldIcon == SYM_DWR) {
        iIconByte = 11 + 27 * (sSoldIcon == SYM_DWR);   // 11 or 38
        iDirByte =
            (2 >> !iSoldDirection) <<
            ((sSoldIcon == SYM_DWR && iSoldDirection == 0) * 2);
    }

    else if (sSoldIcon == SYM_WEY) {
        iIconByte = 21;
        iDirByte = 1 << (iSoldDirection * 2);   // dir 0 -> byte 1; 1 -> 4
    }

    else if (sSoldIcon == SYM_DRW) {
        iIconByte = 23;
        // 0->2;1->4;2->1
        iDirByte = (2 << iSoldDirection) >> ((iSoldDirection == 2) * 3);
    }

    else if ((sSoldIcon == SYM_DKL && iSoldSubType == 1) ||
             (sSoldIcon == SYM_DKL && iSoldSubType == 0
              && sReplaceIcon == "") || sSoldIcon == SYM_EKL) {
        iIconByte = 54;
        switch (iSoldDirection) {
        case 0:
            iDirByte = 18;
            break;
        case 1:
            iDirByte = 20;
            break;
        case 2:
            iDirByte = 36;
            break;
        case 3:
            iDirByte = 34;
            break;              // only DKL
        }
    }

    else if ((sSoldIcon == SYM_DKR && iSoldSubType == 1) ||
             (sSoldIcon == SYM_DKR && iSoldSubType == 0
              && sReplaceIcon == "") || sSoldIcon == SYM_EKR) {
        iIconByte = 27;
        switch (iSoldDirection) {
        case 0:
            iDirByte = 18;
            break;
        case 1:
            iDirByte = 10;
            break;
        case 2:
            iDirByte = 9;
            break;
        case 3:
            iDirByte = 17;
            break;              // only DKR
        }
    }

    QPixmap pixLED;
    QString sPixMapName;
    QWMatrix mx;
    const char *cLEDcol[3] = { "_weiss.xpm", "_gelb.xpm", "_rot.xpm" };
    const char *cLEDpos[3] = { "LED_unten", "LED_mitte", "LED_oben" };
    int j;
    int i;
    int bHaveJumped = 0;
    // load basic icon
    sPixMapName = RES_DIR_ELEM;
    sPixMapName += sSoldIcon;
    sPixMapName += XPM_SUFFIX;
    QPixmap pixBasicIcon = QPixmap(sPixMapName);

    /*TODO: what about symbols with "taste"-name?*/
    if (iSoldLEDoff == true || sSoldIcon == SYM_LEE)
        goto LEDOFF;
        /*TODO: show white pattern on rail */

    // solenoid icons -> cross-sum of right half of iIconByte > 1
    if ((((iIconByte & 0x04) >> 2) + ((iIconByte & 0x02) >> 1) +
         ((iIconByte & 0x1) >> 0)) > 1
        && (((iIconByte & 0x32) >> 5) + ((iIconByte & 0x16) >> 4) +
            ((iIconByte & 0x8) >> 3)) == 1) {
        j = 2;                  // setup RIGHT icon half
        for (i = 4; i >= 1; i /= 2) {
            // switched branch
            if ((((iIconByte & 0x7) & i) >> j == 1) &&
                    (((iDirByte & 0x7) & i) >> j == 1)) {
                sPixMapName = RES_DIR_ELEM;
                sPixMapName += cLEDpos[j];
                sPixMapName += cLEDcol[(iLEDstate_ > 1) + 1];
                pixLED = QPixmap(sPixMapName);
            }
            // normal branch
            else if ((((iIconByte & 0x7) & i) >> j == 1) && 
                    (((iDirByte & 0x7) & i) >> j == 0)) {
                sPixMapName = RES_DIR_ELEM;
                sPixMapName += cLEDpos[j];
                sPixMapName += cLEDcol[0];
                pixLED = QPixmap(sPixMapName);
            }
            else
                pixLED.resize(0, 0);    // make a null pixmap

            bitBlt(&pixBasicIcon, 28, 0, &pixLED, 0, 0, 28, 35, OrROP,
                   false);
            j -= 1;
        }

        j = 2;                  // setup LEFT icon half == root branch
        for (i = 4; i >= 1; i /= 2) {
            if ((((iIconByte & 0x38) >> 3) & i) >> j == 1) {
                sPixMapName = RES_DIR_ELEM;
                sPixMapName += cLEDpos[j];
                sPixMapName += cLEDcol[iLEDstate_];
                pixLED = QPixmap(sPixMapName);
                mx.rotate(180);
                pixLED = pixLED.xForm(mx);
                bitBlt(&pixBasicIcon, 0, 0, &pixLED, 0, 0, 28, 35, OrROP,
                       false);
                break;
            }
            j -= 1;
        }
    }

    // solenoid icons -> cross-sum  of iIconByte == 4    --->  DKW  and  EKW
    if ((((iIconByte & 0x04) >> 2) + ((iIconByte & 0x02) >> 1) +
         ((iIconByte & 0x1) >> 0)) == 2
        && (((iIconByte & 0x32) >> 5) + ((iIconByte & 0x16) >> 4) +
            ((iIconByte & 0x8) >> 3)) == 2) {
        j = 2;                  // setup RIGHT icon half
        int k = 0;
        for (i = 4; i >= 1; i /= 2) {
            // switched branch
            if ((((iIconByte & (7 + k * 49)) >> (3 * k)) & i) >> j == 1 &&
                    (((iDirByte & (7 + k * 49)) >> (3 * k)) & i) >> j == 1) {
                sPixMapName = RES_DIR_ELEM;
                sPixMapName += cLEDpos[j];
                sPixMapName += cLEDcol[(iLEDstate_ > 1) + 1];
                pixLED = QPixmap(sPixMapName);
            }
            // normal branch
            else if ((((iIconByte & (7 + k * 49)) >> (3 * k)) & i) >> j == 1 &&
                    (((iDirByte & (7 + k * 49)) >> (3 * k)) & i) >> j == 0) {
                sPixMapName = RES_DIR_ELEM;
                sPixMapName += cLEDpos[j];
                sPixMapName += cLEDcol[0];
                pixLED = QPixmap(sPixMapName);
            }

            else
                pixLED.resize(0, 0);    // make a null pixmap

            pixLED = pixLED.xForm(mx);
            bitBlt(&pixBasicIcon, 28 * (1 - k), 0, &pixLED, 0, 0, 28, 35,
                   OrROP, false);

            // this "if" equals "break" in left icon half of
            // solenoid elements (see above)
            if (j == 0 && k == 0) {
                k = 1;
                i = 8;
                j = 2;
                mx.rotate(180);
            }
            else
                j -= 1;
        }
    }

    // none-solenoid icons -> cross-sum of right half of iIconByte == 1 +
    // normal crossings
    if ((((iIconByte & 0x4) >> 2) + ((iIconByte & 0x2) >> 1) +
         (iIconByte & 0x1)) == 1) {
      BEGIN_AGAIN:;
        j = 2;
        int k = 0;
        for (i = 4; i >= 1; i /= 2) {
            if ((((iIconByte & (7 + k * 49)) >> (3 * k)) & i) >> j == 1) {
                sPixMapName = RES_DIR_ELEM;
                sPixMapName += cLEDpos[j];
                sPixMapName += cLEDcol[iLEDstate_];
                pixLED = QPixmap(sPixMapName);
                // right half: no rotation; left half: rotate
                mx.rotate(180 * k); 
                pixLED = pixLED.xForm(mx);
                // one LED elements: put in middle of basicIcon
                if (sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS
                    || sSoldIcon == SYM_NRB || sSoldIcon == SYM_SRB
                    || sSoldIcon == SYM_VS || sSoldIcon == SYM_SS
                    || sSoldIcon == SYM_SSH || sSoldIcon == SYM_SSS
                    || sSoldIcon == SYM_RI1 || sSoldIcon == SYM_RI2
                    || sSoldIcon == SYM_WS) {
                    bitBlt(&pixBasicIcon, 14, 0, &pixLED, 0, 0, 28, 35,
                           OrROP, false);
                    break;
                }
                // two LED elements: put left and right of basicIcon
                else 
                    bitBlt(&pixBasicIcon, 28 * (1 - k), 0, &pixLED, 0, 0,
                           28, 35, OrROP, false);

                if (k == 0)     // this "if" equals "break" in left icon half of
                {               // solenoid elements (see above)
                    k = 1;      // second half = left half
                    i = 8;
                    j = 3;
                }               // do the other track in crossings or DKW/EKW
                else if (k == 1 && bHaveJumped == 0 &&
                         (sSoldIcon == SYM_KRH || sSoldIcon == SYM_KRR ||
                          sSoldIcon == SYM_KRL || (iSoldSubType == 0 &&
                                                   (sSoldIcon == SYM_DKR
                                                    || sSoldIcon ==
                                                    SYM_DKL)))) {
                    if ((sSoldIcon == SYM_KRH && sReplaceIcon == SYM_DIL)
                        || (sSoldIcon == SYM_KRR
                            && sReplaceIcon == SYM_GER)
                        || (sSoldIcon == SYM_DKR
                            && sReplaceIcon == SYM_GER))
                        iIconByte = 9;  // SYM_DIR
                    if ((sSoldIcon == SYM_KRH && sReplaceIcon == SYM_DIR)
                        || (sSoldIcon == SYM_KRL
                            && sReplaceIcon == SYM_GER)
                        || (sSoldIcon == SYM_DKL
                            && sReplaceIcon == SYM_GER))
                        iIconByte = 36; // SYM_DIL
                    if ((sSoldIcon == SYM_KRR && sReplaceIcon == SYM_DIR)
                        || (sSoldIcon == SYM_KRL
                            && sReplaceIcon == SYM_DIL)
                        || (sSoldIcon == SYM_DKR
                            && sReplaceIcon == SYM_DIR)
                        || (sSoldIcon == SYM_DKL
                            && sReplaceIcon == SYM_DIL))
                        iIconByte = 18; // SYM_GER
                    if (sSoldIcon == SYM_DKR && sReplaceIcon == SYM_KUR)
                        iIconByte = 10; // SYM_KUR rotated
                    if (sSoldIcon == SYM_DKL && sReplaceIcon == SYM_KUL)
                        iIconByte = 34; // SYM_KUL rotated
                    if (sSoldIcon == SYM_DKR && sReplaceIcon == SYM_KURR)
                        iIconByte = 17; // SYM_KUR
                    if (sSoldIcon == SYM_DKL && sReplaceIcon == SYM_KULR)
                        iIconByte = 20; // SYM_KUL

                    bHaveJumped = 1;
                    iLEDstate_ = LED_OFF;
                    mx.rotate(180);
                    goto BEGIN_AGAIN;
                }
            }
            j -= 1;
        }
    }

  LEDOFF:;
    // now in addition: setup other LEDs like signal or relais lamps

    QString sStateIcon;
    if (sSoldIcon == SYM_HS || sSoldIcon == SYM_HSS)
        switch (iSoldDirection) {
        case DIR_HP0:
            sStateIcon = "LED_hp0";
            break;              // 0
        case DIR_HP2:
            if (SHOW_HP2) {
                sStateIcon = "LED_hp2";
                break;
            }
        case DIR_HP1:
            sStateIcon = "LED_hp1";
            break;              // 1
        case DIR_SH1:
            sStateIcon = "LED_sh1";
            break;              // 3
        }

    else if (sSoldIcon == SYM_SS || sSoldIcon == SYM_ZP
             || sSoldIcon == SYM_WS || sSoldIcon == SYM_BLD
             || sSoldIcon == SYM_SSH || sSoldIcon == SYM_SSS
             )
        switch (iSoldDirection) {
        case DIR_0:
            sStateIcon = sSoldIcon + "_0";
            break;              // 0
        case DIR_1:
            sStateIcon = sSoldIcon + "_1";
            break;              // 1
        }

    else if (sSoldIcon == SYM_VS)
        switch (iSoldDirection) {
        case DIR_HP0:
            sStateIcon = "LED_vs0";
            break;              // 0
        case DIR_HP2:
            if (SHOW_HP2) {
                sStateIcon = "LED_vs2";
                break;
            }
        case DIR_HP1:
            sStateIcon = "LED_vs1";
            break;              // 1
        }

    else if (sSoldIcon == SYM_ENK) {
        switch (iSoldDirection) {
           // in a coupler element, iSoldDirection is only used as a directional
           // value if it´s a bistable coupler. Otherwise the value of iSold-
           // Direction is only used to setup the right icon on screen. The sent
           // direction is either 0 if we use the left decoder connector or 1 if
           // we use the right one. The right value to send is then obtained by
           // copying iSoldSubType value into iRealDirection (see "makeCommand")
        case DIR_ENK_DW:
            sStateIcon = sSoldIcon;
            break;              // 0
        case DIR_ENK_UP:
            sStateIcon = sSoldIcon + "_1";
            break;              // 1
        }
    }

    else if (sSoldIcon == SYM_REL)
        switch (iSoldDirection) {
        case DIR_REL0:
            sStateIcon = sSoldIcon;
            break;              // 0
        case DIR_REL1:
            sStateIcon = sSoldIcon + "_1";
            break;              // 1
        }

    // copy the additional LED pixmaps to basic icon
    sPixMapName = RES_DIR_ELEM;
    sPixMapName += sStateIcon;
    sPixMapName += XPM_SUFFIX;
    QPixmap pixStateIcon = QPixmap(sPixMapName);
    bitBlt(&pixBasicIcon, 0, 0, &pixStateIcon, 0, 0,
           EL_WIDTH, EL_HEIGHT, OrROP, false);

    // rotate the icon if necessary, but without text 
    // (it would be rotated, too!)
    QWMatrix matrix;
    matrix.rotate(180 * (iSoldRotate == 1));    // use a matrix to rotate
    QPixmap pixRotatedIcon = pixBasicIcon.xForm(matrix);


    // now add everything else like text, locked circles
    QPainter p;
    p.begin(&pixRotatedIcon);

    // paint darkgray background if empty element in inverted use
    if (sSoldIcon == SYM_LEE && iSoldInvert == 1)
        p.fillRect(0, 0, EL_WIDTH - 1, EL_HEIGHT - 1,
                                                 /*-1*/
                   QBrush(QColor(darkGray), SolidPattern));

    // setup adress/text and locked symbol
    // no text if we have a "-1"-entry
    if (sSoldText != "-1" && !sSoldText.isEmpty()) {
        // now setup the right font
        QFont f("Helvetica");
        QRect br;               // for frame around text
        QString s;              // empty and straight elements
        if (sSoldIcon == SYM_LEE || sSoldIcon == SYM_GER) {
            f.setPointSize(10);
            f.setWeight(QFont::DemiBold);
            s = sSoldText.data();
        }
        
        // address elements
        else if (sSoldIcon == SYM_ADR) {
            f.setPointSize(12);
            f.setWeight(QFont::DemiBold);
            int iAdr =
                bFBport[iSoldFBport] + 2 * (bFBport[iSoldFBport + 1]) +
                4 * (bFBport[iSoldFBport + 2]) +
                8 * (bFBport[iSoldFBport + 3]) +
                16 * (bFBport[iSoldFBport + 4]) +
                32 * (bFBport[iSoldFBport + 5]) +
                64 * (bFBport[iSoldFBport + 6]) +
                128 * (bFBport[iSoldFBport + 7]);

            // only show if loco address is different from "0"
            //if (iAdr > 0) 
            s.sprintf("%05d", iAdr);
        }
        
        else if (sSoldIcon == SYM_DRE) {
            f.setPointSize(8);
            s.sprintf("%d", iSoldSubType);
        }

        // all other switchable elements
        else {
            f.setPointSize(8);
            if (SHOW_TXT_ADR == TEXT)
                s = sSoldText.data();
            else
                s.setNum(iSoldAddress_1);
        }

        QFontMetrics fm(f);
        br = fm.boundingRect(s);
        br.setWidth(br.width() + 6);
        br.setHeight(br.height() + 4);
        p.setFont(f);

        // calculate matching textframe position
        if (sSoldIcon == SYM_GER)
            br.moveTopLeft(QPoint(EL_WIDTH / 2 - br.width() / 2,
                                  (EL_HEIGHT - br.height() -
                                   2) * iSoldRotate + !iSoldRotate * 2));
        
        else if ((sSoldIcon.startsWith("signal") && iSoldRotate == 0)
                 || (sSoldIcon == SYM_WEL && iSoldRotate == 1)
                 || (sSoldIcon == SYM_WER && iSoldRotate == 0)
                 || (sSoldIcon == SYM_MDC))
            br.moveTopLeft(QPoint(EL_WIDTH / 2 - br.width() / 2, 2));
        
        else if ((sSoldIcon.startsWith("signal") && iSoldRotate == 1) ||
                 (sSoldIcon == SYM_WEL && iSoldRotate == 0) ||
                 (sSoldIcon == SYM_WER && iSoldRotate == 1) ||
                 (sSoldIcon == SYM_ENK) ||
                 (sSoldIcon == SYM_REL) || (sSoldIcon == SYM_BLD))
            br.moveTopLeft(QPoint
                           (EL_WIDTH / 2 - br.width() / 2,
                            EL_HEIGHT - br.height() - 2));
        
        else if ((sSoldIcon == SYM_DRW && iSoldRotate == 0)
                 || (sSoldIcon == SYM_WEY && iSoldRotate == 0)
                 || (sSoldIcon == SYM_DWL && iSoldRotate == 0)
                 || (sSoldIcon == SYM_EKR && iSoldRotate == 0)
                 || (sSoldIcon == SYM_DKR))
            br.moveTopLeft(QPoint(4, EL_HEIGHT - br.height() - 2));
        
        else if ((sSoldIcon == SYM_DRW && iSoldRotate == 1) ||
                 (sSoldIcon == SYM_WEY && iSoldRotate == 1) ||
                 (sSoldIcon == SYM_DWL && iSoldRotate == 1) ||
                 (sSoldIcon == SYM_EKR && iSoldRotate == 1))
            br.moveTopLeft(QPoint(EL_WIDTH - br.width() - 4, 2));
        
        else if ((sSoldIcon == SYM_DWR && iSoldRotate == 0) ||
                 (sSoldIcon == SYM_EKL && iSoldRotate == 1))
            br.moveTopLeft(QPoint(4, 2));
        
        else if ((sSoldIcon == SYM_DWR && iSoldRotate == 1) ||
                 (sSoldIcon == SYM_EKL && iSoldRotate == 0) ||
                 (sSoldIcon == SYM_DKL))
            br.moveTopLeft(QPoint
                           (EL_WIDTH - br.width() - 4,
                            EL_HEIGHT - br.height() - 2));
        
        else if (sSoldIcon == SYM_DRE || sSoldIcon == SYM_SBN)
            br.moveTopLeft(QPoint
                           (EL_WIDTH / 2 - br.width() / 2,
                            EL_HEIGHT / 2 - br.height() / 2));
        
        else if (sSoldIcon == SYM_LEE || sSoldIcon == SYM_ADR)
            br.moveTopLeft(QPoint
                           (EL_WIDTH / 2 - br.width() / 2 + 1,
                            EL_HEIGHT / 2 - br.height() / 2));

        // paint text and frame around text
        if (sSoldIcon == SYM_SBN) {
            p.fillRect(br, QBrush(QColor(220, 220, 220)));
            p.drawRect(br);
        }
        
        else if (sSoldIcon == SYM_DRE)
            p.fillRect(br, QBrush(yellow));
        
        else if (sSoldIcon != SYM_LEE && sSoldIcon != SYM_GER
                 && sSoldIcon != SYM_ADR && !sSoldText.isEmpty()){
            //p.fillRect(br, QBrush(white)); // TODO: optional?
            p.drawRect(br);
        }

        p.setPen(blue);
        p.drawText(br.bottomLeft() + QPoint(2, -1), s.data());
        p.setPen(black);
    }

    // locked circle for solenoids
    QPoint xyLocked;

    if ((sSoldIcon == SYM_WEL && iSoldRotate == 0) ||
            (sSoldIcon == SYM_DWR && iSoldRotate == 0) ||
            (sSoldIcon == SYM_DRW && iSoldRotate == 1) ||
            (sSoldIcon == SYM_WEY && iSoldRotate == 1) ||
            (sSoldIcon == SYM_EKL && iSoldRotate == 1))
        xyLocked = QPoint(42, 23);

    else if ((sSoldIcon == SYM_WER && iSoldRotate == 0) ||
            (sSoldIcon == SYM_DWL && iSoldRotate == 0) ||
            (sSoldIcon == SYM_EKR && iSoldRotate == 0) ||
            (sSoldIcon == SYM_DKR))
        xyLocked = QPoint(42, 7);

    else if ((sSoldIcon == SYM_WEL && iSoldRotate == 1) ||
            (sSoldIcon == SYM_DWR && iSoldRotate == 1) ||
            (sSoldIcon == SYM_DRW && iSoldRotate == 0) ||
            (sSoldIcon == SYM_WEY && iSoldRotate == 0) ||
            (sSoldIcon == SYM_EKL && iSoldRotate == 0) ||
            (sSoldIcon == SYM_DKL))
        xyLocked = QPoint(10, 7);

    else if ((sSoldIcon == SYM_WER && iSoldRotate == 1) ||
            (sSoldIcon == SYM_DWL && iSoldRotate == 1) ||
            (sSoldIcon == SYM_EKR && iSoldRotate == 1))
        xyLocked = QPoint(10, 23);

    else if (sSoldIcon.left(6) == "signal")
        // 5 or 46, 25 or 5
        xyLocked = QPoint(5 + iSoldRotate * 41, 25 - iSoldRotate * 20);

    if (iSoldAddress_1 != -1 && sSoldIcon != SYM_ENK
            && sSoldIcon != SYM_REL && sSoldIcon != SYM_SBN
            && sSoldIcon != SYM_MDC && sSoldIcon != SYM_DRE
            && sSoldIcon != SYM_ADR && sSoldIcon != SYM_BLD
            && sSoldIcon != SYM_VS) {
        p.setPen(black);
        p.setBrush(iSoldLocked > 0 ? yellow : lightGray);
        p.drawEllipse(xyLocked.x(), xyLocked.y(), 5, 5);
    }

    // draw frame around every element
    // grid with thin lines (1 pixel) for mode (edit, show route, record route)
    // thicker lines (2 pixels) for selected elements - guido
    if (iEditMode == NOEDIT)
        // darkgrey if in normal mode
        p.setPen(QPen(QColor(160, 160, 164), 0, SolidLine));

    else if (iEditMode == L_EDIT)
        // thin red if in layout edit mode
        p.setPen(QPen(QColor(255, 0, 0), 0, SolidLine));

    else if (iEditMode == R_SHOW_STO)
        // thick red if in show route mode
        p.setPen(QPen(QColor(255, 0, 0), 2, SolidLine));

    else if (iEditMode == R_EDIT)
        // thin green if in record route mode
        p.setPen(QPen(QColor(0, 196, 0), 0, SolidLine));

    else if (iEditMode == R_SHOW_STA)
        // green if in show route mode
        p.setPen(QPen(QColor(0, 255, 0), 2, SolidLine));

    else if (iEditMode == R_EDIT_CLICKED || iEditMode == R_SHOW_ELM)
      // yellow if clicked element in record route mode
        p.setPen(QPen(QColor(251, 251, 0), 2, SolidLine));

    else if (iEditMode == R_SHOW)
        // blue if element in show route mode
        p.setPen(QPen(QColor(0, 0, 255), 0, SolidLine));

    p.setBrush(NoBrush);
    if (iEditMode == R_SHOW_STO || iEditMode == R_SHOW_STA ||
        iEditMode == R_SHOW_ELM || iEditMode == R_EDIT_CLICKED)
        // selection color within outer rectangle
        p.drawRect(1, 1, EL_WIDTH - 2, EL_HEIGHT - 2);
    else
        p.drawRect(0, 0, EL_WIDTH, EL_HEIGHT);
    p.end();

    // at least show previously painted element in layout and add data tooltip
    this->setBackgroundPixmap(pixRotatedIcon);
    addTooltip();
}


void element::addTooltip()
{
    QToolTip::remove(this);     // remove every tooltip
    // and if wished add new one
    // setup element's tooltip
    // with all information of the member variables
    QString a1, a2;
    a1 = QString::number(iSoldAddress_1);
    a2 = QString::number(iSoldAddress_2);
    
    if (SHOW_DATA_TOOLTIPS) {                               
        QString tip1, tip2;         

        tip1.sprintf("ELEMENT  # %03d\n"
                     "icon     : %s\n"
                     "rotate   : %s (=%1d)\n"
                     "hide LEDs: %s (=%1d)\n"
                     "invers   : %s (=%1d)\n"
                     "decoder  : %s\n"
                     "protocol : %s\n"
                     "adress 1 : %s\n",
                     iSoldIndex,
                     sSoldIcon.data(),
                     iSoldRotate == -1 ? "N/A" : (iSoldRotate ==
                                                  0 ? "No" : "Yes"),
                     iSoldRotate,
                     iSoldLEDoff == -1 ? "N/A" : (iSoldLEDoff ==
                                                  0 ? "No" : "Yes"),
                     iSoldLEDoff,
                     iSoldInvert == -1 ? "N/A" : (iSoldInvert ==
                                                  0 ? "No" : "Yes"),
                     iSoldInvert,
                     sSoldDecoder == "-1" ?  "N/A (=-1)" 
                         : sSoldDecoder.data(),
                     sSoldProtocol == "-1" ? "N/A (=-1)"
                         : (sSoldProtocol == "M" ? "Motorola" : "NMRA/DCC"),
                     iSoldAddress_1 == -1 ?  "N/A (=-1)" : a1.data());

        tip2.sprintf("adress 2 : %s\n"
                     "c conn 1 : %s (=%1d)\n"
                     "c conn 2 : %s (=%1d)\n"
                     "direction: %d\n"
                     "subtype  : %d\n"
                     "text     : %s\n"
                     "lock     : %s (=%1d)\n"
                     "time (ms): %d\n"
                     "FB module: %d\n"
                     "FB port  : %d",
                     iSoldAddress_2 == -1 ? "N/A (=-1)" : a2.data(),
                     iSoldChangeConn[0] ==
                     -1 ? "N/A" : (iSoldChangeConn[0] == 0 ? "No" : "Yes"),
                     iSoldChangeConn[0],
                     iSoldChangeConn[1] ==
                     -1 ? "N/A" : (iSoldChangeConn[1] == 0 ? "No" : "Yes"),
                     iSoldChangeConn[1], iSoldDirection, iSoldSubType,
                     sSoldText == "-1" ? "N/A (=-1)" : sSoldText.data(),
                     iSoldLocked == -1 ? "N/A" : (iSoldLocked ==
                                                  0 ? "No" : "Yes"),
                     iSoldLocked, iSoldActiveTime,
                     iSoldFBport / (16 - FEEDBACK * 8) + 1,
                     iSoldFBport % (16 - FEEDBACK * 8) + 1);

        tip1.append(tip2);

        QToolTip::setFont((QFont) "Courier");   //serd
        QToolTip::add(this, tip1);
    }
}


int element::routeElement(int S, int iNewLEDstate_, int iLastC)
{
    // this slot is always called if a routing is done (SET or RESET);
    // but "setupElementIcon" is only called either
    // with LED_OFF or LED_YEL (never with LED_RED) from this function!
    // S = routing direction
    // C = correction for index
    int D = iSoldDirection;
    int R = iSoldRotate;
    int I = sSoldIcon.contains("links", 1) ? 0 : 1;
    if ((iNewLEDstate_ != LED_OFF || iSoldLEDstate != LED_RED) &&
        (iNewLEDstate_ != LED_YEL || iSoldLEDstate != LED_RED))
        iSoldLEDstate = iNewLEDstate_;

    iSoldRoutingActive = iNewLEDstate_;

    if (sSoldIcon == SYM_GER || sSoldIcon == SYM_ENK || sSoldIcon == SYM_HS
        || sSoldIcon == SYM_NRB || sSoldIcon == SYM_SRB
        || sSoldIcon == SYM_HSS || sSoldIcon == SYM_SS
        || sSoldIcon == SYM_SSH || sSoldIcon == SYM_SSS
        || sSoldIcon == SYM_BUE || sSoldIcon == SYM_RI1
        || sSoldIcon == SYM_RI2 || sSoldIcon == SYM_WS
        || sSoldIcon == SYM_ZP || sSoldIcon == SYM_BLD
        || sSoldIcon == SYM_ADR || sSoldIcon == SYM_VS) {
        setupElementIcon(iSoldLEDstate, "");
        return 0;
    }

    // NEW ICON
    else if (sSoldIcon == SYM_WEL || sSoldIcon == SYM_WER) {
        setupElementIcon(iSoldLEDstate, "");
        if (D == 0 || !(R ^ S))
            return 0;
        if (D == 1
            && (R == 1 && S == 0 && I == 1 || R == 0 && S == 1 && I == 0))
            return -1;
        if (D == 1
            && (R == 1 && S == 0 && I == 0 || R == 0 && S == 1 && I == 1))
            return +1;
    }

    else if (sSoldIcon == SYM_DIL || sSoldIcon == SYM_DIR)      // NEW ICON
    {
        setupElementIcon(iSoldLEDstate, "");
        if (S ^ I)
            return -1;
        if (!(S ^ I))
            return +1;
    }

    else if (sSoldIcon == SYM_KUL || sSoldIcon == SYM_KUR)      // NEW ICON
    {
        setupElementIcon(iSoldLEDstate, "");
        if (!(R ^ S))
            return 0;
        if (R == 1 && S == 0 && I == 1 || R == 0 && S == 1 && I == 0)
            return -1;
        if (R == 1 && S == 0 && I == 0 || R == 0 && S == 1 && I == 1)
            return +1;
    }

    else if (sSoldIcon == SYM_DRW)      // NEW ICON
    {
        setupElementIcon(iSoldLEDstate, "");
        if (!(R ^ S) || D == 0)
            return 0;
        if (R == 0 && S == 1 && D == 1 || R == 1 && S == 0 && D == 2)
            return -1;
        if (R == 0 && S == 1 && D == 2 || R == 1 && S == 0 && D == 1)
            return +1;
    }

    else if (sSoldIcon == SYM_WEY)      // NEW ICON
    {
        setupElementIcon(iSoldLEDstate, "");
        if (!(R ^ S))
            return 0;
        if (R == 0 && S == 1 && D == 1 || R == 1 && S == 0 && D == 0)
            return -1;
        if (R == 0 && S == 1 && D == 0 || R == 1 && S == 0 && D == 1)
            return +1;
    }

    else if (sSoldIcon == SYM_DWL || sSoldIcon == SYM_DWR)      // NEW ICON
    {
        setupElementIcon(iSoldLEDstate, "");
        if ((R ^ S) && D == 1)
            return 0;
        if (S == 0 && I == 0 && (R == 0 || D == 0) || S == 1 && I == 1
            && (R == 1 || D == 0))
            return -1;
        if (S == 0 && I == 1 && (R == 0 || D == 0) || S == 1 && I == 0
            && (R == 1 || D == 0))
            return +1;
    }

    // no new icon necessary
    else if (sSoldIcon == SYM_EKL || sSoldIcon == SYM_EKR) {
        setupElementIcon(iSoldLEDstate, "");
        if (D == 0 || D == 1 && (!(R ^ I) && S == 0 || (R ^ I) && S == 1))
            return 0;
        if ((S ^ I)
            && (R == 0 && D == 1 || R == 0 && D == 2 || R == 1 && D == 2))
            return -1;
        if (!(S ^ I)
            && (R == 1 && D == 1 || R == 0 && D == 2 || R == 1 && D == 2))
            return +1;
    }

    else if ((sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR) && iSoldSubType == 1)       // 4-state-DKWs
    {
        setupElementIcon(iSoldLEDstate, "");
        if (D == 0 || S == 0 && (D == 1 && I == 0 || D == 3 && I == 1) ||
            S == 1 && (D == 3 && I == 0 || D == 1 && I == 1))
            return 0;
        if ((S ^ I) && (D == 1 || D == 2))
            return -1;
        if (!(S ^ I) && (D == 2 || D == 3))
            return +1;
    }

    else if ((sSoldIcon == SYM_DKL || sSoldIcon == SYM_DKR) && iSoldSubType == 0)       // 2-state-DKWs
    {
        QString sReplaceIcon;
        int C = 5;              // dummy value, not used !

        if (D == 0 && iLastC == 0) {
            sReplaceIcon = SYM_GER;
            C = iLastC;         // == 0
        }
        if (D == 0 && I == 0
            && (iLastC == +1 && S == 0 || iLastC == -1 && S == 1)) {
            sReplaceIcon = SYM_DIL;
            C = iLastC;         // == +-1
        }
        if (D == 0 && I == 1
            && (iLastC == -1 && S == 0 || iLastC == +1 && S == 1)) {
            sReplaceIcon = SYM_DIR;
            C = iLastC;         // == +-1
        }

        if (D == 1 && I == 0
            && (iLastC == +1 && S == 0 || iLastC == 0 && S == 1)) {
            sReplaceIcon = SYM_KUL;
            if (iLastC != 0)
                C = 0;
            if (iLastC == 0)
                C = -1;
        }
        if (D == 1 && I == 1
            && (iLastC == -1 && S == 0 || iLastC == 0 && S == 1)) {
            sReplaceIcon = SYM_KUR;
            if (iLastC != 0)
                C = 0;
            if (iLastC == 0)
                C = +1;
        }

        if (D == 1 && I == 0
            && (iLastC == -1 && S == 1 || iLastC == 0 && S == 0)) {
            sReplaceIcon = SYM_KULR;
            if (iLastC != 0)
                C = 0;
            if (iLastC == 0)
                C = +1;
        }
        if (D == 1 && I == 1
            && (iLastC == +1 && S == 1 || iLastC == 0 && S == 0)) {
            sReplaceIcon = SYM_KURR;
            if (iLastC != 0)
                C = 0;
            if (iLastC == 0)
                C = -1;
        }
        setupElementIcon((iSoldLEDstate > 1) + 1, sReplaceIcon);
        return C;
    }

    if (sSoldIcon == SYM_KRL || sSoldIcon == SYM_KRR
        || sSoldIcon == SYM_KRH) {
        QString sReplaceIcon;

        if (iLastC == 0)
            sReplaceIcon = SYM_GER;
        if (S == 0 && iLastC == +1 || S == 1 && iLastC == -1)
            sReplaceIcon = SYM_DIL;
        if (S == 0 && iLastC == -1 || S == 1 && iLastC == +1)
            sReplaceIcon = SYM_DIR;

        /*if (iSoldLEDstate == LED_OFF)
           iSoldLocked = 0;
           else
           iSoldLocked = 1; */
        iSoldLocked = !(iSoldLEDstate == LED_OFF);

        sSaveReplaceIcon = sReplaceIcon;
        setupElementIcon(iSoldLEDstate, sReplaceIcon);
        // return the same correctional value as obtained before
        return iLastC;
    }
    return iLastC;              // this line should never be reached !!!
}


void element::slotOccupyElement(unsigned int iPortNr_)
{
    QString sReplaceIcon = "";

    // this slot is always called if a feedback port toggles and does NOT reset
    // a route (done by GBSArea); but "setupElementIcon" is only called either
    // with LED_OFF or LED_RED (never with LED_YEL) from this slot !
    if ((sSoldIcon == SYM_ADR) &&
        ((iPortNr_ >> 3) * 8 == (unsigned int) iSoldFBport))
        setupElementIcon(iSoldLEDstate, "");
    // evtl. Dauer der Anzeige einstellbar ????

    else if (iPortNr_ == (unsigned int) iSoldFBport) {
        if (iSoldRoutingActive == 0)
            iSoldLEDstate = 2 * bFBport[iPortNr_];
        // either LED_OFF = 0 or LED_RED = 2*1=2
        else
            iSoldLEDstate = bFBport[iPortNr_] + 1;

        if (sSoldIcon == SYM_KRL || sSoldIcon == SYM_KRR
            || sSoldIcon == SYM_KRH)
            sReplaceIcon = sSaveReplaceIcon;    // else sReplaceIcon stays ""

        setupElementIcon(iSoldLEDstate, sReplaceIcon);
    }
}


void element::slotUpdateTurntableData(QPoint newCmd_)
{
    iSoldAddress_1 = iSoldAddress_2 + newCmd_.x() - 1;
    iSoldDirection = newCmd_.y();

    // save track# in subtype if a track key was pressed
    if (sSoldIcon == SYM_DRE && newCmd_.x() >= 4) 
        iSoldSubType = newCmd_.x() * 2 - 9 + newCmd_.y();       

    setupElementIcon(iSoldLEDstate, "");
    makeCommand();
}


void element::slotCopyAvailTracks(QString sAvailTracks_)
{
    sSoldText = sAvailTracks_;  // copy all available tracks at turntable
    addTooltip();               // into element's text field, update tooltip
}


void element::slotRepaintLayout()
{
    setupElementIcon(iSoldLEDstate, "");        // show element now with opposite
    // of text/address labels
}


QSize element::sizeHint() const
{
    return QSize(EL_WIDTH, EL_HEIGHT);
}


void element::writeFileTextToStream(QTextStream & tstream)
{
    tstream << "icon:           " << sSoldIcon << endl;
    tstream << "rotate:         " << iSoldRotate << endl;
    tstream << "invers turnout: " << iSoldInvert << endl;
    tstream << "decoder:        " << sSoldDecoder << endl;
    tstream << "protocol:       " << sSoldProtocol << endl;
    tstream << "address_1:      " << iSoldAddress_1 << endl;
    tstream << "address_2:      " << iSoldAddress_2 << endl;
    tstream << "change conn 1:  " << iSoldChangeConn[0] << endl;
    tstream << "change conn 2:  " << iSoldChangeConn[1] << endl;
    tstream << "direction:      " << iSoldDirection << endl;
    tstream << "subtype:        " << iSoldSubType << endl;
    tstream << "text:           " << sSoldText << endl;
    tstream << "active time:    " << iSoldActiveTime << endl;
    tstream << "feedback port:  " << iSoldFBport << endl;
    tstream << "hide LEDs:      " << iSoldLEDoff << endl;
    tstream << "data 2:         -1\n";
    tstream << "data 3:         -1\n";
}


QString element::getName() const
{
    return sSoldText;
}


bool element::hasSameAddress(int compAddress)
{
    return (compAddress == iSoldAddress_1);
}


bool element::isLocked()
{
    return (LOCKED == iSoldLocked);
}

