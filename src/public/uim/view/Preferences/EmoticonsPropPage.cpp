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
 
// EmoticonsPropPage.cpp : implementation file

#include "stdafx.h"
#include "EmoticonsPropPage.h"
#include "../../controller/UIMApplication.h"
#include "../UIMFileDialog.h"
#include "../../urlbuilder.h"
#include "../HTMLUtilitary.h"
#include "../../HTMLFilter.h"
#include <commands/ShowAddCustomEmoticonDialogCommand.h>


#define			TEMPLATE_BEGIN_STR				"<!-- BEGIN TEMPLATE -->"
#define			TEMPLATE_END_STR				"<!-- END TEMPLATE -->"

#define			TEMPLATE_BEGIN_CUSTOM_STR		"<!-- BEGIN TEMPLATE CUSTOM -->"
#define			TEMPLATE_END_CUSTOM_STR			"<!-- END TEMPLATE CUSTOM -->"

#define			TEMPLATE_BEGIN_LINE_STR			"<!-- BEGIN LINE TEMPLATE -->"
#define			TEMPLATE_END_LINE_STR			"<!-- END LINE TEMPLATE -->"

#define			TEMPLATE_ID_TAG_STR				"@@TAGTEMPLATE@@"
#define			TEMPLATE_ID_NEW_TAG_STR			"@|TAGTEMPLATE@|"
#define			TEMPLATE_FILE_TAG_STR			"@@FILETEMPLATE@@"
#define			TEMPLATE_IMG_ALT_TAG_STR		"@@IMGALTTAGTEMPLATE@@"
#define			TEMPLATE_TEXT_STR				"@@TEXTTEMPLATE@@"


#define			EMOTICON_CTRL_LEFT_POS			20
#define			EMOTICON_CTRL_TOP_POS			60
#define			EMOTICON_CTRL_RIGHT_POS			500
#define			EMOTICON_CTRL_BOTTOM_POS		390
#define			EMOTICON_CTRL_BORDER_WIDTH		1


// CEmoticonsViewCtrl


CEmoticonsViewCtrl::CEmoticonsViewCtrl() :
		m_pWebBrowser(NULL),
		m_strHtml(""),
		m_hCustomEmoticonMenu(NULL)
{
	IUOLMessengerSettingsManager2Ptr pSettingsManager2 = ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(CUIMApplication::GetApplication()->GetSettingsManager());
	
	ATLASSERT(pSettingsManager2);
	if (pSettingsManager2)
	{
		m_pEmoticonSettings = ap_dynamic_cast<IUOLMessengerEmoticonSettings3Ptr>
				(pSettingsManager2->GetEmoticonSettings());
	}
}


CEmoticonsViewCtrl::~CEmoticonsViewCtrl()
{
}


void CEmoticonsViewCtrl::SetText(const CString& strText)
{
	m_strHtml = strText;
	
	if ( (!m_strHtml.IsEmpty()) && (m_pWebBrowser != NULL) )
	{
		CComVariant vURL = CURLBuilder::ResourceToURL(IDR_HTML_CONTACT_INFO);
		CComVariant vFlags = navNoHistory;
		CComVariant vTarget = _T("_self");
		CComVariant vDummy;
		
		HRESULT hr = m_pWebBrowser->Navigate2(&vURL, &vFlags, &vTarget, &vDummy, &vDummy);
		ATLASSERT(SUCCEEDED(hr));
	}
}


void CEmoticonsViewCtrl::SetEmoticonSetIds(int nEmoticonSetIds)
{
	m_nEmoticonSetIds = nEmoticonSetIds;
}


void CEmoticonsViewCtrl::SetEmoticonChangingInfoFinder(IEmoticonChangingInfoFinder* pEmoticonChangingInfoFinder)
{
	m_pEmoticonChangingInfoFinder = pEmoticonChangingInfoFinder;
}


IUOLMessengerEmoticon2Ptr CEmoticonsViewCtrl::GetSelectedEmoticon()
{
	return m_pSelectedEmoticon;
}


CString CEmoticonsViewCtrl::GetSelectedEmoticonShortcut()
{
	return m_strSelectedEmoticonShortcut;
}


CString CEmoticonsViewCtrl::GetSelectedEmoticonFileName()
{
	return m_strSelectedEmoticonPath;
}


HWND CEmoticonsViewCtrl::Create(HWND hWndParent, _U_RECT rect, LPCTSTR szWindowName,
			DWORD dwStyle, DWORD dwExStyle,
			_U_MENUorID MenuOrID, LPVOID lpCreateParam)
{
	HWND hWnd = __super::Create(hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
	
	if (hWnd != NULL)
	{
		ModifyStyle(0, /*WS_HSCROLL |*/ WS_VSCROLL);
		//ModifyStyleEx(0, WS_EX_STATICEDGE);
		
		if (FAILED(CreateControl(L"about:blank")))
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
	
	m_hCustomEmoticonMenu = ::CreatePopupMenu();
	
	if (m_hCustomEmoticonMenu)
	{
		UOLLib::ISettings* pStringLoader = static_cast<CUIMApplication*>(
				CUIMApplication::GetApplication())->GetStringLoader();
		ATLASSERT(pStringLoader);
		
		CString strEmoticonDelMenu("Excluir");
		CString strEmoticonEditShortcutMenu("Editar Atalho");
		CString strEmoticonEditFilePathMenu("Editar Imagem");
		
		if (pStringLoader)
		{
			strEmoticonDelMenu = pStringLoader->GetValue(
					_T("uol.resource.IDS_PROPERTY_PAGE_EMOTICON_DEL_EMOTICON_MENU"));
			strEmoticonEditShortcutMenu = pStringLoader->GetValue(
					_T("uol.resource.IDS_PROPERTY_PAGE_EMOTICON_EDIT_SHORTCUT_MENU"));
			strEmoticonEditFilePathMenu = pStringLoader->GetValue(
					_T("uol.resource.IDS_PROPERTY_PAGE_EMOTICON_EDIT_FILEPATH_MENU"));
		}
		
		::AppendMenu(m_hCustomEmoticonMenu, MF_STRING, COMMAND_ID_EMOTICON_DEL, strEmoticonDelMenu);
		::AppendMenu(m_hCustomEmoticonMenu, MF_STRING, 
				COMMAND_ID_EMOTICON_EDIT_SHORTCUT, strEmoticonEditShortcutMenu);
		::AppendMenu(m_hCustomEmoticonMenu, MF_STRING, 
				COMMAND_ID_EMOTICON_EDIT_FILEPATH, strEmoticonEditFilePathMenu);
	}
	
	return hWnd;
}


void CEmoticonsViewCtrl::OnDestroy()
{
	if (m_hCustomEmoticonMenu)
	{
		::DestroyMenu(m_hCustomEmoticonMenu);
	}
	
	if (m_pWebBrowser)
	{
		ReceiveEvents(FALSE, m_pWebBrowser);
		m_pDocument.Release();
		m_pWebBrowser.Release();
		
		m_pDocument = NULL;
		m_pWebBrowser = NULL;
	}
}


void CEmoticonsViewCtrl::OnSize(UINT, CSize)
{
	Invalidate();

	SetMsgHandled(FALSE);
}


void CEmoticonsViewCtrl::OnEmoticonAction(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	if (nID == COMMAND_ID_EMOTICON_DEL)
	{
		::SendMessage(GetParent(), WM_EMOTICON_DEL, 0, 0);
	}
	else if (nID == COMMAND_ID_EMOTICON_EDIT_SHORTCUT)
	{
		::SendMessage(GetParent(), WM_EMOTICON_EDIT_SHORTCUT, 0, 0);
	}
	else if (nID == COMMAND_ID_EMOTICON_EDIT_FILEPATH)
	{
		::SendMessage(GetParent(), WM_EMOTICON_EDIT_FILEPATH, 0, 0);
	}
}


LRESULT CEmoticonsViewCtrl::OnParentNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//LPNMHDR pNotificationInfo = (LPNMHDR)lParam;
	DWORD code = LOWORD(wParam);
	
	if (code == WM_RBUTTONDOWN)
	{
		// Clear here to be updated on "IsClickOverEmoticonImageElement".
		//
		m_strSelectedEmoticonShortcut.Empty();
		m_strSelectedEmoticonPath.Empty();
		
		if (IsClickOverEmoticonImageElement())
		{
			ShowCustomPopupMenu();
		}
	}
	
	return 0L;
}


LRESULT CEmoticonsViewCtrl::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam)
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


