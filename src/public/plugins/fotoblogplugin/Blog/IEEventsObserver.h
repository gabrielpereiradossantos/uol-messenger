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

#include "exdispid.h"
#include "Exdisp.h"

template <UINT nID, class T, const IID* pdiid>
class IDispEventImplHack: public IDispEventImpl<nID, T, pdiid> 
{
private:
	ATLINLINE ATLAPI AtlGetObjectSourceInterface(IUnknown* punkObj, GUID* plibid, IID* piid, unsigned short* pdwMajor, unsigned short* pdwMinor)
	{
		HRESULT hr = E_FAIL;
		if (punkObj != NULL)
		{
			CComPtr<IDispatch> spDispatch;
			hr = punkObj->QueryInterface(__uuidof(IDispatch), (void**)&spDispatch);
			if (SUCCEEDED(hr))
			{
				CComPtr<ITypeInfo> spTypeInfo;
				hr = spDispatch->GetTypeInfo(0, 0, &spTypeInfo);
				if (SUCCEEDED(hr))
				{
					CComPtr<ITypeLib> spTypeLib;
					hr = spTypeInfo->GetContainingTypeLib(&spTypeLib, 0);
					if (SUCCEEDED(hr))
					{
						TLIBATTR* plibAttr;
						hr = spTypeLib->GetLibAttr(&plibAttr);
						if (SUCCEEDED(hr))
						{
							memcpy(plibid, &plibAttr->guid, sizeof(GUID));
							*pdwMajor = plibAttr->wMajorVerNum;
							*pdwMinor = plibAttr->wMinorVerNum;
							spTypeLib->ReleaseTLibAttr(plibAttr);
							// First see if the object is willing to tell us about the
							// default source interface via IProvideClassInfo2
							CComPtr<IProvideClassInfo2> spProvideClassInfo;
							hr = punkObj->QueryInterface(__uuidof(IProvideClassInfo2), (void**)&spProvideClassInfo);
							if (SUCCEEDED(hr) && spProvideClassInfo != NULL)
								hr = spProvideClassInfo->GetGUID(GUIDKIND_DEFAULT_SOURCE_DISP_IID, piid);
							else
							{
								// No, we have to go hunt for it
								CComPtr<ITypeInfo> spInfoCoClass;
								// If we have a clsid, use that
								// Otherwise, try to locate the clsid from IPersist
								CComPtr<IPersist> spPersist;
								CLSID clsid;
								hr = punkObj->QueryInterface(__uuidof(IPersist), (void**)&spPersist);
								if (SUCCEEDED(hr))
								{
									hr = spPersist->GetClassID(&clsid);
									if (SUCCEEDED(hr))
									{
										hr = spTypeLib->GetTypeInfoOfGuid(clsid, &spInfoCoClass);
										if (SUCCEEDED(hr))
										{
											TYPEATTR* pAttr=NULL;
											spInfoCoClass->GetTypeAttr(&pAttr);
											if (pAttr != NULL)
											{
												HREFTYPE hRef;
												for (int i = 0; i < pAttr->cImplTypes; i++)
												{
													int nType;
													hr = spInfoCoClass->GetImplTypeFlags(i, &nType);
													if (SUCCEEDED(hr))
													{
														if (nType == (IMPLTYPEFLAG_FDEFAULT | IMPLTYPEFLAG_FSOURCE))
														{
															// we found it
															hr = spInfoCoClass->GetRefTypeOfImplType(i, &hRef);
															if (SUCCEEDED(hr))
															{
																CComPtr<ITypeInfo> spInfo;
																hr = spInfoCoClass->GetRefTypeInfo(hRef, &spInfo);
																if (SUCCEEDED(hr))
																{
																	TYPEATTR* pAttrIF;
																	spInfo->GetTypeAttr(&pAttrIF);
																	if (pAttrIF != NULL)
																	{
																		memcpy(piid, &pAttrIF->guid, sizeof(GUID));
																	}
																	spInfo->ReleaseTypeAttr(pAttrIF);
																}
															}
															break;
														}
													}
												}
												spInfoCoClass->ReleaseTypeAttr(pAttr);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		return hr;
	}
public:
	HRESULT Advise(IUnknown *punk) 
	{ 
		AtlGetObjectSourceInterface(punk, &m_libid, &m_iid, &m_wMajorVerNum, &m_wMinorVerNum);
		return DispEventAdvise(punk, &m_iid);
	}

	HRESULT Unadvise(IUnknown *punk) 
	{ 
		AtlGetObjectSourceInterface(punk, &m_libid, &m_iid, &m_wMajorVerNum, &m_wMinorVerNum);
		return DispEventUnadvise(punk, &m_iid);
	}
};

static _ATL_FUNC_INFO FileDownloadInfo = {CC_STDCALL, VT_EMPTY, 2, {VT_BOOL, VT_BYREF | VT_BOOL}};

class CIEEventsObserver: public IDispEventImplHack<0, CIEEventsObserver, &__uuidof(DWebBrowserEvents2)> 
{
public:
	CIEEventsObserver(void);
	virtual ~CIEEventsObserver(void);


	BEGIN_SINK_MAP(CIEEventsObserver)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_BEFORENAVIGATE2, BeforeNavigate2)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_CLIENTTOHOSTWINDOW, ClientToHostWindow)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_COMMANDSTATECHANGE, CommandStateChange)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_DOCUMENTCOMPLETE, DocumentComplete)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_DOWNLOADBEGIN, DownloadBegin)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_DOWNLOADCOMPLETE, DownloadComplete)
		SINK_ENTRY_INFO(0, (__uuidof(DWebBrowserEvents2)), DISPID_FILEDOWNLOAD, FileDownload, &FileDownloadInfo)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_NAVIGATECOMPLETE2, NavigateComplete2)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_NAVIGATEERROR, NavigateError)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_NEWWINDOW2, NewWindow2)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_ONFULLSCREEN, OnFullScreen)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_ONADDRESSBAR, OnAddressBar)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_ONMENUBAR, OnMenuBar)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_ONQUIT, OnQuit)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_ONSTATUSBAR, OnStatusBar)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_ONTHEATERMODE, OnTheaterMode)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_ONTOOLBAR, OnToolBar)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_ONVISIBLE, OnVisible)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_PRINTTEMPLATEINSTANTIATION, PrintTemplateInstantiation)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_PRINTTEMPLATETEARDOWN, PrintTemplateTeardown)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_PRIVACYIMPACTEDSTATECHANGE, PrivacyImpactedStateChange)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_PROGRESSCHANGE, ProgressChange)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_PROPERTYCHANGE, PropertyChange)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_SETSECURELOCKICON, SetSecureLockIcon)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_STATUSTEXTCHANGE, StatusTextChange)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_TITLECHANGE, TitleChange)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_WINDOWCLOSING, WindowClosing)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_WINDOWSETHEIGHT, WindowSetHeight)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_WINDOWSETLEFT, WindowSetLeft)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_WINDOWSETRESIZABLE, WindowSetResizable)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_WINDOWSETTOP, WindowSetTop)
		SINK_ENTRY_EX(0, (__uuidof(DWebBrowserEvents2)), DISPID_WINDOWSETWIDTH, WindowSetWidth)
	END_SINK_MAP()

	HRESULT ReceiveEvents(BOOL bReceive, IWebBrowser2* pWebBrowser);	

