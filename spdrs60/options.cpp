/***************************************************************************
                           options.cpp
                           version 0.5.1 $Revision: 1.23 $
                           -------------------------------
    copyright            : (C) 1999-2003 by Stefan Preis
                         : (C) 2004-2007 Guido Scholz
    email                : guido.scholz@bayernline.de
    last modified        : $Date: 2007-01-28 16:25:48 $
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
#include <qlistview.h>
#include <qvbox.h>
#include <qvgroupbox.h>
#include <qstringlist.h>

#include "gbsarea.h"
#include "options.h"
#include "resources.h"


optionsDialog::optionsDialog(QWidget* parent)
: QTabDialog(parent, "optionsDialog", true)
{
    setupLayoutTab();
    setupElementTab();
    setupDigitalTab();
    setupFeedbackTab();
    setupFeedbackTypeTab();

    setCaption(tr("User preferences"));
    setOKButton();
    setCancelButton();
}


void optionsDialog::setupLayoutTab()
{
    QWidget *w = new QWidget(this, "tabPageOne");
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
            tr("Automatical layout loading and saving"), w, "autolayoutGB");
    tabL->addWidget(autolayoutGB);
    QVBoxLayout* autoGBLayout = new
        QVBoxLayout(autolayoutGB->layout(), 6);
    
    // line with autosave option
    cbAutosave = new QCheckBox(tr("&Save active layout on program exit"),
            autolayoutGB, "autosaveCB");
    autoGBLayout->addWidget(cbAutosave);

    // line with radiobutton and choose button
    QHBoxLayout* chooseLayout = new QHBoxLayout(autoGBLayout);
    cbAutoload = new QCheckBox(tr("&Load this layout on program startup:"),
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
    QWidget *w = new QWidget(this, "tabPageTwo");
    QVBoxLayout* tabL = new QVBoxLayout(w, 10);
    
         
    // elements groupbox
    QButtonGroup* generalBG = new QButtonGroup(6, Qt::Vertical,
            tr("General options"), w, "generalBG");
    tabL->addWidget(generalBG);
    cbShowHp2 = new QCheckBox(tr("Show &orange light for signals"
                " switched to Hp2"), generalBG, "Hp2CB");
    cbShowBlinkingTurnouts = new QCheckBox(tr("Show &blinking turnouts"),
                generalBG, "Hp2CB");
    allwaysSendState = new QCheckBox(tr("A&llways send solenoid states"
                " on routing"), generalBG, "sendStateCB");
    cbGenBubble = new QCheckBox(tr("Show &general bubblehelp "
                "(change needs program restart)"),
            generalBG, "bubbleCB");
    cbDataBubble = new QCheckBox(tr("Show bubblehelp for element &data"),
            generalBG, "databubbleCB");
    cbConvertTime = new QCheckBox(tr("&Convert SRCP 0.8 server time "
                "human readable"),
            generalBG, "converttimeCB");

    
    // text groupbox
    QButtonGroup* soladdrBG = new QButtonGroup(2, Qt::Vertical,
            tr("Solenoid labeling"), w, "soladdrBG");
    tabL->addWidget(soladdrBG);
    rbShowAddr = new QRadioButton(tr("Show decoder &address"),
            soladdrBG);
    rbShowTxt = new QRadioButton(
            tr("Show &text (turnout or signal name)"), soladdrBG);


    // init groupbox
    QButtonGroup* initsigBG = new QButtonGroup(2, Qt::Vertical,
            tr("Initialize signals on startup"), w, "initsigBG");
    tabL->addWidget(initsigBG);
    rbSignalRed =
        new QRadioButton(tr("Always on &Halt (Hp0/Hp00/Sh0)"), initsigBG);
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
    QWidget *w = new QWidget(this, "tabPageThree");
    QVBoxLayout* tabL = new QVBoxLayout(w, 10);
    
    
    // protocol groupbox
    QButtonGroup *protocolBG = new QButtonGroup(2, Vertical,
            tr("Default protocol"), w);
    tabL->addWidget(protocolBG);
    rbProtMS = new QRadioButton("Märklin/M&otorola", protocolBG);
    rbProtNA = new QRadioButton("&NMRA/DCC", protocolBG);
    rbProtPS = new QRadioButton("&Protocol by Server", protocolBG);
    rbProtSE = new QRadioButton("Selectri&x", protocolBG);
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
    coboDecoder->insertItem("Generic Decoder (P)");
    coboDecoder->insertItem("Generic Decoder (S)");
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
    QWidget *w = new QWidget(this, "tabPageFour");
    QVBoxLayout* tabL = new QVBoxLayout(w, 10);
   
    // left column
    // 1. feedback module groupbox
    QButtonGroup* inputsGB = new QButtonGroup(1, Vertical,
            tr("Module size"), w);
    tabL->addWidget(inputsGB);

    rb16inputs = new QRadioButton(tr("1&6 Inputs"), inputsGB);
    rb8inputs = new QRadioButton(tr("&8 Inputs"), inputsGB);

    connect(inputsGB, SIGNAL(clicked(int)),
            this, SLOT(slotLimitModules(int)));

    // 2. bus numbering groupbox
    QButtonGroup* busnoGB = new QButtonGroup(1, Vertical,
            tr("Bus numbering"), w);
    tabL->addWidget(busnoGB);

    fixedBusesRB = new QRadioButton(tr("Fi&xed "
                "(SRCP 0.7)"), busnoGB);
    flexBusesRB = new QRadioButton(tr("F&lexible "
                "(SRCP 0.8)"), busnoGB);

    connect(busnoGB, SIGNAL(clicked(int)),
            this, SLOT(fixFBBusNumbers(int)));
    
    // Feedback bus numbering
    // * SRCP 0.7 -> number is fixed
    // * SRCP 0.8 -> number is variable
    // 3. modules per bus groupbox
    QGroupBox* busGB = new QGroupBox(0, Vertical,
            tr("Connected modules per bus"), w, "busGB");
    tabL->addWidget(busGB);
    QHBoxLayout* busGBL = new QHBoxLayout(busGB->layout(), 6);

    //Rows, Columns 
    QGridLayout* busLayout = new QGridLayout(busGBL, 4, 5, 10,
            "busLayout");
    busLayout->addColSpacing(2, 10);
    
    QSpacerItem* spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    busGBL->addItem(spacer);

    // 1. line
    QLabel* label = new QLabel(tr("&Bus:"), busGB);
    busLayout->addWidget(label, 0, 0);
    bus1LE = new QLineEdit("1", busGB, "bus1LE");
    bus1LE->setMaximumWidth(30);
    bus1LE->setMaxLength(3);
    busLayout->addWidget(bus1LE, 0, 1);
    label->setBuddy(bus1LE);
    
    label = new QLabel(tr("&Modules:"), busGB);
    busLayout->addWidget(label, 0, 3);
    sbFBmod_1 = new QSpinBox(0, 31, 1, busGB, "sbfbmod1");
    busLayout->addWidget(sbFBmod_1, 0, 4);
    label->setBuddy(sbFBmod_1);
    sbFBmod_1->setMaximumWidth(38); //MAGIC
    sbFBmod_1->setWrapping(true);
    
    // 2. line
    label = new QLabel(tr("&Bus:"), busGB);
    busLayout->addWidget(label, 1, 0);
    bus2LE = new QLineEdit("2", busGB, "bus2LE");
    bus2LE->setMaximumWidth(30);
    bus2LE->setMaxLength(3);
    busLayout->addWidget(bus2LE, 1, 1);
    label->setBuddy(bus2LE);

    label = new QLabel(tr("&Modules:"), busGB);
    busLayout->addWidget(label, 1, 3);
    sbFBmod_2 = new QSpinBox(0, 31, 1, busGB, "sbfbmod2");
    busLayout->addWidget(sbFBmod_2, 1, 4);
    label->setBuddy(sbFBmod_2);
    sbFBmod_2->setMaximumWidth(38); //MAGIC
    sbFBmod_2->setWrapping(true);
    
    // 3. line
    label = new QLabel(tr("&Bus:"), busGB);
    busLayout->addWidget(label, 2, 0);
    bus3LE = new QLineEdit("3", busGB, "bus3LE");
    bus3LE->setMaximumWidth(30);
    bus3LE->setMaxLength(3);
    busLayout->addWidget(bus3LE, 2, 1);
    label->setBuddy(bus3LE);
    
    label = new QLabel(tr("&Modules:"), busGB);
    busLayout->addWidget(label, 2, 3);
    sbFBmod_3 = new QSpinBox(0, 31, 1, busGB, "sbfbmod3");
    busLayout->addWidget(sbFBmod_3, 2, 4);
    label->setBuddy(sbFBmod_3);
    sbFBmod_3->setMaximumWidth(38); //MAGIC
    sbFBmod_3->setWrapping(true);
    
    // 4. line
    label = new QLabel(tr("&Bus:"), busGB);
    busLayout->addWidget(label, 3, 0);
    bus4LE = new QLineEdit("4", busGB, "bus4LE");
    bus4LE->setMaximumWidth(30);
    bus4LE->setMaxLength(3);
    busLayout->addWidget(bus4LE, 3, 1);
    label->setBuddy(bus4LE);
    
    label = new QLabel(tr("&Modules:"), busGB);
    busLayout->addWidget(label, 3, 3);
    sbFBmod_4 = new QSpinBox(0, 31, 1, busGB, "sbfbmod4");
    busLayout->addWidget(sbFBmod_4, 3, 4);
    label->setBuddy(sbFBmod_4);
    sbFBmod_4->setMaximumWidth(38); //MAGIC
    sbFBmod_4->setWrapping(true);
    
    // spacer to push group boxes to top
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    tabL->addItem(spacer);

    addTab(w, tr("&Feedback modules"));
}


void optionsDialog::setupFeedbackTypeTab()
{
    QVBox *tab = new QVBox(this, "tapPageFive");
    tab->setMargin(10);
    tab->setSpacing(10);
   
    // 1. feedback module type groupbox
    feedbackTypeGB = new QButtonGroup(3, Vertical,
            tr("Module type"), tab);

    QRadioButton* fbtRB = new QRadioButton(tr("S88 via &DDL"), feedbackTypeGB);
    fbtRB = new QRadioButton(tr("i8&255-Card"), feedbackTypeGB);
    fbtRB = new QRadioButton(tr("S88 via M60&15"), feedbackTypeGB);
    fbtRB = new QRadioButton(tr("&Protocol by server"), feedbackTypeGB);
    fbtRB = new QRadioButton(tr("&Selectrix"), feedbackTypeGB);
    fbtRB->setEnabled(false);

    connect(feedbackTypeGB, SIGNAL(clicked(int)),
            this, SLOT(selectFbModuleType(int)));

    // 2. selectrix init parameters groupbox
    selectrixGB = new QGroupBox(0, Horizontal,
            tr("Selectrix Initialization"), tab, "selectrixGB");
    QHBoxLayout* sxinitGBL = new QHBoxLayout(selectrixGB->layout(), 10);

    // 2a: parameter list
    QListView* selectrixLV = new QListView(selectrixGB, "selectrixLB");
    sxinitGBL->addWidget(selectrixLV);
    selectrixLV->setAllColumnsShowFocus(true);
    selectrixLV->addColumn(tr("Bus"));
    selectrixLV->addColumn(tr("Address"));
    selectrixLV->addColumn(tr("Number"));
    
    // 2b: vertical box layout container for buttons 
    QVBoxLayout* sxinitBtnLayout = new QVBoxLayout(sxinitGBL, 6);

    QSpacerItem* spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    sxinitBtnLayout->addItem(spacer);

    QPushButton* upPB = new QPushButton(tr("&Up"), selectrixGB,
            "upPB");
    sxinitBtnLayout->addWidget(upPB);
    upPB->setEnabled(false);
    
    QPushButton* downPB = new QPushButton(tr("&Down"), selectrixGB,
            "downPB");
    sxinitBtnLayout->addWidget(downPB);
    downPB->setEnabled(false);
    
    QPushButton* editPB = new QPushButton(tr("&Edit..."), selectrixGB,
            "editPB");
    sxinitBtnLayout->addWidget(editPB);
    editPB->setEnabled(false);
    
    QPushButton* addPB = new QPushButton(tr("&Add..."), selectrixGB,
            "addPB");
    sxinitBtnLayout->addWidget(addPB);
    
    QPushButton* removePB = new QPushButton(tr("&Remove"), selectrixGB,
            "removePB");
    sxinitBtnLayout->addWidget(removePB);
    removePB->setEnabled(false);

    // spacer to push buttons to left
    spacer = new QSpacerItem(0, 0,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    sxinitGBL->addItem(spacer);


    addTab(tab, tr("Feedback &type"));
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
    QString sText = coboDecoder->currentText().right(3);

    // default decoder has changed -> set the appropriate default protocol
    if (sText == "(M)")
        rbProtMS->setChecked(true);
    else if (sText == "(D)")
        rbProtNA->setChecked(true);
    else if (sText == "(S)")
        rbProtSE->setChecked(true);
    else
        rbProtPS->setChecked(true);
}


void optionsDialog::slotProtChanged(int)
{
    QString sProt;
    QString sText;

    if (rbProtMS->isChecked())
        sProt = "(M)";
    else if (rbProtNA->isChecked())
        sProt = "(D)";
    else if (rbProtSE->isChecked())
        sProt = "(S)";
    else
        sProt = "(P)";

    // default protocol has changed -> set the appropriate default decoder
    for (int i = 0; i < coboDecoder->count(); i++) {
        sText = coboDecoder->text(i);
        if (sText.right(3) == sProt) {
            coboDecoder->setCurrentItem(i);
            break;
        }
    }
}

/*
 * depending on feedback module type (16 or 8 port) set the spin box
 * ranges, 16 <-> 0, 8 <-> 1
 */
