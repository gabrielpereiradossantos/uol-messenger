 ; UOL Messenger
 ; Copyright (c) 2005 Universo Online S/A
 ;
 ; Direitos Autorais Reservados
 ; All rights reserved
 ;
 ; Este programa é software livre; você pode redistribuí-lo e/ou modificá-lo
 ; sob os termos da Licença Pública Geral GNU conforme publicada pela Free
 ; Software Foundation; tanto a versão 2 da Licença, como (a seu critério)
 ; qualquer versão posterior.
 ; Este programa é distribuído na expectativa de que seja útil, porém,
 ; SEM NENHUMA GARANTIA; nem mesmo a garantia implícita de COMERCIABILIDADE
 ; OU ADEQUAÇÃO A UMA FINALIDADE ESPECÍFICA. Consulte a Licença Pública Geral
 ; do GNU para mais detalhes. 
 ; Você deve ter recebido uma cópia da Licença Pública Geral do GNU junto
 ; com este programa; se não, escreva para a Free Software Foundation, Inc.,
 ; no endereço 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA. 
 ;
 ; This program is free software; you can redistribute it and/or
 ; modify it under the terms of the GNU General Public License
 ; as published by the Free Software Foundation; either version 2
 ; of the License, or (at your option) any later version.
 ; This program is distributed in the hope that it will be useful,
 ; but WITHOUT ANY WARRANTY; without even the implied warranty of
 ; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ; GNU General Public License for more details.
 ; You should have received a copy of the GNU General Public License
 ; along with this program; if not, write to the Free Software
 ; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ;
 ; Universo Online S/A - A/C: UOL Messenger 5o. Andar
 ; Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
 ; São Paulo SP - CEP 01452-002 - BRASIL
 

 ; OBS: script adequado para NSIS v2.25
 ; OBS: script compliant with NSIS v2.25

; Localização

UninstallCaption "${PRODUCT_NAME}"
Caption "${PRODUCT_NAME}"
SubCaption 0 ": Termo De Uso"
SubCaption 1 ": Opções de instalação"
SubCaption 2 ": Pasta de instalação"
SubCaption 3 ": Instalando arquivos"
SubCaption 4 ": Concluído"
BrandingText "NULLSOFT INSTALL SYSTEM"
MiscButtonText "<< Voltar" "Avançar >>" "Cancelar" "Fechar"
LicenseText "" "Concordo"
ComponentText "" "Selecione o tipo de instalação:" "ou selecione os componentes a instalar:"
SpaceTexts "Espaço Necessário: " "Espaço Disponível: "
DirText "" "" "Escolher..."
UninstallText "" "Remover De: "
UninstallSubCaption 0 ": Confirmação"
UninstallSubCaption 1 ": Removendo Arquivos"
UninstallSubCaption 2 ": Concluído"
UninstallButtonText "Remover"
InstallButtonText "Instalar"
DetailsButtonText "Ver detalhes"
CompletedText "Concluído"
ShowInstDetails show
XPStyle on
InstallColors 000000 ffffff

; Vista
RequestExecutionLevel admin

!include Library.nsh
!include FileFunc.nsh
!include ..\include\zipdll.nsh
!include ..\include\InstallerUtils.nsh

!insertmacro RefreshShellIcons


!addplugindir ..\..\..\..\build
!addplugindir ..\nsis_plugins

Var /Global ALREADY_CONTROL
Var /Global ALREADY_MSXML

!define OLD_PRODUCT_LINK_1 "UOL Fone"
!define OLD_PRODUCT_LINK_2 "UOL Fone 2"

!define SHELLFOLDERS "Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders"
!define DIR_BUILD "..\..\..\..\build"

!define SYNC_TERM 0x00100001

!macro TerminateApp
 
    Push $0 ; window handle
    Push $1
    Push $2 ; process handle

    FindWindow $0 '' '${PRODUCT_NAME}' NULL
;    FindWindow $0 '${WND_CLASS}' ''
    IntCmp $0 0 done

    MessageBox MB_OK|MB_ICONEXCLAMATION "O ${PRODUCT_NAME} está sendo executado e será encerrado.$\nVerifique se existem mensagens não lidas antes de pressionar OK."
    System::Call 'user32.dll::GetWindowThreadProcessId(i r0, *i .r1) i .r2'
    System::Call 'kernel32.dll::OpenProcess(i ${SYNC_TERM}, i 0, i r1) i .r2'
    SendMessage $0 ${WM_CLOSE} 0 0 /TIMEOUT=2000
    SendMessage $0 ${WM_ENDSESSION} 1 0 /TIMEOUT=2000
    System::Call 'kernel32.dll::WaitForSingleObject(i r2, i 2000) i .r1'
    IntCmp $1 0 close
    MessageBox MB_YESNOCANCEL|MB_ICONEXCLAMATION "Não foi possivel fechar o ${PRODUCT_NAME}.$\nGostaria de finalizar o processo?" /SD IDYES IDYES terminate IDNO close
    System::Call 'kernel32.dll::CloseHandle(i r2) i .r1'
    Quit

  terminate:
    System::Call 'kernel32.dll::TerminateProcess(i r2, i 0) i .r1'

  close:
    System::Call 'kernel32.dll::CloseHandle(i r2) i .r1'

  done:
    Pop $2
    Pop $1
    Pop $0
 
!macroend


Function .onInit

   System::Call 'kernel32::CreateMutexA(i 0, i 0, t "UOLIMClientInstaller") i .r1 ?e'
   Pop $R0
 
   StrCmp $R0 0 lbl_JumpInstallerInstanceError

   MessageBox MB_OK|MB_ICONSTOP "No momento, não é possível instalar o ${PRODUCT_NAME}. Há uma instalação ou remoção em andamento. (UIM0129)"
   Abort

lbl_JumpInstallerInstanceError:

   System::Call 'kernel32::CreateMutexA(i 0, i 0, t "UOLIMSingleInstanceMutex") i .r1 ?e'
   Pop $R0
 
   StrCmp $R0 0 lbl_JumpClientInstanceError

;!ifdef CORP_INSTALLER
   
   Push "ForceCloseApp"
   Push "No"
   Call GetParameterValue
   Pop $2
   StrCmp $2 "yes" _lbl_WarnForcedClosing
   
   MessageBox MB_YESNO|MB_ICONEXCLAMATION "O ${PRODUCT_NAME} está sendo executado.$\nDeseja que ele seja encerrado para continuar a instalação? $\n$\nVerifique se existem mensagens não lidas antes de prosseguir. (UIM0128)" IDYES _lbl_ProcceedForcedClosing IDNO
   Quit
   
_lbl_WarnForcedClosing:
   
   MessageBox MB_OK|MB_ICONEXCLAMATION "O ${PRODUCT_NAME} está sendo executado e será encerrado.$\nVerifique se existem mensagens não lidas antes de pressionar OK. (UIM0126)"

