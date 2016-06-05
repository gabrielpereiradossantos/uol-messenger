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
#include "FacadeWrapper.h"
#include "./Core/Channel.h"
#include <algorithm>


bool ChannelComparator(CChannel * elem1, CChannel *elem2)
{
	return (elem1->GetName().CompareNoCase(elem2->GetName())>0);
}


CFacadeWrapper::~CFacadeWrapper()
{
}


CFacadeWrapper* CFacadeWrapper::GetInstance()
{
	static CFacadeWrapper instance;

	return &instance;
}


IUOLMessengerFacade* CFacadeWrapper::GetFacadeInstance()
{
	return CFacadeWrapper::GetInstance()->GetFacade();
}


void CFacadeWrapper::Init(IUOLMessengerFacade* pFacade, CUOLMessengerPlugin* pRadioUOLPlugin)
{
	m_pFacade = pFacade;
	m_pRadioUOLPlugin = pRadioUOLPlugin;
}


void CFacadeWrapper::Finalize()
{
	m_pFacade = NULL;
	m_pRadioUOLPlugin = NULL;
}


IUOLMessengerFacade* CFacadeWrapper::GetFacade()
{
	return m_pFacade;
}


CFacadeWrapper::CFacadeWrapper() : 
	m_pFacade(NULL), 
	m_pRadioUOLPlugin(NULL), 
	m_bHasBuiltRadioSystrayMenuOnce(FALSE), 
	m_bLastEnablePlay(-1), 
	m_bLastEnableStoppers(-1), 
	m_bLastEnableNavigation(-1)
{
}


BOOL CFacadeWrapper::HasBuiltRadioSystrayMenuOnce()
{
	return m_bHasBuiltRadioSystrayMenuOnce;
}


void CFacadeWrapper::BuildRadioSystrayMenu()
{
	if (m_pRadioUOLSystrayMenu)
	{
		m_pChannelsMenu->RemoveAllMenuItems(m_pRadioUOLPlugin);
		
		BuildChannelsMenu(CRadioUOLWrapper::GetInstance()->GetFacade()->GetChannels());
		
		return;
	}
	
	IUOLMessengerUIManager8Ptr pUIManager8 = 
			ap_dynamic_cast<IUOLMessengerUIManager8Ptr>(
					m_pFacade->GetUIManager());
	
	if (pUIManager8)
	{
		pUIManager8->WaitSystrayMenuInitialization(m_pRadioUOLPlugin);
		
		m_pRadioUOLSystrayMenu = pUIManager8->AddSystrayMenuItem(
				m_pRadioUOLPlugin, "");
	}
	
	if (m_pRadioUOLSystrayMenu)
	{
		CString itemLabel;
		
		itemLabel.LoadString(IDS_CHANNELS_MENU_LABEL);
		m_pChannelsMenu = m_pRadioUOLSystrayMenu->AddSubMenu(
				m_pRadioUOLPlugin, itemLabel);
		
		itemLabel.LoadString(IDS_PRIOR_MENU_LABEL);
		m_pPriorMenuItem = m_pRadioUOLSystrayMenu->AddMenuItem(
				m_pRadioUOLPlugin, IDS_PRIOR_MENU_LABEL, itemLabel, this, TRUE);
		
		itemLabel.LoadString(IDS_NEXT_MENU_LABEL);
		m_pNextMenuItem = m_pRadioUOLSystrayMenu->AddMenuItem(
				m_pRadioUOLPlugin, IDS_NEXT_MENU_LABEL, itemLabel, this);
		
		itemLabel.LoadString(IDS_PLAY_MENU_LABEL);
		m_pPlayMenuItem = m_pRadioUOLSystrayMenu->AddMenuItem(
				m_pRadioUOLPlugin, IDS_PLAY_MENU_LABEL, itemLabel, this);
		
		itemLabel.LoadString(IDS_PAUSE_MENU_LABEL);
		m_pPauseMenuItem = m_pRadioUOLSystrayMenu->AddMenuItem(
				m_pRadioUOLPlugin, IDS_PAUSE_MENU_LABEL, itemLabel, this);
		
		itemLabel.LoadString(IDS_STOP_MENU_LABEL);
		m_pStopMenuItem = m_pRadioUOLSystrayMenu->AddMenuItem(
				m_pRadioUOLPlugin, IDS_STOP_MENU_LABEL, itemLabel, this);
		
		UpdatePlayerMenuItens(wmppsUndefined);
		
		BuildChannelsMenu(CRadioUOLWrapper::GetInstance()->GetFacade()->GetChannels());
		
		m_bHasBuiltRadioSystrayMenuOnce = TRUE;
	}
}


void CFacadeWrapper::ClearRadioSystrayMenu()
{
	if (m_pRadioUOLSystrayMenu)
	{
		IUOLMessengerUIManager8Ptr pUIManager8 = 
				ap_dynamic_cast<IUOLMessengerUIManager8Ptr>(
						m_pFacade->GetUIManager());
		
		if (pUIManager8)
		{
			pUIManager8->WaitSystrayMenuInitialization(m_pRadioUOLPlugin);
			
			m_pRadioUOLSystrayMenu->RemoveSubMenu(m_pRadioUOLPlugin, m_pChannelsMenu);
			m_pChannelsMenu.reset();
			
			m_pRadioUOLSystrayMenu->RemoveAllMenuItems(m_pRadioUOLPlugin);
			
			pUIManager8->RemoveSystrayMenuItem(m_pRadioUOLPlugin, "");
			m_pRadioUOLSystrayMenu.reset();
		}
		
		m_bLastEnablePlay = -1;
		m_bLastEnableStoppers = -1;
		m_bLastEnableNavigation = -1;
		
		m_bHasBuiltRadioSystrayMenuOnce = FALSE;
	}
}


