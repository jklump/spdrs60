# Makefile.am for centralclock
# (c) Guido Scholz
# last update: 2005-01-07

bin_PROGRAMS = centralclock

# list of all generated (and not distributed) moc-files
nodist_centralclock_SOURCES = centralclock_moc.cpp

centralclock_SOURCES = main.cpp centralclock.cpp centralclock.h

# the library search path. 
centralclock_LDFLAGS = @QT_LDFLAGS@ 
INCLUDES = @QT_INCLUDES@

# remove generated moc-files
CLEANFILES = $(nodist_centralclock_SOURCES)

# rule for moc-file generation
SUFFIXES = _moc.cpp

.h_moc.cpp:
	$(MOC) $< -o $@

