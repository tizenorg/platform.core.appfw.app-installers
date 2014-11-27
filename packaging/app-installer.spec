Name:		app-installers
Summary:	Application installers
Version:	1.0
Release:	0
Group:		App Framework/Application Installer
License:	MIT
Source0:	%{name}-%{version}.tar.gz

BuildRequires:  libcap-devel
BuildRequires:  pkgconfig(pkgmgr)
BuildRequires:  pkgconfig(pkgmgr-parser)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(pkgmgr-installer)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  pkgconfig(libzip)

%description
This is a meta package that installs the common application
installers of Tizen.


%package -n wgt-backend
Summary: backend of WGT files

%description -n wgt-backend
backend for standard widget files WGT


%package -n xpk-backend
Summary: backend of XPK files

%description -n xpk-backend
backend for standard widget files XPK


%prep
%setup -q

%build
%reconfigure
make %{?_smp_mflags}

%install
%make_install

mkdir -p %{buildroot}/etc/package-manager/backend
ln -s /usr/bin/wgt-backend %{buildroot}/etc/package-manager/backend/wgt
ln -s /usr/bin/xpk-backend %{buildroot}/etc/package-manager/backend/xpk



%post
ldconfig

%postun

%files
%defattr(-,root,root)
%{_libdir}/libcommonInstaller.so*

%files -n wgt-backend
/etc/package-manager/backend/wgt
%files -n xpk-backend
/etc/package-manager/backend/xpk