void optionsDialog::slotLimitModules(int iType_)
{
    sbFBmod_1->setRange(0, 31 + iType_ * 31);
    sbFBmod_2->setRange(0, 31 + iType_ * 31);
    sbFBmod_3->setRange(0, 31 + iType_ * 31);
    sbFBmod_4->setRange(0, 31 + iType_ * 31);
}

/* enable Selectrix configuration if choosen */
void optionsDialog::selectFbModuleType(int no)
{
    if (no == 4) 
        selectrixGB->setEnabled(true);
    else
        selectrixGB->setEnabled(false);
}

// TODO: remove this
bool optionsDialog::valuesAreValid()
{
    QString sText;

    sText = leAutoload->text();
    if (sText == "" && leAutoload->isEnabled()) {
        qApp->beep();
        QMessageBox::warning(this,
                             tr("Filename missing"),
                             tr("You have enabled the autoloader,\n"
                                "so you must select a filename, too.\n"),
                             tr("&OK"), 0, 0, 0, 0);
        // butt 1: OK, butt 2+3: not avail.
        // <ENTER> + <ESC> default to butt 0 = OK
        buttGetAutofile->setFocus();
        return false;
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
        return false;
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
        return true;
    }

    // everything is correct, so return VALID
    return true;
}


void optionsDialog::fixFBBusNumbers(int fixed)
{
    if (fixed == 0) {
        bus1LE->setText("1");
        bus2LE->setText("2");
        bus3LE->setText("3");
        bus4LE->setText("4");
        bus1LE->setFocusPolicy(QWidget::NoFocus);
        bus2LE->setFocusPolicy(QWidget::NoFocus);
        bus3LE->setFocusPolicy(QWidget::NoFocus);
        bus4LE->setFocusPolicy(QWidget::NoFocus);
    }
    else {
        bus1LE->setFocusPolicy(QWidget::StrongFocus);
        bus2LE->setFocusPolicy(QWidget::StrongFocus);
        bus3LE->setFocusPolicy(QWidget::StrongFocus);
        bus4LE->setFocusPolicy(QWidget::StrongFocus);
    }
}


