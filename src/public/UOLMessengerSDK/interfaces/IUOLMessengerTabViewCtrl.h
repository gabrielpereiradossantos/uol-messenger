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


class IUOLMessengerTabViewCtrl;


class __declspec(novtable) IUOLMessengerTabViewCtrlObserver
{
public:

	virtual ~IUOLMessengerTabViewCtrlObserver() {};
	
	virtual void OnConversationTabSelected( IUOLMessengerTabViewCtrl* pTabView, 
											IUOLMessengerConversationPtr pConversation, 
											int nTabIndex ) = 0;
};

class __declspec(novtable) IUOLMessengerTabViewCtrlObserver2: public IUOLMessengerTabViewCtrlObserver
{
public:

	virtual ~IUOLMessengerTabViewCtrlObserver2() {};
	
	virtual void OnConversationTabMouseHover( IUOLMessengerTabViewCtrl* pTabView, 
											IUOLMessengerConversationPtr pConversation, 
											CPoint& pt, HWND wHandle ) = 0;
	virtual void OnConversationTabMouseLeave() = 0;
};

class __declspec(novtable) IUOLMessengerTabViewCtrl
{
public:
	enum TabViewType
	{
		TAB_VIEW_MESSAGE_WINDOW = 0,
		TAB_VIEW_OTHER
	};
	
	virtual ~IUOLMessengerTabViewCtrl() {}
	
	void RegisterObserver(IUOLMessengerTabViewCtrlObserver* pObserver);
	void UnregisterObserver(IUOLMessengerTabViewCtrlObserver* pObserver);
	
	virtual void SetTabViewType(int tabViewType) = 0;
	virtual void ShowCloseButton(BOOL bShow) = 0;
	virtual void SetInterButtonSpace(int buttonSpace) = 0;
	virtual void SetTabButtonMargins(int leftMarging, int topMarging, int rightMargin, int bottomMarging) = 0;
	virtual void AutoCalculateTabsWidths(BOOL bCalculate) = 0;
	
	virtual int  CreateTabButton(CString tabButtonName) = 0;
	virtual BOOL SetTabButtonImage(int nTabIndex, HMODULE hModule, DWORD dwResourceID) = 0;
	virtual BOOL RemoveTabButton(int nTabIndex) = 0;
	
};

MAKEAUTOPTR(IUOLMessengerTabViewCtrl);
