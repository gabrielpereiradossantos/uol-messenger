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
#include "EmoticonSettings.h"

#include "SettingsTraits.h"
#include "EmoticonElement.h"
#include "../controller/UIMApplication.h"
#include "../resource.h"
#include <MinDependencySpecialFolder.h>
#include "../controller/SettingsManager.h"


DWORD CEmoticonSettings::VERSION_ATTRIBUTE_DEFAULT_VALUE = 1;


CEmoticonSettings::CEmoticonSettings() : 
	m_userSettingsHandler(""),
	m_bHasUnsavedData(FALSE)
{
	// Initialize emoticon default directory path
	CMinDependencySpecialFolder		minDSF;
	CString	strCompany;
	CString	strProduct;

	strCompany.LoadString(IDS_STRING_COMPANY_NAME);
	strProduct.LoadString(IDS_STRING_PRODUCT_NAME);

	m_strDefaultDirPath.Format(_T("%s%s\\%s"), 
			minDSF.GetCommonAppData(), 
			strCompany, 
			strProduct);

	// Initialize settings processing map
	InitTagProcessMap();

	m_userSettingsHandler.AddSetting("Emoticons", this);
	
	m_strFileVersion.Format("%d", VERSION_ATTRIBUTE_DEFAULT_VALUE);
}


CEmoticonSettings::~CEmoticonSettings()
{
}


void CEmoticonSettings::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"Version", NEW_SETTINGS_CALLBACK(Version, this)},
		{"EmoticonDefaultDirPath", NEW_SETTINGS_CALLBACK(EmoticonDefaultDirPath, this)},
		{"EmoticonSet", NEW_SETTINGS_CALLBACK(EmoticonSet, this)},
	};

	int nSize = sizeof(ih) / sizeof(ih[0]);

	for (int nInd = 0; nInd < nSize; nInd++)
	{
		ms_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}


void CEmoticonSettings::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	m_mapEmoticonSet.RemoveAll();

	CSettingsTraits::Load(pNode, ms_tagProcess, this);
}


void CEmoticonSettings::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, ms_tagProcess, this);
}


void CEmoticonSettings::SetFileVersion(CString strFileVersion)
{
	m_strFileVersion = strFileVersion;
	
	m_bHasUnsavedData = TRUE;
}


CEmoticonSetElementPtr CEmoticonSettings::GetEmoticonSet(const CString& strEmoticonSetName)
{
	return GetEmoticonSetImpl(strEmoticonSetName);
}


void CEmoticonSettings::AddEmoticonSet(CEmoticonSetElementPtr pEmoticonSetElement)
{
	m_mapEmoticonSet.SetAt(pEmoticonSetElement->GetName(), pEmoticonSetElement);
	
	m_bHasUnsavedData = TRUE;
}


void CEmoticonSettings::ReplaceEmoticonSet(CEmoticonSetElementPtr pOldEmoticonSetElement, 
		CEmoticonSetElementPtr pNewEmoticonSetElement)
{
	m_mapEmoticonSet.RemoveKey(pOldEmoticonSetElement->GetName());
	m_mapEmoticonSet.SetAt(pNewEmoticonSetElement->GetName(), pNewEmoticonSetElement);
	
	m_bHasUnsavedData = TRUE;
}


void CEmoticonSettings::AddCustomEmoticonSet()
{
	CEmoticonSetElementPtr pEmoticonSetElement = new CEmoticonSetElement();
	
	if (pEmoticonSetElement)
	{
		pEmoticonSetElement->SetName(GetEmoticonSetNameById(
				IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_CUSTOM));
		
		AddEmoticonSet(pEmoticonSetElement);
		
		m_bHasUnsavedData = TRUE;
	}
}


void CEmoticonSettings::GetEmoticonList(CEmoticonList& listEmoticon)
{
	// Do nothing any more...
	// __declspec(deprecated) method
}


BOOL CEmoticonSettings::IsEmoticonShortcut(const CString& strShortcut)
{
	// Do nothing any more...
	// __declspec(deprecated) method
	
	return FALSE;
}


