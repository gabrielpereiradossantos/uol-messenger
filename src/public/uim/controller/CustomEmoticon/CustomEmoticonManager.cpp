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
#include "CustomEmoticonManager.h"



void CCustomEmoticonObserverNotifier::NotifyAll(void (CUOLMessengerCustomEmoticonManagerObserver::*pfnUpdate)(IUOLMessengerCustomEmoticonPtr), IUOLMessengerCustomEmoticonPtr pCustomEmoticon)
{
	CAtlList<CUOLMessengerCustomEmoticonManagerObserver*> listObservers;
	
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);
		
		listObservers.AddTailList(&m_listObservers);
	}
	
	CUOLMessengerCustomEmoticonManagerObserver* pObserver;
	
	for (POSITION pos = listObservers.GetHeadPosition(); pos;)
	{
		pObserver = listObservers.GetNext(pos);
		
		(pObserver->*pfnUpdate)(pCustomEmoticon);
	}
}



CCustomEmoticonManager::CCustomEmoticonManager()
{
	m_customEmoticonSizeCalculatorWindow.Create(NULL, CRect(0, 0, 0, 0), 
			NULL, SS_BITMAP);
	
	m_customEmoticonSizeCalculator.SubclassWindow(
			m_customEmoticonSizeCalculatorWindow.m_hWnd);
}


CCustomEmoticonManager::~CCustomEmoticonManager()
{
	for (POSITION mapPos = m_customEmoticonIdMap.GetStartPosition(); mapPos; )
	{
		CCustomEmoticonIdMap::CPair* pPair = m_customEmoticonIdMap.GetNext(mapPos);
		
		if (pPair)
		{
			pPair->m_value->DestroyPermanently();
		}
	}
	
	m_customEmoticonIdMap.RemoveAll();
	
	if (m_customEmoticonSizeCalculatorWindow.m_hWnd)
	{
		m_customEmoticonSizeCalculatorWindow.DestroyWindow();
	}
}


BOOL CCustomEmoticonManager::SetCustomEmoticonStoreRootPath(const CString& strStoreRootPath)
{
	if ( (!strStoreRootPath.IsEmpty()) && 
			( (ATLPath::IsDirectory(strStoreRootPath)) || (::CreateDirectory(strStoreRootPath, NULL)) ) )
	{
		m_strStoreRootPath = strStoreRootPath;
		
		return TRUE;
	}
	
	return FALSE;
}


BOOL CCustomEmoticonManager::SetDownloadingEmoticonFile(const CString& strDownloadingEmoticonFile)
{
	if ( (!strDownloadingEmoticonFile.IsEmpty()) && 
			(ATLPath::FileExists(strDownloadingEmoticonFile)) )
	{
		m_strDownloadingEmoticonFile = strDownloadingEmoticonFile;
		
		return TRUE;
	}
	
	return FALSE;
}


BOOL CCustomEmoticonManager::SetDownloadingEmoticonSize(const CSize& emoticonSize)
{
	if ( (emoticonSize.cx > 0) && (emoticonSize.cy > 0) )
	{
		m_szDownloadingEmoticon = emoticonSize;
		
		return TRUE;
	}
	
	return FALSE;
}


CString CCustomEmoticonManager::GetDownloadingEmoticonFile()
{
	return m_strDownloadingEmoticonFile;
}


CSize CCustomEmoticonManager::GetDownloadingEmoticonSize()
{
	return m_szDownloadingEmoticon;
}


