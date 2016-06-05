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
#include "ContextMenu.h"

#include "../controller/UIMApplication.h"
#include "../controller/MethodAuthorizationManager.h"


CContextMenu::CContextMenu(const CString& strLabel) : 
	CContextMenuItem(0, strLabel, NULL)
{
}


CContextMenu::~CContextMenu()
{
	//ATLTRACE(_T("~CContextMenu(), %s\n"), GetLabel());
	DestroyMenu();
}


int CContextMenu::GetCommand()
{
	ATLASSERT(m_wndMenu.m_hMenu != NULL);
	return (int) m_wndMenu.m_hMenu;
}


BOOL CContextMenu::IsMenu()
{
	return TRUE;
}


void CContextMenu::OnCommand(int nCommandId)
{
	CMapMenuItems::CPair* pPair = m_mapItems.Lookup(nCommandId);
	if (pPair != NULL)
	{
		IUOLMessengerContextMenuItemPtr pItem = pPair->m_value;
		pItem->OnCommand(nCommandId);
	}
	else
	{
		CMapMenuItems::CPair* pSubmenuPair;
		for (POSITION pos = m_mapItems.GetStartPosition(); pos; )
		{
			pSubmenuPair = m_mapItems.GetNext(pos);
			IUOLMessengerContextMenuItemPtr pSubMenu = pSubmenuPair->m_value;

			if ((pSubMenu != NULL) && (pSubMenu->IsMenu()))
			{
				pSubMenu->OnCommand(nCommandId);
			}
		}
	}
}


BOOL CContextMenu::CreateMenu(HWND hParent)
{
	if (::IsWindow(hParent))
	{
		ATLASSERT(FALSE);// TODO: BUG, implementar
		Attach(hParent);
	}
	else
	{
		ATLASSERT(m_hWnd == NULL);
		Create(::GetDesktopWindow(), NULL, NULL, WS_POPUPWINDOW);
	}

	if (IsWindow())
	{
		return m_wndMenu.CreatePopupMenu();
	}

	return FALSE;
}

BOOL CContextMenu::AttachMenu(CMenuHandle& menu)
{
	ATLASSERT(m_hWnd == NULL);
	Create(::GetDesktopWindow(), NULL, NULL, WS_POPUPWINDOW);

	if (menu)
	{
		m_wndMenu.Attach(menu);

		return TRUE;
	}

	return FALSE;
}

BOOL CContextMenu::DestroyMenu(BOOL bForce)
{
	if (IsWindow())
	{
		BOOL bDestroy = SendNotifyMessage(WM_CLOSE);
		//BOOL bDestroy = DestroyWindow();
		ATLASSERT(bDestroy != FALSE);
		//m_hWnd = NULL;
		
		m_mapItems.RemoveAll();
		
		//ATLTRACE(_T("CContextMenu::DestroyMenu(), %s\n"), GetLabel());
		
		return m_wndMenu.DestroyMenu();
	}
	
	return FALSE;
}


void CContextMenu::ShowMenu(const CPoint& ptScreen)
{
	if (m_wndMenu.IsNull())
	{
		return;
	}

	if (IsWindow())
	{
		::SetForegroundWindow(m_hWnd);

		m_wndMenu.TrackPopupMenuEx(0, ptScreen.x, ptScreen.y, m_hWnd);

		PostMessage(WM_NULL, 0, 0);
	}
}


BOOL CContextMenu::AddItem(IUOLMessengerContextMenuItemPtr pItem)
{
	BOOL bRet = FALSE;
	
	if ((m_wndMenu.IsNull() == FALSE) && (pItem != NULL))
	{
		int nItemId = pItem->GetCommand();
		
		UINT nFlags = MF_ENABLED;
		nFlags |= pItem->IsMenu() ? MF_POPUP : MF_STRING;
		
		BOOL bRet = m_wndMenu.AppendMenu(nFlags, nItemId, pItem->GetLabel());
		if (bRet)
		{
			m_mapItems.SetAt(nItemId, pItem);
		}
	}
	
	return bRet;
}


BOOL CContextMenu::AddSeparator()
{
	if (m_wndMenu.IsNull())
	{
		return FALSE;
	}

	return m_wndMenu.AppendMenu(MF_ENABLED | MF_SEPARATOR, 1, (LPCTSTR) NULL);
}