_lbl_ProcceedForcedClosing:
   
   nsProcess::_KillProcess /NOUNLOAD "uim.exe"
   Pop $R0

   nsProcess::_Unload

   StrCmp $R0 0 lbl_JumpClientInstanceError
   StrCmp $R0 603 lbl_JumpClientInstanceError

   MessageBox MB_OK "Erro: [$R0] Não foi possível encerrar o ${PRODUCT_NAME} automaticamente. $\nFeche a aplicação manualmente antes de reiniciar a instalação. (UIM0127)"
;!else
;   MessageBox MB_OK|MB_ICONSTOP "O ${PRODUCT_NAME} está sendo executado.$\nÉ preciso fechá-lo antes de instalar uma nova versão. (UIM0130)"
;!endif

   Abort

lbl_JumpClientInstanceError:   

   Call GetPermission
   Pop $R0
   StrCmp $R0 1 lbl_PermissionOk
   MessageBox MB_OK|MB_ICONSTOP "Ocorreu um erro durante a instalação. Verifique se você tem privilégios de administrador do sistema. (UIM0121)"
   Abort
   	
lbl_PermissionOk:
   Call GetWindowsVersion
   Pop $R0

   StrCmp $R0 "NT 4.0" lbl_NotSupportedSystem
   StrCmp $R0 "95" lbl_NotSupportedSystem
   StrCmp $R0 "98" lbl_NotSupportedSystem
   StrCmp $R0 "ME" lbl_NotSupportedSystem

   Goto lbl_SupportedSystem

lbl_NotSupportedSystem:
   MessageBox MB_OK|MB_ICONSTOP "Atenção, o ${PRODUCT_NAME} só é suportado em Windows 2000, XP e superiores. (UIM0115)"
   Abort

lbl_SupportedSystem:
   StrCmp $R0 "98" lbl_CtxWin95
   StrCmp $R0 "ME" lbl_CtxWin95

   SetShellVarContext all
   Goto lbl_AfterContext

lbl_CtxWin95:
   SetShellVarContext current

lbl_AfterContext:

   Call GetInternetExplorerVersion
   Pop $R0

   StrCmp $R0 '' 0 lbl_SupportedIE 

   MessageBox MB_OK|MB_ICONSTOP "Versão do Internet Explorer incompatível. (UIM0116)"
   Abort

lbl_SupportedIE:
   Call GetUOLMessengerVersion 
   
   Pop $R0
   StrCmp $R0 "" lbl_continue

   Call GetUOLMessengerDisplayName 
   
   Pop $R0

!ifdef OLD_PRODUCT_NAME_1   
   StrCmp $R0 "${OLD_PRODUCT_NAME_1} (remover)" lbl_SameProduct
!endif
   
   StrCmp $R0 "${PRODUCT_NAME} (remover)" lbl_SameProduct lbl_NotSameProduct

lbl_SameProduct:
   MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "Você está reinstalando o ${PRODUCT_NAME}.$\nClique OK para prosseguir ou cancele a reinstalação. (UIM0134)" IDOK lbl_continue IDCANCEL
   Abort

lbl_NotSameProduct:
   MessageBox MB_YESNO|MB_ICONEXCLAMATION "Atenção! Você já possui o ${OPPOSITE_PRODUCT_NAME} instalado em seu computador. $\nO ${PRODUCT_NAME} e o ${OPPOSITE_PRODUCT_NAME} possuem as mesmas funcionalidades, tendo como único diferencial o visual.$\n$\nCaso continue a instalação, o ${PRODUCT_NAME} substituirá o ${OPPOSITE_PRODUCT_NAME}. Deseja continuar? (UIM0135)" IDYES lbl_continue IDNO
   ExecShell "open" "$INSTDIR\uim.exe" SW_SHOWNORMAL   
   Abort

lbl_continue:

FunctionEnd


Function .onUserAbort

   MessageBox MB_OK|MB_ICONSTOP "A Instalação do ${PRODUCT_NAME} foi cancelada. Para instalar o programa, é necessário aceitar o contrato. (UIM0118)" 
   Quit

 FunctionEnd


Function GetPermission

	UserInfo::GetName
	IfErrors lbl_Admin
	Pop $0
	UserInfo::GetAccountType
	Pop $1
	
	Push $R0
	StrCmp $1 "Admin" lbl_Admin lbl_NotAdmin
	
lbl_NotAdmin:
	StrCpy $R0 0
	Goto done


lbl_Admin:
	StrCpy $R0 1

done:
	Exch $R0
	

FunctionEnd


Function un.onInit

   System::Call 'kernel32::CreateMutexA(i 0, i 0, t "UOLIMClientInstaller") i .r1 ?e'
   Pop $R0
 
   StrCmp $R0 0 +3

   MessageBox MB_OK|MB_ICONEXCLAMATION "No momento, não é possível remover o ${PRODUCT_NAME}. Há uma instalação ou remoção em andamento. (UIM0125)"
   Abort

   System::Call 'kernel32::CreateMutexA(i 0, i 0, t "UOLIMSingleInstanceMutex") i .r1 ?e'
   Pop $R0
 
   StrCmp $R0 0 NoProgRunning

   MessageBox MB_OK|MB_ICONEXCLAMATION "O ${PRODUCT_NAME} não pode ser removido, porque está sendo executado.$\nFeche o programa antes de desinstalar. (UIM0123)"
   Abort

NoProgRunning:

   Call un.GetPermission
   Pop $R0
   StrCmp $R0 1 lbl_PermissionOk
   MessageBox MB_OK|MB_ICONSTOP "O ${PRODUCT_NAME} não pode ser removido. Verifique se você tem privilégios de administrador do sistema. (UIM0124)"
   Abort
   	
lbl_PermissionOk:

FunctionEnd


Function un.GetPermission

	UserInfo::GetName
	IfErrors lbl_Admin
	Pop $0
	UserInfo::GetAccountType
	Pop $1
	
	Push $R0
	StrCmp $1 "Admin" lbl_Admin lbl_NotAdmin
	
lbl_NotAdmin:
	StrCpy $R0 0
	Goto done


lbl_Admin:
	StrCpy $R0 1

done:
	Exch $R0
	

FunctionEnd


Function .onInstSuccess

   IfRebootFlag +2
   ExecShell "open" "$INSTDIR\uim.exe" SW_SHOWNORMAL

FunctionEnd


Function GetUOLMessengerVersion

  Push $R0

  ReadRegStr $R0 HKLM "Software\UOL\UIM" "Version"

  Exch $R0
 
FunctionEnd


Function GetUOLMessengerDisplayName

  Push $R0

  ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UIM" "DisplayName"

  Exch $R0
 
