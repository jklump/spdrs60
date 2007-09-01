# Makefile.am for spdrs60
# (c) Guido Scholz
# version: $Revision: 1.20 $
# last update: $Date: 2007-09-01 15:59:01 $

SUBDIRS = pixmaps

bin_PROGRAMS = spdrs60

# list of all generated (and not distributed) sources
nodist_spdrs60_SOURCES = \
	aboutdialog.moc.cpp \
	elementcommander.moc.cpp \
	elementdialog.moc.cpp \
	element.moc.cpp \
	feedbacklistbox.moc.cpp \
	feedbackviewer.moc.cpp \
	finder.moc.cpp \
	gbsarea.moc.cpp \
	gbsscrollview.moc.cpp \
	keyboard.moc.cpp \
	mainwindow.moc.cpp \
	messagehistory.moc.cpp \
	newlayoutdialog.moc.cpp \
	options.moc.cpp \
        route.moc.cpp \
	routedialog.moc.cpp \
	routeelementdialog.moc.cpp \
        router.moc.cpp \
        routelistview.moc.cpp \
        routelistwindow.moc.cpp \
	serverinfodialog.moc.cpp \
	turntablecommander.moc.cpp

# list of all sources and headers
spdrs60_SOURCES = main.cpp resources.h \
	aboutdialog.cpp aboutdialog.h \
	elementcommander.cpp elementcommander.h \
	element.cpp element.h \
	elementdialog.cpp elementdialog.h \
	feedbacklistbox.cpp feedbacklistbox.h \
	feedbackmodule.cpp feedbackmodule.h \
	feedbackviewer.cpp feedbackviewer.h \
	finder.cpp finder.h \
	gbsarea.cpp gbsarea.h \
	gbsscrollview.cpp gbsscrollview.h  \
	keyboard.cpp keyboard.h \
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
	serverinfodialog.cpp serverinfodialog.h \
        srcpmessage.cpp srcpmessage.h \
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
	serverinfodialog.cpp \
	turntablecommander.cpp


spdrs60_LDADD = @LIB_QT@

# the library search path. 
spdrs60_LDFLAGS = $(all_libraries) 

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

# define path to Qt translations if configure found it
if TRANSLATE_QT
TRANSLATE_DEF = -DQT_TRANSLATIONS_DIR=\"@QT_TRANSLATIONS_DIR@\"
endif
AM_CXXFLAGS = $(TRANSLATE_DEF) -Wall

EXTRA_DIST = spdrs60_de.ts spdrs60.pro

pkgdata_DATA = spdrs60_de.qm

# all generated files are removed by "make clean"
CLEANFILES = $(nodist_spdrs60_SOURCES) $(pkgdata_DATA)

# rule for moc-file generation (moc_% conflicts with 'lupdate')
%.moc.cpp: %.h
	$(MOC) $< -o $@

# rule for translation file generation
# lrelease 3.0 does not understand option -qm
if LRELEASE_QM
%.qm: %.ts
	$(LRELEASE) -verbose $< -qm $@
else
%.qm: %.ts
	$(LRELEASE) -verbose spdrs60.pro
endif

# rule for translation file updates
%.ts: spdrs60.pro $(translated_source_files)
	$(LUPDATE) -verbose spdrs60.pro