BOOL CContextMenu::InsertItem(int nItemPositionBefore, IUOLMessengerContextMenuItemPtr pItem)
{
	if ((m_wndMenu.IsNull() == FALSE) && (nItemPositionBefore >= 0) && (pItem != NULL))
	{
		int nItemId = pItem->GetCommand();

		UINT nFlags = MF_BYPOSITION | MF_ENABLED;
		nFlags |= pItem->IsMenu() ? MF_POPUP : MF_STRING;

		BOOL bRet = m_wndMenu.InsertMenu(nItemPositionBefore, nFlags, nItemId, pItem->GetLabel());
		if (bRet)
		{
			m_mapItems.SetAt(nItemId, pItem);
		}

		return TRUE;
	}

	return FALSE;
}


BOOL CContextMenu::InsertSeparator(int nItemPositionBefore)
{
	if ((m_wndMenu.IsNull() == FALSE) && (nItemPositionBefore >= 0))
	{
		return m_wndMenu.InsertMenu(nItemPositionBefore, MF_BYPOSITION | MF_ENABLED | MF_SEPARATOR, 1, (LPCTSTR) NULL);
	}

	return FALSE;
}


BOOL CContextMenu::RemoveItem(IUOLMessengerContextMenuItemPtr pItem)
{	
	if ((m_wndMenu.IsNull() == FALSE) && (pItem != NULL))
	{
		int nItemId = pItem->GetCommand();
		m_mapItems.RemoveKey(nItemId);

		return m_wndMenu.RemoveMenu(nItemId, MF_BYCOMMAND);
	}

	return FALSE;
}


BOOL CContextMenu::RemoveItem(int nItemPosition)
{
	if ((m_wndMenu.IsNull() == FALSE) && (nItemPosition >= 0))
	{
		MENUITEMINFO mii = {0};
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_ID | MIIM_TYPE;
		m_wndMenu.GetMenuItemInfo(nItemPosition, TRUE, &mii);

		if (mii.fType & MF_SEPARATOR)
		{
			// Separator
			return m_wndMenu.RemoveMenu(nItemPosition, MF_BYPOSITION);
		}

		CMapMenuItems::CPair* pItemPair = m_mapItems.Lookup(mii.wID);
		if (pItemPair != NULL)
		{
			return RemoveItem(pItemPair->m_value);
		}
	}

	return FALSE;
}

BOOL CContextMenu::RemoveAll()
{
	if (m_wndMenu.IsNull())
	{
		return FALSE;
	}

	int nCount = m_wndMenu.GetMenuItemCount();
	for (int nInd = 0; nInd < nCount; nInd++)
	{
		m_wndMenu.RemoveMenu(nInd, MF_BYPOSITION);
	}

	m_mapItems.RemoveAll();

	return TRUE;
}


int CContextMenu::GetItemCount()
{
	if (m_wndMenu.IsNull())
	{
		return 0;
	}

	return m_wndMenu.GetMenuItemCount();
}


int CContextMenu::FindItemPosition(IUOLMessengerContextMenuItemPtr pItem)
{
	int nItemCount = GetItemCount();
	int nItemId = pItem->GetCommand();
	for (int nInd = 0; nInd < nItemCount; nInd++)
	{
		MENUITEMINFO mii = {0};
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_ID | MIIM_TYPE;
		m_wndMenu.GetMenuItemInfo(nInd, TRUE, &mii);

		if ((mii.wID == nItemId) &&
			!(mii.fType & MF_SEPARATOR))
		{
			return nInd;
		}
	}

	return -1;
}


BOOL CContextMenu::EnableItem(IUOLMessengerContextMenuItemPtr pItem, BOOL bEnable)
{
	BOOL bEnablementChanged = FALSE;
	
	if ((m_wndMenu.IsNull() == FALSE) && (pItem != NULL))
	{
		bEnablementChanged = m_wndMenu.EnableMenuItem(pItem->GetCommand(), 
				MF_BYCOMMAND | (bEnable ? MF_ENABLED : (MF_GRAYED | MF_DISABLED)));		
	}
	
	return bEnablementChanged;
}


BOOL CContextMenu::EnableItemByPosition(int nItemPosition, BOOL bEnable)
{
	BOOL bEnablementChanged = FALSE;
	
	if ((m_wndMenu.IsNull() == FALSE) && (nItemPosition >= 0))
	{
		bEnablementChanged = m_wndMenu.EnableMenuItem(nItemPosition, 
				MF_BYPOSITION | (bEnable ? MF_ENABLED : (MF_GRAYED | MF_DISABLED)));
	}
	
	return bEnablementChanged;
}


BOOL CContextMenu::IsEnabled(IUOLMessengerContextMenuItemPtr pItem)
{
	if ((m_wndMenu.IsNull() == FALSE) && (pItem != NULL))
	{
		int nState = m_wndMenu.GetMenuState(pItem->GetCommand(), MF_BYCOMMAND);

		return !(nState & MF_DISABLED);
	}

	return FALSE;
}


