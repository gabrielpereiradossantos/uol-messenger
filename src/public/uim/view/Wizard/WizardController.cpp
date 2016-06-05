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
#include "WizardController.h"

#include "WizardMSNAccountDialog.h"
#include "WizardUOLAccountDialog.h"
#include "WizardICQAccountDialog.h"
#include "WizardYahooAccountDialog.h"
#include "WizardJabberAccountDialog.h"
#include "WizardPreferencesDialog.h"


CWizardController::CWizardController() : 
		m_bConfigMSN(FALSE),
		m_bConfigUOL(FALSE),
		m_bConfigICQ(FALSE),
		m_bConfigYahoo(FALSE),
		m_bConfigJabber(FALSE)
{
	m_pWizardSettings = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	
	m_pAccountsList = new AccountsList();
}


CWizardController::~CWizardController()
{
}


CWizardController* CWizardController::GetInstance()
{
	static CWizardController instance;

	return &instance;
}


void CWizardController::SetProtocolsToConfig(
		BOOL bConfigMSN, BOOL bConfigUOL, BOOL bConfigICQ, 
		BOOL bConfigYahoo, BOOL bConfigJabber)
{
	m_bConfigMSN = bConfigMSN;
	m_bConfigUOL = bConfigUOL;
	m_bConfigICQ = bConfigICQ;
	m_bConfigYahoo = bConfigYahoo;
	m_bConfigJabber = bConfigJabber;
}


BOOL CWizardController::HaveProtocolsToConfig()
{
	return (m_bConfigMSN || m_bConfigUOL || m_bConfigICQ || m_bConfigYahoo || m_bConfigJabber);
}


CUOLPropPagePtr CWizardController::GetNextValidPage(int iWizardId)
{
	CUOLPropPagePtr pReturningPage = NULL;
	IUOLMessengerProfileManagerPtr pProfileManager = 
			CUIMApplication::GetApplication()->GetProfileManager();
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	
	int iNextWizardPage = GetNextWizardPageID(iWizardId);
	
	if (iNextWizardPage == ID_UOL_PROTOCOL_WIZARD_PAGE)
	{
		// Lets check whether we have a valid UOL user.
		//
		// As this test use to be made on OnInitDialog method of 
		// CWizardMSNAccountDialog, lets make it the same way.
		//
		CWizardMSNAccountDialogPtr pTempPropPage = 
				ap_dynamic_cast<CWizardMSNAccountDialogPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__WizardMSNInstance"));
		
		if (pTempPropPage->ValidateProfileData(pProfileManager, m_pAccountsList.get(), m_pWizardSettings))
		{
			// UOL user already created with profile.
			// Make a new search from it.
			// 
			iNextWizardPage = GetNextWizardPageID(ID_UOL_PROTOCOL_WIZARD_PAGE);
		}
	}
	
	if (iNextWizardPage == ID_MSN_PROTOCOL_WIZARD_PAGE)
	{
		pReturningPage = ap_dynamic_cast<CWizardMSNAccountDialogPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__WizardMSNInstance"));
		
		ap_static_cast<CWizardMSNAccountDialogPtr>(pReturningPage)->SetAccountsList(m_pAccountsList.get());
	}
	else if (iNextWizardPage == ID_UOL_PROTOCOL_WIZARD_PAGE)
	{
		pReturningPage = ap_dynamic_cast<CWizardUOLAccountDialogPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__WizardUOLInstance"));
		
		ap_static_cast<CWizardUOLAccountDialogPtr>(pReturningPage)->SetAccountsList(m_pAccountsList.get());
	}
	else if (iNextWizardPage == ID_ICQ_PROTOCOL_WIZARD_PAGE)
	{
		pReturningPage = ap_dynamic_cast<CWizardICQAccountDialogPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__WizardICQInstance"));
		
		ap_static_cast<CWizardICQAccountDialogPtr>(pReturningPage)->SetAccountsList(m_pAccountsList.get());
	}
	else if (iNextWizardPage == ID_YAHOO_PROTOCOL_WIZARD_PAGE)
	{
		pReturningPage = ap_dynamic_cast<CWizardYahooAccountDialogPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__WizardYahooInstance"));
		
		ap_static_cast<CWizardYahooAccountDialogPtr>(pReturningPage)->SetAccountsList(m_pAccountsList.get());
	}
	else if (iNextWizardPage == ID_JABBER_PROTOCOL_WIZARD_PAGE)
	{
		pReturningPage = ap_dynamic_cast<CWizardJabberAccountDialogPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__WizardJabberInstance"));
		
		ap_static_cast<CWizardJabberAccountDialogPtr>(pReturningPage)->SetAccountsList(m_pAccountsList.get());
	}
	else if (iNextWizardPage == ID_PREFERENCES_WIZARD_PAGE)
	{
		pReturningPage = ap_dynamic_cast<CWizardPreferencesDialogPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__WizardPreferencesInstance"));
		
		ap_static_cast<CWizardPreferencesDialogPtr>(pReturningPage)->SetAccountsList(m_pAccountsList.get());
	}
	
	return pReturningPage;
}


