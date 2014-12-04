Name:           app-installers
Summary:        Application installers
Version:        1.0
Release:        0
Group:          Application Framework/Application State Management
License:        Apache-2.0
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  boost-devel
BuildRequires:  cmake
BuildRequires:  libcap-devel
BuildRequires:  pkgconfig(pkgmgr)
BuildRequires:  pkgconfig(pkgmgr-parser)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(pkgmgr-installer)
BuildRequires:  pkgconfig(openssl)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  pkgconfig(zlib)
BuildRequires:  pkgconfig(minizip)
BuildRequires:  pkgconfig(libzip)
BuildRequires:  pkgconfig(xmlsec1)

Requires: ca-certificates-tizen

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

%prep
%setup -q

%build
%cmake . -DCMAKE_BUILD_TYPE=%{?build_type:%build_type}
make %{?_smp_mflags}

%install
%make_install

mkdir -p %{buildroot}/etc/package-manager/backend
ln -s %{_bindir}/wgt-backend %{buildroot}%{_sysconfdir}/package-manager/backend/wgt
ln -s %{_bindir}/xpk-backend %{buildroot}%{_sysconfdir}/package-manager/backend/xpk

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%{_libdir}/libcommon-installer.so*
%{_libdir}/libcommon-installer-signature.so*
%{_datarootdir}/app-installers/signature_schema.xsd
%license LICENSE LICENSE-xwalk

%files -n wgt-backend
%{_sysconfdir}/package-manager/backend/wgt
%{_bindir}/wgt-backend

%files -n xpk-backend
%{_sysconfdir}/package-manager/backend/xpk
