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
#include "HistoryWindowMonitor.h"

#include "resource.h"
#include "Core/UOLFoneController.h"



CHistoryWindowMonitor::CHistoryWindowMonitor() : 
	m_pUOLMessengerFacade(NULL)
{
}


CHistoryWindowMonitor::~CHistoryWindowMonitor()
{
}


CHistoryWindowMonitor* CHistoryWindowMonitor::GetInstance()
{
	static CHistoryWindowMonitor historyWindowMonitor;
	
	return &historyWindowMonitor;
}


BOOL CHistoryWindowMonitor::Init(IUOLMessengerFacade* pUOLMessengerFacade)
{
	if (pUOLMessengerFacade)
	{
		IUOLMessengerUIManager5Ptr pUIManager = 
			ap_static_cast<IUOLMessengerUIManager5Ptr>(pUOLMessengerFacade->GetUIManager());
		
		if (pUIManager)
		{
			pUIManager->RegisterObserver(this);
			
			m_pUOLMessengerFacade = pUOLMessengerFacade;
			
			return TRUE;
		}
	}
	
	return FALSE;
}


void CHistoryWindowMonitor::Finalize()
{
	if (m_pUOLMessengerFacade)
	{
		IUOLMessengerUIManager5Ptr pUIManager = 
			ap_static_cast<IUOLMessengerUIManager5Ptr>(m_pUOLMessengerFacade->GetUIManager());
		
		if (pUIManager)
		{
			pUIManager->UnregisterObserver(this);
		}
	}
	
	{ CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

		m_mapHistoryWindow.RemoveAll();
	}
}

void CHistoryWindowMonitor::OnCreateHistoryWindow(IUOLMessengerHistoryWindow2* pHistoryWindow) 
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	AddAllButtons(pHistoryWindow);
}

void CHistoryWindowMonitor::OnDestroyHistoryWindow(IUOLMessengerHistoryWindow2* pHistoryWindow) 
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);
	RemoveAllButtons(pHistoryWindow);
}

void CHistoryWindowMonitor::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint& ptClick)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	IUOLMessengerHistoryWindow2* pHistoryWindow;
	
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_cs);
		
		pHistoryWindow = FindHistoryWindow(pButton);
	}

	CString strCommand;
	strCommand.LoadString(IDS_TOOLBAR_BTN_LABEL_CONVERSAS);
	if (pButton->GetCommandName().Compare(strCommand) == 0)
	{
		CString strStatusFilter;
		strStatusFilter.LoadString(IDS_HISTORY_CALL_STATUS_NONE);
		CFacadeWrapper::GetInstance()->ShowHistoryWindow(strStatusFilter);
	}
	else
	{
		strCommand.LoadString(IDS_TOOLBAR_BTN_LABEL_LIG_ATENDIDAS);
		if (pButton->GetCommandName().Compare(strCommand) == 0)
		{
			CString strStatusFilter;
			strStatusFilter.LoadString(IDS_HISTORY_CALL_STATUS_RECEIVED);
			CFacadeWrapper::GetInstance()->ShowHistoryWindow(strStatusFilter);
		}
		else
		{
			strCommand.LoadString(IDS_TOOLBAR_BTN_LABEL_LIG_EFETUADAS);
			if (pButton->GetCommandName().Compare(strCommand) == 0)
			{
				CString strStatusFilter;
				strStatusFilter.LoadString(IDS_HISTORY_CALL_STATUS_DIALED);
				CFacadeWrapper::GetInstance()->ShowHistoryWindow(strStatusFilter);
			}
			else
			{
				strCommand.LoadString(IDS_TOOLBAR_BTN_LABEL_LIG_PERDIDAS);
				if (pButton->GetCommandName().Compare(strCommand) == 0)
				{
					CString strStatusFilter;
					strStatusFilter.LoadString(IDS_HISTORY_CALL_STATUS_MISSED);
					CFacadeWrapper::GetInstance()->ShowHistoryWindow(strStatusFilter);
				}
				else
				{
					ATLASSERT(FALSE);
				}
			}
		}
	}
	
}

void CHistoryWindowMonitor::AddAllButtons(IUOLMessengerHistoryWindow2* pHistoryWindow)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	CImagesArrayPtr toolbarImages = new CAtlArray<IUOLMessengerImageButtonPtr>();
	IUOLMessengerImageButtonPtr pImageButton;

	if (IsVoipHistory(pHistoryWindow))
	{
		pImageButton = AddButton(pHistoryWindow, IDB_TOOLBAR_BTN_LIG_ATENDIDA, IDS_TOOLBAR_BTN_LABEL_LIG_ATENDIDAS);
		toolbarImages->Add(pImageButton);

		pImageButton = AddButton(pHistoryWindow, IDB_TOOLBAR_BTN_LIG_EFETUADA, IDS_TOOLBAR_BTN_LABEL_LIG_EFETUADAS);
		toolbarImages->Add(pImageButton);

		pImageButton = AddButton(pHistoryWindow, IDB_TOOLBAR_BTN_CALL_HISTORY, IDS_TOOLBAR_BTN_LABEL_LIG_PERDIDAS);
		toolbarImages->Add(pImageButton);

		m_mapHistoryWindow.SetAt(pHistoryWindow, toolbarImages);
	}
	else
	{
		// TODO: For future implementation - RFC
		// **********
		//pImageButton = AddButton(pHistoryWindow, IDB_TOOLBAR_BTN_CONVERSA, IDS_TOOLBAR_BTN_LABEL_CONVERSAS);
		//toolbarImages->Add(pImageButton);

		//m_mapHistoryWindow.SetAt(pHistoryWindow, toolbarImages);
		// **********

		m_mapHistoryWindow.RemoveKey(pHistoryWindow);
	}
}

