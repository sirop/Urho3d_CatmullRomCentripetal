TEMPLATE = app
CONFIG -= console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG +=  c++11
CONFIG += release

win32: QMAKE_CFLAGS_RELEASE -= /MD
win32: QMAKE_CXXFLAGS_RELEASE -= /MD



win32: QMAKE_CFLAGS_RELEASE += /MT # /O2 #/Ob2 /DNDEBUG  /fp:fast /Zi /GS- /D _SECURE_SCL=0
macos: QMAKE_CFLAGS_RELEASE = -O3 -DNDEBUG -Wno-invalid-offsetof -ffast-math


TARGET = CatmullRom

#This might be an overkill but I just added all of them as I saw them in the pkgconfig file
#it is for a release build with static library linkage and having angelscript enabled
win32: DEFINES += WIN32 _WINDOWS NDEBUG _SECURE_SCL=0 ENABLE_SSE URHO3D_MINIDUMPS
win32: DEFINES += URHO3D_FILEWATCHER URHO3D_PROFILING URHO3D_LOGGING URHO3D_THREADING URHO3D_ANGELSCRIPT
win32: DEFINES += _CRT_SECURE_NO_WARNINGS HAVE_STDINT_H




# $(URHO_HOME) should point to your Urho3D installation folder
#URHO_HOME = D:\Urho3D\install
win32: INCLUDEPATH += D:\Urho3D\install\include D:\Urho3D\install\include\Urho3D\ThirdParty


#If you wish you can add these ones depending on your needs
# INCLUDEPATH += $(URHO_HOME)\include\Bullet $(URHO_HOME)\include\Direct3D9 $(URHO_HOME)\include\kNet


#$(DIRECTX_SDK) = where your directX sdk is installed for Windows and don't forget to replace x86 with x64 for those on x64 systems
win32: LIBS += -LD:\Urho3D\install\lib   -lUrho3D  # "$(DIRECTX_SDK) \Lib\x86\d3d9.lib"

#Then these guys, every single one of them seems to be needed on Windows
win32: LIBS += kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib
win32: LIBS += dbghelp.lib imm32.lib version.lib winmm.lib ws2_32.lib d3dcompiler.lib d3d11.lib dxgi.lib dxguid.lib

win32: LIBS += -LD:\QtProjects\serial\visual_studio\x64\Release
win32: LIBS += serial.lib
win32: INCLUDEPATH += D:\QtProjects\serial\include D:\QtProjects\serial\include\serial\impl



macos: DEFINES += URHO3D_STATIC_DEFINE URHO3D_ANGELSCRIPT URHO3D_FILEWATCHER URHO3D_IK URHO3D_LOGGING URHO3D_NAVIGATION
macos: DEFINES +=  URHO3D_NETWORK URHO3D_PHYSICS URHO3D_PROFILING URHO3D_THREADING URHO3D_URHO2D
#macos: LIBS += -pthread

macos: LIBS += -L/Users/mike/Urho3D/install/lib/Urho3D   -lUrho3D -liconv #-pthread
macos: INCLUDEPATH += /Users/mike/Urho3D/install/include /Users/mike/Urho3D/install/include/Urho3D/ThirdParty

macos: LIBS += -F/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/System/Library/Frameworks
macos: LIBS += -framework Foundation
macos: LIBS += -framework IOKit
macos: LIBS += -framework AudioToolbox -framework Carbon  -framework Cocoa -framework CoreFoundation
macos: LIBS += -framework SystemConfiguration -framework CoreAudio -framework CoreServices -framework CoreVideo
macos: LIBS += -framework ForceFeedback  -framework OpenGL
macos: LIBS += -L/Users/mike/serial/lib -lserial
macos: INCLUDEPATH += /Users/mike/serial/include /Users/mike/serial/include/serial/impl
message($$INCLUDEPATH)




HEADERS += Sample.h \
           MySpline\Spline.h \
           MySplinePath\SplinePath.h \ #\
           CatmullRom.h
           CatmullRom.h

SOURCES += Sample.inl \
           CatmullRom.cpp \
           MySpline\Spline.cpp \
           MySplinePath\SplinePath.cpp

