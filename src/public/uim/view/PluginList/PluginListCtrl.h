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

#include "PluginGroup.h"

#include "../skin/WindowElement.h"
#include "../GroupListCtrl/GroupListCtrlItem.h"
#include "../GroupListCtrl/GroupListCtrl.h"
#include "../IPainterRepository.h"
#include "../skin/PluginGroupElement.h"
#include "../skin/PluginEntryElement.h"
#include "../ImageButton.h"
#include "../image/imagebuilder.h"
#include "../../model/PluginInfo.h"
#include "../../controller/UOLMessengerPluginManager.h"
#include "../../controller/PluginList.h"


class CPluginListCtrl : 
	public CWindowElement<CPluginListCtrl, GroupListCtrl::CGroupListCtrl>,
	public IPainterRepository
{
public:
	CPluginListCtrl();
	virtual ~CPluginListCtrl();

	DECLARE_ELEMENT_NAME("PluginList");

	void Init();
	void Finalize();

	void AddPlugin(CPluginInfoPtr pPluginInfo);
	void RemovePlugin(CPluginInfoPtr pPluginInfo);
	void RemoveAllPlugins();
	void EnablePlugins();

	virtual void GetMinSize(CSize& size);

protected:
	virtual CString GetName() const;
	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);

	virtual void PaintBackground(CDCHandle& dc, const CRect& rect);
	virtual void OnRButtonDown(GroupListCtrl::CGroupListCtrlItemPtr pItem, const CPoint& pt);

	BEGIN_MSG_MAP(CPluginListCtrl)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		CHAIN_MSG_MAP(GroupListCtrl::CGroupListCtrl)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();

	// IPainterRepository interface
	virtual IPainter* GetPainter(UINT nId);

private:
	int GetGroupHeight() const;
	int GetEntryHeight() const;

	CString FormatPluginNodeName(CPluginInfoPtr pPluginInfo) const;

private:
	CPluginListPtr				m_pPluginList;
	CPluginGroupPtr				m_pAllEntriesGroup;
	CPluginGroupElementPtr		m_pGroupElement;
	CPluginEntryElementPtr		m_pPluginElement;
	IUOLMessengerImagePtr		m_pCheckOnImage;
	IUOLMessengerImagePtr		m_pCheckOffImage;
	IUOLMessengerImagePtr		m_pUpdateImage;
	IUOLMessengerImagePtr		m_pConfigImage;
	IUOLMessengerImagePtr		m_pInstallImage;
	CComAutoCriticalSection		m_csGroups;
};

MAKEAUTOPTR(CPluginListCtrl);
