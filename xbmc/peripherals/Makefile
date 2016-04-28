SRCS=EventScanner.cpp \
     EventScanRate.cpp \
     Peripherals.cpp \

LIB=peripherals.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
