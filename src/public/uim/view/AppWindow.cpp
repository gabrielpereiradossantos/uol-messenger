/* UOL Messenger
* Copyright (c) 2005 Universo Online S/A
*
* Direitos Autorais Reservados
* All rights reserved
*
* Este programa é software livre; você pode redistribuí-lo e/ou modificá-lo
* sob os termos da Licença Pública Geral GNU conforme publicada pela Free
* Software Foundation; tanto a versão 2 da Licença, como (a seu critério)
* qualquer versão posterior.
* Este programa é distribuído na expectativa de que seja útil, porém,
* SEM NENHUMA GARANTIA; nem mesmo a garantia implícita de COMERCIABILIDADE
* OU ADEQUAÇÃO A UMA FINALIDADE ESPECÍFICA. Consulte a Licença Pública Geral
* do GNU para mais detalhes. 
* Você deve ter recebido uma cópia da Licença Pública Geral do GNU junto
* com este programa; se não, escreva para a Free Software Foundation, Inc.,
* no endereço 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA. 
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
* Universo Online S/A - A/C: UOL Messenger 5o. Andar
* Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
* São Paulo SP - CEP 01452-002 - BRASIL  */

#include "StdAfx.h"
#include ".\appwindow.h"

#include <commands/SetUserStatusAllCommand.h>
#include <commands/showprivacydialogcommand.h>

#include "UimView.h"
#include <commands/hidewindowcommand.h>
#include <commands/closeappcommand.h>
#include "../controller/UIMApplication.h"
#include "../controller/UIManager.h"
#include "AlertDialog.h"
#include "Skin/ElementBuilder.h"
#include "Skin/MainDialogElement.h"

class CHideAppWindowCommand : public CHideWindowCommand
{
public:
	CHideAppWindowCommand(CWindow wnd);

protected:
	virtual void Execute();
};

CAppWindow::CAppWindow(void) :
	m_bGotAway(FALSE),
	m_nIdleReportTimer(0)
{
	m_pView = new CUimView();		
}


CAppWindow::~CAppWindow(void)
{
}


HWND CAppWindow::CreateAppWindow()
{
	int nCordX = CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->GetCordX();
	int nCordY = CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->GetCordY();
	int nSizeX = CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->GetSizeX();
	int nSizeY = CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->GetSizeY();

	CRect rectWindow = CRect(CPoint(nCordX, nCordY), CSize(nSizeX, nSizeY));
	
	HMONITOR  hMonitor = ::MonitorFromRect(rectWindow, MONITOR_DEFAULTTONEAREST);
	
	MONITORINFO mInfo = {0};
	mInfo.cbSize = sizeof(MONITORINFO);
	if (0 != GetMonitorInfo(hMonitor, &mInfo))
	{
		// Test whether the entire window is on desktop and reposition if not
		CRect rectMonitor(mInfo.rcWork); // Monitor Work Area

		if (rectWindow.Width() > rectMonitor.Width() )
		{
			rectWindow.left = rectMonitor.left;
			rectWindow.right = rectMonitor.right;
		}
		else
		{
			if (rectMonitor.left > rectWindow.left )
			{
				rectWindow.MoveToX(rectMonitor.left);
			}
			else if (rectMonitor.right < rectWindow.right )
			{
				rectWindow.MoveToX(rectMonitor.right - rectWindow.Width() );
			}
		}

		if (rectWindow.Height() > rectMonitor.Height() )
		{
			rectWindow.top = rectMonitor.top;
			rectWindow.bottom = rectMonitor.bottom;
		}
		else
		{
			if (rectMonitor.top > rectWindow.top )
			{
				rectWindow.MoveToY(rectMonitor.top);
			}
			else if (rectMonitor.bottom < rectWindow.bottom )
			{
				rectWindow.MoveToY(rectMonitor.bottom - rectWindow.Height() );
			}
		}
	}

	return __super::CreateEx(NULL, rectWindow);
}


