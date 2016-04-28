SRCS=DPMSSupport.cpp \
     PowerManager.cpp \
     IPowerSyscall.cpp \

LIB=powermanagement.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
