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
 
#include "../skin/Element.h"

#include "../DummyWindow.h"
#include "../skin/WindowElement.h"
#include "../skin/FontElement.h"
#include "../skin/COLORREF.h"
#include "NotifyPopUpHyperLink.h"

class CNotifyPopUpCaption;
typedef CWindowElement<CNotifyPopUpCaption, CDummyWindow<CWindowImpl<CNotifyPopUpCaption> > > CNotifyPopUpCaptionBase;


class CNotifyPopUpCaption : public CNotifyPopUpCaptionBase
{
public:
	CNotifyPopUpCaption();
	virtual ~CNotifyPopUpCaption();

	void SetTitle(const CString& strTitle);
	CString GetTitle() const;

	void ShowOptionLink(BOOL bShow);

	int GetHeight() const;

	void GetMinSize(CSize& size);
	CString GetName() const;
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	void DoPaint(CDCHandle dc);

protected:
	BEGIN_MSG_MAP(CNotifyPopUpCaption)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CTLCOLORSTATIC(OnCtlColor)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnMouseDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnMouseDown)
		COMMAND_HANDLER_EX(1, BN_CLICKED, OnCommand)
		COMMAND_HANDLER_EX(2, BN_CLICKED, OnCommand)
		CHAIN_MSG_MAP(CNotifyPopUpCaptionBase)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpcs);
	void OnDestroy();
	HBRUSH OnCtlColor(HDC hdc, HWND hWnd);
	LRESULT OnMouseDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnCommand(UINT code, int id, HWND hWnd);

public:
	static enum
	{
		WM_USER_NOTIFYPOPUPOPTIONSBUTTONDOWN = WM_USER + 0x280,
		WM_USER_NOTIFYPOPUPCLOSEBUTTONDOWN
	};

protected:
	CString					m_strTitle;
	BOOL					m_bShowOptionLink;

	int						m_nHeight;

	CFontElementPtr 		m_pFontTitle;
	CCOLORREF				m_clrTitle;

	CFontElementPtr 		m_pFontLink;
	CCOLORREF				m_clrLink;
	CCOLORREF				m_clrVisitedLink;

	CBrush					m_brush;

	CFont					m_fontTitle;
	CFont					m_fontLink;

	CNotifyPopUpStaticText	m_staticTitle;
	CNotifyPopUpHyperLink	m_linkOption;
	CNotifyPopUpHyperLink	m_linkClose;
};

MAKEAUTOPTR(CNotifyPopUpCaption);