/***************************************************************************
                           options.cpp
                           version 0.4.8 $Revision: 1.10 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2006 Guido Scholz
    email                : stefan.preis@wdr.de
    last modified        : $Date: 2006-01-16 14:51:32 $
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
extern bool AUTO_TT_DIR;

extern int DEF_PROTOCOL;
extern int ACTIVE_TIME;
extern int ROUTING_TIME;
extern int FEEDBACK;
extern int FB_MODULES_[4];
extern double TT_ROUND_TIME;

extern QString DEF_LAYOUT;
extern QString EDITOR;
extern QString BROWSER;
extern QString DEF_DECODER;



optionsDialog::optionsDialog(QWidget* parent)
: QTabDialog(parent, "optionsDialog", true)
{
    setupLayoutTab();
    setupElementTab();
    setupDigitalTab();
    setupFeedbackTab();
    fillWithData();             // fill all with data from init file
    bRepaintNecessary = false;  // no repaint necessary yet

    setCaption(tr("User preferences"));
    setOKButton();
    setCancelButton();
}


void optionsDialog::setupLayoutTab()
{
    QWidget *w = new QWidget(this, "tabpageone");
    QVBoxLayout* tabL = new QVBoxLayout(w, 10);
    
    // new layout groupbox
    QGroupBox* newlayoutGB = new QGroupBox(0, Horizontal,
            tr("Default dimensions for new layouts"), w, "newlayoutGB");
    QVBoxLayout* newlayoutGBL = new QVBoxLayout(newlayoutGB->layout(), 6);
    tabL->addWidget(newlayoutGB);
    
    // line with cols number spin box
    QHBoxLayout* dimcolLayout = new QHBoxLayout(newlayoutGBL);
    QLabel* label = new QLabel(tr("&Columns:"),
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
    label = new QLabel(tr("&Rows:"),
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
            tr("External programms"), w, "extprogGB");
    QVBoxLayout* extprogGBL = new QVBoxLayout(extprogGB->layout(), 6);
    tabL->addWidget(extprogGB);
    
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
    coboEditor->setMinimumWidth(120); //MAGIC
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
    coboBrowser->setMinimumWidth(120); //MAGIC
    label->setBuddy(coboBrowser);
    browserLayout->addWidget(coboBrowser);


    // autoload groupbox
    QGroupBox* autolayoutGB = new QGroupBox(0, Horizontal,
            tr("Autoload layout"), w, "autolayoutGB");
    tabL->addWidget(autolayoutGB);
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
    buttGetAutofile = new QPushButton(tr("C&hoose..."), autolayoutGB,
            "choosePB");
    chooseLayout->addWidget(buttGetAutofile);
    connect(buttGetAutofile, SIGNAL(clicked()),
            this, SLOT(slotGetAutofile()));

    // line with lineedit
    leAutoload = new QLineEdit(autolayoutGB, "autoloadLE");
    autoGBLayout->addWidget(leAutoload);


    // spacer to push group boxes to top
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    tabL->addItem(spacer);
    addTab(w, tr("&Layout"));
}


void optionsDialog::setupElementTab()
{
    QWidget *w = new QWidget(this, "tabpagetwo");
    QVBoxLayout* tabL = new QVBoxLayout(w, 10);
    
         
    // elements groupbox
    QButtonGroup* generalBG = new QButtonGroup(1, Qt::Horizontal,
            tr("General options"), w, "generalBG");
    tabL->addWidget(generalBG);
    cbShowHp2 = new QCheckBox(tr("Show &orange light for signals"
                " switched to Hp2"), generalBG, "Hp2CB");
    cbGenBubble = new QCheckBox(tr("Show &general bubblehelp "
                "(change needs program restart)"),
            generalBG, "bubbleCB");
    cbDataBubble = new QCheckBox(tr("Show bubblehelp for element &data"),
            generalBG, "databubbleCB");
    connect(cbDataBubble, SIGNAL(pressed()), this, SLOT(slotSetRepaint()));

    
    // text groupbox
    QButtonGroup* soladdrBG = new QButtonGroup(1, Qt::Horizontal,
            tr("Solenoid labeling"), w, "soladdrBG");
    tabL->addWidget(soladdrBG);
    rbShowAddr = new QRadioButton(tr("Show decoder &address"),
            soladdrBG);
    rbShowTxt = new QRadioButton(
            tr("Show &text (turnout or signal name)"), soladdrBG);
    connect(rbShowAddr, SIGNAL(pressed()), this, SLOT(slotSetRepaint()));
    connect(rbShowTxt, SIGNAL(pressed()), this, SLOT(slotSetRepaint()));


    // init groupbox
    QButtonGroup* initsigBG = new QButtonGroup(1, Qt::Horizontal,
            tr("Initialize signals on startup"), w, "initsigBG");
    tabL->addWidget(initsigBG);
    rbSignalRed =
        new QRadioButton(tr("A&lways on Halt (Hp0/Hp00/Sh0)"), initsigBG);
    rbSignalLay =
        new QRadioButton(tr("As &saved from previous session"), initsigBG);
   

    // spacer to push group boxes to top
    QSpacerItem* spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    tabL->addItem(spacer);
    addTab(w, tr("&Elements"));
}


void optionsDialog::setupDigitalTab()
{
    QWidget *w = new QWidget(this, "tabpagethree");
    QVBoxLayout* tabL = new QVBoxLayout(w, 10);
    
    
    // protocol groupbox
    QButtonGroup *protocolBG = new QButtonGroup(2, Vertical,
            tr("Default protocol"), w);
    tabL->addWidget(protocolBG);
    rbProtMS = new QRadioButton("Märklin/M&otorola", protocolBG);
    rbProtNA = new QRadioButton("&NMRA/DCC", protocolBG);
    connect(protocolBG, SIGNAL(clicked(int)),
            this, SLOT(slotProtChanged(int)));

    // solenoid groupbox
    QGroupBox* solenoidGB = new QGroupBox(0, Horizontal,
            tr("Solenoid defaults"), w, "solenoidGB");
    tabL->addWidget(solenoidGB);
    QVBoxLayout* solenoidGBL = new QVBoxLayout(solenoidGB->layout(), 6);
    // line with decoder
    QHBoxLayout* decoderLayout = new QHBoxLayout(solenoidGBL);
    QLabel* label = new QLabel(tr("Default de&coder:"), solenoidGB);
    decoderLayout->addWidget(label);
    QSpacerItem* spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    decoderLayout->addItem(spacer);
    coboDecoder = new QComboBox(false, solenoidGB);
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
    decoderLayout->addWidget(coboDecoder);
    connect(coboDecoder, SIGNAL(activated(int)), this,
            SLOT(slotDecoderChanged(int)));
    label->setBuddy(coboDecoder);

    // line with activation time
    QHBoxLayout* atimeLayout = new QHBoxLayout(solenoidGBL);
    label = new QLabel(tr("Default &activation time (ms):"), solenoidGB);
    atimeLayout->addWidget(label);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    atimeLayout->addItem(spacer);
    // 20 ms steps
    sbActiveTime = new QSpinBox(50, 2000, 50, solenoidGB, "atimeSB");
    label->setBuddy(sbActiveTime);
    sbActiveTime->setWrapping(true);
    atimeLayout->addWidget(sbActiveTime);

    // line with delay time
    QHBoxLayout* delayLayout = new QHBoxLayout(solenoidGBL);
    label = new QLabel(tr("Routing &delay per element (ms):"), solenoidGB);
    delayLayout->addWidget(label);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    delayLayout->addItem(spacer);
    // 50 ms steps
    sbRoutingTime = new QSpinBox(20, 1000, 20, solenoidGB, "rtSB");
    sbRoutingTime->setWrapping(true);
    label->setBuddy(sbRoutingTime);
    delayLayout->addWidget(sbRoutingTime);


    // turntable groupbox
    QGroupBox* ttGB = new QGroupBox(0, Vertical,
            tr("Turntable defaults"), w, "ttGB");
    tabL->addWidget(ttGB);
    QVBoxLayout* ttGBL = new QVBoxLayout(ttGB->layout(), 6);

    // line with auto tt
    cbAutoTTDir = new QCheckBox(tr("Choose digital &turntable"
                " moving-direction automatically"), ttGB, "ttCB");
    ttGBL->addWidget(cbAutoTTDir);

    // line with tt time
    QHBoxLayout* ttLayout = new QHBoxLayout(ttGBL);
    label = new QLabel(tr("T&ime for a 360° turn of turntable (s.ms):"), ttGB);
    ttLayout->addWidget(label);
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    ttLayout->addItem(spacer);
    leTTRoundTime = new QLineEdit(ttGB, "roundTime");
    leTTRoundTime->setMaxLength(6);
    leTTRoundTime->setMaximumWidth(60); //MAGIC
    label->setBuddy(leTTRoundTime);
    ttLayout->addWidget(leTTRoundTime);

    // spacer to push group boxes to top
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    tabL->addItem(spacer);
    addTab(w, tr("&Digital Data"));
}


void optionsDialog::setupFeedbackTab()
{
    QVBox *tab = new QVBox(this, "tapPageFour");
    tab->setMargin(10);
    tab->setSpacing(10);
    
    // feedback module groupbox
    QButtonGroup* grpBox = new QButtonGroup(3, Vertical,
            tr("Feedback type"), tab);

    rbS88_16 = new QRadioButton(tr("s88 with 1&6 inputs per module"), grpBox);
    rbS88_8 = new QRadioButton(tr("s88 with &8 inputs per module"), grpBox);
    //rbI8255 = new QRadioButton(tr("i8255 IO-Ca&rd"), grpBox);

    connect(grpBox, SIGNAL(clicked(int)),
            this, SLOT(slotLimitModules(int)));

    // bus numbering groupbox
    QButtonGroup* busnoGB = new QButtonGroup(2, Vertical,
            tr("Feedback bus numbering"), tab);

    fixedBusesRB = new QRadioButton(tr("Fi&xed "
                "(SRCP 0.7)"), busnoGB);
    flexBusesRB = new QRadioButton(tr("F&lexible "
                "(SRCP 0.8)"), busnoGB);

    //connect(busnoGB, SIGNAL(clicked(int)),
    //        this, SLOT(slotLimitModules(int)));

    // Feedback bus numbering
    // * SRCP 0.7 -> number is fixed
    // * SRCP 0.8 -> number is variable

    QGroupBox* busGB = new QGroupBox(0, Vertical,
            tr("Connected modules per feedback bus"), tab, "busGB");
    QHBoxLayout* busGBL = new QHBoxLayout(busGB->layout(), 6);


    //Rows, Columns 
    QGridLayout* busLayout = new QGridLayout(busGBL, 4, 5, 10,
            "busLayout");
    busLayout->addColSpacing(2, 40);
    
    QSpacerItem* spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    busGBL->addItem(spacer);

    // 1. line
    QLabel* label = new QLabel(tr("&Bus:"), busGB);
    busLayout->addWidget(label, 0, 0);
    bus1LE = new QLineEdit("1", busGB, "bus1LE");
    bus1LE->setMaximumWidth(40);
    busLayout->addWidget(bus1LE, 0, 1);
    label->setBuddy(bus1LE);
    label = new QLabel(tr("&Modules:"), busGB);
    busLayout->addWidget(label, 0, 3);
    sbFBmod_1 = new QSpinBox(0, 31, 1, busGB, "");
    busLayout->addWidget(sbFBmod_1, 0, 4);
    label->setBuddy(sbFBmod_1);
    sbFBmod_1->setMaximumWidth(60); //MAGIC
    sbFBmod_1->setWrapping(true);
    
    // 2. line
    label = new QLabel(tr("&Bus:"), busGB);
    busLayout->addWidget(label, 1, 0);
    bus2LE = new QLineEdit("2", busGB, "bus2LE");
    bus2LE->setMaximumWidth(40);
    busLayout->addWidget(bus2LE, 1, 1);
    label->setBuddy(bus2LE);
    label = new QLabel(tr("&Modules:"), busGB);
    busLayout->addWidget(label, 1, 3);
    sbFBmod_2 = new QSpinBox(0, 31, 1, busGB, "");
    busLayout->addWidget(sbFBmod_2, 1, 4);
    label->setBuddy(sbFBmod_2);
    sbFBmod_2->setMaximumWidth(60); //MAGIC
    sbFBmod_2->setWrapping(true);

    // 3. line
    label = new QLabel(tr("&Bus:"), busGB);
    busLayout->addWidget(label, 2, 0);
    bus3LE = new QLineEdit("3", busGB, "bus3LE");
    bus3LE->setMaximumWidth(40);
    busLayout->addWidget(bus3LE, 2, 1);
    label->setBuddy(bus3LE);
    label = new QLabel(tr("&Modules:"), busGB);
    busLayout->addWidget(label, 2, 3);
    sbFBmod_3 = new QSpinBox(0, 31, 1, busGB, "");
    busLayout->addWidget(sbFBmod_3, 2, 4);
    label->setBuddy(sbFBmod_3);
    sbFBmod_3->setMaximumWidth(60); //MAGIC
    sbFBmod_3->setWrapping(true);

    // 4. line
    label = new QLabel(tr("&Bus:"), busGB);
    busLayout->addWidget(label, 3, 0);
    bus4LE = new QLineEdit("4", busGB, "bus4LE");
    bus4LE->setMaximumWidth(40);
    busLayout->addWidget(bus4LE, 3, 1);
    label->setBuddy(bus4LE);
    label = new QLabel(tr("&Modules:"), busGB);
    busLayout->addWidget(label, 3, 3);
    sbFBmod_4 = new QSpinBox(0, 31, 1, busGB, "");
    busLayout->addWidget(sbFBmod_4, 3, 4);
    label->setBuddy(sbFBmod_4);
    sbFBmod_4->setMaximumWidth(60); //MAGIC
    sbFBmod_4->setWrapping(true);

    addTab(tab, tr("&Feedback"));
}


void optionsDialog::slotGetAutofile()
{
    QString filename = QFileDialog::getOpenFileName(QDir::homeDirPath(),
                           QString(tr("Layouts")) +
                           " (*" + GF_GBSEXT + ")", this);

    if (!filename.isEmpty())
        leAutoload->setText(filename);
}


void optionsDialog::slotAutoload(bool load)
{
    // if autoload is (de-)selected, do the same with filebutton and
    // namefield
    leAutoload->setEnabled(load);
    buttGetAutofile->setEnabled(load);
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

    sbRoutingTime->setValue(ROUTING_TIME);
    rbS88_16->setChecked(FEEDBACK == FB_16);
    rbS88_8->setChecked(FEEDBACK == FB_8);
    sbFBmod_1->setValue(FB_MODULES_[0]);
    sbFBmod_2->setValue(FB_MODULES_[1]);
    sbFBmod_3->setValue(FB_MODULES_[2]);
    sbFBmod_4->setValue(FB_MODULES_[3]);
    slotLimitModules(FEEDBACK);
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
        ts << "auto ZP 9:      " << 0 << endl;
        ts << "routing delay:  " << sbRoutingTime->value() << endl;
        if (rbS88_16->isChecked() == 1)
            ts << "feedback type:  " << "S88_16" << endl;
        if (rbS88_8->isChecked() == 1)
            ts << "feedback type:  " << "S88_8" << endl;
        //TODO: i8255
        ts << "modules bus #1: " << sbFBmod_1->value() << endl;
        ts << "modules bus #2: " << sbFBmod_2->value() << endl;
        ts << "modules bus #3: " << sbFBmod_3->value() << endl;
        ts << "modules bus #4: " << sbFBmod_4->value() << endl;

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
