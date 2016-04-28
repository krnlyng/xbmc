SRCS=GUIDialogAudioDSPManager.cpp \
     GUIDialogAudioDSPSettings.cpp \
     GUIDialogContentSettings.cpp \
     GUIDialogSettingsBase.cpp \
     GUIDialogSettingsManagerBase.cpp \
     GUIDialogSettingsManualBase.cpp \

LIB=settings_dialogs.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
