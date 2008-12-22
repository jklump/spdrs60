Summary: Switchbox for digital model railroads 
Summary(de): Spurplan-Drucktastenstellwerk für digitale Modelleisenbahnen
Name: spdrs60
Version: 0.5.4
Release: 1%{?dist}
License: GPL
Group: Amusements/Games
Distribution: SuSE Linux
Vendor: Guido Scholz
URL: http://spdrs60.sourceforge.net/
Requires: qt3
BuildRequires: glibc-devel make qt3-devel qt3-devel-tools
BuildRequires: openjade sgml-skel docbook-dsssl-stylesheets
Prefix: /usr
Source: %{name}-%{version}.tar.bz2
Buildroot: %{_tmppath}/%{name}-%{version}-buildroot

%description
Graphical program to comfortably control a digital model railroad.
Visual appearance and usage comply to the SpDr of the german national
railroad company. SpDrS60 needs a Simple Railroad Command Protocol
(SRCP) server (e.g. erddcd or srcpd) as a link to the physical layout
of the model. 

Authors:
--------
    Stefan Preis
    Guido Scholz

%description -l de
Grafisches Programm zur komfortablen Steuerung von Weichen und Signalen. 
Visuelle Darstellung und Bedienung sind eng an das
Spurplandrucktastenstellwerk Bauart Siemens 60 (SpDrS60) der Deutschen
Bundesbahn angelehnt. Zur Steuerung der Modellbahn wird ein
SRCP-konformer Server (z.B. erddcd oder srcpd) benötigt.

Autoren:
--------
    Stefan Preis
    Guido Scholz

%prep
%setup

%build
CFLAGS=$RPM_OPT_FLAGS \
./configure \
	--prefix=%{_prefix} \
	--mandir=%{_mandir} \
	--with-doc-dir=%{_docdir}
make

%install
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT
mkdir $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install-strip

install -d $RPM_BUILD_ROOT%{_datadir}/applications
install -p -m 644 spdrs60.SuSE.desktop "$RPM_BUILD_ROOT%{_datadir}/applications/spdrs60.desktop"

for i in AUTHORS COPYING INSTALL README TODO NEWS ChangeLog spdrs60.lsm ; do
  install -p -m 644 $i "$RPM_BUILD_ROOT%{_docdir}/%{name}/$i"
done

%clean
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/%{name}
%{_bindir}/centralclock
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}*
%docdir %{_mandir}/man1/*
%{_mandir}/man1/*
%docdir %{_mandir}/de/man1/*
%{_mandir}/de/man1/*
%docdir %{_docdir}/%{name}
%{_docdir}/%{name}

%changelog
* Wed Nov 12 2008 Guido Scholz <guido.scholz@bayernline.de> 0.5.4-1
- Update to spdrs60-0.5.4

* Thu Nov 06 2008 Guido Scholz <guido.scholz@bayernline.de> 0.5.3-1
- Update to spdrs60-0.5.3

* Sat Feb 17 2007 Guido Scholz <guido.scholz@bayernline.de>
- Update to spdrs60-0.5.2

* Sun Jan 28 2007 Guido Scholz <guido.scholz@bayernline.de>
- Update to spdrs60-0.5.1

* Sun Jan 08 2006 Guido Scholz <guido.scholz@bayernline.de>
- Update to spdrs60-0.5.0

* Sun Dec 11 2005 Guido Scholz <guido.scholz@bayernline.de>
- Update to spdrs60-0.4.8

* Fri Jan 07 2005 Guido Scholz <guido.scholz@bayernline.de>
- Update to spdrs60-0.4.7

* Fri Dec 31 2004 Guido Scholz <guido.scholz@bayernline.de>
- Update to spdrs60-0.4.6

* Mon Dec 06 2004 Guido Scholz <guido.scholz@bayernline.de>
- Update to spdrs60-0.4.5

* Thu Nov 18 2004 Guido Scholz <guido.scholz@bayernline.de>
- Update to spdrs60-0.4.4

* Sun Oct 25 2004 Guido Scholz <guido.scholz@bayernline.de>
- Update to spdrs60-0.4.3-qt3-18

* Sun Oct 16 2004 Guido Scholz <guido.scholz@bayernline.de>
- Update to spdrs60-0.4.3-qt3_17

* Sun Oct 03 2004 Guido Scholz <guido.scholz@bayernline.de>
- Update to spdrs60-0.4.3-qt3_16

* Tue Sep 28 2004 Guido Scholz <guido.scholz@bayernline.de>
- Update to spdrs60-0.4.3-qt3_15

* Sat Sep 26 2004 Guido Scholz <guido.scholz@bayernline.de>
- Update to spdrs60-0.4.3-qt3_14

* Fri Jul 30 2004 Guido Scholz <guido.scholz@bayernline.de>
- Introduce XDG menu file

* Sun Feb 15 2004 Guido Scholz <guido.scholz@bayernline.de>
- First release
