AM_CPPFLAGS = -I$(top_srcdir)/common/include
AM_CPPFLAGS += -I../include

bin_PROGRAMS = wgt_backend

wgt_backend_CPPFLAGS =$(AM_CPPFLAGS) $(COMMON_DEPS_CFLAGS)
wgt_backend_LDFLAGS = $(COMMON_DEPS_LIBS)
wgt_backend__LDADD = libcommonInstaller.so
wgt_backend__DEPENDENCIES = libcommonInstaller.so
wgt_backend_SOURCES = wgt-backend.cxx
