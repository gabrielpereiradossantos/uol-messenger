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
 
#pragma once


#include <interfaces/IUOLMessengerImagesProtocol.h>
#include "SetImagesProtocol.h"


typedef CAtlMap<CString, CSetImagesProtocolPtr>		CImagesProtocolMap;
typedef CAtlMap<CString, IUOLMessengerImagePtr>		CImagesBlockedMap;
typedef CAtlMap<CString, IUOLMessengerImagePtr>		CImagesAuthorizationMap;

class CUOLMessengerImagesProtocolImpl : public IUOLMessengerImagesProtocol3
{
private:
	CImagesProtocolMap	m_mapDefaulImages;
	CImagesProtocolMap	m_mapUserImages;
	CImagesProtocolMap	m_mapUserStatusImages;
	CImagesProtocolMap	m_mapContactImages;
	CImagesProtocolMap	m_mapContactStatusImages;
	CImagesBlockedMap	m_mapContactBlockedImages;
	CImagesAuthorizationMap		m_mapContactNotAuthorizedImages;
	CImagesProtocolMap	m_mapStatusButtonImages;

public:
	CUOLMessengerImagesProtocolImpl();
	
	virtual IUOLMessengerImagePtr GetDefaultImage(const CString& strProtocolID, DWORD dwId);
	virtual IUOLMessengerImagePtr GetUserImage(const CString& strProtocolID, DWORD dwStatus);
	virtual IUOLMessengerImagePtr GetUserStatusImage(const CString& strProtocolID, DWORD dwStatus);
	virtual IUOLMessengerImagePtr GetContactImage(const CString& strProtocolID, DWORD dwStatus);
	virtual IUOLMessengerImagePtr GetContactStatusImage(IUOLMessengerContactPtr contact);
    virtual IUOLMessengerImagePtr GetContactUnknownStatusImage(IUOLMessengerContactPtr contact);
	virtual IUOLMessengerImagePtr GetContactBlockedImage(const CString& strProtocolID); 
	virtual IUOLMessengerImagePtr GetContactAuthorizationIcon(const CString& strProtocolID, BOOL bAuthorized = FALSE);
	virtual CString GetUserImagePath(const CString& strProtocolID, DWORD dwStatus);
	virtual CString GetContactImagePath(const CString& strProtocolID, DWORD dwStatus);
	virtual IUOLMessengerImagePtr GetStatusButtonImage(const CString& strProtocolID, DWORD dwStatus);

	virtual void InsertDefaultImage(const CString& strProtocolID, DWORD dwId, IUOLMessengerImagePtr pImage);
	virtual void InsertUserImage(const CString& strProtocolID, DWORD dwId,const CString& strPath);
	virtual void InsertUserStatusImage(const CString& strProtocolID,DWORD dwStatus, IUOLMessengerImagePtr pImage);
	virtual void InsertContactImage(const CString& strProtocolID,DWORD dwId,const CString& strPath);
	virtual void InsertContactStatusImage(const CString& strProtocolID,DWORD dwStatus, IUOLMessengerImagePtr pImage);
	virtual void InsertDefaultImage(const CString& strProtocolID,DWORD dwId, const CString& strFilePath);
	virtual void InsertUserStatusImage(const CString& strProtocolID,DWORD dwStatus, const CString& strFilePath);
	virtual void InsertContactStatusImage(const CString& strProtocolID,DWORD dwStatus, const CString& strFilePath);
	virtual void InsertContactBlockedImage(const CString& strProtocolID, IUOLMessengerImagePtr pImage);
	virtual void InsertContactAuthorizationIcon(const CString& strProtocolID, IUOLMessengerImagePtr pImage, BOOL bAuthorized);
	virtual void InsertStatusButtonImage(const CString& strProtocolID, DWORD dwStatus, IUOLMessengerImagePtr pImage);
};

MAKEAUTOPTR(CUOLMessengerImagesProtocolImpl);