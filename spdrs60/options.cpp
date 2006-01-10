/***************************************************************************
                           options.cpp
                           version 0.4.8 $Revision: 1.8 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2006 Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2006-01-10 21:54:52 $
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
   provides an user interface to change various settings of the programm
 ***************************************************************************/
#include <ctype.h>              // for isdigit()
#include <qhbox.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qvgroupbox.h>

#include "options.h"
#include "gbsarea.h"

extern bool SHOW_HP2;
extern bool SHOW_TOOLTIPS;
extern bool SHOW_DATA_TOOLTIPS;
extern bool LOAD_DEF_LAYOUT;
extern bool INIT_SIGNALS;
extern bool SHOW_TXT_ADR;
extern int SERVER;
extern bool AUTO_ZP9;
extern bool AUTO_TT_DIR;
extern bool SERVERLOGIN;

extern int DEF_COLS;
extern int DEF_PROTOCOL;
extern int ACTIVE_TIME;
extern int ROUTING_TIME;
extern int FEEDBACK;
extern int FB_MODULES_[4];
extern int PORT;
extern double TT_ROUND_TIME;

extern QString DEF_LAYOUT;
extern QString EDITOR;
extern QString BROWSER;
extern QString DEF_DECODER;
extern QString HOST;



optionsDialog::optionsDialog(QWidget* parent)
: QTabDialog(parent, "optionsDialog", true)
{
    setupTabLayout();           // construct layout tab
    setupTabData();             // construct data tab
    setupTabInterface();        // construct server/interface tab
    fillWithData();             // fill all with data from init file
    bRepaintNecessary = false;  // no repaint necessary yet

    setCaption(tr("Preferences"));        // set a caption, resize window
    setOKButton();
    setCancelButton();    // ... and show an OK + Esc button
}