/*
void CEmoticonsViewCtrl::HandleMouseRButtonDown()
{
	// Clear here to be updated on "IsClickOverEmoticonImageElement".
	//
	m_strSelectedEmoticonShortcut.Empty();
	m_strSelectedEmoticonPath.Empty();
	
	if (IsClickOverEmoticonImageElement())
	{
		ShowCustomPopupMenu();
	}
}
*/


STDMETHODIMP_(void) CEmoticonsViewCtrl::DocumentComplete(IDispatch* pDisp, VARIANT* /*URL*/)
{
	if (!m_strHtml.IsEmpty())
	{
		CComPtr<IDispatch> pDispatch;
		HRESULT hr;
		
		if (!m_pDocument)
		{
			// Get IDocument2 pointer
			hr = m_pWebBrowser->get_Document(&pDispatch);
			
			if (SUCCEEDED(hr))
			{
				m_pDocument = pDispatch;
			}
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


BOOL CEmoticonsViewCtrl::IsClickOverEmoticonImageElement()
{
	BOOL bIsOver = FALSE;
	
    POINT  CurPoint;
    GetCursorPos(&CurPoint);
	
	::ScreenToClient(m_hWnd, &CurPoint);
	
	if (m_pDocument != NULL)
	{
		CComQIPtr<IHTMLElement> pHitElement;
		
		// Get the element given by the coordinates.
		HRESULT hr = m_pDocument->elementFromPoint(CurPoint.x, CurPoint.y, &pHitElement);
		
		if (SUCCEEDED(hr))
		{
			CComQIPtr<IHTMLImgElement> pImageElement = pHitElement;
			
			if (pImageElement)
			{
				CComBSTR bstrEmoticonShortcut;
				CComBSTR bstrEmoticonPath;
				
				hr = pImageElement->get_alt(&bstrEmoticonShortcut);
				
				if (SUCCEEDED(hr))
				{
					hr = pImageElement->get_src(&bstrEmoticonPath);
				}
				
				if (SUCCEEDED(hr))
				{
					m_strSelectedEmoticonShortcut = bstrEmoticonShortcut;
					m_strSelectedEmoticonPath = bstrEmoticonPath;
					
					if (m_strSelectedEmoticonPath.Find("file://") == 0)
					{
						DWORD bufsize = MAX_PATH;
						CString strEmoticonPath;
						LPTSTR buf = strEmoticonPath.GetBuffer(bufsize);
						::PathCreateFromUrl(m_strSelectedEmoticonPath, buf, &bufsize, NULL);
						strEmoticonPath.ReleaseBuffer(bufsize);
						m_strSelectedEmoticonPath = strEmoticonPath;
					}

					if ( (!m_strSelectedEmoticonShortcut.IsEmpty()) && 
							(ATLPath::FileExists(m_strSelectedEmoticonPath)) )
					{
						bIsOver = TRUE;
					}
				}
			}
		}
	}
	
	return bIsOver;
}


void CEmoticonsViewCtrl::ShowCustomPopupMenu()
{
    POINT  CurPoint;
    GetCursorPos(&CurPoint);
	EmoticonChangingInfoPtr pEmoticonChangingInfo;
	
	if ( (::IsMenu(m_hCustomEmoticonMenu)) && (m_pEmoticonSettings) )
    {
		// Enable/Disable menu itens acordingly with the selected Emoticon 
		// supported options.
		m_pSelectedEmoticon = ap_dynamic_cast<IUOLMessengerEmoticon2Ptr>(
				m_pEmoticonSettings->GetEmoticon(m_nEmoticonSetIds, m_strSelectedEmoticonShortcut));
		
		// Maibe this shortcut is a element on a pending operation.
		// Let's check it out.
		if ( (m_pSelectedEmoticon == NULL) && (m_pEmoticonChangingInfoFinder) )
		{
			pEmoticonChangingInfo = 
					m_pEmoticonChangingInfoFinder->FindChangingItem(m_strSelectedEmoticonShortcut, 
							IEmoticonChangingInfoFinder::OPERATION_ID_ALL, FALSE);
		}
		
		// Ajuste solicitado pelo cliente para nao permitir a edicao de imagens 
		// para os emoticons padrao.
		//
		/*
		if ( (!m_pSelectedEmoticon) || (m_pSelectedEmoticon->IsCustomEmoticon()) )
		{
			::EnableMenuItem(m_hCustomEmoticonMenu, COMMAND_ID_EMOTICON_DEL, MF_ENABLED);
			::EnableMenuItem(m_hCustomEmoticonMenu, COMMAND_ID_EMOTICON_EDIT_SHORTCUT, MF_ENABLED);
		}
		else
		{
			::EnableMenuItem(m_hCustomEmoticonMenu, COMMAND_ID_EMOTICON_DEL, MF_GRAYED);
			::EnableMenuItem(m_hCustomEmoticonMenu, COMMAND_ID_EMOTICON_EDIT_SHORTCUT, MF_GRAYED);
		}
		
		::TrackPopupMenu(m_hCustomEmoticonMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
				CurPoint.x, CurPoint.y, 0, m_hWnd, 0);
		*/
		if ( ( (m_pSelectedEmoticon) && (m_pSelectedEmoticon->IsCustomEmoticon()) ) || 
				(pEmoticonChangingInfo) )
		{
			::TrackPopupMenu(m_hCustomEmoticonMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
					CurPoint.x, CurPoint.y, 0, m_hWnd, 0);
		}
    }
}





// CEmoticonsPropPage dialog


CEmoticonsPropPage::CEmoticonsPropPage() :
	CPreferencesPropPage(IDD_DIALOG_EMOTICONS),
	m_hEmoticonCtrlBorderBrush(NULL)
{
	IUOLMessengerSettingsManager3Ptr pSettingsManager3 = 
			ap_dynamic_cast<IUOLMessengerSettingsManager3Ptr>(
					CUIMApplication::GetApplication()->GetSettingsManager());
	
	ATLASSERT(pSettingsManager3);
	if (pSettingsManager3)
	{
		m_pEmoticonSettings = ap_dynamic_cast<IUOLMessengerEmoticonSettings3Ptr>
				(pSettingsManager3->GetEmoticonSettings());
		
		pSettingsManager3->RegisterObserver(this);
	}
}


CEmoticonsPropPage::~CEmoticonsPropPage()
{
	IUOLMessengerSettingsManager3Ptr pSettingsManager3 = 
			ap_dynamic_cast<IUOLMessengerSettingsManager3Ptr>(
					CUIMApplication::GetApplication()->GetSettingsManager());
	
	ATLASSERT(pSettingsManager3);
	if (pSettingsManager3)
	{
		pSettingsManager3->UnregisterObserver(this);
	}
}


LRESULT CEmoticonsPropPage::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetWindowText(GetTitle());

	DoDataExchange(FALSE);

	m_cmbEmoticonSetNames.SubclassWindow(GetDlgItem(IDC_COMBO_EMOTICON_SET_NAMES));
	m_wndButtonNew = GetDlgItem(IDC_EMOTICON_NEW);
	
	m_strEmoticonSetCustomName = m_pEmoticonSettings->GetEmoticonSetNameById(
			IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_CUSTOM);
	
	InitProtocolList();
	
	m_rectEmoticonCtrl.SetRect(EMOTICON_CTRL_LEFT_POS, EMOTICON_CTRL_TOP_POS, 
			EMOTICON_CTRL_RIGHT_POS, EMOTICON_CTRL_BOTTOM_POS);
	m_rectEmoticonCtrl.DeflateRect(EMOTICON_CTRL_BORDER_WIDTH, 
			EMOTICON_CTRL_BORDER_WIDTH);
	m_emoticonCtrl.Create(m_hWnd, m_rectEmoticonCtrl);
	m_emoticonCtrl.SetEmoticonChangingInfoFinder(this);
	
	// Register the Finder HWND so the AddCustomEmoticonDialog can 
	// make searches.
	//
	IUOLMessengerUIManager5Ptr pUIManager5 = 
			ap_dynamic_cast<IUOLMessengerUIManager5Ptr>(
					CUIMApplication::GetApplication()->GetUIManager());
	
	if (pUIManager5)
	{
		pUIManager5->SetEmoticonPropPageWndHandle(m_hWnd);
	}
	
	return 0L;
}


void CEmoticonsPropPage::OnDestroy()
{
	if (m_hEmoticonCtrlBorderBrush)
	{
		::DeleteObject(m_hEmoticonCtrlBorderBrush);
	}
	
	// Unregister the Finder HWND.
	//
	IUOLMessengerUIManager5Ptr pUIManager5 = 
			ap_dynamic_cast<IUOLMessengerUIManager5Ptr>(
					CUIMApplication::GetApplication()->GetUIManager());
	
	if (pUIManager5)
	{
		pUIManager5->SetEmoticonPropPageWndHandle(NULL);
	}
	
	SetMsgHandled(FALSE);
}


LRESULT CEmoticonsPropPage::OnShowWindow(BOOL bShow, int nStatus)
{
	if (bShow)
	{
		CalculatePaintingRgns();

		// Initialize emoticon ctrls (buttons and emoticon list)...
		UpdateEmoticonList();
	}
	
	return 0L;
}


LRESULT CEmoticonsPropPage::OnEraseBackground(HDC hDc)
{
	::FillRgn(hDc, m_mainWindowRgn, m_brushDialog);
	::FillRgn(hDc, m_emoticonCtrlBorderRgn, m_hEmoticonCtrlBorderBrush);
	
	return 0L;
}


LRESULT CEmoticonsPropPage::OnClickedNew(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	IUOLMessengerUIManager5Ptr pUIManager5 = ap_dynamic_cast<IUOLMessengerUIManager5Ptr>(
			CUIMApplication::GetApplication()->GetUIManager());
	
	if (pUIManager5)
	{
		IUOLMessengerAddCustomEmoticonWindowPtr pAddCustomEmoticonWindow = 
				pUIManager5->GetAddCustomEmoticonDialog("", "", 
						CAddCustomEmoticonDialog::OPERATION_MODE_ADD_FROM_PREFERENCES, this);
		
		if ( (pAddCustomEmoticonWindow) && (pAddCustomEmoticonWindow->ShowModal() == IDOK) )
		{
			UpdateChangingList("", pAddCustomEmoticonWindow->GetCustomEmoticonShortcut(), 
					pAddCustomEmoticonWindow->GetCustomEmoticonFileName(), 
					IEmoticonChangingInfoFinder::OPERATION_ID_ADD);
		}
	}
	
	return 0L;
}


LRESULT CEmoticonsPropPage::OnCbnSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	UpdateEmoticonList();
	
	return 0L;
}


LRESULT CEmoticonsPropPage::OnEmoticonDel(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IUOLMessengerEmoticon2Ptr pEmoticon2 = m_emoticonCtrl.GetSelectedEmoticon();
	
	if (m_pEmoticonSettings)
	{
		CString strEmoticonShortcut;
		
		if (pEmoticon2)
		{
			strEmoticonShortcut = pEmoticon2->GetShortcut();
		}
		else
		{
			strEmoticonShortcut = m_emoticonCtrl.GetSelectedEmoticonShortcut();
		}
		
		CString strMessage;
		strMessage.LoadString(IDS_EMOTICONS_PROPPAGE_REMOVECONFIRM_MESSAGE);
		
		UINT ret = CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_YESNO | MB_ICONQUESTION);
        if (ret == IDYES)
		{
			UpdateChangingList("", strEmoticonShortcut, "", OPERATION_ID_DEL);
		}
	}
	
	return 0L;
}