IUOLMessengerImageButtonPtr CHistoryWindowMonitor::AddButton(IUOLMessengerHistoryWindow2* pHistoryWindow, int nBtnImage, int nTooltip)
{
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

		IUOLMessengerImageButtonContainerPtr pImageButtonContainer = pHistoryWindow->GetToolbar();
		
		if ( (pImageButtonContainer))
		{

			IUOLMessengerImageButtonPtr pImageButton = pImageButtonContainer->CreateExternalButton();

			if (pImageButton)
			{
				IUOLMessengerUIManager4Ptr pUIManager = 
					ap_static_cast<IUOLMessengerUIManager4Ptr>(m_pUOLMessengerFacade->GetUIManager());

				IUOLMessengerImagePtr pUOLFoneButtonImage = pUIManager->LoadImage(
					ATL::_AtlBaseModule.GetResourceInstance(), nBtnImage);
				//pUOLFoneButtonImage.reset();

				CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

				CString strTooltipText;
				strTooltipText.LoadString(nTooltip);

				pImageButton->SetTip(strTooltipText);
				pImageButton->SetImage(pUOLFoneButtonImage);
				pImageButton->SetWidth(32);
				//pImageButton->SetPushButton(TRUE);
				pImageButton->SetAuto(FALSE);
				pImageButton->SetDrawTransparent(TRUE);
				//pImageButton->SetPressed(FALSE);
				pImageButton->SetCommandName(strTooltipText);

				pImageButtonContainer->AddExternalButton(pImageButton);

				pImageButton->AddEventListener(this);

				return pImageButton;
			}
		}
	}
	return NULL;
}


void CHistoryWindowMonitor::RemoveAllButtons(IUOLMessengerHistoryWindow2* pHistoryWindow)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

		IUOLMessengerImageButtonContainerPtr pImageButtonContainer = pHistoryWindow->GetToolbar();

		if (pImageButtonContainer)
		{
			CHistoryWindowMonitor::CImagesArrayPtr pToolbarImages = FindHistoryWindowImageButton(pHistoryWindow);

			if (pToolbarImages !=  NULL)
			{
				int size = (int)pToolbarImages->GetCount();
				for (int i = 0; i < size; i++)
				{
					IUOLMessengerImageButtonPtr pImageButton = pToolbarImages->GetAt(i);
					if (pImageButton)
					{
						pImageButton->RemoveEventListener(this);

						pImageButtonContainer->RemoveExternalButton(pImageButton);
					}
				}
			}

			m_mapHistoryWindow.RemoveKey(pHistoryWindow);
		}

	//	pHistoryWindow->UnregisterObserver(this);
	}
}

IUOLMessengerHistoryWindow2* CHistoryWindowMonitor::FindHistoryWindow(IUOLMessengerImageButton* pButton)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

	for (POSITION posI = m_mapHistoryWindow.GetStartPosition(); posI; 
			m_mapHistoryWindow.GetNextValue(posI))
	{
		CHistoryWindowMonitor::CImagesArrayPtr pTempArray = m_mapHistoryWindow.GetValueAt(posI);

		int size = (int)pTempArray->GetCount();
		for (int i = 0; i < size; i++)
		{
			IUOLMessengerImageButtonPtr pTempButton = pTempArray->GetAt(i);

			if (pTempButton.get() == pButton)
			{
				return m_mapHistoryWindow.GetKeyAt(posI);
			}
		}
	}
	
	return NULL;
}

CHistoryWindowMonitor::CImagesArrayPtr CHistoryWindowMonitor::FindHistoryWindowImageButton(IUOLMessengerHistoryWindow2* pHistoryWindow)
{
	if (pHistoryWindow)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

		CMapHistoryWindow::CPair* pPair = m_mapHistoryWindow.Lookup(pHistoryWindow);
		
		if (pPair)
		{
			return pPair->m_value;
		}
	}
	
	return NULL;
}

BOOL CHistoryWindowMonitor::IsVoipHistory(IUOLMessengerHistoryWindow2* pHistoryWindow)
{
	CString strProtocolId;
	strProtocolId.LoadString(IDS_VOIP_PROTOCOL_ID);

	return (0 == strProtocolId.CompareNoCase(pHistoryWindow->GetProtocolId()));
}