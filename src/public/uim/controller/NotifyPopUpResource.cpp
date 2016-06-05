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
#include "NotifyPopUpResource.h"

#include "../view/NotifyPopUp/NotifyPopUpImages.h"

#include "MinDependencySpecialFolder.h"
#include "../resource.h"
#include "UIMApplication.h"
#include "UIManager.h"
#include "diskcache/IDiskCacheManager.h"


CNotifyPopUpResource::CNotifyPopUpResource() : 
	m_pFacade(NULL)
{
}


CNotifyPopUpResource::~CNotifyPopUpResource()
{
}


BOOL CNotifyPopUpResource::Init(IUOLMessengerFacade* pFacade)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	BOOL bInit = FALSE;
	
	m_pFacade = pFacade;
	
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(m_pFacade->GetUIManager());
	CNotifyPopUpImagesPtr pNPUImages = ap_dynamic_cast<CNotifyPopUpImagesPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__NotifyPopUpImagesInstance"));
	
	if (pNPUImages)
	{
		IUOLMessengerImagePtr pDefaultImage = pNPUImages->GetDefaultImage();
		IUOLMessengerImagePtr pOfflineImage = pNPUImages->GetOfflineImage();
		IUOLMessengerImagePtr pOnlineImage = pNPUImages->GetOnlineImage();
		IUOLMessengerImagePtr pNewMessageImage = pNPUImages->GetNewMessageImage();
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"pDefaultImage = [%x], pOfflineImage = [%x], pOnlineImage = [%x] and pNewMessageImage = [%x].", 
				pDefaultImage.get(), pOfflineImage.get(), pOnlineImage.get(), pNewMessageImage.get());
		
		if ( (pDefaultImage != NULL) && (pDefaultImage != NULL) && 
				(pDefaultImage != NULL) && (pDefaultImage != NULL) )
		{
			bInit = TRUE;
			
			m_mapImages.SetAt(NPR_DEFAULT, pDefaultImage);
			m_mapImages.SetAt(NPR_CONTACT_OFFLINE, pOfflineImage);
			m_mapImages.SetAt(NPR_CONTACT_ONLINE, pOnlineImage);
			m_mapImages.SetAt(NPR_NEW_MESSAGE, pNewMessageImage);
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, "pNPUImages == NULL.");
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bInit = [%d]", bInit);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bInit;
}


void CNotifyPopUpResource::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	m_pFacade = NULL;

	m_mapImages.RemoveAll();
	m_mapUrls.RemoveAll();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


IUOLMessengerImagePtr CNotifyPopUpResource::GetImage(IUOLMessengerNotifyPopUpResource::NotifyPopUpResource resource)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csImages);

	CImageMap::CPair* pPair = m_mapImages.Lookup(resource);

	if (pPair)
	{
		return pPair->m_value;
	}

	return IUOLMessengerImagePtr();
}


void CNotifyPopUpResource::OnItemFetched(IDiskCacheEntryPtr pEntry)
{
	IUOLMessengerNotifyPopUpResource::NotifyPopUpResource resource;

	if (GetResource(pEntry->GetUrl(), resource))
	{
		IUOLMessengerImagePtr pImage = m_pFacade->GetUIManager()->LoadImage(pEntry->GetLocalPath());

		if (pImage)
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csImages);

			CImageMap::CPair* pPair = m_mapImages.Lookup(resource);

			if (pPair)
			{
				m_mapImages.RemoveKey(resource);
			}

			m_mapImages.SetAt(resource, pImage);
		}
	}
}


void CNotifyPopUpResource::OnItemFetchError(const CString& strUrl, DWORD dwError)
{
}


BOOL CNotifyPopUpResource::GetResource(const CString& strUrl, IUOLMessengerNotifyPopUpResource::NotifyPopUpResource& resource)
{
	CUrlMap::CPair* pPair;
	for (POSITION pos = m_mapUrls.GetStartPosition(); pos; )
	{
		pPair = m_mapUrls.GetNext(pos);

		if (0 == strUrl.Compare(pPair->m_value))
		{
			resource = pPair->m_key;
			return TRUE;
		}
	}

	return FALSE;
}