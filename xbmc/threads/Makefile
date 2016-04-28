SRCS=Atomics.cpp \
     Event.cpp \
     Thread.cpp \
     Timer.cpp \
     SystemClock.cpp \
     platform/Implementation.cpp

LIB=threads.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
