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
 
/*
 Copyright (c) 1999, kSet Lab 
 Author: Konstantin Bukreev 
 E-mail: konstantin@mail.primorye.ru 

 Created: 12/27/1999 2:22:55 PM
 Version: 1.0.0
*/
#include "stdafx.h"
#include "HyperLink.h"

bool CRichEditHyperLink::Open (TCHAR* pLink, int how, HWND hWnd)
{  
	HCURSOR hCursor = SetCursor(LoadCursor(0, IDC_APPSTARTING));

	bool bSuccess = false;
	bSuccess = OpenUsingCom (pLink, how, hWnd);  
	if (!bSuccess)
		bSuccess = OpenUsingShellExecute (pLink, how, hWnd);
	SetCursor(hCursor);
	return bSuccess;
}

bool CRichEditHyperLink::OpenUsingCom (TCHAR* pLink, int how, HWND hWnd)
{  
	CComPtr<IUniformResourceLocator> pURL;	
	HRESULT hr = ::CoCreateInstance(CLSID_InternetShortcut, 0, CLSCTX_INPROC_SERVER, IID_IUniformResourceLocator, (void**)&pURL);
	if (SUCCEEDED(hr))
	{
		hr = pURL->SetURL (pLink, IURL_SETURL_FL_GUESS_PROTOCOL);
		if (SUCCEEDED(hr)) 
		{  
			URLINVOKECOMMANDINFO ivci;
			ivci.dwcbSize = sizeof (URLINVOKECOMMANDINFO);
			ivci.dwFlags = 0;//IURL_INVOKECOMMAND_FL_ALLOW_UI;
			ivci.hwndParent = hWnd;
			
			switch (how)
			{
				case OPEN: ivci.pcszVerb = _T ("open");  break;
				case EDIT: ivci.pcszVerb = _T ("edit");  break;
				case PRINT:ivci.pcszVerb = _T ("print"); break;
				default:  ATLASSERT (0);
			}
			hr = pURL->InvokeCommand (&ivci);
		}
   }	
	return SUCCEEDED(hr);
}

bool CRichEditHyperLink::OpenUsingShellExecute (TCHAR* pLink, int how, HWND hWnd)
{
	LPCTSTR mode = 0;
	switch (how)
	{
		case OPEN: mode = _T ("open");  break;
		case EDIT: mode = _T ("edit");  break;
		case PRINT:  mode = _T ("print");  break;
		default: ATLASSERT (false);      
	}

	SHELLEXECUTEINFO sei; memset(&sei, 0, sizeof SHELLEXECUTEINFO);
	sei.cbSize = sizeof SHELLEXECUTEINFO;
	sei.fMask = SEE_MASK_FLAG_NO_UI ;
	sei.hwnd = hWnd;
	sei.lpVerb = mode;
	sei.lpFile = pLink;
	sei.nShow = SW_SHOW;

	ShellExecuteEx(&sei);
	if ((int)sei.hInstApp > HINSTANCE_ERROR)
		return true;
	else
	{
		TCHAR pResult[MAX_PATH]; memset(pResult, 0, MAX_PATH * sizeof TCHAR);
		if ((int)FindExecutable(pLink, 0, pResult) <= HINSTANCE_ERROR)
			return false;
		
		return (int)ShellExecute(hWnd, mode, pResult, pLink, 0, SW_SHOW) > HINSTANCE_ERROR;
	}  
}

#ifdef _WININET_NEED_
bool CRichEditHyperLink::ParseURL(TCHAR* pURL,	URL_COMPONENTS& urlComponents, DWORD dwFlags)
{	
	TCHAR* pstrCanonicalizedURL = 0;
	TCHAR szCanonicalizedURL[INTERNET_MAX_URL_LENGTH];
	DWORD dwNeededLength = INTERNET_MAX_URL_LENGTH;
	bool bRetVal; 
	bool bMustFree = false;

	DWORD dwCanonicalizeFlags = dwFlags & (ICU_NO_ENCODE | ICU_DECODE | ICU_NO_META | ICU_ENCODE_SPACES_ONLY | ICU_BROWSER_MODE);
	DWORD dwCrackFlags = dwFlags & (ICU_ESCAPE | ICU_USERNAME);

	bRetVal = (InternetCanonicalizeUrl(pURL, szCanonicalizedURL, &dwNeededLength, dwCanonicalizeFlags) != FALSE);
	if (!bRetVal)
	{
		if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)	
			return false;
			
		pstrCanonicalizedURL = new TCHAR[dwNeededLength];
		bMustFree = true;
		bRetVal = (InternetCanonicalizeUrl(pURL, pstrCanonicalizedURL, &dwNeededLength, dwCanonicalizeFlags) != FALSE);
		if (!bRetVal)
		{
			delete [] pstrCanonicalizedURL;
			return false;
		}
	}
	else
		pstrCanonicalizedURL = szCanonicalizedURL;
	
	bRetVal = (InternetCrackUrl(pstrCanonicalizedURL, 0, dwCrackFlags, &urlComponents) != FALSE);
	if (bMustFree) delete [] pstrCanonicalizedURL;	
	return bRetVal;
}

bool CRichEditHyperLink::GetInternetScheme (TCHAR* pURL, INTERNET_SCHEME& nScheme, DWORD dwFlags)
{
	if (!pURL) return false;
	URL_COMPONENTS urlComponents;
	memset(&urlComponents, 0, sizeof(URL_COMPONENTS));
	urlComponents.dwStructSize = sizeof(URL_COMPONENTS);
	bool bRetVal = ParseURL(pURL, urlComponents);
	nScheme = urlComponents.nScheme;	
	return bRetVal;
}
#endif