CTabViewPtr CAppWindow::GetAppTabViewCtrl()
{
	return ap_static_cast<CUimViewPtr>(m_pView)->GetAppTabViewCtrl();
}


CContactListTabPtr CAppWindow::GetContactListTab()
{
	return ap_static_cast<CUimViewPtr>(m_pView)->GetContactListTab();
}


CMainFrameFocusNotifierPtr CAppWindow::GetMainFrameFocusNotifier()
{
	return ap_static_cast<CUimViewPtr>(m_pView)->GetMainFrameFocusNotifier();
}


CMainWindowEventsNotifierPtr CAppWindow::GetMainWindowEventsNotifier()
{
	return ap_static_cast<CUimViewPtr>(m_pView)->GetMainWindowEventsNotifier();
}


void CAppWindow::UpdateAccountInfoPanel(IUOLMessengerAccountPtr pAccount)
{
	ap_static_cast<CUimViewPtr>(m_pView)->UpdateAccountInfoPanel(pAccount);
}


CContextMenuPtr CAppWindow::GetSystrayContextMenu()
{
	return ap_static_cast<CUimViewPtr>(m_pView)->GetSystrayContextMenu();
}


LRESULT CAppWindow::OnCreate(LPCREATESTRUCT lpCreatestruct)
{
	__super::OnCreate(lpCreatestruct);

	////m_haccelerator = AtlLoadAccelerators(IDR_MAINFRAME);
	ATL::_U_STRINGorID table = IDR_MAINFRAME;
	m_haccelerator = ::LoadAccelerators(ATL::_AtlBaseModule.GetResourceInstance(), table.m_lpstr);

	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();

	CString strCaption;
	strCaption = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");

	SetCaptionText(strCaption);

	SetTimer(AUTO_AWAY_CHECK_TIMER, 5000);

	return 0L;

}

LRESULT CAppWindow::OnClose()
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	IUOLMessengerCommandPtr pCommand = CAppWindowCommandFactory::GetCommand("HideWindow",this);

	if (pCommand)
	{
		CUIMApplication::GetApplication()->ExecuteCommand(pCommand);		
	}

	return 0L;
}

void CAppWindow::OnDestroy()
{
	WINDOWPLACEMENT wp;

	if (m_nIdleReportTimer != 0)
	{
		KillTimer(IDLE_REPORT_TIMER);
		m_nIdleReportTimer = 0;
	}

	KillTimer(AUTO_AWAY_CHECK_TIMER);

	if (GetWindowPlacement(&wp))
	{
		CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->SetCordX(wp.rcNormalPosition.left);
		CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->SetCordY(wp.rcNormalPosition.top);
		CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->SetSizeY(wp.rcNormalPosition.bottom - wp.rcNormalPosition.top);
		CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->SetSizeX(wp.rcNormalPosition.right - wp.rcNormalPosition.left);

		CUIMApplication::GetApplication()->GetSettingsManager()->SaveSettings();
	}

	::PostQuitMessage(0);

	SetMsgHandled(FALSE);
}


