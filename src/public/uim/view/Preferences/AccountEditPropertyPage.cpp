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
#include "AccountEditPropertyPage.h"

#include "../TextInputLimits.h"
#include "../Image/imageBuilder.h"
#include "MinDependencySpecialFolder.h"
#include "../../controller/UIMApplication.h"
#include "../../AntiSpamValidation.h"
#include "../UIMFileDialog.h"

#include <jabberconstants.h>


CAccountEditPropertyPage::CAccountEditPropertyPage(IUOLMessengerAccountPtr pAccount, OperationMode mode) :
	m_pAccount(pAccount),
	m_mode(mode),
	m_nConnectOnStartUp(0),
	m_nSavePassword(0),
	m_bOldWebAware(FALSE),
	m_bOldRequestAuthorization(TRUE)
{
	m_strTitle.LoadString(IDS_ACCOUNT_EDIT_PROPPAGE_CAPTION);
	SetTitle((LPCTSTR) m_strTitle);

	ATLASSERT(m_pAccount);
}


CAccountEditPropertyPage::~CAccountEditPropertyPage()
{
}


void CAccountEditPropertyPage::OnConnected(IUOLMessengerAccountPtr pAccount)
{
	if (m_mode == OM_CHANGE_ACCOUNT)
	{
		if ((m_pAccount.get()) == (pAccount.get()))
		{
			UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
			CString strMessage;
			strMessage = pStringLoader->GetValue("uol.resource.error.IDS_ACCOUNTS_USER_CONNECTED");

			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
			
			CWindow wndParent = GetPropertySheet();
			wndParent.PostMessage(WM_CLOSE);
		}
	}
}


void CAccountEditPropertyPage::OnDisconnected(IUOLMessengerAccountPtr pAccount)
{
	UpdateFields();
}


void CAccountEditPropertyPage::OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage)
{
	OnDisconnected(pAccount);
}


LRESULT CAccountEditPropertyPage::OnInitDialog(HWND wnd, LPARAM lParam)
{
	if (m_mode == OM_CHANGE_ACCOUNT)
	{
		CUIMApplication::GetApplication()->GetAccountConnectionManager()->RegisterAccountObserver(m_pAccount, this);
	}

	// Set controls text limit...
	struct TextInputLimitInit
	{
		DWORD dwId;
		UINT nLimit;
	};

	TextInputLimitInit limits[] = 
	{
		{ IDC_EDIT_USER, TIL_ACCOUNT_USER },
		{ IDC_EDIT_PASS, TIL_ACCOUNT_PASSWORD },
		{ IDC_EDIT_NICK, TIL_ACCOUNT_NICKNAME }
	};

	for (int nInd = 0; nInd < sizeof(limits) / sizeof(limits[0]); nInd++)
	{
		CEdit wndEdit = GetDlgItem(limits[nInd].dwId);
		wndEdit.SetLimitText(limits[nInd].nLimit);
	}

	m_wndComboBoxProtocols.SubclassWindow(GetDlgItem(IDC_COMBO_NET));

	InitProtocolList();

	// Show values from account...
	int nProtocolInd = m_wndComboBoxProtocols.FindStringExact(-1, m_pAccount->GetProtocolName());

	if (nProtocolInd != CB_ERR)
	{
		m_wndComboBoxProtocols.SetCurSel(nProtocolInd);
	}

	m_strUser = m_pAccount->GetUserId();
	m_strPass = m_pAccount->GetPassword();
	m_strNick = m_pAccount->GetNickName();
	m_pUserIcon = m_pAccount->GetUserIcon();
	m_nConnectOnStartUp = m_pAccount->IsAutoConnectEnabled();
	m_nSavePassword = m_pAccount->IsSavePasswordEnabled();
	m_strOldServer = GetAccountSettingsConnectServer();
	m_bOldWebAware = GetAccountSettingsWebAware();
	m_bOldRequestAuthorization = GetAccountSettingsRequestAuthorization();

	DoDataExchange(FALSE);

	UpdateFields();

	OnSelChangeProtocolComboBoxImpl();

	return 0L;
}


