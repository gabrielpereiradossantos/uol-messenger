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
 
#include "stdafx.h"
#include "EmoticonsWindow.h"
#include "../../controller/UIMApplication.h"
#include "../../resource.h"
#include <strsafe.h>
#include "../../resource.h"
#include "../../urlbuilder.h"
#include "../../controller/UIMApplication.h"
#include <commands/OpenBrowserCommand.h>
#include "../HTMLUtilitary.h"
#include "EmoticonsFrame.h"


/************************************************************************/
/* CEmoticonsHTMLCtrl                                                   */
/************************************************************************/

CEmoticonsHTMLCtrl::CEmoticonsHTMLCtrl(CEmoticonsWindow* pEmoticonWindow) :
	m_pWebBrowser(NULL),
	m_strHtml(""),
	pEmoticonWindow(pEmoticonWindow),
	m_strTag("")
{
}


CEmoticonsHTMLCtrl::~CEmoticonsHTMLCtrl()
{
}

void CEmoticonsHTMLCtrl::SetText(const CString& strText)
{
	m_strHtml = strText;
}

HWND CEmoticonsHTMLCtrl::Create(HWND hWndParent, _U_RECT rect, LPCTSTR szWindowName,
							 DWORD dwStyle, DWORD dwExStyle,
							 _U_MENUorID MenuOrID, LPVOID lpCreateParam)
{
	HWND hWnd = __super::Create(hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);

	if (hWnd != NULL)
	{
		//ModifyStyle(0, /*WS_HSCROLL |*/ WS_VSCROLL);
		
		CComBSTR bstrURL = CURLBuilder::ResourceToURL(IDR_HTML_CONTACT_INFO);

		if (FAILED(CreateControl(bstrURL)))
		{
			ATLASSERT(FALSE);
			DestroyWindow();

			return NULL;
		}	

		if (FAILED(QueryControl(&m_pWebBrowser)))
		{
			ATLASSERT(FALSE);
			DestroyWindow();

			return NULL;
		}

		ReceiveEvents(TRUE, m_pWebBrowser);
	}

	return hWnd;
}


void CEmoticonsHTMLCtrl::OnDestroy()
{
	if (m_pWebBrowser)
	{
		ReceiveEvents(FALSE, m_pWebBrowser);
		m_pDocument.Release();
		m_pWebBrowser.Release();

		m_pDocument = NULL;
		m_pWebBrowser = NULL;
	}
}


LRESULT CEmoticonsHTMLCtrl::OnEraseBackground(HDC hDc)
{
	return 1;
}

void CEmoticonsHTMLCtrl::OnSize(UINT, CSize)
{
	Invalidate();

	SetMsgHandled(FALSE);
}


LRESULT CEmoticonsHTMLCtrl::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DWORD code = HIWORD(lParam);
	
	if (code == WM_RBUTTONDOWN)
	{
		return MA_ACTIVATEANDEAT;
	}
	else
	{
		return MA_ACTIVATE;
	}
}


STDMETHODIMP_(void) CEmoticonsHTMLCtrl::BeforeNavigate2(IDispatch* /*pDisp*/, VARIANT* url, VARIANT* /*Flags*/, VARIANT* /*TargetFrameName*/, VARIANT* /*PostData*/, VARIANT* /*Headers*/,  VARIANT_BOOL* Cancel)
{
	CString strURL(url->bstrVal);
	CString strTag;
	CString strTagTemplate2("@|TAGTEMPLATE@|");

	int start = strURL.Find(strTagTemplate2) + strTagTemplate2.GetLength();

	int end = strURL.Find(strTagTemplate2, start);
	
	if (end>start) 
	{
		strTag = strURL.Mid(start,end - start);
	}

	m_strTag = CHTMLUtilitary::UnscapeText(strTag);
	
	m_pWebBrowser->Stop();
	GetParent().GetParent().PostMessage(WM_CLOSE);
}

STDMETHODIMP_(void) CEmoticonsHTMLCtrl::DocumentComplete(IDispatch* pDisp, VARIANT* /*URL*/)
{
	// Get IDocument2 pointer
	if (!m_pDocument)
	{
		CComPtr<IDispatch> pDispatch;
		HRESULT hr;

		hr = m_pWebBrowser->get_Document(&pDispatch);
		if (SUCCEEDED(hr))
		{
			m_pDocument = pDispatch;
		}

		SAFEARRAY* psa = SafeArrayCreateVector(VT_VARIANT, 0, 1);
		VARIANT *param;
		bstr_t bsData = (LPCTSTR)m_strHtml;
		hr =  SafeArrayAccessData(psa, (LPVOID*)&param);
		if (hr != S_OK)
		{
			return;
		}
		param->vt = VT_BSTR;
		param->bstrVal = (BSTR)bsData;
		hr = m_pDocument->write(psa);	//write your buffer
		if (hr != S_OK)
		{
			return;
		}
		hr = m_pDocument->close();	//and closes the document, "applying" your code  
		if (hr != S_OK)
		{
			return;
		}

		SafeArrayDestroy(psa);
	}
}

