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
#include "MessageReceivingDialogController.h"
#include "../UIMApplication.h"
#include "../UIManager.h"
#include "../../view/skin/ElementBuilder.h"
#include "../../view/Skin/MainDialogElement.h"


typedef struct _NOTIFYDIALOGPARAM
{
	HWND hWndParent;
	CMessageReceivingDialogPtr pDialog;

	_NOTIFYDIALOGPARAM& operator=(const _NOTIFYDIALOGPARAM& ndp)
	{
		pDialog = ndp.pDialog;
		hWndParent = ndp.hWndParent;

		return (*this);
	}

} NOTIFYDIALOGPARAM, *LPNOTIFYDIALOGPARAM;



CMessageReceivingDialogController::CMessageReceivingDialogController()
{
}


CMessageReceivingDialogController::~CMessageReceivingDialogController()
{
}


BOOL CMessageReceivingDialogController::Init()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	ATLASSERT(FALSE == IsWindow());

	HWND hWnd = Create(NULL, NULL, "MessageReceivingDialogController", WS_POPUP);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "hWnd = [%x]", hWnd);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return (hWnd != NULL);
}


void CMessageReceivingDialogController::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"IsWindow() = [%d]", IsWindow());
	
	ATLASSERT(IsWindow());
	if (IsWindow())
	{
		DestroyAll();
		DestroyWindow();
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


CMessageReceivingDialogPtr CMessageReceivingDialogController::CreateNotifyDialog(IUOLMessengerContactPtr pContact, HWND hWndParent)
{
	CMessageReceivingDialogPtr pDialog;

	if (pContact && IsWindow())
	{
		CMessageReceivingDialogPtr pNewDialog;

		CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
		IElementPtr pElement = pUIManagerPtr->GetWindowManager()->GetElement("__MessageReceivingDialogBuilderInstance");			
		if (pElement)
		{
			CElementBuilderPtr pBuilder;				
			pBuilder = ap_dynamic_cast<CMainDialogBuilderPtr>(pElement);				
			if (pBuilder)
			{
				pNewDialog = ap_dynamic_cast<CMessageReceivingDialogPtr>(pBuilder->CreateElement());			
				ATLASSERT(pNewDialog.get());
			}
		}

		pNewDialog->Initialize(pContact);

		NOTIFYDIALOGPARAM ndp;
		ndp.hWndParent = hWndParent;
		ndp.pDialog = pNewDialog;
		
		SendMessage(WM_USER_CREATE_NOTIFY_DIALOG, (WPARAM) &ndp, NULL);
		
		if (pNewDialog->IsWindow())
		{
			pDialog = pNewDialog;
			m_listDialogs.AddTail(pDialog);
		}
	}

	return pDialog;
}


CMessageReceivingDialogPtr CMessageReceivingDialogController::FindNotifyDialog(IUOLMessengerContactPtr pContact)
{
	return FindDialogImpl(pContact);
}


void CMessageReceivingDialogController::DestroyNotifyDialog(IUOLMessengerContactPtr pContact)
{
	CMessageReceivingDialogPtr pDialog = FindDialogImpl(pContact);

	if ((pDialog) && (pDialog->IsWindow()))
	{
		DestroyDialogImpl(pDialog);
	}
}


void CMessageReceivingDialogController::DestroyAll()
{
	while ((int) m_listDialogs.GetCount() > 0)
	{
		CMessageReceivingDialogPtr pDialog = m_listDialogs.RemoveHead();

		if (pDialog->IsWindow())
		{
			DestroyDialogImpl(pDialog);
		}
	}
}


void CMessageReceivingDialogController::RemoveNotifyDialog(CMessageReceivingDialog* pDialog)
{
	ATLASSERT(pDialog != NULL);

	IUOLMessengerContactPtr pContact = pDialog->GetContact();

	CString strName = pContact->GetName();
	CString strAccountUserId = pContact->GetAccount()->GetUserId();
	CString strAccountProtocolId = pContact->GetAccount()->GetProtocolId();

	for (POSITION pos = m_listDialogs.GetHeadPosition(); pos; m_listDialogs.GetNext(pos))
	{
		CMessageReceivingDialogPtr pDialog = m_listDialogs.GetAt(pos);

		if (Match(strName, strAccountUserId, strAccountProtocolId, pDialog.get()))
		{
			m_listDialogs.RemoveAt(pos);
			break;
		}
	}
}


LRESULT CMessageReceivingDialogController::OnCreateNotifyDialog(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	LPNOTIFYDIALOGPARAM pNdp = (LPNOTIFYDIALOGPARAM) wParam;

	if (pNdp->pDialog)
	{
		(pNdp->pDialog)->Create(pNdp->hWndParent);
		
		if ((pNdp->pDialog)->IsWindow())
		{
			(pNdp->pDialog)->ShowWindow(SW_SHOWNORMAL);
		}
	}

	return 0L;
}


LRESULT CMessageReceivingDialogController::OnDestroyNotifyDialog(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	LPNOTIFYDIALOGPARAM pNdp = (LPNOTIFYDIALOGPARAM) wParam;

	if (pNdp->pDialog)
	{
		(pNdp->pDialog)->DestroyWindow();
	}

	return 0L;
}


CMessageReceivingDialogPtr CMessageReceivingDialogController::FindDialogImpl(IUOLMessengerContactPtr pContact)
{
	CMessageReceivingDialogPtr pFindDialog;

	if (pContact)
	{
		CString strName = pContact->GetName();
		CString strAccountUserId = pContact->GetAccount()->GetUserId();
		CString strAccountProtocolId = pContact->GetAccount()->GetProtocolId();

		for (POSITION pos = m_listDialogs.GetHeadPosition(); pos; )
		{
			CMessageReceivingDialogPtr pDialog = m_listDialogs.GetNext(pos);

			if (Match(strName, strAccountUserId, strAccountProtocolId, pDialog.get()))
			{
				pFindDialog = pDialog;
				break;
			}
		}
	}

	return pFindDialog;
}


void CMessageReceivingDialogController::DestroyDialogImpl(CMessageReceivingDialogPtr pDialog)
{
	NOTIFYDIALOGPARAM ndp;
	ndp.hWndParent = NULL;
	ndp.pDialog = pDialog;

	SendMessage(WM_USER_DESTROY_NOTIFY_DIALOG, (WPARAM) &ndp, NULL);

	ATLASSERT(FALSE == pDialog->IsWindow());
}


BOOL CMessageReceivingDialogController::Match(const CString& strContactName, const CString& strAccountUserId, const CString& strAccountProtocolId,  CMessageReceivingDialog* pDialog)
{
	IUOLMessengerContactPtr pTempContact = pDialog->GetContact();

	return ((0 == strContactName.Compare(pTempContact->GetName())) &&
			(0 == strAccountUserId.Compare(pTempContact->GetAccount()->GetUserId())) &&
			(0 == strAccountProtocolId.Compare(pTempContact->GetAccount()->GetProtocolId())));
}