BOOL CEmoticonSettings::GetEmoticonPath(const CString& strShortcut, CString& strPath)
{
	// Do nothing any more...
	// __declspec(deprecated) method
	
	return FALSE;
}


BOOL CEmoticonSettings::AddEmoticon(const CString& strShortcut, const CString& strPath)
{
	IUOLMessengerEmoticonPtr pNewEmoticon = AddEmoticon2(strShortcut, strPath);
	
	return (pNewEmoticon != NULL);
}


BOOL CEmoticonSettings::RemoveEmoticon(const CString& strShortcut)
{
	IUOLMessengerEmoticonPtr pRemovedEmoticon = RemoveEmoticon2(strShortcut);
	
	return (pRemovedEmoticon != NULL);
}


BOOL CEmoticonSettings::LoadSettings()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	BOOL bLoaded = FALSE;

	BuildUserSettingsPaths();

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_strEmoticonsFilePath = [%s].", m_strEmoticonsFilePath);
	
	m_userSettingsHandler.Init(m_strEmoticonsFilePath, FALSE, FALSE);

	// Try loading user settings
	if (ATLPath::FileExists(m_strEmoticonsFilePath))
	{
		bLoaded = m_userSettingsHandler.LoadSettings(FALSE);
	}

	// If user settings loading failed, load default settings
	if (FALSE == bLoaded)
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogInfo(__FUNCTION__, "First loading try failed.");
		
		// Create directories if necessary
		BOOL bPathExists = TRUE;

		CString strDir;
		int nFind = m_strEmoticonsFilePath.Find('\\');
		while (-1 != nFind)
		{
			strDir = m_strEmoticonsFilePath.Left(nFind);
			bPathExists = ::PathFileExists(strDir);
			
			if (!bPathExists)
			{
				bPathExists = ::CreateDirectory(strDir, NULL);
			}

			nFind = m_strEmoticonsFilePath.Find('\\', nFind+1);
		}

		::CopyFile(GetDefaultSettingsFilePath(), m_strEmoticonsFilePath, FALSE);

		bLoaded = m_userSettingsHandler.LoadSettings(FALSE);
		
		// First time the file is created using the default one, we need 
		// to add the "Custom" emoticon set.
		//
		if (bLoaded)
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogInfo(__FUNCTION__, "Adding custom emoticon set.");
			
			AddCustomEmoticonSet();
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogInfo(__FUNCTION__, "Custom emoticon set added.");
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogDebug(__FUNCTION__, "Second loading try failed.");
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogInfo(__FUNCTION__, "Emoticon settings loaded on first try.");
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bLoaded = [%d].", bLoaded);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bLoaded;
}


BOOL CEmoticonSettings::SaveSettings()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	BOOL bSaveOk = TRUE;
	
	if (m_bHasUnsavedData)
	{
		m_bHasUnsavedData = FALSE;
		
		if (m_mapEmoticonSet.IsEmpty())
		{
			::DeleteFile(m_strEmoticonsFilePath);
			
			bSaveOk = TRUE;
		}
		else
		{
			bSaveOk = m_userSettingsHandler.SaveSettings();
		}
		
		if (bSaveOk)
		{
			CSettingsManagerPtr pSettingsManager = ap_dynamic_cast<CSettingsManagerPtr>(CUIMApplication::GetApplication()->GetSettingsManager());
			
			if (pSettingsManager)
			{
				pSettingsManager->NotifyEmoticonSettingsChanged();
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, 
						"pSettingsManager == NULL.");
			}
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bSaveOk = [%d].", bSaveOk);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bSaveOk;
}


CString CEmoticonSettings::GetDefaultDirPath()
{
	return m_strDefaultDirPath;
}


void CEmoticonSettings::GetEmoticonList2(CAtlList<IUOLMessengerEmoticonPtr>& listEmoticons)
{
	// Do nothing any more...
	// __declspec(deprecated) method
}


IUOLMessengerEmoticonPtr CEmoticonSettings::GetEmoticon2(const CString& strShortcut)
{
	// Do nothing any more...
	// __declspec(deprecated) method
	
	return IUOLMessengerEmoticonPtr();
}