FunctionEnd


; GetUOLMessengerFolder
; Retorna no topo do stack a Pasta Comum do UOL Messenger
Function GetUOLMessengerFolder
 
  Push $R0

  ReadRegStr $R0 HKLM "${SHELLFOLDERS}" "Common AppData"
  StrCmp $R0 "" defaultAppData appDataFound

defaultAppData:
  StrCpy $R0 "$WINDIR\Application Data\"

appDataFound:
  StrCpy $R0 "$R0\UOL\UIM"

  Exch $R0
 
FunctionEnd


; un.GetUOLMessengerFolder
; Retorna no topo do stack a Pasta Comum do UOL Messenger

Function un.GetUOLMessengerFolder
  Push $R0

  ReadRegStr $R0 HKLM "${SHELLFOLDERS}" "Common AppData"
  StrCmp $R0 "" defaultAppData appDataFound

defaultAppData:
  StrCpy $R0 "$WINDIR\Application Data\"

appDataFound:
  StrCpy $R0 "$R0\UOL\UIM"

  Exch $R0

FunctionEnd


; GetUOLFolder
; Retorna no topo do stack a Pasta Comum do UOL
Function GetUOLFolder
 
  Push $R0

  ReadRegStr $R0 HKLM "${SHELLFOLDERS}" "Common AppData"
  StrCmp $R0 "" defaultAppData appDataFound

defaultAppData:
  StrCpy $R0 "$WINDIR\Application Data\"

appDataFound:
  StrCpy $R0 "$R0\UOL"

  Exch $R0
 
FunctionEnd


; GetUserAppDataFolder
; Retorna no topo do stack a Pasta de Dados do usuario corrente
Function GetUserAppDataFolder
 
  Push $R0

  ReadRegStr $R0 HKCU "${SHELLFOLDERS}" "AppData"
  StrCmp $R0 "" userDefaultAppData userAppDataFound

userDefaultAppData:
  StrCpy $R0 "$WINDIR\Application Data\"

userAppDataFound:

  Exch $R0
 
FunctionEnd


; GetCtrlCenterFolder
; Retorna no topo do stack a Pasta Comum do CtrlCenter
Function GetCtrlCenterFolder
 
  Push $R0

  StrCpy $R0 "$PROGRAMFILES\UOL\Controle"

  Exch $R0
 
FunctionEnd

; un.GetCtrlCenterFolder
; Retorna no topo do stack a Pasta Comum do CtrlCenter


 ; GetWindowsVersion
 ;
 ; Based on Yazno's function, http://yazno.tripod.com/powerpimpit/
 ; Updated by Joost Verburg
 ;
 ; Returns on top of stack
 ;
 ; Windows Version (95, 98, ME, NT x.x, 2000, XP, 2003)
 ; or
 ; '' (Unknown Windows Version)
 ;
 ; Usage:
 ;   Call GetWindowsVersion
 ;   Pop $R0
 ;   ; at this point $R0 is "NT 4.0" or whatnot

 
 Function GetWindowsVersion
 
   Push $R0
   Push $R1
   
   ReadRegStr $R0 HKLM \
   "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion

   IfErrors 0 lbl_winnt
   
   ; we are not NT
   ReadRegStr $R0 HKLM \
   "SOFTWARE\Microsoft\Windows\CurrentVersion" VersionNumber
 
   StrCpy $R1 $R0 1
   StrCmp $R1 '4' 0 lbl_error
 
   StrCpy $R1 $R0 3
 
   StrCmp $R1 '4.0' lbl_win32_95
   StrCmp $R1 '4.9' lbl_win32_ME lbl_win32_98
 
 lbl_win32_95:
     StrCpy $R0 '95'
   Goto lbl_done
 
 lbl_win32_98:
     StrCpy $R0 '98'
   Goto lbl_done
 
 lbl_win32_ME:
     StrCpy $R0 'ME'
   Goto lbl_done
 
   lbl_winnt:
 
   StrCpy $R1 $R0 1
 
   StrCmp $R1 '3' lbl_winnt_x
   StrCmp $R1 '4' lbl_winnt_x
 
   StrCpy $R1 $R0 3
   StrCmp $R1 '5.0' lbl_winnt_2000
   StrCmp $R1 '5.1' lbl_winnt_XP
   StrCmp $R1 '5.2' lbl_winnt_2003
   StrCmp $R1 '6.0' lbl_winnt_Vista lbl_error
 
 lbl_winnt_x:
     StrCpy $R0 "NT $R0" 6
   Goto lbl_done
 
 lbl_winnt_2000:
     Strcpy $R0 '2000'
   Goto lbl_done
 
 lbl_winnt_XP:
     Strcpy $R0 'XP'
   Goto lbl_done
 
 lbl_winnt_2003:
     Strcpy $R0 '2003'
   Goto lbl_done
 
 lbl_winnt_Vista:
     Strcpy $R0 'Vista'
   Goto lbl_done
 
 lbl_error:
   Strcpy $R0 ''
 
 lbl_done:
   Pop $R1
   Exch $R0
 
 FunctionEnd


 Function ReleasePermission
   
   Pop $0
   Pop $1
   
   AccessControl::ReleasePermission "$0" "$1"

 FunctionEnd


 Function GetInternetExplorerVersion
 
   Push $R0
   Push $R1
 
   ReadRegStr $R0 HKLM "Software\Microsoft\Internet Explorer" "Version"

   IfErrors lbl_ErrorVersionIE 0
 
   StrCpy $R1 $R0 1
       
   StrCmp $R1 5 lbl_VersionIE_5 

   StrCmp $R1 6 lbl_VersionIE_6

   StrCmp $R1 7 lbl_VersionIE_7

   StrCmp $R1 8 lbl_VersionIE_8 lbl_ErrorVersionIE

lbl_VersionIE_5:
   StrCpy $R0 5
   Goto lbl_done

lbl_VersionIE_6:
   StrCpy $R0 6
   Goto lbl_done

lbl_VersionIE_7:
   StrCpy $R0 7
   Goto lbl_done

lbl_VersionIE_8:
   StrCpy $R0 8
   Goto lbl_done
   
lbl_ErrorVersionIE:
   StrCpy $R0 ''

lbl_done:
   Pop $R1
   Exch $R0
 
