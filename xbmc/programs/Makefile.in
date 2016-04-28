SRCS=GUIViewStatePrograms.cpp \
     GUIWindowPrograms.cpp \
     
LIB=programs.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