int CWizardController::GetNextWizardPageID(int iCurrentWizardID)
{
	int iNextWizardPage = ID_PREFERENCES_WIZARD_PAGE;
	
	if (iCurrentWizardID == ID_PROFILE_WIZARD_PAGE)
	{
		if (m_bConfigMSN)
		{
			iNextWizardPage = ID_MSN_PROTOCOL_WIZARD_PAGE;
		}
		else if (m_bConfigUOL)
		{
			iNextWizardPage = ID_UOL_PROTOCOL_WIZARD_PAGE;
		}
		else if (m_bConfigICQ)
		{
			iNextWizardPage = ID_ICQ_PROTOCOL_WIZARD_PAGE;
		}
		else if (m_bConfigYahoo)
		{
			iNextWizardPage = ID_YAHOO_PROTOCOL_WIZARD_PAGE;
		}
		else if (m_bConfigJabber)
		{
			iNextWizardPage = ID_JABBER_PROTOCOL_WIZARD_PAGE;
		}
	}
	else if (iCurrentWizardID == ID_MSN_PROTOCOL_WIZARD_PAGE)
	{
		if (m_bConfigUOL)
		{
			iNextWizardPage = ID_UOL_PROTOCOL_WIZARD_PAGE;
		}
		else if (m_bConfigICQ)
		{
			iNextWizardPage = ID_ICQ_PROTOCOL_WIZARD_PAGE;
		}
		else if (m_bConfigYahoo)
		{
			iNextWizardPage = ID_YAHOO_PROTOCOL_WIZARD_PAGE;
		}
		else if (m_bConfigJabber)
		{
			iNextWizardPage = ID_JABBER_PROTOCOL_WIZARD_PAGE;
		}
	}
	else if (iCurrentWizardID == ID_UOL_PROTOCOL_WIZARD_PAGE)
	{
		if (m_bConfigICQ)
		{
			iNextWizardPage = ID_ICQ_PROTOCOL_WIZARD_PAGE;
		}
		else if (m_bConfigYahoo)
		{
			iNextWizardPage = ID_YAHOO_PROTOCOL_WIZARD_PAGE;
		}
		else if (m_bConfigJabber)
		{
			iNextWizardPage = ID_JABBER_PROTOCOL_WIZARD_PAGE;
		}
	}
	else if (iCurrentWizardID == ID_ICQ_PROTOCOL_WIZARD_PAGE)
	{
		if (m_bConfigYahoo)
		{
			iNextWizardPage = ID_YAHOO_PROTOCOL_WIZARD_PAGE;
		}
		else if (m_bConfigJabber)
		{
			iNextWizardPage = ID_JABBER_PROTOCOL_WIZARD_PAGE;
		}
	}
	else if (iCurrentWizardID == ID_YAHOO_PROTOCOL_WIZARD_PAGE)
	{
		if (m_bConfigJabber)
		{
			iNextWizardPage = ID_JABBER_PROTOCOL_WIZARD_PAGE;
		}
	}
	
	return iNextWizardPage;
}

