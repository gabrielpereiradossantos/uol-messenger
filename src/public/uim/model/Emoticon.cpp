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

#include "Emoticon.h"
#include "SettingsTraits.h"
#include "../view/Image/ImageBuilder.h"

// CEmoticon
CEmoticon::CEmoticon(int nId) :  CImageButton(/*nId*/)
{
	InitTagProcessMap();
}

CEmoticon::~CEmoticon(void)
{
}

void CEmoticon::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"Symbol", NEW_SETTINGS_CALLBACK(Symbol, this)},
		{"Path", NEW_SETTINGS_CALLBACK(Path, this)},
		{"Name", NEW_SETTINGS_CALLBACK(Name, this)}
	};

	int nSize = sizeof(ih) / sizeof(ih[0]);

	for (int nInd = 0; nInd < nSize; nInd++)
	{
		ms_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}

void CEmoticon::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Load(pNode, ms_tagProcess, this);
}

void CEmoticon::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, ms_tagProcess, this);
}

CString CEmoticon::GetSymbol() const
{
	return m_strSymbol;
}

void CEmoticon::SetSymbol(const CString& strSymbol)
{
	m_strSymbol = strSymbol;
}

CString CEmoticon::GetPath() const
{
	return m_strPath;
}

void CEmoticon::SetPath(const CString& strPath)
{
	m_strPath = strPath;
}

CString CEmoticon::GetName() const
{
	return m_strName;
}

void CEmoticon::SetName(const CString& strName)
{
	m_strName = strName;
}

IMPLEMENT_SETTINGS_CALLBACK(CEmoticon, Symbol)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strSymbol);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strSymbol);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CEmoticon, Path)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strPath);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strPath);

		do
		{
			if (m_pThis->m_strPath.IsEmpty())
			{
				break;
			}

			HANDLE hFile = CreateFile(m_pThis->m_strPath, GENERIC_READ, FILE_SHARE_READ, 
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

			if (INVALID_HANDLE_VALUE == hFile)
			{
				break;
			}

			int size = GetFileSize(hFile, NULL);
			BYTE * pBuffer = new BYTE[size];

			if (!pBuffer)
			{
				break;
			}

			DWORD dwRead;
			ReadFile(hFile, pBuffer, size, & dwRead, NULL);
			CloseHandle(hFile);

			IUOLMessengerImagePtr pImage = UOLMessengerImage::CImageBuilder::CreateImage(pBuffer, size);
			m_pThis->SetImage(pImage);

		} while (FALSE);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CEmoticon, Name)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strName);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strName);
	}
}