BOOL CContextMenu::IsEnabled(int nItemPosition)
{
	if ((m_wndMenu.IsNull() == FALSE) && (nItemPosition >= 0))
	{
		int nState = m_wndMenu.GetMenuState(nItemPosition, MF_BYPOSITION);

		return !(nState & MF_DISABLED);
	}

	return FALSE;
}


void CContextMenu::CheckItem(IUOLMessengerContextMenuItemPtr pItem, BOOL bChecked)
{
	if ((m_wndMenu.IsNull() == FALSE) && (pItem != NULL))
	{
		m_wndMenu.CheckMenuItem(pItem->GetCommand(), MF_BYCOMMAND | (bChecked ? MF_CHECKED : MF_UNCHECKED));
	}
}


void CContextMenu::CheckItem(int nItemPosition, BOOL bChecked)
{
	if ((m_wndMenu.IsNull() == FALSE) && (nItemPosition >= 0))
	{
		m_wndMenu.CheckMenuItem(nItemPosition, MF_BYPOSITION | (bChecked ? MF_CHECKED : MF_UNCHECKED));
	}
}


BOOL CContextMenu::IsChecked(IUOLMessengerContextMenuItemPtr pItem)
{	
	if ((m_wndMenu.IsNull() == FALSE) && (pItem != NULL))
	{
		int nState = m_wndMenu.GetMenuState(pItem->GetCommand(), MF_BYCOMMAND);

		return (nState & MF_CHECKED);
	}

	return FALSE;
}


BOOL CContextMenu::IsChecked(int nItemPosition)
{
	if ((m_wndMenu.IsNull() == FALSE) && (nItemPosition >= 0))
	{
		int nState = m_wndMenu.GetMenuState(nItemPosition, MF_BYPOSITION);

		return (nState & MF_CHECKED);
	}

	return FALSE;
}


void CContextMenu::RemoveAllMenuElements()
{
	CMapMenuItems::CPair* pMenuElementPair = NULL;
	
	for (POSITION pos = m_mapItems.GetStartPosition(); pos; )
	{
		pMenuElementPair = m_mapItems.GetNext(pos);
		IUOLMessengerContextMenuItemPtr pSubMenu = pMenuElementPair->m_value;
		
		if (pSubMenu != NULL)
		{
			if (pSubMenu->IsMenu())
			{
				CContextMenuPtr pContextMenu = 
						ap_dynamic_cast<CContextMenuPtr>(pSubMenu);
				
				if (pContextMenu)
				{
					pContextMenu->RemoveAllMenuElements();
					RemoveItem(ap_static_cast<IUOLMessengerContextMenuItemPtr>(pContextMenu));
					pContextMenu->DestroyMenu();
				}
			}
			else
			{
				BOOL bRemoved = m_wndMenu.RemoveMenu(
						pSubMenu->GetCommand(), MF_BYCOMMAND);
				
				ATLASSERT(bRemoved == TRUE);
			}
		}
	}
	
	m_mapItems.RemoveAll();
}


void CContextMenu::OnCommand(UINT nId, int nCode, HWND hWnd)
{
	//ATLTRACE(_T("CContextMenu::OnCommand() - nId = %d, nCode = %d, hWnd = 0x%x\n"), nId, nCode, hWnd);

	OnCommand(nCode);
}


void CContextMenu::OnClose()
{
	//ATLTRACE(_T("CContextMenu::OnClose()\n"));
	BOOL bDestroy = DestroyWindow();
	ATLASSERT(bDestroy);
}


//void CContextMenu::OnDestroy()
//{
//	ATLTRACE(_T("CContextMenu::OnDestroy()\n"));
//	SetMsgHandled(FALSE);
//}


//void CContextMenu::OnFinalMessage(HWND hWnd)
//{
//	ATLTRACE(_T("CContextMenu::OnFinalMessage() - 0x%x"), hWnd);
//
//	delete this;
//}


IUOLMessengerContextMenuItemPtr CContextMenu::AddCustomMenuItem(
		const CString& strLabel, IUOLMessengerMenuItemCallback* pCallback, 
		BOOL bAddSeparator)
{
	if ( (strLabel.IsEmpty()) || (pCallback == NULL) )
	{
		return NULL;
	}
	
	if (bAddSeparator)
	{
		AddSeparator();
	}
	
	IUOLMessengerContextMenuItemPtr pMenuItem = new CContextMenuItem(NULL, (LPCTSTR)strLabel, pCallback);
	
	AddItem(pMenuItem);
	
	return pMenuItem;
}


