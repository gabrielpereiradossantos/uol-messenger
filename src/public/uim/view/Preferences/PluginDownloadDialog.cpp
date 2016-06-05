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

#include "PluginDownloadDialog.h"
#include "../../controller/UIMApplication.h"

ULONG CPluginDownloadDialog::MIN_TOTAL_BYTES = 1;


CPluginDownloadDialog::CPluginDownloadDialog(CPluginNodePtr pPluginNode) : 
	m_pPluginNode(pPluginNode),
	m_bStopped(FALSE),
	m_bInitialized(FALSE)
{
	ATLASSERT(m_pPluginNode);

	m_strCancelText.LoadString(IDS_FILE_TRANSFER_DLG_CANCEL_BUTTON);
	m_strDoneText.LoadString(IDS_FILE_TRANSFER_DLG_DONE_BUTTON);
}


CPluginDownloadDialog::~CPluginDownloadDialog()
{
}


void CPluginDownloadDialog::OnDownloadBegin()
{
}


void CPluginDownloadDialog::OnProgressChange(ULONG nProgress, ULONG nProgressMax)
{
	if (!IsWindow())
	{
		return;
	}

	if (!m_bInitialized)
	{
		m_wndProgress.SetRange32(0, nProgressMax);
		m_bInitialized = TRUE;
	}

	ULONG nTransferedKBytes = (ULONG) (double) ceil(nProgress / 1024.0);
	ULONG nTotalKBytes = (ULONG) (double) ceil(nProgressMax / 1024.0);

	double dPercent = (nProgressMax > 0) ? (100.0 * ((double) nProgress / nProgressMax)) : 0.0;

	ATLASSERT(nTransferedKBytes <= nTotalKBytes);

	CString strFileNameText;
	strFileNameText.Format(IDS_FILE_TRANSFER_DLG_FILE_FORMAT, m_pPluginNode->GetPluginInfo()->GetFileName(), nTransferedKBytes, nTotalKBytes, dPercent);

	m_wndFileName.SetWindowText(strFileNameText);
	m_wndProgress.SetPos(nProgress);
}


void CPluginDownloadDialog::OnDownloadComplete(DWORD dwError)
{
	if (dwError == CHttpDownloader::DE_ERROR_SUCCESS)
	{
		EndDialog(IDOK);
	}
	else
	{
		EndDialog(IDCANCEL);
	}
}


LRESULT CPluginDownloadDialog::OnInitDialog(HWND /*hWnd*/, LPARAM /*lparam*/)
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader != NULL);

	CString strTitle = pStringLoader->GetValue("uol.resource.IDS_FILE_TRANSFER_DLG_CAPTION");
	ATLASSERT(FALSE == strTitle.IsEmpty());

	SetWindowText(strTitle);

	// Get dialog controls
	m_wndFileName = GetDlgItem(IDC_STATIC_FILE_NAME);
	m_wndContact = GetDlgItem(IDC_STATIC_CONTACT);
	m_wndProgress = GetDlgItem(IDC_PROGRESS_TRANSFER);
	m_wndCancelButton = GetDlgItem(IDCANCEL);

	CPluginInfoPtr pPluginInfo = m_pPluginNode->GetPluginInfo();

	// Init dialog controls...
	CString strFileNameText;
	strFileNameText.Format((UINT) IDS_PLUGIN_DOWNLOAD_INIT_FILE_NAME_FORMAT, pPluginInfo->GetFileName());

	CString strContactText;
	strContactText.Format(IDS_FILE_TRANSFER_DLG_CONTACT_RECEIVE_FORMAT, pPluginInfo->GetDownloadUrl());

	m_wndFileName.SetWindowText(strFileNameText);
	m_wndContact.SetWindowText(strContactText);

	m_wndProgress.SetRange32(0, MIN_TOTAL_BYTES);
	m_wndProgress.SetPos(0);
	m_wndProgress.SetStep(1);

	m_wndCancelButton.SetWindowText(m_strCancelText);

	CenterWindow();

	m_pPluginNode->InstallPlugin(this);

	return 0L;
}


void CPluginDownloadDialog::OnClose()
{
	m_pPluginNode->CancelInstallation();

	EndDialog(IDCANCEL);
}


void CPluginDownloadDialog::OnDestroy()
{
	SetMsgHandled(FALSE);
}


void CPluginDownloadDialog::OnButtonCancel(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	CString strButtonText;
	m_wndCancelButton.GetWindowText(strButtonText);

	if (0 == strButtonText.Compare(m_strCancelText))
	{
		m_pPluginNode->CancelInstallation();
	}

	EndDialog(IDCANCEL);
}


void CPluginDownloadDialog::OnFinalMessage()
{
}