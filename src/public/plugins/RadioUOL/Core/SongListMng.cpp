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
#include <atlhttp.h>
#include <atlrx.h>
#include <process.h>

#include "RequestTicket.h"
#include "RequestChannels.h"
#include "RequestWMPURL.h"
#include "RequestSongInfo.h"
#include "RequestFerrugem.h"
#include "SongListMng.h"
#include "util.h"

#include "../FacadeWrapper.h"
#include "../resource.h"


//////////////////////////////////////////////////////////////////////////
// CSongListMng
//////////////////////////////////////////////////////////////////////////


CSongListMng::CSongListMng(void):
	m_nThreadSongInfoRequest(0),
	m_hChannelsThread(0),
	m_channelsThreadID(0),
	m_hURLThread(0),
	m_urlThreadID(0),
	m_hSongTitleThread(0),
	m_songTitleThreadID(0),
	m_nThreadCount(0),
	m_strURLSeek(""),
	m_nResultFerrugem(0),
	m_strFerrugemURL(""),
	m_bChannelsRead(FALSE)
{
}

CSongListMng::~CSongListMng(void)
{
	ATLTRACE("%s\n",__FUNCTION__);
	//if (m_hConfigThread) 
	//{
	//	ATLTRACE("%s WAITING m_hConfigThread \n",__FUNCTION__);
	//	WaitForSingleObject(m_hConfigThread, INFINITE);
	//	m_hConfigThread = NULL;
	//}

	if (m_hChannelsThread) 
	{
		ATLTRACE("%s WAITING m_hChannelsThread \n",__FUNCTION__);
		WaitForSingleObject(m_hChannelsThread,INFINITE);
		m_hChannelsThread = NULL;
	}

	if (m_hURLThread) 
	{
		ATLTRACE("%s WAITING m_hURLThread \n",__FUNCTION__);
		WaitForSingleObject(m_hURLThread,INFINITE);
		m_hURLThread = NULL;
	}

	if (m_hSongTitleThread) 
	{
		ATLTRACE("%s WAITING m_hSongTitleThread \n",__FUNCTION__);
		WaitForSingleObject(m_hSongTitleThread,INFINITE);
		m_hSongTitleThread = NULL;
	}

	ClearStyles();

	ATLASSERT (m_vObservers.GetCount()==0);
	m_vObservers.RemoveAll();

	DeleteFile(GetJPGFilePath());
}

void CSongListMng::ClearStyles()
{
	for (POSITION pos = m_styles.GetHeadPosition(); pos; )
	{
		CStyle* pStyle = m_styles.GetNext(pos);
		ATLASSERT(pStyle!=NULL);
		delete pStyle;
	}
	m_styles.RemoveAll();
}


BOOL CSongListMng::RegisterObserver(CSongListObserver* observer)
{
	m_vObservers.AddTail(observer);
	ATLASSERT(observer == m_vObservers.GetTail());
	return TRUE;
}

