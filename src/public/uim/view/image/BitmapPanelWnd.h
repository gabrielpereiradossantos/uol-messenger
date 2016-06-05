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
 
/* Copyright (c) - UOL Inc,
 * Todos os direitos reservados
 *
 * Este arquivo e uma propriedade confidencial do Universo Online Inc.
 * Nenhuma parte do mesmo pode ser copiada, reproduzida, impressa ou
 * transmitida por qualquer meio sem autorizacao expressa e por escrito
 * de um representante legal do Universo Online Inc.
 *
 * All rights reserved
 *
 * This file is a confidential property of Universo Online Inc.
 * No part of this file may be reproduced or copied in any form or by
 * any means without written permission from an authorized person from
 * Universo Online Inc. 
 */

#pragma once

#include "ieeventsobserver.h"


//typedef CWinTraits<WS_CHILD | WS_CLIPCHILDREN, WS_EX_TOPMOST> CTopMostControlWinTraits;

class CBitmapPanelWnd : public CWindowImpl<CBitmapPanelWnd, CAxWindow>,
						public CIEEventsObserver
{
public:
	CBitmapPanelWnd(void);
	virtual ~CBitmapPanelWnd(void);

	BOOL	SetUrl(const CString& strUrl);
	void	SetOfflineUrl(const CString&  strUrl);
	void	SetNotFoundUrlList(const CString& strUrlList);

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL);

protected:
	BEGIN_MSG_MAP(CBitmapPanelWnd)		
		MSG_WM_DESTROY(OnDestroy)
	END_MSG_MAP()

	void OnDestroy();	

	STDMETHOD_(void, NavigateError)(IDispatch* /*pDisp*/, VARIANT* /*URL*/, VARIANT* /*TargetFrameName*/, VARIANT* /*StatusCode*/, VARIANT_BOOL* /*Cancel*/);
	STDMETHOD_(void, DocumentComplete)(IDispatch* /*pDisp*/, VARIANT* URL);

private:
	BOOL Navigate(const CString& strUrl);

private:
	CComPtr<IWebBrowser2>		m_pWebBrowser;
	CString						m_strOfflineUrl;
	CAtlList<CString>			m_UrlNotFoundList;
};
