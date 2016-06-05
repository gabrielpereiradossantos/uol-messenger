/* UOL Messenger
 * Copyright (c) 2005 Universo Online S/A
 *
 * Direitos Autorais Reservados
 * All rights reserved
 *
 * Este programa � software livre; voc� pode redistribu�-lo e/ou modific�-lo
 * sob os termos da Licen�a P�blica Geral GNU conforme publicada pela Free
 * Software Foundation; tanto a vers�o 2 da Licen�a, como (a seu crit�rio)
 * qualquer vers�o posterior.
 * Este programa � distribu�do na expectativa de que seja �til, por�m,
 * SEM NENHUMA GARANTIA; nem mesmo a garantia impl�cita de COMERCIABILIDADE
 * OU ADEQUA��O A UMA FINALIDADE ESPEC�FICA. Consulte a Licen�a P�blica Geral
 * do GNU para mais detalhes. 
 * Voc� deve ter recebido uma c�pia da Licen�a P�blica Geral do GNU junto
 * com este programa; se n�o, escreva para a Free Software Foundation, Inc.,
 * no endere�o 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA. 
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
 * S�o Paulo SP - CEP 01452-002 - BRASIL  */
 
#pragma once

#include <IEEventsObserver.h>

class CUIMDetailsCtrl : public CWindowImpl<CUIMDetailsCtrl, CAxWindow>,
						public CIEEventsObserver
{
public:
	DECLARE_WND_CLASS_EX("HTMLDetailsWindow", CS_DBLCLKS, COLOR_WINDOW);

	CUIMDetailsCtrl();
	virtual ~CUIMDetailsCtrl();

	void SetInfoText(const CString& strText);

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL);

protected:
	BEGIN_MSG_MAP(CUIMDetailsCtrl)	
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_ERASEBKGND(OnEraseBackground)
		MSG_WM_SIZE(OnSize)
	END_MSG_MAP()

	void OnDestroy();
	LRESULT OnEraseBackground(HDC hDc);
	void OnSize(UINT, CSize);

	STDMETHOD_(void, BeforeNavigate2)(IDispatch* /*pDisp*/, VARIANT* url, VARIANT* /*Flags*/, VARIANT* /*TargetFrameName*/, VARIANT* /*PostData*/, VARIANT* /*Headers*/,  VARIANT_BOOL* Cancel);
	STDMETHOD_(void, DocumentComplete)(IDispatch* /*pDisp*/, VARIANT* URL);

private:
	HRESULT SetInfoTextImpl(CComPtr<IHTMLElement> pBody, const CString& strText);

private:
	CComPtr<IWebBrowser2>		m_pWebBrowser;
	CComPtr<IHTMLDocument2>		m_pDocument;
};