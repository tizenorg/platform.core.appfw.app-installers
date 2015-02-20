Name:           app-installers
Summary:        Application installers
Version:        1.0
Release:        0
Group:          Application Framework/Package Management
License:        Apache-2.0
Source0:        %{name}-%{version}.tar.gz
Source1001:     wgt-backend.manifest

BuildRequires:  boost-devel
BuildRequires:  cmake
BuildRequires:  libcap-devel
BuildRequires:  gtest-devel
BuildRequires:  pkgconfig(pkgmgr)
BuildRequires:  pkgconfig(pkgmgr-parser)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(pkgmgr-installer)
BuildRequires:  pkgconfig(security-manager)
BuildRequires:  pkgconfig(openssl)
BuildRequires:  pkgconfig(libiri)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  pkgconfig(zlib)
BuildRequires:  pkgconfig(minizip)
BuildRequires:  pkgconfig(libzip)
BuildRequires:  pkgconfig(libtzplatform-config)
BuildRequires:  pkgconfig(xmlsec1)
BuildRequires:  pkgconfig(vconf)

Requires: ca-certificates-tizen
Requires: libtzplatform-config

%description
This is a meta package that installs the common application
installers of Tizen.

%package -n wgt-backend
Summary: Backend of WGT files

%description -n wgt-backend
Backend for standard widget files WGT

%package -n xpk-backend
Summary: Backend of XPK files

%description -n xpk-backend
Backend for standard widget files XPK

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

%build
%cmake . -DCMAKE_BUILD_TYPE=%{?build_type:%build_type} -DWRT_LAUNCHER=%{_bindir}/xwalk-launcher
make %{?_smp_mflags}

%install
%make_install

mkdir -p %{buildroot}/etc/package-manager/backend
ln -s %{_bindir}/wgt-backend %{buildroot}%{_sysconfdir}/package-manager/backend/wgt
ln -s %{_bindir}/xpk-backend %{buildroot}%{_sysconfdir}/package-manager/backend/xpk
ln -s %{_bindir}/tpk-backend %{buildroot}%{_sysconfdir}/package-manager/backend/tpk

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%{_libdir}/libcommon-installer.so*
%{_libdir}/libcommon-installer-signature.so*
%{_libdir}/libcommon-installer-utils.so*
%{_libdir}/libcommon-installer-parser.so*
%{_datarootdir}/app-installers/signature_schema.xsd
%license LICENSE LICENSE-xwalk

%files -n wgt-backend
%manifest wgt-backend.manifest
%{_sysconfdir}/package-manager/backend/wgt
%{_bindir}/wgt-backend

%files -n xpk-backend
%{_sysconfdir}/package-manager/backend/xpk

%files -n tpk-backend
%{_sysconfdir}/package-manager/backend/tpk
%{_bindir}/tpk-backend

%files tests
%{_bindir}/app-installers-ut/*
%{_datadir}/app-installers-ut/*