IUOLMessengerEmoticonPtr CEmoticonSettings::AddEmoticon2(const CString& strShortcut, const CString& strPath)
{
	return AddEmoticon(EMOTICON_SET_ID_CUSTOM, strShortcut, strPath);
}


IUOLMessengerEmoticonPtr CEmoticonSettings::RemoveEmoticon2(const CString& strShortcut)
{
	return RemoveEmoticon(EMOTICON_SET_ID_CUSTOM, strShortcut);
}


CString CEmoticonSettings::GetFileVersion()
{
	return m_strFileVersion;
}


BOOL CEmoticonSettings::LoadDefaultSettings()
{
	BOOL bLoaded = FALSE;
	
	m_strEmoticonsFilePath = GetDefaultSettingsFilePath();
	
	m_userSettingsHandler.Init(m_strEmoticonsFilePath, FALSE, FALSE);
	
	bLoaded = m_userSettingsHandler.LoadSettings(FALSE);
	
	return bLoaded;
}


CString CEmoticonSettings::GetEmoticonSetNameById(EmoticonSetId emoticonSetId)
{
	if (emoticonSetId & EMOTICON_SET_ID_ICQ)
	{
		return "ICQ";
	}
	else if (emoticonSetId & EMOTICON_SET_ID_JABBER)
	{
		return "Jabber";
	}
	else if (emoticonSetId & EMOTICON_SET_ID_MSN)
	{
		return "MSN";
	}
	else if (emoticonSetId & EMOTICON_SET_ID_TODAOFERTA)
	{
		return "TodaOferta";
	}
	else if (emoticonSetId & EMOTICON_SET_ID_UOL)
	{
		return "UOL";
	}
	else if (emoticonSetId & EMOTICON_SET_ID_YAHOO)
	{
		return "Yahoo";
	}
	else if (emoticonSetId & EMOTICON_SET_ID_CUSTOM)
	{
		return "Custom";
	}
	
	return "";
}


IUOLMessengerEmoticonSettings3::EmoticonSetId CEmoticonSettings::GetEmoticonSetIdByName(CString strEmoticonSetName)
{
	if (strEmoticonSetName.CompareNoCase("ICQ") == 0)
	{
		return EMOTICON_SET_ID_ICQ;
	}
	else if (strEmoticonSetName.CompareNoCase("Jabber") == 0)
	{
		return EMOTICON_SET_ID_JABBER;
	}
	else if (strEmoticonSetName.CompareNoCase("MSN") == 0)
	{
		return EMOTICON_SET_ID_MSN;
	}
	else if (strEmoticonSetName.CompareNoCase("TodaOferta") == 0)
	{
		return EMOTICON_SET_ID_TODAOFERTA;
	}
	else if (strEmoticonSetName.CompareNoCase("UOL") == 0)
	{
		return EMOTICON_SET_ID_UOL;
	}
	else if (strEmoticonSetName.CompareNoCase("Yahoo") == 0)
	{
		return EMOTICON_SET_ID_YAHOO;
	}
	else if (strEmoticonSetName.CompareNoCase("Custom") == 0)
	{
		return EMOTICON_SET_ID_CUSTOM;
	}
	
	return EMOTICON_SET_ID_INVALID;
}


void CEmoticonSettings::GetEmoticonSetNamesList(CAtlList<CString>& listEmoticonSetNames)
{
	listEmoticonSetNames.RemoveAll();
	
	for (POSITION pos = m_mapEmoticonSet.GetHeadPosition(); pos;)
	{
		listEmoticonSetNames.AddTail(m_mapEmoticonSet.GetKeyAt(pos));
		
		m_mapEmoticonSet.GetNext(pos);
	}
}


