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
 


#include "stdafx.h"
#include "utils.h"
#include "UIMApplication.h"
#include "../resource.h"
#include <fileversion.h>

#pragma comment(lib, "Urlmon.lib")

CString GetIEUserAgent()
{
	DWORD dwBufferSize = 4096;
	CString strUserAgent;

	HRESULT  hr = ::ObtainUserAgentString(0, strUserAgent.GetBuffer(dwBufferSize + 1), &dwBufferSize);

	strUserAgent.ReleaseBuffer();

	if (FAILED(hr))
	{
		strUserAgent.Empty();
	}

	return strUserAgent;
}


CString GetAppUserAgent()
{
	TCHAR szPath[MAX_PATH];
	CString strPath;
	
	HINSTANCE hInst = ATL::_AtlBaseModule.GetModuleInstance();
	if (::GetModuleFileName(hInst, szPath, MAX_PATH))
	{
		strPath = szPath;
	}
	
	CFileVersion fv(strPath);
	ULONGLONG llVersion = fv.GetVersion();
	USHORT* pVersion = reinterpret_cast<USHORT*>(&llVersion);

	CString strAppUserAgent;

	CString strUserAgent;
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);
	strUserAgent = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");

	CString	strAppVersion;
	strAppVersion.Format(_T("%d.%d.%d.%d"), pVersion[3], pVersion[2], pVersion[1], pVersion[0]);

	CString strIEUserAgent;
	strIEUserAgent = GetIEUserAgent();
	int iIndex = strIEUserAgent.Find(_T("("));
	iIndex = strIEUserAgent.GetLength() - iIndex;
	CString strInfo = strIEUserAgent.Right(iIndex);

	strAppUserAgent.Format(_T("%s/%s %s"), strUserAgent, strAppVersion,strInfo);

	return strAppUserAgent;
}


CString GetDefaultBrowser()
{
	CString strBrowser;

	CRegKey regKey;

	if (ERROR_SUCCESS == regKey.Open(HKEY_CLASSES_ROOT, _T("http\\shell\\open\\command"), KEY_READ))
	{
		DWORD cbData = 0;

		// Get the default value
		if ((ERROR_SUCCESS == regKey.QueryValue(NULL, NULL, NULL, &cbData) ) &&
			(cbData > 0))
		{
			TCHAR* psz = new TCHAR[cbData];

			if (psz != NULL)
			{
				::ZeroMemory(psz, cbData);

				if (ERROR_SUCCESS == regKey.QueryValue(NULL, NULL, (LPBYTE)psz, &cbData))
				{
					strBrowser = psz;
				}
				
				delete[] psz;
			}
		}

		if (strBrowser.GetLength() > 0)
		{
			// Strip the full path from the string
			int nStart = strBrowser.Find('"');
			int nEnd = strBrowser.ReverseFind('"');

			// Do we have either quote?
			// If so, then the path contains spaces
			if (nStart >= 0 && nEnd >= 0)
			{
				// Are they the same?
				if (nStart != nEnd)
				{   
					// Get the full path
					strBrowser = strBrowser.Mid(nStart + 1, nEnd - nStart - 1);
				}
			}
			else
			{
				// We may have a pathname with spaces but
				// no quotes (Netscape), e.g.:
				//   C:\PROGRAM FILES\NETSCAPE\COMMUNICATOR\PROGRAM\NETSCAPE.EXE -h "%1"
				// Look for the last backslash
				int nIndex = strBrowser.ReverseFind('\\');

				if (nIndex > 0)
				{
					// Look for the next space after the final backslash
					int nSpace = strBrowser.Find(' ', nIndex);

					if (nSpace > 0)
					{
						strBrowser = strBrowser.Left(nSpace);
					}
				}
			}
		}
	}

	return strBrowser;
}
