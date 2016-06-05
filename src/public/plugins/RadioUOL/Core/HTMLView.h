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

#include <interfaces/IUOLMessengerFrameWindowView.h>
#include "resource.h"
#include "wtlext.h"
#include "util.h"

class CSampleFrameWindowView;
typedef CWindowImpl<CSampleFrameWindowView> _CSampleFrameWindowViewBase;

class CSampleFrameWindowView : 
	public CWTLAxControl<CWTLIExplorer,IWebBrowser2>,
	public IUOLMessengerFrameWindowView

{
public:
	virtual ~CSampleFrameWindowView();

	BEGIN_MSG_MAP( CWTLIExplorer )
		MESSAGE_HANDLER(WM_CREATE, OnCreate)			
		//MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()


	LRESULT OnCreate(UINT uMsg, WPARAM wParam , LPARAM lParam, BOOL& bHandled);

	// BEGIN_SINK_MAP is optional, you don't need to define or use it if you don't want
	BEGIN_SINK_MAP( CWTLIExplorer )
		//SINK_ENTRY(0, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete2 )
		SINK_ENTRY(0, DISPID_BEFORENAVIGATE2, OnBeforeNavigate2 )
	END_SINK_MAP()


	virtual EVENTFN OnNavigateComplete2( IDispatch* pDisp,  VARIANT* URL );
	virtual EVENTFN OnBeforeNavigate2( 
		IDispatch*		/*pDisp*/,
		VARIANT* url	/*URL*/,
		VARIANT*		/*Flags*/,
		VARIANT*		/*TargetFrameName*/, 
		VARIANT*		/*PostData*/, 
		VARIANT*		/*Headers*/,  
		VARIANT_BOOL*	/*Cancel*/);


protected:
	// IUOLMessengerFrameWindowView implementation
	virtual HWND CreateView(HWND hWndParent);
	virtual void GetMinSize(CSize& size);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnSize(UINT nType, const CSize& size);
	//LRESULT OnCreate(LPCREATESTRUCT lpCreatestruct);
	//void OnDestroy();
};

MAKEAUTOPTR(CSampleFrameWindowView);