void CAccountEditPropertyPage::OnDestroy()
{
	//m_wndComboBoxProtocols.UnsubclassWindow();
	if (m_mode == OM_CHANGE_ACCOUNT)
	{
		CUIMApplication::GetApplication()->GetAccountConnectionManager()->UnregisterAccountObserver(m_pAccount, this);
	}
}


void CAccountEditPropertyPage::OnPaint(HDC hDC)
{
	CPaintDC dc(m_hWnd);

	if (m_pUserIcon != NULL)
	{
		IUOLMessengerImagePtr pImage = m_pUserIcon->GetImage();

		if (pImage != NULL)
		{
			IUOLMessengerImageDrawStrategyPtr pStrategy = new UOLMessengerImage::CDrawStretchOpaqueStrategy();

			pImage->Draw(CDCHandle(dc), 
					146, 186, 
					48, 48,
					pStrategy);
		}
	}

	SetMsgHandled(FALSE);
}


void CAccountEditPropertyPage::OnSelChangeProtocolComboBox(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	OnSelChangeProtocolComboBoxImpl();

	DoDataExchange(TRUE);

	UpdateAccountSettingsConnectServer();
}


void CAccountEditPropertyPage::OnButtonChangeAvatar(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	int nCurSel = -1;

	if (m_pAccount)
	{
		// Init file dialog...
		CMinDependencySpecialFolder mdsf;
		CString strUserDir = mdsf.GetPersonal();

		TCHAR szFilter[MAX_PATH + 1] = "Arquivos de Imagem (*.bmp;*.gif;*.jpeg;*.jpg)|*.bmp;*.gif;*.jpeg;*.jpg|";

		CWindow tmpWindow = CUIMApplication::GetApplication()->GetUIManager()->GetAppWindow();
        CUIMFileDialog fileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, tmpWindow.m_hWnd);
		fileDialog.m_ofn.lpstrInitialDir = CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->GetLastAvatarFolder();
		
		// Request image path
		if ((IDOK == fileDialog.DoModal()) && (fileDialog.m_szFileName != NULL))
		{
			// Try load image
			CString strFilePath = fileDialog.m_szFileName;

			CString strFileFolder = strFilePath.Left(strFilePath.ReverseFind('\\'));
			CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->SetLastAvatarFolder(strFileFolder);

			IUOLMessengerUserIconPtr pUserIcon = CUIMApplication::GetApplication()->GetUserIconCacheManager()->CacheIcon(m_pAccount.get(), strFilePath);

			if (pUserIcon != NULL)
			{
				// Update account user icon...
				if ((m_pAccount != NULL))
				{
					m_pAccount->SetUserIcon(pUserIcon);

					IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

					if (pAccountConnectionManager->IsConnected(m_pAccount))
					{
						pAccountConnectionManager->UploadDisplayIcon(m_pAccount);
					}
				}
								
				// Update UI...
				m_pUserIcon = pUserIcon;

				InvalidateRect(NULL);
				UpdateWindow();
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogWarning(__FUNCTION__, 
						"O arquivo [%s] é inválido. Somente os formatos (*.bmp;*.gif;*.jpeg;*.jpg) são aceitos.", 
						strFilePath);
				
				UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
				CString strMessage;
				strMessage = pStringLoader->GetValue("uol.resource.error.IDS_ACCOUNTS_PROPPAGE_INVALID_FILE_MSG");

				CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
			}
		}
	}
}


void CAccountEditPropertyPage::OnKillFocus(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	DoDataExchange(TRUE);

	UpdateAccountSettingsConnectServer();

	SetAccountSettingsWebAware(m_bOldWebAware);
	SetAccountSettingsRequestAuthorization(m_bOldRequestAuthorization);
}


