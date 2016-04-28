SRCS=ConsoleDeviceKitPowerSyscall.cpp \
     ConsoleUPowerSyscall.cpp \
     UPowerSyscall.cpp \
     LogindUPowerSyscall.cpp

LIB=powermanagement_linux.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
