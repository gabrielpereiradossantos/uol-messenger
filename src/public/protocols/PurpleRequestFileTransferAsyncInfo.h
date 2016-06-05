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
 

#ifndef _PURPLE_REQUEST_FILE_TRANSFER_ASYNC_INFO_H_
#define _PURPLE_REQUEST_FILE_TRANSFER_ASYNC_INFO_H_

#include "RequestFileTransferAsyncInfo.h"
#include <ft.h>


class CPurpleRequestFileTransferAsyncInfo : public CRequestFileTransferAsyncInfo
{
public:
	CPurpleRequestFileTransferAsyncInfo();
	virtual ~CPurpleRequestFileTransferAsyncInfo();
	
	virtual CPurpleRequestFileTransferAsyncInfo* Clone();
	
	// CRequestFileTransferAsyncInfo interface.
	virtual long GetId() const;
	virtual std::string GetSender() const;
	virtual std::string GetFileName() const;
	virtual bool IsWaitingDecision() const;
	virtual bool WasAccepted() const;
	virtual bool WasRejected() const;
	virtual bool WasLocalyCanceled() const;
	virtual bool WasRemotelyCanceled() const;
	virtual void SetTransferAccepted();
	virtual void SetTransferRejected();
	virtual void SetTransferCanceledLocal();
	virtual void SetTransferCanceledRemote();
	
	void SetPurpleXFer(PurpleXfer* pXfer);
	void SetOkCallback(GCallback cbOK);
	void SetCancelCallback(GCallback cbCancel);
	void SetUserData(void* pUserData);
	void SetSender(std::string& strSender);
	void SetFileName(std::string& strFileName);
	
	PurpleXfer* GetPurpleXFer();
	GCallback GetOkCallback();
	GCallback GetCancelCallback();
	void* GetUserData();
	std::string GetFileName();
	
protected:
	
	PurpleXfer*		m_pXfer;
	GCallback		m_cbOK;
	GCallback		m_cbCancel;
	void*			m_pUserData;
	std::string		m_strSender;
	std::string		m_strFileName;
	
	bool			m_bAccepted;
	bool			m_bRejected;
	bool			m_bCanceledLocal;
	bool			m_bCanceledRemote;
};

#endif // _PURPLE_REQUEST_FILE_TRANSFER_ASYNC_INFO_H_
