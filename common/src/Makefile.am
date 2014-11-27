AM_CPPFLAGS = -I../include
lib_LTLIBRARIES = libcommonInstaller.la

libcommonInstaller_la_CPPFLAGS =$(AM_CPPFLAGS) $(COMMON_DEPS_CFLAGS)
libcommonInstaller_la_LDFLAGS = $(COMMON_DEPS_LIBS)

libcommonInstaller_la_SOURCES = AppInstaller.cxx AppInstaller.hxx step.hxx 

#lib_LTLIBRARIES += libcommonSteps.la
#libcommonSteps_la_SOURCES = step-unzip.cxx step-unzip.hxx
