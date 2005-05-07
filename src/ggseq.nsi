; ggseq.nsi

!define VERSION "0.3.0"

;--------------------------------

; The name of the installer
Name "Gungirl Sequencer ${VERSION}"

; The file to write
OutFile "ggseq-${VERSION}.exe"

; Erst einschalten wenn ggseq auch styles unterstützt
; XPStyle on

; The default installation directory
InstallDir $PROGRAMFILES\ggseq-${VERSION}

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\NSIS_ggseq-${VERSION}" "Install_Dir"

Icon ../icons/ggseq_256.ico
UninstallIcon ../icons/uninst_256.ico

;LoadLanguageFile "${NSISDIR}\Contrib\Language Files\English.nlf"
;--------------------------------
;Version Information

 ; VIProductVersion ${VERSION}.0
 ; VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Gungirl Sequencer"
 ; VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© Richard Spindler"
 ; VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Gungirl Sequencer Installer"
 ; VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" ${VERSION}

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Gungirl Sequencer" 

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  file mingwm10.dll
  File ggseq.exe
  File libsndfile-1.dll
  File libsamplerate.dll
  File ..\doc\ggseq.htb
  File ..\COPYING
  
    ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\NSIS_ggseq-${VERSION} "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ggseq-${VERSION}" "DisplayName" "Gungirl Sequencer"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ggseq-${VERSION}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ggseq-${VERSION}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ggseq-${VERSION}" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

  ; Write .ggseq File stuff
  
  WriteRegStr HKCR ".ggseq" "" "GGSEQ.File"
  WriteRegStr HKCR "GGSEQ.File" "" "Gungirl Sequencer File"
  WriteRegStr HKCR "GGSEQ.File\DefaultIcon" "" "$INSTDIR\ggseq.exe,2"
  ReadRegStr $R0 HKCR "GGSEQ.File\shell\open\command" ""
  WriteRegStr HKCR "GGSEQ.File\shell" "" "open"
  WriteRegStr HKCR "GGSEQ.File\shell\open\command" "" '$INSTDIR\ggseq.exe "%1"'
  
SectionEnd ; end the section

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  SetShellVarContext all

  CreateDirectory "$SMPROGRAMS\Gungirl Sequencer"
  CreateShortCut "$SMPROGRAMS\Gungirl Sequencer\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Gungirl Sequencer\Gungirl Sequencer.lnk" "$INSTDIR\ggseq.exe" "" "$INSTDIR\ggseq.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"

  ReadRegStr $R0 HKCR ".ggseq" ""
  StrCmp $R0 "GGSEQ.File" 0 +2
    DeleteRegKey HKCR ".ggseq"

  DeleteRegKey HKCR "GGSEQ.File"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ggseq-${VERSION}"
  DeleteRegKey HKLM SOFTWARE\NSIS_ggseq-${VERSION}

  ; Remove files and uninstaller
  Delete $INSTDIR\ggseq.exe
  Delete $INSTDIR\mingwm10.dll
  Delete $INSTDIR\libsndfile-1.dll
  Delete $INSTDIR\libsamplerate.dll
  Delete $INSTDIR\ggseq.htb
  Delete $INSTDIR\COPYING
  Delete $INSTDIR\uninstall.exe

  SetShellVarContext all

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Gungirl Sequencer\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Gungirl Sequencer"
  RMDir "$INSTDIR"

SectionEnd
