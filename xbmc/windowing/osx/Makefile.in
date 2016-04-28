SRCS= \
      WinEventsOSX.mm \
      WinSystemOSX.mm \
      WinSystemOSXGL.mm
     
.SUFFIXES : .m .mm
LIB=windowing_osx.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.mm,%.P,$(SRCS))
