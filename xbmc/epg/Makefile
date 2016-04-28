INCLUDES=-I. -I.. -I../../ -I../linux -I../cores -I../../guilib -I../posix -I../utils

SRCS=EpgInfoTag.cpp \
	EpgSearchFilter.cpp \
	Epg.cpp \
	EpgContainer.cpp \
	EpgDatabase.cpp \
	GUIEPGGridContainer.cpp

LIB=epg.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
