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
 
// ChatInviteDialog.h : interface of the CChatInviteDialog class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../resource.h"
#include "MainDialog.h"
#include "Skin/StaticIconElement.h"
#include "Skin/FontElement.h"
#include "../controller/UIMApplication.h"
#include "TransparentStatic.h"
#include "FindContactPanel.h"
#include "ChatList/ChatInviteListCtrl.h"
#include "ChatList/ChatInviteListCtrlBuilder.h"
#include "FindContactPanelBuilder.h"
#include "ButtonWMEventNotifier.h"

class CChatInviteDialog : 
	public CMainDialog,
	public CFindPanelObserver
{
public:
	CChatInviteDialog(void);
	~CChatInviteDialog(void);

    void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
    void DoPaint(CDCHandle& dc);
	void SetConversation(IUOLMessengerConversation2Ptr pConversation);
	void UpdateLayout();

	//CFindPanelObserver method
    virtual void OnFindPanelTextChanged(CString strFindContactText);
    virtual void OnFindPanelArrowKeyPressed();
	virtual void OnFindPanelEscPressed();

protected:
	BEGIN_MSG_MAP(CChatInviteDialog)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_COMMAND(OnCommand)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_ON_KEYDOWN_EVENT_NOTIFIER, OnKeyDownEventNotifier)
		MESSAGE_HANDLER(WM_ON_KEYUP_EVENT_NOTIFIER, OnKeyUpEventNotifier)
		MESSAGE_HANDLER(WM_ON_CHAR_EVENT_NOTIFIER, OnCharEventNotifier)
		COMMAND_ID_HANDLER(ID_ACCELERATOR_FIND, OnAcceleratorFindContact)
		CHAIN_MSG_MAP(CMainDialog)
	END_MSG_MAP()

	LRESULT OnInitDialog(HWND, LPARAM);
    
	void OnCommand(UINT nId, int nCode, HWND hWnd);
	void DoOK();
	void DoCancel();

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnAcceleratorFindContact(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	// WMEventNotifier
	LRESULT OnKeyDownEventNotifier(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyUpEventNotifier(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCharEventNotifier(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	void CreateFindContactPanel();
	void CreateChatInviteListCtrl();
	void PopulateChatInviteList();
	BOOL HasAtLeastOneContactChecked();
	
private:
	CString							m_strText;
    CFontElementPtr					m_pFontText;
	CFindContactPanelPtr			m_pFindContactPanel;
	CChatInviteListCtrlPtr			m_pChatInviteListCtrl;
	IUOLMessengerConversation2Ptr   m_pConversation;
	CComAutoCriticalSection			m_csLock;
	CButtonWMEventNotifier			m_btnOk;
	CButtonWMEventNotifier			m_btnCancel;
	BOOL							m_bIsCtrlKeyPressed;
};

MAKEAUTOPTR(CChatInviteDialog);
