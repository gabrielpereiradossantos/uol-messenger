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
 
// MessageBox.h : interface of the CMessageBox class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../resource.h"
#include "MainDialog.h"
#include "Skin/StaticIconElement.h"
#include "Skin/FontElement.h"
#include <interfaces/IUOLMessengerImage.h>
#include "../controller/UIMApplication.h"
#include "TransparentStatic.h"

class CMessageBox : public CMainDialog
{
public:
	enum DialogMetrics
	{
		DM_IMAGE_WIDTH = 32,
		DM_IMAGE_HEIGHT = DM_IMAGE_WIDTH,
		DM_INTERNAL_SPACE = 14,
        DM_BUTTON_HSPACE = 21,
        DM_BUTTON_VSPACE = 11,
        DM_STATIC_HSPACE = 19,
        DM_STATIC_VSPACE = 11,
	};
    
    CMessageBox(void);
	~CMessageBox(void);
    void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
    void SetMessageBoxFlags(UINT msgboxFlags);
    void SetMessageBoxTitle(CString strTitle);
    void SetMessageBoxText(CString strText);
    void DoPaint(CDCHandle& dc);

protected:
	BEGIN_MSG_MAP(CMessageBox)
		MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
        COMMAND_ID_HANDLER(IDYES, OnYes)
		COMMAND_ID_HANDLER(IDNO, OnNo)
        CHAIN_MSG_MAP(CMainDialog)
        REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnInitDialog(HWND, LPARAM);
    LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnYes(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNo(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
    CStaticIconElementPtr	m_pIconError;
    CStaticIconElementPtr	m_pIconInformation;
    CStaticIconElementPtr	m_pIconQuestion;
    CStaticIconElementPtr	m_pIconWarning;
    UINT                    m_MessageBoxFlags;
    CFontElementPtr			m_pFont;
    CFont                   m_FontText;
    CRect					m_rectMsgIcon;
    CString                 m_Title;
    CString                 m_Text;
    CTransparentStatic      m_StaticText;
};

MAKEAUTOPTR(CMessageBox);