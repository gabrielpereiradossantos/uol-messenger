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
 * São Paulo SP - CEP 01452-002 - BRASIL */


#ifndef _I_OLE_CLIENT_DOC_HOST_WRAPPER_H_
#define _I_OLE_CLIENT_DOC_HOST_WRAPPER_H_


#include <mshtmhst.h>


enum WebContextMenuMode
{
	kMenuModeFirst = 0,
	kDefaultMenuSupport = 0,
	kNoContextMenu,
	kTextSelectionOnly,
	kAllowAllButViewSource,
	kCustomMenuSupport,
	kMenuModeLast = kCustomMenuSupport
};


__interface ICustomContextMenuHandler
{
	BOOL CanShowCustomContextMenu();
	int  GetCustomContextMenuType();
	BOOL CanShowTextSelectionOnlyMenu();
	BOOL CanShowImageSelectionOnlyMenu();
	BOOL CanForwardContextMenuHandling();
};


class COleClientDocHostWrapper : public IOleClientSite, public IDocHostUIHandler
{
public:
	COleClientDocHostWrapper(IWebBrowser2* pWebBrowser2, 
			CString strInitialNavigateUrl, IDispatch* pExternalDispatch = NULL, 
			ICustomContextMenuHandler* pCustomContextMenuHandler = NULL);
	
	virtual ~COleClientDocHostWrapper();
	
	VOID SetDefaultClientSite(IOleClientSite* pClientSite);
	VOID SetDefaultExternalDispatch(IDispatch* pExternalDispatch);
	
	IOleClientSite* GetDefaultClientSite()
	{
		return m_pDefaultClientSite;
	}
	
	IDocHostUIHandler* GetDefaultDocHostUIHandler()
	{
		return m_pDefaultDocHostUIHandler;
	}
	
	IDispatch* GetDefaultExternalDispatch()
	{
		return m_pExternalDispatch;
	}
	
	ULONG GetContextMenuMode()
	{
		return m_uContextMenuMode;
	}
	
	VOID SetContextMenuMode(ULONG inMode, UINT uCustomMenuId);
	
	VOID SetCustomMenuMode(HMENU hCustomMenu, HWND hWndHandler);
	
// *** IUnknown ***
	
	STDMETHOD(QueryInterface)(
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ VOID **ppvObject);
	
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	
// *** IOleClientSite ***        
	
	STDMETHOD(SaveObject)();
	STDMETHOD(GetMoniker)(
		/* [in] */ DWORD dwAssign,
		/* [in] */ DWORD dwWhichMoniker,
		/* [out] */ IMoniker **ppmk);
	
	STDMETHOD(GetContainer)(
		/* [out] */ IOleContainer **ppContainer);
	
	STDMETHOD(ShowObject)();
	STDMETHOD(OnShowWindow)(
		/* [in] */ BOOL fShow);
	
	STDMETHOD(RequestNewObjectLayout)();
	
// *** IDocHostUIHandler ***
	
	STDMETHOD(ShowContextMenu)( 
		/* [in] */ DWORD dwID,
		/* [in] */ POINT *ppt,
		/* [in] */ IUnknown *pcmdtReserved,
		/* [in] */ IDispatch *pdispReserved);
	STDMETHOD(GetHostInfo)( 
		/* [out][in] */ DOCHOSTUIINFO *pInfo);
	STDMETHOD(ShowUI)( 
		/* [in] */ DWORD dwID,
		/* [in] */ IOleInPlaceActiveObject *pActiveObject,
		/* [in] */ IOleCommandTarget *pCommandTarget,
		/* [in] */ IOleInPlaceFrame *pFrame,
		/* [in] */ IOleInPlaceUIWindow *pDoc);
	STDMETHOD(HideUI)();
	STDMETHOD(UpdateUI)();
	STDMETHOD(EnableModeless)( 
		/* [in] */ BOOL fEnable);
	STDMETHOD(OnDocWindowActivate)( 
		/* [in] */ BOOL fActivate);
	STDMETHOD(OnFrameWindowActivate)( 
		/* [in] */ BOOL fActivate);
	STDMETHOD(ResizeBorder)( 
		/* [in] */ LPCRECT prcBorder,
		/* [in] */ IOleInPlaceUIWindow *pUIWindow,
		/* [in] */ BOOL fRameWindow);
	STDMETHOD(TranslateAccelerator)( 
		/* [in] */ LPMSG lpMsg,
		/* [in] */ const GUID *pguidCmdGroup,
		/* [in] */ DWORD nCmdID);
	STDMETHOD(GetOptionKeyPath)( 
		/* [out] */ LPOLESTR *pchKey,
		/* [in] */ DWORD dw);
	STDMETHOD(GetDropTarget)( 
		/* [in] */ IDropTarget *pDropTarget,
		/* [out] */ IDropTarget **ppDropTarget);
	STDMETHOD(GetExternal)( 
		/* [out] */ IDispatch **ppDispatch);
	STDMETHOD(TranslateUrl)( 
		/* [in] */ DWORD dwTranslate,
		/* [in] */ OLECHAR *pchURLIn,
		/* [out] */ OLECHAR **ppchURLOut);
	STDMETHOD(FilterDataObject)( 
		/* [in] */ IDataObject *pDO,
		/* [out] */ IDataObject **ppDORet);
	
protected:
	
	HRESULT ModifyContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved);
	HRESULT CustomContextMenu(POINT *ppt, IUnknown *pcmdtReserved);
	
private:
	
	LONG				m_cRef;
	
	ULONG				m_uContextMenuMode;
	HMENU				m_hCustomMenu;
	HWND				m_hWndHandler;
	BOOL				m_bLocalLoadedMenu;
	
	IOleClientSite*		m_pDefaultClientSite;
	IDocHostUIHandler*	m_pDefaultDocHostUIHandler;
	
	IDispatch*			m_pExternalDispatch;
	
	ICustomContextMenuHandler*	m_pCustomContextMenuHandler;
};

#endif //_I_OLE_CLIENT_DOC_HOST_WRAPPER_H_

