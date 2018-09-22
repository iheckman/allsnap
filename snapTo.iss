;--snapTo Install
;
;

[Setup]
OutputBaseFilename=snapto100
OutputDir=I:\IVAN\snapTo\site
AppName=snapTo
AppVerName=snapTo version 1.0
AppMutex=IVAN_HECKMAN_SNAPTO_MUTEX
AppCopyright=Copyright (C) 2002 Ivan Heckman
DefaultDirName={pf}\snapTo
DefaultGroupName=snapTo
UninstallDisplayIcon={app}\snapTo.exe
AlwaysCreateUninstallIcon=yes
AllowNoIcons=yes
; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

[Files]
Source: "snapTo\Release\snapTo.exe"; DestDir: "{app}"
Source: "Release\snapToA.exe"; DestDir: "{app}"
Source: "Release\snap_libA.dll"; DestDir: "{app}"
Source: "Release_UNICODE\snapToW.exe"; DestDir: "{app}"
Source: "Release_UNICODE\snap_libW.dll"; DestDir: "{app}"
Source: "snap.wav"; DestDir: "{app}"
Source: "unsnap.wav"; DestDir: "{app}"
Source: "ReadMe.txt"; DestDir: "{app}"; Flags: isreadme

[Tasks]
Name: startup; Description: "Run snapTo on &startup"; Flags: unchecked;
Name: desktopicon; Description: "Create a &desktop icon";  Flags: unchecked
Name: quicklaunchicon; Description: "Create a &quick launch icon";  Flags: unchecked


[Icons]
Name: "{group}\snapTo"; Filename: "{app}\snapTo.EXE"; WorkingDir: "{app}"
Name: "{userdesktop}\snapTo"; Filename: "{app}\snapTo.exe"; WorkingDir: "{app}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\snapTo"; Filename: "{app}\snapTo.exe";WorkingDir: "{app}"; Tasks: quicklaunchicon
Name: "{userstartup}\snapTo"; Filename: "{app}\snapTo.exe"; WorkingDir: "{app}"; Tasks: startup
; NOTE: Most apps do not need registry entries to be pre-created. If you
; don't know what the registry is or if you need to use it, then chances are
; you don't need a [Registry] section.

[Registry]
; Start "Software\My Company\My Program" keys under HKEY_CURRENT_USER
; and HKEY_LOCAL_MACHINE. The flags tell it to always delete the
; "My Program" keys upon uninstall, and delete the "My Company" keys
; if there is nothing left in them.
Root: HKCU; Subkey: "Software\ICH"; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: "Software\ICH\snapTo"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\ICH\snapTo\Settings"; ValueType: string; ValueName: "snap_sound_file"; ValueData: "{app}\snap.wav"
Root: HKCU; Subkey: "Software\ICH\snapTo\Settings"; ValueType: string; ValueName: "unsnap_sound_file"; ValueData: "{app}\unsnap.wav"

[Run]
Filename: "{app}\snapTo.EXE"; Description: "Launch application"; Flags: postinstall nowait skipifsilent unchecked
