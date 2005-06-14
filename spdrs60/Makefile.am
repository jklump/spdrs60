# Makefile.am for spdrs60
# (c) Guido Scholz
# version: $Revision: 1.7 $
# last update: $Date: 2005-06-14 18:52:52 $

SUBDIRS = pixmaps

bin_PROGRAMS = spdrs60

# list of all generated (and not distributed) sources
nodist_spdrs60_SOURCES = \
	elementcommander.moc.cpp \
	elementdialog.moc.cpp \
	element.moc.cpp \
	fbmodule.moc.cpp \
	feedback.moc.cpp \
	finder.moc.cpp \
	gbsarea.moc.cpp \
	gbsscrollview.moc.cpp \
	keyboard.moc.cpp \
	mainwindow.moc.cpp \
	newlayoutdialog.moc.cpp \
	options.moc.cpp \
	routedialog.moc.cpp \
        route.moc.cpp \
        router.moc.cpp \
        routingtable.moc.cpp \
        routingviewer.moc.cpp \
	turntablecommander.moc.cpp

# list of all sources and headers
spdrs60_SOURCES = main.cpp resources.h \
	elementcommander.cpp elementcommander.h \
	element.cpp element.h \
	elementdialog.cpp elementdialog.h \
	fbmodule.cpp fbmodule.h \
	feedback.cpp feedback.h \
	finder.cpp finder.h \
	gbsarea.cpp gbsarea.h \
	gbsscrollview.cpp gbsscrollview.h  \
	keyboard.cpp keyboard.h \
	mainwindow.cpp mainwindow.h \
	newlayoutdialog.cpp newlayoutdialog.h \
	options.cpp options.h \
	routedialog.cpp routedialog.h \
        route.cpp route.h \
        router.cpp router.h \
        routingtable.cpp routingtable.h \
        routingviewer.cpp routingviewer.h \
	turntablecommander.cpp turntablecommander.h
	
spdrs60_LDADD = @LIB_QT@

# the library search path. 
spdrs60_LDFLAGS = $(all_libraries) 

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

EXTRA_DIST = spdrs60_de.ts spdrs60.pro

pkgdata_DATA = spdrs60_de.qm

# remove generated files
CLEANFILES = $(nodist_spdrs60_SOURCES) $(pkgdata_DATA)

# rule for moc-file generation (moc_% conflicts with 'lupdate')
%.moc.cpp: %.h
	$(MOC) $< -o $@

# rule for translation file generation
%.qm: %.ts
	$(LRELEASE) -verbose $< -qm $@

