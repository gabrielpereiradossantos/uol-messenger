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

#include "Element.h"
#include "../image/image.h"
#include "../Image/imageBuilder.h"

class CProtocolBagElement;
class CProtocolBagListElement;
class CProtocolContactStatusElement;
class CProtocolContactIconElement;
class CProtocolImageListElement;
class CProtocolUserIconElement;
class CProtocolUserStatusElement;
class CProtocolStatusButtonElement;
class CProtocolContactBlockedElement;
class CProtocolContactAuthorizationElement;

MAKEAUTOPTR(CProtocolBagElement);
MAKEAUTOPTR(CProtocolBagListElement);
MAKEAUTOPTR(CProtocolContactStatusElement);
MAKEAUTOPTR(CProtocolContactIconElement);
MAKEAUTOPTR(CProtocolImageListElement);
MAKEAUTOPTR(CProtocolUserIconElement);
MAKEAUTOPTR(CProtocolUserStatusElement);
MAKEAUTOPTR(CProtocolStatusButtonElement);
MAKEAUTOPTR(CProtocolContactBlockedElement);
MAKEAUTOPTR(CProtocolContactAuthorizationElement);

//////////////////////////////////////////////////////////////////////////
// CProtocolBagListElement
//////////////////////////////////////////////////////////////////////////


class CProtocolBagListElement : public CElement
{
public:
	DECLARE_ELEMENT_NAME("ProtocolBagList");
	CProtocolBagListElement();
	virtual ~CProtocolBagListElement();
	CProtocolBagElementPtr GetProtocolBagList(const CString &protocol);
protected:
	virtual CString GetName() const;
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	void OnElementBuilt();
private:
	CProtocolBagElementPtr m_pProtocolBagElement;
	CAtlList<CProtocolBagElementPtr> m_listProtocolBagElement;

};


//////////////////////////////////////////////////////////////////////////
// CProtocolBagElement
//////////////////////////////////////////////////////////////////////////

class CProtocolBagElement : public CElement
{
public:
	DECLARE_ELEMENT_NAME("ProtocolBag");
	CProtocolBagElement();
	CProtocolImageListElementPtr GetProtocolImageList();
	CString GetProtocolName();
	IUOLMessengerImagePtr GetProtocolIcon() {return m_pIcon;} 
	virtual ~CProtocolBagElement();
protected:
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	void OnElementBuilt();
private:
	CString m_strName;
	IUOLMessengerImagePtr	m_pIcon;
	CProtocolImageListElementPtr m_ProtocolImageListElement;
};

//////////////////////////////////////////////////////////////////////////
// CProtocolImageListElement
//////////////////////////////////////////////////////////////////////////


class CProtocolImageListElement : public CElement
{
public:
	DECLARE_ELEMENT_NAME("ProtocolImageList");
	CProtocolImageListElement();
	CProtocolUserIconElementPtr GetProtocolUserIconElement();
	CProtocolUserStatusElementPtr GetProtocolUserStatusElement();
	CProtocolContactIconElementPtr GetProtocolContactIconElement();
	CProtocolContactStatusElementPtr GetProtocolContactStatus();
	CProtocolStatusButtonElementPtr GetProtocolStatusButton();
	CProtocolContactBlockedElementPtr GetProtocolContactBlocked();
	CProtocolContactAuthorizationElementPtr GetProtocolContactAuthorizationElement();
	virtual ~CProtocolImageListElement();
protected:
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	void OnElementBuilt();
	CProtocolUserIconElementPtr m_pUserIconElement;
	CProtocolUserStatusElementPtr m_pUserStatusElement;
	CProtocolContactIconElementPtr m_pContactIconElement;
	CProtocolContactStatusElementPtr m_pContactStatusElement;
	CProtocolStatusButtonElementPtr m_pStatusButtonElement;
	CProtocolContactBlockedElementPtr m_pContactBlockedElement;
	CProtocolContactAuthorizationElementPtr m_pContactAuthorizationElement;
};



//////////////////////////////////////////////////////////////////////////
// CProtocolUserIconElement
//////////////////////////////////////////////////////////////////////////

class CProtocolUserIconElement : public CElement
{
public:
	DECLARE_ELEMENT_NAME("UserIcon");
	CProtocolUserIconElement();
	IUOLMessengerImagePtr GetUserIcon() {return m_pIcon;}
	CString GetUserIconPath() {return m_pathIcon;} 
	virtual ~CProtocolUserIconElement();
protected:
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	IUOLMessengerImagePtr	m_pIcon;
	CString m_pathIcon;
};

