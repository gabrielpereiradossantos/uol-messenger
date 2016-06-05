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
#include ".\uolwindowmanager.h"

#include "../view/skin/solidbackgroundelement.h"
#include "../view/skin/skingrammar.h"

#include "UIMApplication.h"
#include "../resource.h"


CUOLWindowManager::CUOLWindowManager(void)
{
}

CUOLWindowManager::~CUOLWindowManager(void)
{
}

CUOLWindowManager::LoadError CUOLWindowManager::Load(IUOLMessengerThemePropertiesPtr pThemeProperties)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	CString strFile = pThemeProperties->GetDirPath() + _T("\\skin.xml");
	ATLASSERT(FALSE == strFile.IsEmpty());
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "Theme file = [%s].", strFile);
	
	LoadError leError = CUOLWindowManager::LE_ERROR_SUCCESS;

	CSkinGrammar tg;

	try
	{
		BOOL bParse = tg.Parse(strFile, this);
		
		if (!bParse)
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "Parse failed!!!");
			
			leError = CUOLWindowManager::LE_ERROR_FAILED;
		}
	}
	catch (const CSkinException& se)
	{
		CString strMsgError;

		UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
		ATLASSERT(pStringLoader);
		CString strFormat;
		switch (se.GetHResultError())
		{
			case E_TAG_NOTFOUND:
				strFormat = pStringLoader->GetValue("uol.resource.error.IDS_SKIN_ERROR_TAG_NOTFOUND");
				strMsgError.Format(strFormat, se.GetFilename(), se.GetLine(), se.GetCol());
				break;

			case E_ATTRIBUTE_NOTFOUND:
				strFormat = pStringLoader->GetValue("uol.resource.error.IDS_SKIN_ERROR_ATTRIBUTE_NOTFOUND");
				strMsgError.Format(strFormat, se.GetFilename(), se.GetLine(), se.GetCol());
				break;

			case E_FILE_INVALID:
				strFormat = pStringLoader->GetValue("uol.resource.error.IDS_SKIN_ERROR_FILE_INVALID");
				strMsgError.Format(strFormat, se.GetFilename());
				break;

			case E_ACCESSDENIED:
				strFormat = pStringLoader->GetValue("uol.resource.error.IDS_SKIN_ERROR_ACCESSDENIED");
				strMsgError.Format(strFormat, se.GetFilename());
				break;

			case E_INVALID_VERSION:
				strMsgError = pStringLoader->GetValue("uol.resource.error.IDS_SKIN_ERROR_VERSION");
				break;

			default:
				strMsgError = pStringLoader->GetValue("uol.resource.error.IDS_SKIN_ERROR_GENERAL");
		}

		leError = CUOLWindowManager::LE_DEFAULT_SKIN_PARSE_ERROR;

		CString strMessage;
		CString strCaptionError;

		strCaptionError = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");
		strMessage = strMsgError;

		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"Catched error [%s] when parsing skin of file [%s].", 
				strMessage, strFile);

		ATLASSERT(FALSE == strMessage.IsEmpty());
		MessageBox(NULL, strMessage, strCaptionError, MB_ICONWARNING | MB_OK);
	}

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "leError = %d", leError);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return leError;
}

IElementPtr CUOLWindowManager::GetElement(const CString& strName) const
{
	if (strName.IsEmpty())
	{
		return IElementPtr();
	}

	// licença poética (gambis)
	if (('#' == strName[0]) && (7 == strName.GetLength()))
	{
		CSolidBackgroundElementPtr pSolidBackground;

		try
		{
			pSolidBackground = new CSolidBackgroundElement();

			pSolidBackground->SetColor(strName);
		}
		catch(const CSkinException&)
		{
			ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "background");
		}

		return pSolidBackground;
	}

	const CAtlMap<CString, IElementPtr>::CPair* pPair = m_theSymbolTable.Lookup(strName);

	if (NULL != pPair)
	{
		return pPair->m_value;
	}

	return IElementPtr();
}

void CUOLWindowManager::InsertElement(IElementPtr& pElement)
{
	CString strName = pElement->GetName();	

	if (NULL != GetElement(strName).get())
	{
		throw CSkinException();	
	}

	m_theSymbolTable[strName] = pElement;
}