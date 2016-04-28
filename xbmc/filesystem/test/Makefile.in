SRCS= \
  TestDirectory.cpp \
  TestFile.cpp \
  TestFileFactory.cpp \
  TestNfsFile.cpp \
  TestRarFile.cpp \
  TestZipFile.cpp

LIB=filesystemTest.a

INCLUDES += -I../../../lib/gtest/include

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