void CAppWindow::OnTimer(UINT nTimerId, TIMERPROC tp)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (nTimerId == AUTO_AWAY_CHECK_TIMER)
	{
		// Check elapsed seconds timeout 
		UINT nSetAwayInterval = CUIMApplication::GetApplication()->GetSettingsManager()->GetStatusSettings()->GetTimeout() * 60;
		UINT nElapsedTimeout = (::GetTickCount() - GetLastActivityTickCount()) / 1000;

		if (0 == m_nIdleReportTimer)
		{
			if (nElapsedTimeout >= IDLE_REPORT_INTERVAL)
			{
				m_nIdleReportTimer = SetTimer(IDLE_REPORT_TIMER, IDLE_REPORT_INTERVAL * 1000);

				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogInfo(__FUNCTION__, "SetTimer(IDLE_REPORT_TIMER)");
			}
		}
		else
		{
			if (nElapsedTimeout < IDLE_REPORT_INTERVAL)
			{
				KillTimer(IDLE_REPORT_TIMER);
				m_nIdleReportTimer = 0;

				CUIMApplication::GetApplication()->GetAccountConnectionManager()->SetIdleAll(0);

				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogInfo(__FUNCTION__, "KillTimer(IDLE_REPORT_TIMER); SetIdleAll(0)");
			}
		}

		if (nElapsedTimeout > nSetAwayInterval)
		{
			if (!m_bGotAway)
			{
				if ((CUIMApplication::GetApplication()->GetSettingsManager()->GetStatusSettings()->IsAutoAwayEnabled()) &&
					(CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectedAccountsCount() > 0))
				{
					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogInfo(__FUNCTION__, "CSetUserStatusAllCommand(AS_AUTO_AWAY)");
					
					ATLTRACE(_T("Away timeout\n"));
					CUIMApplication::GetApplication()->ExecuteCommand(new CSetUserStatusAllCommand(IUOLMessengerAccountConnectionManager::AS_AUTO_AWAY));
				}

				m_bGotAway = TRUE;

				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogInfo(__FUNCTION__, "m_bGotAway = TRUE.");
			}
		}
		else
		{
			if (m_bGotAway)
			{
				if ((CUIMApplication::GetApplication()->GetSettingsManager()->GetStatusSettings()->IsAutoAwayEnabled()) &&
					(CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectedAccountsCount() > 0))
				{
					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogInfo(__FUNCTION__, "CSetUserStatusAllCommand(AS_AUTO_AWAY_RETURN)");
					
					ATLTRACE(_T("Away return\n"));
					CUIMApplication::GetApplication()->ExecuteCommand(new CSetUserStatusAllCommand(IUOLMessengerAccountConnectionManager::AS_AUTO_AWAY_RETURN));					
				}

				m_bGotAway = FALSE;

				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogInfo(__FUNCTION__, "m_bGotAway = FALSE.");
			}
		}
	}
	else if (nTimerId == IDLE_REPORT_TIMER)
	{
		UINT nElapsedTimeout = (::GetTickCount() - GetLastActivityTickCount()) / 1000;

		CUIMApplication::GetApplication()->GetAccountConnectionManager()->SetIdleAll(nElapsedTimeout);

		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, "SetIdleAll(%u)", nElapsedTimeout);
	}
	else
	{
		SetMsgHandled(FALSE);
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

void CAppWindow::OnEndSession(BOOL bEndSession, UINT /*uReason*/)
{
	if (bEndSession)
	{
		CUIMApplication::GetApplication()->ExecuteCommand(new CCloseAppCommand());		
	}
}

CString CAppWindow::GetName() const
{
	return "__AppWindowInstance";
}

IUOLMessengerCommandPtr CAppWindow::GetCommand(const CString& strCommand)
{
	if (strCommand.CompareNoCase("HideWindow") == 0)
	{
		CContactListTabPtr pContactListTab = GetContactListTab();
		if (pContactListTab.get())
		{
			CFindContactPanelPtr pFindContactPanel = pContactListTab->GetFindContactPanel();
			if (pFindContactPanel.get())
			{
				pFindContactPanel->ClearText();
			}
		}
	}

	IUOLMessengerCommandPtr pCommand = CAppWindowCommandFactory::GetCommand(strCommand, this);

	if (pCommand.get())
	{
		return pCommand;
	}

	return __super::GetCommand(strCommand);
}

IUOLMessengerCommandPtr CAppWindowCommandFactory::GetCommand(const CString& strCommand, CWindow* pWindow)
{
	ATLASSERT(pWindow); 

	if (strCommand.CompareNoCase("HideWindow") == 0)
	{
		return new CHideAppWindowCommand(*pWindow);
	}

	return IUOLMessengerCommandPtr();	
}


CHideAppWindowCommand::CHideAppWindowCommand(CWindow wnd) : 
CHideWindowCommand(wnd) { }

void CHideAppWindowCommand::Execute()
{
	__super::Execute();

	if (FALSE == CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->GetDontShowHideAppWarning())	
	{
		CAlertDialogPtr pDialog;

		CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
		IElementPtr pElement = pUIManagerPtr->GetWindowManager()->GetElement("__AlertDialogBuilderInstance");			
		if (pElement)
		{
			CElementBuilderPtr pBuilder;				
			pBuilder = ap_dynamic_cast<CMainDialogBuilderPtr>(pElement);				
			if (pBuilder)
			{
				pDialog = ap_dynamic_cast<CAlertDialogPtr>(pBuilder->CreateElement());			
				ATLASSERT(pDialog.get());
			}
		}

		if (IDOK == pDialog->DoModal())
		{
			if (pDialog->m_bCheck)
			{
				CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->SetDontShowHideAppWarning();
			}
		}		
	}	
}


//BOOL CAppWindow::PreTranslateMessage(MSG* pMsg)
//{
//	// << debug code
//	//return FALSE;
//	// >> debug code
//	// Pass keyboard messages along to the child window that has the focus.
//	// When the browser has the focus, the message is passed to its containing
//	// CAxWindow, which lets the control handle the message.
//	if ((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
//		(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
//	{
//		return FALSE;
//	}
//
//	HWND hWndCtl = ::GetFocus();
//
//	if (IsChild(hWndCtl))
//	{
////		if(::TranslateAccelerator(m_hWnd, m_haccelerator, pMsg))
////			return TRUE;
//
//		// Find a direct child of the dialog from the window that has focus
//		while (::GetParent(hWndCtl) != m_hWnd)
//		{
//			hWndCtl = ::GetParent(hWndCtl);
//		}
//
//		// Give control a chance to translate this message
//		if (::SendMessage(hWndCtl, WM_FORWARDMSG, 0, (LPARAM)pMsg) != 0)
//		{
//			return TRUE;
//		}
//	}
//
//	// A normal control has the focus, so call IsDialogMessage() so that
//	// the dialog shortcut keys work (TAB, etc.)
//	//return IsDialogMessage(pMsg);
//	return FALSE;
//}

//LRESULT CAppWindow::OnAccelaratorPreference(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		pUOLMessengerFacade->ExecuteCommand(new CShowPreferencesCommand());
//	}
//	return S_OK;
//}
//
//LRESULT CAppWindow::OnAccelaratorAddContact(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
//		IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("ShowAddContactDialog"));
//		if (pCommand)
//		{
//			pUOLMessengerFacade->ExecuteCommand(pCommand);
//		}
//	}
//	return S_OK;
//}
//
//LRESULT CAppWindow::OnAccelaratorPrivacy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		pUOLMessengerFacade->ExecuteCommand(new CShowPrivacyDialogCommand(pUOLMessengerFacade->GetUIManager()->GetAppWindow()));
//	}
//	return S_OK;
//}

//LRESULT CAppWindow::OnAccelaratorHelp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
//		IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("ShowHelp"));
//		if (pCommand)
//		{
//			pUOLMessengerFacade->ExecuteCommand(pCommand);
//		}
//	}
//	return S_OK;
//}

//LRESULT CAppWindow::OnAccelaratorOnline(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
//		IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("SetOnlineStatusAll"));
//		if (pCommand)
//		{
//			pUOLMessengerFacade->ExecuteCommand(pCommand);
//		}
//	}
//
//	return S_OK;
//}
//
//LRESULT CAppWindow::OnAccelaratorOffline(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	ATLTRACE("%s",__FUNCTION__);
//	IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();
//
//	if (pUOLMessengerFacade)
//	{
//		IUOLMessengerCommandFactoryPtr pFactory = pUOLMessengerFacade->GetCommandFactory();
//		IUOLMessengerCommandPtr pCommand = pFactory->GetCommand(_T("DisconnectAllAccounts"));
//		if (pCommand)
//		{
//			pUOLMessengerFacade->ExecuteCommand(pCommand);
//		}
//	}
//
//	return S_OK;
//}
