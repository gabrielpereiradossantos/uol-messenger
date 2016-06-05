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

#include "thread.h"
#include "utils.h"
#include "UIMApplication.h"

#include "../../../ctrlcenter/ctrlcenter/_ctrlcenter.h"


class CBugReportThread : public CThread
{
public:
	CBugReportThread() : 
			m_hwnd(NULL),
			m_bIsCrashReport(FALSE)
	{
	}


	void SetHwnd(HWND hwnd)
	{
		m_hwnd = hwnd;
	}


	void SetIsCrashReport(BOOL bIsCrashReport)
	{
		m_bIsCrashReport = bIsCrashReport;
	}


	void AddReportInfoList(CAtlList<COpenBugReportCommand::ReportInfoPtr>& reportInfoList)
	{
		m_reportInfoList.AddTailList(&reportInfoList);
	}
	
	
	virtual void Run()
	{
		::CoInitialize(NULL);
		
		HRESULT hr;
		UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
		try
		{
			hr = m_pBugReport.CoCreateInstance(__uuidof(CBugReport));
			
			if (SUCCEEDED(hr))
			{
				m_pBugReport->Init(m_hwnd);
				
				CComPtr<IReportList> pReportList;
				hr = m_pBugReport->QueryInterface(__uuidof(IReportList), (void**) &pReportList);
				
				if (SUCCEEDED(hr))
				{
					CComPtr<IReport> pReport;
					
					COpenBugReportCommand::ReportInfoPtr pReportInfo;
					
					for (POSITION listPos = m_reportInfoList.GetHeadPosition(); listPos; )
					{
						pReportInfo = m_reportInfoList.GetNext(listPos);
						
						hr = pReport.CoCreateInstance(__uuidof(CReport));
						
						if (SUCCEEDED(hr))
						{
							pReport->SetName(pReportInfo->strReportInfoTitle.AllocSysString());
							pReport->SetValue(pReportInfo->strReportInfoData.AllocSysString());
							
							pReportList->Add(pReport);
							
							pReport.Release();
						}
					}
					
					//hr = pReport.CoCreateInstance(__uuidof(CReport));
					
					//if (SUCCEEDED(hr))
					//{
					//	CUOLBarReport uolBarReport;
					//	uolBarReport.BuildReport();
					
					//	CString strName("UOLBar Registry");
					//	CString strValue = uolBarReport.GetText();
					
					//	pReport->SetName(strName.AllocSysString());
					//	pReport->SetValue(strValue.AllocSysString());
					//	pReportList->Add(pReport);
					
					//	pReport.Release();
					//}
					
					hr = pReport.CoCreateInstance(__uuidof(CReport));
					
					if (SUCCEEDED(hr))
					{
						// Add ProdID Info
						CString strName;
						CString strValue;
						
						strName.LoadString(IDS_PRODID_NAME);
						strValue.LoadString(IDS_PRODID_VALUE);
						
						pReport->SetName(strName.AllocSysString());
						pReport->SetValue(strValue.AllocSysString());
						pReportList->Add(pReport);
						
						pReport.Release();
					}
				}
				
				pReportList.Release();
				
				CString strTitle, strEmail, strCookieName, strCookieValue, strMethod, strUrl;
				
				if (pStringLoader != NULL)
				{
					strTitle = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");
				}
				else
				{
					// This should not be used, but will be necessary whether 
					// we have a crash on the very initial startup of 
					// application.
					//
					strTitle.LoadString(IDS_APP_NAME);
				}
				
//				strEmail = CUOLBandController::GetInstance()->GetUserOptions()->User;
				
				//if (CUserOptions::MEMBER == CUOLBandController::GetInstance()->GetAuthenticatedUserType())
				//{
				//	CString strDomain;
				//	strDomain.LoadString(IDS_MEMBER_EMAIL_DOMAIN);
				//	strEmail += "@";
				//	strEmail += strDomain;
				//}
				
//				CString strCookie = CUOLBandController::GetInstance()->GetAuthCookie();
				
				//int nPos = strCookie.Find("=");
				
				//if (-1 != nPos)
				//{
				//	strCookieName = strCookie.Mid(0,nPos);
				//	strCookieValue = strCookie.Mid(nPos+1);
				//}
				
				strMethod.LoadString(IDS_BUGREPORT_SERVLET_METHOD);
				strUrl.LoadString(IDS_BUGREPORT_SERVLET_URL);
				
				m_pBugReport->SetTitle(strTitle.AllocSysString());
				m_pBugReport->SetEmail(strEmail.AllocSysString());
				m_pBugReport->SetCookie(strCookieName.AllocSysString(), strCookieValue.AllocSysString());
				m_pBugReport->SetMethod(strMethod.AllocSysString());
				m_pBugReport->SetUrl(strUrl.AllocSysString());
				m_pBugReport->SetUserAgent(GetIEUserAgent().AllocSysString());
				
				CComQIPtr<IBugReport3> pBugReport3 = m_pBugReport;
				if (pBugReport3 != NULL)
				{
					if (m_bIsCrashReport)
					{
						pBugReport3->SetReportType(ReportType::TYPE_CRASH_REPORT);
					}
					else
					{
						pBugReport3->SetReportType(ReportType::TYPE_BUG_REPORT);
					}
				}
				else
				{
					// User doesn't have BugReport3 interface, try use BugReport2 instead
					CComQIPtr<IBugReport2> pBugReport2 = m_pBugReport;
					if (pBugReport2 != NULL)
					{
						pBugReport2->SetIsCrashReport(m_bIsCrashReport);
					}
				}
				
				m_pBugReport->ShowDialog();
				
				{
					CComCritSecLock<CComAutoCriticalSection> lock(m_csReportDialog);
					
					m_pBugReport.Release();
				}
			}
		}
		catch (const _com_error& e) 
		{
			MSGASSERT(e.ErrorMessage());
			hr = e.Error();
		}
		
		::CoUninitialize();
	}

	void Stop()
	{
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_csReportDialog);
			
			if (m_pBugReport)
			{
				CComQIPtr<IBugReport3> pBugReport3 = m_pBugReport;

				if (pBugReport3 != NULL)
				{
					pBugReport3->CloseDialog();
				}
			}
		}
		
		Wait();
	}
	
private:
	HWND	m_hwnd;
	BOOL	m_bIsCrashReport;
	
	CAtlList<COpenBugReportCommand::ReportInfoPtr>		m_reportInfoList;
	
	CComPtr<IBugReport>		m_pBugReport;
	CComAutoCriticalSection	m_csReportDialog;
};