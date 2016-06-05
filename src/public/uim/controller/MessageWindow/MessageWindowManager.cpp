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
 
#include "stdafx.h"
#include "MessageWindowManager.h"

#include "../UIMApplication.h"
#include "../UIManager.h"
#include "../../view/MessageWindowBuilder.h"


void CMessageWindowObserverNotifier::NotifyAll(void (CUOLMessengerMessageWindowManagerObserver::*pfnUpdate)(IUOLMessengerMessageWindow*), IUOLMessengerMessageWindow* pMessageWindow)
{
	CAtlList<CUOLMessengerMessageWindowManagerObserver*> listObservers;

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);

		listObservers.AddTailList(&m_listObservers);
	}

	CUOLMessengerMessageWindowManagerObserver* pObserver;

	for (POSITION pos = listObservers.GetHeadPosition(); pos;)
	{
		pObserver = listObservers.GetNext(pos);
		(pObserver->*pfnUpdate)(pMessageWindow);
	}
}






typedef struct _MESSAGEWINDOWPARAM
{
	CMessageWindowFramePtr pMessageWindow;

	_MESSAGEWINDOWPARAM& operator=(const _MESSAGEWINDOWPARAM& mwp)
	{
		pMessageWindow = mwp.pMessageWindow;

		return (*this);
	}

} MESSAGEWINDOWPARAM, *LPMESSAGEWINDOWPARAM;



CMessageWindowManager::CMessageWindowManager()
{
}


CMessageWindowManager::~CMessageWindowManager()
{
}


BOOL CMessageWindowManager::Init()
{
	ATLASSERT(FALSE == IsWindow());

	HWND hWnd = Create(::GetDesktopWindow(), NULL, "MessageWindowManager", WS_POPUP);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "hWnd = [%x]", hWnd);

	return (hWnd != NULL);
}


void CMessageWindowManager::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"IsWindow() = [%d]", IsWindow());
	
	ATLASSERT(IsWindow());
	if (IsWindow())
	{
		while ((int) m_listMessageWindows.GetCount() > 0)
		{
			CMessageWindowFramePtr pMessageWindow = m_listMessageWindows.RemoveHead();

			if (pMessageWindow->IsWindow())
			{
				MESSAGEWINDOWPARAM mwp;
				mwp.pMessageWindow = pMessageWindow;

				SendMessage(WM_USER_DESTROY_MESSAGE_WINDOW, (WPARAM) &mwp, NULL);
			}
		}

		DestroyWindow();
	}
}


IUOLMessengerMessageWindowPtr CMessageWindowManager::CreateMessageWindow()
{
	CUIManager* pUIManager = dynamic_cast<CUIManager*>(CUIMApplication::GetApplication()->GetUIManager().get());
	ATLASSERT(pUIManager);

	CMessageWindowBuilderPtr pMessageWindowBuilder;
	CMessageWindowFramePtr pMessageWindow;

	if (pUIManager)
	{
		pMessageWindowBuilder = ap_dynamic_cast<CMessageWindowBuilderPtr>(const_cast<CUOLWindowManager*>(pUIManager->GetWindowManager())->GetElement("__MessageWindowBuilderInstance"));
		ATLASSERT(pMessageWindowBuilder.get());

		pMessageWindow = ap_dynamic_cast<CMessageWindowFramePtr>(pMessageWindowBuilder->CreateElement());		
		ATLASSERT(pMessageWindow.get());
	}	

	if (pMessageWindow)
	{
		pMessageWindow->RegisterFrameObserver(this);

		ATLASSERT(IsWindow());

		MESSAGEWINDOWPARAM mwp;
		mwp.pMessageWindow = pMessageWindow;

		SendMessage(WM_USER_CREATE_MESSAGE_WINDOW, (WPARAM) &mwp, NULL);

		if (IsWindow())
		{
			//pMessageWindow->CenterWindow(::GetDesktopWindow());

			m_listMessageWindows.AddTail(pMessageWindow);
		}
		else
		{
			pMessageWindow = CMessageWindowFramePtr();
		}
	}

	return pMessageWindow;
}


IUOLMessengerMessageWindowPtr CMessageWindowManager::FindConversationMessageWindow(IUOLMessengerContactPtr pContact, IUOLMessengerConversationPtr& pConversation)
{
	if (pContact != NULL)
	{
		return FindConversationMessageWindow(pContact->GetAccount(), pContact->GetName(), pConversation);
	}

	return IUOLMessengerMessageWindowPtr();
}


