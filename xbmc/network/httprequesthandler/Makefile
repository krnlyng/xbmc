SRCS=HTTPFileHandler.cpp \
     HTTPImageHandler.cpp \
     HTTPImageTransformationHandler.cpp \
     HTTPJsonRpcHandler.cpp \
     HTTPPythonHandler.cpp \
     HTTPVfsHandler.cpp \
     HTTPWebinterfaceAddonsHandler.cpp \
     HTTPWebinterfaceHandler.cpp \
     IHTTPRequestHandler.cpp \

LIB=httprequesthandlers.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
