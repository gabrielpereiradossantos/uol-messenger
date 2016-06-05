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
#include "ConversationWindowSplitter.h"

#include "../controller/UIMApplication.h"

#include <interfaces/IUOLMessengerMessageSettings.h>

#define MINIMUM_HORIZONTAL_POS 50
#define MINIMUM_VERTICAL_POS 150

/////////////////////////////////////////////////
// CConversationWindowHorizontalSplitter Class
CConversationWindowHorizontalSplitter::CConversationWindowHorizontalSplitter() :
m_nSplitterBarHeight(0),
m_bSplitterPositioned(FALSE),
m_nBottomPanelHeight(-1)
{
}


CConversationWindowHorizontalSplitter::~CConversationWindowHorizontalSplitter()
{
}



CString CConversationWindowHorizontalSplitter::GetName() const
{
	return "__ConversationWindowHorizontalSplitterInstance_" + m_strGUID;
}


void CConversationWindowHorizontalSplitter::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	try
	{
		GetParameterValue(m_nSplitterBarHeight, pMapParams, "SplitterHeight");
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "SplitterHeight");
		m_nSplitterBarHeight = 1;
	}

	try
	{
		CString strFileName;
		GetParameterValue(strFileName, pMapParams, "CenterImage");
		m_pCenterImage = UOLMessengerImage::CImageBuilder::LoadImage(strFileName);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "CenterImage");
	}	
}


void CConversationWindowHorizontalSplitter::DoPaint(CDCHandle& dc)
{
	if (m_nSinglePane == SPLIT_PANE_NONE && m_xySplitterPos == -1)
	{
		SetSplitterPos();
	}

	DrawSplitterBar(dc);
}


void CConversationWindowHorizontalSplitter::UpdateSplitterPosition()
{
	if (IsWindow())
	{
		CRect rect;
		GetSplitterRect(&rect);

		// Update splitter bar position
		if (!m_bSplitterPositioned)
		{
			if (rect.Height() > 0)
			{
				m_bSplitterPositioned = TRUE;

				IUOLMessengerMessageSettings3Ptr pMessageSettings3;
				pMessageSettings3 = ap_dynamic_cast<IUOLMessengerMessageSettings3Ptr>(CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings());
				ATLASSERT(pMessageSettings3 != NULL);

				int nPos = pMessageSettings3->GetConversationWindowHorizontalSplitterPosition();

				if (nPos <= MINIMUM_HORIZONTAL_POS)
				{
					nPos = GetDefaultSplitterPosition();
				}

				SetSplitterPos(nPos);
			}
		}	
	}
}


void CConversationWindowHorizontalSplitter::UpdateLayout()
{
	SetSplitterRect();
}


LRESULT CConversationWindowHorizontalSplitter::OnCreate(LPCREATESTRUCT /*lpcs*/)
{
	m_cxySplitBar = m_nSplitterBarHeight;
	m_cxyBarEdge = 0;
	m_cxyMin = MINIMUM_HORIZONTAL_POS;

	m_nProportionalPos = 0;
	m_bUpdateProportionalPos = FALSE;

	return 0L;
}


void CConversationWindowHorizontalSplitter::OnDestroy()
{
	DWORD dwSplitterPos = GetSplitterPos();

	int nMaxPos = m_rcSplitter.bottom - m_cxySplitBar - m_cxyBarEdge;

	if ((dwSplitterPos >= MINIMUM_HORIZONTAL_POS) && (dwSplitterPos < nMaxPos))
	{
		dwSplitterPos = dwSplitterPos;

		IUOLMessengerMessageSettings3Ptr pMessageSettings3;
		pMessageSettings3 = ap_dynamic_cast<IUOLMessengerMessageSettings3Ptr>(CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings());
		ATLASSERT(pMessageSettings3 != NULL);

		pMessageSettings3->SetConversationWindowHorizontalSplitterPosition(dwSplitterPos);
	}
}


void CConversationWindowHorizontalSplitter::OnLButtonDblClick(UINT nFlags, CPoint& ptClick)
{
	DWORD dwSplitterPos = GetSplitterPos();

	ATLTRACE(_T("%s - Splitter position=%d\n"), __FUNCTION__, dwSplitterPos);

	int nMaxPos = m_rcSplitter.bottom - m_cxySplitBar - m_cxyBarEdge;

	if (0 == (nMaxPos - dwSplitterPos))
	{
		if (m_nBottomPanelHeight > -1)
		{
			ATLTRACE(_T("Restore splitter position\n"));

			SetSplitterPos(nMaxPos - m_nBottomPanelHeight);
		}
		else
		{
			ATLTRACE(_T("Default splitter position\n"));

			SetSplitterPos(GetDefaultSplitterPosition());
		}
	}
	else
	{
		ATLTRACE(_T("Save splitter position\n"));

		m_nBottomPanelHeight = nMaxPos - dwSplitterPos;

		SetSplitterPos(nMaxPos);
	}
}


void CConversationWindowHorizontalSplitter::DrawSplitterBar(CDCHandle& dc)
{
	CRect rect;
	if (GetSplitterBarRect(&rect))
	{
		if ((m_pActiveBackground != NULL) && (m_pCenterImage != NULL))
		{
			m_pActiveBackground->Draw(dc, rect);

			int x = (rect.Width() - m_pCenterImage->GetWidth()) / 2;
			int cx = m_pCenterImage->GetWidth();
			int y = rect.top;
			int cy = m_pCenterImage->GetHeight();
			
			IUOLMessengerImageDrawStrategyPtr pStrategy = new UOLMessengerImage::CDrawOpaqueStrategy();

			m_pCenterImage->Draw(dc, x, y, cx, cy, pStrategy);
		}
		else
		{
			__super::DrawSplitterBar(dc);
		}
	}
}