//////////////////////////////////////////////////////////////////////////
// CProtocolUserStatusElement
//////////////////////////////////////////////////////////////////////////
class CProtocolUserStatusElement : public CElement
{
public:
	DECLARE_ELEMENT_NAME("UserStatus");
	CProtocolUserStatusElement();
	virtual ~CProtocolUserStatusElement();
	IUOLMessengerImagePtr GetUserIconOnline() {return m_pOnline;}
	IUOLMessengerImagePtr GetUserIconOffline() {return m_pOffline;}
	IUOLMessengerImagePtr GetUserIconBusy() {return m_pBusy;}
	IUOLMessengerImagePtr GetUserIconAway() {return m_pAway;}
	IUOLMessengerImagePtr GetUserIconInvisible() {return m_pInvisible;}
    IUOLMessengerImagePtr GetUserIconUnknown() {return m_pUnknown;} 

protected:
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	IUOLMessengerImagePtr	m_pOnline;
	IUOLMessengerImagePtr	m_pOffline;
	IUOLMessengerImagePtr	m_pAway;
	IUOLMessengerImagePtr	m_pInvisible;
	IUOLMessengerImagePtr	m_pBusy;
    IUOLMessengerImagePtr	m_pUnknown;
};

//////////////////////////////////////////////////////////////////////////
// CProtocolContactIconElement
//////////////////////////////////////////////////////////////////////////
class CProtocolContactIconElement : public CElement
{
public:
	DECLARE_ELEMENT_NAME("ContactIcon");
	CProtocolContactIconElement();
	virtual ~CProtocolContactIconElement();
	IUOLMessengerImagePtr GetContactIcon() {return m_pIcon;}
	CString GetContactIconPath() {return m_pathIcon;} 

protected:
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	IUOLMessengerImagePtr	m_pIcon;
	CString m_pathIcon;

};

//////////////////////////////////////////////////////////////////////////
// CProtocolContactStatusElement
//////////////////////////////////////////////////////////////////////////
class CProtocolContactStatusElement : public CElement
{
public:
	DECLARE_ELEMENT_NAME("ContactStatus");
	CProtocolContactStatusElement();
	virtual ~CProtocolContactStatusElement();
	IUOLMessengerImagePtr GetUserIconOnline() {return m_pOnline;}
	IUOLMessengerImagePtr GetUserIconOffline() {return m_pOffline;}
	IUOLMessengerImagePtr GetUserIconBusy() {return m_pBusy;}
	IUOLMessengerImagePtr GetUserIconAway() {return m_pAway;}
	IUOLMessengerImagePtr GetUserIconIdle() {return m_pIdle;} 
    IUOLMessengerImagePtr GetUserIconUnknown() {return m_pUnknown;} 

protected:
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	IUOLMessengerImagePtr	m_pOnline;
	IUOLMessengerImagePtr	m_pOffline;
	IUOLMessengerImagePtr	m_pBusy;
	IUOLMessengerImagePtr	m_pAway;
	IUOLMessengerImagePtr	m_pIdle;
    IUOLMessengerImagePtr	m_pUnknown;
};

//////////////////////////////////////////////////////////////////////////
// CProtocolStatusButtonElement
//////////////////////////////////////////////////////////////////////////
class CProtocolStatusButtonElement : public CElement
{
public:
	DECLARE_ELEMENT_NAME("StatusButton");
	CProtocolStatusButtonElement();
	virtual ~CProtocolStatusButtonElement();
	IUOLMessengerImagePtr GetStatusButtonOnline() {return m_pOnline;}
	IUOLMessengerImagePtr GetStatusButtonOffline() {return m_pOffline;}
	IUOLMessengerImagePtr GetStatusButtonBusy() {return m_pBusy;}
	IUOLMessengerImagePtr GetStatusButtonAway() {return m_pAway;}
    IUOLMessengerImagePtr GetStatusButtonInvisible() {return m_pInvisible;} 
    IUOLMessengerImagePtr GetStatusButtonUnknown() {return m_pUnknown;} 

protected:
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	IUOLMessengerImagePtr	m_pOnline;
	IUOLMessengerImagePtr	m_pOffline;
	IUOLMessengerImagePtr	m_pBusy;
	IUOLMessengerImagePtr	m_pAway;
    IUOLMessengerImagePtr	m_pInvisible;
    IUOLMessengerImagePtr	m_pUnknown;
};

//////////////////////////////////////////////////////////////////////////
// CProtocolContactBlockedElement
//////////////////////////////////////////////////////////////////////////
class CProtocolContactBlockedElement : public CElement
{
public:
	DECLARE_ELEMENT_NAME("ContactBlocked");
	CProtocolContactBlockedElement();
	virtual ~CProtocolContactBlockedElement();
	IUOLMessengerImagePtr GetIconBlocked() {return m_pIcon;}

protected:
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	IUOLMessengerImagePtr	m_pIcon;
};


//////////////////////////////////////////////////////////////////////////
// CProtocolContactAuthorizationElement
//////////////////////////////////////////////////////////////////////////
class CProtocolContactAuthorizationElement : public CElement
{
public:
	DECLARE_ELEMENT_NAME("ContactAuthorization");
	
	CProtocolContactAuthorizationElement();
	virtual ~CProtocolContactAuthorizationElement();

	IUOLMessengerImagePtr GetNotAuthorizedIcon() {return m_pNotAuthorizedIcon;}

protected:
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	IUOLMessengerImagePtr	m_pNotAuthorizedIcon;
};
