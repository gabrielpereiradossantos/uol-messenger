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

#include <atlcoll.h>
#include "RadioConfig.h"
#include "style.h"
#include "channel.h"
#include "RadioConfig.h"

//////////////////////////////////////////////////////////////////////////
// CSongListObserver
//////////////////////////////////////////////////////////////////////////

class CSongListObserver 
{
public:
	virtual void OnLoadedChannels(int /*result*/, int /*bNotify*/) = 0;
	virtual void OnLoadedWMPURL(int /*result*/)	= 0;
	virtual void OnLoadedSongTitle(int /*result*/) = 0;
	virtual void OnLoadedFerrugem(int /*result*/) = 0;
};


//////////////////////////////////////////////////////////////////////////
// CSongListMng
//////////////////////////////////////////////////////////////////////////


class CSongListMng
{
public:
	
	CSongListMng(void);
	virtual ~CSongListMng(void);

	CString GetSongName()	  { return m_strSongName; }
	CString GetArtistName()   { return m_strArtistName; }
	CString GetCdName()		  { return m_strCdName; }
	CString GetNextSongName() { return m_strNextSongName; }
	CString GetWMPURL()		  { return m_strWMPUrl;} 
	int		GetResult(CString &url );
	
	BOOL	RegisterObserver(CSongListObserver* observer);
	BOOL	UnRegisterObserver(CSongListObserver* observer);
	
	CAtlList<CStyle*> & GetStyle() {return m_styles; }
	IUOLMessengerImagePtr GetSongImage() {return m_pSongIcon;}
	
	BOOL FireChannelsThread(BOOL bNotify =TRUE);
	void FireURLThread(int id);
	void FireURLThread(const CString &url );
	void FireGetSongInfoThread(const CString& title);
	
private:

	enum eSeek { 
		CHANNEL,
		SONG 
	};

	CRadioConfig *GetRadioUOLConfig();
	void ClearStyles();
	static CString GetJPGFilePath() ;
	BOOL	m_bChannelsRead;
	int		m_nResultFerrugem;					// Hold how many result where found in ferrugem
	int		m_nMusicId;
	int		m_nThreadCount;						// Used to start a new request in thread
	int		m_nThreadSongInfoRequest;			// Used to start a new request in thread
	eSeek	m_seeking;							
	CString m_strFerrugemURL;					// Hold the url to be used with ferrugem
	CString m_strURLSeek;						
	CString m_strWMPUrl;						// Hold the url to be used in windows media playes
	CString m_strSongTitleId;					// Song id in playlist
	
	CString m_strSongName;						// These variables hold information about current song
	CString m_strArtistName;					
	CString m_strCdName;
	CString m_strNextSongName;					
	IUOLMessengerImagePtr	m_pSongIcon;		

	CAtlList<CStyle*> m_styles;					// Hole syles/channel, used to fill popup 
	CAtlList<CSongListObserver*> m_vObservers;	// Hold observers
	CAtlArray<CString> m_vSongInfo;				// Hold Information about url to retrive information 
												// about a song
	BOOL		m_bNotifyChannels;

	// threads
	static UINT WINAPI GetChannelsThread(LPVOID pParam);
	static UINT WINAPI GetSongInfoThread(LPVOID pParam);
	static UINT WINAPI GetURLThread(LPVOID pParam);
	HANDLE	m_hURLThread;
	HANDLE	m_hChannelsThread;
	HANDLE	m_hSongTitleThread;
	unsigned int m_urlThreadID;
	unsigned int m_songTitleThreadID;
	unsigned int m_channelsThreadID;
};
