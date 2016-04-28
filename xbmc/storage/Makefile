SRCS=AutorunMediaJob.cpp \
     cdioSupport.cpp \
     DetectDVDType.cpp \
     IoSupport.cpp \
     MediaManager.cpp \
     
LIB=storage.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
