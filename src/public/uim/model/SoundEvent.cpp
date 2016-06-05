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
#include "SoundEvent.h"

#include "SettingsTraits.h"


CSoundEvent::CSoundEvent() : 
	m_bIsEnabled(TRUE)
{
	InitTagProcessMap();
}


CSoundEvent::CSoundEvent(const CString& strEventType, const CString& strDescription, const CString& strFilePath, BOOL bIsEnabled) :
	m_strEventType(strEventType),
	m_strDescription(strDescription),
	m_strFilePath(strFilePath),
	m_bIsEnabled(bIsEnabled)
{
	InitTagProcessMap();
}


CSoundEvent::~CSoundEvent()
{
}


CString CSoundEvent::GetEventType() const
{
	return m_strEventType;
}


CString CSoundEvent::GetDescription() const
{
	return m_strDescription;
}


void CSoundEvent::SetDescription(const CString& strDescription)
{
	m_strDescription = strDescription;
}


CString CSoundEvent::GetFilePath() const
{
	return m_strFilePath;
}


void CSoundEvent::SetFilePath(const CString& strFilePath)
{
	m_strFilePath = strFilePath;
}


BOOL CSoundEvent::IsEnabled() const
{
	return m_bIsEnabled;
}


void CSoundEvent::SetEnabled(BOOL bEnabled)
{
	m_bIsEnabled = bEnabled;
}


void CSoundEvent::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"Enabled", NEW_SETTINGS_CALLBACK(Enabled, this)},
		{"EventType", NEW_SETTINGS_CALLBACK(EventType, this)},
		{"EventDescription", NEW_SETTINGS_CALLBACK(EventDescription, this)},
		{"FilePath", NEW_SETTINGS_CALLBACK(FilePath, this)}
	};

	int nSize = sizeof(ih) / sizeof(ih[0]);

	for (int nInd = 0; nInd < nSize; nInd++)
	{
		ms_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}


void CSoundEvent::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Load(pNode, ms_tagProcess, this);
}


void CSoundEvent::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, ms_tagProcess, this);
}


IMPLEMENT_SETTINGS_CALLBACK(CSoundEvent, Enabled)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bIsEnabled);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bIsEnabled);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CSoundEvent, EventType)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, (CString)m_pThis->m_strEventType);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, (CString&)m_pThis->m_strEventType);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CSoundEvent, EventDescription)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, (CString)m_pThis->m_strDescription);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, (CString&)m_pThis->m_strDescription);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CSoundEvent, FilePath)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, (CString)m_pThis->m_strFilePath);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, (CString&)m_pThis->m_strFilePath);
	}
}