LRESULT CEmoticonsPropPage::OnEmoticonEditShortcut(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IUOLMessengerUIManager5Ptr pUIManager5 = ap_dynamic_cast<IUOLMessengerUIManager5Ptr>(
			CUIMApplication::GetApplication()->GetUIManager());
	
	CString strOldEmoticonShortcut = m_emoticonCtrl.GetSelectedEmoticonShortcut();
	CString strOldEmoticonFileName = m_emoticonCtrl.GetSelectedEmoticonFileName();
	
	if ( (pUIManager5) && (!strOldEmoticonShortcut.IsEmpty()) && 
			(!strOldEmoticonFileName.IsEmpty()) )
	{
		IUOLMessengerAddCustomEmoticonWindowPtr pAddCustomEmoticonWindow;
		
		if ( (!strOldEmoticonShortcut.IsEmpty()) && (!strOldEmoticonFileName.IsEmpty()) )
		{
			pAddCustomEmoticonWindow = pUIManager5->GetAddCustomEmoticonDialog(
							strOldEmoticonShortcut, strOldEmoticonFileName, 
							CAddCustomEmoticonDialog::OPERATION_MODE_CHANGE_SHORTCUT, this);
		}
		
		if ( (pAddCustomEmoticonWindow) && (pAddCustomEmoticonWindow->ShowModal() == IDOK) )
		{
			CString strEmoticonShortcut = pAddCustomEmoticonWindow->GetCustomEmoticonShortcut();
			CString strEmoticonFileName = pAddCustomEmoticonWindow->GetCustomEmoticonFileName();
			
			UpdateChangingList(strOldEmoticonShortcut, strEmoticonShortcut, 
					strEmoticonFileName, OPERATION_ID_CHANGE_SHORTCUT);
		}
	}
	
	return 0L;
}


