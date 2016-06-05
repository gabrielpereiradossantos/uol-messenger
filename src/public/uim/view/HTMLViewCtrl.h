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
 
// HTMLViewCtrl.h : interface of the CHTMLViewCtrl class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../resource.h"
#include "IEEventsObserver.h"


typedef struct _HTMLMESSAGEPARAM
{
	CString strHtml;
	
	_HTMLMESSAGEPARAM& operator= (const _HTMLMESSAGEPARAM& amp)
	{
		strHtml = amp.strHtml;
	
		return *this;
	}
} HTMLMESSAGEPARAM, *LPHTMLMESSAGEPARAM;


// CHTMLViewCtrl Class
class CHTMLViewCtrl : public CWindowImpl<CHTMLViewCtrl, CAxWindow>, 
						    public CIEEventsObserver
{
public:
	enum
	{
		WM_ON_SET_HTML = WM_USER + 0x011
	};

	DECLARE_WND_CLASS_EX("HTMLViewCtrl", CS_DBLCLKS, COLOR_WINDOW);
		
	CHTMLViewCtrl();
	virtual ~CHTMLViewCtrl();

	void SetHtml(const CString& strHtml);
	
	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
				DWORD dwStyle = 0, DWORD dwExStyle = 0,
				_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL);
	
protected:	
	BEGIN_MSG_MAP(CHTMLViewCtrl)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MESSAGE_HANDLER(WM_ON_SET_HTML, OnSetHtml)
	END_MSG_MAP()
	
	void OnDestroy();
	void OnSize(UINT, CSize);
	
	STDMETHOD_(void, DocumentComplete)(IDispatch* /*pDisp*/, VARIANT* URL);
	
	LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSetHtml(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
private:
	CComAutoCriticalSection		m_htmlCS;
	CComPtr<IWebBrowser2>		m_pWebBrowser;
	CComPtr<IHTMLDocument2>		m_pDocument;
	CString						m_strHtml;
};