FunctionEnd


 
 ; GetWindowsVersion
 ;
 ; Based on Yazno's function, http://yazno.tripod.com/powerpimpit/
 ; Updated by Joost Verburg
 ;
 ; Returns on top of stack
 ;
 ; Windows Version (95, 98, ME, NT x.x, 2000, XP, 2003)
 ; or
 ; '' (Unknown Windows Version)
 ;
 ; Usage:
 ;   Call GetWindowsVersion
 ;   Pop $R0
 ;   ; at this point $R0 is "NT 4.0" or whatnot

 
 Function un.GetWindowsVersion
 
   Push $R0
   Push $R1
 
   ReadRegStr $R0 HKLM \
   "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion

   IfErrors 0 lbl_winnt
   
   ; we are not NT
   ReadRegStr $R0 HKLM \
   "SOFTWARE\Microsoft\Windows\CurrentVersion" VersionNumber
 
   StrCpy $R1 $R0 1
   StrCmp $R1 '4' 0 lbl_error
 
   StrCpy $R1 $R0 3
 
   StrCmp $R1 '4.0' lbl_win32_95
   StrCmp $R1 '4.9' lbl_win32_ME lbl_win32_98
 
 lbl_win32_95:
     StrCpy $R0 '95'
   Goto lbl_done
 
 lbl_win32_98:
     StrCpy $R0 '98'
   Goto lbl_done
 
 lbl_win32_ME:
     StrCpy $R0 'ME'
   Goto lbl_done
 
 lbl_winnt:
   StrCpy $R1 $R0 1
 
   StrCmp $R1 '3' lbl_winnt_x
   StrCmp $R1 '4' lbl_winnt_x
 
   StrCpy $R1 $R0 3
 
   StrCmp $R1 '5.0' lbl_winnt_2000
   StrCmp $R1 '5.1' lbl_winnt_XP
   StrCmp $R1 '5.2' lbl_winnt_2003 lbl_error
 
 lbl_winnt_x:
     StrCpy $R0 "NT $R0" 6
   Goto lbl_done
 
 lbl_winnt_2000:
     Strcpy $R0 '2000'
   Goto lbl_done
 
 lbl_winnt_XP:
     Strcpy $R0 'XP'
   Goto lbl_done
 
 lbl_winnt_2003:
     Strcpy $R0 '2003'
   Goto lbl_done
 
 lbl_error:
     Strcpy $R0 ''
 
 lbl_done:
   Pop $R1
   Exch $R0
 
 FunctionEnd


Name "${PRODUCT_NAME}"
OutFile "${OUTPUT_FILE}"

; Some default compiler settings (uncomment and change at will):

SetCompress auto ; (can be off or force)

; SetDatablockOptimize on ; (can be off)

CRCCheck on ; (can be off)

SetDateSave on ; (can be on to have files restored to their original date)
Icon "..\common\UOL.ico"
WindowIcon on

; ComponentText [text][subtext][subtext2] 

   LicenseText "É recomendável fechar todos os seus programas antes de prosseguir.$\n$\n \
      Leia com atenção o termo a seguir."
   LicenseData "licenca.txt"

   InstallDir "$PROGRAMFILES\UOL\UIM"
   InstallDirRegKey HKEY_LOCAL_MACHINE "SOFTWARE\UOL\UIM" ""
   SetOverwrite on
   ;FileErrorText "Erro ao gravar arquivo."
   AllowSkipFiles off

   DirText ""

   Section "Install" ; (default section)

   ClearErrors   
   
; add files / whatever that need to be installed here.

   CreateDirectory "$PROGRAMFILES\UOL"
   Push "1"
   Push "$PROGRAMFILES\UOL"
   Call ReleasePermission
   
   CreateDirectory $INSTDIR 
   Push "0"
   Push "$INSTDIR"
   Call ReleasePermission
   
   CreateDirectory $INSTDIR\Plugins
   Push "0"
   Push "$INSTDIR\Plugins"
   Call ReleasePermission   

   CreateDirectory $INSTDIR\ca-certs
   Push "0"
   Push "$INSTDIR\ca-certs"
   Call ReleasePermission   

   CreateDirectory "$TEMP\UIM_TMP"

   Call GetWindowsVersion
   Pop $R0

   StrCmp $R0 "XP" lbl_CtxWinXP

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\gdiplus.dll" 
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\gdiplus.dll" 
   Rename /REBOOTOK "$TEMP\UIM_TMP\gdiplus.dll" "$INSTDIR\gdiplus.dll"

lbl_CtxWinXP:

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\uim.exe"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\uim.exe"
   Rename /REBOOTOK "$TEMP\UIM_TMP\uim.exe" "$INSTDIR\uim.exe"

   SetOutPath "$INSTDIR"
   File  "${ICON_FILE}"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${ICON_FILE}"
   Rename /REBOOTOK "$TEMP\UIM_TMP\${ICON_FILE}" "$INSTDIR\${ICON_FILE}"
   
   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\charset.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\charset.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\charset.dll" "$INSTDIR\charset.dll.exe"
   
   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\purple.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\purple.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\purple.dll" "$INSTDIR\purple.dll"
   
   ;Delete old libgaim (gaim 1.5).
   SetOutPath "$INSTDIR"
   IfFileExists "$INSTDIR\gaim.dll" 0 lbl_libGaimAlreadyDeleted
     Delete "$INSTDIR\gaim.dll"
   
