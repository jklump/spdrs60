# Makefile.am for spdrs60
# (c) Guido Scholz
# version: $Revision: 1.41 $
# last update: $Date: 2008-12-25 18:21:26 $

SUBDIRS = pixmaps

# test to run by "make check"
check_PROGRAMS = srcp_test
TESTS = $(check_PROGRAMS)
srcp_test_SOURCES = \
	srcp_test.cpp srcpmessage.h srcpmessage.cpp
srcp_test_LDADD = @LIB_QT@
srcp_test_LDFLAGS = @QT_LDFLAGS@
srcp_test_CXXFLAGS = @QT_INCLUDES@


bin_PROGRAMS = spdrs60

# list of all generated (and not distributed) sources
nodist_spdrs60_SOURCES = \
	aboutdialog_moc.cpp \
	axlecountergrouppanel_moc.cpp \
	bufferstoprightpanel_moc.cpp \
	bufferstopleftpanel_moc.cpp \
	bufferstoptoppanel_moc.cpp \
	commandport_moc.cpp \
	drivedialog_moc.cpp \
	dualdrivedialog_moc.cpp \
	elementcommander_moc.cpp \
	elementlabeldialog_moc.cpp \
	element_moc.cpp \
	feedbacklistbox_moc.cpp \
	feedbacktriggerdialog_moc.cpp \
	feedbackviewer_moc.cpp \
	finder_moc.cpp \
	gbsarea_moc.cpp \
	gbsscrollview_moc.cpp \
	infoport_moc.cpp \
	keyboard_moc.cpp \
	layouteditmodeagrp_moc.cpp \
	levelcrossinggrouppanel_moc.cpp \
	mainwindow_moc.cpp \
	messagehistory_moc.cpp \
	newlayoutdialog_moc.cpp \
	options_moc.cpp \
	paintitembutton_moc.cpp \
	paintitemwindow_moc.cpp \
	powersupplygrouppanel_moc.cpp \
	routedialog_moc.cpp \
	routeelementdialog_moc.cpp \
	routegrouppanel_moc.cpp \
	routelistview_moc.cpp \
	routelistwindow_moc.cpp \
	route_moc.cpp \
	router_moc.cpp \
	section_moc.cpp \
	serverinfodialog_moc.cpp \
	signalgrouppanel_moc.cpp \
	spdrpanel_moc.cpp \
	srcpport_moc.cpp \
	trainnumberdialog_moc.cpp \
	turntablecommander_moc.cpp \
	turnoutgrouppanel_moc.cpp \
	variantdialog_moc.cpp \
	virtualaddressdialog_moc.cpp

# list of all sources and headers
spdrs60_SOURCES = main.cpp resources.h \
	aboutdialog.cpp aboutdialog.h \
	axlecountergrouppanel.cpp axlecountergrouppanel.h \
	bufferstoprightpanel.cpp bufferstoprightpanel.h \
	bufferstopleftpanel.cpp bufferstopleftpanel.h \
	bufferstoptoppanel.cpp bufferstoptoppanel.h \
	commandport.cpp commandport.h \
	crcfmessage.cpp crcfmessage.h \
	drivedialog.cpp drivedialog.h \
	dualdrivedialog.cpp dualdrivedialog.h \
	elementcommander.cpp elementcommander.h \
	element.cpp element.h \
	elementlabeldialog.cpp elementlabeldialog.h \
	feedbacklistbox.cpp feedbacklistbox.h \
	feedbackmodule.cpp feedbackmodule.h \
	feedbacktriggerdialog.cpp feedbacktriggerdialog.h \
	feedbackviewer.cpp feedbackviewer.h \
	finder.cpp finder.h \
	gbsarea.cpp gbsarea.h \
	gbsscrollview.cpp gbsscrollview.h  \
	infoport.cpp infoport.h \
	keyboard.cpp keyboard.h \
	layouteditmodeagrp.cpp layouteditmodeagrp.h \
	levelcrossinggrouppanel.cpp levelcrossinggrouppanel.h \
	mainwindow.cpp mainwindow.h \
	messagehistory.cpp messagehistory.h \
	newlayoutdialog.cpp newlayoutdialog.h \
	options.cpp options.h \
	paintitembutton.cpp paintitembutton.h \
	paintitemwindow.cpp paintitemwindow.h \
        panelfactory.cpp panelfactory.h \
	powersupplygrouppanel.cpp powersupplygrouppanel.h \
	preferences.cpp preferences.h \
	route.cpp route.h \
	routedialog.cpp routedialog.h \
	routeelementdialog.cpp routeelementdialog.h \
	routeelementlvi.cpp routeelementlvi.h \
	routegrouppanel.cpp routegrouppanel.h \
	routelistview.cpp routelistview.h \
	routelistwindow.cpp routelistwindow.h \
	routelvi.cpp routelvi.h \
	router.cpp router.h \
	section.cpp section.h \
	serverinfodialog.cpp serverinfodialog.h \
	signalgrouppanel.cpp signalgrouppanel.h \
        spdrpanel.h spdrpanel.cpp \
	srcpmessage.cpp srcpmessage.h \
	srcpport.cpp srcpport.h \
	trainnumberdialog.cpp trainnumberdialog.h \
	turntablecommander.cpp turntablecommander.h \
	turnoutgrouppanel.cpp turnoutgrouppanel.h \
	variantdialog.cpp variantdialog.h \
	virtualaddressdialog.cpp virtualaddressdialog.h

# list here all files containing string translations
translations = \
	translations/spdrs60_de.ts \
	translations/spdrs60_it.ts

translationsdir = $(pkgdatadir)/translations

# generated message files, installed but not distributed
translations_DATA = \
	translations/spdrs60_de.qm \
	translations/spdrs60_it.qm

# misc files which are distributed but not installed
EXTRA_DIST = spdrs60.pro $(translations) config_w32.h \
	     translations/README translations/TRANSLATORS

# all generated files to be removed by "make clean"
CLEANFILES = $(nodist_spdrs60_SOURCES) $(translations_DATA)

spdrs60_LDADD = @LIB_QT@

# the library search path. 
spdrs60_LDFLAGS = @QT_LDFLAGS@


# define path to Qt translations if configure found it
if TRANSLATE_QT
TRANSLATE_DEF = -DQT_TRANSLATIONS_DIR=\"@QT_TRANSLATIONS_DIR@\"
endif
AM_CXXFLAGS = $(TRANSLATE_DEF) -DTRANSLATIONSDIR=\"$(translationsdir)\" @QT_INCLUDES@

# rule for moc-file generation
SUFFIXES = _moc.cpp

.h_moc.cpp:
	$(MOC) $< -o $@

# rule for translation file updates
# lrelease 3.0 does not understand option -ts
if LUPDATE_TS
$(translations): $(spdrs60_SOURCES)
	$(LUPDATE) -verbose $(spdrs60_SOURCES) -ts $@
else
$(translations): spdrs60.pro $(spdrs60_SOURCES)
	$(LUPDATE) -verbose spdrs60.pro
endif

# rule for message file generation
# lrelease 3.0 does not understand option -qm
if LRELEASE_QM
.ts.qm:
	$(LRELEASE) -verbose $< -qm $@
else
.ts.qm:
	$(LRELEASE) -verbose spdrs60.pro
endif