void optionsDialog::setupTabLayout()
{
    QVBox *tab1 = new QVBox(this, "tapPageOne");
    tab1->setMargin(10);
    tab1->setSpacing(10);
         
    // elements groupbox
    QButtonGroup* generalBG = new QButtonGroup(1, Qt::Horizontal,
            tr("General options"), tab1, "generalBG");
    cbShowHp2 = new QCheckBox(tr("Show &orange light for signals"
                " switched to Hp2"), generalBG, "Hp2CB");
    cbGenBubble = new QCheckBox(tr("Show &general bubblehelp (e.g."
                " for buttons; a change needs program restart)"),
            generalBG, "bubbleCB");
    cbDataBubble = new QCheckBox(tr("Show elements' &data bubblehelp"),
            generalBG, "databubbleCB");
    connect(cbDataBubble, SIGNAL(pressed()), this, SLOT(slotSetRepaint()));

    // line with two group boxes
    QHBox* solHB = new QHBox(tab1, "solHB");
    solHB->setSpacing(10);
    
    
    // left groupbox
    QButtonGroup* soladdrBG = new QButtonGroup(1, Qt::Horizontal,
            tr("Solenoids' text fields show"), solHB, "soladdrBG");
    rbShowAddr = new QRadioButton(tr("&Address (e.g. decoder address)"),
            soladdrBG);
    rbShowTxt = new QRadioButton(
            tr("&Text (e.g. turnout or signal name)"), soladdrBG);
    connect(rbShowAddr, SIGNAL(pressed()), this, SLOT(slotSetRepaint()));
    connect(rbShowTxt, SIGNAL(pressed()), this, SLOT(slotSetRepaint()));

    // right groupbox
    QButtonGroup* initsigBG = new QButtonGroup(1, Qt::Horizontal,
            tr("Initialize signals on startup"), solHB, "initsigBG");
    rbSignalRed =
        new QRadioButton(tr("A&lways on Halt (Hp0/Hp00/Sh0)"), initsigBG);
    rbSignalLay =
        new QRadioButton(tr("As &saved from previous session"), initsigBG);
   
    
    // new layout groupbox
    QGroupBox* newlayoutGB = new QGroupBox(0, Horizontal,
            tr("Default dimensions for new layouts"), tab1, "newlayoutGB");
    QVBoxLayout* newlayoutGBL = new QVBoxLayout(newlayoutGB->layout(), 6);
    
    // line with cols number spin box
    QHBoxLayout* dimcolLayout = new QHBoxLayout(newlayoutGBL);
    label = new QLabel(tr("Default new &columns for empty layout:"),
            newlayoutGB);
    dimcolLayout->addWidget(label);
    QSpacerItem* spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    dimcolLayout->addItem(spacer);
    sbDefaultCols = new QSpinBox(MIN_COLS, MAX_COLS, 1, newlayoutGB,
            "colsSB");
    dimcolLayout->addWidget(sbDefaultCols);
    sbDefaultCols->setWrapping(true);
    label->setBuddy(sbDefaultCols);

    // line with rows number spin box
    QHBoxLayout* dimrowLayout = new QHBoxLayout(newlayoutGBL);
    label = new QLabel(tr("Default new &rows for empty layout:"),
            newlayoutGB);
    dimrowLayout->addWidget(label);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    dimrowLayout->addItem(spacer);
    sbDefaultRows = new QSpinBox(MIN_ROWS, MAX_ROWS, 1, newlayoutGB,
            "rowsSB");
    dimrowLayout->addWidget(sbDefaultRows);
    sbDefaultRows->setWrapping(true);
    label->setBuddy(sbDefaultRows);


    // external program groupbox
    QGroupBox* extprogGB = new QGroupBox(0, Horizontal,
            tr("External programms"), tab1, "extprogGB");
    QVBoxLayout* extprogGBL = new QVBoxLayout(extprogGB->layout(), 6);
    
    // line with editor cb
    QHBoxLayout* editorLayout = new QHBoxLayout(extprogGBL);
    label = new QLabel(tr("&Editor for layout and preferences files:"),
            extprogGB);
    editorLayout->addWidget(label);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    editorLayout->addItem(spacer);
    coboEditor = new QComboBox(true, extprogGB);
    coboEditor->insertItem("kwrite");
    coboEditor->insertItem("kedit");
    coboEditor->insertItem("nedit");
    coboEditor->insertItem("xterm -e vim");
    coboEditor->setMinimumWidth(160); //MAGIC
    label->setBuddy(coboEditor);
    editorLayout->addWidget(coboEditor);

    // line with browser cb
    QHBoxLayout* browserLayout = new QHBoxLayout(extprogGBL);
    label = new QLabel(tr("&Browser for documentation:"), extprogGB);
    browserLayout->addWidget(label);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    browserLayout->addItem(spacer);
    coboBrowser = new QComboBox(true, extprogGB);
    coboBrowser->insertItem("firefox");
    coboBrowser->insertItem("konqueror");
    coboBrowser->insertItem("mozilla");
    coboBrowser->insertItem("netscape");
    coboBrowser->insertItem("opera");
    coboBrowser->insertItem("xterm -e links");
    coboBrowser->insertItem("xterm -e lynx");
    coboBrowser->insertItem("xterm -e w3m");
    coboBrowser->setMinimumWidth(160); //MAGIC
    label->setBuddy(coboBrowser);
    browserLayout->addWidget(coboBrowser);


    // autoload groupbox
    QGroupBox* autolayoutGB = new QGroupBox(0, Horizontal,
            tr("Autoload layout"), tab1, "autolayoutGB");
    QVBoxLayout* autoGBLayout = new
        QVBoxLayout(autolayoutGB->layout(), 6);
    
    // line with radiobutton and choose button
    QHBoxLayout* chooseLayout = new QHBoxLayout(autoGBLayout);
    cbAutoload = new QCheckBox(tr("Autoload this la&yout on startup:"),
            autolayoutGB, "autoloadCB");
    chooseLayout->addWidget(cbAutoload);
    connect(cbAutoload, SIGNAL(toggled(bool)),
            this, SLOT(slotAutoload(bool)));
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    chooseLayout->addItem(spacer);
    buttGetAutofile = new QPushButton("C&hoose...", autolayoutGB,
            "choosePB");
    chooseLayout->addWidget(buttGetAutofile);
    connect(buttGetAutofile, SIGNAL(clicked()),
            this, SLOT(slotGetAutofile()));

    // line with lineedit
    leAutoload = new QLineEdit(autolayoutGB, "autoloadLE");
    autoGBLayout->addWidget(leAutoload);

    addTab(tab1, "&Layout");
}


