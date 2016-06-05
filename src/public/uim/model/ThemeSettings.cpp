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
#include "ThemeSettings.h"
#include "SettingsTraits.h"

CThemeSettings::CThemeSettings(CString strSelectedThemeFolder) : 
m_themeSettingsHandler(strSelectedThemeFolder + "\\Theme.xml")
{
	m_pSoundSettings = new CSoundSettings();
	m_pEmoticonSettings = new CEmoticonSettings();

// xxx 	m_pDefaultTheme = new CDefaultTheme();

	m_themeSettingsHandler.AddSetting("Sound", m_pSoundSettings.get());
	//m_themeSettingsHandler.AddSetting("Emoticons", m_pEmoticonSettings.get());

// xxx	m_themeSettingsHandler.AddSetting("Theme", m_pDefaultTheme.get());
}

void CThemeSettings::SetThemeFolder(CString strThemeFolder) 
{
	m_themeSettingsHandler.Init(strThemeFolder + "\\Theme.xml", FALSE, FALSE);
}

CThemeSettings::~CThemeSettings(void)
{
}

BOOL CThemeSettings::Load()
{
	BOOL bRet = m_themeSettingsHandler.LoadSettings(FALSE);

	return bRet;
}

BOOL CThemeSettings::Save()
{
	BOOL bRet = m_themeSettingsHandler.SaveSettings();

	return bRet;
}

IUOLMessengerSoundSettingsPtr CThemeSettings::GetSoundSettings() const
{
	return m_pSoundSettings;
}

IUOLMessengerEmoticonSettingsPtr CThemeSettings::GetEmoticonSettings() const
{
	return m_pEmoticonSettings;
}

//IUOLMessengerThemePtr CThemeSettings::GetDefaultTheme() const
//{
//	return m_pDefaultTheme;
//}
