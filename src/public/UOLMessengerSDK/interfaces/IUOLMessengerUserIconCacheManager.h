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

#include "IUOLMessengerUserIcon.h"
#include "IUOLMessengerAccount.h"
#include "IUOLMessengerContact.h"
#include "IUOLMessengerProfileManager.h"


class __declspec(novtable) IUOLMessengerUserIconCacheManager
{
public:
	virtual ~IUOLMessengerUserIconCacheManager() {}

	virtual IUOLMessengerUserIconPtr RegisterCachedImage(IUOLMessengerAccount* pAccount, const CString& strCachedImagePath) = 0;
	virtual IUOLMessengerUserIconPtr RegisterCachedImage(IUOLMessengerProfileElement* pProfileElement, const CString& strCachedImagePath) = 0;
	virtual IUOLMessengerUserIconPtr RegisterCachedImage(IUOLMessengerContact* pContact, const CString& strCachedImagePath) = 0;

	virtual IUOLMessengerUserIconPtr CacheIcon(IUOLMessengerAccount* pAccount, const CString& strOriginalPath, const CString& strNewImageFormat = _T("")) = 0;
	virtual IUOLMessengerUserIconPtr CacheIcon(IUOLMessengerProfileElement* pProfileElement, const CString& strOriginalPath, const CString& strNewImageFormat = _T("")) = 0;
	virtual IUOLMessengerUserIconPtr CacheIcon(IUOLMessengerContact* pContact, const char* pIconBuffer, int nBufferLength, const CString& strNewImageFormat = _T("")) = 0;

	virtual IUOLMessengerUserIconPtr GetCachedIcon(IUOLMessengerAccount* pAccount) = 0;
	virtual IUOLMessengerUserIconPtr GetCachedIcon(IUOLMessengerProfileElement* pProfileElement) = 0;
	virtual IUOLMessengerUserIconPtr GetCachedIcon(IUOLMessengerContact* pContact) = 0;

	virtual void PurgeAccountIcons() = 0;
	virtual void PurgeProfileIcons() = 0;
	virtual void PurgeContactIcons() = 0;

	virtual void ClearAccountIcons() = 0;
	virtual void ClearProfileIcons() = 0;
	virtual void ClearContactIcons() = 0;

	virtual CString GetPreferedIconFormat(CIMProtocol* pProtocol) = 0;

public:
	static const TCHAR IMG_FORMAT_BMP[];
	static const TCHAR IMG_FORMAT_JPEG[];
	static const TCHAR IMG_FORMAT_PNG[];
};

MAKEAUTOPTR(IUOLMessengerUserIconCacheManager);

__declspec(selectany)
const TCHAR IUOLMessengerUserIconCacheManager::IMG_FORMAT_BMP[] = _T("bmp");

__declspec(selectany)
const TCHAR IUOLMessengerUserIconCacheManager::IMG_FORMAT_JPEG[] = _T("jpeg");

__declspec(selectany)
const TCHAR IUOLMessengerUserIconCacheManager::IMG_FORMAT_PNG[] = _T("png");
