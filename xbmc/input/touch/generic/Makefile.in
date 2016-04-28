SRCS=GenericTouchActionHandler.cpp \
     GenericTouchInputHandler.cpp \
     GenericTouchPinchDetector.cpp \
     GenericTouchRotateDetector.cpp \
     GenericTouchSwipeDetector.cpp \

LIB=input_touch_generic.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
