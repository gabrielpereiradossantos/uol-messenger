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


#include "stdafx.h"
#include "OleClientDocHostWrapper.h"
#include <mshtmcid.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#define IDR_BROWSE_CONTEXT_MENU		24641
#define IDM_EXTRA_ITEM				6047


COleClientDocHostWrapper::COleClientDocHostWrapper(IWebBrowser2* pWebBrowser2, 
			CString strInitialNavigateUrl, IDispatch* pExternalDispatch, 
			ICustomContextMenuHandler* pCustomContextMenuHandler) : 
		m_pDefaultClientSite(NULL), 
		m_pDefaultDocHostUIHandler(NULL), 
		m_pExternalDispatch(NULL), 
		m_pCustomContextMenuHandler(pCustomContextMenuHandler), 
		m_uContextMenuMode(kDefaultMenuSupport), 
		m_cRef(0), 
		m_hCustomMenu(NULL), 
		m_hWndHandler(NULL), 
		m_bLocalLoadedMenu(FALSE)
{
	IOleObject *pIOleObj = NULL;
	
	if (pWebBrowser2 != NULL)
	{
		if (!strInitialNavigateUrl.IsEmpty())
		{
			// Navigate, so the document is created and the IDocHostUIHandler 
			// initialization works fine too, giving us condition to control 
			// menus and etc.
			// Here you can delegate this first Navigation.
			// If a Navigate is issued after IWebBrowser component creation, 
			// this is not needed and "strInitialNavigateUrl" should be empty.
			//
			CComVariant vURL = strInitialNavigateUrl;
			CComVariant vFlags = navNoHistory;
			CComVariant vTarget = _T("_self");
			CComVariant vDummy;
			
			HRESULT hr = pWebBrowser2->Navigate2(&vURL, &vFlags, &vTarget, &vDummy, &vDummy);
			ATLASSERT(SUCCEEDED(hr));
		}
		
		pWebBrowser2->QueryInterface(IID_IOleObject, (void**)&pIOleObj);
	}
	
	if (pIOleObj != NULL)
	{
		IOleClientSite *pOldClientSite = NULL;
		
		if (pIOleObj->GetClientSite(&pOldClientSite) == S_OK)
		{
			SetDefaultClientSite(pOldClientSite);
			pOldClientSite->Release();
		}
		
		pIOleObj->SetClientSite(this);
	}
	
	SetDefaultExternalDispatch(pExternalDispatch);
}


COleClientDocHostWrapper::~COleClientDocHostWrapper()
{
	SetDefaultClientSite(NULL);
	SetDefaultExternalDispatch(NULL);
	
	if ( (m_hCustomMenu) && (m_bLocalLoadedMenu) )
	{
		::DestroyMenu(m_hCustomMenu);
	}
}


VOID COleClientDocHostWrapper::SetDefaultClientSite(IOleClientSite *pClientSite)
{
	if (pClientSite != NULL)
	{
		pClientSite->AddRef();

		m_pDefaultClientSite = pClientSite;
		m_pDefaultClientSite->QueryInterface(IID_IDocHostUIHandler, (VOID **)&m_pDefaultDocHostUIHandler);
	}
	else
	{
		if (m_pDefaultClientSite != NULL)
		{
			m_pDefaultClientSite->Release();
			m_pDefaultClientSite = NULL;
		}

		if (m_pDefaultDocHostUIHandler != NULL)
		{
			m_pDefaultDocHostUIHandler->Release();
			m_pDefaultDocHostUIHandler = NULL;
		}
	}
}


VOID COleClientDocHostWrapper::SetDefaultExternalDispatch(IDispatch* pExternalDispatch)
{
	if (pExternalDispatch)
	{
		pExternalDispatch->AddRef();
		
		m_pExternalDispatch = pExternalDispatch;
	}
	else if (m_pExternalDispatch)
	{
		m_pExternalDispatch->Release();
		
		pExternalDispatch = NULL;
	}
}


