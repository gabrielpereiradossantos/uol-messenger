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


class __declspec(novtable) IUOLMessengerCommonSettings
{
public:
	virtual ~IUOLMessengerCommonSettings() {}

	virtual BOOL LoadSettings() = 0;
	virtual BOOL SaveSettings() = 0;

	virtual void SetValue(const CString& strKey, const CString& strValue) = 0;
	virtual BOOL GetValue(const CString& strKey, CString& strValue) const = 0;

	virtual void SetValue(const CString& strKey, int nValue) = 0;
	virtual BOOL GetValue(const CString& strKey, int& nValue) const = 0;

	virtual CString GetAppMode() const = 0;
	virtual void SetAppMode(const CString& strAppMode) = 0;

	virtual CString GetDefaultTheme() const = 0;
	virtual void SetDefaultTheme(const CString& strDefaultTheme) = 0;

public:
	static const TCHAR APP_MODE_SETTING[];
	static const TCHAR APP_THEME_SETTING[];
	static const TCHAR APP_STRING_RESOURCE_SETTING[];
	static const TCHAR APP_LINK_ICON_SETTING[];
	static const TCHAR APP_LINK_ICON_INDEX_SETTING[];
	static const TCHAR APP_MODE_DEFAULT_VALUE[];
	static const TCHAR APP_THEME_DEFAULT_VALUE[];
	static const TCHAR APP_STRING_RESOURCE_DEFAULT_VALUE[];
};

MAKEAUTOPTR(IUOLMessengerCommonSettings);


__declspec(selectany)
const TCHAR IUOLMessengerCommonSettings::APP_MODE_SETTING[] = _T("AppMode");

__declspec(selectany)
const TCHAR IUOLMessengerCommonSettings::APP_THEME_SETTING[] = _T("AppTheme");

__declspec(selectany)
const TCHAR IUOLMessengerCommonSettings::APP_STRING_RESOURCE_SETTING[] = _T("AppStringResource");

__declspec(selectany)
const TCHAR IUOLMessengerCommonSettings::APP_LINK_ICON_SETTING[] = _T("AppLinkIcon");

__declspec(selectany)
const TCHAR IUOLMessengerCommonSettings::APP_LINK_ICON_INDEX_SETTING[] = _T("AppLinkIconIndex");

__declspec(selectany)
const TCHAR IUOLMessengerCommonSettings::APP_MODE_DEFAULT_VALUE[] = _T("UOLMessenger");

__declspec(selectany)
const TCHAR IUOLMessengerCommonSettings::APP_THEME_DEFAULT_VALUE[] = _T("Default");

__declspec(selectany)
const TCHAR IUOLMessengerCommonSettings::APP_STRING_RESOURCE_DEFAULT_VALUE[] = _T("res.xml");