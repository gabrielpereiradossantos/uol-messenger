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
#include "util.h"
#include <atlpath.h>
#include <atlhttp.h>
#include <atlrx.h>

// DownloadFile
// Return 
// 0 - Ok
// 1 - Download server error
// 2 - Can't write file
// 3 - Internet off

int DownloadFile(const CString & strUrl, 	const CString & strFileName)
{
	CString strFolder = strFileName;
	TCHAR* ptr = strFolder.GetBuffer();
	ATLPath::RemoveFileSpec(ptr);
	strFolder.ReleaseBuffer();
	CAtlHttpClient httpClient;
	CString strUrlFileName;
	strUrlFileName = ATLPath::FindFileName(strUrl);

	if (httpClient.Navigate(strUrl))
	{	
		
		int status = httpClient.GetStatus();
		LPCURL curUrl =( httpClient.GetCurrentUrl());
		CString strCurrentUrl= curUrl->GetUrlPath();
		CString currentFile = ATLPath::FindFileName(strCurrentUrl);
		if (strUrlFileName.CompareNoCase(currentFile)!=0)
		{
			return 1;
		}

		const BYTE* pByte = httpClient.GetBody();
		DWORD dwLen = httpClient.GetBodyLength();

		if (dwLen)
		{	
			CAtlFile file;			

			if (! strFileName.IsEmpty())
			{

				if (FALSE == ATLPath::FileExists(strFolder))
				{
					::CreateDirectory(strFolder, NULL);
				}

//				CString strPluginPath = strDirNameWithBackSlash + strFileName;

				//ATLTRACE("%s %s %d",__FUNCTION__,strUrl,strPluginPath);

				if (SUCCEEDED(file.Create(strFileName, GENERIC_WRITE, 0, CREATE_ALWAYS)))
				{
					
					if (file.Write(pByte, dwLen))
					{
						CRadioUOLError error(CRadioUOLError::SAVECONFIG);
						return 2;
					}

					file.Close();

					return 0;
				}
				else 
				{
					return 2;
				}
			}
		}				
	}

	CString tmp;
	
	int status = httpClient.GetStatus();

	if (status==-1)
	{
		return 3;
	}

	ATLTRACE("%s http status code %d\n",__FUNCTION__, httpClient.GetStatus());

	return 0;
}

//CString GetXMLFilePath() 
//{
//	CString m_current_profile;
//	CString m_path;
//	IUOLMessengerProfileElementPtr pProfileElement;
//	ATLASSERT(CFacadeWrapper::GetFacadeInstance() != NULL);
//	m_current_profile = CFacadeWrapper::GetFacadeInstance()->GetProfileManager()->GetCurrentProfile();
//	CFacadeWrapper::GetFacadeInstance()->GetProfileManager()->GetProfileElement(m_current_profile, pProfileElement);
//	m_path = pProfileElement->GetProfilePath();
//	m_path = m_path + _T("\\tmp_uolradio.jpg");
//	return m_path;
//}

CString GetField(CString source, CString regex) 
{
	char* pszSource = source.GetBuffer();
	char* pszRegex = regex.GetBuffer();

	CAtlRegExp<CAtlRECharTraitsMB> reExp;
	CString tmpString;
	REParseError status = reExp.Parse((unsigned char*)pszRegex);   
	if (REPARSE_ERROR_OK != status)
	{
		ATLASSERT(FALSE);
		source.ReleaseBuffer();
		regex.ReleaseBuffer();
		return "";
	}

	CAtlREMatchContext<CAtlRECharTraitsMB> mcUrl;
	if (!reExp.Match((unsigned char*)pszSource, &mcUrl))
	{
		// Unexpected error.
		//ATLASSERT(FALSE);
		source.ReleaseBuffer();
		regex.ReleaseBuffer();
		return "";
	}

	for (UINT nGroupIndex = 0; nGroupIndex < mcUrl.m_uNumGroups;
		++nGroupIndex)
	{
		const CAtlREMatchContext<CAtlRECharTraitsMB>::RECHAR* szStart = 0;
		const CAtlREMatchContext<CAtlRECharTraitsMB>::RECHAR* szEnd = 0;
		mcUrl.GetMatch(nGroupIndex, &szStart, &szEnd);

		int nLength = (int) (szEnd - szStart);
		//printf("%d: \"%.*s\"\n", nGroupIndex, nLength, szStart);

		tmpString = szStart;
		tmpString = tmpString.Left(nLength);
	}

	source.ReleaseBuffer();
	regex.ReleaseBuffer();

	return tmpString;
}