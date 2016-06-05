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
 * São Paulo SP - CEP 01452-002 - BRASIL 
 *
 *
 * Based on:
 *
 * idispimp.CPP
 * IDispatch for Extending Dynamic HTML Object Model
 *
 * Copyright (c)1995-2000 Microsoft Corporation, All Rights Reserved
 */


#include "stdafx.h"
#include "HistoryCtrlExternalDispatch.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const	WCHAR pszAcceptFile[15] = L"AcceptFile";
const	WCHAR pszRejectFile[15] = L"RejectFile";

#define			DISPID_AcceptFile			676767
#define			DISPID_RejectFile			787878


/*
 * CHistoryCtrlExternalDispatch::CHistoryCtrlExternalDispatch
 * CHistoryCtrlExternalDispatch::~CHistoryCtrlExternalDispatch
 *
 * Parameters (Constructor):
 *  pSite           PCSite of the site we're in.
 *  pUnkOuter       LPUNKNOWN to which we delegate.
 */

CHistoryCtrlExternalDispatch::CHistoryCtrlExternalDispatch(HWND hHostingWnd) : 
		CWMExternalDispImpl(hHostingWnd)
{
	InitInvokeMap();
}


CHistoryCtrlExternalDispatch::~CHistoryCtrlExternalDispatch(void)
{
}


void CHistoryCtrlExternalDispatch::InitInvokeMap()
{
	m_invokeMap["AcceptFile"] = DISPID_AcceptFile;
	m_invokeMap["RejectFile"] = DISPID_RejectFile;
}


HRESULT CHistoryCtrlExternalDispatch::GetDispIDs(
		LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	HRESULT hr = NOERROR;
	
	for (int i = 0; i < cNames; i++)
	{
		CString searchingName(rgszNames[i]);
		
		CInvokeMap::CPair* pInvokePair = m_invokeMap.Lookup(searchingName);
		
		if (pInvokePair)
		{
			rgDispId[i] = pInvokePair->m_value;
		}
		else
		{
			// One or more are unknown so set the return code accordingly.
			hr = ResultFromScode(DISP_E_UNKNOWNNAME);
			rgDispId[i] = DISPID_UNKNOWN;
		}
	}
	
	return hr;
}


//IDispatch
STDMETHODIMP CHistoryCtrlExternalDispatch::GetIDsOfNames(
            /* [in] */ REFIID riid,
            /* [size_is][in] */ OLECHAR** rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID* rgDispId)
{
	return GetDispIDs(rgszNames, cNames, lcid, rgDispId);
}


STDMETHODIMP CHistoryCtrlExternalDispatch::Invoke(
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID /*riid*/,
            /* [in] */ LCID /*lcid*/,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS* pDispParams,
            /* [out] */ VARIANT* pVarResult,
            /* [out] */ EXCEPINFO* /*pExcepInfo*/,
            /* [out] */ UINT* puArgErr)
{
	HRESULT hresult;
	VARIANTARG variantArg0;
	
	if (dispIdMember == DISPID_AcceptFile)
	{
		if (wFlags & DISPATCH_METHOD)
		{
			if ( (pDispParams != NULL) && (pDispParams->cArgs == 1) )
			{
				VariantInit(&variantArg0);
				
				hresult = DispGetParam(pDispParams, 0, VT_I4, &variantArg0, puArgErr);
				
				if(hresult != NOERROR)
				{
					return hresult;
				}
				
				if (m_hHostingWnd)
				{
					::SendMessage(m_hHostingWnd, 
							WM_DISPATCH_ACCEPT_FILE, variantArg0.intVal, 0);
				}
			}
		}
	}
	else if (dispIdMember == DISPID_RejectFile)
	{
		if (wFlags & DISPATCH_METHOD)
		{
			if ( (pDispParams != NULL) && (pDispParams->cArgs == 1) )
			{
				VariantInit(&variantArg0);
				
				hresult = DispGetParam(pDispParams, 0, VT_I4, &variantArg0, puArgErr);
				
				if(hresult != NOERROR)
				{
					return hresult;
				}
				
				if (m_hHostingWnd)
				{
					::SendMessage(m_hHostingWnd, 
							WM_DISPATCH_REJECT_FILE, variantArg0.intVal, 0);
				}
			}
		}
	}
	
	return S_OK;
}