LRESULT CEmoticonsPropPage::OnEmoticonEditFilePath(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IUOLMessengerUIManager5Ptr pUIManager5 = ap_dynamic_cast<IUOLMessengerUIManager5Ptr>(
			CUIMApplication::GetApplication()->GetUIManager());
	
	CString strOldEmoticonShortcut = m_emoticonCtrl.GetSelectedEmoticonShortcut();
	CString strOldEmoticonFileName = m_emoticonCtrl.GetSelectedEmoticonFileName();
	
	if ( (pUIManager5) && (!strOldEmoticonShortcut.IsEmpty()) && 
			(!strOldEmoticonFileName.IsEmpty()) )
	{
		if (ATLPath::IsRelative(strOldEmoticonFileName))
		{
			CString strThemePath = m_pEmoticonSettings->GetDefaultDirPath();
			
			strOldEmoticonFileName = strThemePath + "\\" + strOldEmoticonFileName;
		}
		
		IUOLMessengerAddCustomEmoticonWindowPtr pAddCustomEmoticonWindow;
		
		if ( (!strOldEmoticonShortcut.IsEmpty()) && (!strOldEmoticonFileName.IsEmpty()) )
		{
			pAddCustomEmoticonWindow = pUIManager5->GetAddCustomEmoticonDialog(
							strOldEmoticonShortcut, strOldEmoticonFileName, 
							CAddCustomEmoticonDialog::OPERATION_MODE_CHANGE_FILEPATH, this);
		}
		
		if ( (pAddCustomEmoticonWindow) && (pAddCustomEmoticonWindow->ShowModal() == IDOK) )
		{
			CString strEmoticonShortcut = pAddCustomEmoticonWindow->GetCustomEmoticonShortcut();
			CString strEmoticonFileName = pAddCustomEmoticonWindow->GetCustomEmoticonFileName();
			
			UpdateChangingList("", strEmoticonShortcut, 
					strEmoticonFileName, OPERATION_ID_CHANGE_FILENAME);
		}
	}
	
	return 0L;
}


LRESULT CEmoticonsPropPage::OnFindChangingInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	EmoticonFindParams* pFindParams = (EmoticonFindParams*)lParam;
	EmoticonChangingInfoPtr pChangingInfo;
	
	if (pFindParams != NULL)
	{
		pChangingInfo = FindChangingItem(pFindParams->strShortcut, 
				pFindParams->nChangingInfoOperations, pFindParams->bConsiderOldShortcut);
		
		if (pChangingInfo)
		{
			pFindParams->bFound = TRUE;
		}
		else
		{
			pFindParams->bFound = FALSE;
		}
		
		return NOERROR;
	}
	
	return E_FAIL;
}


void CEmoticonsPropPage::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
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
		GetParameterValue(m_templateFileWithCustom, pMapParams, "templateFileWithCustom");
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "templateFileWithCustom");
	}
	
	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "clrEmoticonViewCtrlBorder");
		m_clrEmoticonCtrlBorder.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "clrEmoticonViewCtrlBorder");
		m_clrEmoticonCtrlBorder.SetColor("#4ea2c6");
	}
	
	m_hEmoticonCtrlBorderBrush = ::CreateSolidBrush(m_clrEmoticonCtrlBorder);
}


LRESULT CEmoticonsPropPage::OnCtlColor(HDC hdc, HWND hwnd)
{
	return (LRESULT) m_brushStatic.m_hBrush;
}


void CEmoticonsPropPage::OnEmoticonSettingsChanged()
{
	UpdateEmoticonList();
}


