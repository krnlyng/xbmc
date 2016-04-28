SRCS=NetworkLinux.cpp \
     ZeroconfAvahi.cpp \
     ZeroconfBrowserAvahi.cpp \

LIB=network_linux.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
