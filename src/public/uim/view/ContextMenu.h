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
 
#pragma once

#include "ContextMenuItem.h"
#include "interfaces/IUOLMessengerContextMenu.h"


class CContextMenu : public CWindowImpl<CContextMenu>,
					 public	CContextMenuItem, 
					 public IUOLMessengerContextMenu
{
public:
	DECLARE_WND_CLASS("UIMContextMenuClass")

	CContextMenu(const CString& strLabel = _T(""));
	virtual ~CContextMenu();

	virtual int GetCommand();

	virtual BOOL IsMenu();
	virtual void OnCommand(int nCommandId);

	BOOL CreateMenu(HWND hParent = NULL);
	BOOL AttachMenu(CMenuHandle& menu); 
	BOOL DestroyMenu(BOOL bForce = FALSE);
	void ShowMenu(const CPoint& ptScreen);

	BOOL AddItem(IUOLMessengerContextMenuItemPtr pItem);
	BOOL AddSeparator();

	BOOL InsertItem(int nItemPositionBefore, IUOLMessengerContextMenuItemPtr pItem);
	BOOL InsertSeparator(int nItemPositionBefore);

	BOOL RemoveItem(IUOLMessengerContextMenuItemPtr pItem);
	BOOL RemoveItem(int nItemPosition);
	BOOL RemoveAll();

	int GetItemCount();

	int FindItemPosition(IUOLMessengerContextMenuItemPtr pItem);

	BOOL EnableItem(IUOLMessengerContextMenuItemPtr pItem, BOOL bEnable);
	BOOL EnableItemByPosition(int nItemPosition, BOOL bEnable);
	BOOL IsEnabled(IUOLMessengerContextMenuItemPtr pItem);
	BOOL IsEnabled(int nItemPosition);

	void CheckItem(IUOLMessengerContextMenuItemPtr pItem, BOOL bChecked);
	void CheckItem(int nItemPosition, BOOL bChecked);
	BOOL IsChecked(IUOLMessengerContextMenuItemPtr pItem);
	BOOL IsChecked(int nItemPosition);
	
	// Remove the items and call the same method for those that are 
	// CContextMenu as well.
	//
	void RemoveAllMenuElements();

protected:
	
	BEGIN_MSG_MAP_EX(CContextMenu)
		MSG_WM_COMMAND(OnCommand)
		MSG_WM_CLOSE(OnClose)
		//MSG_WM_DESTROY(OnDestroy)
	END_MSG_MAP()
	
	void OnCommand(UINT nId, int nCode, HWND hWnd);
	void OnClose();
	//void OnDestroy();
	
	//void OnFinalMessage(HWND hWnd);
	
	virtual IUOLMessengerContextMenuItemPtr AddCustomMenuItem(
			const CString& strLabel, IUOLMessengerMenuItemCallback* pCallback, 
			BOOL bAddSeparator = FALSE);
	
	virtual IUOLMessengerContextMenuItemPtr AddMenuItem(
			void* pUOLMessengerPlugin, int nCommandId, 
			const CString& strLabel, IUOLMessengerMenuItemCallback* pCallback, 
			BOOL bAddSeparator = FALSE);
	
	virtual void RemoveMenuItem(
			void* pUOLMessengerPlugin, IUOLMessengerContextMenuItemPtr pItem);
	
	virtual void RemoveAllMenuItems(void* pUOLMessengerPlugin);
	
	virtual BOOL EnableMenuItem(void* pUOLMessengerPlugin, 
			IUOLMessengerContextMenuItemPtr pItem, BOOL bEnable);
	
	virtual IUOLMessengerContextMenuPtr AddSubMenu(
			void* pUOLMessengerPlugin, const CString& strLabel);
	
	virtual void RemoveSubMenu(
			void* pUOLMessengerPlugin, IUOLMessengerContextMenuPtr pSubMenu);
	
private:
	CString m_strLabel;

	typedef CAtlMap<int, IUOLMessengerContextMenuItemPtr> CMapMenuItems;

	WTL::CMenu					m_wndMenu;

	CMapMenuItems	m_mapItems;
};

MAKEAUTOPTR(CContextMenu);