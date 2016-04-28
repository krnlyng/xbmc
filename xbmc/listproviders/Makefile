SRCS  = DirectoryProvider.cpp
SRCS += IListProvider.cpp
SRCS += StaticProvider.cpp
     
LIB=listproviders.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