void optionsDialog::getPreferences(Preferences& prf)
{
    prf.layoutcols = sbDefaultCols->value();
    prf.layoutrows = sbDefaultRows->value();
    prf.hp2 = cbShowHp2->isChecked();
    prf.blinkingturnouts = cbShowBlinkingTurnouts->isChecked();
    prf.sendstate = allwaysSendState->isChecked();
    prf.tooltips = cbGenBubble->isChecked();
    prf.datatooltips = cbDataBubble->isChecked();
    prf.converttime = cbConvertTime->isChecked();
    prf.addresslabeling = rbShowAddr->isChecked();
    prf.initsignalsred = rbSignalRed->isChecked();
    prf.autoload = cbAutoload->isChecked();
    prf.autosave = cbAutosave->isChecked();
    prf.autolayout = leAutoload->text();
    prf.editor = coboEditor->currentText();
    prf.browser = coboBrowser->currentText();
    if (rbProtMS->isChecked())
        prf.protocol = 1;
    else if (rbProtNA->isChecked())
        prf.protocol = 0;
    else if (rbProtSE->isChecked())
        prf.protocol = 3;
    else
        prf.protocol = 2;
    prf.decoder = coboDecoder->currentText();
    prf.autottdir = cbAutoTTDir->isChecked();
    prf.activetime = sbActiveTime->value();
    prf.routingtime = sbRoutingTime->value();
    prf.ttroundtime = leTTRoundTime->text().toDouble();
    if (rb16inputs->isChecked())
        prf.fbfactor = 0;
    else 
        prf.fbfactor = 1;
#if QT_VERSION >= 0x030300
    prf.fbmoduletype = feedbackTypeGB->selectedId();
#else
    prf.fbmoduletype = feedbackTypeGB->id(feedbackTypeGB->selected());
#endif
    prf.fixedbusnum = fixedBusesRB->isChecked();
    prf.fbbus1.modules = sbFBmod_1->value();
    prf.fbbus2.modules = sbFBmod_2->value();
    prf.fbbus3.modules = sbFBmod_3->value();
    prf.fbbus4.modules = sbFBmod_4->value();
    prf.fbbus1.number = bus1LE->text().toUInt();
    prf.fbbus2.number = bus2LE->text().toUInt();
    prf.fbbus3.number = bus3LE->text().toUInt();
    prf.fbbus4.number = bus4LE->text().toUInt();
}


