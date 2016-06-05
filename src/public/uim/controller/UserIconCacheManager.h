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

#include <interfaces/IUOLMessengerUserIconCacheManager.h>

using namespace Gdiplus;

class CUserIconCacheManager : public IUOLMessengerUserIconCacheManager
{
public:
	CUserIconCacheManager();
	virtual ~CUserIconCacheManager();

	BOOL Init(IUOLMessengerProfileManagerPtr pProfileManager);
	void Finalize();

protected:
	virtual IUOLMessengerUserIconPtr RegisterCachedImage(IUOLMessengerAccount* pAccount, const CString& strCachedImagePath);
	virtual IUOLMessengerUserIconPtr RegisterCachedImage(IUOLMessengerProfileElement* pProfileElement, const CString& strCachedImagePath);
	virtual IUOLMessengerUserIconPtr RegisterCachedImage(IUOLMessengerContact* pContact, const CString& strCachedImagePath);

	virtual IUOLMessengerUserIconPtr CacheIcon(IUOLMessengerAccount* pAccount, const CString& strOriginalPath, const CString& strNewImageFormat = _T(""));
	virtual IUOLMessengerUserIconPtr CacheIcon(IUOLMessengerProfileElement* pProfileElement, const CString& strOriginalPath, const CString& strNewImageFormat = _T(""));
	virtual IUOLMessengerUserIconPtr CacheIcon(IUOLMessengerContact* pContact, const char* pIconBuffer, int nBufferLength, const CString& strNewImageFormat = _T(""));

	virtual IUOLMessengerUserIconPtr GetCachedIcon(IUOLMessengerAccount* pAccount);
	virtual IUOLMessengerUserIconPtr GetCachedIcon(IUOLMessengerProfileElement* pProfileElement);
	virtual IUOLMessengerUserIconPtr GetCachedIcon(IUOLMessengerContact* pContact);

	virtual void PurgeAccountIcons();
	virtual void PurgeContactIcons();
	virtual void PurgeProfileIcons();

	virtual void ClearAccountIcons();
	virtual void ClearContactIcons();
	virtual void ClearProfileIcons();

	virtual CString GetPreferedIconFormat(CIMProtocol* pProtocol);

private:
	CString GetAccountIconCacheDir() const;
	void SetAccountIconCacheDir(const CString& strDirPath);

	CString GetProfileIconCacheDir() const;
	void SetProfileIconCacheDir(const CString& strDirPath);

	CString GetContactIconBaseDir() const;
	void SetContactIconBaseDir(const CString& strDirPath);

	void InternalSetCacheDir(CString& strCurDir, const CString& strNewDir);
	IUOLMessengerUserIconPtr CreateUserIcon(const CString& strCachedImagePath);

	void CalculateImageSize(const CSize& sizeCurrrent, const CSize& sizeMin, const CSize& sizeMax, CSize& sizeNew);
	int GetEncoderClsid(const CString& strImageFormat, CLSID* pClsid);

	CString BuildContactUserIconKey(IUOLMessengerContact* pContact);
	CString BuildContactUserIconKey(const CString& strAccountUserId, const CString& strAccountProtocolId, const CString& strName);
	
	CString GetContactIconDir(IUOLMessengerContact* pContact);
	CString GetContactIconFileName(IUOLMessengerContact* pContact);

	BOOL SaveIcon(LPCTSTR lpszFilePath, const char* pIconBuffer, int nBufferLength);

	void LoadContactIcons();
	void ExtractAccount(const CString& strDir, CString& strUserId, CString& strProtocolId);
	void ExtractContactName(const CString& strIconFilePath, CString& strContactName);

	Image* GetPhotoImage(Bitmap* pBitmap, int cx, int cy);

private:
	typedef CAtlMap<IUOLMessengerAccount*, IUOLMessengerUserIconPtr> CAccountUserIconMap;
	typedef CAtlMap<IUOLMessengerProfileElement*, IUOLMessengerUserIconPtr> CProfileUserIconMap;
	typedef CAtlMap<CString, IUOLMessengerUserIconPtr> CContactUserIconMap;

	static const char* ACCOUNT_ICON_FILE_NAME_PREFIX;
	static const char* PROFILE_ICON_FILE_NAME_PREFIX;
	static const char* CONTACT_ICON_FILE_NAME_PREFIX;
	static const char* CONTACT_ICON_FILE_NAME_SUFIX;

	static const LONG ICON_MAX_WIDTH;
	static const LONG ICON_MAX_HEIGHT;

	CString		m_strAccountIconCacheDir;
	CString		m_strProfileIconCacheDir;
	CString		m_strContactIconBaseDir;

	CAccountUserIconMap		m_mapAccountIcons;
	CProfileUserIconMap		m_mapProfileIcons;
	CContactUserIconMap		m_mapContactIcons;
};