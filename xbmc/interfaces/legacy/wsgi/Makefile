SRCS = WsgiErrorStream.cpp \
       WsgiInputStream.cpp \
       WsgiResponse.cpp \
       WsgiResponseBody.cpp \

LIB=legacy-wsgi.a

CXXFLAGS+= -DSWIGRUNTIME_DEBUG -DSTATIC_LINKED

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))

