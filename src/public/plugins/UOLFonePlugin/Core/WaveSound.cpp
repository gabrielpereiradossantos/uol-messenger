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
#include "WaveSound.h"


CWaveSound::CWaveSound() : 
	m_nLoops(INFINITE_LOOP),
	m_bStopFlag(FALSE),
	m_thread(this),
	m_hWaveOut(NULL), 
	m_bIsStartingToPlay(FALSE)
{
	m_hCanStopEvent = ::CreateEvent(NULL, FALSE, FALSE, "CAN_STOP_PLAYING_EVENT");
}


CWaveSound::~CWaveSound()
{
}


void CWaveSound::SetOutputDevice(const CString& strDeviceName)
{
	m_strDeviceName = strDeviceName;
}


void CWaveSound::SetLoops(int nLoops)
{
	m_nLoops = nLoops;
}


BOOL CWaveSound::Play()
{
	if (FALSE == m_thread.IsAlive())
	{
		m_bStopFlag = FALSE;
		
		if (IsStartingToPlay())
		{
			StopSound();
		}
		
		SetStartingToPlay(TRUE);

		return m_thread.Start();
	}

	return FALSE;
}


BOOL CWaveSound::Stop()
{
	if (m_thread.IsAlive())
	{
		m_bStopFlag = TRUE;

		StopSound();

		m_thread.MessageLoopWait();
	}

	return TRUE;
}


void CWaveSound::Run()
{
	int nMaxLoops = m_nLoops;
	CString strDeviceOutput = m_strDeviceName;

	LPBYTE lpData = NULL;
	DWORD dwDataSize = 0;

	BOOL bOK = GetWaveData(&lpData, dwDataSize);

	if ((bOK) && (lpData != NULL) && (dwDataSize > 0))
	{
		DWORD dwSoundLength = 0;
		WAVEFORMATEX wfex = {0};
				
		if (ReadWaveFormat(&wfex, lpData, dwDataSize, &dwSoundLength))
		{
			for (int nInd = 0; ((nInd < nMaxLoops) || nMaxLoops == INFINITE_LOOP) && (FALSE == m_bStopFlag); nInd++)
			{
				if (!PlaySound(&wfex, strDeviceOutput, lpData, dwSoundLength))
				{
					SetStartingToPlay(FALSE);
					
					break;
				}
			}
			
			SetStartingToPlay(FALSE);
		}
		else
		{
			SetStartingToPlay(FALSE);
		}
	}
	else
	{
		SetStartingToPlay(FALSE);
	}
}


BOOL CWaveSound::ReadWaveFormat(LPWAVEFORMATEX lpWfex, LPBYTE lpData, DWORD dwDataSize, LPDWORD pdwSoundLength)
{
	if ((!lpWfex) || 
		(!pdwSoundLength) ||
		(!lpData) ||
		(0 == dwDataSize))
	{
		return FALSE;
	}

	// Offsets from OpenWengo OWSound lib...
	static const int WF_OFFSET_FORMATTAG = 20;
	static const int WF_OFFSET_CHANNELS = 22;
	static const int WF_OFFSET_SAMPLESPERSEC = 24;
	static const int WF_OFFSET_AVGBYTESPERSEC = 28;
	static const int WF_OFFSET_BLOCKALIGN = 32;
	static const int WF_OFFSET_BITSPERSAMPLE = 34;
	static const int WF_OFFSET_DATASIZE = 40;
	static const int WF_OFFSET_DATA = 44;
	static const int WF_HEADER_SIZE = WF_OFFSET_DATA;

	// Get file length
	if (dwDataSize < WF_HEADER_SIZE)
	{
		return FALSE;
	}

	*pdwSoundLength = dwDataSize - WF_HEADER_SIZE;

	// Set the format attribute members
	LPBYTE lpHeader = reinterpret_cast<LPBYTE>(lpData);

	lpWfex->cbSize = sizeof(WAVEFORMATEX);
	lpWfex->wFormatTag = * ((WORD*) (lpHeader + WF_OFFSET_FORMATTAG));
	lpWfex->nChannels = * ((WORD*) (lpHeader + WF_OFFSET_CHANNELS));
	lpWfex->nSamplesPerSec = * ((DWORD*) (lpHeader + WF_OFFSET_SAMPLESPERSEC));
	lpWfex->nAvgBytesPerSec = * ((DWORD*) (lpHeader + WF_OFFSET_AVGBYTESPERSEC));
	lpWfex->nBlockAlign = * ((WORD*) (lpHeader + WF_OFFSET_BLOCKALIGN));
	lpWfex->wBitsPerSample = * ((WORD*) (lpHeader + WF_OFFSET_BITSPERSAMPLE));

	return TRUE;
}


