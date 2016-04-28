SRCS= \
	Exception.cpp \
	ilog.cpp

LIB=commons.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(patsubst %.S,,$(SRCS))))
