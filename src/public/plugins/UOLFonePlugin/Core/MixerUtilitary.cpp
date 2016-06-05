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
#include ".\mixerutilitary.h"


CMixerUtilitary::CMixerUtilitary(void)
{
	m_hMixer = NULL;
}


CMixerUtilitary::~CMixerUtilitary(void)
{
}


BOOL CMixerUtilitary::InitializeMixerComponents(CWindow* callbackWindow, HMIXER &hMixer)
{
	ATLASSERT(m_hMixer == NULL);
	
	// Get the number of mixer devices present in the system.
	m_nNumMixers = ::mixerGetNumDevs();
	
	m_hMixer = NULL;
	::ZeroMemory(&m_mxcaps, sizeof(MIXERCAPS));
	
	// Open the first mixer.
	// A "mapper" for audio mixer devices does not currently exist.
	if (m_nNumMixers != 0)
	{
		MMRESULT result = ::mixerOpen(&m_hMixer, 
				0, 
				reinterpret_cast<DWORD>(callbackWindow->m_hWnd),
				NULL, 
				MIXER_OBJECTF_MIXER | CALLBACK_WINDOW);
		
		if (result != MMSYSERR_NOERROR)
		{
			return FALSE;
		}
		
		result = ::mixerGetDevCaps(reinterpret_cast<UINT>(m_hMixer),
				&m_mxcaps, 
				sizeof(MIXERCAPS));
		
		if (result != MMSYSERR_NOERROR)
		{
			return FALSE;
		}
	}
	
	hMixer = m_hMixer;
	
	return TRUE;
}


BOOL CMixerUtilitary::UninitializeMixerComponents()
{
	BOOL bSucc = TRUE;
	
	if (m_hMixer != NULL)
	{
		bSucc = (::mixerClose(m_hMixer) == MMSYSERR_NOERROR);
		m_hMixer = NULL;
	}
	
	return bSucc;
}


BOOL CMixerUtilitary::GetComponentVolumeControlAndBounds(
		DWORD componentTypeID, DWORD &ctrlID, DWORD &lowerBound, DWORD &upperBound)
{
	MIXERCONTROL mixerControl;
	
	if (!GetControlByComponentAndControlType(componentTypeID, 
			MIXERCONTROL_CONTROLTYPE_VOLUME, mixerControl)) {
		
		return FALSE;
	}
	
	// Return desired values.
	lowerBound = mixerControl.Bounds.dwMinimum;
	upperBound = mixerControl.Bounds.dwMaximum;
	ctrlID = mixerControl.dwControlID;
	
	return TRUE;
}


BOOL CMixerUtilitary::GetSystemVolumeLevelForControl(
		DWORD &dwVal, DWORD ctrlID) const
{
	if (m_hMixer == NULL)
	{
		return FALSE;
	}
	
	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = ctrlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;
	
	MMRESULT result = ::mixerGetControlDetails(reinterpret_cast<HMIXEROBJ>(m_hMixer),
			&mxcd,
			MIXER_OBJECTF_HMIXER |
			MIXER_GETCONTROLDETAILSF_VALUE);
	
	if (result != MMSYSERR_NOERROR)
	{
		return FALSE;
	}
	
	dwVal = mxcdVolume.dwValue;
	
	return TRUE;
}


BOOL CMixerUtilitary::SetSystemVolumeLevelForControl(DWORD dwVal, DWORD ctrlID) const
{
	if (m_hMixer == NULL)
	{
		return FALSE;
	}
	
	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume = { dwVal };
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = ctrlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;
	
	MMRESULT result = ::mixerSetControlDetails(reinterpret_cast<HMIXEROBJ>(m_hMixer),
			&mxcd,
			MIXER_OBJECTF_HMIXER |
			MIXER_SETCONTROLDETAILSF_VALUE);
	
	if (result != MMSYSERR_NOERROR)
	{
		return FALSE;
	}
	
	return TRUE;
}


BOOL CMixerUtilitary::GetComponentMuteControl(DWORD componentTypeID, DWORD &ctrlID)
{
	MIXERCONTROL mixerControl;
	
	if (!GetControlByComponentAndControlType(componentTypeID, 
			MIXERCONTROL_CONTROLTYPE_MUTE, mixerControl)) {
		
		return FALSE;
	}
	
	// Return desired value.
	ctrlID = mixerControl.dwControlID;
	
	return TRUE;
}


BOOL CMixerUtilitary::GetSystemMuteValueForControl(LONG &lVal, DWORD ctrlID) const
{
	if (m_hMixer == NULL)
	{
		return FALSE;
	}
	
	MIXERCONTROLDETAILS_BOOLEAN mxcdMute;
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = ctrlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
	mxcd.paDetails = &mxcdMute;
	
	MMRESULT result = ::mixerGetControlDetails(reinterpret_cast<HMIXEROBJ>(m_hMixer),
			&mxcd,
			MIXER_OBJECTF_HMIXER |
			MIXER_GETCONTROLDETAILSF_VALUE);
	
	if (result != MMSYSERR_NOERROR)
	{
		return FALSE;
	}
	
	lVal = mxcdMute.fValue;
	
	return TRUE;
}


BOOL CMixerUtilitary::SetSystemMuteValueForControl(LONG lVal, DWORD ctrlID) const
{
	if (m_hMixer == NULL)
	{
		return FALSE;
	}
	
	MIXERCONTROLDETAILS_BOOLEAN mxcdMute = { lVal };
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = ctrlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
	mxcd.paDetails = &mxcdMute;
	
	MMRESULT result = ::mixerSetControlDetails(reinterpret_cast<HMIXEROBJ>(m_hMixer),
			&mxcd,
			MIXER_OBJECTF_HMIXER |
			MIXER_SETCONTROLDETAILSF_VALUE);
	
	if (result != MMSYSERR_NOERROR)
	{
		return FALSE;
	}
	
	return TRUE;
}


//
// Protected members.
//


BOOL CMixerUtilitary::GetControlByComponentAndControlType(DWORD componentTypeID, 
		DWORD ctrlTypeID, MIXERCONTROL &mixerControl)
{
	if (m_hMixer == NULL)
	{
		return FALSE;
	}
	
	// Get dwLineID.
	MIXERLINE mxl;
	mxl.cbStruct = sizeof(MIXERLINE);
	mxl.dwComponentType = componentTypeID;
	
	MMRESULT result = ::mixerGetLineInfo(reinterpret_cast<HMIXEROBJ>(m_hMixer),
			&mxl,
			MIXER_OBJECTF_HMIXER |
			MIXER_GETLINEINFOF_COMPONENTTYPE);
	
	if (result != MMSYSERR_NOERROR)
	{
		return FALSE;
	}
	
	// Get dwControlID.
	MIXERLINECONTROLS mxlc;
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = ctrlTypeID;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mixerControl;
	
	result = ::mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(m_hMixer),
			&mxlc,
			MIXER_OBJECTF_HMIXER |
			MIXER_GETLINECONTROLSF_ONEBYTYPE);
	
	if (result != MMSYSERR_NOERROR)
	{
		return FALSE;
	}
	
	return TRUE;
}

