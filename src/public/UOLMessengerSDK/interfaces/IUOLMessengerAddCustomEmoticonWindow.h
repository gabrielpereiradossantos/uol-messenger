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


/*
#define			OPERATION_ID_ADD				1
#define			OPERATION_ID_DEL				2
#define			OPERATION_ID_CHANGE_SHORTCUT	4
#define			OPERATION_ID_CHANGE_FILENAME	8
#define			OPERATION_ID_ALL				15	// Must be the sum of all others.
*/


class EmoticonChangingInfo
{
public:
	EmoticonChangingInfo(CString strOldShortcut, CString strShortcut, 
			CString strFileName, int nComboOperationIds)
	{
		m_strOldShortcut = strOldShortcut;
		m_strShortcut = strShortcut;
		m_strFileName = strFileName;
		m_nComboOperationIds = nComboOperationIds;
	};
	
	virtual ~EmoticonChangingInfo()
	{
	};
	
public:
	
	CString		m_strOldShortcut;
	CString		m_strShortcut;
	CString		m_strFileName;
	int			m_nComboOperationIds;
};

MAKEAUTOPTR(EmoticonChangingInfo);



struct EmoticonFindParams
{
	CString strShortcut;
	int nChangingInfoOperations;
	BOOL bConsiderOldShortcut;
	BOOL bFound;
};


__interface IEmoticonChangingInfoFinder
{
	enum EmoticonChangingOperations
	{
		OPERATION_ID_ADD				= 1, 
		OPERATION_ID_DEL				= 2, 
		OPERATION_ID_CHANGE_SHORTCUT	= 4, 
		OPERATION_ID_CHANGE_FILENAME	= 8, 
		OPERATION_ID_ALL				= 15	// Must be the sum of all others.
	};
	
	enum EmoticonChangingConsultingMessage
	{
		WM_FIND_CHANGING_INFO = WM_USER + 0X554
	};
	
	EmoticonChangingInfoPtr FindChangingItem(CString strShortcut, 
			int cmbOperationIds, BOOL bConsiderOldShortcut);
};



class __declspec(novtable) IUOLMessengerAddCustomEmoticonWindow
{
public:
	
	enum OperationModeId
	{
		OPERATION_MODE_INVALID = 1,
		OPERATION_MODE_ADD_FROM_PREFERENCES,
		OPERATION_MODE_ADD_FROM_MESSAGE_WINDOW,
		OPERATION_MODE_CHANGE_SHORTCUT,
		OPERATION_MODE_CHANGE_FILEPATH
	};
	
public:
	virtual ~IUOLMessengerAddCustomEmoticonWindow() { }
	
	virtual INT_PTR ShowModal() = 0;
	virtual CString GetCustomEmoticonShortcut() = 0;
	virtual CString GetCustomEmoticonFileName() = 0;
};

MAKEAUTOPTR(IUOLMessengerAddCustomEmoticonWindow);

