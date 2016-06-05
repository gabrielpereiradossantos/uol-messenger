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
#include "InfoWindowController.h"		  

#include "../../view/InfoFrameBuilder.h"
#include "../UIMApplication.h"
#include "../UIManager.h"


CInfoWindowController CInfoWindowController::ms_instance;



CInfoWindowController::CInfoWindowController()
{
}


CInfoWindowController::~CInfoWindowController()
{
}


CInfoWindowController* CInfoWindowController::GetInstance()
{
	return &ms_instance;
}


BOOL CInfoWindowController::Init()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	DWORD dwStyle = WS_POPUP;
	HWND hWnd =	__super::Create(::GetDesktopWindow(), NULL, "InfoWindowController", dwStyle);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "hWnd = [%x]", hWnd);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return (hWnd != NULL);
}


void CInfoWindowController::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"IsWindow() = [%d]", IsWindow());
	
	if (IsWindow())
	{
		DestroyAllInfoWindows();
		DestroyWindow();
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


CInfoFramePtr CInfoWindowController::CreateInfoWindow(IUOLMessengerContactPtr pContact)
{
	CInfoFramePtr pInfoWindow;

	if ((pContact != NULL) && IsWindow())
	{
		CUIManager* pUIManager = dynamic_cast<CUIManager*>(CUIMApplication::GetApplication()->GetUIManager().get());
		ATLASSERT(pUIManager != NULL);

		CInfoFrameBuilderPtr pInfoFrameBuilder;
		CInfoFramePtr pInfoWindow;

		if (pUIManager != NULL)
		{
			pInfoFrameBuilder = ap_dynamic_cast<CInfoFrameBuilderPtr>(const_cast<CUOLWindowManager*>(pUIManager->GetWindowManager())->GetElement("__InfoWindowBuilderInstance"));
			ATLASSERT(pInfoFrameBuilder.get());

			pInfoWindow = ap_dynamic_cast<CInfoFramePtr>(pInfoFrameBuilder->CreateElement());
			ATLASSERT(pInfoWindow.get());
		}
	
		if (pInfoWindow != NULL)
		{
			pInfoWindow->SetContact(pContact);

			INFOFRAMEPARAM ifp;
			ifp.pInfoFrame = pInfoWindow;

			SendMessage(WM_USER_CREATE_INFO_WINDOW, (WPARAM) &ifp, NULL);
			
			ATLASSERT(pInfoWindow->IsWindow());
			if (pInfoWindow->IsWindow())
			{
				pInfoWindow->ShowWindow(SW_NORMAL);
			}

			m_listInfoWindow.AddTail(pInfoWindow);
		}
	}

	return pInfoWindow;
}


BOOL CInfoWindowController::DestroyInfoWindow(IUOLMessengerContactPtr pContact)
{
	CInfoFramePtr pInfoWindow = FindInfoWindow(pContact);

	if (pInfoWindow != NULL)
	{
		return DestroyInfoWindow(pInfoWindow);
	}

	return FALSE;
}


BOOL CInfoWindowController::DestroyInfoWindow(CInfoFramePtr pInfoWindow)
{
	POSITION pos = m_listInfoWindow.Find(pInfoWindow);

	if (pos)
	{
		CInfoFramePtr pRemovedInfoWindow = m_listInfoWindow.GetAt(pos);

		ATLASSERT(pRemovedInfoWindow.get() == pInfoWindow.get());

		if (pRemovedInfoWindow->IsWindow())
		{
			pRemovedInfoWindow->SendMessage(WM_CLOSE);
			//pRemovedInfoWindow->DestroyWindow();
		}

		m_listInfoWindow.RemoveAt(pos);

		return TRUE;
	}

	return FALSE;
}


void CInfoWindowController::DestroyAllInfoWindows()
{
	while ((int) m_listInfoWindow.GetCount() > 0)
	{
		CInfoFramePtr pInfoWindow = m_listInfoWindow.RemoveHead();

		if (pInfoWindow->IsWindow())
		{
			pInfoWindow->DestroyWindow();
		}
	}
}


CInfoFramePtr CInfoWindowController::FindInfoWindow(IUOLMessengerContactPtr pContact)
{
	if (pContact != NULL)
	{
		CString strContact = pContact->GetName();
		IUOLMessengerAccountPtr pAccount = pContact->GetAccount();

		for (POSITION pos = m_listInfoWindow.GetHeadPosition(); pos; )
		{
			CInfoFramePtr pInfoWindow = m_listInfoWindow.GetNext(pos);

			IUOLMessengerContactPtr pTempContact = pInfoWindow->GetContact();

			if ((0 == strContact.Compare(pTempContact->GetName())) &&
				(0 == pTempContact->GetAccount()->GetUserId().Compare(pAccount->GetUserId())) &&
				(0 == pTempContact->GetAccount()->GetProtocolId().Compare(pAccount->GetProtocolId())))
			{
				return pInfoWindow;
			}
		}
	}

	return CInfoFramePtr();
}


LRESULT CInfoWindowController::OnCreateInfoWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPINFOFRAMEPARAM pIfp = (LPINFOFRAMEPARAM) wParam;

	pIfp->pInfoFrame->Create(::GetDesktopWindow(), CRect(CPoint(200, 200), CSize(400, 400)));

	return 0L;
}

