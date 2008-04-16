/*
 * aboutdialog.cpp
 * ---------------
 * Begin
 *   2007-08-26
 * 
 * Last modified
 *   $Date: 2008-04-16 19:51:04 $
 *   $Revision: 1.7 $
 *
 * Copyright
 *   (C) 2007 Guido Scholz <guido.scholz@bayernline.de>
 * 
 * Description
 *   Dialog window to display program information
 *
 * License
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 */

#include <qfont.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qgrid.h>
#include <qpixmap.h>

#include "aboutdialog.h"

#ifdef WIN32
#include "config_w32.h"
#else
#include "config.h"
#endif

/* application icon */
#include "../icons/spdrs60_48.xpm"


AboutDialog::AboutDialog(QWidget* parent): QDialog(parent,
        "AboutDialog", true)
{
    setCaption(tr("About %1").arg(PACKAGE));
    QVBoxLayout* baseLayout = new QVBoxLayout(this, 10, 10);

    QLabel* label;

    // first line: pixmap, programname, version
    QHBoxLayout* pixmapLayout = new QHBoxLayout(baseLayout, 20, "xpmLayout");
    label = new QLabel(this, "pixmapLabel");
    pixmapLayout->addWidget(label);
    label->setPixmap(QPixmap(spdrs60_48_xpm));

    label = new QLabel(PACKAGE_STRING, this, "appLabel");
    pixmapLayout->addWidget(label);
    QFont font;
    font.setPointSize(18);
    font.setWeight(QFont::Bold);
    label->setFont(font);

    pixmapLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));

    // second line: description
    label = new QLabel(tr(
                "spdrs60 is a SRCP client to control digital model railways.\n"
                "Visual appearance and usage comply to the original SpDrS60\n"
                "switchbox (Spurplandrucktastenstellwerk Bauart Siemens 60)\n"
                "of the german national railroad company. spdrs60 needs a\n"
                "SRCP server (e.g. erddcd or srcpd) as hardware link."),
          this, "descLabel");
    baseLayout->addWidget(label);

    // third line: web link
    label = new QLabel("http://spdrs60.sourceforge.net/", this,
            "webLabel");
    baseLayout->addWidget(label);


    // authors groupbox
    QGroupBox* authorGB = new QGroupBox(2, Qt::Vertical,
            tr("Authors"), this, "authorsGB");
    baseLayout->addWidget(authorGB);

    label = new QLabel("(C) 1999-2003 Stefan Preis <stefan.preis@wdr.de>",
            authorGB);
    label = new QLabel("(C) 2004-2008 Guido Scholz "
            "<guido.scholz@bayernline.de>", authorGB);

    // contributors groupbox, names in two vertical columns
    QGroupBox* contribGB = new QGroupBox(2, Qt::Horizontal,
            tr("Contributors"), this, "contribGB");
    baseLayout->addWidget(contribGB);

    label = new QLabel("Dirk Armbrust", contribGB);
    label = new QLabel("Klaus Mannweiler", contribGB);
    label = new QLabel("Sven Roth", contribGB);
    label = new QLabel(QString::fromUtf8("David Rütti"), contribGB);
    label = new QLabel(QString::fromUtf8("André Schenk"), contribGB);
    label = new QLabel("Ullrich Schicke", contribGB);
    label = new QLabel(QString::fromUtf8("Björn Schließmann"), contribGB);
    label = new QLabel(QString::fromUtf8("Rüdiger Seidel"), contribGB);
    label = new QLabel("Dietmar Toelg", contribGB);

    // OK button
    QHBoxLayout* buttonLayout = new QHBoxLayout(0, 0, 6);
    baseLayout->addLayout(buttonLayout);
    buttonLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));

    QPushButton* okPB = new QPushButton(tr("OK"), this);
    buttonLayout->addWidget(okPB);
    okPB->setDefault(true);
    connect(okPB, SIGNAL(clicked()), this, SLOT(accept()));

    buttonLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding,
                QSizePolicy::Minimum));
}

