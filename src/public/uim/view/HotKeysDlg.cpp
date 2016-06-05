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
 
// HotKeysDlg.cpp : implementation of the CHotKeysDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HotKeysDlg.h"

#include "../controller/UIMApplication.h"
#include "../controller/UIManager.h"


#define SKIN_TOP_MARGIN		26
#define CTRL_LEFT_MARGIN	20
#define CTRL_TOP_MARGIN		44
#define CTRL_WIDTH			514
#define CTRL_HEIGHT			354


CHotKeysDlg::CHotKeysDlg(void) : 
		CMainDialog(IDD_DIALOG_HOTKEYS)
{
}


CHotKeysDlg::~CHotKeysDlg(void)
{
}


LRESULT CHotKeysDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	__super::OnInitDialog(NULL, NULL);

	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	
	CString strTitle;
	CString strTitleFormat;
	CString strTitleDefault;
	
	strTitle = pStringLoader->GetValue("uol.resource.IDS_HOTKEYS_TITLE");
	strTitleFormat = pStringLoader->GetValue("uol.resource.IDS_TITLE_WINDOW_DEFAULT");
	strTitleDefault.Format(strTitleFormat, strTitle);
	
	//Set title and text
	SetTitle(strTitleDefault);

	// Create HTML control and set it's content
	CRect rect(CTRL_LEFT_MARGIN, CTRL_TOP_MARGIN, CTRL_WIDTH, CTRL_HEIGHT);
	m_HTMLViewCtrl.Create(m_hWnd, rect);
	
	if(!m_templateFile.IsEmpty())
	{
		FILE *fp;
		int nReads;
		CHAR buffer[10000];
		
		CString strThemePath = m_templateFile;
		
		IUOLMessengerUIManager3Ptr pUIManager3 = ap_dynamic_cast<IUOLMessengerUIManager3Ptr>(CUIMApplication::GetApplication()->GetUIManager());
		
		if (pUIManager3)
		{
			IUOLMessengerThemePropertiesPtr pThemeProperties = pUIManager3->GetCurrentThemeProperties();
			
			if (pThemeProperties)
			{
				strThemePath = pThemeProperties->GetDirPath() + _T("\\") + m_templateFile;
			}
		}
		
		CString strHTML;
		
		fp = fopen(strThemePath,"rt");
		ATLASSERT(fp!=NULL);
		nReads = fread(buffer,sizeof(char),10000,fp);
		buffer[nReads]=0x00;
		strHTML = buffer;
		fclose(fp);

		m_HTMLViewCtrl.SetHtml(strHTML);		
	}
	
	CenterWindow();

	return TRUE;
}


LRESULT CHotKeysDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}


void CHotKeysDlg::OnPaint(HDC hDc)
{
	__super::OnPaint(hDc);

	HDC hDC = GetDC();
	
	HPEN hPen = ::CreatePen(PS_SOLID, 1, m_clrHTMLCtrlBorder);

	SelectObject(hDC, hPen); 

	Rectangle(hDC, CTRL_LEFT_MARGIN - 1, CTRL_TOP_MARGIN - 1, CTRL_WIDTH + 1, CTRL_HEIGHT + 1);

	DeleteObject(hPen);
}


LRESULT CHotKeysDlg::OnDestroy()
{
	m_HTMLViewCtrl.DestroyWindow();

	return 0L;
}


void CHotKeysDlg::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);
	
	try
	{
		GetParameterValue(m_templateFile, pMapParams, "templateFile");
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "templateFile");
	}

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "clrHTMLCtrlBorder");
		m_clrHTMLCtrlBorder.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "clrHTMLCtrlBorder");
		m_clrHTMLCtrlBorder.SetColor("#4ea2c6");
	}
}