BOOL CSongListMng::UnRegisterObserver(CSongListObserver* observer)
{
	for(POSITION pos = m_vObservers.GetHeadPosition(); pos; )
	{
		POSITION oldpos = pos;
		CSongListObserver *tmpObserver = m_vObservers.GetNext(pos);
		if (tmpObserver==observer) {
			m_vObservers.RemoveAt(oldpos);
			break;
		}
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////
// Channel Thread
///////////////////////////////////////////////////////////////////

BOOL CSongListMng::FireChannelsThread(BOOL bNotify)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	m_bNotifyChannels = bNotify;


	if (m_hChannelsThread!=0) 
	{
		ATLTRACE("%s Thread not launched!\n",__FUNCTION__);
		return FALSE;
	}

	m_hChannelsThread = (HANDLE) _beginthreadex(NULL, 0, GetChannelsThread, this, 0, &m_channelsThreadID);
	if (m_hChannelsThread) 
	{
	}
	else
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	return TRUE;
}


UINT WINAPI CSongListMng::GetChannelsThread(LPVOID pParam)
{
	CSongListMng * pItem = static_cast <CSongListMng*> (pParam);
	ATLASSERT(pItem != NULL);
	int nResult=0;

	if (pItem->m_bChannelsRead==FALSE)
	{

		CRadioConfig* config = CRadioUOLWrapper::GetFacadeInstance()->GetConfig();
		nResult = (config==NULL)? CRadioUOLError::CONFIG_FILE : 0;

		if (nResult==0)
		{
			CRequestChannels channels(	config->GetChannelsURL(), pItem->m_styles );
			nResult = channels.Execute();	
		}

		if (nResult==0)
		{
			pItem->m_bChannelsRead=TRUE;
		}
	}


	// Notify 
	for(POSITION pos = pItem->m_vObservers.GetHeadPosition(); pos; )
	{
		CSongListObserver* pObserver;
		pObserver = pItem->m_vObservers.GetNext(pos);
		ATLASSERT(pObserver!=NULL);
		pObserver->OnLoadedChannels(nResult, pItem->m_bNotifyChannels);
	}

	CloseHandle(pItem->m_hChannelsThread);
	pItem->m_hChannelsThread = 0;
	return 0;
}

///////////////////////////////////////////////////////////////////
// Get WMP URL Thread
///////////////////////////////////////////////////////////////////


void CSongListMng::FireURLThread(const CString &url )
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	m_nThreadCount++;
	m_strURLSeek = url;
	m_seeking = SONG;

	if (m_hURLThread==0)
	{
		m_nThreadCount++;

		ATLASSERT(m_hURLThread==0);
		m_hURLThread = (HANDLE) _beginthreadex(NULL, 0, GetURLThread, this, 0, &m_urlThreadID);
		if (m_hURLThread) 
		{

		}
		else
		{
			ATLASSERT(FALSE);
		}
	}
	else 
	{
		ATLTRACE(_T("%s %s\n"), __FUNCTION__, "Thread nao pode ser incializada");
	}
}




void CSongListMng::FireURLThread(int id)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	m_nMusicId = id;
	m_nThreadCount++;
	m_seeking = CHANNEL;

	if (m_hURLThread==0)
	{
		m_nThreadCount++;

		ATLASSERT(m_hURLThread==0);
		m_hURLThread = (HANDLE) _beginthreadex(NULL, 0, GetURLThread, this, 0, &m_urlThreadID);
		if (m_hURLThread) 
		{
			ATLTRACE(_T("%s thread %d launched\n"), __FUNCTION__, m_urlThreadID);
		}
		else
		{
			ATLTRACE(_T("%s Unable to launch thread\n"), __FUNCTION__);
			ATLASSERT(FALSE);
		}
	}
	else 
	{
		ATLTRACE(_T("%s Thread %d still running\n"), __FUNCTION__, m_urlThreadID);
	}
}