VOID COleClientDocHostWrapper::SetContextMenuMode(ULONG inMode, UINT uCustomMenuId)
{
	ATLASSERT( (inMode >= kMenuModeFirst) && (inMode <= kMenuModeLast) );
	
	if ( (inMode >= kMenuModeFirst) && (inMode <= kMenuModeLast) )
	{
		if (inMode != kCustomMenuSupport)
		{
			m_uContextMenuMode = inMode;
		}
		else if (uCustomMenuId != NULL)
		{
			if ( (m_hCustomMenu) && (m_bLocalLoadedMenu) )
			{
				::DestroyMenu(m_hCustomMenu);
			}
			
			m_hCustomMenu = ::LoadMenu(_AtlBaseModule.GetResourceInstance(), 
					MAKEINTRESOURCE(uCustomMenuId));
			
			if (m_hCustomMenu)
			{
				m_uContextMenuMode = inMode;
				m_bLocalLoadedMenu = TRUE;
			}
		}
	}
}


VOID COleClientDocHostWrapper::SetCustomMenuMode(HMENU hCustomMenu, HWND hWndHandler)
{
	if (hCustomMenu != NULL)
	{
		m_uContextMenuMode = kCustomMenuSupport;
		
		m_bLocalLoadedMenu = FALSE;
		
		m_hCustomMenu = hCustomMenu;
		m_hWndHandler = hWndHandler;
	}
}


// *** IUnknown ***


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::QueryInterface(REFIID riid, LPVOID *ppv) 
{
	HRESULT result = S_OK;
	
	// Always set out parameter to NULL, validating it first.
	if (IsBadWritePtr(ppv, sizeof(LPVOID)))
	{
		result = E_INVALIDARG;
	}
	
	if (result == S_OK)
	{
		*ppv = NULL; 
		
		if (IsEqualIID(riid, IID_IUnknown))
		{
			*ppv = this;
		}
		else if (IsEqualIID(riid, IID_IOleClientSite))
		{
			*ppv = (IOleClientSite *) this;
		}
		else if (IsEqualIID(riid, IID_IDocHostUIHandler))
		{
			*ppv = (IDocHostUIHandler *) this;
		}
		else
		{
			result = E_NOINTERFACE;
		}
	}
	
    if (result == S_OK)
	{
		this->AddRef();
	}
	
	return result; 
}


ULONG STDMETHODCALLTYPE COleClientDocHostWrapper::AddRef() 
{    
	InterlockedIncrement(&m_cRef); 
	
	return m_cRef; 
} 
 

ULONG STDMETHODCALLTYPE COleClientDocHostWrapper::Release() 
{ 
	// Decrement the object's internal counter.
	ULONG ulRefCount = InterlockedDecrement(&m_cRef); 
	
	if (0 == m_cRef) 
	{
		delete this; 
	}
	
	return ulRefCount; 
} 