CString CEmoticonsHTMLCtrl::GetTag() const
{
	return m_strTag;
}



//////////////////////////////////////////////////////////////////////////
// CEmoticonsWindow
//////////////////////////////////////////////////////////////////////////

void CEmoticonsWindow::LoadEmoticons() 
{
	CString strHtml("");
	if(!m_strEmoticonFile.IsEmpty())
	{

		FILE *fp;
		int nReads;
		CHAR buffer[10000];

		CString themepath = m_strEmoticonFile;
		
		IUOLMessengerUIManager3Ptr pUIManager3 = ap_dynamic_cast<IUOLMessengerUIManager3Ptr>(CUIMApplication::GetApplication()->GetUIManager());

		if (pUIManager3)
		{
			IUOLMessengerThemePropertiesPtr pThemeProperties = pUIManager3->GetCurrentThemeProperties();

			if (pThemeProperties)
			{
				themepath = pThemeProperties->GetDirPath() + _T("\\") + m_strEmoticonFile;
			}
		}

		CString strTemplate;

		fp = fopen(themepath,"rt");
		ATLASSERT(fp!=NULL);
		nReads = fread(buffer,sizeof(char),10000,fp);
		buffer[nReads]=0x00;
		strTemplate = buffer;
		fclose(fp);

		CString strHTMLTop;
		CString strHTMLBottom;
		CString strBeginTemplate("<!-- BEGIN TEMPLATE -->");
		CString strEndTemplate("<!-- END TEMPLATE -->");

		CString strBeginLineTemplate("<!-- BEGIN LINE TEMPLATE -->");
		CString strEndLineTemplate("<!-- END LINE TEMPLATE -->");

		CString strTagTemplate("@@TAGTEMPLATE@@");
		CString strTagTemplate2("@|TAGTEMPLATE@|");
		CString strFileTemplate("@@FILETEMPLATE@@");
		CString strAltTemplate("@@IMGALTTAGTEMPLATE@@");
		
		CString strOut;

		int iBeginTemplate;
		int iEndTemplate;
		CString strTemplateRow;
		iBeginTemplate = strTemplate.Find(strBeginTemplate) + strBeginTemplate.GetLength();
		iEndTemplate = strTemplate.Find(strEndTemplate);
		strHTMLTop = strTemplate.Left(iBeginTemplate);
		strTemplateRow = strTemplate.Mid(iBeginTemplate, iEndTemplate - iBeginTemplate);
		strHTMLBottom = strTemplate.Right(strTemplate.GetLength() - (strHTMLTop.GetLength() + strTemplateRow.GetLength()));

		CString auxRowTemplate = strTemplateRow;

		LPWSTR lpszW = new WCHAR[255];

		CAtlList<IUOLMessengerEmoticonPtr> emoticonsList;

		int iLastTagTemplate = -1;

		IUOLMessengerSettingsManager2Ptr pSettingsManager2 = ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(CUIMApplication::GetApplication()->GetSettingsManager());
		IUOLMessengerEmoticonSettings3Ptr pEmoticonSettings3;

		ATLASSERT(pSettingsManager2);
		if (pSettingsManager2)
		{
			pEmoticonSettings3 = ap_dynamic_cast<IUOLMessengerEmoticonSettings3Ptr>(
					pSettingsManager2->GetEmoticonSettings());
		}

		int nEmoticonSetIds = IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_INVALID;
		
		ATLASSERT(pEmoticonSettings3);
		ATLASSERT(m_pConversation);
		if ( (pEmoticonSettings3) && (m_pConversation) && (m_pConversation->GetAccount()) && 
				(m_pConversation->GetAccount()->GetProtocol()) )
		{
			pEmoticonSettings3->GetEmoticonList(
					m_pConversation->GetAccount()->GetProtocol()->GetId().c_str(), emoticonsList);
		}

		if (emoticonsList.GetCount() > 0)
		{
			::SendMessage(m_pMainFrame->m_hWnd, 
					CEmoticonsFrame::WM_RECALCULATE_FRAME_SIZE, emoticonsList.GetCount(), 0);
		}
		
		for (POSITION pos = emoticonsList.GetHeadPosition(); pos; emoticonsList.GetNext(pos))
		{
			IUOLMessengerEmoticonPtr pEmoticon = emoticonsList.GetAt(pos);

			CString strFilePath = pEmoticon->GetFilePath();

			if (ATLPath::IsRelative(strFilePath))
			{
				CString themepath = pSettingsManager2->GetEmoticonSettings()->GetDefaultDirPath();

				strFilePath = themepath + "\\" + strFilePath;
			}

			/*
			if (strOut.Find(strFilePath) >= 0 || auxRowTemplate.Find(strFilePath) >= 0)
			{
				continue;
			}
			*/

			iLastTagTemplate = auxRowTemplate.Find(strTagTemplate);
			if (iLastTagTemplate < 0)
			{
				strOut += auxRowTemplate;
				auxRowTemplate = strTemplateRow;
				iLastTagTemplate = auxRowTemplate.Find(strTagTemplate);
			}
			CHTMLUtilitary::Replace(auxRowTemplate, strFileTemplate, strFilePath);
			if (pEmoticon->GetShortcut().Find("\"") < 0)
			{
				CHTMLUtilitary::Replace(auxRowTemplate, strTagTemplate, 
						strTagTemplate2 + CHTMLUtilitary::ScapeText(pEmoticon->GetShortcut()) + 
						strTagTemplate2);
				CHTMLUtilitary::Replace(auxRowTemplate, strAltTemplate, pEmoticon->GetShortcut());
			}
			else
			{
				CHTMLUtilitary::ReplaceIncludingQuotes(auxRowTemplate, strTagTemplate, 
						strTagTemplate2 + CHTMLUtilitary::ScapeText(pEmoticon->GetShortcut()) + 
						strTagTemplate2, '\'');
				CHTMLUtilitary::ReplaceIncludingQuotes(auxRowTemplate, strAltTemplate, 
						pEmoticon->GetShortcut(), '\'');
			}
		}

		CString sAux = auxRowTemplate;
		if (iLastTagTemplate >= 0)
		{
			if (auxRowTemplate.Find(strBeginLineTemplate, iLastTagTemplate) >= 0)
			{
				sAux = auxRowTemplate.Left(auxRowTemplate.Find(strBeginLineTemplate, iLastTagTemplate));
				int tmppos = auxRowTemplate.Find(strEndLineTemplate, iLastTagTemplate) + strEndLineTemplate.GetLength();
				while (auxRowTemplate.Find(strEndLineTemplate, tmppos) >= 0)
				{
					tmppos = auxRowTemplate.Find(strEndLineTemplate, tmppos) + strEndLineTemplate.GetLength();
				}
				sAux += auxRowTemplate.Right(auxRowTemplate.GetLength() - tmppos);
			}
		}
		strOut += sAux;
		strHtml = strHTMLTop + strOut + strHTMLBottom;
	}

	m_detailsCtrl.SetText(strHtml);
}

