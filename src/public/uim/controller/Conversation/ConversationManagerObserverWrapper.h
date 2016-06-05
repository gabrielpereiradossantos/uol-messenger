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

#include "../../ObserverWrapper.h"
#include <interfaces/IUOLMessengerConversationManager.h>


class CConversationManagerObserverWrapper : public CObserverWrapper<CUOLMessengerConversationManagerObserver>
{
public:
	CConversationManagerObserverWrapper(CUOLMessengerConversationManagerObserver* pObserver);
	virtual ~CConversationManagerObserverWrapper();

	virtual void OnIMReceived(IUOLMessengerConversationPtr pConversation, const CString& strText, DWORD dwMessageReceivingStatus);
	virtual void OnIMSent(IUOLMessengerConversationPtr pConversation, const CString& strText);
	virtual void OnIMSendError(IUOLMessengerConversationPtr pConversation, IMProtocolError sendError);

	virtual void OnContactTyping(IUOLMessengerConversationPtr pConversation, TypingState state);
	virtual void OnConversationWindowClosed(IUOLMessengerConversationPtr pConversation);
	virtual void OnConversationTimeout(IUOLMessengerConversationPtr pConversation);
	
	virtual void OnNudgeReceived(IUOLMessengerConversationPtr pConversation, const CString& strText, DWORD dwMessageNudgeStatus);
};

MAKEAUTOPTR(CConversationManagerObserverWrapper);