CString CEmoticonSettings::GetProtocolIdByEmoticonSetName(const CString& strEmoticonSetName)
{
	IUOLMessengerProtocolManager3Ptr pProtocolManager3 = 
			ap_dynamic_cast<IUOLMessengerProtocolManager3Ptr>(
					CUIMApplication::GetApplication()->GetProtocolManager());
	
	if (pProtocolManager3)
	{
		if (strEmoticonSetName.CompareNoCase("ICQ") == 0)
		{
			return pProtocolManager3->GetProtocolStrId(
					IUOLMessengerProtocolManager3::PROTOCOL_ID_ICQ);
		}
		else if (strEmoticonSetName.CompareNoCase("Jabber") == 0)
		{
			return pProtocolManager3->GetProtocolStrId(
					IUOLMessengerProtocolManager3::PROTOCOL_ID_JABBER);
		}
		else if (strEmoticonSetName.CompareNoCase("MSN") == 0)
		{
			return pProtocolManager3->GetProtocolStrId(
					IUOLMessengerProtocolManager3::PROTOCOL_ID_MSN);
		}
		else if (strEmoticonSetName.CompareNoCase("TodaOferta") == 0)
		{
			return pProtocolManager3->GetProtocolStrId(
					IUOLMessengerProtocolManager3::PROTOCOL_ID_TODAOFERTA);
		}
		else if (strEmoticonSetName.CompareNoCase("UOL") == 0)
		{
			return pProtocolManager3->GetProtocolStrId(
					IUOLMessengerProtocolManager3::PROTOCOL_ID_UOL);
		}
		else if (strEmoticonSetName.CompareNoCase("Yahoo") == 0)
		{
			return pProtocolManager3->GetProtocolStrId(
					IUOLMessengerProtocolManager3::PROTOCOL_ID_YAHOO);
		}
	}
	
	return "";
}


int CEmoticonSettings::GetEmoticonSetIdsByProtocol(const CString& strProtocolId, BOOL bCombineCustomEmoticons)
{
	int nEmoticonSetIds = GetEmoticonSetIdByProtocolStrId(strProtocolId);
	
	if (nEmoticonSetIds != IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_INVALID)
	{
		IUOLMessengerProtocolManagerPtr pProtocolManager = 
				CUIMApplication::GetApplication()->GetProtocolManager();
		
		CIMProtocol5* pIMProtocol5 = NULL;
		
		if (pProtocolManager)
		{
			pIMProtocol5 = dynamic_cast<CIMProtocol5*>(pProtocolManager->GetProtocol(strProtocolId));
		}
		
		if ( (bCombineCustomEmoticons) && (pIMProtocol5) && (pIMProtocol5->HasCustomEmoticonSupport()) )
		{
			nEmoticonSetIds |= EMOTICON_SET_ID_CUSTOM;
		}
	}
	
	return nEmoticonSetIds;
}


void CEmoticonSettings::GetEmoticonList(int comboEmoticonSetIds, CEmoticonList& listEmoticon)
{
	for (POSITION pos = m_mapEmoticonSet.GetHeadPosition(); pos;)
	{
		CEmoticonSetElementMap::CPair* pPair = m_mapEmoticonSet.GetNext(pos);
		
		if (GetEmoticonSetIdByName(pPair->m_key) & comboEmoticonSetIds)
		{
			pPair->m_value->GetEmoticonList(listEmoticon);
		}
	}
}


void CEmoticonSettings::GetEmoticonList(int comboEmoticonSetIds, CAtlList<IUOLMessengerEmoticonPtr>& listEmoticons)
{
	for (POSITION pos = m_mapEmoticonSet.GetHeadPosition(); pos;)
	{
		CEmoticonSetElementMap::CPair* pPair = m_mapEmoticonSet.GetNext(pos);
		
		if (GetEmoticonSetIdByName(pPair->m_key) & comboEmoticonSetIds)
		{
			pPair->m_value->GetEmoticonList(listEmoticons);
		}
	}
}


void CEmoticonSettings::GetEmoticonList(const CString& strProtocolId, CAtlList<IUOLMessengerEmoticonPtr>& listEmoticons)
{
	int nEmoticonSetIds = GetEmoticonSetIdsByProtocol(strProtocolId, TRUE);
	
	if (nEmoticonSetIds != EMOTICON_SET_ID_INVALID)
	{
		GetEmoticonList(nEmoticonSetIds, listEmoticons);
	}
}


