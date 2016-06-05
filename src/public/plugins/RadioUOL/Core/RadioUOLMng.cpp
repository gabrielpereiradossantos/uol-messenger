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
#include "RadioUOLMng.h"
#include "../resource.h"
#include <atltypes.h>
/////////////////////////////////////////////////////////////////////
// CRadioUOLMng
/////////////////////////////////////////////////////////////////////

const IID DIID__WMPOCXEvents = {0x6BF52A51,0x394A,0x11d3,{0xB1,0x53,0x00,0xC0,0x4F,0x79,0xFA,0xA6}};

CRadioUOLMng::CRadioUOLMng():
	m_songs(NULL),
	m_pConfig(NULL),
	m_nPlayCount(0)
{
}

CRadioUOLMng::~CRadioUOLMng()
{
	if (m_pView!=NULL)
	{
		if (m_pView->IsWindow())
		{
			m_pView->DestroyWindow();
		}
	}
	m_pConfig = NULL;
}

void CRadioUOLMng::UpdateDisplay()
{
	if (m_uolView.IsWindow())
	{
		CString strText;
		WMPPlayState state;
		if (m_spPlayer!=NULL) {
			m_spPlayer->get_playState(&state);
			switch (state ) {
		case wmppsPlaying:
		case wmppsPaused:
		case wmppsStopped:
			if (m_songs)
			{
				m_uolView.SetSongTitle(m_songs->GetSongName(), GetCurrentTime());	
				m_uolView.ShowBuyButton( (m_songs->GetResult(strText)>0 ));
			}
			break;
		case wmppsReady:
		case wmppsUndefined:
			strText.LoadString(IDS_CHOOSE_STATION);
			m_uolView.SetSongTitle(strText, _T(""));	
			m_uolView.ShowBuyButton(FALSE);
			break;
		default:
			CComBSTR status;
			m_spPlayer->get_status(&status);
			m_uolView.SetSongTitle(CString(status), _T(""));	
			m_uolView.ShowBuyButton(FALSE);
			break;
			}

			m_uolView.Invalidate();
		}
	}
}


HRESULT CRadioUOLMng::OnPlayStateChange(long NewState)
{
	//ATLTRACE("%s NewState %d\n",__FUNCTION__,NewState);
	
	UpdateDisplay();
    
	if (m_spPlayer != NULL)
	{
		WMPPlayState state;
		m_spPlayer->get_playState(&state);
		
		CFacadeWrapper::GetInstance()->UpdatePlayerMenuItens(state);
	}
	
	return S_OK;
}

HRESULT CRadioUOLMng::OnMediaError(IDispatch *pMediaObject )
{
	ATLTRACE("%s\n",__FUNCTION__);
	
	CFacadeWrapper::GetInstance()->UpdatePlayerMenuItens(wmppsUndefined);
	
	static int lastPlayCount=-1;
	if (m_nPlayCount != lastPlayCount)
	{
		lastPlayCount = m_nPlayCount;
		long lErrCode = 0;

		if (pMediaObject)
		{
			IWMPMedia2* pWMPMedia2 = NULL;
			HRESULT hr = pMediaObject->QueryInterface(__uuidof(IWMPMedia2), (void**) &pWMPMedia2);
			if (hr == S_OK)
			{
				IWMPErrorItem* pItem = NULL;
				hr = pWMPMedia2->get_error(&pItem);
				if ((hr == S_OK) && (pItem != NULL))
				{
					hr = pItem->get_errorCode(&lErrCode);
					if (hr == S_OK)
					{
						// If you want to know error description, uncomment these lines
						CComBSTR bstrErrDesc;
						hr = pItem->get_errorDescription(&bstrErrDesc);
						if (hr == S_OK)
						{
							CString strErr;
							strErr.Format("errCode=%ld errDesc=%S\n", lErrCode, bstrErrDesc);
						}
					}
				}
				if (pItem)
				{
					pItem->Release();
				}
			}
			if (pWMPMedia2)
			{
				pWMPMedia2->Release();
			}
		}
		
		if ((lErrCode == SERVER_BUSY_ERROR) || (lErrCode == SERVER_UNKNOWN_ERROR))
		{
			return S_OK;
		}
		else
		{
			CRadioUOLError error(CRadioUOLError::STREAMING_ERRO);
		}
	}

	if (m_pC != NULL)
	{
		m_pC->stop();
	}
	
    return S_OK;
}

HRESULT STDMETHODCALLTYPE  CRadioUOLMng::OpenStateChange(long NewState)
{
	ATLTRACE("%s state %d %s\n",__FUNCTION__, NewState,GetCurrentSong() );
	if (NewState==wmposMediaOpen)
	{
		if (m_songs)
		{
			m_songs->FireGetSongInfoThread(GetCurrentSong());
		}
	}
	if ((wmposOpeningUnknownURL==NewState)||(NewState==wmposMediaOpen))
	{
		m_nPlayCount++;
	}
	return S_OK;
}


