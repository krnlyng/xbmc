SRCS=	\
	TestEvent.cpp \
	TestSharedSection.cpp \
	TestAtomics.cpp \
	TestThreadLocal.cpp

LIB=threadTest.a

INCLUDES += -I../../../lib/gtest/include

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