BOOL CWaveSound::PlaySound(const LPWAVEFORMATEX lpWfex, const CString& strDeviceName, LPBYTE lpData, DWORD dwDataLength)
{
	if ((!lpWfex) ||
		(!lpData) ||
		(0 == dwDataLength))
	{
		return FALSE;
	}

	// Opens output audio wave device
	HANDLE hDoneEvent = ::CreateEvent(NULL, FALSE, FALSE, "PLAYSOUND_DONE_EVENT");

	if (!hDoneEvent)
	{
		return FALSE;
	}

	HWAVEOUT hWaveOut = OpenWaveOutDevice(lpWfex, strDeviceName, hDoneEvent);

	if (hWaveOut)
	{
		// Initializes wave header
		WAVEHDR waveHeader;
		::ZeroMemory(&waveHeader, sizeof(WAVEHDR));
		waveHeader.lpData = (char*) lpData;
		waveHeader.dwBufferLength = dwDataLength;
		waveHeader.dwUser = 0;
		waveHeader.dwFlags = 0;
		waveHeader.dwLoops = 0;
		waveHeader.dwBytesRecorded = 0;
		waveHeader.lpNext = 0;
		waveHeader.reserved = 0;

		SetWaveOut(hWaveOut);

		// Plays buffer
		MMRESULT mmr = ::waveOutPrepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR));
		
		if (MMSYSERR_NOERROR == mmr)
		{
			mmr = ::waveOutWrite(hWaveOut, &waveHeader, sizeof(WAVEHDR));
	
			// Now we consider that the sound playing is already fully and 
			// a call to "::waveOutReset" won't lead to crash.
			//
			SetStartingToPlay(FALSE);
			::SetEvent(m_hCanStopEvent);
			
			if (MMSYSERR_NOERROR == mmr)
			{
				// Waits for audio to finish playing
				while (!(waveHeader.dwFlags & WHDR_DONE))
				{
					::WaitForSingleObject(hDoneEvent, INFINITE);
				}

				// Cleans up
				mmr = ::waveOutUnprepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR));

				if (MMSYSERR_NOERROR == mmr)
				{
					mmr = ::waveOutClose(hWaveOut);
				}
			}
		}

		::CloseHandle(hDoneEvent);

		return (mmr == MMSYSERR_NOERROR);
	}

	return FALSE;
}


void CWaveSound::StopSound()
{
	while (IsStartingToPlay())
	{
		::WaitForSingleObject(m_hCanStopEvent, 350);
	}
	
	::ResetEvent(m_hCanStopEvent);
	
	HWAVEOUT hWaveOut = GetWaveOut();
	
	if (hWaveOut)
	{
		StopWaveOutDevice(hWaveOut);
		SetWaveOut(NULL);
	}
}


HWAVEOUT CWaveSound::OpenWaveOutDevice(const LPWAVEFORMATEX lpWfex, const CString& strDeviceName, HANDLE hDoneEvent)
{
	HWAVEOUT hWaveOut = NULL;

	WAVEOUTCAPS woc;
	int nDevices = ::waveOutGetNumDevs();
	int nDeviceId;
	for (nDeviceId = 0; nDeviceId < nDevices; nDeviceId++)
	{
		if (MMSYSERR_NOERROR == ::waveOutGetDevCaps(nDeviceId, &woc, sizeof(WAVEOUTCAPS)))
		{
			if (strDeviceName.Compare(woc.szPname) == 0)
			{
				// Found device
				break;
			}
		}
	}

	if (nDeviceId == nDevices)
	{
		// Requested device not found, use default device
		nDeviceId = WAVE_MAPPER;
	}

	// Open wave out
	MMRESULT mmr = ::waveOutOpen(&hWaveOut, nDeviceId, lpWfex, (DWORD) hDoneEvent, 0, CALLBACK_EVENT);
	ATLASSERT(mmr == MMSYSERR_NOERROR);

	return hWaveOut;
}


void CWaveSound::StopWaveOutDevice(HWAVEOUT hWaveOut)
{
	MMRESULT mmr;
	mmr = ::waveOutReset(hWaveOut);
	ATLASSERT(mmr == MMSYSERR_NOERROR);

	//mmr = ::waveOutClose(hWaveOut);
	//ATLASSERT(mmr == MMSYSERR_NOERROR);
}


void CWaveSound::SetWaveOut(HWAVEOUT hWaveOut)
{
	m_hWaveOut = hWaveOut;
}


HWAVEOUT CWaveSound::GetWaveOut()
{
	return m_hWaveOut;
}

void CWaveSound::SetStartingToPlay(BOOL bIsStartingToPlay)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csStartingToPlay);
	
	m_bIsStartingToPlay = bIsStartingToPlay;
}


BOOL CWaveSound::IsStartingToPlay()
{
	BOOL bIsStartingToPlay = FALSE;
	
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csStartingToPlay);
		
		bIsStartingToPlay = m_bIsStartingToPlay;
	}
	
	return bIsStartingToPlay;
}