CCustomEmoticonPtr CCustomEmoticonManager::CreateCustomEmoticon(
		IUOLMessengerConversationPtr pConversation, const CString& strEmoticon)
{
	CCustomEmoticonPtr pNewCustomEmoticon = 
			ap_dynamic_cast<CCustomEmoticonPtr>(FindCustomEmoticon(pConversation, strEmoticon));
	
	if (!pNewCustomEmoticon.get())
	{
		pNewCustomEmoticon = new CCustomEmoticon(pConversation->GetAccount(), 
				pConversation->GetContact(), strEmoticon, m_strStoreRootPath);
		
		if (pNewCustomEmoticon.get())
		{
			CString strEmoticonId = CCustomEmoticon::BuildEmoticonId(pConversation->GetAccount(), 
					pConversation->GetContact(), strEmoticon);
			
			if (!strEmoticonId.IsEmpty())
			{
				if (pNewCustomEmoticon->CreateEmoticonFile())
				{
					m_customEmoticonIdMap[strEmoticonId] = pNewCustomEmoticon;
					
					return pNewCustomEmoticon;
				}
			}
		}
	}
	
	return CCustomEmoticonPtr();
}


BOOL CCustomEmoticonManager::UpdateCustomEmoticonData(CCustomEmoticonPtr pCustomEmoticon, 
		const unsigned char* pEmoticonData, unsigned int nEmoticonDataSize, 
		const CString& strFileExtension)
{
	CCustomEmoticonPtr pSearchingEmoticon = 
			ap_dynamic_cast<CCustomEmoticonPtr>(FindCustomEmoticon(pCustomEmoticon));
	
	if (pSearchingEmoticon.get())
	{
		return pSearchingEmoticon->UpdateEmoticonData(pEmoticonData, 
				nEmoticonDataSize, strFileExtension);
	}
	
	return FALSE;
}


BOOL CCustomEmoticonManager::UpdateCustomEmoticonData(IUOLMessengerConversationPtr pConversation, 
		const CString& strEmoticon, const unsigned char* pEmoticonData, 
		unsigned int nEmoticonDataSize, const CString& strFileExtension)
{
	CCustomEmoticonPtr pSearchingEmoticon = 
			ap_dynamic_cast<CCustomEmoticonPtr>(FindCustomEmoticon(pConversation, strEmoticon));
	
	if (pSearchingEmoticon.get())
	{
		return pSearchingEmoticon->UpdateEmoticonData(pEmoticonData, 
				nEmoticonDataSize, strFileExtension);
	}
	
	return FALSE;
}


BOOL CCustomEmoticonManager::StoreCustomEmoticon(CCustomEmoticonPtr pCustomEmoticon)
{
	CCustomEmoticonPtr pSearchingEmoticon = 
			ap_dynamic_cast<CCustomEmoticonPtr>(FindCustomEmoticon(pCustomEmoticon));
	
	if (pSearchingEmoticon.get())
	{
		BOOL bStoredEmoticon = pSearchingEmoticon->StoreEmoticon();
		
		if (bStoredEmoticon)
		{
			// It's better if that work's, but the notification must be sent anyway.
			//
			if (m_customEmoticonSizeCalculator.Load(pSearchingEmoticon->GetEmoticonPath()) == TRUE)
			{
				SIZE sCustomEmoticonSize = m_customEmoticonSizeCalculator.GetSize();
				
				pSearchingEmoticon->SetEmoticonHeight(sCustomEmoticonSize.cy);
				pSearchingEmoticon->SetEmoticonWidth(sCustomEmoticonSize.cx);
			}
			
			m_observerNotifier.NotifyAll(
					&CUOLMessengerCustomEmoticonManagerObserver::OnCustomEmoticonStored, pSearchingEmoticon);
		}
		
		return bStoredEmoticon;
	}
	
	return FALSE;
}