void optionsDialog::setupTabData()
{
    QWidget *w = new QWidget(this, "page two");

    label = new QLabel(tr("Default protocol:"), w);
    label->setGeometry(10, 12, 300, 20);

    QButtonGroup *grpBox = new QButtonGroup("", w);
    grpBox->setGeometry(10, label->y() + 20, 300, 60);
    grpBox->setFrameStyle(QFrame::NoFrame);

    rbProtMS = new QRadioButton("Märklin/Motorola", grpBox);
    rbProtMS->setGeometry(0, 10, 300, 20);
    rbProtNA = new QRadioButton("NMRA/DCC", grpBox);
    rbProtNA->setGeometry(0, 40, 300, 20);

    connect(grpBox, SIGNAL(clicked(int)),
            this, SLOT(slotProtChanged(int)));

    label = new QLabel(tr("Default solenoid decoder:"), w);
    label->setGeometry(10, grpBox->y() + grpBox->height() + 15, 300, 20);

    coboDecoder = new QComboBox(false, w);
    coboDecoder->insertItem("Märklin k83 WD (M)");
    coboDecoder->insertItem("Märklin k84 SD (M)");
    coboDecoder->insertItem("Viessm. 5211 WD (M)");
    coboDecoder->insertItem("Viessm. 5213 SD (M)");
    // Signalbaustein, extra Code!
    // coboDecoder->insertItem( "Viessmann 5210 (M)" );
    coboDecoder->insertItem("Littf. QS-DEC-II WD (M)");
    coboDecoder->insertItem("Littf. S-DEC-4 WD (M)");
    coboDecoder->insertItem("Littf. SA-DEC-4 SD (M)");
    coboDecoder->insertItem("Littf. M-DEC-MM WD (M)");
    // Signalbaustein, extra Code!
    // coboDecoder->insertItem( "Littfinsky LS-DEC (M)" );
    coboDecoder->insertItem("EDiTS WD (M)");
    coboDecoder->insertItem("EDiTS SD (M)");
    coboDecoder->insertItem("Littf. S-DEC-4 WD (D)");
    coboDecoder->insertItem("Littf. SA-DEC-4 SD (D)");
    coboDecoder->insertItem("Littf. M-DEC-DC WD (D)");
    // Signalbaustein, extra Code!
    // coboDecoder->insertItem( "Littfinsky LS-DEC (D)" );
    coboDecoder->insertItem("Lenz LS 100 WD (D)");
    coboDecoder->insertItem("Lenz LS 110 WD (D)");
    coboDecoder->insertItem("Lenz LS 130 SD (D)");

    coboDecoder->setGeometry(270, grpBox->y() + grpBox->height() + 15, 195,
                             25);
    connect(coboDecoder, SIGNAL(activated(int)), this,
            SLOT(slotDecoderChanged(int)));

    label = new QLabel(tr("Default activation time (ms):"), w);
    label->setGeometry(10, coboDecoder->y() + 32, 300, 20);

    sbActiveTime = new QSpinBox(50, 2000, 50, w, "");   // 20 ms steps
    sbActiveTime->resize(60, 25);
    sbActiveTime->move(270, label->y());
    sbActiveTime->setWrapping(true);    // enables to spin "over" the limits

    line = new QFrame(w);
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    line->setGeometry(10, label->y() + label->height() + 15, 455, 2);

    cbAutoTTDir = new QCheckBox(tr("Choose digital turntable"
                " moving-direction automatically"), w, "");
    cbAutoTTDir->move(10, line->y() + 12);
    cbAutoTTDir->resize(cbAutoTTDir->sizeHint());

    label = new QLabel(tr("Time for a 360° turn of turntable (s.ms):"), w);
    label->setGeometry(10, cbAutoTTDir->y() + 32, 300, 20);

    leTTRoundTime = new QLineEdit(w, "roundTime");
    leTTRoundTime->resize(60, 25);
    leTTRoundTime->move(270, label->y());
    leTTRoundTime->setMaxLength(6);

    line = new QFrame(w);
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    line->setGeometry(10, label->y() + label->height() + 15, 455, 2);

    label = new QLabel(tr("Routing delay per element (ms):"), w);
    label->setGeometry(10, line->y() + 15, 300, 20);

    sbRoutingTime = new QSpinBox(20, 1000, 20, w, "");  // 50 ms steps
    sbRoutingTime->resize(60, 25);
    sbRoutingTime->move(270, line->y() + 15);
    sbRoutingTime->setWrapping(true);   // enables to spin "over" the limits

    line = new QFrame(w);
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    line->setGeometry(10, label->y() + label->height() + 15, 455, 2);

    grpBox = new QButtonGroup("", w);
    grpBox->setGeometry(10, line->y() + 35, 460, 60);
    grpBox->setFrameStyle(QFrame::NoFrame);

    rbS88_16 = new QRadioButton(tr("s88 (or compatible) with 16 ports"
                " per module (Märklin, Viessmann)"), grpBox);
    rbS88_16->move(0, 10);
    rbS88_16->resize(rbS88_16->sizeHint());
    rbS88_8 = new QRadioButton(tr("s88 (or compatible) with 8 ports per"
                " module (EDiTS, Friberg)"), grpBox);
    rbS88_8->move(0, 40);
    rbS88_8->resize(rbS88_8->sizeHint());

    label = new QLabel(tr("Feedback type:"), w);
    label->move(10, line->y() + 15);
    label->resize(label->sizeHint());

    connect(grpBox, SIGNAL(clicked(int)),
            this, SLOT(slotLimitModules(int)));

    label = new QLabel(tr("Number of s88-modules on bus:"), w);
    label->setGeometry(10, grpBox->y() + grpBox->height() + 10, 300, 30);
    QLabel *label2 = new QLabel(tr("#1"), w);
    label2->move(10, label->y() + 30);
    label2->resize(label2->sizeHint());
    label2 = new QLabel(tr("#2"), w);
    label2->move(10 + 58, label->y() + 30);
    label2->resize(label2->sizeHint());
    label2 = new QLabel(tr("#3"), w);
    label2->move(10 + 2 * 58, label->y() + 30);
    label2->resize(label2->sizeHint());
    label2 = new QLabel(tr("#4"), w);
    label2->move(10 + 3 * 58, label->y() + 30);
    label2->resize(label2->sizeHint());

    sbFBmod_1 = new QSpinBox(0, 31, 1, w, "");
    sbFBmod_1->resize(50, 25);
    sbFBmod_1->move(10, label2->y() + 15);
    sbFBmod_1->setWrapping(true);

    sbFBmod_2 = new QSpinBox(0, 31, 1, w, "");
    sbFBmod_2->resize(sbFBmod_1->size());
    sbFBmod_2->move(sbFBmod_1->x() + sbFBmod_1->width() + 8,
                    sbFBmod_1->y());
    sbFBmod_2->setWrapping(true);

    sbFBmod_3 = new QSpinBox(0, 31, 1, w, "");
    sbFBmod_3->resize(sbFBmod_1->size());
    sbFBmod_3->move(sbFBmod_2->x() + sbFBmod_2->width() + 8,
                    sbFBmod_1->y());
    sbFBmod_3->setWrapping(true);

    sbFBmod_4 = new QSpinBox(0, 31, 1, w, "");
    sbFBmod_4->resize(sbFBmod_1->size());
    sbFBmod_4->move(sbFBmod_3->x() + sbFBmod_3->width() + 8,
                    sbFBmod_1->y());
    sbFBmod_4->setWrapping(true);

    w->setFixedSize(500, TABHEIGHT);
    this->addTab(w, tr("&Digital Data"));
}


