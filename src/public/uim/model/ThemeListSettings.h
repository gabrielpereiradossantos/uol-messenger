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

#include "Settings.h"
#include "Theme.h"


class CThemeListSettings : public CSettings
{
public:
	CThemeListSettings(const CString& strRootKey = m_szRootKeyValue) : 
		CSettings(strRootKey)
	{
	}

	~CThemeListSettings(void)
	{
	}

	void Load()
	{
		return;

		m_pTraits->OpenGroup(m_strKey);

		// Read current theme
		CString strCurrentTheme;
		m_pTraits->GetStringValue(m_szCurrentTheme, strCurrentTheme);

		// Read themes list and search for current theme...
		CString strThemeGroup;
		CString strThemeName;
		CString strFilePath;
		CThemePtr pTheme;
		BOOL bNewGroup;
		BOOL bRead;
		int nInd = 1;

		strThemeGroup.Format(_T("%s%d"), m_szTheme, nInd);
		bNewGroup = m_pTraits->OpenGroup(strThemeGroup);
		while (bNewGroup)
		{
			bRead = m_pTraits->GetStringValue(m_szThemeName, strThemeName);

			if (bRead)
			{
				bRead = m_pTraits->GetStringValue(m_szThemeFilePath, strFilePath);

				if (bRead)
				{
					pTheme = new CTheme(strThemeName, strFilePath);

					if (0 == strCurrentTheme.Compare(strThemeName))
					{
						m_pCurrentTheme = pTheme;
					}

					m_listThemes.AddTail(pTheme);
				}
			}
			m_pTraits->CloseGroup();

			// TODO: corrigir leitura de grupos!!!
			// Adjust values for next group iteration
			//nInd++;
			//strThemeGroup.Format(_T("%s%d"), m_szTheme, nInd);
			//bNewGroup = m_pTraits->OpenGroup(strThemeGroup);
		}

		m_pTraits->CloseGroup();
	}

	void Save()
	{
		return;

		m_pTraits->OpenGroup(m_strKey);

		ATLASSERT(m_pCurrentTheme != NULL);
		m_pTraits->SetStringValue(m_szCurrentTheme, m_pCurrentTheme->GetName());

		CThemePtr pTheme;
		CString strGroupName;
		int nInd = 1;
		for (POSITION pos = m_listThemes.GetHeadPosition(); pos;)
		{
			pTheme = m_listThemes.GetNext(pos);

			ATLASSERT(pTheme != NULL);
			strGroupName.Format(_T("%s%d"), m_szTheme, nInd);

			m_pTraits->OpenGroup(strGroupName);
			m_pTraits->SetStringValue(m_szThemeName, pTheme->GetName());
			m_pTraits->SetStringValue(m_szThemeFilePath, pTheme->GetFilePath());
			m_pTraits->CloseGroup();
		}

		m_pTraits->CloseGroup();
	}

	CThemePtr GetCurrentTheme()
	{
		return m_pCurrentTheme;
	}

	void GetThemes(CAtlList<CThemePtr>& listThemes)
	{
		listThemes.RemoveAll();
		listThemes.AddTailList(&m_listThemes);
	}

private:
	CThemePtr				m_pCurrentTheme;
	CAtlList<CThemePtr>		m_listThemes;

	static const TCHAR			m_szRootKeyValue[];
	static const TCHAR			m_szCurrentTheme[];
	static const TCHAR			m_szTheme[];
	static const TCHAR			m_szThemeName[];
	static const TCHAR			m_szThemeFilePath[];
};

__declspec(selectany) const TCHAR CThemeListSettings::m_szRootKeyValue[] = _T("Themes");
__declspec(selectany) const TCHAR CThemeListSettings::m_szCurrentTheme[] = _T("CurrentTheme");
__declspec(selectany) const TCHAR CThemeListSettings::m_szTheme[] = _T("Theme");
__declspec(selectany) const TCHAR CThemeListSettings::m_szThemeName[] = _T("Name");
__declspec(selectany) const TCHAR CThemeListSettings::m_szThemeFilePath[] = _T("FilePath");