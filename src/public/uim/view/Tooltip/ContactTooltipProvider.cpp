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
#include "ContactTooltipProvider.h"

#include "../../resource.h"
#include "../../controller/UIMApplication.h"
#include "../../view/UserIcon.h"


CContactTooltipProvider::CContactTooltipProvider(IUOLMessengerContactPtr pContact)
{
	m_pContact = pContact;
}


CContactTooltipProvider::~CContactTooltipProvider()
{
}


CString CContactTooltipProvider::GetTitle() const
{
	ATLASSERT(m_pContact);

	CString strTitle;
	CString strContact = m_pContact->GetAlias();

	IUOLMessengerContact4Ptr pContact4 = ap_dynamic_cast<IUOLMessengerContact4Ptr>(m_pContact);
	if (pContact4)
	{
		CString strStatusMessage = pContact4->GetStatusMessage();
		if (!strStatusMessage.IsEmpty())
		{
			strTitle.Format("%s - %s", strContact, strStatusMessage);
		}
		else
		{
			strTitle = strContact;
		}
	}
	else
	{
		strTitle = strContact;
	}

	return strTitle;
}


IUOLMessengerImagePtr CContactTooltipProvider::GetImage() const
{
	ATLASSERT(m_pContact);

	IUOLMessengerUserIconPtr pUserIcon = m_pContact->GetUserIcon();
	
	if (pUserIcon != NULL)
	{
		return pUserIcon->GetImage();
	}
    else
    {
        // Return default user icon
        CString protocolId = m_pContact->GetAccount()->GetProtocolId();

        IUOLMessengerImagesProtocolPtr pImagesProtocol = CUIMApplication::GetApplication()->GetUIManager()->GetImagesProtocol();
        ATLASSERT(pImagesProtocol.get()!=NULL);

        CString strIconPath = pImagesProtocol->GetContactImagePath(protocolId, 1);
        IUOLMessengerImagePtr pImage = pImagesProtocol->GetContactImage(protocolId, 1);

        pUserIcon = new CUserIcon(pImage, strIconPath);

        return pUserIcon->GetImage();
    }


	return IUOLMessengerImagePtr();
}


void CContactTooltipProvider::GetTextLines(CAtlList<CString>& listLines) const
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();

    IUOLMessengerAccountPtr pAccount;

	listLines.RemoveAll();

	ATLASSERT(m_pContact);

	// Add user line...
	CString strAccountLine;

	pAccount = m_pContact->GetAccount();

	ATLASSERT(pAccount != NULL);

	if (pAccount != NULL)
	{
        strAccountLine.Format(IDS_CONTACT_TOOLTIP_ACCOUNT_LINE_FORMAT, 
			pAccount->GetProtocolName(), m_pContact->GetName());
	}

	listLines.AddTail(strAccountLine);

	// Add status line...
	CString strStatusLine;

    strStatusLine = pStringLoader->GetValue("uol.resource.IDS_CONTACT_TOOLTIP_STATUS_LINE");
		
	DWORD dwStatus = m_pContact->GetStatus();

	CString strStatus;
	switch(dwStatus)
	{
	case IUOLMessengerContact::CS_ONLINE:
		strStatus.Format(IDS_STATUS_ONLINE);
		break;
	case IUOLMessengerContact::CS_OFFLINE:
		strStatus.Format(IDS_STATUS_OFFLINE);
		break;
	case IUOLMessengerContact::CS_AWAY:
		strStatus.Format(IDS_STATUS_AWAY);
		break;
	case IUOLMessengerContact::CS_IDLE:
		strStatus.Format(IDS_STATUS_IDLE);
		break;
	case IUOLMessengerContact::CS_BUSY:
		strStatus.Format(IDS_STATUS_BUSY);
		break;
    default:
        strStatus = pStringLoader->GetValue("uol.resource.IDS_CONTACT_TOOLTIP_ND");
        break;
	};

	strStatusLine += strStatus;

	if (CUIMApplication::GetApplication()->GetPrivacyManager()->IsDenied(pAccount, m_pContact->GetName()))
	{
		CString strBlocked;
		strBlocked.Format(IDS_CONTACT_TOOLTIP_BLOCKED, strStatusLine);
		strStatusLine = strBlocked;
	}

	listLines.AddTail(strStatusLine);

	// Add group line...
	CString strGroupLine;
	strGroupLine = pStringLoader->GetValue("uol.resource.IDS_CONTACT_TOOLTIP_GROUP_LINE");

	if (m_pContact->GetGroup().get())
	{
		strGroupLine += m_pContact->GetGroup()->GetName();
	}
    else
    {
        strGroupLine += pStringLoader->GetValue("uol.resource.IDS_CONTACT_TOOLTIP_ND");
    }

	listLines.AddTail(strGroupLine);

	// Add contact of account line...
	CString strAccountOfLine;
	strAccountOfLine = pStringLoader->GetValue("uol.resource.IDS_CONTACT_TOOLTIP_CONTACT_OF_ACCOUNT_LINE");
	
	if (pAccount != NULL)
	{
		strAccountOfLine += pAccount->GetUserId();
	}
    else
    {
        strAccountOfLine += pStringLoader->GetValue("uol.resource.IDS_CONTACT_TOOLTIP_ND");
    }

	listLines.AddTail(strAccountOfLine);
}