UINT WINAPI CSongListMng::GetURLThread(LPVOID pParam)
{

	CSongListMng * pItem = static_cast <CSongListMng*> (pParam);

	//BOOL bResult;
	int nResult = 0;

	ATLASSERT(pItem != NULL);

	CRadioConfig* config = pItem->GetRadioUOLConfig();

	nResult = (config == NULL) ? CRadioUOLError::CONFIG_FILE : 0 ;

	//bResult = (config !=NULL);

	int savedMusicId=-1 ;

	do {

		CString strticket;

		savedMusicId = pItem->m_nThreadCount;


		////////////////////////

		// Procurando Canais

		if (pItem->m_seeking==CHANNEL)
		{
			if (nResult==0)
			{
				CRequestTicket ticket(config->GetTicketURL(), config->GetTicketRegex());
				nResult = ticket.Execute();
				strticket = ticket.GetTicket();
				ATLTRACE("%s TK=%s\n",__FUNCTION__,strticket);
			}

			if (savedMusicId != pItem->m_nThreadCount) 
			{
				continue;
			}

			if (nResult==0) 
			{
				CString strText;
				CString tmpString;
				tmpString.Format("nomeplaylist=%d&opcao=playlist",pItem->m_nMusicId);
				strText.Format(config->GetWMPURL(),strticket, tmpString);
				CRequestWMPURL wmpurl(strText, config->GetWMPURLRegEx(),pItem->m_vSongInfo);
				nResult = wmpurl.Execute();
				if (nResult==0)
				{
					pItem->m_strWMPUrl = wmpurl.GetWMPURL();
				}
			}

		} else {

			// Busca por Musica Individual
			if (nResult==0)
			{
				CRequestTicket ticket(config->GetTicketURL(), config->GetTicketRegex());
				nResult = ticket.Execute();
				strticket = ticket.GetTicket();
				ATLTRACE("%s TK=%s\n",__FUNCTION__,strticket);
			}

			if (savedMusicId != pItem->m_nThreadCount) 
			{
				continue;
			}

			CString strText;
			strText.Format(pItem->GetRadioUOLConfig()->GetWMPURL(), strticket, pItem->m_strURLSeek);

			CRequestWMPURL wmpurl(strText, config->GetWMPURLRegEx(), pItem->m_vSongInfo);
			nResult = wmpurl.Execute();
			if (nResult==0)
			{
				pItem->m_strWMPUrl = wmpurl.GetWMPURL();
			}


		}

		if (savedMusicId != pItem->m_nThreadCount) 
		{
			continue;
		}

		// Notify observers
		for(POSITION pos = pItem->m_vObservers.GetHeadPosition(); pos; )
		{
			CSongListObserver* pObserver;
			pObserver = pItem->m_vObservers.GetNext(pos);
			ATLASSERT(pObserver!=NULL);
			pObserver->OnLoadedWMPURL(nResult);
		}



	} while (savedMusicId != pItem->m_nThreadCount);
	///
	///

	CloseHandle(pItem->m_hURLThread);
	pItem->m_hURLThread = 0;
	pItem->m_nThreadCount=0;
	return 0;
}

///////////////////////////////////////////////////////////////////
// SongInfo Thread
///////////////////////////////////////////////////////////////////

void CSongListMng::FireGetSongInfoThread(const CString& title)
{
	ATLTRACE("%s SongID %s\n",__FUNCTION__, title);
	m_nThreadSongInfoRequest++;
	m_strSongTitleId = title;
	if (m_nThreadSongInfoRequest==1) 
	{
		m_hSongTitleThread = (HANDLE) _beginthreadex(NULL, 0, GetSongInfoThread, this, 0, &m_songTitleThreadID);
		if (m_hSongTitleThread) 
		{
		}
		else
		{
			ATLASSERT(FALSE);

		}
	}
	else 
	{
	}
}


