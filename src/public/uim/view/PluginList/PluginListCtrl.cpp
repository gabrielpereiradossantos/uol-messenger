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
#include "PluginListCtrl.h"
#include "PluginNode.h"
#include "../../resource.h"
#include "../../controller/UIMApplication.h"
#include "../../controller/UIManager.h"


using namespace GroupListCtrl;


CPluginListCtrl::CPluginListCtrl() : 
	m_pGroupElement(NULL),
	m_pPluginElement(NULL)
{
	m_pPluginList = new CPluginList();
}


CPluginListCtrl::~CPluginListCtrl()
{
	if (m_pGroupElement)
	{
		m_pGroupElement.reset();
	}

	if (m_pPluginElement)
	{
		m_pPluginElement.reset();
	}

	if (m_pPluginList)
	{
		m_pPluginList.reset();
	}
}


void CPluginListCtrl::Init()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
	{
		IElementPtr pElement = m_listChilds.GetNext(pos);

		if (0 == pElement->GetName().CompareNoCase(_T("__PluginGroupElementInstance")))
		{
			m_pGroupElement = ap_dynamic_cast<CPluginGroupElementPtr>(pElement);
		}
		else if (0 == pElement->GetName().CompareNoCase(_T("__PluginEntryElementInstance")))
		{
			m_pPluginElement = ap_dynamic_cast<CPluginEntryElementPtr>(pElement);
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CPluginListCtrl::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CPluginListCtrl::AddPlugin(CPluginInfoPtr pPluginInfo)
{
	// Create Plugin Node
	CPluginNodePtr pPluginNode = new CPluginNode(pPluginInfo, m_pCheckOnImage,
		m_pCheckOffImage, m_pUpdateImage, m_pConfigImage, m_pInstallImage);
	CString strEntryName = pPluginNode->GetPluginInternalName();

	static_cast<CEntry*>(pPluginNode.get())->SetName(strEntryName);
	pPluginNode->SetDefaultPainter(GetPainter(IPainterRepository::PR_PLUGIN_DEFAULT_PAINTER));
	pPluginNode->SetSelectionPainter(GetPainter(IPainterRepository::PR_PLUGIN_SELECTION_PAINTER));
	pPluginNode->SetTextFont(m_pPluginElement->GetTextFont());
	pPluginNode->SetTextColor(m_pPluginElement->GetTextColor());
	pPluginNode->SetSelectedTextFont(m_pPluginElement->GetSelectedTextFont());
	pPluginNode->SetSelectedTextColor(m_pPluginElement->GetSelectedTextColor());
	pPluginNode->SetTitleTextFont(m_pPluginElement->GetTitleTextFont());
	pPluginNode->SetTitleTextColor(m_pPluginElement->GetTitleTextColor());
	
	static_cast<CEntry*>(pPluginNode.get())->SetHeight(GetEntryHeight());

	AddEntry(m_pAllEntriesGroup, pPluginNode);
	m_pPluginList->AddPlugin(pPluginNode);
}


void CPluginListCtrl::RemovePlugin(CPluginInfoPtr pPluginInfo)
{
	CString strEntryName = pPluginInfo->GetName();
	CPluginNodePtr pPluginNode = m_pPluginList->FindPlugin(strEntryName);

	if (pPluginNode)
	{
		RemoveEntry(strEntryName, m_pAllEntriesGroup->GetName());
		m_pPluginList->RemovePlugin(pPluginNode);
	}
}


void CPluginListCtrl::RemoveAllPlugins()
{
	RemoveAll(FALSE);
	m_pPluginList->RemoveAllPlugins();
}


CString CPluginListCtrl::GetName() const
{
	return "__PluginListInstance";
}


void CPluginListCtrl::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	try
	{
		CString strImage;
		GetParameterValue(strImage, pMapParams, "button_check_on");
		m_pCheckOnImage = UOLMessengerImage::CImageBuilder::LoadImage(strImage);
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "button_check_on");
	}

	try
	{
		CString strImage;
		GetParameterValue(strImage, pMapParams, "button_check_off");
		m_pCheckOffImage = UOLMessengerImage::CImageBuilder::LoadImage(strImage);
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "button_check_off");
	}

	try
	{
		CString strImage;
		GetParameterValue(strImage, pMapParams, "button_config");
		m_pConfigImage = UOLMessengerImage::CImageBuilder::LoadImage(strImage);
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "button_config");
	}

	try
	{
		CString strImage;
		GetParameterValue(strImage, pMapParams, "button_update");
		m_pUpdateImage = UOLMessengerImage::CImageBuilder::LoadImage(strImage);
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "button_update");
	}

	try
	{
		CString strImage;
		GetParameterValue(strImage, pMapParams, "button_install");
		m_pInstallImage = UOLMessengerImage::CImageBuilder::LoadImage(strImage);
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "button_install");
	}
}


