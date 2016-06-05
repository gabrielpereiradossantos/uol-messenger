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
#include "UOLMessengerImagesProtocolImpl.h"
#include "../view/image/ImageBuilder.h"



CUOLMessengerImagesProtocolImpl::CUOLMessengerImagesProtocolImpl()
{
}


//IUOLMessengerImagePtr CUOLMessengerImagesProtocolImpl::GetStatusImage(IUOLMessengerContactPtr contact)
//{
//	CImagesProtocolMap::CPair* pPair;
//
//	pPair = m_mapImagesProtocol.Lookup(contact->GetAccount()->GetProtocolId());
//
//	if (pPair != NULL)
//	{
//		return pPair->m_value->GetImage(contact->GetStatus());
//	}
//
//	return IUOLMessengerImagePtr();
//}


//void CUOLMessengerImagesProtocolImpl::InsertImage(const CString& strProtocolID, DWORD dwStatus, const CString& strFilePath)
//{
//	CImagesProtocolMap::CPair* pPair;
//    CSetImagesProtocolPtr pImagesProtocol;
//
//	pPair = m_mapImagesProtocol.Lookup(strProtocolID);
//
//	if (pPair == NULL)
//	{
//		pImagesProtocol = new CSetImagesProtocol();
//	}
//	else
//	{
//		pImagesProtocol = pPair->m_value;
//	}
//
//	IUOLMessengerImagePtr pImage = UOLMessengerImage::CImageBuilder::LoadImage(strFilePath);
//
//    pImagesProtocol->InsertImage(dwStatus, pImage);
//	m_mapImagesProtocol.SetAt(strProtocolID, pImagesProtocol);
//
//}
//
//
//void CUOLMessengerImagesProtocolImpl::InsertImage(const CString& strProtocolID, DWORD dwStatus, IUOLMessengerImagePtr pImage)
//{
//	CImagesProtocolMap::CPair* pPair;
//    CSetImagesProtocolPtr pImagesProtocol;
//
//	pPair = m_mapImagesProtocol.Lookup(strProtocolID);
//
//	if (pPair == NULL)
//	{
//		pImagesProtocol = new CSetImagesProtocol();
//	}
//	else
//	{
//		pImagesProtocol = pPair->m_value;
//	}
//
//    pImagesProtocol->InsertImage(dwStatus, pImage);
//	m_mapImagesProtocol.SetAt(strProtocolID, pImagesProtocol);
//}
//////////////////////////////////////////////////////////////////////////
// Inserts
//////////////////////////////////////////////////////////////////////////

void CUOLMessengerImagesProtocolImpl::InsertDefaultImage(const CString& strProtocolID, DWORD dwId,IUOLMessengerImagePtr pImage)
{
	CImagesProtocolMap::CPair* pPair;
	CSetImagesProtocolPtr pImagesProtocol;

	pPair = m_mapDefaulImages.Lookup(strProtocolID);

	if (pPair == NULL)
	{
		pImagesProtocol = new CSetImagesProtocol();
	}
	else
	{
		pImagesProtocol = pPair->m_value;
	}

	pImagesProtocol->InsertImage(dwId, pImage);
	m_mapDefaulImages.SetAt(strProtocolID, pImagesProtocol);
}

void CUOLMessengerImagesProtocolImpl::InsertUserImage(const CString& strProtocolID, DWORD dwId, const CString& strPath)
{
	IUOLMessengerImagePtr pImage = UOLMessengerImage::CImageBuilder::LoadImage(strPath,FALSE);
	ATLASSERT(pImage.get());
	CImagesProtocolMap::CPair* pPair;
	CSetImagesProtocolPtr pImagesProtocol;

	pPair = m_mapUserImages.Lookup(strProtocolID);

	if (pPair == NULL)
	{
		pImagesProtocol = new CSetImagesProtocol();
	}
	else
	{
		pImagesProtocol = pPair->m_value;
	}

	pImagesProtocol->InsertImage(dwId, pImage,strPath);
	m_mapUserImages.SetAt(strProtocolID, pImagesProtocol);
}

void CUOLMessengerImagesProtocolImpl::InsertUserStatusImage(const CString& strProtocolID,DWORD dwStatus, IUOLMessengerImagePtr pImage)
{
	CImagesProtocolMap::CPair* pPair;
	CSetImagesProtocolPtr pImagesProtocol;

	pPair = m_mapUserStatusImages.Lookup(strProtocolID);

	if (pPair == NULL)
	{
		pImagesProtocol = new CSetImagesProtocol();
	}
	else
	{
		pImagesProtocol = pPair->m_value;
	}

	pImagesProtocol->InsertImage(dwStatus, pImage);
	m_mapUserStatusImages.SetAt(strProtocolID, pImagesProtocol);
}