void CEmoticonsPropPage::UpdateChangingList(CString strOldShortcut, 
		CString strShortcut, CString strFileName, int nOperationId)
{
	// First of all we sinalize to parent window that the emoticon 
	// configuration will change.
	GetParent().SendMessage(WM_USER_PREFS_CHANGED);	
	
	EmoticonChangingInfoPtr pEmoticonChangingInfo;
	BOOL bAddOperationRemoved = FALSE;
	
	switch (nOperationId)
	{
		case IEmoticonChangingInfoFinder::OPERATION_ID_DEL:
			
			// No reason to make changes if we are going to remove 
			// the emoticon.
			pEmoticonChangingInfo = FindChangingItem(strShortcut, 
					OPERATION_ID_ADD | OPERATION_ID_CHANGE_SHORTCUT | 
					OPERATION_ID_CHANGE_FILENAME, TRUE);
			
			if (pEmoticonChangingInfo)
			{
				if (pEmoticonChangingInfo->m_nComboOperationIds == 
						IEmoticonChangingInfoFinder::OPERATION_ID_ADD)
				{
					bAddOperationRemoved = TRUE;
				}
				
				m_changingList.RemoveAt(m_changingList.Find(pEmoticonChangingInfo));
			}
			
			// If the original operation was an addition, we need to do nothing.
			if (!bAddOperationRemoved)
			{
				m_changingList.AddTail(new EmoticonChangingInfo(
						strOldShortcut, strShortcut, strFileName, nOperationId));
			}
			
		break;
			
		case IEmoticonChangingInfoFinder::OPERATION_ID_ADD:
			
			m_changingList.AddTail(new EmoticonChangingInfo(
					strOldShortcut, strShortcut, strFileName, nOperationId));
			
		break;
		
		case IEmoticonChangingInfoFinder::OPERATION_ID_CHANGE_SHORTCUT:
			
			pEmoticonChangingInfo = 
					FindChangingItem(strOldShortcut, OPERATION_ID_ALL, FALSE);
			
			if (!pEmoticonChangingInfo)
			{
				m_changingList.AddTail(new EmoticonChangingInfo(
						strOldShortcut, strShortcut, strFileName, nOperationId));
			}
			else if (pEmoticonChangingInfo->m_nComboOperationIds == 
					IEmoticonChangingInfoFinder::OPERATION_ID_ADD)
			{
				pEmoticonChangingInfo->m_strShortcut = strShortcut;
			}
			else if (pEmoticonChangingInfo->m_nComboOperationIds == 
					IEmoticonChangingInfoFinder::OPERATION_ID_CHANGE_FILENAME)
			{
				pEmoticonChangingInfo->m_nComboOperationIds |= OPERATION_ID_CHANGE_SHORTCUT;
				pEmoticonChangingInfo->m_strOldShortcut = strOldShortcut;
				pEmoticonChangingInfo->m_strShortcut = strShortcut;
			}
			else
			{
				pEmoticonChangingInfo->m_strShortcut = strShortcut;
			}
			
		break;
		
		case IEmoticonChangingInfoFinder::OPERATION_ID_CHANGE_FILENAME:
			
			pEmoticonChangingInfo = 
					FindChangingItem(strShortcut, OPERATION_ID_ALL, FALSE);
			
			if (!pEmoticonChangingInfo)
			{
				m_changingList.AddTail(new EmoticonChangingInfo(
						strOldShortcut, strShortcut, strFileName, nOperationId));
			}
			else if (pEmoticonChangingInfo->m_nComboOperationIds == 
					IEmoticonChangingInfoFinder::OPERATION_ID_ADD)
			{
				pEmoticonChangingInfo->m_strFileName = strFileName;
			}
			else // OPERATION_ID_CHANGE_SHORTCUT ou OPERATION_ID_CHANGE_FILENAME
			{
				pEmoticonChangingInfo->m_nComboOperationIds |= OPERATION_ID_CHANGE_FILENAME;
				pEmoticonChangingInfo->m_strFileName = strFileName;
			}
			
		break;
	}
	
	// TODO: Get it better adding just the new element instead of loading all the page.
	UpdateEmoticonList();
}


EmoticonChangingInfoPtr CEmoticonsPropPage::FindChangingItem(CString strShortcut, 
		int cmbOperationIds, BOOL bConsiderOldShortcut)
{
	for (POSITION pos = m_changingList.GetHeadPosition(); pos; )
	{
		EmoticonChangingInfoPtr pEmoticonChangingInfo = m_changingList.GetNext(pos);
		
		if (pEmoticonChangingInfo->m_nComboOperationIds & cmbOperationIds)
		{
			CString strComparingShortcut;
			
			if ( (pEmoticonChangingInfo->m_nComboOperationIds & OPERATION_ID_CHANGE_SHORTCUT) && 
					(bConsiderOldShortcut) )
			{
				strComparingShortcut = pEmoticonChangingInfo->m_strOldShortcut;
			}
			else
			{
				strComparingShortcut = pEmoticonChangingInfo->m_strShortcut;
			}
			
			if (strShortcut.CompareNoCase(strComparingShortcut) == 0)
			{
				return pEmoticonChangingInfo;
			}
		}
	}
	
	return NULL;
}


void CEmoticonsPropPage::UpdateEmoticonList()
{
	// Firstly we block the new button.
	m_wndButtonNew.EnableWindow(FALSE);
	
	// Get the name of the current Emoticon Set.
	CString strEmoticonSetName;
	m_cmbEmoticonSetNames.GetLBText(m_cmbEmoticonSetNames.GetCurSel(), strEmoticonSetName);
	
	// Update buttons enablement.
	IUOLMessengerProtocolManagerPtr pProtocolManager = 
			CUIMApplication::GetApplication()->GetProtocolManager();
	
	CIMProtocol5* pIMProtocol5 = NULL;
	
	if (pProtocolManager)
	{
		pIMProtocol5 = dynamic_cast<CIMProtocol5*>(pProtocolManager->GetProtocol(
				m_pEmoticonSettings->GetProtocolIdByEmoticonSetName(strEmoticonSetName)));
	}
	
	if ( (pIMProtocol5) && (pIMProtocol5->HasCustomEmoticonSupport()) )
	{
		m_wndButtonNew.EnableWindow(TRUE);
		
		UpdateEmoticonListWithCustom(strEmoticonSetName);
	}
	else
	{
		m_wndButtonNew.EnableWindow(FALSE);
		
		UpdateEmoticonListSimple(strEmoticonSetName);
	}
}


