
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

#include "../resource.h"
#include "skin/AppCloseAlertElement.h"
#include "MainDialog.h"
#include "TransparentStatic.h"
#include "TransparentControls.h"


class CAlertDialog : public CMainDialog,
					 public CWinDataExchange<CAlertDialog>,
					 public CTransparentControls<CAlertDialog>
{
public:
	CAlertDialog();
	virtual ~CAlertDialog();

protected:
	BEGIN_DDX_MAP(CAlertDialog)		
		DDX_CHECK(IDC_CHECK_CONFIRM, m_bCheck)				
	END_DDX_MAP()
	
	BEGIN_MSG_MAP(CAlertDialog)		
		CHAIN_MSG_MAP(CTransparentControls<CAlertDialog>)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		CHAIN_MSG_MAP(CMainDialog)
	END_MSG_MAP()

	LRESULT OnInitDialog(HWND, LPARAM);
	void OnDestroy();
	void DoPaint(CDCHandle& dc);

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	CStaticIconElementPtr GetTrayIconSample();

public:	
	CString		m_strCaption;	
	WTL::CBitmap m_bmpSample;
	BOOL		m_bCheck;

	CAppCloseAlertElementPtr	m_pAppCloseAlertElement;
	OSVERSIONINFO				m_ovi;

	CTransparentStatic	m_label1TransparencyApplier;
	CTransparentStatic	m_label2TransparencyApplier;
};
MAKEAUTOPTR(CAlertDialog);