lbl_libGaimAlreadyDeleted:
   
   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\liboscar.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\liboscar.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\liboscar.dll" "$INSTDIR\liboscar.dll"
   
   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\iconv.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\iconv.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\iconv.dll" "$INSTDIR\iconv.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\IdleTracker.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\IdleTracker.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\IdleTracker.dll" "$INSTDIR\IdleTracker.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\intl.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\intl.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\intl.dll" "$INSTDIR\intl.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\libglib-2.0-0.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\libglib-2.0-0.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\libglib-2.0-0.dll" "$INSTDIR\libglib-2.0-0.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\libgmodule-2.0-0.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\libgmodule-2.0-0.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\libgmodule-2.0-0.dll" "$INSTDIR\libgmodule-2.0-0.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\libgobject-2.0-0.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\libgobject-2.0-0.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\libgobject-2.0-0.dll" "$INSTDIR\libgobject-2.0-0.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\libgthread-2.0-0.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\libgthread-2.0-0.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\libgthread-2.0-0.dll" "$INSTDIR\libgthread-2.0-0.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\libxml2.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\libxml2.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\libxml2.dll" "$INSTDIR\libxml2.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\freebl3.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\freebl3.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\freebl3.dll" "$INSTDIR\freebl3.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\libeay32.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\libeay32.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\freebl3.dll" "$INSTDIR\libeay32.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\nspr4.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\nspr4.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\nspr4.dll" "$INSTDIR\nspr4.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\nss3.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\nss3.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\nss3.dll" "$INSTDIR\nss3.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\nssckbi.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\nssckbi.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\nssckbi.dll" "$INSTDIR\nssckbi.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\plc4.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\plc4.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\plc4.dll" "$INSTDIR\plc4.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\plds4.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\plds4.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\plds4.dll" "$INSTDIR\plds4.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\softokn3.dll"  
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\softokn3.dll"  
   Rename /REBOOTOK "$TEMP\UIM_TMP\softokn3.dll" "$INSTDIR\softokn3.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\ssl3.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\ssl3.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\ssl3.dll" "$INSTDIR\ssl3.dll"

   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\zlib1.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\zlib1.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\zlib1.dll" "$INSTDIR\zlib1.dll"
   
   SetOutPath "$INSTDIR"
      File  "${DIR_BUILD}\smime3.dll"
      IfErrors 0 +4
      SetOutPath "$TEMP\UIM_TMP"
      File  "${DIR_BUILD}\smime3.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\smime3.dll" "$INSTDIR\smime3.dll"
   
   ;Install new MSVC libs.
   Call GetWindowsVersion
   Pop $R0
   
   StrCmp $R0 "2000" lbl_NotSupportedManifest
   
   SetOutPath "$INSTDIR\Microsoft.VC80.CRT"
   File  "..\msvc\Microsoft.VC80.CRT\Microsoft.VC80.CRT.manifest"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\msvc\Microsoft.VC80.CRT\Microsoft.VC80.CRT.manifest"
   Rename /REBOOTOK "$TEMP\UIM_TMP\Microsoft.VC80.CRT.manifest" "$INSTDIR\Microsoft.VC80.CRT\Microsoft.VC80.CRT.manifest"
   
   SetOutPath "$INSTDIR\Microsoft.VC80.CRT"
   File  "..\msvc\Microsoft.VC80.CRT\msvcm80.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\msvc\Microsoft.VC80.CRT\msvcm80.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\msvcm80.dll" "$INSTDIR\Microsoft.VC80.CRT\msvcm80.dll"
   
   SetOutPath "$INSTDIR\Microsoft.VC80.CRT"
   File  "..\msvc\Microsoft.VC80.CRT\msvcp80.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\msvc\Microsoft.VC80.CRT\msvcp80.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\msvcp80.dll" "$INSTDIR\Microsoft.VC80.CRT\msvcp80.dll"
   
   SetOutPath "$INSTDIR\Microsoft.VC80.CRT"
   File  "..\msvc\Microsoft.VC80.CRT\msvcr80.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\msvc\Microsoft.VC80.CRT\msvcr80.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\msvcr80.dll" "$INSTDIR\Microsoft.VC80.CRT\msvcr80.dll"
   
   Goto lbl_MSVC_Libs_Done
   
lbl_NotSupportedManifest:
   
   SetOutPath "$INSTDIR"
   File  "..\msvc\Microsoft.VC80.CRT\msvcp80.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\msvc\Microsoft.VC80.CRT\msvcp80.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\msvcp80.dll" "$INSTDIR\msvcp80.dll"
   
   SetOutPath "$INSTDIR"
   File  "..\msvc\Microsoft.VC80.CRT\msvcr80.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\msvc\Microsoft.VC80.CRT\msvcr80.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\msvcr80.dll" "$INSTDIR\msvcr80.dll"
   
   ; This file must be installed only on Windows 2000 too... 
   ; So let's do it on this block...
   ;
   SetOutPath "$INSTDIR"
   File  "..\dbghelp\dbghelp.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\dbghelp\dbghelp.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\dbghelp.dll" "$INSTDIR\dbghelp.dll"
   
lbl_MSVC_Libs_Done:

   ;Install ca-certs files
   SetOutPath "$INSTDIR\ca-certs"
   File  "..\common\ca-certs\CAcert_Class3.pem"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\common\ca-certs\CAcert_Class3.pem"
   Rename /REBOOTOK "$TEMP\UIM_TMP\CAcert_Class3.pem" "$INSTDIR\ca-certs\CAcert_Class3.pem"

   SetOutPath "$INSTDIR\ca-certs"
   File  "..\common\ca-certs\CAcert_Root.pem"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\common\ca-certs\CAcert_Root.pem"
   Rename /REBOOTOK "$TEMP\UIM_TMP\CAcert_Root.pem" "$INSTDIR\ca-certs\CAcert_Root.pem"

   SetOutPath "$INSTDIR\ca-certs"
   File  "..\common\ca-certs\Equifax_Secure_CA.pem"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\common\ca-certs\Equifax_Secure_CA.pem"
   Rename /REBOOTOK "$TEMP\UIM_TMP\Equifax_Secure_CA.pem" "$INSTDIR\ca-certs\Equifax_Secure_CA.pem"

   SetOutPath "$INSTDIR\ca-certs"
   File  "..\common\ca-certs\GTE_CyberTrust_Global_Root.pem"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\common\ca-certs\GTE_CyberTrust_Global_Root.pem"
   Rename /REBOOTOK "$TEMP\UIM_TMP\GTE_CyberTrust_Global_Root.pem" "$INSTDIR\ca-certs\GTE_CyberTrust_Global_Root.pem"

   SetOutPath "$INSTDIR\ca-certs"
   File  "..\common\ca-certs\Microsoft_Secure_Server_Authority.pem"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\common\ca-certs\Microsoft_Secure_Server_Authority.pem"
   Rename /REBOOTOK "$TEMP\UIM_TMP\Microsoft_Secure_Server_Authority.pem" "$INSTDIR\ca-certs\Microsoft_Secure_Server_Authority.pem"

   SetOutPath "$INSTDIR\ca-certs"
   File  "..\common\ca-certs\StartCom_Free_SSL_CA.pem"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\common\ca-certs\StartCom_Free_SSL_CA.pem"
   Rename /REBOOTOK "$TEMP\UIM_TMP\StartCom_Free_SSL_CA.pem" "$INSTDIR\ca-certs\StartCom_Free_SSL_CA.pem"

   SetOutPath "$INSTDIR\ca-certs"
   File  "..\common\ca-certs\Verisign_Class3_Primary_CA.pem"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\common\ca-certs\Verisign_Class3_Primary_CA.pem"
   Rename /REBOOTOK "$TEMP\UIM_TMP\Verisign_Class3_Primary_CA.pem" "$INSTDIR\ca-certs\Verisign_Class3_Primary_CA.pem"

   SetOutPath "$INSTDIR\ca-certs"
   File  "..\common\ca-certs\VeriSign_Class_3_Public_Primary_Certification_Authority_-_G5.pem"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\common\ca-certs\VeriSign_Class_3_Public_Primary_Certification_Authority_-_G5.pem"
   Rename /REBOOTOK "$TEMP\UIM_TMP\VeriSign_Class_3_Public_Primary_Certification_Authority_-_G5.pem" "$INSTDIR\ca-certs\VeriSign_Class_3_Public_Primary_Certification_Authority_-_G5.pem"

   SetOutPath "$INSTDIR\ca-certs"
   File  "..\common\ca-certs\Verisign_RSA_Secure_Server_CA.pem"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\common\ca-certs\Verisign_RSA_Secure_Server_CA.pem"
   Rename /REBOOTOK "$TEMP\UIM_TMP\Verisign_RSA_Secure_Server_CA.pem" "$INSTDIR\ca-certs\Verisign_RSA_Secure_Server_CA.pem"
   
   ;Install old MSVC libs whether we have removed it on prior install.
   SetOutPath "$INSTDIR"
   File  "..\msvc\msvcr71.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\msvc\msvcr71.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\msvcr71.dll" "$INSTDIR\msvcr71.dll"
   
   SetOutPath "$INSTDIR"
   File  "${DIR_BUILD}\vc2005\Settings.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\vc2005\Settings.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\Settings.dll" "$INSTDIR\Settings.dll"

   SetOutPath "$INSTDIR\Plugins"
   File  "${DIR_BUILD}\Plugins\libuol.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\Plugins\libuol.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\libuol.dll" "$INSTDIR\Plugins\libuol.dll"