BOOL CCustomEmoticonManager::StoreCustomEmoticon(IUOLMessengerConversationPtr pConversation, 
												 const CString& strEmoticon)
{
	CCustomEmoticonPtr pSearchingEmoticon = 
			ap_dynamic_cast<CCustomEmoticonPtr>(FindCustomEmoticon(pConversation, strEmoticon));
	
	if (pSearchingEmoticon.get())
	{
		BOOL bStoredEmoticon = pSearchingEmoticon->StoreEmoticon();
		
		if (bStoredEmoticon)
		{
			// It's better if that work's, but the notification must be sent anyway.
			//
			if (m_customEmoticonSizeCalculator.Load(pSearchingEmoticon->GetEmoticonPath()) == TRUE)
			{
				SIZE sCustomEmoticonSize = m_customEmoticonSizeCalculator.GetSize();
				
				pSearchingEmoticon->SetEmoticonHeight(sCustomEmoticonSize.cy);
				pSearchingEmoticon->SetEmoticonWidth(sCustomEmoticonSize.cx);
			}
			
			m_observerNotifier.NotifyAll(
					&CUOLMessengerCustomEmoticonManagerObserver::OnCustomEmoticonStored, pSearchingEmoticon);
		}
		
		return bStoredEmoticon;
	}
	
	return FALSE;
}


BOOL CCustomEmoticonManager::DestroyCustomEmoticon(CCustomEmoticonPtr pCustomEmoticon)
{
	BOOL bDestroyed = FALSE;
	
	CCustomEmoticonPtr pSearchingEmoticon = 
			ap_dynamic_cast<CCustomEmoticonPtr>(FindCustomEmoticon(pCustomEmoticon));
	
	if (!pSearchingEmoticon.get())
	{
		if ((bDestroyed = pSearchingEmoticon->DestroyPermanently()) == TRUE)
		{
			CString strCustomEmoticonId = CCustomEmoticon::BuildEmoticonId(pSearchingEmoticon);
			
			m_customEmoticonIdMap.RemoveKey(strCustomEmoticonId);
		}
	}
	
	return bDestroyed;
}


IUOLMessengerCustomEmoticonPtr CCustomEmoticonManager::FindCustomEmoticon(IUOLMessengerConversationPtr pConversation, const CString& strEmoticon)
{
	return InternalFindCustomEmoticon(pConversation->GetAccount(), pConversation->GetContact(), strEmoticon);
}


IUOLMessengerCustomEmoticonPtr CCustomEmoticonManager::FindCustomEmoticon(IUOLMessengerCustomEmoticonPtr pCustomEmoticon)
{
	return InternalFindCustomEmoticon(pCustomEmoticon->GetEmoticonAccount(), pCustomEmoticon->GetEmoticonContact(), pCustomEmoticon->GetEmoticonShortcut());
}


void CCustomEmoticonManager::GetCustomEmoticonList(IUOLMessengerConversationPtr pConversation, 
		CAtlList<IUOLMessengerCustomEmoticonPtr>& customEmoticonList)
{
	customEmoticonList.RemoveAll();
	
	for (POSITION mapPos = m_customEmoticonIdMap.GetStartPosition(); mapPos; )
	{
		CCustomEmoticonIdMap::CPair* pPair = m_customEmoticonIdMap.GetNext(mapPos);
		
		CString conversationOwnerId = CCustomEmoticon::BuildEmoticonOwnerId(
				pConversation->GetAccount(), pConversation->GetContact());
		
		if ( (pPair) && (pPair->m_key.Find(conversationOwnerId) == 0) )
		{
			customEmoticonList.AddHead(pPair->m_value);
		}
	}
}


BOOL CCustomEmoticonManager::CalculateImagePresentationSize(LONG& lImageHeight, LONG& lImageWidth)
{
	BOOL bChanged = FALSE;
	double divisionFactor = 0.0;
	
	if ( (lImageHeight > IUOLMessengerCustomEmoticon::DEFAULT_CUSTOM_EMOTICON_HEIGHT) || 
			(lImageWidth > IUOLMessengerCustomEmoticon::DEFAULT_CUSTOM_EMOTICON_WIDTH) )
	{
		if (lImageHeight > lImageWidth)
		{
			divisionFactor = static_cast<double>(lImageWidth) / static_cast<double>(lImageHeight);
			
			lImageHeight = IUOLMessengerCustomEmoticon::DEFAULT_CUSTOM_EMOTICON_HEIGHT;
			
			lImageWidth = static_cast<double>(lImageHeight) * divisionFactor;
		}
		else if (lImageHeight < lImageWidth)
		{
			divisionFactor = static_cast<double>(lImageHeight) / static_cast<double>(lImageWidth);
			
			lImageWidth = IUOLMessengerCustomEmoticon::DEFAULT_CUSTOM_EMOTICON_WIDTH;
			
			lImageHeight = static_cast<double>(lImageWidth) * divisionFactor;
		}
		else
		{
			lImageHeight = IUOLMessengerCustomEmoticon::DEFAULT_CUSTOM_EMOTICON_HEIGHT;
			lImageWidth = IUOLMessengerCustomEmoticon::DEFAULT_CUSTOM_EMOTICON_WIDTH;
		}
		
		bChanged = TRUE;
	}
	
	return bChanged;
}


