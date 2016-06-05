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


#include <interfaces/IUOLMessengerContextMenuItem.h>
#include "UOLFoneCapabilitiesHandler.h"


class CContactListMonitor : 
		public CUOLMessengerContactListCtrlObserver, 
		public CUOLMessengerImageButtonEventListener2, 
		public IUOLMessengerMenuItemCallback, 
		public CUOLMessengerContactObserver2
{
public:
	
	virtual ~CContactListMonitor(void);
	
	static CContactListMonitor* GetInstance();
	
	BOOL Init(IUOLMessengerFacade* pUOLMessengerFacade);
	void Finalize();

	void UpdateContactList(BOOL bConnected);
	
protected:
	
	CContactListMonitor(void);
	
	// CUOLMessengerContactListCtrlObserver methods
	//
	virtual void OnAddContact(IUOLMessengerContactNodePtr pContact);
	virtual void OnRemoveContact(IUOLMessengerContactNodePtr pContact);
	virtual void OnRemoveContacts(CAtlList<IUOLMessengerContactNodePtr>& listContacts);
	virtual void OnShowContextMenu(IUOLMessengerContextMenuPtr pContactContextMenu, 
			IUOLMessengerContactNodePtr pContactNode);
	
	// CUOLMessengerImageButtonEventListener2 methods
	//
	//virtual void OnLButtonDown(IUOLMessengerImageButton* pButton, const CPoint& ptClick);
	virtual void OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint& ptClick);
	//virtual void OnLButtonDblClick(IUOLMessengerImageButton* pButton, const CPoint& ptClick);
	//virtual void OnRButtonDown(IUOLMessengerImageButton* pButton, const CPoint& ptClick);
	//virtual void OnRButtonDblClick(IUOLMessengerImageButton* pButton, const CPoint& ptClick);
	//virtual void OnMouseMove(IUOLMessengerImageButton* pButton, const CPoint& ptClick);
	
	// IUOLMessengerMenuItemCallback methods
	//
	virtual void OnCommand(IUOLMessengerContextMenuItem* pItem);
	
	// CUOLMessengerContactObserver methods
	//
	virtual void Update(IUOLMessengerContact* pContact);
	virtual void UpdateAlias(IUOLMessengerContact* pContact);
	virtual void UpdateStatus(IUOLMessengerContact* pContact);
	virtual void UpdateIcon(IUOLMessengerContact* pContact);
	
	// CUOLMessengerContactObserver2 methods
	//
	virtual void UpdateExtendedCapabilities(IUOLMessengerContact2* pContact);
	
	void MakeCall(IUOLMessengerContactNodePtr pContactNode);
	
private:
	
	void AddCustomButtons();
	void RemoveCustomButtons();
	void AddCustomButton(IUOLMessengerContactNodePtr pContactNode);
	void RemoveCustomButton(IUOLMessengerContactNodePtr pContactNode, BOOL unregisterContact);
	
	void UpdateButtonCondition(IUOLMessengerContact* pContact);
	
	void AddCustomMenuItems();
	void RemoveCustomMenuItems();
	void AddCustomMenuItem(IUOLMessengerContactNodePtr pContactNode);
	void RemoveCustomMenuItem(IUOLMessengerContactNodePtr pContactNode);
	
	IUOLMessengerContextMenuItemPtr CreateUOLFoneContextMenuItem(
			IUOLMessengerContactNodePtr pContactNode);
	
	IUOLMessengerContactNodePtr FindContactNode(IUOLMessengerImageButton2* pButton);
	IUOLMessengerContactNodePtr FindContactNode(IUOLMessengerContact* pContact);
	
	IUOLMessengerImageButtonPtr FindContactNodeImageButton(IUOLMessengerContactNodePtr pContactNode);
	
	IUOLMessengerContactNodePtr FindContactNode(IUOLMessengerContextMenuItem* pMenuItem);

	BOOL IsCompatible(IUOLMessengerContactPtr pContact);
	
private:
	
	IUOLMessengerContactListCtrlPtr		m_pContactListCtrl;
	
	IUOLMessengerFacade*	m_pUOLMessengerFacade;
	
	IUOLMessengerImagePtr	m_pUOLFoneButtonImage;
	
	typedef CAtlMap<IUOLMessengerContactNodePtr, IUOLMessengerImageButton2Ptr>	CMapContactNode;
	CMapContactNode			m_mapContactNode;
	
	CComAutoCriticalSection		m_buttonsCS;
	
	typedef CAtlMap<IUOLMessengerContactNodePtr, IUOLMessengerContextMenuItemPtr>	CMapContactNodeMenuItem;
	CMapContactNodeMenuItem		m_mapContactNodeMenuItem;
	
	CComAutoCriticalSection		m_menuItemsCS;
	
	CString						m_menuItemLabel;
	
	CUOLFoneCapabilitiesHandler	m_capabilitiesHandler;
	
	IUOLMessengerContextMenuItemPtr		m_pCurrentMenuItem;
	IUOLMessengerContactNodePtr			m_pCurrentMenuItemContactNode;
};