IUOLMessengerMessageWindowPtr CMessageWindowManager::FindConversationMessageWindow(IUOLMessengerAccountPtr pAccount, const CString& strContactName, IUOLMessengerConversationPtr& pConversation)
{
	for (POSITION pos = m_listMessageWindows.GetHeadPosition(); pos;)
	{
		CMessageWindowFramePtr pMessageWindow = m_listMessageWindows.GetNext(pos);

    	IUOLMessengerConversationPtr pTempConversation = pMessageWindow->FindConversation(pAccount, strContactName);

		if (pTempConversation != NULL)
		{
			pConversation = pTempConversation;

			return pMessageWindow;
		}
	}

	return IUOLMessengerMessageWindowPtr();
}


IUOLMessengerMessageWindowPtr CMessageWindowManager::FindConversationMessageWindow(const CString& strChatKey, IUOLMessengerConversationPtr& pConversation)
{
	for (POSITION pos = m_listMessageWindows.GetHeadPosition(); pos;)
	{
		CMessageWindowFramePtr pMessageWindow = m_listMessageWindows.GetNext(pos);

    	IUOLMessengerConversationPtr pTempConversation = pMessageWindow->FindConversation(strChatKey);

		if (pTempConversation != NULL)
		{
			pConversation = pTempConversation;

			return pMessageWindow;
		}
	}

	return IUOLMessengerMessageWindowPtr();
}


BOOL CMessageWindowManager::DestroyMessageWindow(IUOLMessengerMessageWindow* pMessageWindow)
{
	BOOL bDestroyed = FALSE;

	for (POSITION pos = m_listMessageWindows.GetHeadPosition(); pos; m_listMessageWindows.GetNext(pos))
	{
		CMessageWindowFramePtr pTempMessageWindow = m_listMessageWindows.GetAt(pos);

		if (pTempMessageWindow.get() == pMessageWindow)
		{
			m_listMessageWindows.RemoveAt(pos);

			bDestroyed = TRUE;
			break;
		}
	}

	return bDestroyed;
}


void CMessageWindowManager::GetMessageWindowList(CAtlList<IUOLMessengerMessageWindowPtr>& listMessageWindows)
{
	listMessageWindows.RemoveAll();

	for (POSITION pos = m_listMessageWindows.GetHeadPosition(); pos; )
	{
		CMessageWindowFramePtr pMessageWindow = m_listMessageWindows.GetNext(pos);

		listMessageWindows.AddTail(pMessageWindow);
	}
}


IUOLMessengerMessageWindowPtr CMessageWindowManager::GetFirstMessageWindow()
{
	if (GetMessageWindowCount() > 0)
	{
		IUOLMessengerMessageWindowPtr pMessageWindow = m_listMessageWindows.GetHead();

		return pMessageWindow;
	}

	return IUOLMessengerMessageWindowPtr();
}


IUOLMessengerMessageWindowPtr CMessageWindowManager::GetLastMessageWindow()
{
	if (GetMessageWindowCount() > 0)
	{
		IUOLMessengerMessageWindowPtr pMessageWindow = m_listMessageWindows.GetTail();

		return pMessageWindow;
	}

	return IUOLMessengerMessageWindowPtr();
}


int CMessageWindowManager::GetMessageWindowCount() const
{
	return (int) m_listMessageWindows.GetCount();
}


void CMessageWindowManager::RegisterObserver(CUOLMessengerMessageWindowManagerObserver* pObserver)
{
	m_observerNotifier.RegisterObserver(pObserver);
}


void CMessageWindowManager::UnregisterObserver(CUOLMessengerMessageWindowManagerObserver* pObserver)
{
	m_observerNotifier.UnregisterObserver(pObserver);
}