int CAccountEditPropertyPage::OnApply()
{
	if (ValidateFields())
	{
		CIMProtocol* pProtocol = GetComboProtocol();

		IUOLMessengerAccountManagerPtr pAccountManager = CUIMApplication::GetApplication()->GetAccountManager();
		ATLASSERT(pAccountManager != NULL);

		BOOL bChanged = pAccountManager->SetAccountProtocol(m_pAccount, pProtocol->GetId().c_str());
		ATLASSERT(bChanged);

		bChanged = pAccountManager->SetAccountUserId(m_pAccount, m_strUser);
		ATLASSERT(bChanged);

		if (m_nSavePassword)
		{
			m_pAccount->SetPassword(m_strPass);
		}
		else
		{
			// Empty password
			m_pAccount->SetPassword(_T(""));
		}

		m_pAccount->EnableAutoConnect(m_nConnectOnStartUp);
		m_pAccount->EnableSavePassword(m_nSavePassword);
		m_pAccount->SetNickName(m_strNick);

		// Update remote alias
		IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();

		if (pAccountConnectionManager->IsConnected(m_pAccount))
		{
			pAccountConnectionManager->UpdateServerAlias(m_pAccount);
		}

		m_pAccount->SetUserIcon(m_pUserIcon);

		m_strOldServer = GetAccountSettingsConnectServer();
		m_bOldWebAware = GetAccountSettingsWebAware();
		m_bOldRequestAuthorization = GetAccountSettingsRequestAuthorization();

		return PSNRET_NOERROR;
	}

	return PSNRET_INVALID_NOCHANGEPAGE;
}


BOOL CAccountEditPropertyPage::OnQueryCancel()
{
	// FALSE = allow cancel
	// TRUE = prevent cancel
	SetAccountSettingsConnectServer(m_strOldServer);
	SetAccountSettingsWebAware(m_bOldWebAware);
	SetAccountSettingsRequestAuthorization(m_bOldRequestAuthorization);

	return FALSE;
}


void CAccountEditPropertyPage::InitProtocolList()
{
	CConstAtlListIterator< CIMProtocol* > itProtocolsList;
	CUIMApplication::GetApplication()->GetProtocolManager()->GetProtocolList(itProtocolsList);

	//m_wndComboBoxProtocols.Attach(GetDlgItem(IDC_COMBO_NET));

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
			int nItem = m_wndComboBoxProtocols.AddString(strProtocol);
			m_wndComboBoxProtocols.SetItemData(nItem, (DWORD_PTR) pProtocol);

			ATLASSERT(m_wndComboBoxProtocols.GetCount() > 0);
		}

		itProtocolsList++;
	}
}


void CAccountEditPropertyPage::SplitUserId(const CString& strUserId, const CString& strSplitter, CString& strSuffix, CString& strPrefix)
{
	if (FALSE == strSplitter.IsEmpty())
	{
		int nFind = strUserId.Find(strSplitter);

		if (nFind != -1)
		{
			strSuffix = strUserId.Left(nFind);
			strPrefix = strUserId.Mid(nFind + 1);
		}

		return;
	}

	strSuffix = strUserId;
	strPrefix = "";
}


void CAccountEditPropertyPage::UpdateFields()
{
	DWORD connectionState = CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectionState(m_pAccount);

	BOOL bDisconnected = (connectionState == IUOLMessengerAccountConnectionManager::CS_DISCONNECTED);

	GetDlgItem(IDC_EDIT_USER).EnableWindow(bDisconnected);
	GetDlgItem(IDC_EDIT_PASS).EnableWindow(bDisconnected);
	GetDlgItem(IDC_COMBO_NET).EnableWindow(bDisconnected);
}