;; <TODAOFERTA>
   SetOutPath "$INSTDIR\Plugins"
   File  "${DIR_BUILD}\Plugins\libuolto.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\Plugins\libuolto.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\libuolto.dll" "$INSTDIR\Plugins\libuolto.dll"
;; </TODAOFERTA>
   
   SetOutPath "$INSTDIR\Plugins"
   File  "${DIR_BUILD}\Plugins\libjabber.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\Plugins\libjabber.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\libjabber.dll" "$INSTDIR\Plugins\libjabber.dll"

   SetOutPath "$INSTDIR\Plugins"
   File  "${DIR_BUILD}\Plugins\libmsn.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\Plugins\libmsn.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\libmsn.dll" "$INSTDIR\Plugins\libmsnp9.dll"

   ;Delete old libmsnp9 (now it must call libmsn).
   SetOutPath "$INSTDIR\Plugins"
   IfFileExists "$INSTDIR\Plugins\libmsnp9.dll" 0 lbl_libMSNAlreadyDeleted
     Delete "$INSTDIR\Plugins\libmsnp9.dll"
   
lbl_libMSNAlreadyDeleted:
   
   SetOutPath "$INSTDIR\Plugins"
   File  "${DIR_BUILD}\Plugins\libicq.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\Plugins\libicq.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\libicq.dll" "$INSTDIR\Plugins\libicq.dll"
   
   ;Delete old liboscar (gaim 1.5).
   SetOutPath "$INSTDIR\Plugins"
   IfFileExists "$INSTDIR\Plugins\liboscar.dll" 0 lbl_libOscarAlreadyDeleted
     Delete "$INSTDIR\Plugins\liboscar.dll"
   
lbl_libOscarAlreadyDeleted:
   
   SetOutPath "$INSTDIR\Plugins"
   File  "${DIR_BUILD}\Plugins\libyahoo.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\Plugins\libyahoo.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\libyahoo.dll" "$INSTDIR\Plugins\libyahoo.dll"

   SetOutPath "$INSTDIR\Plugins"
   File  "${DIR_BUILD}\Plugins\ssl.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\Plugins\ssl.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\ssl.dll" "$INSTDIR\Plugins\ssl.dll"

   SetOutPath "$INSTDIR\Plugins"
   File  "${DIR_BUILD}\Plugins\ssl-nss.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\Plugins\ssl-nss.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\ssl-nss.dll" "$INSTDIR\Plugins\ssl-nss.dll"
   

   Call GetUOLFolder
   Pop $R0
   CreateDirectory $R0
   Push "1"
   Push "$R0"
   Call ReleasePermission
   
   
   StrCpy $R1 "$R0\UIM" 
   CreateDirectory $R1 
   Push "0"
   Push "$R1"
   Call ReleasePermission  
   
   ; Copy logger ini file
   SetOutPath $R1
   File  "..\common\UIMLog.ini"
   
   
   StrCpy $R1 "$R0\lib" 
   CreateDirectory $R1 
   Push "0"
   Push "$R1"
   Call ReleasePermission  

!ifndef NO_UOL_FONE_CLIENT_DLL
   SetOutPath $R1
   File "/oname=UOLFoneClient-${UOL_FONE_CLIENT_VERSION}.dll" "..\lib\UOLFoneClient.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File "/oname=UOLFoneClient-${UOL_FONE_CLIENT_VERSION}.dll" "..\lib\UOLFoneClient.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\UOLFoneClient-${UOL_FONE_CLIENT_VERSION}.dll" "$R1\UOLFoneClient-${UOL_FONE_CLIENT_VERSION}.dll"
!endif

!ifndef NO_UOL_FONE_SERVER_CONFIG_PROVIDER_DLL
   SetOutPath $R1
   File "/oname=UOLFoneServerConfigProvider-${UOL_FONE_SERVER_CONFIG_PROVIDER_VERSION}.dll" "..\lib\UOLFoneServerConfigProvider.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File "/oname=UOLFoneServerConfigProvider-${UOL_FONE_SERVER_CONFIG_PROVIDER_VERSION}.dll" "..\lib\UOLFoneServerConfigProvider.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\UOLFoneServerConfigProvider-${UOL_FONE_SERVER_CONFIG_PROVIDER_VERSION}.dll" "$R1\UOLFoneServerConfigProvider-${UOL_FONE_SERVER_CONFIG_PROVIDER_VERSION}.dll"
!else
!warning "Skipped UOLFoneServerConfigProvider.dll"
!endif

!ifndef NO_UOL_FONE_CRYPT_DLL
   SetOutPath $R1
   File "/oname=UOLFoneCrypt.dll" "..\lib\UOLFoneCrypt.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File "/oname=UOLFoneCrypt.dll" "..\lib\UOLFoneCrypt.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\UOLFoneCrypt.dll" "$R1\UOLFoneCrypt.dll"
!else
!warning "Skipped UOLFoneCrypt.dll"
!endif

!ifdef INCLUDE_G729_DLL
   SetOutPath "$R1\plugins"
   File "/oname=g729.dll" "..\lib\codecs\g729.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File "/oname=g729.dll" "..\lib\codecs\g729.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\g729.dll" "$R1\plugins\g729.dll"
!else
!warning "Skipped G729.dll"
!endif

   StrCpy $R1 "$R0\Controle" 
   CreateDirectory $R1
   Push "0"
   Push $R1
   Call ReleasePermission   

   SetOutPath $R1
   IfFileExists "$R1\UOLDesktopSetup.xml" +2
   File  "..\common\UOLDesktopSetup.xml"
   IfErrors lbl_FileError

   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\AppRegister.exe"
   IfErrors lbl_FileError lbl_Theme
   