BOOL CCustomEmoticonManager::CalculateImagePresentationSize(LONG& lImageHeight, LONG& lImageWidth, LONG lHeightLimit, LONG lWidthLimit)
{
	BOOL bChanged = FALSE;
	double divisionFactor = 0.0;
	
	if ( (lImageHeight > lHeightLimit) || 
			(lImageWidth > lWidthLimit) )
	{
		if (lImageHeight > lImageWidth)
		{
			divisionFactor = static_cast<double>(lImageWidth) / static_cast<double>(lImageHeight);
			
			lImageHeight = lHeightLimit;
			
			lImageWidth = static_cast<double>(lImageHeight) * divisionFactor;
		}
		else if (lImageHeight < lImageWidth)
		{
			divisionFactor = static_cast<double>(lImageHeight) / static_cast<double>(lImageWidth);
			
			lImageWidth = lWidthLimit;
			
			lImageHeight = static_cast<double>(lImageWidth) * divisionFactor;
		}
		else
		{
			lImageHeight = lHeightLimit;
			lImageWidth = lWidthLimit;
		}
		
		bChanged = TRUE;
	}
	
	return bChanged;
}


void CCustomEmoticonManager::EncodeShortcutToFileName(CString& strFileName)
{
	CCryptography cryptography;
	int nStrLength = strFileName.GetLength();
	
	cryptography.EncodeBase64(strFileName, nStrLength);
}


void CCustomEmoticonManager::DecodeFileNameToShortcut(CString& strShortcut)
{
	CCryptography cryptography;
	int nStrLength = strShortcut.GetLength();
	
	cryptography.DecodeBase64(strShortcut, nStrLength);
}


void CCustomEmoticonManager::RegisterObserver(CUOLMessengerCustomEmoticonManagerObserver* pObserver)
{
	m_observerNotifier.RegisterObserver(pObserver);
}


void CCustomEmoticonManager::UnregisterObserver(CUOLMessengerCustomEmoticonManagerObserver* pObserver)
{
	m_observerNotifier.UnregisterObserver(pObserver);
}


/*
IUOLMessengerCustomEmoticonPtr CCustomEmoticonManager::GetCustomEmoticon(IUOLMessengerConversationPtr pConversation, const CString& strEmoticon)
{
	IUOLMessengerCustomEmoticonPtr pCustomEmoticon = FindCustomEmoticon(pConversation, strEmoticon)
	
	if (FindCustomEmoticon
}
*/


IUOLMessengerCustomEmoticonPtr CCustomEmoticonManager::InternalFindCustomEmoticon(
		IUOLMessengerAccountPtr pAccount, IUOLMessengerContactPtr pContact, const CString& strEmoticon)
{
	CString strEmoticonId = CCustomEmoticon::BuildEmoticonId(pAccount, pContact, strEmoticon);
	
	if (!strEmoticonId.IsEmpty())
	{
		CCustomEmoticonIdMap::CPair* pPair = m_customEmoticonIdMap.Lookup(strEmoticonId);
		
		if (pPair)
		{
			return pPair->m_value;
		}
	}
	
	return IUOLMessengerCustomEmoticonPtr();
}

