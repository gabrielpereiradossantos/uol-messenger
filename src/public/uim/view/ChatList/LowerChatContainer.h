/* UOL Messenger
* Copyright (c) 2005 Universo Online S/A
*
* Direitos Autorais Reservados
* All rights reserved
*
* Este programa � software livre; voc� pode redistribu�-lo e/ou modific�-lo
* sob os termos da Licen�a P�blica Geral GNU conforme publicada pela Free
* Software Foundation; tanto a vers�o 2 da Licen�a, como (a seu crit�rio)
* qualquer vers�o posterior.
* Este programa � distribu�do na expectativa de que seja �til, por�m,
* SEM NENHUMA GARANTIA; nem mesmo a garantia impl�cita de COMERCIABILIDADE
* OU ADEQUA��O A UMA FINALIDADE ESPEC�FICA. Consulte a Licen�a P�blica Geral
* do GNU para mais detalhes. 
* Voc� deve ter recebido uma c�pia da Licen�a P�blica Geral do GNU junto
* com este programa; se n�o, escreva para a Free Software Foundation, Inc.,
* no endere�o 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA. 
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
* S�o Paulo SP - CEP 01452-002 - BRASIL  */


#pragma once

#include "ReadyForChatListCtrl.h"
#include "../TitlePanel.h"
#include <interfaces/IUOLMessengerConversation.h>


class CLowerChatContainer;
typedef CWindowImpl<CLowerChatContainer> CLowerChatContainerBase;

class CLowerChatContainer : public CLowerChatContainerBase
{
public:
    DECLARE_WND_CLASS("LowerChatContainer");

	virtual ~CLowerChatContainer();

    CReadyForChatListCtrlPtr GetReadyForChatListCtrl();
    CTitlePanelPtr GetTitlePanel();
    void CLowerChatContainer::SetConversation(IUOLMessengerConversation2Ptr pConversation);

    void UpdateLayout();

protected:
	BEGIN_MSG_MAP(CLowerChatContainer)			
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
        MSG_WM_SIZE(OnSize)
        FORWARD_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
    void OnSize(UINT nType, CSize size);
		
private:
    void CreateReadyForChatListCtrl();	
	void CreateTitlePanel();
    void UpdateLayout(CSize size);

private:
	CReadyForChatListCtrlPtr        m_pReadyForChatListCtrl;
    CTitlePanelPtr                  m_pTitlePanel;
    IUOLMessengerConversation2Ptr   m_pConversation;
};

MAKEAUTOPTR(CLowerChatContainer);