LRESULT CMessageWindowManager::OnCreateMessageWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	LPMESSAGEWINDOWPARAM pMwp = (LPMESSAGEWINDOWPARAM) wParam;

	if (pMwp->pMessageWindow)
	{
		CRect rectWindow(CPoint(200, 200), CSize(400, 380));

		if ( 0 < GetMessageWindowCount()) //Open in cascade
		{
			WINDOWPLACEMENT wp;
			CMessageWindowFramePtr mwfLast = ap_dynamic_cast<CMessageWindowFramePtr>(GetLastMessageWindow());
			if (NULL != mwfLast)
			{
				if (::GetWindowPlacement(mwfLast->m_hWnd, &wp))
				{
					rectWindow = CRect(CPoint(wp.rcNormalPosition.left+25,wp.rcNormalPosition.top+25),
						CSize(wp.rcNormalPosition.right - wp.rcNormalPosition.left,wp.rcNormalPosition.bottom - wp.rcNormalPosition.top) );

					HMONITOR  hMonitor = ::MonitorFromRect(rectWindow, MONITOR_DEFAULTTONEAREST);

					MONITORINFO mInfo = {0};
					mInfo.cbSize = sizeof(MONITORINFO);
					if (0 != GetMonitorInfo(hMonitor, &mInfo))
					{
						CRect rectMonitor(mInfo.rcWork); // Monitor Work Area

						if (rectMonitor.left > rectWindow.left || rectMonitor.right < rectWindow.right || 
							rectMonitor.top > rectWindow.top || rectMonitor.bottom < rectWindow.bottom)
						{
							rectWindow = CRect(CPoint(0, 0), rectWindow.Size());
						}
					}
				}
			}
		}
		else
		{
			IUOLMessengerMessageSettings2Ptr pMs = ap_dynamic_cast<IUOLMessengerMessageSettings2Ptr>(CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings());
			if( NULL != pMs)
			{
				int nCordX = pMs->GetCordX();
				int nCordY = pMs->GetCordY();
				int nSizeX = pMs->GetSizeX();
				int nSizeY = pMs->GetSizeY();
				
				rectWindow = CRect(CPoint(nCordX, nCordY), CSize(nSizeX, nSizeY));
			}

			HMONITOR  hMonitor = ::MonitorFromRect(rectWindow, MONITOR_DEFAULTTONEAREST);

			MONITORINFO mInfo = {0};
			mInfo.cbSize = sizeof(MONITORINFO);
			if (0 != GetMonitorInfo(hMonitor, &mInfo))
			{
				CRect rectMonitor(mInfo.rcWork); // Monitor Work Area

				// Test whether the entire window is on desktop and reposition if not
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
		}

		(pMwp->pMessageWindow)->Create(::GetDesktopWindow(), rectWindow);
	}
	
	return 0L;
}


LRESULT CMessageWindowManager::OnDestroyMessageWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	LPMESSAGEWINDOWPARAM pMwp = (LPMESSAGEWINDOWPARAM) wParam;

	if ((pMwp->pMessageWindow) &&
		((pMwp->pMessageWindow)->IsWindow()))
	{
		(pMwp->pMessageWindow)->DestroyWindow();
	}

	return 0L;
}


void CMessageWindowManager::OnCreateWindow(CMessageWindowFrame* pMessageWindowFrame)
{
	m_observerNotifier.NotifyAll(&CUOLMessengerMessageWindowManagerObserver::OnCreateMessageWindow, pMessageWindowFrame);
}


void CMessageWindowManager::OnDestroyWindow(CMessageWindowFrame* pMessageWindowFrame)
{
	if(1 == GetMessageWindowCount())//last window
	{
		WINDOWPLACEMENT wp;
		if (::GetWindowPlacement(pMessageWindowFrame->m_hWnd, &wp))
		{
			IUOLMessengerMessageSettings2Ptr pMs = ap_dynamic_cast<IUOLMessengerMessageSettings2Ptr>(CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings());
			if( NULL != pMs)
			{
				pMs->SetCordX(wp.rcNormalPosition.left);
				pMs->SetCordY(wp.rcNormalPosition.top);
				pMs->SetSizeX(wp.rcNormalPosition.right - wp.rcNormalPosition.left);
				pMs->SetSizeY(wp.rcNormalPosition.bottom - wp.rcNormalPosition.top);
			}
			CUIMApplication::GetApplication()->GetSettingsManager()->SaveSettings();
		}
	}

	pMessageWindowFrame->UnregisterFrameObserver(this);

	m_observerNotifier.NotifyAll(&CUOLMessengerMessageWindowManagerObserver::OnDestroyMessageWindow, pMessageWindowFrame);
}