UINT WINAPI CSongListMng::GetSongInfoThread(LPVOID pParam)
{
	CSongListMng * pItem = static_cast <CSongListMng*> (pParam);
	ATLASSERT(pItem != NULL);
	int savedThreadSongInfoRequest=-1;
	BOOL nResult=0;


	do 
	{

		savedThreadSongInfoRequest = pItem->m_nThreadSongInfoRequest;
		CString tmp;
		unsigned int id = atoi(pItem->m_strSongTitleId);


		if (nResult==0)
		{
			tmp.LoadString(IDS_SONG_INFO_URL);
			nResult = (pItem->m_vSongInfo.GetCount()>=id) ? 0 : CRadioUOLError::END_OF_STREAM;
			if (nResult==0) 
			{
				CRequestSongInfo songinfo(pItem->GetRadioUOLConfig()->GetSongInfoURL(),
					pItem->m_vSongInfo[id-1] , 
					pItem->GetRadioUOLConfig()->GetSongInfoSongRegEx(),
					pItem->GetRadioUOLConfig()->GetSongInfoArtistRegEx() ,
					pItem->GetRadioUOLConfig()->GetSongInfoJpegRegEx(),
					pItem->GetRadioUOLConfig()->GetSongInfoCdRegEx(),
					pItem->GetRadioUOLConfig()->GetSongInfoNextSongRegEx()
					);


				if (nResult==0)
				{
					nResult = songinfo.Execute();
				}

				if (nResult==0) 
				{
					pItem->m_strSongName = songinfo.GetSongName();
					pItem->m_strArtistName = songinfo.GetArtistName();
					pItem->m_strCdName = songinfo.GetCd();
					pItem->m_strNextSongName = songinfo.GetNextSong();
					CString strDownloadFileName = GetJPGFilePath();
					if (DownloadFile(songinfo.GetJpgPath(), strDownloadFileName )==0)
					{
						IUOLMessengerUIManagerPtr pUIManager = CFacadeWrapper::GetFacadeInstance()->GetUIManager();

						if (pUIManager)
						{
							pItem->m_pSongIcon = pUIManager->LoadImage(strDownloadFileName);
						}
					}
					else
					{
						pItem->m_pSongIcon = NULL;
					}
				}
				else
				{
					pItem->m_strSongName = _T("Não disponível");
					pItem->m_strArtistName = _T("Não disponível");
					pItem->m_strCdName = _T("Não disponível");
					pItem->m_strNextSongName = _T("Não disponível");
					pItem->m_pSongIcon = NULL;
				}
			}
		}

		// Notifica que foi lido as informacoes da musica

		for(POSITION pos = pItem->m_vObservers.GetHeadPosition(); pos; )
		{
			CSongListObserver* pObserver;
			pObserver = pItem->m_vObservers.GetNext(pos);
			ATLASSERT(pObserver!=NULL);
			pObserver->OnLoadedSongTitle(nResult);
		}


		/// Busca no Ferrugem
		CString strText;

		strText.Format(pItem->GetRadioUOLConfig()->GetFerrugemURL(),pItem->m_strSongName);
		//strText.Format(pItem->GetRadioUOLConfig()->GetFerrugemURL(),"a");

		//nResult = (strText.IsEmpty()==FALSE);
		//if (bResult)
		//{
		CRequestFerrugem requestFerrugem(strText, pItem->GetRadioUOLConfig()->GetFerrugemURLRegEx());
		nResult = requestFerrugem.Execute();
		pItem->m_strFerrugemURL = strText;

		if (nResult==0)
		{
			pItem->m_nResultFerrugem = requestFerrugem.GetResult();
		}
		//}

		// Notifica que foram lidas as informacoes do Ferrugem
		for(POSITION pos = pItem->m_vObservers.GetHeadPosition(); pos; )
		{
			CSongListObserver* pObserver;
			pObserver = pItem->m_vObservers.GetNext(pos);
			ATLASSERT(pObserver!=NULL);
			pObserver->OnLoadedFerrugem(nResult);
		}




	} while (savedThreadSongInfoRequest!=pItem->m_nThreadSongInfoRequest);
	///
	///

	CloseHandle(pItem->m_hSongTitleThread);
	pItem->m_hSongTitleThread = 0;
	pItem->m_nThreadSongInfoRequest=0;
	return 0;
}



CString CSongListMng::GetJPGFilePath() 
{
	static int id=0;
	id++;
	CString m_current_profile;
	CString m_path;
	IUOLMessengerProfileElementPtr pProfileElement;
	ATLASSERT(CFacadeWrapper::GetFacadeInstance() != NULL);
	m_current_profile = CFacadeWrapper::GetFacadeInstance()->GetProfileManager()->GetCurrentProfile();
	CFacadeWrapper::GetFacadeInstance()->GetProfileManager()->GetProfileElement(m_current_profile, pProfileElement);
	m_path = pProfileElement->GetProfilePath();
	m_path = m_path + _T("\\tmp_uolradio.jpg");
	return m_path;
}

int CSongListMng::GetResult(CString &url) 
{
	if (m_nResultFerrugem>0)
	{
		url = m_strFerrugemURL;
	}
	else
	{
		url ="";
	}

	return m_nResultFerrugem; 
}

CRadioConfig* CSongListMng::GetRadioUOLConfig()
{
	if (CRadioUOLWrapper::GetFacadeInstance()!=NULL)
	{	
		return CRadioUOLWrapper::GetFacadeInstance()->GetConfig();
	}
	return NULL;

}