int CConversationWindowHorizontalSplitter::GetDefaultSplitterPosition()
{
	CRect rect;
	GetClientRect(&rect);

	int nPos = rect.Height() * 0.25;

	return nPos;
}


/////////////////////////////////////////////////
// CConversationWindowVerticalSplitter Class
CConversationWindowVerticalSplitter::CConversationWindowVerticalSplitter() :
m_nSplitterBarHeight(0),
m_bSplitterPositioned(FALSE),
m_nRightPanelHeight(-1)
{
}


CConversationWindowVerticalSplitter::~CConversationWindowVerticalSplitter()
{
}



CString CConversationWindowVerticalSplitter::GetName() const
{
	return "__ConversationWindowVerticalSplitterInstance_" + m_strGUID;
}


void CConversationWindowVerticalSplitter::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	try
	{
		GetParameterValue(m_nSplitterBarHeight, pMapParams, "SplitterHeight");
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "SplitterHeight");
		m_nSplitterBarHeight = 1;
	}

	try
	{
		CString strFileName;
		GetParameterValue(strFileName, pMapParams, "CenterImage");
		m_pCenterImage = UOLMessengerImage::CImageBuilder::LoadImage(strFileName);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "CenterImage");
	}	
}


void CConversationWindowVerticalSplitter::DoPaint(CDCHandle& dc)
{
	if (m_nSinglePane == SPLIT_PANE_NONE && m_xySplitterPos == -1)
	{
		SetSplitterPos();
	}

	DrawSplitterBar(dc);
}


void CConversationWindowVerticalSplitter::UpdateSplitterPosition()
{
	if (IsWindow())
	{
		CRect rect;
		GetSplitterRect(&rect);

		// Update splitter bar position
		if (!m_bSplitterPositioned)
		{
			if (rect.Height() > 0)
			{
				m_bSplitterPositioned = TRUE;

				IUOLMessengerMessageSettings3Ptr pMessageSettings3;
				pMessageSettings3 = ap_dynamic_cast<IUOLMessengerMessageSettings3Ptr>(CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings());
				ATLASSERT(pMessageSettings3 != NULL);

				int nPos = pMessageSettings3->GetConversationWindowVerticalSplitterPosition();

				if (nPos <= MINIMUM_VERTICAL_POS)
				{
					nPos = GetDefaultSplitterPosition();
				}

				SetSplitterPos(nPos);
			}
		}
	}
}


void CConversationWindowVerticalSplitter::UpdateLayout()
{
	SetSplitterRect();
}


LRESULT CConversationWindowVerticalSplitter::OnCreate(LPCREATESTRUCT /*lpcs*/)
{
	m_cxySplitBar = m_nSplitterBarHeight;
	m_cxyBarEdge = 0;
	m_cxyMin = MINIMUM_VERTICAL_POS;

	m_nProportionalPos = 0;
	m_bUpdateProportionalPos = FALSE;

	return 0L;
}


void CConversationWindowVerticalSplitter::OnDestroy()
{
	DWORD dwSplitterPos = GetSplitterPos();

	int nMaxPos = m_rcSplitter.right - m_cxySplitBar - m_cxyBarEdge;

	if ((dwSplitterPos >= MINIMUM_VERTICAL_POS) && (dwSplitterPos < nMaxPos))
	{
		IUOLMessengerMessageSettings3Ptr pMessageSettings3;
		pMessageSettings3 = ap_dynamic_cast<IUOLMessengerMessageSettings3Ptr>(CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings());
		ATLASSERT(pMessageSettings3 != NULL);

		pMessageSettings3->SetConversationWindowVerticalSplitterPosition(dwSplitterPos);
	}
}


void CConversationWindowVerticalSplitter::OnLButtonDblClick(UINT nFlags, CPoint& ptClick)
{
	DWORD dwSplitterPos = GetSplitterPos();

	ATLTRACE(_T("%s - Splitter position=%d\n"), __FUNCTION__, dwSplitterPos);

	int nMaxPos = m_rcSplitter.right - m_cxySplitBar - m_cxyBarEdge;

	if (0 == (nMaxPos - dwSplitterPos))
	{
		if (m_nRightPanelHeight > -1)
		{
			ATLTRACE(_T("Restore splitter position\n"));

			SetSplitterPos(nMaxPos - m_nRightPanelHeight);
		}
		else
		{
			ATLTRACE(_T("Default splitter position\n"));

			SetSplitterPos(GetDefaultSplitterPosition());
		}
	}
	else
	{
		ATLTRACE(_T("Save splitter position\n"));

		m_nRightPanelHeight = nMaxPos - dwSplitterPos;

		SetSplitterPos(nMaxPos);
	}
}


void CConversationWindowVerticalSplitter::DrawSplitterBar(CDCHandle& dc)
{
	CRect rect;
	if (GetSplitterBarRect(&rect))
	{
		if ((m_pActiveBackground != NULL) && (m_pCenterImage != NULL))
		{
			m_pActiveBackground->Draw(dc, rect);

			int x = rect.left;
			int cx = m_pCenterImage->GetWidth();
			int y = (rect.Height() - m_pCenterImage->GetHeight()) / 2;
			int cy = m_pCenterImage->GetHeight();
			
			IUOLMessengerImageDrawStrategyPtr pStrategy = new UOLMessengerImage::CDrawOpaqueStrategy();

			m_pCenterImage->Draw(dc, x, y, cx, cy, pStrategy);
		}
		else
		{
			__super::DrawSplitterBar(dc);
		}
	}
}


int CConversationWindowVerticalSplitter::GetDefaultSplitterPosition()
{
	CRect rect;
	GetClientRect(&rect);

	int nPos = rect.Width() * 0.7;

	return nPos;
}