void CUOLMessengerImagesProtocolImpl::InsertContactImage(const CString& strProtocolID,DWORD dwId, const CString& strPath)
{
	IUOLMessengerImagePtr pImage = UOLMessengerImage::CImageBuilder::LoadImage(strPath,FALSE);
	ATLASSERT(pImage.get()!=NULL);

	CImagesProtocolMap::CPair* pPair;
	CSetImagesProtocolPtr pImagesProtocol;

	pPair = m_mapContactImages.Lookup(strProtocolID);

	if (pPair == NULL)
	{
		pImagesProtocol = new CSetImagesProtocol();
	}
	else
	{
		pImagesProtocol = pPair->m_value;
	}

	pImagesProtocol->InsertImage(dwId, pImage,strPath);
	m_mapContactImages.SetAt(strProtocolID, pImagesProtocol);
}

void CUOLMessengerImagesProtocolImpl::InsertContactStatusImage(const CString& strProtocolID,DWORD dwStatus, IUOLMessengerImagePtr pImage)
{
	CImagesProtocolMap::CPair* pPair;
	CSetImagesProtocolPtr pImagesProtocol;

	pPair = m_mapContactStatusImages.Lookup(strProtocolID);

	if (pPair == NULL)
	{
		pImagesProtocol = new CSetImagesProtocol();
	}
	else
	{
		pImagesProtocol = pPair->m_value;
	}

	pImagesProtocol->InsertImage(dwStatus, pImage);
	m_mapContactStatusImages.SetAt(strProtocolID, pImagesProtocol);
}

//////////////////////////////////////////////////////////////////////////
// Insert (CString)
//////////////////////////////////////////////////////////////////////////

void CUOLMessengerImagesProtocolImpl::InsertDefaultImage(const CString& strProtocolID, DWORD dwId, const CString& strFilePath)
{
	IUOLMessengerImagePtr pImage = UOLMessengerImage::CImageBuilder::LoadImage(strFilePath);
	InsertDefaultImage(strProtocolID,dwId,pImage);
}

//void CUOLMessengerImagesProtocolImpl::InsertUserImage(const CString& strProtocolID, DWORD dwId,const CString& strFilePath)
//{
//	IUOLMessengerImagePtr pImage = UOLMessengerImage::CImageBuilder::LoadImage(strFilePath);
//	InsertUserImage(strProtocolID,dwId,pImage);
//}

void CUOLMessengerImagesProtocolImpl::InsertUserStatusImage(const CString& strProtocolID,DWORD dwStatus, const CString& strFilePath)
{
	IUOLMessengerImagePtr pImage = UOLMessengerImage::CImageBuilder::LoadImage(strFilePath);
	InsertUserStatusImage(strProtocolID,dwStatus,pImage);
}

//void CUOLMessengerImagesProtocolImpl::InsertContactImage(const CString& strProtocolID,DWORD dwId, const CString& strFilePath)
//{
//	IUOLMessengerImagePtr pImage = UOLMessengerImage::CImageBuilder::LoadImage(strFilePath);
//	InsertContactImage(strProtocolID,dwId,pImage);
//}

void CUOLMessengerImagesProtocolImpl::InsertContactStatusImage(const CString& strProtocolID,DWORD dwStatus, const CString& strFilePath)
{
	IUOLMessengerImagePtr pImage = UOLMessengerImage::CImageBuilder::LoadImage(strFilePath);

	InsertContactStatusImage(strProtocolID,dwStatus,pImage);
}

void CUOLMessengerImagesProtocolImpl::InsertContactBlockedImage(const CString& strProtocolID, IUOLMessengerImagePtr pImage)
{
	m_mapContactBlockedImages.SetAt(strProtocolID, pImage);
}


void CUOLMessengerImagesProtocolImpl::InsertContactAuthorizationIcon(const CString& strProtocolID, IUOLMessengerImagePtr pImage, BOOL bAuthorized)
{
	if (FALSE == bAuthorized)
	{
		CImagesAuthorizationMap::CPair* pPair;

		pPair = m_mapContactNotAuthorizedImages.Lookup(strProtocolID);

		if (pPair != NULL)
		{
			m_mapContactNotAuthorizedImages.RemoveKey(strProtocolID);
		}

		m_mapContactNotAuthorizedImages.SetAt(strProtocolID, pImage);
	}
}


void CUOLMessengerImagesProtocolImpl::InsertStatusButtonImage(const CString& strProtocolID, DWORD dwStatus, IUOLMessengerImagePtr pImage)
{
	CImagesProtocolMap::CPair* pPair;
	CSetImagesProtocolPtr pImagesProtocol;
	
	pPair = m_mapStatusButtonImages.Lookup(strProtocolID);
	
	if (pPair == NULL)
	{
		pImagesProtocol = new CSetImagesProtocol();
	}
	else
	{
		pImagesProtocol = pPair->m_value;
	}
	
	pImagesProtocol->InsertImage(dwStatus, pImage);
	m_mapStatusButtonImages.SetAt(strProtocolID, pImagesProtocol);
}