CString CRadioUOLMng::GetCurrentTime() const
{
	CString retString;
	ATLASSERT(m_pC!=NULL);
	if (m_pC!=NULL) 
	{
		BSTR tmpString;
		m_pC->get_currentPositionString(&tmpString);
		retString = tmpString;
	}
	return retString;
}


CString CRadioUOLMng::GetCurrentSong() const
{
	CString retString="";
	ATLASSERT(m_spPlayer!=NULL);
	if (m_spPlayer!=NULL) 
	{
		IWMPMedia *media=NULL;
		m_spPlayer->get_currentMedia(&media);
		if (media!=NULL) 
		{
			BSTR name;
			media->get_name(&name);
			retString = name;
		}
	}
	return retString;
}


void CRadioUOLMng::PlayURL(const CString& url)
{
	m_songs->FireURLThread(url);
}


void CRadioUOLMng::ShowSearch(int i)
{
	CString strUrl;
	CString strCaption;
	CRadioConfig* pConfig;

	if (CRadioUOLWrapper::GetFacadeInstance()==NULL)
	{
		return;
	}

	pConfig = CRadioUOLWrapper::GetFacadeInstance()->GetConfig();
	if (pConfig==NULL)
	{
		CRadioUOLError error(CRadioUOLError::CONFIG_FILE);
		return;
	}

	switch (i) 
	{
	case 1:
		
		strCaption.LoadString(IDS_SEARCH_CAPTION);
		strUrl = pConfig->GetSiteSearchURL();
		break;
	case 2:
		strCaption.LoadString(IDS_CHANNEL_CAPTION);
		strUrl = pConfig->GetSiteChannelURL();
		break;
	default:
		ATLASSERT(FALSE);
	}

	IUOLMessengerUIManagerPtr pUIManager = CFacadeWrapper::GetFacadeInstance()->GetUIManager();

	if (pUIManager)
	{
		CWindow wndFrameWindow;

		if (m_pFrameWindow)
		{
			wndFrameWindow = m_pFrameWindow->GetHWND();

			if (wndFrameWindow.IsWindow())
			{
				wndFrameWindow.BringWindowToTop();
				wndFrameWindow.ShowWindow(SW_NORMAL);
				(*m_pView)->Navigate(CComBSTR(strUrl),NULL,   NULL,NULL,NULL);
				return ;
			}
		}

		if (!m_pView)
		{
			m_pView = new CSampleFrameWindowView();
		}

		m_pFrameWindow = pUIManager->CreateFrameWindow(m_pView, CRect(CPoint(0, 0), CSize(490, (420+83))));

		if (m_pFrameWindow)
		{
			wndFrameWindow = m_pFrameWindow->GetHWND();

			CRect rect;
			CRect rectWindow;
			CRect cRect;
			CRect CrectWindow;
			CRect parRect;
			CRect rectDesktop;

			pUIManager->GetAppWindow().GetClientRect(&parRect);
			pUIManager->GetAppWindow().ClientToScreen(&parRect);

			wndFrameWindow.GetClientRect(rect);
			wndFrameWindow.GetWindowRect(rectWindow);
			wndFrameWindow.GetClientRect(cRect);
			wndFrameWindow.GetWindowRect(CrectWindow);

			wndFrameWindow.ClientToScreen(&cRect);
			wndFrameWindow.ClientToScreen(&CrectWindow);

			::GetWindowRect(::GetDesktopWindow(),&rectDesktop);

			int x,y;

			if (parRect.left>=(cRect.Width()+1))
			{
				x = parRect.left-cRect.Width()+1;
			} 
			else
			{
				x = parRect.right+1;
			}

			if (parRect.bottom>rectDesktop.bottom)
			{
				y = parRect.top;
			} 
			else
			{
				y = parRect.bottom-cRect.Height()+1;
			}


			//x=x - 5;
			//y=y - 24;
			x = x - (CrectWindow.right - cRect.right - 3);
			y = y - (CrectWindow.bottom - cRect.bottom - 3);


			wndFrameWindow.SetWindowPos(NULL,x, y,0,0,SWP_NOSIZE | SWP_NOZORDER);
			//wndFrameWindow.CenterWindow(::GetDesktopWindow());
			wndFrameWindow.ShowWindow(SW_NORMAL);

			m_pFrameWindow->SetCaptionText(strCaption);
			m_pFrameWindow->SetStatusBarText(_T(""));
		}

		(*m_pView)->Navigate(CComBSTR(strUrl),NULL,   NULL,NULL,NULL);
	}
}


CRadioConfig* CRadioUOLMng::GetConfig()
{
	return m_pConfig;
}


HRESULT CRadioUOLMng::OnMouseClick(short  nButton,  short  nShiftState,  long  fX,  long  fY)
{
    return S_OK;
}