void optionsDialog::setupTabInterface()
{
    QWidget *w = new QWidget(this, "page three");

    QButtonGroup *grpBox = new QButtonGroup("", w);
    grpBox->setGeometry(10, 3, 300, 300);
    grpBox->setFrameStyle(QFrame::NoFrame);

    label = new QLabel(tr("Server connected via TCP/IP:"), w);
    label->resize(label->sizeHint());
    label->move(10, 10);

    rbServer = new QRadioButton(tr("SRCP-Server (DDL, m6051d)"), grpBox);
    rbServer->setGeometry(0, label->y() + 25, 300, 20);

    lServerIP = new QLabel(tr("Hostname (IP or DNS):"), grpBox);
    lServerIP->setGeometry(20, rbServer->y() + 40, 200, 20);

    leHost = new QLineEdit(w, "host");
    leHost->setGeometry(230, rbServer->y() + 40, 150, 25);
    leHost->setMaxLength(15);

    lServerPort = new QLabel(tr("Portnumber (10000-65535):"), grpBox);
    lServerPort->setGeometry(20, lServerIP->y() + 30, 200, 20);

    lePort = new QLineEdit(w, "port");
    lePort->setGeometry(230, lServerIP->y() + 30, 150, 25);
    lePort->setMaxLength(5);

    connect(lePort, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotPortChanged(const QString&)));

    /*new in qt3_17 */
    cbAutologin = new QCheckBox(tr("Server login on startup"), w, "");
    cbAutologin->move(31, lServerPort->y() + 34);
    cbAutologin->resize(cbAutologin->sizeHint());

    cbAutoZP9 = new QCheckBox(tr("Autostart voltage on layout"), w, "");
    cbAutoZP9->move(31, lServerPort->y() + 70);
    cbAutoZP9->resize(cbAutoZP9->sizeHint());

    w->setFixedSize(500, TABHEIGHT);
    this->addTab(w, tr("&Server/Interface"));
}


