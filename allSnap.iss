;--allSnap Install
;
;

[Setup]
OutputBaseFilename=asnp1314
AppName=allSnap
AppVerName=allSnap version 1.34 Beta 4 [Non Debug]
AppMutex=IVAN_HECKMAN_ALLSNAP_MUTEX,IVAN_HECKMAN_ALLSNAP_MUTEXB
AppCopyright=Copyright (C) 2002-2004 Ivan Heckman
AppPublisher=Ivan Heckman
AppPublisherURL=http://members.rogers.com/ivanheckman
AppUpdatesURL=http://members.rogers.com/ivanheckman
AppVersion=1.31.4
DefaultDirName={pf}\allSnap
DefaultGroupName=allSnap
UninstallDisplayIcon={app}\allSnap.exe
LicenseFile=EULA.txt
AllowNoIcons=yes
; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

[Files]
Source: "Release\snapToA.exe"; DestDir: "{app}"; DestName: "allSnap.exe"; MinVersion: 1, 0
Source: "Release\snap_libA.dll"; DestDir: "{app}"; MinVersion: 1, 0
Source: "Release_UNICODE\snapToW.exe"; DestDir: "{app}"; DestName: "allSnap.exe"; MinVersion: 0, 1
Source: "Release_UNICODE\snap_libW.dll"; DestDir: "{app}"; MinVersion: 0, 1
Source: "Release\auto_lineup.dll"; DestDir: "{app}";
Source: "snap.wav"; DestDir: "{app}"
Source: "unsnap.wav"; DestDir: "{app}"
Source: "allSnap.chm"; DestDir: "{app}"

[Tasks]
Name: startup; Description: "Run allSnap on &startup"; Flags: unchecked;
Name: desktopicon; Description: "Create a &desktop icon";  Flags: unchecked
Name: quicklaunchicon; Description: "Create a &quick launch icon";  Flags: unchecked


[Code]
function MessageBox(hWnd: Integer; lpText, lpCaption: String; uType: Cardinal): Integer;
external 'MessageBoxA@user32.dll stdcall';
               {
function InitializeSetup(): Boolean;
begin
  if (RegKeyExists(HKCU,'Software\IvanHeckman\allSnap')) then begin
      Result := IDYES = MsgBox('A pre-existing version of allSnap was detected on your computer.' #13#13
      'It is suggested that you uninstall any old version before installing this one.' #13
      'Do you still want to continue installing this version?', mbInformation, MB_YESNO);
  end else
    Result := True;
end;
           }
function GetPathInstalled( AppID: String ): String;
var
   sPrevPath: String;
begin
  sPrevPath := '';
  if not RegQueryStringValue( HKLM,
    'Software\Microsoft\Windows\CurrentVersion\Uninstall\'+AppID+'_is1',
    'Inno Setup: App Path', sPrevpath) then
    RegQueryStringValue( HKCU, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\'+AppID+'_is1' ,
      'Inno Setup: App Path', sPrevpath);

  Result := sPrevPath;
end;


function InitializeSetup(): Boolean;
var
	sPrevPath: String;
	sPrevID: String;
begin
  sPrevID := 'allSnap';
  sPrevPath := GetPathInstalled( sprevID );

  if ( Length(sPrevPath) > 0 ) then begin
     Result := IDYES = MsgBox('A pre-existing version of allSnap was detected on your computer.' #13#13
      'It is suggested that you uninstall any old version before installing this one.' #13
      'Do you still want to continue installing this version?', mbInformation, MB_YESNO);
  end else
    Result := True;
end;


[Icons]
Name: "{group}\allSnap"; Filename: "{app}\allSnap.EXE"; WorkingDir: "{app}"
Name: "{group}\allSnap Help"; Filename: "{app}\allSnap.chm"; WorkingDir: "{app}"
Name: "{userdesktop}\allSnap"; Filename: "{app}\allSnap.exe"; WorkingDir: "{app}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\allSnap"; Filename: "{app}\allSnap.exe";WorkingDir: "{app}"; Tasks: quicklaunchicon
Name: "{userstartup}\allSnap"; Filename: "{app}\allSnap.exe"; WorkingDir: "{app}"; Tasks: startup
; NOTE: Most apps do not need registry entries to be pre-created. If you
; don't know what the registry is or if you need to use it, then chances are
; you don't need a [Registry] section.

[Registry]
; Start "Software\My Company\My Program" keys under HKEY_CURRENT_USER
; and HKEY_LOCAL_MACHINE. The flags tell it to always delete the
; "My Program" keys upon uninstall, and delete the "My Company" keys
; if there is nothing left in them.
Root: HKCU; Subkey: "Software\IvanHeckman"; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: "Software\IvanHeckman\allSnap"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\IvanHeckman\allSnap\Settings"; ValueType: string; ValueName: "snap_sound_file"; ValueData: "{app}\snap.wav"
Root: HKCU; Subkey: "Software\IvanHeckman\allSnap\Settings"; ValueType: string; ValueName: "unsnap_sound_file"; ValueData: "{app}\unsnap.wav"
Root: HKCU; Subkey: "Software\IvanHeckman\allSnap\Settings"; ValueType: multisz; ValueName: "skinned_windows"; ValueData: "Shell_TrayWnd{break}icoCTactTrilly{break}ConsoleWindowClass{break}Winamp PE{break}Winamp EQ{break}Winamp MB{break}Winamp v1.x{break}BaseWindow_RootWnd"
Root: HKCU; Subkey: "Software\IvanHeckman\allSnap\Settings"; ValueType: multisz; ValueName: "ignored_windows"; ValueData: "Progman{break}IDEOwner{break}NeilShadow"

[Run]
Filename: "{app}\allSnap.EXE"; Description: "Launch application"; Flags: postinstall nowait skipifsilent unchecked
