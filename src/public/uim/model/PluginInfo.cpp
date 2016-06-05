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
#include "PluginInfo.h"


CPluginInfo::CPluginInfo(const CString& strInternalName) :
	m_strInternalName((LPCTSTR) strInternalName),
	m_pPlugin(NULL),
	m_lVersion(0),
	m_enumUpdateInfo(CPluginInfo::none)
{
}


CPluginInfo::~CPluginInfo()
{
}


//CUOLMessengerPlugin* CPluginInfo::GetPlugin()
//{
//	return m_pPlugin;
//}


CString CPluginInfo::GetInternalName() const
{
	return m_strInternalName;
}


CString CPluginInfo::GetName() const
{
	if ((m_pPlugin) && (m_enumUpdateInfo == none))
	{
		return (LPCTSTR) m_pPlugin->GetName();
	}

	return m_strName;
}


CString CPluginInfo::GetDesc() const
{
	if ((m_pPlugin) && (m_enumUpdateInfo == none))
	{
		return (LPCTSTR) m_pPlugin->GetDesc();
	}

	return m_strDesc;
}


ULONGLONG CPluginInfo::GetVersion() const
{
	if ((m_pPlugin) && (m_enumUpdateInfo == none))
	{
		return m_pPlugin->GetVersion();
	}

	return m_lVersion;
}


CString CPluginInfo::GetDownloadUrl() const
{
	return m_strDownloadUrl;
}


CString CPluginInfo::GetDirName() const
{
	return m_strDirName;
}


CString CPluginInfo::GetFileName() const
{
	return m_strFileName;
}


CString CPluginInfo::GetIconUrl() const
{
	return m_strIconUrl;
}


IUOLMessengerImagePtr CPluginInfo::GetIcon()
{
	if ((m_pPlugin) && (m_enumUpdateInfo == none))
	{
		return m_pPlugin->GetIcon();
	}

	return m_pIcon;
}


CPluginInfo::UpdateInfo CPluginInfo::GetUpdateInfo() const
{
	return m_enumUpdateInfo;
}


BOOL CPluginInfo::IsInstallAvailable() const
{
	return (m_enumUpdateInfo == installAvailable);
}


BOOL CPluginInfo::IsUpdateAvailable() const
{
	return (m_enumUpdateInfo == updateAvailable);
}


void CPluginInfo::SetPlugin(CUOLMessengerPlugin* pPlugin)
{
	m_pPlugin = pPlugin;
}


void CPluginInfo::SetName(const CString& strName)
{
	m_strName = (LPCTSTR) strName;
}


void CPluginInfo::SetDesc(const CString& strDesc)
{
	m_strDesc = (LPCTSTR) strDesc;
}


void CPluginInfo::SetVersion(ULONGLONG lVersion)
{
	m_lVersion = lVersion;
}


void CPluginInfo::SetDownloadUrl(const CString& strDownloadUrl)
{
	m_strDownloadUrl = (LPCTSTR) strDownloadUrl;
}


void CPluginInfo::SetDirName(const CString& strDirName)
{
	m_strDirName = (LPCTSTR) strDirName;
}


void CPluginInfo::SetFileName(const CString& strFileName)
{
	m_strFileName = (LPCTSTR) strFileName;
}


void CPluginInfo::SetIconUrl(const CString& strIconUrl)
{
	m_strIconUrl = (LPCTSTR) strIconUrl;
}


void CPluginInfo::SetIcon(IUOLMessengerImagePtr pIcon)
{
	m_pIcon = pIcon;
}


void CPluginInfo::SetUpdateInfo(UpdateInfo enumUpdateInfo)
{
	m_enumUpdateInfo = enumUpdateInfo;
}