BOOL CAccountEditPropertyPage::ValidateFields()
{
	DoDataExchange(TRUE);

	CIMProtocol * pProtocol = GetComboProtocol();
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);
	// Check empty username
	if (m_strUser.IsEmpty())
	{
		CString strMessage;
		strMessage = pStringLoader->GetValue("uol.resource.error.IDS_ACCOUNTS_PROPPAGE_EMPTY_USER_MESSAGE");

		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);

		GetDlgItem(IDC_EDIT_USER).SetFocus();
		return FALSE;
	}

	if (m_strUser.IsEmpty())
	{
		::DebugBreak();
	}

	// Check valid user name
	if (!pProtocol->IsValidUserName((LPCTSTR) m_strUser))
	{
		CString strMessageFormat;
		strMessageFormat = pStringLoader->GetValue("uol.resource.error.IDS_ACCOUNTS_PROPPAGE_INVALID_USERNAME_MSG_FORMAT");
		
		CString strMessage;
		strMessage.Format(strMessageFormat, m_strUser, pProtocol->GetName().c_str());

		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);

		GetDlgItem(IDC_EDIT_USER).SetFocus();
		return FALSE;
	}

	if (0 == strcmp(_T("prpl-uol"), pProtocol->GetId().c_str()))
	{
		if (CAntiSpamValidation::checkEMail(m_strUser) != CAntiSpamValidation::AS_NO_ERROR)
		{
			CString strMessageFormat;
			strMessageFormat = pStringLoader->GetValue("uol.resource.error.IDS_ACCOUNTS_PROPPAGE_INVALID_USERNAME_MSG_FORMAT");

			CString strMessage;
			strMessage.Format(strMessageFormat, m_strUser, pProtocol->GetName().c_str());

			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);

			GetDlgItem(IDC_EDIT_USER).SetFocus();
			return FALSE;
		}
	}

	// Check duplicated accounts
	CString strProtocolId = pProtocol->GetId().c_str();
	
	BOOL bDuplicate = CheckDuplicateAccount(m_strUser, strProtocolId, m_pAccount);
	if (bDuplicate)
	{
		CString strMessageFormat;
		strMessageFormat = pStringLoader->GetValue("uol.resource.error.IDS_ACCOUNTS_PROPPAGE_USER_ALREADY_EXISTS_MSG_FORMAT");

		CString strMessage;
		strMessage.Format(strMessageFormat, pProtocol->GetName().c_str(), m_strUser);
		
		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(m_hWnd, strMessage, MB_OK | MB_ICONERROR);
		
		GetDlgItem(IDC_EDIT_USER).SetFocus();
		return FALSE;
	}
	
	return TRUE;
}


CIMProtocol* CAccountEditPropertyPage::GetComboProtocol()
{
	int nCurSelItem = m_wndComboBoxProtocols.GetCurSel();

	DWORD_PTR pData = m_wndComboBoxProtocols.GetItemData(nCurSelItem);
	
	if (pData != LB_ERR)
	{
		return (CIMProtocol*) pData;
	}

	return NULL;
}


void CAccountEditPropertyPage::OnSelChangeProtocolComboBoxImpl()
{
	CIMProtocol* pProtocol = GetComboProtocol();
	if (pProtocol != NULL)
	{
		CWindow wndParent = GetPropertySheet();

		CHANGEADVANCEDPROPPAGEPARAM cap;
		cap.strProtocolId = pProtocol->GetId().c_str();

		wndParent.SendMessage(WM_USER_CHANGE_ADVANCED_PROPPAGE, (WPARAM) &cap, 0);
	}
}


BOOL CAccountEditPropertyPage::CheckDuplicateAccount(const CString& strUserId, const CString& strProtocolId, IUOLMessengerAccountPtr pCurrentAccount)
{
	IUOLMessengerAccountPtr pFindAccount = CUIMApplication::GetApplication()->GetAccountManager()->FindAccount(strUserId, strProtocolId);

	return ((pFindAccount != NULL) && (pFindAccount != pCurrentAccount));
}


CString CAccountEditPropertyPage::GetParentTitle()
{
	CString strTitle;

	CWindow wndParent = GetPropertySheet();
	
	ATLASSERT(wndParent.IsWindow());
	wndParent.GetWindowText(strTitle);

	return strTitle;
}


