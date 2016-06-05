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
#include "PreferencesPropertySheet.h"

#include "AccountsPropPage.h"
#include "ContactsPropPage.h"
#include "MessagesPropPage.h"
#include "StatusPropPage.h"
#include "SoundsPropPage.h"
#include "GeneralPropPage.h"
#include "EmoticonsPropPage.h"
#include "NetworkPropPage.h"
#include "PluginsPropPage.h"

#include "../../controller/UIMApplication.h"


CPreferencesPropertySheet::CPreferencesPropertySheet(LPCTSTR lpszTitle, int nActivePage, HWND hWndParent)	
{
	m_pView = new CTabbedWindow(lpszTitle, nActivePage, hWndParent);
}


CPreferencesPropertySheet::~CPreferencesPropertySheet()
{
}


HWND CPreferencesPropertySheet::CreatePreferecesWindow(HWND hWndParent)
{
	CRect rect(0, 0, 530, 570);

	return __super::CreateEx(hWndParent, rect);
}


void CPreferencesPropertySheet::UpdateLayout(BOOL bResizeBars)
{
	__super::UpdateLayout(bResizeBars);	
}


void CPreferencesPropertySheet::SetActivePage(IUOLMessengerUIManager::PreferencesTab tabId)
{
	CTabbedWindow* pTabbedWindow = dynamic_cast<CTabbedWindow*>(m_pView.get());

	if (pTabbedWindow)
	{
		pTabbedWindow->SetActivePageIndex(tabId);
	}
}


void CPreferencesPropertySheet::SetActivePage(const CString& strPageTitle)
{
	CTabbedWindow* pTabbedWindow = dynamic_cast<CTabbedWindow*>(m_pView.get());

	if (pTabbedWindow)
	{
		int nTabId = pTabbedWindow->GetPageIndex(strPageTitle);
		if ( TW_INDEX_NOT_FOUND  != nTabId )
		{
			pTabbedWindow->SetActivePageIndex(nTabId);
		}
	}
}


BOOL CPreferencesPropertySheet::HandlePropPageCommand(
		IUOLMessengerPropertyPageCommandPtr pPropPageCommand)
{
	CTabbedWindow* pTabbedWindow = dynamic_cast<CTabbedWindow*>(m_pView.get());
	
	if (pTabbedWindow)
	{
		int nTabId = pTabbedWindow->GetActivePageIndex();
		
		// We need to validate here the relationship between the pages 
		// and the commands they could receive.
		//
		if ( (nTabId == 0) && 
				(pPropPageCommand->GetPropPageCommandType() == 
						IUOLMessengerPropertyPageCommandFactory::PP_COMMAND_TYPE_NEW_ACCOUNT) )
		{
			return pTabbedWindow->HandlePropPageCommand(pPropPageCommand);
		}
	}
	
	return FALSE;
}


void CPreferencesPropertySheet::RemovePrefsPage(HWND hwndPrefsPage)
{
	CTabbedWindow* pTabbedWindow = dynamic_cast<CTabbedWindow*>(m_pView.get());
	
	if (pTabbedWindow)
	{
		pTabbedWindow->RemovePage(hwndPrefsPage);
	}
}


LRESULT CPreferencesPropertySheet::OnCreate(LPCREATESTRUCT lpCs)
{
	__super::OnCreate(lpCs);

	CTabbedWindow* pTabbedWindow = dynamic_cast<CTabbedWindow*>(m_pView.get());
	pTabbedWindow->SetParent(m_hWnd);
	pTabbedWindow->SetBackground(m_pBackground);

	CString strPreferencesTitle;
	CString strTitleDefault;
	CString strTitleFormat;

	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	strTitleFormat = pStringLoader->GetValue("uol.resource.IDS_TITLE_WINDOW_DEFAULT");
	strPreferencesTitle = pStringLoader->GetValue("uol.resource.IDS_PREFERENCES_PROPERTYSHEET_TITLE");
	strTitleDefault.Format(strTitleFormat, strPreferencesTitle);

	SetCaptionText(strTitleDefault);

	return 0L;
}


void CPreferencesPropertySheet::OnClose()
{
	CTabbedWindow* pTabbedWindow = dynamic_cast<CTabbedWindow*>(m_pView.get());
	
	pTabbedWindow->RemoveAll();

	IUOLMessengerAccountManagerPtr pAccountManager = CUIMApplication::GetApplication()->GetAccountManager();
	if (pAccountManager != NULL)
	{
		pAccountManager->SaveAccounts();
	}

	BOOL bRet = DestroyWindow();
	ATLASSERT(bRet);

	m_hWnd = NULL;
}


LRESULT CPreferencesPropertySheet::OnSysCommand(
		UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	// Nao sei pq eu precisei do "+ 2", mas por algum motivo eu recebo os 
	// valores das constantes acrescidos de 2.
	//
	if ( wParam == (SC_MAXIMIZE + 2) )
	{
		bHandled = TRUE;
	}
	else
	{
		bHandled = FALSE;
	}
	
	return 0L;
}


void CPreferencesPropertySheet::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
	__super::SetParameters(pMapParams, pElementTable);
	
	try
	{
		CString strBackground;
		GetParameterValue(strBackground, pMapParams, "background");

		m_pBackground = ap_dynamic_cast<CBackgroundElementPtr>(pElementTable->GetElement(strBackground));
		ATLASSERT(m_pBackground);
	}
	catch(const CSkinException&)
	{
	}
}


//CString CPreferencesPropertySheet::GetName() const
//{
//	return "__PreferencesWindowInstance";
//}