void optionsDialog::slotGetAutofile()
{
    QString filename = QFileDialog::getOpenFileName(QDir::homeDirPath(),
                           QString(tr("Layouts")) +
                           " (*" + GF_GBSEXT + ")", this);

    if (!filename.isEmpty())
        leAutoload->setText(filename);
}


void optionsDialog::slotAutoload(bool bEnable_)
{
    // if autoload is (de-)selected, do the same with filebutton and namefield
    leAutoload->setEnabled(bEnable_);
    buttGetAutofile->setEnabled(bEnable_);
}


void optionsDialog::slotDecoderChanged(int)
{
    QString sText = coboDecoder->currentText();

    // default decoder has changed -> set the appropriate default protocol
    if (sText.right(3) == "(M)")
        rbProtMS->setChecked(true);
    else if (sText.right(3) == "(D)")
        rbProtNA->setChecked(true);
}


void optionsDialog::slotProtChanged(int)
{
    QString sProt = (rbProtMS->isChecked() == 1) ? "(M)" : "(D)";
    QString sText;

    // default protocol has changed -> set the appropriate default decoder
    for (int i = 0; i < coboDecoder->count(); i++) {
        sText = coboDecoder->text(i);
        if (sText.right(3) == sProt) {
            coboDecoder->setCurrentItem(i);
            break;
        }
    }
}


// depending on feedback module type (16 or 8 port) set the spin box ranges
void optionsDialog::slotLimitModules(int iType_)
{
    sbFBmod_1->setRange(0, 31 + iType_ * 31);
    sbFBmod_2->setRange(0, 31 + iType_ * 31);
    sbFBmod_3->setRange(0, 31 + iType_ * 31);
    sbFBmod_4->setRange(0, 31 + iType_ * 31);
}