IUOLMessengerContextMenuItemPtr CContextMenu::AddMenuItem(
		void* pUOLMessengerPlugin, int nCommandId, 
		const CString& strLabel, IUOLMessengerMenuItemCallback* pCallback, 
		BOOL bAddSeparator)
{
	IUOLMessengerContextMenuItemPtr pMenuItem;
	
	CMethodAuthorizationManagerPtr pMethodAuthorizationMethod = 
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetMethodAuthorizationManager();
	
	if ( (pMethodAuthorizationMethod) && 
			(pMethodAuthorizationMethod->IsAuthorized(
					pUOLMessengerPlugin, "AddMenuItem")) )
	{
		if ( (!strLabel.IsEmpty()) && (pCallback != NULL) )
		{
			if (bAddSeparator)
			{
				AddSeparator();
			}
			
			pMenuItem = new CContextMenuItem(nCommandId, (LPCTSTR)strLabel, pCallback);
			
			AddItem(pMenuItem);
		}
	}
	
	return pMenuItem;
}


void CContextMenu::RemoveMenuItem(
		void* pUOLMessengerPlugin, IUOLMessengerContextMenuItemPtr pItem)
{
	CMethodAuthorizationManagerPtr pMethodAuthorizationMethod = 
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetMethodAuthorizationManager();
	
	if ( (pMethodAuthorizationMethod) && 
			(pMethodAuthorizationMethod->IsAuthorized(
					pUOLMessengerPlugin, "RemoveMenuItem")) )
	{
		RemoveItem(pItem);
	}
}


void CContextMenu::RemoveAllMenuItems(void* pUOLMessengerPlugin)
{
	CMethodAuthorizationManagerPtr pMethodAuthorizationMethod = 
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetMethodAuthorizationManager();
	
	if ( (pMethodAuthorizationMethod) && 
			(pMethodAuthorizationMethod->IsAuthorized(
					pUOLMessengerPlugin, "RemoveAllMenuItems")) )
	{
		RemoveAllMenuElements();
	}
}


BOOL CContextMenu::EnableMenuItem(void* pUOLMessengerPlugin, 
		IUOLMessengerContextMenuItemPtr pItem, BOOL bEnable)
{
	CMethodAuthorizationManagerPtr pMethodAuthorizationMethod = 
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetMethodAuthorizationManager();
	
	BOOL bEnablementChanged = FALSE;
	
	if ( (pMethodAuthorizationMethod) && 
			(pMethodAuthorizationMethod->IsAuthorized(
					pUOLMessengerPlugin, "EnableMenuItem")) )
	{
		bEnablementChanged = EnableItemByPosition(FindItemPosition(pItem), bEnable);
	}
	
	return bEnablementChanged;
}


IUOLMessengerContextMenuPtr CContextMenu::AddSubMenu(
		void* pUOLMessengerPlugin, const CString& strLabel)
{
	CContextMenuPtr pSubMenu;
	
	CMethodAuthorizationManagerPtr pMethodAuthorizationMethod = 
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetMethodAuthorizationManager();
	
	if ( (pMethodAuthorizationMethod) && 
			(pMethodAuthorizationMethod->IsAuthorized(
					pUOLMessengerPlugin, "AddSubMenu")) )
	{
		if (!strLabel.IsEmpty())
		{
			pSubMenu = new CContextMenu((LPCTSTR)strLabel);
			
			if (pSubMenu->CreateMenu())
			{
				AddItem(pSubMenu);
			}
		}
	}
	
	return pSubMenu;
}


void CContextMenu::RemoveSubMenu(
		void* pUOLMessengerPlugin, IUOLMessengerContextMenuPtr pSubMenu)
{
	CMethodAuthorizationManagerPtr pMethodAuthorizationMethod = 
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetMethodAuthorizationManager();
	
	if ( (pMethodAuthorizationMethod) && 
			(pMethodAuthorizationMethod->IsAuthorized(
					pUOLMessengerPlugin, "RemoveSubMenu")) )
	{
		CContextMenuPtr pContextMenu = ap_static_cast<CContextMenuPtr>(pSubMenu);
		
		if (pContextMenu)
		{
			RemoveItem(ap_static_cast<IUOLMessengerContextMenuItemPtr>(pContextMenu));
			
			// Force the destruction considering that the creation of this submenu 
			// could be made by a thread what will lead to first "IsWindow()" test 
			// failure.
			pContextMenu->DestroyMenu(TRUE);
		}
	}
}