public:
	//STDMETHOD_(void, BeforeNavigate2)(IDispatch* /*pDisp*/, VARIANT* &/*url*/, VARIANT* &/*Flags*/, VARIANT*& /*TargetFrameName*/, VARIANT*& /*PostData*/, VARIANT*& /*Headers*/,  VARIANT_BOOL*& /*Cancel*/){}
	STDMETHOD_(void, BeforeNavigate2)(IDispatch* /*pDisp*/, VARIANT* /*url*/, VARIANT* /*Flags*/, VARIANT* /*TargetFrameName*/, VARIANT* /*PostData*/, VARIANT* /*Headers*/,  VARIANT_BOOL* /*Cancel*/){}
	//STDMETHOD_(void, ClientToHostWindow)(long*& /*CX*/, long*& /*CY*/){}
	STDMETHOD_(void, ClientToHostWindow)(long* /*CX*/, long* /*CY*/){}
	STDMETHOD_(void, CommandStateChange)(long /*Command*/,VARIANT_BOOL /*Enable*/){}
	STDMETHOD_(void, DocumentComplete)(IDispatch* /*pDisp*/, VARIANT* /*URL*/){}
	STDMETHOD_(void, DownloadBegin)(VOID){}
	STDMETHOD_(void, DownloadComplete)(VOID){}	
	//STDMETHOD_(void, FileDownload)(VARIANT_BOOL* &Cancel){}			// Known problem. See this thread for a workaround: 
	STDMETHOD_(void, FileDownload)(BOOL /*isPageDownload*/, BOOL* /*Cancel*/){} // http://groups.google.com/groups?&threadm=eKwBmo%23%23BHA.2360%40tkmsftngp02
	STDMETHOD_(void, NavigateComplete2)(IDispatch* /*pDisp*/, VARIANT* /*URL*/){}
	//STDMETHOD_(void, NavigateError)(IDispatch* /*pDisp*/, VARIANT* /*URL*/, VARIANT* /*TargetFrameName*/, VARIANT* /*StatusCode*/, VARIANT_BOOL* &/*Cancel*/){}
	STDMETHOD_(void, NavigateError)(IDispatch* /*pDisp*/, VARIANT* /*URL*/, VARIANT* /*TargetFrameName*/, VARIANT* /*StatusCode*/, VARIANT_BOOL* /*Cancel*/){}
	STDMETHOD_(void, NewWindow2)(IDispatch** /*ppDisp*/, VARIANT_BOOL * /*Cancel*/) {}
	STDMETHOD_(void, OnFullScreen)(VARIANT_BOOL /*FullScreen*/){}
	STDMETHOD_(void, OnAddressBar)(VARIANT_BOOL /*AddressBar*/){}
	STDMETHOD_(void, OnMenuBar)(VARIANT_BOOL /*MenuBar*/){}
	STDMETHOD_(void, OnQuit)(VOID){}
	STDMETHOD_(void, OnStatusBar)(VARIANT_BOOL /*StatusBar*/){}
	STDMETHOD_(void, OnTheaterMode)(VARIANT_BOOL /*TheaterMode*/){}
	STDMETHOD_(void, OnToolBar)(VARIANT_BOOL /*ToolBar*/){}
	STDMETHOD_(void, OnVisible)(VARIANT_BOOL /*Visible*/){}
	STDMETHOD_(void, PrintTemplateInstantiation)(IDispatch* /*pDisp*/){}
	STDMETHOD_(void, PrintTemplateTeardown)(IDispatch* /*pDisp*/){}
	STDMETHOD_(void, PrivacyImpactedStateChange)(VARIANT_BOOL /*PrivacyImpacted*/){}
	STDMETHOD_(void, ProgressChange)(long /*Progress*/,long /*ProgressMax*/){}
	STDMETHOD_(void, PropertyChange)(BSTR /*szProperty*/){}
	STDMETHOD_(void, SetSecureLockIcon)(DWORD /*SecureLockIcon*/){}
	STDMETHOD_(void, StatusTextChange)(BSTR /*Text*/){}
	STDMETHOD_(void, TitleChange)(BSTR /*Text*/){}
	//STDMETHOD_(void, WindowClosing)(VARIANT_BOOL /*IsChildWindow*/,VARIANT_BOOL* &/*Cancel*/){}
	STDMETHOD_(void, WindowClosing)(VARIANT_BOOL /*IsChildWindow*/,VARIANT_BOOL* /*Cancel*/){}
	STDMETHOD_(void, WindowSetHeight)(long /*Height*/){}
	STDMETHOD_(void, WindowSetLeft)(long /*Left*/){}
	STDMETHOD_(void, WindowSetResizable)(VARIANT_BOOL /*Resizable*/){}
	STDMETHOD_(void, WindowSetTop)(long /*Top*/){}
	STDMETHOD_(void, WindowSetWidth)(long /*Width*/){}		
};
	