void optionsDialog::setPreferences(const Preferences& prf)
{
    sbDefaultCols->setValue(prf.layoutcols);
    sbDefaultRows->setValue(prf.layoutrows);
    cbShowHp2->setChecked(prf.hp2);
    cbShowBlinkingTurnouts->setChecked(prf.blinkingturnouts);
    allwaysSendState->setChecked(prf.sendstate);
    cbGenBubble->setChecked(prf.tooltips);
    cbDataBubble->setChecked(prf.datatooltips);
    cbConvertTime->setChecked(prf.converttime);
    
    if (prf.addresslabeling)
        rbShowAddr->setChecked(true);
    else
        rbShowTxt->setChecked(true);

    if (prf.initsignalsred)
        rbSignalRed->setChecked(true);
    else
        rbSignalLay->setChecked(true);
    
    if (prf.autoload) {
        cbAutoload->setChecked(true);
        leAutoload->setText(prf.autolayout);
    }
    else
        rbSignalLay->setChecked(false);

    slotAutoload(prf.autoload);

    if (prf.autosave)
        cbAutosave->setChecked(true);

    bool found = false;
    for (int i = 0; i < coboEditor->count(); i++) {
        if (coboEditor->text(i) == prf.editor) {
            coboEditor->setCurrentItem(i);
            found = true;
            break;
        }
    }
    if (!found) {
        //TODO: prog == ""
        coboEditor->insertItem(prf.editor);
        coboEditor->setCurrentItem(coboEditor->count() - 1);
    }

    found = false;
    for (int i = 0; i < coboBrowser->count(); i++) {
        if (coboBrowser->text(i) == prf.browser) {
            coboBrowser->setCurrentItem(i);
            found = true;
            break;
        }
    }
    if (!found) {
        //TODO: prog == ""
        coboBrowser->insertItem(prf.browser);
        coboBrowser->setCurrentItem(coboBrowser->count() - 1);
    }

    switch (prf.protocol) {
        case 0:
            rbProtNA->setChecked(true);
            break;
        case 1:
            rbProtMS->setChecked(true);
            break;
        case 2:
            rbProtPS->setChecked(true);
            break;
        case 3:
            rbProtSE->setChecked(true);
            break;
    }

    found = false;
    for (int i = 0; i < coboDecoder->count(); i++) {
        if (coboDecoder->text(i) == prf.decoder) {
            coboDecoder->setCurrentItem(i);
            found = true;
            break;
        }
    }
    if (!found) {
        coboDecoder->setCurrentItem(1);
    }

    cbAutoTTDir->setChecked(prf.autottdir);
    sbActiveTime->setValue(prf.activetime);
    sbRoutingTime->setValue(prf.routingtime);

    QString t;
    t.sprintf("%.2f", prf.ttroundtime);
    leTTRoundTime->setText(t);

    if (prf.fbfactor == 0) {
        rb16inputs->setChecked(true);
        slotLimitModules(0);
    }
    else {
        rb8inputs->setChecked(true);
        slotLimitModules(1);
    }

    if (prf.fixedbusnum) {
        fixedBusesRB->setChecked(true);
        fixFBBusNumbers(0);
    }
    else {
        flexBusesRB->setChecked(true);
        fixFBBusNumbers(1);
    }
    
    bus1LE->setText(QString::number(prf.fbbus1.number));
    sbFBmod_1->setValue(prf.fbbus1.modules);
    bus2LE->setText(QString::number(prf.fbbus2.number));
    sbFBmod_2->setValue(prf.fbbus2.modules);
    bus3LE->setText(QString::number(prf.fbbus3.number));
    sbFBmod_3->setValue(prf.fbbus3.modules);
    bus4LE->setText(QString::number(prf.fbbus4.number));
    sbFBmod_4->setValue(prf.fbbus4.modules);
    
    feedbackTypeGB->setButton(prf.fbmoduletype);
    selectFbModuleType(prf.fbmoduletype);
}

