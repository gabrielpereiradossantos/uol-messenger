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
 

#include "PurpleRequestFileTransferAsyncInfo.h"
#include "PurpleUtils.h"

using namespace std;


CPurpleRequestFileTransferAsyncInfo::CPurpleRequestFileTransferAsyncInfo() :
		m_pXfer(NULL),
		m_cbOK(NULL),
		m_cbCancel(NULL),
		m_pUserData(NULL),
		m_strFileName(""),
		m_bAccepted(false),
		m_bRejected(false),
		m_bCanceledLocal(false),
		m_bCanceledRemote(false)
{
}


CPurpleRequestFileTransferAsyncInfo::~CPurpleRequestFileTransferAsyncInfo()
{
	if (m_pXfer)
	{
		purple_xfer_unref(m_pXfer);
	}
}


CPurpleRequestFileTransferAsyncInfo* CPurpleRequestFileTransferAsyncInfo::Clone()
{
	CPurpleRequestFileTransferAsyncInfo* pNewFileTransferAsyncInfo = 
			new CPurpleRequestFileTransferAsyncInfo();
	
	if (pNewFileTransferAsyncInfo)
	{
		purple_xfer_ref(GetPurpleXFer());
		
		pNewFileTransferAsyncInfo->SetPurpleXFer(GetPurpleXFer());
		pNewFileTransferAsyncInfo->SetOkCallback(GetOkCallback());
		pNewFileTransferAsyncInfo->SetCancelCallback(GetCancelCallback());
		pNewFileTransferAsyncInfo->SetUserData(GetUserData());
		pNewFileTransferAsyncInfo->SetSender(GetSender());
		pNewFileTransferAsyncInfo->SetFileName(GetFileName());
	}
	
	return pNewFileTransferAsyncInfo;
}


long CPurpleRequestFileTransferAsyncInfo::GetId() const
{
	return (long) m_pXfer->ui_data;
}


std::string CPurpleRequestFileTransferAsyncInfo::GetSender() const
{
	return m_strSender;
}


std::string CPurpleRequestFileTransferAsyncInfo::GetFileName() const
{
	if (m_pXfer->filename != NULL)
	{
		string strFileName("");
		
		GetLocaleTextFromUTF8(m_pXfer->filename, strFileName);
		
		return strFileName;
	}
	
	return "";
}


bool CPurpleRequestFileTransferAsyncInfo::IsWaitingDecision() const
{
	return ( (!m_bAccepted) && (!m_bRejected) && (!m_bCanceledLocal) && (!m_bCanceledRemote));
}


bool CPurpleRequestFileTransferAsyncInfo::WasAccepted() const
{
	return m_bAccepted;
}


bool CPurpleRequestFileTransferAsyncInfo::WasRejected() const
{
	return m_bRejected;
}


bool CPurpleRequestFileTransferAsyncInfo::WasLocalyCanceled() const
{
	return m_bCanceledLocal;
}


bool CPurpleRequestFileTransferAsyncInfo::WasRemotelyCanceled() const
{
	return m_bCanceledRemote;
}


void CPurpleRequestFileTransferAsyncInfo::SetTransferAccepted()
{
	m_bAccepted = true;
}


void CPurpleRequestFileTransferAsyncInfo::SetTransferRejected()
{
	m_bRejected = true;
}


void CPurpleRequestFileTransferAsyncInfo::SetTransferCanceledLocal()
{
	m_bCanceledLocal = true;
}


void CPurpleRequestFileTransferAsyncInfo::SetTransferCanceledRemote()
{
	m_bCanceledRemote = true;
}


void CPurpleRequestFileTransferAsyncInfo::SetPurpleXFer(PurpleXfer* pXfer)
{
	purple_xfer_ref(pXfer);
	
	m_pXfer = pXfer;
}


void CPurpleRequestFileTransferAsyncInfo::SetOkCallback(GCallback cbOK)
{
	m_cbOK = cbOK;
}


void CPurpleRequestFileTransferAsyncInfo::SetCancelCallback(GCallback cbCancel)
{
	m_cbCancel = cbCancel;
}


void CPurpleRequestFileTransferAsyncInfo::SetUserData(void* pUserData)
{
	m_pUserData = pUserData;
}


void CPurpleRequestFileTransferAsyncInfo::SetSender(std::string& strSender)
{
	m_strSender = strSender;
}


void CPurpleRequestFileTransferAsyncInfo::SetFileName(std::string& strFileName)
{
	m_strFileName = strFileName;
}


PurpleXfer* CPurpleRequestFileTransferAsyncInfo::GetPurpleXFer()
{
	return m_pXfer;
}


GCallback CPurpleRequestFileTransferAsyncInfo::GetOkCallback()
{
	return m_cbOK;
}


GCallback CPurpleRequestFileTransferAsyncInfo::GetCancelCallback()
{
	return m_cbCancel;
}


void* CPurpleRequestFileTransferAsyncInfo::GetUserData()
{
	return m_pUserData;
}


std::string CPurpleRequestFileTransferAsyncInfo::GetFileName()
{
	return m_strFileName;
}

