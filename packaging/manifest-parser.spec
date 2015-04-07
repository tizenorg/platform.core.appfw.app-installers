Name:           manifest-parser
Summary:        Manifest parser
Version:        1.0
Release:        0
Group:          Application Framework/Package Management
License:        Apache-2.0
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  boost-devel
BuildRequires:  cmake
BuildRequires:  gtest-devel
BuildRequires:  pkgconfig(pkgmgr-parser)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(libiri)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  pkgconfig(vconf)

Requires: ca-certificates-tizen
Requires: libtzplatform-config

%description
This is a meta package that installs manifest parser.

%package devel
Summary:    Manifest parser development files
Group:      Application Framework/Package Management
Requires:   %{name} = %{version}
Requires:   boost-devel
Provides:   manifest-parser-devel

%description devel
This package contains necessaary files for manifest-parser development

%package tests
Summary: Unit tests for manifest-parser
Requires: %{name} = %{version}

%description tests
Unit tests for al modules of manifest-parser

%prep
%setup -q

%build
# BUILD_PARSER macro is defined so we can build two separate projects
# with cmake, which are on one repository. Later this macro
# is used in CMakeLists.txt files to distinguish, which project
# is currently being build.
%cmake . -DCMAKE_BUILD_TYPE=%{?build_type:%build_type} -DBUILD_PARSER=1
make %{?_smp_mflags}

%install
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%{_libdir}/libmanifest-parser.so*
%{_libdir}/libmanifest-handlers.so*
%license LICENSE LICENSE-xwalk

%files devel
%{_libdir}/pkgconfig/*.pc
%{_includedir}/*

%files tests
%{_bindir}/manifest-parser-ut/*
%{_datadir}/manifest-parser-ut/*
