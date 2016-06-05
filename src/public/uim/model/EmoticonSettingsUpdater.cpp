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
#include ".\EmoticonSettingsUpdater.h"
#include ".\SettingsFileVersionChecker.h"

#include "..\resource.h"
#include <MinDependencySpecialFolder.h>
#include "../controller/UIMApplication.h"


CEmoticonSettingsUpdater::CEmoticonSettingsUpdater(void)
{
	m_pDefaultEmoticonSettings = new CEmoticonSettings();
	m_pUserEmoticonSettings = new CEmoticonSettings();
}


CEmoticonSettingsUpdater::~CEmoticonSettingsUpdater(void)
{
}


void CEmoticonSettingsUpdater::UpdatePersonalSettings(void)
{
	CString strDefaultFileVersion;
	CString strUserFileVersion;
	BOOL bGotDefaultFileVersion = FALSE;
	BOOL bGotUserFileVersion = FALSE;
	
	BOOL bLoad;
	BOOL hasChanges = FALSE;
	
	CSettingsFileVersionCheckerPtr pDefaultFileChecker = new 
			CSettingsFileVersionChecker("Emoticons", GetDefaultEmoticonFilePath());
	
	CSettingsFileVersionCheckerPtr pUserFileChecker = new 
			CSettingsFileVersionChecker("Emoticons", GetUserEmoticonFilePath());
	
	if ( (pDefaultFileChecker->GetFileLoaded()) && (pUserFileChecker->GetFileLoaded()) )
	{
		bGotDefaultFileVersion = pDefaultFileChecker->GetFileVersion(strDefaultFileVersion);
		bGotUserFileVersion = pUserFileChecker->GetFileVersion(strUserFileVersion);
	}
	
	if ( (!bGotDefaultFileVersion) || (!bGotUserFileVersion) || 
			(strDefaultFileVersion.Compare(strUserFileVersion) <= 0) )
	{
		return;
	}
	
	IUOLMessengerEmoticonSettings3Ptr pDefaultEmoticonSettings = 
			ap_dynamic_cast<IUOLMessengerEmoticonSettings3Ptr>(m_pDefaultEmoticonSettings);
	IUOLMessengerEmoticonSettings3Ptr pUserEmoticonSettings = 
			ap_dynamic_cast<IUOLMessengerEmoticonSettings3Ptr>(m_pUserEmoticonSettings);
	
	bLoad = pDefaultEmoticonSettings->LoadDefaultSettings();
	
	if (bLoad)
	{
		bLoad = pUserEmoticonSettings->LoadSettings();
	}
	
	if (bLoad)
	{
		// Get the EmoticonSet names from the Default file.
		CAtlList<CString> listDefaultEmoticonSetName;
		pDefaultEmoticonSettings->GetEmoticonSetNamesList(listDefaultEmoticonSetName);
		
		for (POSITION emoticonSetNamePos = listDefaultEmoticonSetName.GetHeadPosition(); 
				emoticonSetNamePos; )
		{
			CString strEmoticonSetName = 
					listDefaultEmoticonSetName.GetNext(emoticonSetNamePos);
			
			CEmoticonSetElementPtr pDefaultEmoticonSetElement = 
					m_pDefaultEmoticonSettings->GetEmoticonSet(strEmoticonSetName);
			
			CEmoticonSetElementPtr pUserEmoticonSetElement = 
					m_pUserEmoticonSettings->GetEmoticonSet(strEmoticonSetName);
			
			if (!pUserEmoticonSetElement)
			{
				m_pUserEmoticonSettings->AddEmoticonSet(pDefaultEmoticonSetElement);
				
				hasChanges = TRUE;
			}
			else
			{
				m_pUserEmoticonSettings->ReplaceEmoticonSet(
					pUserEmoticonSetElement, pDefaultEmoticonSetElement);
				
				hasChanges = TRUE;
			}
			
			/*
			// Old merge code.
			// Client wants to update everything.
			//
			else if (pDefaultEmoticonSetElement->GetEmoticonCount() > 
					pUserEmoticonSetElement->GetEmoticonCount())
			{
				CAtlList<IUOLMessengerEmoticonPtr> listDefaultEmoticons;
				pDefaultEmoticonSetElement->GetEmoticonList(listDefaultEmoticons);
				
				for (POSITION emoticonPos = listDefaultEmoticons.GetHeadPosition(); emoticonPos; )
				{
					IUOLMessengerEmoticon2Ptr pDefaultEmoticon = 
							ap_dynamic_cast<IUOLMessengerEmoticon2Ptr>(
									listDefaultEmoticons.GetNext(emoticonPos));
					
					if ( (pDefaultEmoticon) && (!pUserEmoticonSetElement->IsEmoticonShorcut(
							pDefaultEmoticon->GetDefaultShortcut())) )
					{
						pUserEmoticonSetElement->AddEmoticon(pDefaultEmoticon);
						
						hasChanges = TRUE;
					}
				}
			}
			*/
		}
		
		// We need to validate whether the "Custom" emoticon set already 
		// exists on user configuration file.
		// If don't, we must add it.
		//
		CEmoticonSetElementPtr pUserCustomEmoticonSetElement = 
				m_pUserEmoticonSettings->GetEmoticonSet(
						pUserEmoticonSettings->GetEmoticonSetNameById(
								IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_CUSTOM));
		
		if (pUserCustomEmoticonSetElement == NULL)
		{
			m_pUserEmoticonSettings->AddCustomEmoticonSet();
			
			hasChanges = TRUE;
		}
	}
	
	if (hasChanges)
	{
		m_pUserEmoticonSettings->SetFileVersion(
				pDefaultEmoticonSettings->GetFileVersion());
		
		pUserEmoticonSettings->SaveSettings();
	}
}


CString CEmoticonSettingsUpdater::GetDefaultEmoticonFilePath()
{
	// Initialize emoticon default directory path
	CMinDependencySpecialFolder		minDSF;
	CString	strCompany;
	CString	strProduct;
	CString strEmoticonsFile;
	CString strDefaultEmoticonFilePath;
	
	strCompany.LoadString(IDS_STRING_COMPANY_NAME);
	strProduct.LoadString(IDS_STRING_PRODUCT_NAME);
	strEmoticonsFile.LoadString(IDS_FILE_EMOTICONS_SETTINGS);
	
	strDefaultEmoticonFilePath.Format(_T("%s%s\\%s\\%s"), 
			minDSF.GetCommonAppData(), 
			strCompany, 
			strProduct, 
			strEmoticonsFile);
	
	return strDefaultEmoticonFilePath;
}


CString CEmoticonSettingsUpdater::GetUserEmoticonFilePath()
{
	IUOLMessengerProfileManagerPtr pProfileManager = CUIMApplication::GetApplication()->GetProfileManager();
	CString strUserEmoticonFilePath;
	
	if (pProfileManager != NULL)
	{
		IUOLMessengerProfileElementPtr pProfileElement;
		
		pProfileManager->GetProfileElement(pProfileManager->GetCurrentProfile(), pProfileElement);
		
		CString strProfilePathWithBackSlash = pProfileElement->GetProfilePath();
		
		if (strProfilePathWithBackSlash.Right(1) != '\\')
		{
			strProfilePathWithBackSlash += _T("\\");
		}
		
		CString strEmoticonsFile;
		strEmoticonsFile.LoadString(IDS_FILE_EMOTICONS_SETTINGS);
		
		strUserEmoticonFilePath = strProfilePathWithBackSlash + strEmoticonsFile;
	}
	
	return strUserEmoticonFilePath;
}