// *** IOleClientSite ***


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::SaveObject()
{
	if (m_pDefaultClientSite != NULL)
	{
		return m_pDefaultClientSite->SaveObject();
	}
	else
	{
		return E_FAIL;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::GetMoniker(DWORD dwAssign, 
		DWORD dwWhichMoniker, LPMONIKER *ppmk)
{
	if (m_pDefaultClientSite != NULL)
	{
		return m_pDefaultClientSite->GetMoniker(dwAssign, dwWhichMoniker, ppmk);
	}
	else
	{
		if (!IsBadWritePtr(ppmk, sizeof(*ppmk)))
		{
			*ppmk = NULL;
		}
		
		return E_NOTIMPL;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::GetContainer(LPOLECONTAINER *ppContainer)
{	
	if (m_pDefaultClientSite != NULL)
	{
		return m_pDefaultClientSite->GetContainer(ppContainer);
	}
	else
	{
		if (!IsBadWritePtr(ppContainer, sizeof(*ppContainer)))
		{
			*ppContainer = NULL;
		}
		
		return E_NOINTERFACE;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::ShowObject()
{
	if (m_pDefaultClientSite != NULL)
	{
		return m_pDefaultClientSite->ShowObject();
	}
	else
	{
		return S_OK;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::OnShowWindow(BOOL fShow)
{
	if (m_pDefaultClientSite != NULL)
	{
		return m_pDefaultClientSite->OnShowWindow(fShow);
	}
	else
	{
		return S_OK;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::RequestNewObjectLayout()
{
	if (m_pDefaultClientSite != NULL)
	{
		return m_pDefaultClientSite->RequestNewObjectLayout();
	}
	else
	{
		return E_NOTIMPL;
	}
}


// *** IDocHostUIHandler ***


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::ShowContextMenu(DWORD dwID, POINT *ppt, 
		IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	HRESULT hResult		= S_FALSE;
	BOOL	bHandled	= FALSE;
	
	switch (m_uContextMenuMode)
	{
		case kDefaultMenuSupport:
		break;
		
		case kNoContextMenu:
			
			hResult	= S_OK;
			bHandled	= TRUE;
			
		break;
		
		case kTextSelectionOnly:
			
			if (dwID != CONTEXT_MENU_TEXTSELECT)
			{
				hResult		= S_OK;
				bHandled	= TRUE;
			}
			
		break;
		
		case kAllowAllButViewSource:
			
			if (dwID == CONTEXT_MENU_DEFAULT)
			{
				hResult		= ModifyContextMenu(dwID, ppt, pcmdtReserved);
				bHandled	= TRUE;
			}
			
		break;
		
		case kCustomMenuSupport:
			
			if (dwID == CONTEXT_MENU_DEFAULT)
			{
				if ( (m_pCustomContextMenuHandler) && 
						(m_pCustomContextMenuHandler->GetCustomContextMenuType() == 
								CONTEXT_MENU_DEFAULT) )
				{
					if (m_pCustomContextMenuHandler->CanShowCustomContextMenu())
					{
						hResult		= CustomContextMenu(ppt, pcmdtReserved);
						bHandled	= TRUE;
					}
				}
				
				if ( (!bHandled) && (m_pCustomContextMenuHandler) && 
						(!m_pCustomContextMenuHandler->CanForwardContextMenuHandling()) )
				{
					// Don't let the context menu be handled further.
					//
					hResult		= S_OK;
					bHandled	= TRUE;
				}
			}
			else if (dwID == CONTEXT_MENU_TEXTSELECT)
			{
				if ( (m_pCustomContextMenuHandler) && 
						(m_pCustomContextMenuHandler->GetCustomContextMenuType() == 
								CONTEXT_MENU_TEXTSELECT) )
				{
					if (m_pCustomContextMenuHandler->CanShowCustomContextMenu())
					{
						hResult		= CustomContextMenu(ppt, pcmdtReserved);
						bHandled	= TRUE;
					}
				}
				
				if ( (!bHandled) && (m_pCustomContextMenuHandler) && 
						(!m_pCustomContextMenuHandler->CanShowTextSelectionOnlyMenu()) && 
						(!m_pCustomContextMenuHandler->CanForwardContextMenuHandling()) )
				{
					// Don't let the context menu be handled further.
					//
					hResult		= S_OK;
					bHandled	= TRUE;
				}
			}
			else if (dwID == CONTEXT_MENU_IMAGE)
			{
				if ( (m_pCustomContextMenuHandler) && 
						(m_pCustomContextMenuHandler->GetCustomContextMenuType() == 
								CONTEXT_MENU_IMAGE) )
				{
					if (m_pCustomContextMenuHandler->CanShowCustomContextMenu())
					{
						hResult		= CustomContextMenu(ppt, pcmdtReserved);
						bHandled	= TRUE;
					}
				}
				
				if ( (!bHandled) && (m_pCustomContextMenuHandler) && 
						(!m_pCustomContextMenuHandler->CanShowImageSelectionOnlyMenu()) && 
						(!m_pCustomContextMenuHandler->CanForwardContextMenuHandling()) )
				{
					// Don't let the context menu be handled further.
					//
					hResult		= S_OK;
					bHandled	= TRUE;
				}
			}
			else if ( (m_pCustomContextMenuHandler) && 
					(!m_pCustomContextMenuHandler->CanForwardContextMenuHandling()) )
			{
				// Don't let the context menu be handled further.
				//
				hResult		= S_OK;
				bHandled	= TRUE;
			}
			
		break;
	}
	
	if (!bHandled)
	{
		if (m_pDefaultDocHostUIHandler != NULL)
		{
			hResult = m_pDefaultDocHostUIHandler->ShowContextMenu(dwID, ppt, 
					pcmdtReserved, pdispReserved);
		}
		else
		{
			hResult = S_FALSE;
		}
	}
	
	return hResult;
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	if (m_pDefaultDocHostUIHandler != NULL)
	{
		return m_pDefaultDocHostUIHandler->GetHostInfo(pInfo);
	}
	else
	{
		return E_NOTIMPL;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::ShowUI(DWORD dwID, 
		IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget,
		IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc)
{
	if (m_pDefaultDocHostUIHandler != NULL)
	{
		return m_pDefaultDocHostUIHandler->ShowUI(dwID, pActiveObject, 
						pCommandTarget, pFrame, pDoc);
	}
	else
	{
		return S_FALSE;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::HideUI()
{
	if (m_pDefaultDocHostUIHandler != NULL)
	{
		return m_pDefaultDocHostUIHandler->HideUI();
	}
	else
	{
		return E_NOTIMPL;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::UpdateUI()
{
	if (m_pDefaultDocHostUIHandler != NULL)
	{
		return m_pDefaultDocHostUIHandler->UpdateUI();
	}
	else
	{
		return E_NOTIMPL;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::EnableModeless(BOOL fEnable)
{
	if (m_pDefaultDocHostUIHandler != NULL)
	{
		return m_pDefaultDocHostUIHandler->EnableModeless(fEnable);
	}
	else
	{
		return E_NOTIMPL;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::OnDocWindowActivate(BOOL fActivate)
{
	if (m_pDefaultDocHostUIHandler != NULL)
	{
		return m_pDefaultDocHostUIHandler->OnDocWindowActivate(fActivate);
	}
	else
	{
		return E_NOTIMPL;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::OnFrameWindowActivate(BOOL fActivate)
{
	if (m_pDefaultDocHostUIHandler != NULL)
	{
		return m_pDefaultDocHostUIHandler->OnFrameWindowActivate(fActivate);
	}
	else
	{
		return E_NOTIMPL;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::ResizeBorder(LPCRECT prcBorder,
		IOleInPlaceUIWindow __RPC_FAR *pUIWindow, BOOL fRameWindow)
{
	if (m_pDefaultDocHostUIHandler != NULL)
	{
		return m_pDefaultDocHostUIHandler->ResizeBorder(prcBorder, pUIWindow, fRameWindow);
	}
	else
	{
		return E_NOTIMPL;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::TranslateAccelerator(LPMSG lpMsg,
		const GUID *pguidCmdGroup, DWORD nCmdID)
{
	if (m_pDefaultDocHostUIHandler != NULL)
	{
		return m_pDefaultDocHostUIHandler->TranslateAccelerator(lpMsg, pguidCmdGroup, nCmdID);
	}
	else
	{
		return E_NOTIMPL;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::GetOptionKeyPath( 
		LPOLESTR *pchKey, DWORD dw)
{
	if (m_pDefaultDocHostUIHandler != NULL)
	{
		return m_pDefaultDocHostUIHandler->GetOptionKeyPath(pchKey, dw);
	}
	else
	{
		if (!IsBadWritePtr(pchKey, sizeof(*pchKey)))
		{
			*pchKey = NULL;
		}
		
		return E_NOTIMPL;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::GetDropTarget( 
		IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
	if (m_pDefaultDocHostUIHandler != NULL)
	{
		HRESULT result = m_pDefaultDocHostUIHandler->GetDropTarget(pDropTarget, ppDropTarget);
		
		//Returning S_FALSE seems to disable DragNDrop, while DragNDrop is by default on.
		//Changing return code to E_FAIL seems to fix things. 
		if (result == S_FALSE)
		{
			result = E_FAIL;
		}
		
		return result;
	}
	else
	{
		if (!IsBadWritePtr(ppDropTarget, sizeof(*ppDropTarget))) 
		{
			*ppDropTarget = NULL;
		}
		
		return E_NOTIMPL;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::GetExternal(IDispatch **ppDispatch)
{
	if ( (m_pExternalDispatch) && (!IsBadWritePtr(ppDispatch, sizeof(*ppDispatch))) )
	{
		m_pExternalDispatch->AddRef();
		
		*ppDispatch = m_pExternalDispatch;
		
		return S_OK;
	}
	else if (m_pDefaultDocHostUIHandler != NULL)
	{
		return m_pDefaultDocHostUIHandler->GetExternal(ppDispatch);
	}
	else
	{
		if (!IsBadWritePtr(ppDispatch, sizeof(*ppDispatch)))
		{
			*ppDispatch = NULL;
		}
		
		return E_NOTIMPL;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::TranslateUrl( 
		DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
	if (m_pDefaultDocHostUIHandler != NULL)
	{
		return m_pDefaultDocHostUIHandler->TranslateUrl(dwTranslate, pchURLIn, ppchURLOut);
	}
	else
	{
		if (!IsBadWritePtr(ppchURLOut, sizeof(*ppchURLOut)))
		{
			*ppchURLOut = NULL;
		}
		
		return S_FALSE;
	}
}


HRESULT STDMETHODCALLTYPE COleClientDocHostWrapper::FilterDataObject( 
		IDataObject *pDO, IDataObject  **ppDORet)
{
	if (m_pDefaultDocHostUIHandler != NULL)
	{
		return m_pDefaultDocHostUIHandler->FilterDataObject(pDO, ppDORet);
	}
	else
	{
		if (!IsBadWritePtr(ppDORet, sizeof(*ppDORet)))
		{
			*ppDORet = NULL;
		}
		
		return S_FALSE;
	}
}


HRESULT COleClientDocHostWrapper::ModifyContextMenu(
		DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved) 
{
    //#define IDR_BROWSE_CONTEXT_MENU  24641
    //#define IDR_FORM_CONTEXT_MENU    24640
    #define SHDVID_GETMIMECSETMENU   27
    #define SHDVID_ADDMENUEXTENSIONS 53
	
    HRESULT hr;
    HINSTANCE hinstSHDOCLC;
    HWND hwnd;
    HMENU hMenu;
    IOleCommandTarget *spCT;
    IOleWindow *spWnd;
    MENUITEMINFO mii = {0};
    VARIANT var, var1, var2;
	
    hr = pcmdtReserved->QueryInterface(IID_IOleCommandTarget, (void**)&spCT);
    hr = pcmdtReserved->QueryInterface(IID_IOleWindow, (void**)&spWnd);
    hr = spWnd->GetWindow(&hwnd);
	
    hinstSHDOCLC = LoadLibrary(TEXT("SHDOCLC.DLL"));
	
    hMenu = LoadMenu(hinstSHDOCLC,
                     MAKEINTRESOURCE(IDR_BROWSE_CONTEXT_MENU));
	
    hMenu = GetSubMenu(hMenu, dwID);
	
    // Get the language submenu
    hr = spCT->Exec(&CGID_ShellDocView, SHDVID_GETMIMECSETMENU, 0, NULL, &var);
	
    mii.cbSize = sizeof(mii);
    mii.fMask  = MIIM_SUBMENU;
    mii.hSubMenu = (HMENU) var.byref;
	
    // Add language submenu to Encoding context item
    SetMenuItemInfo(hMenu, IDM_LANGUAGE, FALSE, &mii);
	
    // Insert Shortcut Menu Extensions from registry
    V_VT(&var1) = VT_PTR;
    V_BYREF(&var1) = hMenu;
	
    V_VT(&var2) = VT_I4;
    V_I4(&var2) = dwID;
	
    hr = spCT->Exec(&CGID_ShellDocView, SHDVID_ADDMENUEXTENSIONS, 0, &var1, &var2);
	
    // Remove View Source
    ::DeleteMenu(hMenu, IDM_VIEWSOURCE, MF_BYCOMMAND);
	// Remove the item that produces the exta separator
	::DeleteMenu(hMenu, IDM_EXTRA_ITEM, MF_BYCOMMAND);
	
    // Show shortcut menu
    int iSelection = ::TrackPopupMenu(hMenu,
			TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
            ppt->x,
            ppt->y,
            0,
            hwnd,
            (RECT*)NULL);
	
    // Send selected shortcut menu item command to shell
	if (iSelection != 0)
	{
		LRESULT lr = ::SendMessage(hwnd, WM_COMMAND, iSelection, NULL);
	}
	
    FreeLibrary(hinstSHDOCLC);
    
	return S_OK;
}


HRESULT COleClientDocHostWrapper::CustomContextMenu(
		POINT *ppt, IUnknown *pcmdtReserved) 
{
	HRESULT		hr				= 0;
	INT			iSelection		= 0;
	
	if ( (m_hCustomMenu != NULL) && (m_hWndHandler) )
	{
		// Show shortcut menu.
		iSelection = ::TrackPopupMenu(m_hCustomMenu,
				TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
				ppt->x,
				ppt->y,
				0,
				m_hWndHandler,
				(RECT*)NULL);
		
		// Send selected shortcut menu item command to shell.
		if (iSelection != 0)
		{
			(void) ::SendMessage(m_hWndHandler, WM_COMMAND, iSelection, NULL);
		}
	}
	
    return S_OK;
}