IUOLMessengerEmoticonPtr CEmoticonSettings::AddEmoticon(int nEmoticonSetId, const CString& strShortcut, const CString& strPath)
{
	IUOLMessengerEmoticonPtr pNewEmoticon;
	BOOL bAddedToLibpurple = (nEmoticonSetId != EMOTICON_SET_ID_CUSTOM);
	
	// First we try to add the emoticon to libpurple.
	IUOLMessengerAccountPtr pAccount = GetAccount();
	if ( (!bAddedToLibpurple) && (pAccount) && (pAccount->GetProtocol()) )
	{
		CIMProtocol5* pProtocol5 = dynamic_cast<CIMProtocol5*>(pAccount->GetProtocol());
		
		if (pProtocol5)
		{
			IMProtocolError imProtocolError = pProtocol5->AddCustomEmoticon(
					(LPCTSTR) strShortcut, (LPCTSTR) strPath);
			
			if (imProtocolError == IM_PROTOCOL_ERROR_SUCCESS)
			{
				bAddedToLibpurple = TRUE;
			}
		}
	}
	
	CEmoticonSetElementPtr pEmoticonSet = 
			GetEmoticonSetImpl(GetEmoticonSetNameById((EmoticonSetId)nEmoticonSetId));
	
	if ( (bAddedToLibpurple) && (pEmoticonSet) )
	{
		pNewEmoticon = pEmoticonSet->AddEmoticon(strShortcut, strPath);
		
		if (pNewEmoticon)
		{
			m_bHasUnsavedData = TRUE;
		}
	}
	
	return pNewEmoticon;
}


IUOLMessengerEmoticonPtr CEmoticonSettings::RemoveEmoticon(int nEmoticonSetId, const CString& strShortcut)
{
	IUOLMessengerEmoticonPtr pRemovedEmoticon;
	BOOL bDeletedFromLibpurple = (nEmoticonSetId != EMOTICON_SET_ID_CUSTOM);
	
	// First we try to remove the emoticon from libpurple.
	IUOLMessengerAccountPtr pAccount = GetAccount();
	if ( (!bDeletedFromLibpurple) && (pAccount) && (pAccount->GetProtocol()) )
	{
		CIMProtocol5* pProtocol5 = dynamic_cast<CIMProtocol5*>(pAccount->GetProtocol());
		
		if (pProtocol5)
		{
			IMProtocolError imProtocolError = pProtocol5->RemoveCustomEmoticon(
					(LPCTSTR) strShortcut);
			
			if (imProtocolError == IM_PROTOCOL_ERROR_SUCCESS)
			{
				bDeletedFromLibpurple = TRUE;
			}
		}
	}
	
	CEmoticonSetElementPtr pEmoticonSet = 
			GetEmoticonSetImpl(GetEmoticonSetNameById((EmoticonSetId)nEmoticonSetId));
	
	if ( (bDeletedFromLibpurple) && (pEmoticonSet) )
	{
		pRemovedEmoticon = pEmoticonSet->RemoveEmoticon(strShortcut);
		
		if (pRemovedEmoticon)
		{
			m_bHasUnsavedData = TRUE;
		}
	}
	
	return pRemovedEmoticon;
}


