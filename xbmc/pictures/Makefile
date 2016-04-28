SRCS=GUIDialogPictureInfo.cpp \
     GUIViewStatePictures.cpp \
     GUIWindowPictures.cpp \
     GUIWindowSlideShow.cpp \
     Picture.cpp \
     PictureInfoLoader.cpp \
     PictureInfoTag.cpp \
     PictureScalingAlgorithm.cpp \
     PictureThumbLoader.cpp \
     SlideShowPicture.cpp \
     
LIB=pictures.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