void CPluginListCtrl::GetMinSize(CSize& size)
{
	CGroupListCtrl::GetSize(size);

	size.cy = GetEntryHeight();
}


void CPluginListCtrl::PaintBackground(CDCHandle& dc, const CRect& rect)
{
	__super::DrawBackground(dc, rect);
}


void CPluginListCtrl::OnRButtonDown(CGroupListCtrlItemPtr pItem, const CPoint& pt)
{
}


LRESULT CPluginListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	LRESULT lRet = CGroupListCtrl::OnCreate(lpCreateStruct);

	CString strAllEntriesGroupName = _T("__PluginGroup__");
	
	m_pAllEntriesGroup = new CPluginGroup();
	static_cast<CEntryGroup*>(m_pAllEntriesGroup.get())->SetName(strAllEntriesGroupName);
	static_cast<CEntryGroup*>(m_pAllEntriesGroup.get())->SetHeight(0);
	
	ATLASSERT(m_pGroupElement != NULL);
	
	AddGroup(m_pAllEntriesGroup, FALSE);

	return lRet;
}


void CPluginListCtrl::OnDestroy()
{
	RemoveAll(FALSE);
	m_pPluginList->RemoveAllPlugins();

	SetMsgHandled(FALSE);
}


IPainter* CPluginListCtrl::GetPainter(UINT nId)
{
	if ((nId == IPainterRepository::PR_GROUP_DEFAULT_PAINTER) ||
		(nId == IPainterRepository::PR_GROUP_SELECTION_PAINTER))
	{
		// Group painter
		if (m_pGroupElement != NULL)
		{
			return m_pGroupElement->GetPainter(nId);
		}
	}

	// Element painter
	if (m_pPluginElement != NULL)
	{
		return m_pPluginElement->GetPainter(nId);
	}

	return NULL;
}


int CPluginListCtrl::GetGroupHeight() const
{
	int nHeight = 0;

	if (m_pGroupElement != NULL)
	{
		nHeight = m_pGroupElement->GetHeight();
	}

	return nHeight;
}


int CPluginListCtrl::GetEntryHeight() const
{
	int nHeight = 0;

	if (m_pPluginElement != NULL)
	{
		nHeight = m_pPluginElement->GetHeight();
	}

	return nHeight;
}


CString CPluginListCtrl::FormatPluginNodeName(CPluginInfoPtr pPluginInfo) const
{
	CString strName;
	strName.Format("%s:%s", pPluginInfo->GetName(), 
		pPluginInfo->GetInternalName());

	return strName.Trim();
}


void CPluginListCtrl::EnablePlugins()
{
	CAtlList<CPluginNodePtr> listEntries;
	m_pPluginList->GetEntries(listEntries);

	for (POSITION pos = listEntries.GetHeadPosition(); pos;)
	{
		CPluginNodePtr pPluginNode = listEntries.GetNext(pos);

		if (pPluginNode)
		{
			pPluginNode->EnablePlugin();
		}
	}
}