void optionsDialog::slotPortChanged(const QString & cNewPort_)
{
    QString sCorrection = cNewPort_;

    // reject character input if it is not a digit
    for (uint i = 0; i < sCorrection.length(); i++)
        if (isdigit(cNewPort_[i]) == false) {
            sCorrection.replace(i, 1, '\0');    // correction code replaces
            lePort->setText(sCorrection);       // the wrong user entry in
        }                       // line edits
}


void optionsDialog::slotSetRepaint()
{
    // user has changed options that need a layout repaint
    bRepaintNecessary = true;
}


void optionsDialog::fillWithData()
{
    int i;

    // layout section
    cbShowHp2->setChecked(SHOW_HP2);
    cbGenBubble->setChecked(SHOW_TOOLTIPS);
    cbDataBubble->setChecked(SHOW_DATA_TOOLTIPS);
    rbShowAddr->setChecked(SHOW_TXT_ADR == ADDRESS);
    rbShowTxt->setChecked(SHOW_TXT_ADR == TEXT);
    rbSignalRed->setChecked(INIT_SIGNALS == RED);
    rbSignalLay->setChecked(INIT_SIGNALS == LAYOUT);
    sbDefaultCols->setValue(DEF_COLS);
    cbAutoload->setChecked(LOAD_DEF_LAYOUT);

    //if (DEF_LAYOUT != "-1" && LOAD_DEF_LAYOUT != 0 )
    if (DEF_LAYOUT != "-1" && LOAD_DEF_LAYOUT)
        leAutoload->setText(DEF_LAYOUT);
    else if (DEF_LAYOUT == "-1")
        slotAutoload(false);

    bool editorfound = false;
    for (i = 0; i < coboEditor->count(); i++) {
        if (coboEditor->text(i) == EDITOR) {
            coboEditor->setCurrentItem(i);
            editorfound = true;
            break;
        }
    }
    if (!editorfound) {
        coboEditor->insertItem(EDITOR);
        coboEditor->setCurrentItem(coboEditor->count() - 1);
    }
    
    bool browserfound = false;
    for (i = 0; i < coboBrowser->count(); i++) {
        if (coboBrowser->text(i) == BROWSER) {
            coboBrowser->setCurrentItem(i);
            browserfound = true;
            break;
        }
    }
    if (!browserfound) {
        coboBrowser->insertItem(BROWSER);
        coboBrowser->setCurrentItem(coboBrowser->count() - 1);
    }

    // data section
    rbProtMS->setChecked(DEF_PROTOCOL == PROT_MS);
    rbProtNA->setChecked(DEF_PROTOCOL == PROT_NA);

    for (i = 0; i < coboDecoder->count(); i++) {
        if (coboDecoder->text(i) == DEF_DECODER) {
            coboDecoder->setCurrentItem(i);
            break;
        }
    }
    sbActiveTime->setValue(ACTIVE_TIME);
    cbAutoTTDir->setChecked(AUTO_TT_DIR);

    QString sText;
    sText.sprintf("%.2f", TT_ROUND_TIME);
    leTTRoundTime->setText(sText);

    cbAutoZP9->setChecked(AUTO_ZP9);
    sbRoutingTime->setValue(ROUTING_TIME);
    rbS88_16->setChecked(FEEDBACK == FB_16);
    rbS88_8->setChecked(FEEDBACK == FB_8);
    sbFBmod_1->setValue(FB_MODULES_[0]);
    sbFBmod_2->setValue(FB_MODULES_[1]);
    sbFBmod_3->setValue(FB_MODULES_[2]);
    sbFBmod_4->setValue(FB_MODULES_[3]);
    slotLimitModules(FEEDBACK);

    // interface section
    if (HOST != "-1")
        leHost->setText(HOST);
    else
        leHost->setText("");

    if (PORT != -1)
        lePort->setText(sText.setNum(PORT));
    else
        lePort->setText("");

    cbAutologin->setChecked(SERVERLOGIN);
    rbServer->setChecked(SERVER == SRCP);
    iSelectedServer = SERVER;   // save the server type from config file
}


