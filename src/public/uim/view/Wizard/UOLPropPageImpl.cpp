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
#include "UOLPropPageImpl.h"
#include "UOLWizard.h"

#include <commands/OpenBrowserCommand.h>
#include <jabberconstants.h>

#include "../../controller/UIMApplication.h"


const CString CUOLPropPageImpl::m_strNextText = _T("&Avançar");
const CString CUOLPropPageImpl::m_strPreviousText = _T("&Voltar");
const CString CUOLPropPageImpl::m_strCancelText = _T("&Sair");

#define MAX_LEN_PORT		5
#define MAX_PORT		65535


CUOLPropPageImpl::CUOLPropPageImpl()
{
	m_BoldFont.CreateFont(40, 0, 1,
			1, FW_BOLD, FALSE, FALSE,
			FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
			_T("Arial"));
}

CUOLPropPageImpl::~CUOLPropPageImpl()
{
}

void CUOLPropPageImpl::SetUOLWizard(CUOLWizard* pWizard)
{
	__super::SetUOLWizard(pWizard);

	m_pValidate = new CValidateAccountsDialog(m_pWizard->GetWizardSettings());

	if (m_strTitle.IsEmpty())
	{
		if (m_pWizard)
		{
			UOLLib::ISettings* pResourceSettings = m_pWizard->GetWizardSettings();

			if (pResourceSettings)
			{
				m_strTitle = pResourceSettings->GetValue(_T("uol.resource.IDS_APP_NAME"));
			}
		}
	}

	::SetWindowText(m_pWizard->m_hWnd, m_strTitle);
}

BOOL CUOLPropPageImpl::HasNextPage() 
{
	return FALSE; 
}	

CString CUOLPropPageImpl::GetNextBtnText() 
{ 
	return m_strNextText; 
}	

CUOLPropPagePtr CUOLPropPageImpl::GetNextPage(CUOLPropPagePtr pCurrentProp) 
{
	return m_pNextPage;
}	

BOOL CUOLPropPageImpl::HasPreviousPage() 
{
	return FALSE; 
}

CString CUOLPropPageImpl::GetPreviousBtnText() 
{
	return m_strPreviousText; 
}	

CUOLPropPagePtr CUOLPropPageImpl::GetPreviousPage(CUOLPropPagePtr pCurrentProp) 
{
    return m_pPreviousPage;
}

CString CUOLPropPageImpl::GetCancelBtnText() 
{ 
	return m_strCancelText; 
}	

BOOL CUOLPropPageImpl::OnBtnPrevious() 
{
	return FALSE;
}	

BOOL CUOLPropPageImpl::OnBtnNext()
{
	return FALSE;
}

BOOL CUOLPropPageImpl::OnBtnCancel() 
{
	return TRUE;
}

void CUOLPropPageImpl::OnBtnCustom(CUOLPropPagePtr pCurrentProp, UINT nCmdId)
{
	CString strSignUrl;

	if (nCmdId == ID_BTNCUSTOM)
	{
        strSignUrl = m_pWizard->GetWizardSettings()->GetValue(_T("uol.resource.IDS_URL_SIGNATURE_UOL"));

        CUIMApplication::GetApplication()->ExecuteCommand(new COpenBrowserCommand(strSignUrl));
	}
}


void CUOLPropPageImpl::CreateBtnCustom(int x, int width)
{
	m_pWizard->CreateBtnCustom(x, width);
}

void CUOLPropPageImpl::ShowBtnCustom(BOOL bShow, const CString& strText)
{
	m_pWizard->ShowBtnCustom(bShow, strText);
}

void CUOLPropPageImpl::Finalize()
{
    m_pPreviousPage.reset();
	m_pNextPage.reset();
	m_pBitmapBackgroundElement.reset();
    m_pValidate.reset();
}

void CUOLPropPageImpl::SetTitle(const CString& strTitle)
{
	m_strTitle = strTitle;
}

