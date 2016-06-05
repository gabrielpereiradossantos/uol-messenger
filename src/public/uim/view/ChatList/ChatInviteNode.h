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

#include "../GroupListCtrl/Entry.h"
#include "../ContactList/ComparatorAdapter.h"
#include "../IPainter.h"
#include "../UOLToolBar.h"
#include "../skin/FontElement.h"
#include "../ImageButton.h"
#include "../Tooltip/ContactTooltip.h"
#include "../FindContactPanel.h"

#include <interfaces/IUOLMessengerContactListSettings.h>
#include <interfaces/IUOLMessengerChatInviteNode.h>

class CChatInviteNode : 
	public IUOLMessengerChatInviteNode,
	public GroupListCtrl::CEntry, 
	public CUOLMessengerContactObserver,
    public CUOLMessengerImageButtonEventListener2
{
public:
	CChatInviteNode(IUOLMessengerContactPtr pContact,
		IUOLMessengerImagePtr pCheckOnImage, IUOLMessengerImagePtr pCheckOffImage);
	virtual ~CChatInviteNode();

	void SetComparatorAdapter(CComparatorAdapter* pComparator);
	CComparatorAdapter* GetComparatorAdapter() const;

	void SetDefaultPainter(IPainter* pPainter);
	IPainter* GetDefaultPainter(IPainter* pPainter) const;

	void SetPainter(IPainter* pPainter);
	IPainter* GetPainter() const;

	void SetSelectionPainter(IPainter* pPainter);
	IPainter* GetSelectionPainter() const;

	void SetTextFont(CFontElementPtr pFont);
	void SetSelectedTextFont(CFontElementPtr pFont);
	void SetTextFontBlocked(CFontElementPtr pFont);
	void SetDropTargetTextFont(CFontElementPtr pFont);
	void SetTextColor(COLORREF clrText);
	void SetTextColorBlocked(COLORREF clrText);
	void SetSelectedTextColor(COLORREF clrText);
	void SetDropTargetTextColor(COLORREF clrText);
	void SetFindContactPanel(CFindContactPanelPtr pFindContactPanel);

	void SetAllContactsGroup(const CString& strAllContactsGroup);
	CString GetAllContactsGroup() const;

	void GetMenuItens(CAtlList<IUOLMessengerContextMenuItemPtr>& listItens);

	// IUOLMessengerChatInviteNode interface
	virtual IUOLMessengerContactPtr GetContact() const;
	virtual void AddCustomButton(IUOLMessengerImageButton2Ptr pButton);
	virtual void RemoveCustomButton(IUOLMessengerImageButton2Ptr pButton);
	virtual void AddCustomMenuItem(IUOLMessengerContextMenuItemPtr pCustomMenuItem);
	virtual void RemoveCustomMenuItem(IUOLMessengerContextMenuItemPtr pCustomMenuItem);
	virtual BOOL IsChecked();

	// CImageButtonEventListener methods
	void OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick);

protected:
	// CEntry interface
	virtual void SetName(const CString& strName);
	virtual CString GetName() const;

	virtual BOOL IsVisible() const;

	virtual int Compare(const CEntry* pEntry) const;

	virtual void Paint(CDCHandle& dc);
	virtual void DrawFocusRect(CDCHandle& dc);
	virtual void DrawBackground(CDCHandle& dc);

	virtual void OnLButtonDown(const CPoint&);
	virtual void OnLButtonUp(const CPoint&);
	virtual void OnLBtnDblClick(const CPoint&);
	virtual void OnRButtonDown(const CPoint&);
	virtual void OnRBtnDblClick(const CPoint&);

	virtual void OnMouseHover(const CPoint&);
	virtual void OnMouseLeave();
	virtual void OnMouseMove(const CPoint& pt);
	virtual void OnReturnKeyPressed();
	virtual void OnTimer(UINT nId);

	// CUOLMessengerContactObserver interface
	virtual void Update(IUOLMessengerContact* pContact);
	virtual void UpdateAlias(IUOLMessengerContact* pContact);
	virtual void UpdateStatus(IUOLMessengerContact* pContact);
	virtual void UpdateIcon(IUOLMessengerContact* pContact);

	void RestorePainter(int nTimeout);

	virtual void RegisterObserver(CUOLMessengerChatInviteNodeObserver* pObserver);
	virtual void UnregisterObserver(CUOLMessengerChatInviteNodeObserver* pObserver);

private:
	void DrawText(CDCHandle& dc);
    void DrawStatus(CDCHandle& dc);
	void DrawCheckbox(CDCHandle& dc);
	
	void UpdateImpl();

	void NotifyObservers(void (CUOLMessengerChatInviteNodeObserver::*pfnCallback)(IUOLMessengerChatInviteNode*, IUOLMessengerImageButton2Ptr), IUOLMessengerImageButton2Ptr);

	// For fixed elements (like status and avatar), returns the element drawing rect.
	// For variable elements, it returns the available rect.
	//
	void GetRectForElement(IUOLMessengerContactListSettings2::ContactNodeElements nContactElement, CRect& rElementRect);

	void UpdateButtonState(const CPoint& pt, CImageButtonPtr pButton, CImageButton::MouseState mouseState);
	BOOL IsPointInButtonRect(const CPoint& pt, CImageButtonPtr pButton);
	BOOL IsFindContactMatch() const;
	
private:
	IUOLMessengerContactPtr	m_pContact;
	CComparatorAdapter*		m_pComparatorAdapter;
	IPainter*				m_pDefaultPainter;
	IPainter*				m_pCurrentPainter;
	IPainter*				m_pSelectionPainter;
	CString					m_strAllContactsGroup;
	CString					m_strName;
	CString					m_strChatKey;
	time_t					m_timeRestore;
    CRect					m_rectStatus;
	CRect					m_rectCheckbox;
	CFontElementPtr			m_pFontTextNormal;
	CFontElementPtr			m_pFontTextSelected;
	CFontElementPtr			m_pFontContactBlocked;
	CFontElementPtr			m_pFontTextDropTarget;
	COLORREF				m_clrTextNormal;
	COLORREF				m_clrTextSelected;
	COLORREF				m_clrTextBlocked;
	COLORREF				m_clrTextDropTarget;
	CContactTooltipPtr		m_pContactTooltip;
	CUOLToolBarPtr			m_pToolbar;
	CComAutoCriticalSection	m_criticalSection;
	CImageButtonPtr			m_pCheckOnButton;
	CImageButtonPtr			m_pCheckOffButton;
	BOOL					m_bIsChecked;
	CFindContactPanelPtr	m_pFindContactPanel;

	CAtlList<CUOLMessengerChatInviteNodeObserver*>	m_listObservers;
	CAtlList<IUOLMessengerImageButton2Ptr>			m_listButtons;
	CAtlList<IUOLMessengerContextMenuItemPtr>		m_listMenus;
}; 

MAKEAUTOPTR(CChatInviteNode);