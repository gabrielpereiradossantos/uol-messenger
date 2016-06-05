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
 
#pragma once


#include "IUOLMessengerConversation.h"


class __declspec(novtable) IUOLMessengerMessageTemplateRecord
{
public:
	
	enum InvalidReturnConstants
	{
		INVALID_RECORD_TYPE = -1,
		INVALID_FILE_TRANSFER_ID = -1
	};
	
public:
	virtual ~IUOLMessengerMessageTemplateRecord() {}
	
	virtual int GetRecordType() const = 0;
	virtual IUOLMessengerConversationPtr GetConversation() const = 0;
	virtual CString GetAuthor() const = 0;
	virtual CString GetAuthorId() const = 0;
	virtual CString GetAuthorIconPath() const = 0;
	virtual CString GetAccountUserId() const = 0;
	virtual CString GetAccountNickName() const = 0;
	virtual CString GetAccountIconPath() const = 0;
	virtual CString GetContactName() const = 0;
	virtual CString GetContactAlias() const = 0;
	virtual CString GetContactIconPath() const = 0;
	virtual CString GetService() const = 0;
	virtual CString GetMessage() const = 0;
	virtual CString GetMessageParsed() = 0;
	virtual CTime GetTime() const = 0;
	virtual int GetFileTransferId() = 0;
	virtual CString GetFileTransferName() = 0;
};

MAKEAUTOPTR(IUOLMessengerMessageTemplateRecord);

