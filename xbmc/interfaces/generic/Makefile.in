SRCS=ILanguageInvoker.cpp \
     LanguageInvokerThread.cpp \
     ScriptInvocationManager.cpp \

LIB=interfaces-generic.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
