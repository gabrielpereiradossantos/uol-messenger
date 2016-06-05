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

#include "../skin/fontelement.h"

#include "../Tooltip/ContactTooltip.h"

#include <interfaces/IUOLMessengerContactListSettings.h>
#include <interfaces/IUOLMessengerReadyForChatNode.h>

class CReadyForChatNode : 
	public IUOLMessengerReadyForChatNode,
	public GroupListCtrl::CEntry, 
	public CUOLMessengerContactObserver
{
public:
	CReadyForChatNode(IUOLMessengerContactPtr pContact, const CString& strChatKey);
	virtual ~CReadyForChatNode();

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

	void SetAllContactsGroup(const CString& strAllContactsGroup);
	CString GetAllContactsGroup() const;

	void GetMenuItens(CAtlList<IUOLMessengerContextMenuItemPtr>& listItens);

	// IUOLMessengerReadyForChatNode interface
	virtual IUOLMessengerContactPtr GetContact() const;
	virtual void AddCustomButton(IUOLMessengerImageButton2Ptr pButton);
	virtual void RemoveCustomButton(IUOLMessengerImageButton2Ptr pButton);
	virtual void AddCustomMenuItem(IUOLMessengerContextMenuItemPtr pCustomMenuItem);
	virtual void RemoveCustomMenuItem(IUOLMessengerContextMenuItemPtr pCustomMenuItem);

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

	virtual void RegisterObserver(CUOLMessengerReadyForChatNodeObserver* pObserver);
	virtual void UnregisterObserver(CUOLMessengerReadyForChatNodeObserver* pObserver);

private:
	void DrawText(CDCHandle& dc);
    void DrawStatus(CDCHandle& dc);
	
	void UpdateImpl();

	void NotifyObservers(void (CUOLMessengerReadyForChatNodeObserver::*pfnCallback)(IUOLMessengerReadyForChatNode*, IUOLMessengerImageButton2Ptr), IUOLMessengerImageButton2Ptr);

	// For fixed elements (like status and avatar), returns the element drawing rect.
	// For variable elements, it returns the available rect.
	//
	void GetRectForElement(IUOLMessengerContactListSettings2::ContactNodeElements nContactElement, CRect& rElementRect);
	
private:
#ifdef _LEAKDEBUG
	static LONG ms_nRefs;
#endif

	IUOLMessengerContactPtr	m_pContact;
	CComparatorAdapter*	m_pComparatorAdapter;
	IPainter*			m_pDefaultPainter;
	IPainter*			m_pCurrentPainter;
	IPainter*			m_pSelectionPainter;

	CString		m_strName;
	time_t		m_timeRestore;
    CRect		m_rectStatus;

	CFontElementPtr	m_pFontTextNormal;
	CFontElementPtr m_pFontTextSelected;
	CFontElementPtr m_pFontContactBlocked;
	CFontElementPtr m_pFontTextDropTarget;
	COLORREF		m_clrTextNormal;
	COLORREF		m_clrTextSelected;
	COLORREF		m_clrTextBlocked;
	COLORREF		m_clrTextDropTarget;

	CString		m_strAllContactsGroup;

	CContactTooltipPtr m_pContactTooltip;

	CUOLToolBarPtr m_pToolbar;

	CAtlList<CUOLMessengerReadyForChatNodeObserver*> m_listObservers;

	CComAutoCriticalSection	m_criticalSection;

	CAtlList<IUOLMessengerImageButton2Ptr> m_listButtons;
	CAtlList<IUOLMessengerContextMenuItemPtr> m_listMenus;

    CString         m_strChatKey;
}; 

MAKEAUTOPTR(CReadyForChatNode);