void optionsDialog::done(int r)
{
    // user has clicked OK to accept changes
    if (r == QDialog::Accepted) {
        // something's wrong or missing, so don't leave this dialog
        if (checkForWarnings() == INVALID)
            return;

        // everything is okay, write config file and leave dialog
        QFile file(QDir::homeDirPath() + "/" + SPDRS60_INIT);
        QString s;

        if (!file.open(IO_WriteOnly)) {
            emit showLogMessage(tr("Error: Could not save configuration"
                        " file: ~/%1").arg(SPDRS60_INIT), MT_INFO, HL_CMND);
            return;
        }

        emit showLogMessage(tr("Writing SpDrS60 configuration"
                    " file: ~/%1").arg(SPDRS60_INIT), MT_INFO, HL_CMND);

        QDateTime dt = QDateTime::currentDateTime();
        QTextStream ts(&file);

        ts << "# SpDrS60 for Linux config file" << endl;
        ts << "# Last modified: " << dt.toString(Qt::ISODate) << endl;
        ts << "#" << endl;
        ts << "# LAYOUT SECTION" << endl;
        ts << "#" << endl;
        ts << "show hp2:       " << (int) cbShowHp2->isChecked() << endl;
        ts << "show gen bubb:  " << (int) cbGenBubble->isChecked() << endl;
        ts << "show data bubb: " << (int) cbDataBubble->
            isChecked() << endl;
        ts << "in text fields: " << ((rbShowAddr->isChecked() == 1) ?
                                     "address" : "text") << endl;
        ts << "init signals as:" << ((rbSignalRed->isChecked() == 1) ?
                                     "red" : "saved") << endl;
        ts << "def new cols:   " << sbDefaultCols->value() << endl;
        ts << "autoloader:     " << (int) cbAutoload->isChecked() << endl;
        ts << "autoload file:  " << ((cbAutoload->isChecked() == 1) ?
                                     leAutoload->
                                     text() : (QString) "-1") << endl;
        ts << "editor name:    " << coboEditor->currentText() << endl;
        ts << "browser name:   " << coboBrowser->currentText() << endl;
        ts << "#" << endl;
        ts << "# DATA SECTION" << endl;
        ts << "#" << endl;
        ts << "def protocol:   " << ((rbProtMS->isChecked() == 1) ?
                                     "Motorola" : "DCC") << endl;
        ts << "def decoder:    " << coboDecoder->currentText() << endl;
        ts << "activation time:" << sbActiveTime->value() << endl;
        ts << "auto tt direct.:" << (int) cbAutoTTDir->isChecked() << endl;
        ts << "tt round time:  " << leTTRoundTime->text() << endl;
        ts << "auto ZP 9:      " << (int) cbAutoZP9->isChecked() << endl;
        ts << "routing delay:  " << sbRoutingTime->value() << endl;
        if (rbS88_16->isChecked() == 1)
            ts << "feedback type:  " << "S88_16" << endl;
        if (rbS88_8->isChecked() == 1)
            ts << "feedback type:  " << "S88_8" << endl;
        ts << "modules bus #1: " << sbFBmod_1->value() << endl;
        ts << "modules bus #2: " << sbFBmod_2->value() << endl;
        ts << "modules bus #3: " << sbFBmod_3->value() << endl;
        ts << "modules bus #4: " << sbFBmod_4->value() << endl;
        ts << "#" << endl;
        ts << "# SERVER/INTERFACE SECTION" << endl;
        ts << "#" << endl;
        if (rbServer->isChecked())
            ts << "server:         " << "SRCP" << endl;

        ts << "hostname:       " << ((rbServer->isChecked() == 1) ?
                                     leHost->
                                     text() : (QString) "-1") << endl;
        ts << "port number:    " << ((rbServer->isChecked() == 1) ?
                                     lePort->
                                     text() : (QString) "-1") << endl;
        ts << "comport:        " << "-1" << endl;
        ts << "baud:           " << "-1" << endl;
        ts << "databits:       " << "-1" << endl;
        ts << "stoppbits:      " << "-1" << endl;
        ts << "parity:         " << "-1" << endl;
        ts << "autologin:      " << (int) cbAutologin->isChecked() << endl;

        file.close();

        emit refreshConfigData();       // in MainWindow re-read Config file
        if (bRepaintNecessary)  // repaint layout if f.e. no bubblehelp
            emit repaintLayout();       // is wished
    }
    QDialog::done(r);           // close this dialog
}


