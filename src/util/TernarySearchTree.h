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

#include "TernarySearchTreeVisitor.h"

// Ternary search tree class
class CTernarySearchTree
{
private:
	// Ternary search tree node class
	class CTreeNode
	{
	public:
		CTreeNode(_TUCHAR nDigit)
		{
			digit = nDigit;
			data = NULL;
			l = NULL;
			m = NULL;
			r = NULL;
		}

		~CTreeNode(void)
		{
			if (l != NULL) delete l;
			if (m != NULL) delete m;
			if (r != NULL) delete r;
		}

	public:
		enum
		{ 
			NULL_DIGIT		= 0,	// '\0' terminator
			WILDCARD_DIGIT	= 42	// '*' terminator
		};

		_TUCHAR		digit;	// digit
		LPVOID		data;
		CTreeNode*	l;		// left sub-tree
		CTreeNode*	m;		// middle sub-tree
		CTreeNode*	r;		// right sub-tree		
	};

public:
	// Constructor
	CTernarySearchTree(void);

	// Destructor
	virtual ~CTernarySearchTree(void);
	
	// Insert a new string if it does not exists
	BOOL Insert(LPCTSTR szItem, LPVOID lpVoid = NULL);
	
	// Check if a specified string match one on the tree
	LPVOID Match(LPCTSTR szItem);

	// Call function Visit for every string on the tree according to strings order
	// NOTE: In this structure, '*' comes before all other characteres, including
	//       '\0' and those on the interval ('\0', '*')
	void SortStrings(CTernarySearchTreeVisitor& visitor);

	// Remove a string from the tree
	BOOL Remove(LPCTSTR szItem);

	// Remove all strings from the tree
	BOOL RemoveAll(void);

	// Get number of strings on the tree
	ULONG GetSize();

private:
	BOOL InsertR(CTreeNode* &h, LPCTSTR szItem, LPVOID lpVoid);
	
	LPVOID MatchR(CTreeNode* h, LPCTSTR szItem);

	void SortStringsR(CTreeNode* h, CString& strResult, INT nInd, CTernarySearchTreeVisitor& visitor);

	BOOL RemoveR(CTreeNode* &h, LPCTSTR szItem);

	void ReplaceRootNode(CTreeNode* &h);

private:
	CTreeNode*	m_pHead;	// tree root
	ULONG		m_lSize;	// inserted string counter
};