BOOL CUOLPropPageImpl::ValidateProfileData(IUOLMessengerProfileManagerPtr pProfileManager, CAtlList<IUOLMessengerAccountPtr>* pAccountsList, UOLLib::ISettings* pWizardSettings)
{
	IUOLMessengerProfileElementPtr pProfileElement;

	CProtocolSettingsPtr pProtocolSettings;	

	pProfileManager->GetProfileElement(pProfileManager->GetCurrentProfile(), pProfileElement);

	CString strEmail(pProfileElement->GetProfileName());
	CString strPassword(pProfileElement->GetProfilePassword());
	CString strDomain;
	CString strServer;
	CString strPort(_T("5222"));
	CString strProtocolId(_T("prpl-uol"));
	
	int nPort;

	BOOL bValid = FALSE;

	struct DomainServer
	{
		CString nDomainResId;
		CString nServerResId;
	};

	DomainServer domainServers[2];
	
	if (pWizardSettings != NULL)
	{
		domainServers[0].nDomainResId = pWizardSettings->GetValue("uol.resource.IDS_JABBER_UOL_EMAIL_DOMAIN");
		domainServers[0].nServerResId = pWizardSettings->GetValue("uol.resource.IDS_JABBER_UOL_SERVER");
		domainServers[1].nDomainResId = pWizardSettings->GetValue("uol.resource.IDS_JABBER_BOL_EMAIL_DOMAIN");
		domainServers[1].nServerResId = pWizardSettings->GetValue("uol.resource.IDS_JABBER_UOL_SERVER");
	}
	else
	{
		domainServers[0].nDomainResId = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_JABBER_UOL_EMAIL_DOMAIN");
		domainServers[0].nServerResId = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_JABBER_UOL_SERVER");
		domainServers[1].nDomainResId = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_JABBER_BOL_EMAIL_DOMAIN");
		domainServers[1].nServerResId = m_pWizard->GetWizardSettings()->GetValue("uol.resource.IDS_JABBER_UOL_SERVER");
	}

	int nCount = sizeof(domainServers) / sizeof(domainServers[0]);

	for (int nInd = 0; nInd < nCount; nInd++)
	{
		strDomain = domainServers[nInd].nDomainResId;

		if (strEmail.Find(strDomain) != -1)
		{
			strServer = domainServers[nInd].nServerResId;
			break;
		}
	}

	// Create UOL account
	if (!(strEmail.IsEmpty() || strPassword.IsEmpty() || strServer.IsEmpty() ||strPort.IsEmpty()) && ((strPort.GetLength() > MAX_LEN_PORT) || !((nPort = atoi(strPort)) == 0) || ((nPort = atoi(strPort)) > MAX_PORT)))
	{
		IUOLMessengerAccountPtr pAccount = CUIMApplication::GetApplication()->GetAccountManager()->CreateAccount(strEmail, strProtocolId);

		if (pAccount)
		{
			pAccount->SetPassword(strPassword);

			pProtocolSettings = pAccount->GetProtocolSettings();

			if (pProtocolSettings)
			{
				pProtocolSettings->SetString(JABBER_SETTING_CONNECT_SERVER, strServer.GetString());
				pProtocolSettings->SetInteger(JABBER_SETTING_CONNECT_PORT, nPort);
			}

			CUIMApplication::GetApplication()->GetAccountManager()->AddAccount(pAccount);

			if (m_pValidate.get() == NULL)
			{
				m_pValidate = new CValidateAccountsDialog(pWizardSettings);
			}
				
			if (m_pValidate->DoValidation(CValidateAccountsDialog::ID_JABBER_UOL_AUTH, pAccount) == IDOK)
			{
				pAccountsList->AddHead(pAccount);

				strEmail.Empty();
				strPassword.Empty();
				strServer.Empty();

				bValid = TRUE;
			}
			else
			{
				CUIMApplication::GetApplication()->GetAccountManager()->RemoveAccount(pAccount);
			}
		}
	}

	return bValid;
}