int optionsDialog::checkForWarnings()
{
    QString sText;

    sText = leAutoload->text();
    if (sText == "" && leAutoload->isEnabled() == true) {
        qApp->beep();
        QMessageBox::warning(this,
                             tr("Filename missing"),
                             tr("You have enabled the autoloader,\n"
                                "so you must select a filename, too.\n"),
                             tr("&OK"), 0, 0, 0, 0);
        // butt 1: OK, butt 2+3: not avail.
        // <ENTER> + <ESC> default to butt 0 = OK
        buttGetAutofile->setFocus();
        return INVALID;
    }

    sText = leTTRoundTime->text();
    if (sText == "") {
        qApp->beep();
        QMessageBox::warning(this,
                             tr("Turntable at lightspeed"),
                             tr("Please specify the total time\n"
                                "that a turntable needs for a whole\n"
                                "360° turn.\n"),
                             tr("&OK"), 0, 0, 0, 0);
        // butt 1: OK, butt 2+3: not avail.
        // <ENTER> + <ESC> default to butt 0 = OK
        leTTRoundTime->setFocus();
        return INVALID;
    }

    sText = leHost->text();
    if (sText == "" && leHost->isEnabled() == true) {
        qApp->beep();
        QMessageBox::warning(this,
                             tr("Hostname missing"),
                             tr("You have selected a SRCP server,\n"
                                "so you must enter a hostname\n"
                                "(DNS name or IP address) too.\n"),
                             tr("&OK"), 0, 0, 0, 0);
        // butt 1: OK, butt 2+3: not avail.
        // <ENTER> + <ESC> default to butt 0 = OK
        leHost->setFocus();
        return INVALID;
    }

    sText = lePort->text();
    int iPortValue = sText.toInt();
    if (sText == "" && lePort->isEnabled() == true ||
        iPortValue < 10000 || iPortValue > 65535) {
        qApp->beep();
        QMessageBox::warning(this,
                             tr("Port number missing"),
                             tr("You have selected a SRCP server,\n"
                                "so you must enter a valid port number, too\n"
                                "(Value between 10000 and 65535)."),
                             tr("&OK"), 0, 0, 0, 0);
        // butt 1: OK, butt 2+3: not avail.
        // <ENTER> + <ESC> default to butt 0 = OK
        lePort->setFocus();
        return INVALID;
    }

    if (sbFBmod_1->value() == 0 && sbFBmod_2->value() == 0 &&
        sbFBmod_3->value() == 0 && sbFBmod_4->value() == 0) {
        qApp->beep();
        QMessageBox::information(this,
                                 tr("Feedback modules missing"),
                                 tr
                                 ("On every feedback bus there seem to be\n"
                                  "no feedback modules.\n\n"
                                  "I assume that at least 1 module\n"
                                  "is connected to bus #1."), tr("&Yes"),
                                 0, 0, 0, 0);
        // butt 1: Yes, butt 2+3: N/A.
        // <ENTER> + <ESC> default to butt 0 = OK
        sbFBmod_1->setValue(1);
        return VALID;
    }

    return VALID;               // everything is correct, so return VALID
}
