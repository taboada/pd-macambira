# Microsoft Developer Studio Project File - Name="zexy" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ZEXY - WIN32 RELEASE
!MESSAGE Dies ist kein g�ltiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und f�hren Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "zexy.mak".
!MESSAGE 
!MESSAGE Sie k�nnen beim Ausf�hren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "zexy.mak" CFG="ZEXY - WIN32 RELEASE"
!MESSAGE 
!MESSAGE F�r die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "zexy - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "obj\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZEXY_EXPORTS" /YX /FD /c
# ADD CPP /nologo /Zp16 /W3 /GX /I "..\..\..\pd\src" /D "WIN32" /D "NT" /D "_WINDOWS" /D "ZEXY" /D "Z_WANT_LPT" /FR /YX /FD /c
# SUBTRACT CPP /O<none>
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0xc07 /d "NDEBUG"
# ADD RSC /l 0xc07
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib wsock32.lib uuid.lib libc.lib oldnames.lib pd.lib /nologo /dll /machine:I386 /nodefaultlib /out:"..\zexy.dll" /libpath:"../../bin" /libpath:"C:\Programme\pd\bin" /export:zexy_setup
# SUBTRACT LINK32 /pdb:none
# Begin Target

# Name "zexy - Win32 Release"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\a2l.c
# End Source File
# Begin Source File

SOURCE=.\atoi.c
# End Source File
# Begin Source File

SOURCE=.\avg_tilde.c
# End Source File
# Begin Source File

SOURCE=.\blockmirror_tilde.c
# End Source File
# Begin Source File

SOURCE=.\blockswap_tilde.c
# End Source File
# Begin Source File

SOURCE=.\date.c
# End Source File
# Begin Source File

SOURCE=.\demultiplex.c
# End Source File
# Begin Source File

SOURCE=.\demultiplex_tilde.c
# End Source File
# Begin Source File

SOURCE=.\dfreq_tilde.c
# End Source File
# Begin Source File

SOURCE=.\dirac_tilde.c
# End Source File
# Begin Source File

SOURCE=.\dot.c
# End Source File
# Begin Source File

SOURCE=.\drip.c
# End Source File
# Begin Source File

SOURCE=.\envrms_tilde.c
# End Source File
# Begin Source File

SOURCE=.\glue.c
# End Source File
# Begin Source File

SOURCE=.\index.c
# End Source File
# Begin Source File

SOURCE=.\length.c
# End Source File
# Begin Source File

SOURCE=.\limiter_tilde.c
# End Source File
# Begin Source File

SOURCE=.\list2int.c
# End Source File
# Begin Source File

SOURCE=.\list2symbol.c
# End Source File
# Begin Source File

SOURCE=.\lister.c
# End Source File
# Begin Source File

SOURCE=.\lpt.c
# End Source File
# Begin Source File

SOURCE=.\makesymbol.c
# End Source File
# Begin Source File

SOURCE=.\mavg.c
# End Source File
# Begin Source File

SOURCE=.\minmax.c
# End Source File
# Begin Source File

SOURCE=.\msgfile.c
# End Source File
# Begin Source File

SOURCE=.\multiline_tilde.c
# End Source File
# Begin Source File

SOURCE=.\multiplex.c
# End Source File
# Begin Source File

SOURCE=.\multiplex_tilde.c
# End Source File
# Begin Source File

SOURCE=.\niagara.c
# End Source File
# Begin Source File

SOURCE=.\noish_tilde.c
# End Source File
# Begin Source File

SOURCE=.\noisi_tilde.c
# End Source File
# Begin Source File

SOURCE=.\operating_system.c
# End Source File
# Begin Source File

SOURCE=.\pack_tilde.c
# End Source File
# Begin Source File

SOURCE=.\packel.c
# End Source File
# Begin Source File

SOURCE=.\pdf_tilde.c
# End Source File
# Begin Source File

SOURCE=.\prime.c
# End Source File
# Begin Source File

SOURCE=.\quantize_tilde.c
# End Source File
# Begin Source File

SOURCE=.\repack.c
# End Source File
# Begin Source File

SOURCE=.\repeat.c
# End Source File
# Begin Source File

SOURCE=.\sfplay.c
# End Source File
# Begin Source File

SOURCE=.\sfrecord.c
# End Source File
# Begin Source File

SOURCE=.\sigzero_tilde.c
# End Source File
# Begin Source File

SOURCE=.\sort.c
# End Source File
# Begin Source File

SOURCE=.\step_tilde.c
# End Source File
# Begin Source File

SOURCE=.\strcmp.c
# End Source File
# Begin Source File

SOURCE=.\sum.c
# End Source File
# Begin Source File

SOURCE=.\swap_tilde.c
# End Source File
# Begin Source File

SOURCE=.\symbol2list.c
# End Source File
# Begin Source File

SOURCE=.\tabdump.c
# End Source File
# Begin Source File

SOURCE=.\tabminmax.c
# End Source File
# Begin Source File

SOURCE=.\tabset.c
# End Source File
# Begin Source File

SOURCE=.\tavg_tilde.c
# End Source File
# Begin Source File

SOURCE=.\time.c
# End Source File
# Begin Source File

SOURCE=.\unpack_tilde.c
# End Source File
# Begin Source File

SOURCE=.\urn.c
# End Source File
# Begin Source File

SOURCE=.\winNT_portio.c
# End Source File
# Begin Source File

SOURCE=.\wrap.c
# End Source File
# Begin Source File

SOURCE=.\z_sigbin.c
# End Source File
# Begin Source File

SOURCE=.\z_tilde.c
# End Source File
# Begin Source File

SOURCE=.\zexy.c
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\m_pd.h
# End Source File
# Begin Source File

SOURCE=.\Zexy.h
# End Source File
# End Group
# Begin Group "generic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\z_zexy.h
# End Source File
# End Group
# End Target
# End Project
