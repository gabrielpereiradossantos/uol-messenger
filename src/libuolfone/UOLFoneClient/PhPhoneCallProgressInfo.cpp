/* UOL Fone
 * Copyright (c) 2006 Universo Online S/A
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
 * Universo Online S/A - A/C: UOL Fone 5o. Andar
 * Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
 * São Paulo SP - CEP 01452-002 - BRASIL  */


#include "stdafx.h"
#include "PhPhoneCallProgressInfo.h"
#include "ExtraResponseData.h"


CPhPhoneCallProgressInfo::CPhPhoneCallProgressInfo(int nCallId, const phCallStateInfo_t* pCallStateInfo)
{
	m_nCallId = nCallId;
	
	ATLASSERT(pCallStateInfo != NULL);
	m_pCallStateInfo = pCallStateInfo;
}


CPhPhoneCallProgressInfo::~CPhPhoneCallProgressInfo()
{
}


LONG CPhPhoneCallProgressInfo::GetLineId() const
{
	return m_pCallStateInfo->vlid;
}


LONG CPhPhoneCallProgressInfo::GetCallId() const
{
	return m_nCallId;
}


CString CPhPhoneCallProgressInfo::GetPhoneURI() const
{
	CString strURI;
	
	if (FALSE == ::IsBadReadPtr((m_pCallStateInfo->u).remoteUri, sizeof(const char*)))
	{
		strURI = (m_pCallStateInfo->u).remoteUri;
	}
	
	return strURI;
}


EnumPhoneCallState CPhPhoneCallProgressInfo::GetCallState() const
{
	int nCallStatus = m_pCallStateInfo->event;
	
	EnumPhoneCallState state = UFC_PHONE_CALL_STATE_UNKNOWN;
	
	switch (nCallStatus)
	{
		case phDIALING:
			ATLTRACE(_T("phDIALING\n"));
			state = UFC_PHONE_CALL_STATE_DIALING;
		break;
		
		case phRINGING:
			ATLTRACE(_T("phRINGING\n"));
			state = UFC_PHONE_CALL_STATE_RINGING;
		break;
		
		case phNOANSWER:
			ATLTRACE(_T("phNOANSWER\n"));
			state = UFC_PHONE_CALL_STATE_NO_ANSWER; 
		break;
		
		case phCALLBUSY:
			ATLTRACE(_T("phCALLBUSY\n"));
			state = UFC_PHONE_CALL_STATE_BUSY;
		break;
		
		case phCALLREDIRECTED:
			ATLTRACE(_T("phCALLREDIRECTED\n"));
		break;
		
		case phCALLOK:
			ATLTRACE(_T("phCALLOK\n"));
			state = UFC_PHONE_CALL_STATE_TALKING;
		break;
		
		case phCALLHELD:
			ATLTRACE(_T("phCALLHELD\n"));
		case phCALLRESUMED:
			ATLTRACE(_T("phCALLRESUMED\n"));
		break;
		
		case phHOLDOK:
			ATLTRACE(_T("phHOLDOK\n"));
			state = UFC_PHONE_CALL_STATE_HOLD;
		break;
		
		case phRESUMEOK:
			ATLTRACE(_T("phRESUMEOK\n"));
			state = UFC_PHONE_CALL_STATE_RESUMED;
		break;
		
		case phINCALL:
			ATLTRACE(_T("phINCALL\n"));
			state = UFC_PHONE_CALL_STATE_INCOMING;		
		break;
		
		case phCALLCLOSED:
			ATLTRACE(_T("phCALLCLOSED\n"));
			state = UFC_PHONE_CALL_STATE_CLOSED;
		break;
		
		case phCALLERROR:
			ATLTRACE(_T("phCALLERROR\n"));
			if ((603 == m_pCallStateInfo->u.errorCode) ||
					(480 == m_pCallStateInfo->u.errorCode) ||
					(408 == m_pCallStateInfo->u.errorCode))
			{
				//Decline
				state = UFC_PHONE_CALL_STATE_NO_ANSWER;
			}
			else
			{
				state = UFC_PHONE_CALL_STATE_ERROR;
			}
		break;
		
		case phDTMF:
			ATLTRACE(_T("phDTMF\n"));
		break;
		
		case phXFERPROGRESS:	// Transfer progress
		case phXFEROK:		// Transfer OK
		case phXFERFAIL:	// Transfer failed
		case phXFERREQ:		// Transfer request
			ATLTRACE(_T("phXfer...\n"));
		break;
		
		case phCALLREPLACED:
			ATLTRACE(_T("phCALLREPLACED\n"));
		break;
		
		case phRINGandSTART:
			ATLTRACE(_T("phRINGandSTART\n"));
			state = UFC_PHONE_CALL_STATE_RINGING;
		break;
		
		case phRINGandSTOP:
			ATLTRACE(_T("phRINGandSTOP\n"));
			state = UFC_PHONE_CALL_STATE_RINGING;
		break;
		
		case phCALLCLOSEDandSTOPRING:
			ATLTRACE(_T("phCALLCLOSEDandSTOPRING\n"));
			state = UFC_PHONE_CALL_STATE_CLOSED;
		break;
		
		default:
			state = UFC_PHONE_CALL_STATE_UNKNOWN;
	}
	
	return state;
}


BOOL CPhPhoneCallProgressInfo::HasExtraInfo() const
{
	return ( (lstrcmp(m_pCallStateInfo->reason_phrase, "")) || 
			(lstrcmp(m_pCallStateInfo->textinfo, "")) || 
			(m_pCallStateInfo->status_code != 0) || 
			(m_pCallStateInfo->error_source != 0) );
}


void CPhPhoneCallProgressInfo::GetExtraInfo(CAtlList<CHeaderDataPtr>& listHeaderInfo) const
{
	listHeaderInfo.RemoveAll();
	
	if (lstrcmp(m_pCallStateInfo->reason_phrase, ""))
	{
		CHeaderDataPtr pHeaderInfo = new CHeaderData(
				CExtraResponseData::GetExtraResponseDataName(CALL_ERROR_REASON_PHRASE), 
				CString(m_pCallStateInfo->reason_phrase));
		
		listHeaderInfo.AddTail(pHeaderInfo);
	}
	
	if (lstrcmp(m_pCallStateInfo->textinfo, ""))
	{
		CHeaderDataPtr pHeaderInfo = new CHeaderData(
				CExtraResponseData::GetExtraResponseDataName(CALL_ERROR_TEXT_INFO), 
				CString(m_pCallStateInfo->textinfo));
		
		listHeaderInfo.AddTail(pHeaderInfo);
	}
	
	if (m_pCallStateInfo->status_code != 0)
	{
		CString strStatusCode;
		strStatusCode.Format("%d", m_pCallStateInfo->status_code);
		
		CHeaderDataPtr pHeaderInfo = new CHeaderData(
				CExtraResponseData::GetExtraResponseDataName(CALL_ERROR_STATUS_CODE), 
				strStatusCode);
		
		listHeaderInfo.AddTail(pHeaderInfo);
	}
	
	if (m_pCallStateInfo->error_source != 0)
	{
		CString strErrorSource;
		strErrorSource.Format("%d", m_pCallStateInfo->error_source);
		
		CHeaderDataPtr pHeaderInfo = new CHeaderData(
				CExtraResponseData::GetExtraResponseDataName(CALL_ERROR_ERROR_SOURCE), 
				strErrorSource);
		
		listHeaderInfo.AddTail(pHeaderInfo);
	}
}

