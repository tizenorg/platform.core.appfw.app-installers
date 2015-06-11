#Available values for profile: mobile, wearable, tv, ivi, common
%{!?profile:%define profile tv}

Name:           app-installers
Summary:        Application installers
Version:        1.0
Release:        1
Group:          Application Framework/Package Management
License:        Apache-2.0
Source0:        %{name}-%{version}.tar.gz
Source1001:     wgt-backend.manifest
Source1002:     tpk-backend.manifest

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
%{_datarootdir}/app-installers/signature_schema.xsd
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
%{_bindir}/app-installers-ut/*
%{_datadir}/app-installers-ut/*