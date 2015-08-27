#Available values for profile: mobile, wearable, tv, ivi, common
%{!?profile:%define profile tv}

Name:           app-installers
Summary:        Application installers
Version:        1.7
Release:        1
Group:          Application Framework/Package Management
License:        Apache-2.0
Source0:        %{name}-%{version}.tar.gz
Source1001:     wgt-backend.manifest
Source1002:     tpk-backend.manifest
Source1003:     app-installers-tests.manifest

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
BuildRequires:  pkgconfig(manifest-parser)
BuildRequires:  pkgconfig(manifest-handlers)
BuildRequires:  pkgconfig(capi-security-privilege-manager)
BuildRequires:  pkgconfig(libwebappenc)

Requires: ca-certificates-tizen
Requires: libtzplatform-config

%description
This is a meta package that installs the common application
installers of Tizen.

%package -n wgt-backend
Summary: Backend of WGT files

%description -n wgt-backend
Backend for standard widget files WGT

%package -n tpk-backend
Summary: Backend of TPK files

%description -n tpk-backend
Backend for tizen package files

%package tests
Summary: Unit tests for app-installers
Requires: %{name} = %{version}

%description tests
Unit tests for al modules of app-installers


%prep
%setup -q

cp %{SOURCE1001} .
cp %{SOURCE1002} .
cp %{SOURCE1003} .

%build
#Variable for setting symlink to runtime
runtime_path=%{_bindir}/xwalk-launcher
%if "%{profile}" == "mobile" || "%{profile}" == "wearable" || "%{profile}" == "tv"
runtime_path=%{_bindir}/wrt
%endif
%cmake . -DCMAKE_BUILD_TYPE=%{?build_type:%build_type} -DWRT_LAUNCHER=${runtime_path}
make %{?_smp_mflags}

%install
%make_install

mkdir -p %{buildroot}/etc/package-manager/backend
ln -s %{_bindir}/wgt-backend %{buildroot}%{_sysconfdir}/package-manager/backend/wgt
ln -s %{_bindir}/tpk-backend %{buildroot}%{_sysconfdir}/package-manager/backend/tpk

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%{_libdir}/libcommon-installer.so*
%license LICENSE

%files -n wgt-backend
%manifest wgt-backend.manifest
%{_sysconfdir}/package-manager/backend/wgt
%{_bindir}/wgt-backend

%files -n tpk-backend
%{_sysconfdir}/package-manager/backend/tpk
%manifest tpk-backend.manifest
%{_bindir}/tpk-backend

%files tests
%manifest app-installers-tests.manifest
%{_bindir}/app-installers-ut/*
%{_datadir}/app-installers-ut/*

%changelog
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