lbl_FileError:
   MessageBox MB_OK|MB_ICONSTOP "Ocorreu um erro durante a instalação. Por favor, repita a operação. (UIM0131)"
   Quit

lbl_Theme:
   Call GetUOLMessengerFolder
   Pop $R0
   
   CreateDirectory $R0 
   Push "0"
   Push $R0
   Call ReleasePermission  
   
   StrCpy $R1 "$R0\Themes"
   CreateDirectory $R1  
   Push "0"
   Push $R1
   Call ReleasePermission  

   ; Remove all theme folders first.
   RMDir /r "$R1\Default"
   RMDir /r "$R0\Emoticons"
   RMDir /r "$R0\Sounds"
   
   SetOutPath "$TEMP\UIM_TMP"
   File  "Default.zip"

   ZipDLL::extractall "$TEMP\UIM_TMP\Default.zip" "$R1\Default"
   Pop $R1

   StrCmp $R1 "success" +3
   ZipDLL::extractall "$TEMP\UIM_TMP\Default.zip" "$TEMP\UIM_TMP\Default"
   Rename /REBOOTOK "$TEMP\UIM_TMP\Default" "$R0\Themes\Default"
   
   Delete /REBOOTOK "$TEMP\UIM_TMP\Default.zip"

   SetOutPath "$TEMP\UIM_TMP"
   File  "..\common\Emoticons.zip"

   ZipDLL::extractall "$TEMP\UIM_TMP\Emoticons.zip" "$R0"
   Pop $R1

   StrCmp $R1 "success" +3
   ZipDLL::extractall "$TEMP\UIM_TMP\Emoticons.zip" "$TEMP\UIM_TMP\Emoticons"
   Rename /REBOOTOK "$TEMP\UIM_TMP\Emoticons" "$R0"
   
   Delete /REBOOTOK "$TEMP\UIM_TMP\Emoticons.zip"

   SetOutPath "$TEMP\UIM_TMP"
   File  "..\common\Sounds.zip"

   ZipDLL::extractall "$TEMP\UIM_TMP\Sounds.zip" "$R0"
   Pop $R1

   StrCmp $R1 "success" +3
   ZipDLL::extractall "$TEMP\UIM_TMP\Sounds.zip" "$TEMP\UIM_TMP\Sounds"
   Rename /REBOOTOK "$TEMP\UIM_TMP\Sounds" "$R0"
   
   Delete /REBOOTOK "$TEMP\UIM_TMP\Sounds.zip"

   StrCpy $R1 "$R0\Plugins"
    
   CreateDirectory $R1 
   Push "0"
   Push $R1
   Call ReleasePermission  

   SetOutPath "$R1"
   File  "..\common\localuolmessengerplugins.xml"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\common\localuolmessengerplugins.xml"
   Rename /REBOOTOK "$TEMP\UIM_TMP\localuolmessengerplugins.xml" "$R0\Plugins\localuolmessengerplugins.xml"

   CreateDirectory "$R1\Fotoblog" 
   Push "0"
   Push "$R1\Fotoblog"
   Call ReleasePermission  

   SetOutPath "$R1\Fotoblog"
   File  "${DIR_BUILD}\plugins\photoblog_plugin\FotoBlogPlugin.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\plugins\photoblog_plugin\FotoBlogPlugin.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\FotoBlogPlugin.dll" "$R0\Plugins\Fotoblog\FotoBlogPlugin.dll"

   CreateDirectory "$R1\crypto" 
   Push "0"
   Push "$R1\crypto"
   Call ReleasePermission  

   SetOutPath "$R1\crypto"
   File  "${DIR_BUILD}\plugins\uolcrypto\uolcrypto.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\plugins\uolcrypto\uolcrypto.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\uolcrypto.dll" "$R0\Plugins\crypto\uolcrypto.dll"

!ifdef INSTALL_RADIO_UOL_PLUGIN
   CreateDirectory "$R1\radioUOL" 
   Push "0"
   Push "$R1\radioUOL"
   Call ReleasePermission  

   SetOutPath "$R1\radioUOL"
   File  "${DIR_BUILD}\plugins\radiouol\RadioUOL.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\plugins\radiouol\RadioUOL.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\RadioUOL.dll" "$R0\Plugins\radiouol\RadioUOL.dll"
!else
!warning "Skipped Radio UOL plugin"
!endif

   CreateDirectory "$R1\UOLFone" 
   Push "0"
   Push "$R1\UOLFone"
   Call ReleasePermission  

   SetOutPath "$R1\UOLFone"
   File  "${DIR_BUILD}\plugins\uolfone_plugin\UOLFonePlugin.dll"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "${DIR_BUILD}\plugins\uolfone_plugin\UOLFonePlugin.dll"
   Rename /REBOOTOK "$TEMP\UIM_TMP\UOLFonePlugin.dll" "$R0\Plugins\UOLFone\UOLFonePlugin.dll"

   StrCpy $R1 "$R0\Config"
    
   CreateDirectory $R1 
   Push "0"
   Push $R1
   Call ReleasePermission  

   SetOutPath "$R1"
   File  "..\common\filter.ext"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "..\common\filter.ext"
   Rename /REBOOTOK "$TEMP\UIM_TMP\filter.ext" "$R0\Config\filter.ext"

   File  "Common.xml"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "Common.xml"
   Rename /REBOOTOK "$TEMP\UIM_TMP\Common.xml" "$R0\Config\Common.xml"

   File  "res.xml"
   IfErrors 0 +4
   SetOutPath "$TEMP\UIM_TMP"
   File  "res.xml"
   Rename /REBOOTOK "$TEMP\UIM_TMP\res.xml" "$R0\Config\res.xml"

   SetShellVarContext all	
   CreateDirectory "$SMPROGRAMS\UOL"

   Delete /REBOOTOK "$SMPROGRAMS\UOL\${OLD_PRODUCT_LINK_1}.lnk"
   Delete /REBOOTOK "$SMPROGRAMS\UOL\${OLD_PRODUCT_LINK_2}.lnk"
   Delete /REBOOTOK "$SMPROGRAMS\UOL\${OPPOSITE_PRODUCT_LINK}.lnk"
   CreateShortCut "$SMPROGRAMS\UOL\${PRODUCT_LINK}.lnk" "$INSTDIR\uim.exe" \
  			      "" "$INSTDIR\${ICON_FILE}" 0 SW_SHOWNORMAL \
				  "" "${PRODUCT_NAME}"

   SetShellVarContext current
   Delete /REBOOTOK "$DESKTOP\${OLD_PRODUCT_LINK_1}.lnk"
   Delete /REBOOTOK "$DESKTOP\${OLD_PRODUCT_LINK_2}.lnk"
   Delete /REBOOTOK "$DESKTOP\${OPPOSITE_PRODUCT_LINK}.lnk"
   CreateShortCut "$DESKTOP\${PRODUCT_LINK}.lnk" \
                   "$INSTDIR\uim.exe" "" "$INSTDIR\${ICON_FILE}" 0
   
   Delete /REBOOTOK "$SMSTARTUP\${OLD_PRODUCT_LINK_1}.lnk"
   Delete /REBOOTOK "$SMSTARTUP\${OLD_PRODUCT_LINK_2}.lnk"
   Delete /REBOOTOK "$SMSTARTUP\${OPPOSITE_PRODUCT_LINK}.lnk"

   ;Don't create startup link if it is a reinstallation
   Call GetUOLMessengerDisplayName   
   Pop $R0
   StrCmp $R0 "${PRODUCT_NAME} (remover)" lbl_DontCreateStartupLink lbl_CreateStartupLink