void CEmoticonsPropPage::UpdateEmoticonListSimple(CString strSelectedEmoticonSet)
{
	CString strHtml("");
	
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
		
		CString strTemplate;
		
		fp = fopen(strThemePath,"rt");
		ATLASSERT(fp!=NULL);
		nReads = fread(buffer,sizeof(char),10000,fp);
		buffer[nReads]=0x00;
		strTemplate = buffer;
		fclose(fp);
		
		CString strHTMLTop;
		CString strHTMLBottom;
		CString strBeginTemplate(TEMPLATE_BEGIN_STR);
		CString strEndTemplate(TEMPLATE_END_STR);
		
		CString strBeginLineTemplate(TEMPLATE_BEGIN_LINE_STR);
		CString strEndLineTemplate(TEMPLATE_END_LINE_STR);
		
		int iBeginTemplate;
		int iEndTemplate;
		CString strTemplateRow;
		iBeginTemplate = strTemplate.Find(strBeginTemplate) + strBeginTemplate.GetLength();
		iEndTemplate = strTemplate.Find(strEndTemplate);
		strHTMLTop = strTemplate.Left(iBeginTemplate);
		strTemplateRow = strTemplate.Mid(iBeginTemplate, iEndTemplate - iBeginTemplate);
		strHTMLBottom = strTemplate.Right(strTemplate.GetLength() - (strHTMLTop.GetLength() + strTemplateRow.GetLength()));
		
		int nEmoticonSetId = 
				m_pEmoticonSettings->GetEmoticonSetIdByName(strSelectedEmoticonSet);
		
		m_emoticonCtrl.SetEmoticonSetIds(nEmoticonSetId);
		
		CAtlList<IUOLMessengerEmoticonPtr> listEmoticon;
		m_pEmoticonSettings->GetEmoticonList(nEmoticonSetId, listEmoticon);
		
		CString strGeneratedHtmlBlock;
		GenerateEmoticonListHtmlBlock(strGeneratedHtmlBlock, strTemplateRow, listEmoticon, FALSE);
		
		strHtml = strHTMLTop + strGeneratedHtmlBlock + strHTMLBottom;
	}
	
	m_emoticonCtrl.SetText(strHtml);
}


void CEmoticonsPropPage::UpdateEmoticonListWithCustom(CString strSelectedEmoticonSet)
{
	CString strHtml("");
	
	if(!m_templateFileWithCustom.IsEmpty())
	{
		FILE *fp;
		int nReads;
		CHAR buffer[10000];
		
		CString strThemePath = m_templateFileWithCustom;
		
		IUOLMessengerUIManager3Ptr pUIManager3 = ap_dynamic_cast<IUOLMessengerUIManager3Ptr>(CUIMApplication::GetApplication()->GetUIManager());
		
		if (pUIManager3)
		{
			IUOLMessengerThemePropertiesPtr pThemeProperties = pUIManager3->GetCurrentThemeProperties();
			
			if (pThemeProperties)
			{
				strThemePath = pThemeProperties->GetDirPath() + _T("\\") + m_templateFileWithCustom;
			}
		}
		
		CString strTemplate;
		
		fp = fopen(strThemePath,"rt");
		ATLASSERT(fp!=NULL);
		nReads = fread(buffer,sizeof(char),10000,fp);
		buffer[nReads]=0x00;
		strTemplate = buffer;
		fclose(fp);
		
		CString strHTMLTop;
		CString strHTMLMiddle;
		CString strHTMLBottom;
		CString strBeginTemplate(TEMPLATE_BEGIN_STR);
		CString strEndTemplate(TEMPLATE_END_STR);
		CString strBeginCustomTemplate(TEMPLATE_BEGIN_CUSTOM_STR);
		CString strEndCustomTemplate(TEMPLATE_END_CUSTOM_STR);
		
		int iBeginCustomTemplate;
		int iEndCustomTemplate;
		int iBeginTemplate;
		int iEndTemplate;
		CString strTemplateRowCustom;
		CString strTemplateRow;
		iBeginCustomTemplate = strTemplate.Find(strBeginCustomTemplate) + strBeginCustomTemplate.GetLength();
		iEndCustomTemplate = strTemplate.Find(strEndCustomTemplate);
		iBeginTemplate = strTemplate.Find(strBeginTemplate) + strBeginTemplate.GetLength();
		iEndTemplate = strTemplate.Find(strEndTemplate);
		strHTMLTop = strTemplate.Left(iBeginCustomTemplate);
		strTemplateRowCustom = strTemplate.Mid(iBeginCustomTemplate, iEndCustomTemplate - iBeginCustomTemplate);
		strHTMLMiddle = strTemplate.Mid(iEndCustomTemplate, iBeginTemplate - iEndCustomTemplate);
		strTemplateRow = strTemplate.Mid(iBeginTemplate, iEndTemplate - iBeginTemplate);
		strHTMLBottom = strTemplate.Right(strTemplate.GetLength() - (strHTMLTop.GetLength() + 
				strTemplateRow.GetLength() + strHTMLMiddle.GetLength() + strTemplateRow.GetLength()));
		
		int nEmoticonSetId = 
				m_pEmoticonSettings->GetEmoticonSetIdByName(strSelectedEmoticonSet);
		
		m_emoticonCtrl.SetEmoticonSetIds(nEmoticonSetId | 
				IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_CUSTOM);
		
		// First we process custom block.
		//
		CAtlList<IUOLMessengerEmoticonPtr> listEmoticon;
		m_pEmoticonSettings->GetEmoticonList(
				IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_CUSTOM, listEmoticon);
		
		CString strGeneratedHtmlBlockCustom;
		GenerateEmoticonListHtmlBlock(strGeneratedHtmlBlockCustom, strTemplateRowCustom, listEmoticon, TRUE);
		
		// After we process the selected emoticon set block.
		//
		listEmoticon.RemoveAll();
		m_pEmoticonSettings->GetEmoticonList(nEmoticonSetId, listEmoticon);
		
		CString strGeneratedHtmlBlock;
		GenerateEmoticonListHtmlBlock(strGeneratedHtmlBlock, strTemplateRow, listEmoticon, FALSE);
		
		strHtml = strHTMLTop + strGeneratedHtmlBlockCustom + strHTMLMiddle + 
				strGeneratedHtmlBlock + strHTMLBottom;
	}
	
	m_emoticonCtrl.SetText(strHtml);
}


