# Makefile.am for spdrs60
# (c) Guido Scholz
# version: $Revision: 1.25 $
# last update: $Date: 2007-09-14 15:47:58 $

SUBDIRS = pixmaps

bin_PROGRAMS = spdrs60

# list of all generated (and not distributed) sources
nodist_spdrs60_SOURCES = \
	aboutdialog_moc.cpp \
	commandport_moc.cpp \
	elementcommander_moc.cpp \
	elementdialog_moc.cpp \
	element_moc.cpp \
	feedbackchooser_moc.cpp \
	feedbacklistbox_moc.cpp \
	feedbackviewer_moc.cpp \
	finder_moc.cpp \
	gbsarea_moc.cpp \
	gbsscrollview_moc.cpp \
	infoport_moc.cpp \
	keyboard_moc.cpp \
	layouteditmodeagrp_moc.cpp \
	mainwindow_moc.cpp \
	messagehistory_moc.cpp \
	newlayoutdialog_moc.cpp \
	options_moc.cpp \
        route_moc.cpp \
	routedialog_moc.cpp \
	routeelementdialog_moc.cpp \
        router_moc.cpp \
        routelistview_moc.cpp \
        routelistwindow_moc.cpp \
        section_moc.cpp \
	serverinfodialog_moc.cpp \
	singlefeedbackdialog_moc.cpp \
	doublefeedbackdialog_moc.cpp \
        spdritem_moc.cpp \
        spdrdecorativeitem_moc.cpp \
        spdrtrackitem_moc.cpp \
        spdraxlecountergroupitem_moc.cpp \
        spdrextbtngroupitem_moc.cpp \
        spdrlevelcrossinggroupitem_moc.cpp \
        spdrpowersupplygroupitem_moc.cpp \
        spdrroutegroupitem_moc.cpp \
        spdrsignalgroupitem_moc.cpp \
        spdrturnoutgroupitem_moc.cpp \
        spdrfhtitem_moc.cpp \
        spdrsgtitem_moc.cpp \
        spdrwgtitem_moc.cpp \
	srcpport_moc.cpp \
	turntablecommander_moc.cpp

# list of all sources and headers
spdrs60_SOURCES = main.cpp resources.h filekeys.h \
	aboutdialog.cpp aboutdialog.h \
	commandport.cpp commandport.h \
	elementcommander.cpp elementcommander.h \
	element.cpp element.h \
	elementdialog.cpp elementdialog.h \
	feedbackchooser.cpp feedbackchooser.h \
	feedbacklistbox.cpp feedbacklistbox.h \
	feedbackmodule.cpp feedbackmodule.h \
	feedbackreceiver.cpp feedbackreceiver.h \
	feedbacksinglebtnreceiver.cpp feedbacksinglebtnreceiver.h \
	feedbackdoublebtnreceiver.cpp feedbackdoublebtnreceiver.h \
	feedbackviewer.cpp feedbackviewer.h \
	finder.cpp finder.h \
	gbsarea.cpp gbsarea.h \
	gbsscrollview.cpp gbsscrollview.h  \
	infoport.cpp infoport.h \
	keyboard.cpp keyboard.h \
	layouteditmodeagrp.cpp layouteditmodeagrp.h \
	mainwindow.cpp mainwindow.h \
	messagehistory.cpp messagehistory.h \
	newlayoutdialog.cpp newlayoutdialog.h \
	options.cpp options.h \
        preferences.cpp preferences.h \
        route.cpp route.h \
	routedialog.cpp routedialog.h \
	routeelementdialog.cpp routeelementdialog.h \
	routeelementlvi.cpp routeelementlvi.h \
	routelvi.cpp routelvi.h \
	routelistview.cpp routelistview.h \
	routelistwindow.cpp routelistwindow.h \
        router.cpp router.h \
	section.cpp section.h \
	serverinfodialog.cpp serverinfodialog.h \
	singlefeedbackdialog.cpp singlefeedbackdialog.h \
	doublefeedbackdialog.cpp doublefeedbackdialog.h \
        spdritem.cpp spdritem.h \
        spdrtrackitem.cpp spdrtrackitem.h \
        spdrdecorativeitem.cpp spdrdecorativeitem.h \
        spdrextbtngroupitem.cpp spdrextbtngroupitem.h \
        spdraxlecountergroupitem.cpp spdraxlecountergroupitem.h \
        spdrlevelcrossinggroupitem.cpp spdrlevelcrossinggroupitem.h \
        spdrpowersupplygroupitem.cpp spdrpowersupplygroupitem.h \
        spdrroutegroupitem.cpp spdrroutegroupitem.h \
        spdrsignalgroupitem.cpp spdrsignalgroupitem.h \
        spdrturnoutgroupitem.cpp spdrturnoutgroupitem.h \
        spdrfhtitem.cpp spdrfhtitem.h \
        spdrsgtitem.cpp spdrsgtitem.h \
        spdrwgtitem.cpp spdrwgtitem.h \
        srcpmessage.cpp srcpmessage.h \
	srcpport.cpp srcpport.h \
	turntablecommander.cpp turntablecommander.h


# This is the same list which is assembled in spdrs60.pro under SOURCES.
# Watch out to keep it synchronized. Only files containing translatable
# strings are listed here (and in spdrs60.pro).
translated_source_files = \
	aboutdialog.cpp \
	elementcommander.cpp \
	element.cpp \
	elementdialog.cpp \
	feedbacklistbox.cpp \
	feedbackviewer.cpp \
	finder.cpp \
	gbsarea.cpp \
	gbsscrollview.cpp \
	keyboard.cpp \
	layouteditmodeagrp.cpp \
	main.cpp \
	mainwindow.cpp \
	messagehistory.cpp \
	newlayoutdialog.cpp \
	options.cpp \
	routedialog.cpp \
	routeelementdialog.cpp \
	routeelementlvi.cpp \
	route.cpp \
	router.cpp \
	routelistview.cpp \
	routelistwindow.cpp \
	section.cpp \
	serverinfodialog.cpp \
	srcpport.cpp \
	turntablecommander.cpp


# generated files containing translations, installed but not distributed
pkgdata_DATA = spdrs60_de.qm spdrs60_it.qm


# list here all files containing string translations
translatedstrings = spdrs60_de.ts spdrs60_it.ts


# misc files which are distributed but not installed
EXTRA_DIST = spdrs60.pro $(translatedstrings)


# all generated files to be removed by "make clean"
CLEANFILES = $(nodist_spdrs60_SOURCES) $(pkgdata_DATA)


spdrs60_LDADD = @LIB_QT@

# the library search path. 
spdrs60_LDFLAGS = $(all_libraries) 

# set the include path for X and Qt
INCLUDES= $(all_includes)

# define path to Qt translations if configure found it
if TRANSLATE_QT
TRANSLATE_DEF = -DQT_TRANSLATIONS_DIR=\"@QT_TRANSLATIONS_DIR@\"
endif
AM_CXXFLAGS = $(TRANSLATE_DEF) -Wall


# rule for moc-file generation
SUFFIXES = _moc.cpp

.h_moc.cpp: $(spdrs60_SOURCES)
	$(MOC) $< -o $@


# rule for translation file generation
# lrelease 3.0 does not understand option -qm
if LRELEASE_QM
.ts.qm: $(translatedstrings)
	$(LRELEASE) -verbose $< -qm $@
else
.ts.qm: $(translatedstrings)
	$(LRELEASE) -verbose spdrs60.pro
endif

# rule for translation file updates
%.ts: spdrs60.pro $(translated_source_files)
	$(LUPDATE) -verbose spdrs60.pro

