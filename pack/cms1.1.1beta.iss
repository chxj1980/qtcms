; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "CMS"
#define MyAppVersion "1.1.14"
#define MyAppExeName "CMS.exe"
#define MyDefaultDirName "D:"
#define SourceDir "E:\code\qtcms"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{C300BB2E-1079-43BF-A820-97BB23065926}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}

DefaultGroupName={#MyAppName}
AllowNoIcons=yes
OutputDir={#SourceDir}\pack
OutputBaseFilename= CMS
SetupIconFile={#SourceDir}\pack\main.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "en_GB"; MessagesFile: "compiler:Default.isl"
Name: "zh_CN"; MessagesFile: "compiler:Languages\SimpleChinese.isl"
[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "{#SourceDir}\output\Release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
; Source: "{#SourceDir}\output\Release\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "libutinity.dll"; Flags: dontcopy
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}" ;IconFilename: "{app}\��Ŀ1.ico";
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}";IconFilename: "{app}\��Ŀ1.ico";
Name: "{group}\Player for old version"; Filename: "{app}\Player\Player.exe";IconFilename: "{app}\��Ŀ1.ico";
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}";  IconFilename: "{app}\��Ŀ1.ico";Tasks: desktopicon   ;WorkingDir:"{app}"
Name: "{commondesktop}\Player for old version"; Filename: "{app}\Player\Player.exe";  IconFilename: "{app}\��Ŀ1.ico";Tasks: desktopicon   ;WorkingDir:"{app}"
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}";  IconFilename: "{app}\��Ŀ1.ico";Tasks: quicklaunchicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Code]
function setCmsLanguage(language : pchar; path : pchar) : Integer;
external 'setCmsLanguage@files:libutinity.dll stdcall';

procedure curLanguage();
var
  language : String;
begin
  language := ActiveLanguage();
  setCmsLanguage(pchar(language),ExpandConstant('{app}'));
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    // I can chage the cms language here
    curLanguage();
  end;
end;