IUOLMessengerEmoticonPtr CEmoticonSettings::ChangeEmoticonShortcut(int nEmoticonSetId, const CString& strOldShortcut, const CString& strNewShortcut)
{
	IUOLMessengerEmoticonPtr pChangedEmoticon;
	BOOL bChangedOnLibpurple = (nEmoticonSetId != EMOTICON_SET_ID_CUSTOM);
	
	// First we try to change the emoticon on libpurple.
	IUOLMessengerAccountPtr pAccount = GetAccount();
	if ( (!bChangedOnLibpurple) && (pAccount) && (pAccount->GetProtocol()) )
	{
		CIMProtocol5* pProtocol5 = dynamic_cast<CIMProtocol5*>(pAccount->GetProtocol());
		
		if (pProtocol5)
		{
			IMProtocolError imProtocolError = pProtocol5->ChangeCustomEmoticonShortcut(
					(LPCTSTR) strOldShortcut, (LPCTSTR) strNewShortcut);
			
			if (imProtocolError == IM_PROTOCOL_ERROR_SUCCESS)
			{
				bChangedOnLibpurple = TRUE;
			}
		}
	}
	
	CEmoticonSetElementPtr pEmoticonSet = 
			GetEmoticonSetImpl(GetEmoticonSetNameById((EmoticonSetId)nEmoticonSetId));
	
	if ( (bChangedOnLibpurple) && (pEmoticonSet) )
	{
		pChangedEmoticon = pEmoticonSet->ChangeEmoticonShortcut(strOldShortcut, strNewShortcut);
		
		if (pChangedEmoticon)
		{
			m_bHasUnsavedData = TRUE;
		}
	}
	
	return pChangedEmoticon;
}


IUOLMessengerEmoticonPtr CEmoticonSettings::ChangeEmoticonFilePath(int nEmoticonSetId, const CString& strShortcut, const CString& strNewFilePath)
{
	IUOLMessengerEmoticonPtr pChangedEmoticon;
	BOOL bChangedOnLibpurple = (nEmoticonSetId != EMOTICON_SET_ID_CUSTOM);
	
	// First we try to change the emoticon on libpurple.
	IUOLMessengerAccountPtr pAccount = GetAccount();
	if ( (!bChangedOnLibpurple) && (pAccount) && (pAccount->GetProtocol()) )
	{
		CIMProtocol5* pProtocol5 = dynamic_cast<CIMProtocol5*>(pAccount->GetProtocol());
		
		if (pProtocol5)
		{
			IMProtocolError imProtocolError = pProtocol5->RemoveCustomEmoticon(
					(LPCTSTR) strShortcut);
			
			if (imProtocolError == IM_PROTOCOL_ERROR_SUCCESS)
			{
				imProtocolError = pProtocol5->AddCustomEmoticon(
						(LPCTSTR) strShortcut, (LPCTSTR) strNewFilePath);
			}
			
			if (imProtocolError == IM_PROTOCOL_ERROR_SUCCESS)
			{
				bChangedOnLibpurple = TRUE;
			}
		}
	}
	
	CEmoticonSetElementPtr pEmoticonSet = 
			GetEmoticonSetImpl(GetEmoticonSetNameById((EmoticonSetId)nEmoticonSetId));
	
	if ( (bChangedOnLibpurple) && (pEmoticonSet) )
	{
		pChangedEmoticon = pEmoticonSet->ChangeEmoticonFilePath(strShortcut, strNewFilePath);
		
		if (pChangedEmoticon)
		{
			m_bHasUnsavedData = TRUE;
		}
	}
	
	return pChangedEmoticon;
}


IUOLMessengerEmoticonSettings3::EmoticonSetId CEmoticonSettings::GetEmoticonOwnerId(int comboEmoticonSetIds, const CString& strShortcut)
{
	for (POSITION pos = m_mapEmoticonSet.GetHeadPosition(); pos;)
	{
		CEmoticonSetElementMap::CPair* pPair = m_mapEmoticonSet.GetNext(pos);
		IUOLMessengerEmoticonPtr pEmoticon;
		
		if ( (pPair != NULL) && 
				(GetEmoticonSetIdByName(pPair->m_key) & comboEmoticonSetIds) && 
				((pEmoticon = pPair->m_value->GetEmoticon(strShortcut)) != NULL) )
		{
			return GetEmoticonSetIdByName(pPair->m_key);
		}
	}
	
	return EMOTICON_SET_ID_INVALID;
}


IUOLMessengerEmoticonPtr CEmoticonSettings::GetEmoticon(int comboEmoticonSetIds, const CString& strShortcut)
{
	return SearchEmoticonSetsForEmoticon(comboEmoticonSetIds, strShortcut);
}


