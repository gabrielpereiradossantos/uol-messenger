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
#include "EmoticonParserManager.h"

#include "interfaces/IUOLMessengerSettingsManager.h"
#include "interfaces/IUOLMessengerEmoticonSettings.h"
#include "UIMApplication.h"


CEmoticonParserManager* CEmoticonParserManager::GetInstance(BOOL bCombineCustomEmoticons)
{
	static CEmoticonParserManager parserManager(bCombineCustomEmoticons);
	
	return &parserManager;
}


CEmoticonParserManager::~CEmoticonParserManager()
{
	for (POSITION itemPos = m_mapParsers.GetStartPosition(); itemPos; )
	{
		CParsersMap::CPair* pParsersPair = m_mapParsers.GetNext(itemPos);
		
		delete pParsersPair->m_value;
		pParsersPair->m_value = NULL;
	}
}


CEmoticonParserManager::CEmoticonParserManager(BOOL bCombineCustomEmoticons)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"bCombineCustomEmoticons = [%d]", bCombineCustomEmoticons);
	
	m_bCombineCustomEmoticons = bCombineCustomEmoticons;

	InitializeParsersMap();
	
	IUOLMessengerSettingsManager3Ptr pSettingsManager3 = ap_dynamic_cast<IUOLMessengerSettingsManager3Ptr>(CUIMApplication::GetApplication()->GetSettingsManager());
	
	ATLASSERT(pSettingsManager3);
	if (pSettingsManager3)
	{
		pSettingsManager3->RegisterObserver(this);
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, "pSettingsManager3 == NULL");
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CEmoticonParserManager::InitializeParsersMap()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	IUOLMessengerSettingsManager2Ptr pSettingsManager2 = ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(CUIMApplication::GetApplication()->GetSettingsManager());
	IUOLMessengerEmoticonSettings3Ptr pEmoticonSettings3;
	
	ATLASSERT(pSettingsManager2);
	if (pSettingsManager2)
	{
		pEmoticonSettings3 = ap_dynamic_cast<IUOLMessengerEmoticonSettings3Ptr>(
				pSettingsManager2->GetEmoticonSettings());
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, "pSettingsManager2 == NULL");
	}
	
	ATLASSERT(pEmoticonSettings3);
	if (pEmoticonSettings3)
	{
		CAtlList<CString> listEmoticonSetNames;
		pEmoticonSettings3->GetEmoticonSetNamesList(listEmoticonSetNames);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"Preparing the parser for %d emoticon sets found.", 
				listEmoticonSetNames.GetCount());
		
		for (POSITION pos = listEmoticonSetNames.GetHeadPosition(); pos; )
		{
			CString strEmoticonSetName = listEmoticonSetNames.GetAt(pos);
			CString strProtocolId = 
					pEmoticonSettings3->GetProtocolIdByEmoticonSetName(strEmoticonSetName);
			
			int nEmoticonSetIds = pEmoticonSettings3->GetEmoticonSetIdsByProtocol(
					strProtocolId, m_bCombineCustomEmoticons);
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogDebug(__FUNCTION__, 
					"Emoticon set Id's combination for emoticon set [%s] and protocol [%s] is [%d].", 
					strEmoticonSetName, strProtocolId, nEmoticonSetIds);
			
			if ( (nEmoticonSetIds != IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_INVALID) && 
					(!strProtocolId.IsEmpty()) )
			{
				m_mapParsers[strProtocolId] = new CParse(nEmoticonSetIds, TRUE);
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogWarning(__FUNCTION__, 
						"Could not create parser for emoticon set [%s] and protocol [%s].", 
						strEmoticonSetName, strProtocolId);
			}
			
			listEmoticonSetNames.GetNext(pos);
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, "pEmoticonSettings3 == NULL");
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


CString CEmoticonParserManager::ParseHtmlText(const CString& strProtocolId, const CString& strMessage)
{
	CParsersMap::CPair* pParsersPair = m_mapParsers.Lookup(strProtocolId);
	
	if (pParsersPair)
	{
		return pParsersPair->m_value->ParseHtmlText(strMessage);
	}
	
	return CString("");
}


int CEmoticonParserManager::FindTAGinString(const CString& strProtocolId, TCHAR* str, int& lenTag, CNodeData* node)
{
	CParsersMap::CPair* pParsersPair = m_mapParsers.Lookup(strProtocolId);
	
	if (pParsersPair)
	{
		return pParsersPair->m_value->FindTAGinString(str, lenTag, node);
	}
	
	return -1;
}


CString CEmoticonParserManager::Index2Tag(const CString& strProtocolId, int index)
{
	CParsersMap::CPair* pParsersPair = m_mapParsers.Lookup(strProtocolId);
	
	if (pParsersPair)
	{
		return pParsersPair->m_value->Index2Tag(index);
	}
	
	return CString("");
}


void CEmoticonParserManager::UpdateTags(const CString& strProtocolId)
{
	CParsersMap::CPair* pParsersPair = m_mapParsers.Lookup(strProtocolId);
	
	if (pParsersPair)
	{
		pParsersPair->m_value->UpdateTags();
	}
}


void CEmoticonParserManager::UpdateTags()
{
	for (POSITION itemPos = m_mapParsers.GetStartPosition(); itemPos; )
	{
		CParsersMap::CPair* pParsersPair = m_mapParsers.GetNext(itemPos);
		
		pParsersPair->m_value->UpdateTags();
	}
}


void CEmoticonParserManager::PrepareTags(const CString& strProtocolId)
{
	CParsersMap::CPair* pParsersPair = m_mapParsers.Lookup(strProtocolId);
	
	if (pParsersPair)
	{
		pParsersPair->m_value->PrepareTags();
	}
}


void CEmoticonParserManager::PrepareTags()
{
	for (POSITION itemPos = m_mapParsers.GetStartPosition(); itemPos; )
	{
		CParsersMap::CPair* pParsersPair = m_mapParsers.GetNext(itemPos);
		
		pParsersPair->m_value->PrepareTags();
	}
}


void CEmoticonParserManager::OnEmoticonSettingsChanged()
{
	UpdateTags();
}


void CEmoticonParserManager::UnregisterObserver()
{
	IUOLMessengerSettingsManager3Ptr pSettingsManager3 = ap_dynamic_cast<IUOLMessengerSettingsManager3Ptr>(CUIMApplication::GetApplication()->GetSettingsManager());
	
	ATLASSERT(pSettingsManager3);
	if (pSettingsManager3)
	{
		pSettingsManager3->UnregisterObserver(this);
	}
}