CEmoticonsWindow::CEmoticonsWindow(CMainFrame* pMainFrame, CString strEmoticonFile) : 
m_pMainFrame(pMainFrame),
m_detailsCtrl(this),
m_strEmoticonFile(strEmoticonFile)
{
}

void CEmoticonsWindow::InsertEmoticon(const CString &filepath, const CString &emoticontag)
{
	CEmoticonImageTag::CPair* pPair;

	BOOL found = TRUE;

	pPair = m_imageTag.Lookup(filepath);

	if (pPair == NULL) 
	{ 
		m_imageTag.SetAt(filepath, emoticontag);

		found = FALSE;
	}
};


void CEmoticonsWindow::OnSize(UINT nType, const CSize& size)
{
	m_detailsCtrl.SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE | SWP_NOZORDER);
	SetMsgHandled(FALSE);
}

LRESULT CEmoticonsWindow::OnCreate(LPCREATESTRUCT lpCreatestruct)
{
	LoadEmoticons();

	CRect rectClient;
	GetClientRect(&rectClient);

	m_detailsCtrl.Create(m_hWnd, rectClient, NULL, 
		WS_CHILDWINDOW | WS_VISIBLE  | ES_MULTILINE | ES_READONLY);
//		WS_CHILDWINDOW | WS_VISIBLE  | WS_VSCROLL | ES_MULTILINE | ES_READONLY);
	m_detailsCtrl.ShowWindow(SW_NORMAL);

	CUIMApplication::GetApplication()->AddMessageFilter(this);	

	return 0L;
}

void CEmoticonsWindow::OnDestroy()
{
	m_pEmoticonWindow->AddEmoticon(m_detailsCtrl.GetTag());

	CUIMApplication::GetApplication()->RemoveMessageFilter(this);	

	SetMsgHandled(FALSE);
}

BOOL CEmoticonsWindow::PreTranslateMessage(MSG* pMsg)
{
	// This was stolen from an SDI app using a form view.
	//
	// Pass keyboard messages along to the child window that has the focus.
	// When the browser has the focus, the message is passed to its containing
	// CAxWindow, which lets the control handle the message.

	if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
		return FALSE;

	HWND hWndCtl = ::GetFocus();

	if(IsChild(hWndCtl))
	{
		// find a direct child of the dialog from the window that has focus
		while(::GetParent(hWndCtl) != m_hWnd)
			hWndCtl = ::GetParent(hWndCtl);

		// give control a chance to translate this message
		if(::SendMessage(hWndCtl, WM_FORWARDMSG, 0, (LPARAM)pMsg) != 0)
			return TRUE;
	}

	// A normal control has the focus, so call IsDialogMessage() so that
	// the dialog shortcut keys work (TAB, etc.)
	//return IsDialogMessage(pMsg);
	return FALSE;
}


void CEmoticonsWindow::RegisterMessageEditPanel(CEmoticonAdd * emoticon) {
	ATLASSERT(emoticon!=NULL);
	m_pEmoticonWindow = emoticon;
}

void CEmoticonsWindow::SetConversation(IUOLMessengerConversationPtr pConversation)
{
	m_pConversation = pConversation;
}