void CFacadeWrapper::UpdatePlayerMenuItens(WMPPlayState wmpPlayState)
{
	BOOL bEnablePlay;
	BOOL bEnableStoppers;
	BOOL bEnableNavigation;
	BOOL bIsChangingOption = FALSE;
	
	if (m_pRadioUOLSystrayMenu)
	{
		switch (wmpPlayState)
		{
			case wmppsUndefined:
				
				bEnablePlay = FALSE;
				bEnableStoppers = FALSE;
				bEnableNavigation = FALSE;
				bIsChangingOption = TRUE;
				
			break;
			
			case wmppsStopped:
			case wmppsPaused:
				
				bEnablePlay = TRUE;
				bEnableStoppers = FALSE;
				bEnableNavigation = TRUE;
				bIsChangingOption = TRUE;
				
			break;
			
			case wmppsPlaying:
				
				bEnablePlay = FALSE;
				bEnableStoppers = TRUE;
				bEnableNavigation = TRUE;
				bIsChangingOption = TRUE;
				
			break;
		}
		
		if (bIsChangingOption)
		{
			if (bEnablePlay != m_bLastEnablePlay)
			{
				m_bLastEnablePlay = bEnablePlay;
				
				m_pRadioUOLSystrayMenu->EnableMenuItem(
						m_pRadioUOLPlugin, m_pPlayMenuItem, bEnablePlay);
			}
			
			if (bEnableStoppers != m_bLastEnableStoppers)
			{
				m_bLastEnableStoppers = bEnableStoppers;
				
				m_pRadioUOLSystrayMenu->EnableMenuItem(
						m_pRadioUOLPlugin, m_pPauseMenuItem, bEnableStoppers);
				m_pRadioUOLSystrayMenu->EnableMenuItem(
						m_pRadioUOLPlugin, m_pStopMenuItem, bEnableStoppers);
			}
			
			if (bEnableNavigation != m_bLastEnableNavigation)
			{
				m_bLastEnableNavigation = bEnableNavigation;
				
				m_pRadioUOLSystrayMenu->EnableMenuItem(
						m_pRadioUOLPlugin, m_pPriorMenuItem, bEnableNavigation);
				m_pRadioUOLSystrayMenu->EnableMenuItem(
						m_pRadioUOLPlugin, m_pNextMenuItem, bEnableNavigation);
			}
		}
	}
}


void CFacadeWrapper::OnCommand(IUOLMessengerContextMenuItem* pItem)
{
	if (pItem == NULL)
	{
		return;
	}
	
	switch (pItem->GetCommand())
	{
		case IDS_PRIOR_MENU_LABEL:
			CRadioUOLWrapper::GetInstance()->GetFacade()->PlayerPrevious();
		break;
		
		case IDS_NEXT_MENU_LABEL:
			CRadioUOLWrapper::GetInstance()->GetFacade()->PlayerNext();
		break;
		
		case IDS_PLAY_MENU_LABEL:
			CRadioUOLWrapper::GetInstance()->GetFacade()->PlayerPlay();
		break;
		
		case IDS_PAUSE_MENU_LABEL:
			CRadioUOLWrapper::GetInstance()->GetFacade()->PlayerPause();
		break;
		
		case IDS_STOP_MENU_LABEL:
			CRadioUOLWrapper::GetInstance()->GetFacade()->PlayerStop();
		break;
		
		default:
			CRadioUOLWrapper::GetInstance()->GetFacade()->PlaySelectedChannel(pItem->GetCommand());
		break;
	}
}


void CFacadeWrapper::BuildChannelsMenu(CAtlList<CStyle*>& channelsList)
{
	CStyle* pStyle = NULL;
	IUOLMessengerContextMenuPtr pStyleMenu;
	
	for (POSITION pos = channelsList.GetHeadPosition(); pos; )
	{
		CStyle* pStyle = channelsList.GetNext(pos);
		std::vector<CChannel*> &channels = pStyle->GetChannel();
		
		pStyleMenu = m_pChannelsMenu->AddSubMenu(
				m_pRadioUOLPlugin, pStyle->GetName());
		
		if (pStyleMenu)
		{
			int iChannelIndex, iTotalChannels;
			iTotalChannels = (int) channels.size();
			
			std::sort(channels.begin(), channels.end(), ChannelComparator);
			
			for (iChannelIndex = (iTotalChannels - 1); iChannelIndex >= 0; iChannelIndex--)
			{
				CChannel* pChannel = channels[iChannelIndex];
				
				pStyleMenu->AddMenuItem(m_pRadioUOLPlugin, 
						atoi(pChannel->GetId()), (LPCTSTR)pChannel->GetName(), this);
			}
		}
	}
}

