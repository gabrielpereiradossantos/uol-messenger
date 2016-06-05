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

#include <interfaces/IUOLMessengerChatListCtrl.h>
#include <interfaces/IUOLMessengerContactList.h>
#include <interfaces/IUOLMessengerContactListOrderTraits.h>
#include <interfaces/IUOLMessengerContactListSettings.h>
#include <interfaces/IUOLMessengerConversation.h>

#include "../GroupListCtrl/GroupListCtrl.h"
#include "../skin/WindowElement.h"
#include "../skin/GroupElement.h"
#include "../skin/ContactElement.h"
#include "../IPainterRepository.h"
#include "../ContactList/ComparatorAdapter.h"
#include "ChatMenu.h"
#include "../../controller/utils.h"

#include <DropTarget.h>

typedef CDropTarget<GroupListCtrl::CGroupListCtrl> CChatListCtrlTarget;

class CChatListCtrl : 
	public IUOLMessengerChatListCtrl,
	public CWindowElement<CChatListCtrl, CChatListCtrlTarget>,
    public CUOLMessengerContactListObserver,
	public IUOLMessengerSettingsEventListener,
	public IPainterRepository
{
public:
	DECLARE_ELEMENT_NAME("ChatList");

    CChatListCtrl();
	virtual ~CChatListCtrl();

	void Init(IUOLMessengerContactListPtr pContactList);
    void Finalize();

	IUOLMessengerContact3Ptr GetSelectedContact();
	void SetOrderTraits(IUOLMessengerContactListOrderTraitsPtr pOrderTraits);
	IUOLMessengerContactListOrderTraitsPtr GetOrderTraits();

	void ChangeContactPainter(IUOLMessengerContact3Ptr pContact, UINT nPainterId);
    void AddChatContact(IUOLMessengerContact3Ptr pContact);
    void RemoveChatContact(IUOLMessengerContact3Ptr pContact);
    void RemoveChatContacts();
    void SetChatKey(const CString& strChatKey);
    void SetConversation(IUOLMessengerConversation2Ptr pConversation);

protected:
	virtual CString GetName() const;
	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	virtual void GetMinSize(CSize& size);
	
    // CUOLMessengerContactListObserver interface
	virtual void OnAddContact(IUOLMessengerContactPtr pContact);
    virtual void OnRemoveContact(IUOLMessengerContactPtr pContact);
    virtual void OnRemoveContacts(CAtlList<IUOLMessengerContactPtr>& listContacts);
    virtual void OnMoveContact(IUOLMessengerContactPtr pContact, const CString& strOldGroup) {};
    virtual void OnAddGroup(IUOLMessengerGroupPtr pGroup) {};
    virtual void OnRemoveGroup(IUOLMessengerGroupPtr pGroup) {};
    virtual void OnRemoveAllGroups() {};

    // IUOLMessengerSettingsEventListener
	virtual void OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId);

	// IPainterRepository interface
	virtual IPainter* GetPainter(UINT nId);

	BEGIN_MSG_MAP(CChatListCtrl)
		MSG_WM_TIMER(OnTimer)
		CHAIN_MSG_MAP(CChatListCtrlTarget)
	END_MSG_MAP()

//	void OnMouseMove(UINT nFlags, const CPoint& pt);
	void OnTimer(UINT nTimerId, TIMERPROC timerproc);
//	void OnLButtonUpMsg(UINT nFlags, CPoint& pt);
	
	virtual void PaintBackground(CDCHandle& dc, const CRect& rect);
	virtual void OnRButtonDown(GroupListCtrl::CGroupListCtrlItemPtr pItem, const CPoint& pt);

	// CDropTarget methods
	virtual BOOL IsValidObject(IDataObject *pDataObject);
	virtual DWORD DoDragEnter(DWORD grfKeyState, POINTL pt, DWORD dwAllowed);
	virtual DWORD DoDragOver(DWORD grfKeyState, POINTL pt, DWORD dwAllowed);
	virtual void DoDragLeave();
	virtual DWORD DoDrop(IDataObject *pDataObject, POINTL pt);

	// IUOLMessengerChatListCtrl methods
	virtual void GetContacts(CAtlList<IUOLMessengerChatNodePtr>& listContacts);
	virtual IUOLMessengerImageButton2Ptr CreateContactButton();
	virtual IUOLMessengerContextMenuItemPtr CreateContextMenuItem(const CString& strLabel, IUOLMessengerMenuItemCallback* pCallback);
	
private:
	static const int TIMER_SCROLL_VIEW;
    static const int NODE_HEIGHT;

	enum ScrollCommand
	{
		SC_SCROLL_UP = -1,
		SC_SCROLL_NONE = 0,
		SC_SCROLL_DOWN = 1,
	};

	void InternalAddContact(IUOLMessengerContact3Ptr pContact);
	void InternalRemoveContact(IUOLMessengerContact3Ptr pContact, BOOL bRedraw = TRUE);
	CString FormatChatNodeName(IUOLMessengerContact3Ptr pContact);
	CString FormatChatNodeName(const CString& strContact, const CString& strAccountUserId, const CString& strAccountProtocolId);
	int GetGroupHeight() const;
	int GetContactHeight(SizeImage sizeImage) const;

	DWORD QueryDragEffect(DWORD grfKeyState, const CPoint& ptClient);
	void DoScrolling(ScrollCommand scCommand);
    BOOL IsSameAccount(IUOLMessengerAccountPtr pAccount);

    void CreateChatGroup();

private:
	ScrollCommand		m_scrollCommand;
	UINT				m_nTimerScroll;
	int					m_nScrollLineHeight;
	IDataObject*		m_pTransferDataObject;

	IUOLMessengerContactListPtr		m_pContactList;
	CComparatorAdapterPtr			m_pComparatorAdapter;
	CGroupElement*					m_pGroupElement;
	CContactElement*				m_pContactElement;

	GroupListCtrl::CEntryGroupPtr	m_pChatGroup;

	CChatMenuPtr					m_pChatMenu;

	CComAutoCriticalSection	m_criticalSection;
    CStringGUID	                    m_strGUID;
    CString                         m_strChatKey;
    IUOLMessengerConversation2Ptr   m_pConversation;
};

MAKEAUTOPTR(CChatListCtrl);