BOOL CEmoticonSettings::GetEmoticonPath(int comboEmoticonSetIds, const CString& strShortcut, CString& strPath)
{
	IUOLMessengerEmoticonPtr pEmoticon;
	
	pEmoticon = SearchEmoticonSetsForEmoticon(comboEmoticonSetIds, strShortcut);
	
	if (pEmoticon.get())
	{
		strPath = pEmoticon->GetFilePath();
		return TRUE;
	}
	
	return FALSE;
}


BOOL CEmoticonSettings::IsEmoticonShortcut(int comboEmoticonSetIds, const CString& strShortcut)
{
	IUOLMessengerEmoticonPtr pEmoticon;
	
	pEmoticon = SearchEmoticonSetsForEmoticon(comboEmoticonSetIds, strShortcut);
	
	if (pEmoticon.get())
	{
		return TRUE;
	}
	
	return FALSE;
}


CEmoticonSetElementPtr CEmoticonSettings::GetEmoticonSetImpl(CString emoticonSetName)
{
	CEmoticonSetElementMap::CPair* pPair = m_mapEmoticonSet.Lookup(emoticonSetName);
	
	if (pPair != NULL)
	{
		return pPair->m_value;
	}
	
	return CEmoticonSetElementPtr();
}


IUOLMessengerEmoticonPtr CEmoticonSettings::SearchEmoticonSetsForEmoticon(
		int comboEmoticonSetIds, const CString& strShortcut)
{
	for (POSITION pos = m_mapEmoticonSet.GetHeadPosition(); pos;)
	{
		CEmoticonSetElementMap::CPair* pPair = m_mapEmoticonSet.GetNext(pos);
		IUOLMessengerEmoticonPtr pEmoticon;
		
		if ( (pPair != NULL) && 
				(GetEmoticonSetIdByName(pPair->m_key) & comboEmoticonSetIds) && 
				((pEmoticon = pPair->m_value->GetEmoticon(strShortcut)) != NULL) )
		{
			return pEmoticon;
		}
	}
	
	return IUOLMessengerEmoticonPtr();
}


IMPLEMENT_SETTINGS_CALLBACK(CEmoticonSettings, Version)
{
	if (bSave)
	{
		CSettingsTraits::SetStringAttributeValue(pNode, strKey, m_pThis->m_strFileVersion);
	}
	else
	{
		CSettingsTraits::GetStringAttibuteValue(pNode, strKey, m_pThis->m_strFileVersion);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CEmoticonSettings, EmoticonSet)
{
	CEmoticonSetElementPtr pEmoticonSetElement;
	
	if (bSave)
	{
		for (POSITION pos = m_pThis->m_mapEmoticonSet.GetHeadPosition(); pos;)
		{
			CEmoticonSetElementMap::CPair* pPair = m_pThis->m_mapEmoticonSet.GetNext(pos);
			
			pEmoticonSetElement = pPair->m_value;
			
			MSXML::IXMLDOMNodePtr pChild = CSettingsTraits::OpenGroup(pNode, strKey);
			pEmoticonSetElement->Save(pChild, strKey);
		}
	}
	else
	{
		pEmoticonSetElement = new CEmoticonSetElement();
		
		if (pEmoticonSetElement)
		{
			pEmoticonSetElement->Load(pNode, strKey);
			m_pThis->m_mapEmoticonSet.SetAt(pEmoticonSetElement->GetName(), pEmoticonSetElement);
		}
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CEmoticonSettings, EmoticonDefaultDirPath)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strDefaultDirPath);
	}
	else
	{
		CString strDefaultDirPath;

		CSettingsTraits::GetStringValue(pNode, strKey, strDefaultDirPath);

		if (FALSE == strDefaultDirPath.IsEmpty())
		{
			m_pThis->m_strDefaultDirPath = strDefaultDirPath;
		}
	}
}


