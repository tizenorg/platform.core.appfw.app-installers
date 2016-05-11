#Available values for profile: mobile, wearable, tv, ivi, common
%{!?profile:%define profile tv}

Name:           app-installers
Summary:        Application installers
Version:        1.8
Release:        1
Group:          Application Framework/Package Management
License:        Apache-2.0
Source0:        %{name}-%{version}.tar.gz
Source1000:     app-installers.manifest
Source1001:     app-installers-tests.manifest

BuildRequires:  boost-devel
BuildRequires:  cmake
BuildRequires:  libcap-devel
BuildRequires:  gtest-devel
BuildRequires:  pkgconfig(pkgmgr)
BuildRequires:  pkgconfig(pkgmgr-parser)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(pkgmgr-installer)
BuildRequires:  pkgconfig(security-manager)
BuildRequires:  pkgconfig(libiri)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  pkgconfig(zlib)
BuildRequires:  pkgconfig(minizip)
BuildRequires:  pkgconfig(libzip)
BuildRequires:  pkgconfig(libtzplatform-config)
BuildRequires:  pkgconfig(cert-svc-vcore)
BuildRequires:  pkgconfig(manifest-parser-utils)
BuildRequires:  pkgconfig(delta-manifest-handlers)
BuildRequires:  pkgconfig(security-privilege-manager)
BuildRequires:  pkgconfig(tpk-manifest-handlers)
BuildRequires:  pkgconfig(dbus-1)
BuildRequires:  pkgconfig(aul)

Requires: ca-certificates-tizen
Requires: libtzplatform-config
Requires: xdelta3

%description
This is a meta package that installs the common application
installers of Tizen.

%package devel
Summary:    App-installers development files
Group:      Application Framework/Package Management
Requires:   %{name} = %{version}

%description devel
This package contains header files of app-installers common library

%package tests
Summary: Unit tests for app-installers
Requires: %{name} = %{version}

%description tests
Unit tests for al modules of app-installers

%prep
%setup -q

cp %{SOURCE1000} .
cp %{SOURCE1001} .

%build
%cmake . -DCMAKE_BUILD_TYPE=%{?build_type:%build_type}
make %{?_smp_mflags}

%install
%make_install

%post

%postun

%files
%defattr(-,root,root)
%{_datarootdir}/app-installers/plugins_list.txt
%manifest app-installers.manifest
%{_libdir}/libapp-installers.so*
%attr(6750,root,root) %{_bindir}/pkgdir-tool
%attr(6755,root,root) %{_sysconfdir}/gumd/useradd.d/20_pkgdir-tool-add.post
%{_bindir}/pkg-install-manifest
%license LICENSE

%files devel
%{_includedir}/app-installers/common/*.h
%{_includedir}/app-installers/common/*/*.h
%{_includedir}/app-installers/common/*/*/*.h
%{_libdir}/pkgconfig/app-installers.pc

%files tests
%manifest app-installers-tests.manifest
%{_bindir}/app-installers-ut/*
%{_datadir}/app-installers-ut/*

%changelog
* Tue Sep 01 2015 Pawel Sikorski <p.sikorski@samsung.com> 1.8-1
- manifest-parser alignement; icons fixes; boost namespace cleanup
- package directories update (security policy, tmp and cache dirs)
- apps killing during update/deinstallation

* Thu Aug 27 2015 Pawel Sikorski <p.sikorski@samsung.com> 1.7-1
- fixes: privileges list length, manifest generation, icon for tpk
- RequestMode introduction

* Fri Aug 21 2015 Pawel Sikorski <p.sikorski@samsung.com> 1.6-1
- Icons generation fix;
- Privilege translation for backward compatilibity

* Fri Aug 21 2015 Pawel Sikorski <p.sikorski@samsung.com> 1.5-1
- [Bug] Fix installation/deinstallation mode with smack

* Thu Aug 20 2015 Pawel Sikorski <p.sikorski@samsung.com> 1.4-1
- WebApp encryption alignement to the libwebappenc API change
- TPK recovery, update; WGT RDS fixup; icon fixes;
- Smoke tests updated

* Tue Aug 18 2015 Pawel Sikorski <p.sikorski@samsung.com> 1.3-1
- TPK installer exposed and TPK smoke tests added; smoke tests cleanup;
- preserving icons extensions in backup; fix deinstallation rollback
- SMACK label changed

* Fri Aug 14 2015 Pawel Sikorski <p.sikorski@samsung.com> 1.2-1
- Account info added to manifest.xml during installation; Smoke tests added; TPK update detection

* Fri Aug 14 2015 Pawel Sikorski <p.sikorski@samsung.com> 1.1-1
- Initial rpm changelog; Recovery (WGT) mode added, warngings cleanup; undo() can return ERROR