//////////////////////////////////////////////////////////////////////////
// Gets
//////////////////////////////////////////////////////////////////////////

IUOLMessengerImagePtr CUOLMessengerImagesProtocolImpl::GetDefaultImage(const CString& strProtocolID, DWORD dwId)
{
	CImagesProtocolMap::CPair* pPair;

	pPair = m_mapDefaulImages.Lookup(strProtocolID);

	if (pPair != NULL)
	{
		return pPair->m_value->GetImage(dwId);
	}

	return IUOLMessengerImagePtr();
}

IUOLMessengerImagePtr CUOLMessengerImagesProtocolImpl::GetUserImage(const CString& strProtocolID, DWORD dwStatus)
{
	CImagesProtocolMap::CPair* pPair;

	pPair = m_mapUserImages.Lookup(strProtocolID);

	if (pPair != NULL)
	{
		return pPair->m_value->GetImage(dwStatus);
	}

	return IUOLMessengerImagePtr();
}

IUOLMessengerImagePtr CUOLMessengerImagesProtocolImpl::GetUserStatusImage(const CString& strProtocolID, DWORD dwStatus)
{
	CImagesProtocolMap::CPair* pPair;

	pPair = m_mapUserStatusImages.Lookup(strProtocolID);

	if (pPair != NULL)
	{
		return pPair->m_value->GetImage(dwStatus);
	}

	return IUOLMessengerImagePtr();
}

IUOLMessengerImagePtr CUOLMessengerImagesProtocolImpl::GetContactImage(const CString& strProtocolID, DWORD dwStatus)
{
	CImagesProtocolMap::CPair* pPair;

	pPair = m_mapContactImages.Lookup(strProtocolID);

	if (pPair != NULL)
	{
		return pPair->m_value->GetImage(dwStatus);
	}

	return IUOLMessengerImagePtr();
}

IUOLMessengerImagePtr CUOLMessengerImagesProtocolImpl::GetContactStatusImage(IUOLMessengerContactPtr contact)
{
	CImagesProtocolMap::CPair* pPair;

	pPair = m_mapContactStatusImages.Lookup(contact->GetAccount()->GetProtocolId());

	if (pPair != NULL)
	{
		return pPair->m_value->GetImage(contact->GetStatus());
	}

	return IUOLMessengerImagePtr();
}

IUOLMessengerImagePtr CUOLMessengerImagesProtocolImpl::GetContactUnknownStatusImage(IUOLMessengerContactPtr contact)
{
	// This should be used when chat participant is not contact
    
    CImagesProtocolMap::CPair* pPair;

	pPair = m_mapContactStatusImages.Lookup(contact->GetAccount()->GetProtocolId());

	if (pPair != NULL)
	{
		return pPair->m_value->GetImage(IUOLMessengerContact::CS_UNKNOWN);
	}

	return IUOLMessengerImagePtr();
}

IUOLMessengerImagePtr CUOLMessengerImagesProtocolImpl::GetContactBlockedImage(const CString& strProtocolID)
{
	CImagesBlockedMap::CPair* pPair;

	pPair = m_mapContactBlockedImages.Lookup(strProtocolID);

	if (pPair != NULL)
	{
		return pPair->m_value;
	}

	return IUOLMessengerImagePtr();
}

IUOLMessengerImagePtr CUOLMessengerImagesProtocolImpl::GetContactAuthorizationIcon(const CString& strProtocolID, BOOL bAuthorized)
{
	if (FALSE == bAuthorized)
	{
		CImagesAuthorizationMap::CPair* pPair;

		pPair = m_mapContactNotAuthorizedImages.Lookup(strProtocolID);

		if (pPair != NULL)
		{
			return pPair->m_value;
		}
	}

	return IUOLMessengerImagePtr();
}

 CString CUOLMessengerImagesProtocolImpl::GetUserImagePath(const CString& strProtocolID, DWORD dwStatus)
{
	CImagesProtocolMap::CPair* pPair;

	pPair = m_mapContactImages.Lookup(strProtocolID);

	if (pPair != NULL)
	{
		return pPair->m_value->GetImagePath(dwStatus);
	}

	return "";
}

 CString CUOLMessengerImagesProtocolImpl::GetContactImagePath(const CString& strProtocolID, DWORD dwStatus)
{
	CImagesProtocolMap::CPair* pPair;

	pPair = m_mapContactImages.Lookup(strProtocolID);

	if (pPair != NULL)
	{
		return pPair->m_value->GetImagePath(dwStatus);
	}

	return "";
}

IUOLMessengerImagePtr CUOLMessengerImagesProtocolImpl::GetStatusButtonImage(const CString& strProtocolID, DWORD dwStatus)
{
	CImagesProtocolMap::CPair* pPair;
	
	pPair = m_mapStatusButtonImages.Lookup(strProtocolID);
	
	if (pPair != NULL)
	{
		return pPair->m_value->GetImage(dwStatus);
	}
	
	return IUOLMessengerImagePtr();
}