void CEmoticonSettings::BuildUserSettingsPaths()
{
	m_strEmoticonsFolderPath.Empty();
	m_strEmoticonsFilePath.Empty();

	IUOLMessengerProfileManagerPtr pProfileManager = CUIMApplication::GetApplication()->GetProfileManager();

	if (pProfileManager != NULL)
	{
		IUOLMessengerProfileElementPtr pProfileElement;

		pProfileManager->GetProfileElement(pProfileManager->GetCurrentProfile(), pProfileElement);
	
		CString strProfilePathWithBackSlash = pProfileElement->GetProfilePath();

		if (strProfilePathWithBackSlash.Right(1) != '\\')
		{
			strProfilePathWithBackSlash += _T("\\");
		}

		CString strEmoticonsFolder;
		strEmoticonsFolder.LoadString(IDS_DIR_EMOTICONS);

		m_strEmoticonsFolderPath = strProfilePathWithBackSlash + strEmoticonsFolder;

		CString strEmoticonsFile;
		strEmoticonsFile.LoadString(IDS_FILE_EMOTICONS_SETTINGS);

		m_strEmoticonsFilePath = strProfilePathWithBackSlash + strEmoticonsFile;
	}
}


CString CEmoticonSettings::GetUserSettingsFilePath()
{
	return m_strEmoticonsFilePath;
}


CString CEmoticonSettings::GetUserSettingsFolderPath()
{
	return m_strEmoticonsFolderPath;
}


CString CEmoticonSettings::GetDefaultSettingsFilePath()
{
	CString strEmoticonsFile;
	strEmoticonsFile.LoadString(IDS_FILE_EMOTICONS_SETTINGS);

	CString strDefaultSettingsFilePath = GetDefaultDirPath() + _T("\\") + strEmoticonsFile;

	return strDefaultSettingsFilePath;
}


int CEmoticonSettings::GetEmoticonSetIdByProtocolStrId(const CString& strProtocolId)
{
	IUOLMessengerProtocolManager3Ptr pProtocolManager3 = 
			ap_dynamic_cast<IUOLMessengerProtocolManager3Ptr>(
					CUIMApplication::GetApplication()->GetProtocolManager());
	
	if (pProtocolManager3)
	{
		if (strProtocolId.Compare(pProtocolManager3->GetProtocolStrId(
				IUOLMessengerProtocolManager3::PROTOCOL_ID_ICQ)) == 0)
		{
			return EMOTICON_SET_ID_ICQ;
		}
		else if (strProtocolId.Compare(pProtocolManager3->GetProtocolStrId(
				IUOLMessengerProtocolManager3::PROTOCOL_ID_JABBER)) == 0)
		{
			return EMOTICON_SET_ID_JABBER;
		}
		else if (strProtocolId.Compare(pProtocolManager3->GetProtocolStrId(
				IUOLMessengerProtocolManager3::PROTOCOL_ID_MSN)) == 0)
		{
			return EMOTICON_SET_ID_MSN;
		}
		else if (strProtocolId.Compare(pProtocolManager3->GetProtocolStrId(
				IUOLMessengerProtocolManager3::PROTOCOL_ID_TODAOFERTA)) == 0)
		{
			return EMOTICON_SET_ID_TODAOFERTA;
		}
		else if (strProtocolId.Compare(pProtocolManager3->GetProtocolStrId(
				IUOLMessengerProtocolManager3::PROTOCOL_ID_UOL)) == 0)
		{
			return EMOTICON_SET_ID_UOL;
		}
		else if (strProtocolId.Compare(pProtocolManager3->GetProtocolStrId(
				IUOLMessengerProtocolManager3::PROTOCOL_ID_YAHOO)) == 0)
		{
			return EMOTICON_SET_ID_YAHOO;
		}
	}
	
	return EMOTICON_SET_ID_INVALID;
}


IUOLMessengerAccountPtr CEmoticonSettings::GetAccount()
{
	IUOLMessengerAccountManagerPtr pAccountManager = 
			CUIMApplication::GetApplication()->GetAccountManager();
	
	if (pAccountManager)
	{
		CAtlList<IUOLMessengerAccountPtr> listAccounts;
		pAccountManager->GetAccountList(listAccounts);
		
		if (listAccounts.GetCount() > 0)
		{
			return listAccounts.GetHead();
		}
	}
	
	return IUOLMessengerAccountPtr();
}

