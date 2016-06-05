/* UOL Fone
 * Copyright (c) 2006 Universo Online S/A
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
 * Universo Online S/A - A/C: UOL Fone 5o. Andar
 * Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
 * São Paulo SP - CEP 01452-002 - BRASIL  */


#include "stdafx.h"
#include "Preferences.h"


CPreferences::CPreferences()
{
	m_strAudioCodecsPrecedence = _T("ILBC/8000,G723/8000,PCMA/8000,GSM/8000");
	m_strVideoCodecsPrecedence = _T("H263/90000");
}


CPreferences::~CPreferences()
{
}


void CPreferences::SetCurrentAudioInputDevice(const CString& strDeviceName)
{
	m_strAudioInputDevice = strDeviceName;
}


void CPreferences::SetCurrentAudioOutputDevice(const CString& strDeviceName)
{
	m_strAudioOutputDevice = strDeviceName;
}


void CPreferences::SetAudioCodecsPrecedence(const CString& strCodecsPrecedence)
{
	m_strAudioCodecsPrecedence = strCodecsPrecedence;
}


void CPreferences::SetVideoCodecsPrecedence(const CString& strCodecsPrecedence)
{
	m_strVideoCodecsPrecedence = strCodecsPrecedence;
}


CString CPreferences::GetCurrentAudioInputDevice()
{
	return m_strAudioInputDevice;
}


CString CPreferences::GetCurrentAudioOutputDevice()
{
	return m_strAudioOutputDevice;
}


CString CPreferences::GetAudioCodecsPrecedence()
{
	return m_strAudioCodecsPrecedence;
}


CString CPreferences::GetVideoCodecsPrecedence()
{
	return m_strVideoCodecsPrecedence;
}
