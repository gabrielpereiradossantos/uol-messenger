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
#include "Cryptography.h"

#include <base64.h>


CCryptography::CCryptography()
{
	m_strKey = _T("#wY9!-|/*");
	m_crw.SetKey(m_strKey);
}

CCryptography::CCryptography(CString strKey)
{
	m_crw.SetKey(strKey);
}

CCryptography::~CCryptography()
{
}

void CCryptography::Encrypt(CString& strValue, int& nLength)
{
	LPTSTR  lpszBuffer;

	m_crw.Encrypt(strValue, &lpszBuffer, &nLength);
	strValue.SetString(lpszBuffer, nLength);
}

bool CCryptography::Decrypt(CString& strValue, int& nLength)
{
	LPTSTR lpszBuffer;
	int nLenPass;

	if (!strValue.IsEmpty())
	{
        nLenPass = (int)strValue.GetAt(0);
	
		if (nLenPass >= 0)
		{
			m_crw.Decrypt(strValue, &lpszBuffer, &nLength);
			strValue.SetString(lpszBuffer, nLength);

			return true;
		}
	}

    return false;
}

void CCryptography::EncodeBase64(CString& strValue, int& nLength)
{
	CString strRet;
	int nRequiredBuffer;
	LPTSTR szBuffer;

	BOOL bOk; 

	if (nLength > 0)
	{
		nRequiredBuffer = ((nLength * 4) / 3) + 4;
		szBuffer = strRet.GetBuffer(nRequiredBuffer + 1);

		bOk = Base64Encode((const BYTE*)(LPCTSTR)strValue, 
								strValue.GetLength(), 
								szBuffer, 
								&nRequiredBuffer, 
								ATL_BASE64_FLAG_NOCRLF);

		szBuffer[nRequiredBuffer] = NULL;
		strRet.ReleaseBuffer();
		strValue.SetString(strRet, nRequiredBuffer);
		nLength = nRequiredBuffer;
	}
}

void CCryptography::DecodeBase64(CString& strValue, int& nLength)
{
	CString strRet;
	int nRequiredBuffer;
	LPTSTR szBuffer;

	BOOL bOk;

	if (nLength > 0)
	{
		nRequiredBuffer = (nLength * 3) / 4 ;
		szBuffer = strRet.GetBuffer(nRequiredBuffer + 1);

		bOk = Base64Decode((LPCTSTR)strValue,
								strValue.GetLength(),
								(BYTE*) szBuffer,
								&nRequiredBuffer);	

		szBuffer[nRequiredBuffer] = NULL;
		strRet.ReleaseBuffer();
		strValue.SetString(strRet, nRequiredBuffer);
		nLength = nRequiredBuffer;
	}
}

BOOL CCryptography::EncodeBase64(unsigned char* pszSrcData, 
		long iSrcSize, CString& strDstData, long& iDstSize)
{
	BOOL bEncodedOk = FALSE;
	CString strRet;
	LPTSTR szBuffer;
	int nRequiredBuffer;
	
	if ( (pszSrcData != NULL) && (iSrcSize > 0) )
	{
		nRequiredBuffer = ((iSrcSize * 4) / 3) + 4;
		szBuffer = strRet.GetBuffer(nRequiredBuffer + 1);
		
		bEncodedOk = Base64Encode(
				pszSrcData, 
				iSrcSize, 
				szBuffer, 
				&nRequiredBuffer, 
				ATL_BASE64_FLAG_NOCRLF);
		
		if (bEncodedOk == TRUE)
		{
			szBuffer[nRequiredBuffer] = NULL;
			strRet.ReleaseBuffer();
			strDstData.SetString(strRet, nRequiredBuffer);
			iDstSize = nRequiredBuffer;
		}
	}
	
	return bEncodedOk;
}

BOOL CCryptography::DecodeBase64(CString strSrcData, 
		long iSrcSize, unsigned char*& pszDstData, long& iDstSize)
{
	BOOL bDencodedOk = FALSE;
	CString strRet;
	int nRequiredBuffer;
	LPTSTR szBuffer;
	
	if ( (iSrcSize > 0) && (!strSrcData.IsEmpty()) )
	{
		nRequiredBuffer = (iSrcSize * 3) / 4 ;
		pszDstData = (unsigned char*)malloc(nRequiredBuffer + 1);
		
		bDencodedOk = Base64Decode((LPCTSTR)strSrcData,
								iSrcSize,
								pszDstData,
								&nRequiredBuffer);	
		
		if (bDencodedOk == TRUE)
		{
			pszDstData[nRequiredBuffer] = NULL;
			iDstSize = nRequiredBuffer;
		}
	}
	
	return bDencodedOk;
}

