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

#include <interfaces/IUOLMessengerFacade.h>
#include "FacadeWrapper.h"
#include "UOLFoneCapabilitiesHandler.h"


class CHistoryWindowMonitor : 
		public CUOLMessengerImageButtonEventListener2, 
		public CUOLMessengerUIManagerObserver
{
public:
	typedef CAtlArray<IUOLMessengerImageButtonPtr> CImagesArray;
	MAKEAUTOPTR(CImagesArray);

	virtual ~CHistoryWindowMonitor();
	
	static CHistoryWindowMonitor* GetInstance();
	
	BOOL Init(IUOLMessengerFacade* pUOLMessengerFacade);
	void Finalize();

protected:
	
	CHistoryWindowMonitor();

	// CUOLMessengerUIManagerObserver methods
	virtual void OnCreateHistoryWindow(IUOLMessengerHistoryWindow2* pHistoryWindow);
	virtual void OnDestroyHistoryWindow(IUOLMessengerHistoryWindow2* pHistoryWindow);

	// CUOLMessengerImageButtonEventListener2 methods
	virtual void OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint& ptClick);

private:
	
	void AddAllButtons(IUOLMessengerHistoryWindow2* pHistoryWindow);
	void RemoveAllButtons(IUOLMessengerHistoryWindow2* pHistoryWindow);
	IUOLMessengerImageButtonPtr AddButton(IUOLMessengerHistoryWindow2* pHistoryWindow, int nImage, int nTooltip);
	//void RemoveButton(IUOLMessengerHistoryWindow2* pHistoryWindow, int nImage, int nTooltip);
	
	IUOLMessengerHistoryWindow2* FindHistoryWindow(IUOLMessengerImageButton* pButton);
	CImagesArrayPtr FindHistoryWindowImageButton(IUOLMessengerHistoryWindow2* pHistoryWindow);

	BOOL IsVoipHistory(IUOLMessengerHistoryWindow2* pHistoryWindow);
	
private:

	IUOLMessengerFacade*	m_pUOLMessengerFacade;
	
	typedef CAtlMap<IUOLMessengerHistoryWindow2*, CImagesArrayPtr> CMapHistoryWindow;
    CMapHistoryWindow			m_mapHistoryWindow;

	CComAutoCriticalSection		m_cs;
	
	CUOLFoneCapabilitiesHandler	m_capabilitiesHandler;
};