void CEmoticonsPropPage::GenerateEmoticonListHtmlBlock(CString& strGeneratedHtmlBlock, 
		const CString& strTemplateRow, CAtlList<IUOLMessengerEmoticonPtr>& listEmoticon, 
		BOOL bIsCustomList)
{
	BOOL bProcessed = FALSE;

    CString auxRowTemplate = strTemplateRow;
	
	CString strBeginLineTemplate(TEMPLATE_BEGIN_LINE_STR);
	CString strEndLineTemplate(TEMPLATE_END_LINE_STR);
	
	CString strTagTemplate(TEMPLATE_ID_TAG_STR);
	CString strTagTemplate2(TEMPLATE_ID_NEW_TAG_STR);
	CString strFileTemplate(TEMPLATE_FILE_TAG_STR);
	CString strAltTemplate(TEMPLATE_IMG_ALT_TAG_STR);
	CString strTextTemplate(TEMPLATE_TEXT_STR);
	
	int iLastTagTemplate = -1;
	
	for (POSITION pos = listEmoticon.GetHeadPosition(); pos; )
	{
        IUOLMessengerEmoticon2Ptr pEmoticon2 = 
				ap_dynamic_cast<IUOLMessengerEmoticon2Ptr>(listEmoticon.GetNext(pos));
		
		EmoticonChangingInfoPtr pEmoticonChangingInfo;
		
		if (pEmoticon2)
		{
			pEmoticonChangingInfo = FindChangingItem(pEmoticon2->GetShortcut(), OPERATION_ID_ALL, TRUE);
		}
		
		if ( (pEmoticon2) && ( (!pEmoticonChangingInfo) || 
				(pEmoticonChangingInfo->m_nComboOperationIds != OPERATION_ID_DEL) ) )
		{
			bProcessed = TRUE;
			
			CString strShortcut;
			CString strFilePath;
			
			if (!pEmoticonChangingInfo)
			{
				strShortcut = pEmoticon2->GetShortcut();
				strFilePath = pEmoticon2->GetFilePath();
			}
			else if ( (pEmoticonChangingInfo->m_nComboOperationIds & OPERATION_ID_CHANGE_FILENAME) || 
					(pEmoticonChangingInfo->m_nComboOperationIds & OPERATION_ID_CHANGE_SHORTCUT) )
			{
				strShortcut = pEmoticonChangingInfo->m_strShortcut;
				strFilePath = pEmoticonChangingInfo->m_strFileName;
			}
			
			if (ATLPath::IsRelative(strFilePath))
			{
				CString strThemePath = m_pEmoticonSettings->GetDefaultDirPath();
				
				strFilePath = strThemePath + "\\" + strFilePath;
			}
			
			/*
			if (strGeneratedHtmlBlock.Find(strFilePath) >= 0 || auxRowTemplate.Find(strFilePath) >= 0)
			{
				continue;
			}
			*/
			
			iLastTagTemplate = auxRowTemplate.Find(strTagTemplate);
			if (iLastTagTemplate < 0)
			{
				strGeneratedHtmlBlock += auxRowTemplate;
				auxRowTemplate = strTemplateRow;
				iLastTagTemplate = auxRowTemplate.Find(strTagTemplate);
			}
			
			CHTMLUtilitary::Replace(auxRowTemplate, strFileTemplate, strFilePath);
			
			if (strShortcut.Find("\"") < 0)
			{
				CHTMLUtilitary::Replace(auxRowTemplate, strTagTemplate, 
						strTagTemplate2 + CHTMLUtilitary::ScapeText(strShortcut) + 
						strTagTemplate2);
				CHTMLUtilitary::Replace(auxRowTemplate, strAltTemplate, strShortcut);
				CHTMLUtilitary::Replace(auxRowTemplate, strTextTemplate, 
						CHTMLFilter::TextToHTML(strShortcut));
			}
			else
			{
				CHTMLUtilitary::ReplaceIncludingQuotes(auxRowTemplate, strTagTemplate, 
						strTagTemplate2 + CHTMLUtilitary::ScapeText(strShortcut) + 
						strTagTemplate2, '\'');
				CHTMLUtilitary::ReplaceIncludingQuotes(auxRowTemplate, strAltTemplate, 
						strShortcut, '\'');
				CHTMLUtilitary::Replace(auxRowTemplate, strTextTemplate, 
						CHTMLFilter::TextToHTML(strShortcut));
			}
		}
	}
	
	if (bIsCustomList)
	{
		for (POSITION pos = m_changingList.GetHeadPosition(); pos; )
		{
			EmoticonChangingInfoPtr pEmoticonChangingInfo = m_changingList.GetNext(pos);
			
			if (pEmoticonChangingInfo->m_nComboOperationIds == 
					IEmoticonChangingInfoFinder::OPERATION_ID_ADD)
			{
				bProcessed = TRUE;
				
				CString strShortcut = pEmoticonChangingInfo->m_strShortcut;
				CString strFilePath = pEmoticonChangingInfo->m_strFileName;
				
				iLastTagTemplate = auxRowTemplate.Find(strTagTemplate);
				if (iLastTagTemplate < 0)
				{
					strGeneratedHtmlBlock += auxRowTemplate;
					auxRowTemplate = strTemplateRow;
					iLastTagTemplate = auxRowTemplate.Find(strTagTemplate);
				}
				
				CHTMLUtilitary::Replace(auxRowTemplate, strFileTemplate, strFilePath);
				
				if (strShortcut.Find("\"") < 0)
				{
					CHTMLUtilitary::Replace(auxRowTemplate, strTagTemplate, 
							strTagTemplate2 + CHTMLUtilitary::ScapeText(strShortcut) + 
							strTagTemplate2);
					CHTMLUtilitary::Replace(auxRowTemplate, strAltTemplate, strShortcut);
					CHTMLUtilitary::Replace(auxRowTemplate, strTextTemplate, 
							CHTMLFilter::TextToHTML(strShortcut));
				}
				else
				{
					CHTMLUtilitary::ReplaceIncludingQuotes(auxRowTemplate, strTagTemplate, 
							strTagTemplate2 + CHTMLUtilitary::ScapeText(strShortcut) + 
							strTagTemplate2, '\'');
					CHTMLUtilitary::ReplaceIncludingQuotes(auxRowTemplate, strAltTemplate, 
							strShortcut, '\'');
					CHTMLUtilitary::Replace(auxRowTemplate, strTextTemplate, 
							CHTMLFilter::TextToHTML(strShortcut));
				}
			}
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
	
	if (bProcessed)
    {
        strGeneratedHtmlBlock += sAux;
    }
}


void CEmoticonsPropPage::InitProtocolList()
{
	// Always the Emoticon Sets should have the same name of the protocols 
	// wich they are related with.
	//
	CConstAtlListIterator< CIMProtocol* > itProtocolsList;
	CUIMApplication::GetApplication()->GetProtocolManager()->GetProtocolList(itProtocolsList);
	
	CIMProtocol* pProtocol;
	
	while (itProtocolsList != NULL)
	{
		pProtocol = *itProtocolsList;
		if (pProtocol != NULL)
		{
			CString strProtocol = pProtocol->GetName().c_str();
			
			COMBOBOXEXITEM cbItem = {0};
			cbItem.iItem = -1;
			cbItem.mask = CBEIF_TEXT /*| CBEIF_IMAGE*/;
			cbItem.pszText = (LPTSTR) (LPCTSTR) strProtocol;
			cbItem.cchTextMax = strProtocol.GetLength();
			
			//int nItem = m_wndComboBoxProtocols.InsertItem(&cbItem);
			int nItem = m_cmbEmoticonSetNames.AddString(strProtocol);
			m_cmbEmoticonSetNames.SetItemData(nItem, (DWORD_PTR) pProtocol);
			
			ATLASSERT(m_cmbEmoticonSetNames.GetCount() > 0);
		}
		
		itProtocolsList++;
	}
	
	m_cmbEmoticonSetNames.SetCurSel(m_cmbEmoticonSetNames.FindString(0, "MSN"));
}


void CEmoticonsPropPage::OnApply()
{
	int nEmoticonSetId = IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_INVALID;
	
	for (POSITION pos = m_changingList.GetHeadPosition(); pos; )
	{
		EmoticonChangingInfoPtr pEmoticonChangingInfo = m_changingList.GetNext(pos);
		
		if (pEmoticonChangingInfo->m_nComboOperationIds == 
				IEmoticonChangingInfoFinder::OPERATION_ID_ADD)
		{
			m_pEmoticonSettings->AddEmoticon2(
					pEmoticonChangingInfo->m_strShortcut, 
					pEmoticonChangingInfo->m_strFileName);
		}
		else if (pEmoticonChangingInfo->m_nComboOperationIds == OPERATION_ID_DEL)
		{
			m_pEmoticonSettings->RemoveEmoticon2(
					pEmoticonChangingInfo->m_strShortcut);
		}
		else 
		{
			if (pEmoticonChangingInfo->m_nComboOperationIds & OPERATION_ID_CHANGE_SHORTCUT)
			{
				nEmoticonSetId = m_pEmoticonSettings->GetEmoticonOwnerId(
						IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_ALL, 
						pEmoticonChangingInfo->m_strOldShortcut);
				
				if (nEmoticonSetId != IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_INVALID)
				{
					m_pEmoticonSettings->ChangeEmoticonShortcut(nEmoticonSetId, 
							pEmoticonChangingInfo->m_strOldShortcut, 
							pEmoticonChangingInfo->m_strShortcut);
				}
			}
			
			if (pEmoticonChangingInfo->m_nComboOperationIds & OPERATION_ID_CHANGE_FILENAME)
			{
				nEmoticonSetId = m_pEmoticonSettings->GetEmoticonOwnerId(
						IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_ALL, 
						pEmoticonChangingInfo->m_strShortcut);
				
				if (nEmoticonSetId != IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_INVALID)
				{
					m_pEmoticonSettings->ChangeEmoticonFilePath(nEmoticonSetId, 
							pEmoticonChangingInfo->m_strShortcut, 
							pEmoticonChangingInfo->m_strFileName);
				}
			}
		}
	}
	
	m_changingList.RemoveAll();
	
	m_pEmoticonSettings->SaveSettings();
}


void CEmoticonsPropPage::CalculatePaintingRgns()
{
	CRgnHandle hMainClippingRgn;
	CRgnHandle hBorderClippingRgn;
	CRgn hAddButtonRgn;
	CRgn hStaticEmoticonSetRgn;
	CRgn hComboEmoticonSetRgn;
	CRgn hEmoticonCtrlDeflatedRgn;
	
	CRect windowRect;
	GetClientRect(&windowRect);
	
	CRect addButtonRect;
	::GetWindowRect(m_wndButtonNew.m_hWnd, &addButtonRect);
	ScreenToClient(addButtonRect);
	
	CRect staticEmoticonSetRect;
	::GetWindowRect(GetDlgItem(IDC_STATIC_EMOTICON_SET), &staticEmoticonSetRect);
	ScreenToClient(staticEmoticonSetRect);
	
	CRect cmbEmoticonSetRect;
	::GetWindowRect(m_cmbEmoticonSetNames.m_hWnd, &cmbEmoticonSetRect);
	ScreenToClient(cmbEmoticonSetRect);
	
	CRect emoticonCtrlDeflatedRect = m_rectEmoticonCtrl;
	CRect emoticonCtrlInflatedRect = emoticonCtrlDeflatedRect;
	emoticonCtrlInflatedRect.InflateRect(EMOTICON_CTRL_BORDER_WIDTH, 
			EMOTICON_CTRL_BORDER_WIDTH);
	
	hMainClippingRgn.CreateRectRgn(windowRect.left, windowRect.top, 
			windowRect.left + windowRect.Width(), windowRect.top + windowRect.Height());
	
	hAddButtonRgn.CreateRectRgn(addButtonRect.left, addButtonRect.top, 
			addButtonRect.left + addButtonRect.Width(),
			addButtonRect.top + addButtonRect.Height());
	
	hStaticEmoticonSetRgn.CreateRectRgn(staticEmoticonSetRect.left, staticEmoticonSetRect.top, 
			staticEmoticonSetRect.left + staticEmoticonSetRect.Width(),
			staticEmoticonSetRect.top + staticEmoticonSetRect.Height());
	
	hComboEmoticonSetRgn.CreateRectRgn(cmbEmoticonSetRect.left, cmbEmoticonSetRect.top, 
			cmbEmoticonSetRect.left + cmbEmoticonSetRect.Width(),
			cmbEmoticonSetRect.top + cmbEmoticonSetRect.Height());
	
	hEmoticonCtrlDeflatedRgn.CreateRectRgn(emoticonCtrlDeflatedRect.left, 
			emoticonCtrlDeflatedRect.top, 
			emoticonCtrlDeflatedRect.left + emoticonCtrlDeflatedRect.Width(),
			emoticonCtrlDeflatedRect.top + emoticonCtrlDeflatedRect.Height());
	
	hBorderClippingRgn.CreateRectRgn(emoticonCtrlInflatedRect.left, 
			emoticonCtrlInflatedRect.top, 
			emoticonCtrlInflatedRect.left + emoticonCtrlInflatedRect.Width(),
			emoticonCtrlInflatedRect.top + emoticonCtrlInflatedRect.Height());
	
	hMainClippingRgn.CombineRgn(hAddButtonRgn, RGN_DIFF);
	hMainClippingRgn.CombineRgn(hStaticEmoticonSetRgn, RGN_DIFF);
	hMainClippingRgn.CombineRgn(hComboEmoticonSetRgn, RGN_DIFF);
	hMainClippingRgn.CombineRgn(hEmoticonCtrlDeflatedRgn, RGN_DIFF);
	
	hBorderClippingRgn.CombineRgn(hEmoticonCtrlDeflatedRgn, RGN_DIFF);
	
	m_mainWindowRgn = (HRGN) hMainClippingRgn;
	m_emoticonCtrlBorderRgn = (HRGN) hBorderClippingRgn;
}

