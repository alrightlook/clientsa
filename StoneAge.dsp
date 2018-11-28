# Microsoft Developer Studio Project File - Name="StoneAge" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=StoneAge - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "StoneAge.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "StoneAge.mak" CFG="StoneAge - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "StoneAge - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "StoneAge - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "StoneAge - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ddraw.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dsound.lib winmm.lib imm32.lib dinput.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /out:"StoneAge.exe"

!ELSEIF  "$(CFG)" == "StoneAge - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "SYSTEMINC" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_DEB UG_MSG" /D "_LOG_MSG" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ddraw.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dsound.lib winmm.lib imm32.lib dinput.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Ä§Á¦_cg.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "StoneAge - Win32 Release"
# Name "StoneAge - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\system\action.cpp
# End Source File
# Begin Source File

SOURCE=.\system\battleMap.cpp
# End Source File
# Begin Source File

SOURCE=.\system\battleMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\system\battleProc.cpp
# End Source File
# Begin Source File

SOURCE=.\system\character.cpp
# End Source File
# Begin Source File

SOURCE=.\system\chat.cpp
# End Source File
# Begin Source File

SOURCE=.\system\directDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\system\field.cpp
# End Source File
# Begin Source File

SOURCE=.\system\font.cpp
# End Source File
# Begin Source File

SOURCE=.\system\gamemain.cpp
# End Source File
# Begin Source File

SOURCE=.\system\Gemini.cpp
# End Source File
# Begin Source File

SOURCE=.\system\handletime.cpp
# End Source File
# Begin Source File

SOURCE=.\system\ime.cpp
# End Source File
# Begin Source File

SOURCE=.\system\init.cpp
# End Source File
# Begin Source File

SOURCE=.\system\loadrealbin.cpp
# End Source File
# Begin Source File

SOURCE=.\system\loadsprbin.cpp
# End Source File
# Begin Source File

SOURCE=.\system\login.cpp
# End Source File
# Begin Source File

SOURCE=.\system\lssproto_cli.cpp
# End Source File
# Begin Source File

SOURCE=.\system\lssproto_util.cpp
# End Source File
# Begin Source File

SOURCE=.\system\main.cpp
# End Source File
# Begin Source File

SOURCE=.\system\map.cpp
# End Source File
# Begin Source File

SOURCE=.\system\math2.cpp
# End Source File
# Begin Source File

SOURCE=.\system\menu.cpp
# End Source File
# Begin Source File

SOURCE=.\system\mouse.cpp
# End Source File
# Begin Source File

SOURCE=.\system\netmain.cpp
# End Source File
# Begin Source File

SOURCE=.\system\netproc.cpp
# End Source File
# Begin Source File

SOURCE=.\oft\oft.cpp
# End Source File
# Begin Source File

SOURCE=.\oft\pat_dat.cpp
# End Source File
# Begin Source File

SOURCE=.\system\Pattern.cpp
# End Source File
# Begin Source File

SOURCE=.\system\pc.cpp
# End Source File
# Begin Source File

SOURCE=.\system\process.cpp
# End Source File
# Begin Source File

SOURCE=.\system\produce.cpp
# End Source File
# Begin Source File

SOURCE=.\system\Radar.cpp
# End Source File
# Begin Source File

SOURCE=.\system\savedata.cpp
# End Source File
# Begin Source File

SOURCE=.\system\Sprdisp.cpp
# End Source File
# Begin Source File

SOURCE=.\system\sprmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\oft\sys2.cpp
# End Source File
# Begin Source File

SOURCE=.\system\t_music.cpp
# End Source File
# Begin Source File

SOURCE=.\system\testView.cpp
# End Source File
# Begin Source File

SOURCE=.\system\tool.cpp
# End Source File
# Begin Source File

SOURCE=.\system\unpack.cpp
# End Source File
# Begin Source File

SOURCE=.\oft\Work.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\POINTER.CUR
# End Source File
# Begin Source File

SOURCE=.\sa_mouse.cur
# End Source File
# Begin Source File

SOURCE=.\Script1.rc
# End Source File
# End Group
# End Target
# End Project