CString CAccountEditPropertyPage::GetAccountConnectServer(const CString& strEmail, BOOL& bForce)
{
	CString strServer;

	CIMProtocol* pProtocol = GetComboProtocol();

	bForce = FALSE;

	if ((pProtocol != NULL) && 
		(pProtocol->IsValidUserName((LPCTSTR) strEmail)))
	{
		// TODAOFERTA Protocol - prpl-uolto
		if (0 == (pProtocol->GetId()).compare("prpl-uolto"))
		{
			bForce = TRUE;

			UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();

			if (pStringLoader)
			{
				strServer = pStringLoader->GetValue("uol.resource.domain.todaoferta");
				ATLASSERT(FALSE == strServer.IsEmpty());
			}
		}
		// UOL Protocol - prpl-uol
		else if (0 == (pProtocol->GetId()).compare("prpl-uol"))
		{
			CString strBOLDomain;
			CString strUOLDomain;
			strBOLDomain.LoadString(IDS_JABBER_BOL_EMAIL_DOMAIN);
			strUOLDomain.LoadString(IDS_JABBER_UOL_EMAIL_DOMAIN);			

			if ((strEmail.Find(strBOLDomain) != -1) || (strEmail.Find(strUOLDomain) != -1))
			{
				strServer.LoadString(IDS_JABBER_UOL_SERVER);
			}
		}
		// Jabber Protocol - prpl-jabber
		else if (0 == (pProtocol->GetId()).compare("prpl-jabber"))
		{
			CString strDomain;
			strDomain.LoadString(IDS_JABBER_GTALK_EMAIL_DOMAIN);

			if (strEmail.Find(strDomain) != -1)
			{
				strServer.LoadString(IDS_JABBER_GTALK_SERVER);
			}
		}
	}

	return strServer;
}


CString CAccountEditPropertyPage::GetAccountSettingsConnectServer()
{
	CString strServer;

	CWindow wndParent = GetPropertySheet();

	wndParent.SendMessage(WM_USER_GET_ADVANCED_SETTING_STRING, (WPARAM) JABBER_SETTING_CONNECT_SERVER, (LPARAM) &strServer);

	return strServer;
}


void CAccountEditPropertyPage::SetAccountSettingsConnectServer(const CString& strServer)
{
	CWindow wndParent = GetPropertySheet();

	wndParent.SendMessage(WM_USER_SET_ADVANCED_SETTING_STRING, (WPARAM) JABBER_SETTING_CONNECT_SERVER, (LPARAM) (LPCTSTR) strServer);
}


void CAccountEditPropertyPage::UpdateAccountSettingsConnectServer()
{
	// Autofill connection server according to account user id 
	// if user has not specified one
	BOOL bForce = FALSE;
	CString strSystemServer = GetAccountConnectServer(m_strUser, bForce);

	if (FALSE == strSystemServer.IsEmpty())
	{
		CString strUserServer = GetAccountSettingsConnectServer();

		if ((strUserServer.IsEmpty()) || bForce)
		{
			ATLTRACE(_T("%s - Fill connection server = [%s]\n"), __FUNCTION__, strSystemServer);

			SetAccountSettingsConnectServer(strSystemServer);
		}
	}
}


BOOL CAccountEditPropertyPage::GetAccountSettingsWebAware()
{
	BOOL bWebAware = FALSE;

	CWindow wndParent = GetPropertySheet();

	wndParent.SendMessage(WM_USER_GET_ADVANCED_SETTING_BOOLEAN, (WPARAM) "web_aware", (LPARAM) &bWebAware);

	return bWebAware;
}


void CAccountEditPropertyPage::SetAccountSettingsWebAware(const BOOL& bWebAware)
{
	CWindow wndParent = GetPropertySheet();

	wndParent.SendMessage(WM_USER_SET_ADVANCED_SETTING_BOOLEAN, (WPARAM) "web_aware", (LPARAM) bWebAware);
}

BOOL CAccountEditPropertyPage::GetAccountSettingsRequestAuthorization()
{
	BOOL bRequestAuthorization = TRUE;

	CWindow wndParent = GetPropertySheet();

	wndParent.SendMessage(WM_USER_GET_ADVANCED_SETTING_BOOLEAN, (WPARAM) "authorization", (LPARAM) &bRequestAuthorization);

	return bRequestAuthorization;
}


void CAccountEditPropertyPage::SetAccountSettingsRequestAuthorization(const BOOL& bRequestAuthorization)
{
	CWindow wndParent = GetPropertySheet();

	wndParent.SendMessage(WM_USER_SET_ADVANCED_SETTING_BOOLEAN, (WPARAM) "authorization", (LPARAM) bRequestAuthorization);
}
