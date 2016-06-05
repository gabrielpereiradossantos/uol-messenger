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
 
#include "StdAfx.h"
#include "SettingsHandler.h"
#include "SettingsTraits.h"

#include "MinDependencySpecialFolder.h"
#include "../resource.h"


CSettingsHandler::CSettingsHandler(const CString& strPath, BOOL bRelative, BOOL bGlobal)
{
	Init(strPath, bRelative, bGlobal);
}

CSettingsHandler::CSettingsHandler(const CString& fullPath)
{
	Init(fullPath, FALSE, FALSE);
}

CSettingsHandler::~CSettingsHandler(void)
{
	ms_tagProcess.RemoveAll();
}

void CSettingsHandler::Init(const CString& strPath, BOOL bRelative, BOOL bGlobal)
{
	CMinDependencySpecialFolder	minDSF;
	CString						strCompany;
	CString						strProduct;

	strCompany.LoadString(IDS_STRING_COMPANY_NAME);
	strProduct.LoadString(IDS_STRING_PRODUCT_NAME);

	if (m_strRoot.IsEmpty())
	{
		m_strRoot = strProduct;
	}

	if (bRelative)
	{
		if (bGlobal)
			m_strPath.Format("%s%s\\%s\\%s", minDSF.GetCommonAppData(), strCompany, strProduct, strPath);
		else
			m_strPath.Format("%s%s\\%s\\%s", minDSF.GetAppData(), strCompany, strProduct, strPath);
	}
	else
	{
		m_strPath.Format("%s", strPath);
	}
}

void CSettingsHandler::SetRootTag(const CString& strRootTag)
{
	m_strRoot = strRootTag;
}

void CSettingsHandler::AddSetting(const CString& strSetting, ISettings* pSetting)
{
	ms_tagProcess.SetAt(strSetting, pSetting);
}

void CSettingsHandler::RemoveSetting(const CString& strSetting)
{
	ms_tagProcess.RemoveKey(strSetting);
}

BOOL CSettingsHandler::LoadSettings(BOOL bCreate)
{
	MSXML::IXMLDOMDocumentPtr			pXMLDoc;	
	MSXML::IXMLDOMNodePtr				pNode;

	BOOL bSuccess = FALSE;

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	HRESULT hr = pXMLDoc.CreateInstance(__uuidof(MSXML::DOMDocument));

	if (SUCCEEDED(hr))
	{
		pXMLDoc->Putasync(VARIANT_FALSE);
		pXMLDoc->PutpreserveWhiteSpace(VARIANT_TRUE);
	}

	ATLASSERT(!m_strPath.IsEmpty());

	VARIANT_BOOL bResult = pXMLDoc->load(CComVariant(m_strPath)); 

	if (bResult == VARIANT_FALSE && bCreate)
	{
		pNode = pXMLDoc->createElement(m_strRoot.AllocSysString());

		if (NULL != pNode)
		{
			pXMLDoc->appendChild(pNode);
		}
	}

	if (bResult == VARIANT_TRUE || bCreate)
	{
        pNode = pXMLDoc->selectSingleNode(m_strRoot.AllocSysString());
	}

	if (pNode)
	{
		bSuccess = CSettingsTraits::Load(pNode, ms_tagProcess);
		pNode.Release();
	}
	else
	{
		MSXML::IXMLDOMParseErrorPtr pParserError = pXMLDoc->parseError;

		if (pParserError)
		{
			long lineNumber;
			long linePosition;
			long filePosition;
			long errorCode; 
			CComBSTR urlString;
			CComBSTR reasonString;
			CComBSTR sourceString;

			pParserError->get_line(&lineNumber);
			pParserError->get_linepos(&linePosition);
			pParserError->get_filepos(&filePosition);
			pParserError->get_errorCode(&errorCode); 
			pParserError->get_url(&urlString);
			pParserError->get_reason(&reasonString);
			pParserError->get_srcText(&sourceString);
		}
	}

	if (pXMLDoc)
	{
		pXMLDoc.Release();
	}

	CoUninitialize();

	return bSuccess;
}


BOOL CSettingsHandler::SaveSettings()
{
	BOOL bSuccess = FALSE;
	
	MSXML::IXMLDOMDocumentPtr			pXMLDoc;	
	MSXML::IXMLDOMNodePtr				pNode;

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	HRESULT hr = pXMLDoc.CreateInstance(__uuidof(MSXML::DOMDocument));

	if (SUCCEEDED(hr))
	{
		pXMLDoc->Putasync(VARIANT_FALSE);
		pXMLDoc->PutpreserveWhiteSpace(VARIANT_TRUE);
	}

	ATLASSERT(!m_strPath.IsEmpty());

	pNode = pXMLDoc->createElement(m_strRoot.AllocSysString());

	if (NULL != pNode)
	{
		CSettingsTraits::Save(pNode, ms_tagProcess);
		pXMLDoc->appendChild(pNode);
		pNode.Release();
	}
	
	int nFind;
	CString strDir;
	BOOL bPathExists = TRUE;

	nFind = m_strPath.Find('\\');
	while (-1 != nFind)
	{
		strDir = m_strPath.Left(nFind);
		bPathExists = ::PathFileExists(strDir);
		if (!bPathExists)
		{
			bPathExists = ::CreateDirectory(strDir, NULL);
		}
		nFind = m_strPath.Find('\\', nFind+1);
	}

	bSuccess = (S_OK == pXMLDoc->save((CComVariant(m_strPath))));

	if (pXMLDoc)
	{
		pXMLDoc.Release();
	}

	CoUninitialize();

	return bSuccess;
}
