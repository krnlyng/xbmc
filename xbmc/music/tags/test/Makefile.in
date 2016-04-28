SRCS= \
  TestTagLoaderTagLib.cpp

LIB=tagsTest.a

INCLUDES += -I../../../../lib/gtest/include

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
