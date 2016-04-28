SRCS=GUIControlSettings.cpp \
     GUIWindowSettings.cpp \
     GUIWindowSettingsCategory.cpp \
     GUIWindowSettingsScreenCalibration.cpp \
     GUIWindowTestPattern.cpp \

LIB=settings_windows.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
