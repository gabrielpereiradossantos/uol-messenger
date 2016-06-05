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
#include "ElementBuilder.h"
#include "COLORREF.h"
#include "../image/image.h"
#include "../image/imagebuilder.h"

class CMainDialogBuilder : public CElementBuilder
{
};
MAKEAUTOPTR(CMainDialogBuilder);


class CMessageBoxBuilder : public CMainDialogBuilder
{
public:
	virtual ~CMessageBoxBuilder() {}

	DECLARE_ELEMENT_NAME("MessageBox");
	virtual CElementPtr AllocElement();

	virtual CString GetName() const
	{
		return "__MessageBoxBuilderInstance";
	}
};
MAKEAUTOPTR(CMessageBoxBuilder);


class CAddCustomEmoticonDialogBuilder : public CMainDialogBuilder
{
public:
	virtual ~CAddCustomEmoticonDialogBuilder() {}

	DECLARE_ELEMENT_NAME("AddCustomEmoticonDialog");
	virtual CElementPtr AllocElement();

	virtual CString GetName() const
	{
		return "__AddCustomEmoticonDialogBuilderInstance";
	}
};
MAKEAUTOPTR(CAddCustomEmoticonDialogBuilder);


class CChatInviteDialogBuilder : public CMainDialogBuilder
{
public:
	virtual ~CChatInviteDialogBuilder() {}

	DECLARE_ELEMENT_NAME("ChatInviteDialog");
	virtual CElementPtr AllocElement();

	virtual CString GetName() const
	{
		return "__ChatInviteDialogBuilderInstance";
	}
};
MAKEAUTOPTR(CChatInviteDialogBuilder);


class CContactDialogBuilder : public CMainDialogBuilder
{
public:
	virtual ~CContactDialogBuilder() {}

	DECLARE_ELEMENT_NAME("ContactDialog");
	virtual CElementPtr AllocElement();

	virtual CString GetName() const
	{
		return "__ContactDialogBuilderInstance";
	}
};
MAKEAUTOPTR(CContactDialogBuilder);


class CInputDialogBuilder : public CMainDialogBuilder
{
public:
	virtual ~CInputDialogBuilder() {}

	DECLARE_ELEMENT_NAME("InputDialog");
	virtual CElementPtr AllocElement();

	virtual CString GetName() const
	{
		return "__InputDialogBuilderInstance";
	}
};
MAKEAUTOPTR(CInputDialogBuilder);


class CPrivacyDialogBuilder : public CMainDialogBuilder
{
public:
	virtual ~CPrivacyDialogBuilder() {}

	DECLARE_ELEMENT_NAME("PrivacyDialog");
	virtual CElementPtr AllocElement();

	virtual CString GetName() const
	{
		return "__PrivacyDialogBuilderInstance";
	}
};
MAKEAUTOPTR(CPrivacyDialogBuilder);


class CMessageReceivingDialogBuilder : public CMainDialogBuilder
{
public:
	virtual ~CMessageReceivingDialogBuilder() {}

	DECLARE_ELEMENT_NAME("MessageReceivingDialog");
	virtual CElementPtr AllocElement();

	virtual CString GetName() const
	{
		return "__MessageReceivingDialogBuilderInstance";
	}
};
MAKEAUTOPTR(CMessageReceivingDialogBuilder);


class CAuthorizeDialogBuilder : public CMainDialogBuilder
{
public:
	virtual ~CAuthorizeDialogBuilder() {}

	DECLARE_ELEMENT_NAME("AuthorizeDialog");
	virtual CElementPtr AllocElement();

	virtual CString GetName() const
	{
		return "__AuthorizeDialogBuilderInstance";
	}
};
MAKEAUTOPTR(CAuthorizeDialogBuilder);


class CConfirmDlgBuilder : public CMainDialogBuilder
{
public:
	virtual ~CConfirmDlgBuilder() {}

	DECLARE_ELEMENT_NAME("ConfirmDialog");
	virtual CElementPtr AllocElement();

	virtual CString GetName() const
	{
		return "__ConfirmDialogBuilderInstance";
	}
};
MAKEAUTOPTR(CConfirmDlgBuilder);


class CPasswordChangeDlgBuilder : public CMainDialogBuilder
{
public:
	virtual ~CPasswordChangeDlgBuilder() {}

	DECLARE_ELEMENT_NAME("PasswordChangeDlg");
	virtual CElementPtr AllocElement();

	virtual CString GetName() const
	{
		return "__PasswordChangeDlgBuilderInstance";
	}
};
MAKEAUTOPTR(CPasswordChangeDlgBuilder);


class CAlertDialogBuilder : public CMainDialogBuilder
{
public:
	virtual ~CAlertDialogBuilder() {}

	DECLARE_ELEMENT_NAME("AlertDialog");
	virtual CElementPtr AllocElement();

	virtual CString GetName() const
	{
		return "__AlertDialogBuilderInstance";
	}
};
MAKEAUTOPTR(CAlertDialogBuilder);


class CAboutDlgBuilder : public CMainDialogBuilder
{
public:
	virtual ~CAboutDlgBuilder() {}

	DECLARE_ELEMENT_NAME("AboutDlg");
	virtual CElementPtr AllocElement();

	virtual CString GetName() const
	{
		return "__AboutDlgBuilderInstance";
	}
};
MAKEAUTOPTR(CAboutDlgBuilder);


class CHotKeysDlgBuilder : public CMainDialogBuilder
{
public:
	virtual ~CHotKeysDlgBuilder() {}

	DECLARE_ELEMENT_NAME("HotKeysDlg");
	virtual CElementPtr AllocElement();

	virtual CString GetName() const
	{
		return "__HotKeysDlgBuilderInstance";
	}
};
MAKEAUTOPTR(CHotKeysDlgBuilder);