lbl_CreateStartupLink:
   CreateShortCut "$SMSTARTUP\${PRODUCT_LINK}.lnk" \
                   "$INSTDIR\uim.exe" "" "$INSTDIR\${ICON_FILE}" 0
   Goto lbl_RefreshShellIcons

lbl_DontCreateStartupLink:
   Goto lbl_RefreshShellIcons

lbl_RefreshShellIcons:
   ${RefreshShellIcons}

   Call GetCtrlCenterFolder
   Pop $R0
   IfFileExists "$R0\ctrlcenter.dll" 0 new_control
   StrCpy $ALREADY_CONTROL 1

new_control:
   CreateDirectory "$R0"
   Push "0"
   Push "$R0"
   Call ReleasePermission  

   !insertmacro InstallLib REGDLL $ALREADY_CONTROL REBOOT_PROTECTED "${DIR_BUILD}\ctrlcenter.dll" "$R0\ctrlcenter.dll" "$R0"
   IfErrors 0 +2
   SetRebootFlag true
   
   IfFileExists "$SYSDIR\msxml3.dll" 0 new_msxml
   StrCpy $ALREADY_MSXML 1

new_msxml: 
   !insertmacro InstallLib REGDLL $ALREADY_MSXML REBOOT_NOTPROTECTED "..\msxml\msxml3.dll" "$SYSDIR\msxml3.dll" "$SYSDIR"
   !insertmacro InstallLib DLL $ALREADY_MSXML REBOOT_NOTPROTECTED "..\msxml\msxml3a.dll" "$SYSDIR\msxml3a.dll" "$SYSDIR"
   !insertmacro InstallLib DLL $ALREADY_MSXML REBOOT_NOTPROTECTED "..\msxml\msxml3r.dll" "$SYSDIR\msxml3r.dll" "$SYSDIR"
   IfErrors 0 +2
   SetRebootFlag true
   
; Grava numero de versão sequencial
     
   WriteRegDWORD HKEY_LOCAL_MACHINE "Software\UOL\UIM" "Version" "${INSTALL_SEQUENCE}"
   WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\UOL\UIM" "" "$INSTDIR"
   WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\UIM" "DisplayIcon" "$INSTDIR\${ICON_FILE},0"
   WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\UIM" "DisplayName" "${PRODUCT_NAME} (remover)"
   WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\UIM" "UninstallString" '"$INSTDIR\uninst.exe"'

   ifErrors 0 lbl_Registry_Ok
   
   MessageBox MB_OK|MB_ICONSTOP "Ocorreu um erro durante a instalação. Verifique se você tem privilégios de administrador do sistema. (UIM0119)"
   Quit

lbl_Registry_Ok:
   ExecWait '"$TEMP\UIM_TMP\AppRegister.exe" -ia uol_im' $0
   IfErrors 0 
   StrCmp $0 0 +6
   WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\RunOnce" "Register" '"$TEMP\UIM_TMP\AppRegister.exe$\" -ia uol_im' 
   SetRebootFlag true
   IfErrors 0 +3
   MessageBox MB_OK|MB_ICONSTOP "Ocorreu um erro durante a instalação. Por favor, repita a operação. (UIM0133)"
   Quit
   
; write out uninstaller
   WriteUninstaller "$INSTDIR\uninst.exe"

   IfRebootFlag 0 +3 
   MessageBox MB_YESNO|MB_ICONEXCLAMATION "Não foi possível finalizar a instalação. A operação será concluída ao reinicializar o Windows.$\nDeseja reinicializar agora? (UIM0120)" IDYES 0 IDNO +2
   Reboot 
   
   SectionEnd ; end of default section


; begin uninstall settings/section
UninstallText "Este programa vai remover o ${PRODUCT_NAME} do seu computador"

   UninstallIcon "..\common\UOL.ico"
   
   Section Uninstall

   SetRebootFlag false

   Call un.GetUOLMessengerFolder
   Pop $R0

; add delete commands to delete whatever files/registry keys/etc you installed here.

   Call un.GetWindowsVersion
   Pop $R3

   StrCmp $R3 "98" lbl_remCtxWin95
   StrCmp $R3 "ME" lbl_remCtxWin95

   SetShellVarContext all
   Goto lbl_remAfterContext

lbl_remCtxWin95:
   SetShellVarContext current

lbl_remAfterContext:
   CreateDirectory "$TEMP\UIM_TMP"
   SetOutPath $TEMP\UIM_TMP
   File "${DIR_BUILD}\AppRegister.exe"

   ExecWait '"$TEMP\UIM_TMP\AppRegister.exe" -ua uol_im' $0
   IfErrors 0 
   StrCmp $0 0 +3
   WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\RunOnce" "UnRegister" '"$TEMP\UIM_TMP\AppRegister.exe$\" -ua uol_im' 
   SetRebootFlag true

   Delete /REBOOTOK "$INSTDIR\uninst.exe"
   Delete /REBOOTOK "$SMPROGRAMS\UOL\${PRODUCT_LINK}.lnk"

   RMDir /r "$SMPROGRAMS\UOL\UIM" 
   RMDir /r "$SMPROGRAMS\UOL\UIM" 
   RMDir "$SMPROGRAMS\UOL"

   SetShellVarContext current
   Delete /REBOOTOK "$DESKTOP\${PRODUCT_LINK}.lnk"
   Delete /REBOOTOK "$SMSTARTUP\${PRODUCT_LINK}.lnk"

   RMDir /r /REBOOTOK "$INSTDIR"  

   DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\UOL\UIM"
   DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\UIM"

   IfRebootFlag  0 +3
   MessageBox MB_YESNO|MB_ICONEXCLAMATION "Para finalizar a remoção do ${PRODUCT_NAME} é preciso reinicializar.$\nDeseja reinicializar agora? (UIM0122)" IDYES 0 IDNO +2
   Reboot

   SectionEnd ; end of uninstall section

; eof
