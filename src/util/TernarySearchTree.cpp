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
 

#include "StdAfx.h"

#include "TernarySearchTreeVisitor.h"
#include "TernarySearchtree.h"


CTernarySearchTree::CTernarySearchTree(void) : m_lSize(0)
{
	m_pHead = NULL;
}


CTernarySearchTree::~CTernarySearchTree(void)
{
	if (m_pHead != NULL)
	{
		delete m_pHead;
		m_lSize = 0;
	}
}


BOOL CTernarySearchTree::Insert(LPCTSTR szItem, LPVOID lpVoid)
{
	if (szItem != NULL)
	{
		BOOL bRet = InsertR(m_pHead, szItem, lpVoid);
		if (bRet)
		{
			// Adjust number of strings
			++m_lSize;
		}

		return bRet;
	}

	return FALSE;
}


LPVOID CTernarySearchTree::Match(LPCTSTR szItem)
{
	LPVOID lpVoid = MatchR(m_pHead, szItem);

	return lpVoid;
}


void CTernarySearchTree::SortStrings(CTernarySearchTreeVisitor& visitor)
{
	CString	strItem;
	SortStringsR(m_pHead, strItem, 0, visitor);
}


BOOL CTernarySearchTree::Remove(LPCTSTR szItem)
{
	if (szItem != NULL)
	{
		BOOL bRet = RemoveR(m_pHead, szItem);
		if (bRet)
		{
			// Adjust number of strings
			--m_lSize;
		}

		return bRet;
	}

	return FALSE;
}


BOOL CTernarySearchTree::RemoveAll(void)
{
	if (m_pHead != NULL)
	{
		delete m_pHead;
		m_pHead = NULL;
		m_lSize = 0;

		return TRUE;
	}

	return FALSE;
}


ULONG CTernarySearchTree::GetSize()
{
	return m_lSize;
}

BOOL CTernarySearchTree::InsertR(CTreeNode* &h, LPCTSTR szItem, LPVOID lpVoid)
{
	BOOL bNewString = FALSE;
	_TUCHAR nItemDigit = *szItem;

	if (NULL == h)
	{
		h = new CTreeNode(nItemDigit);

		bNewString = TRUE;
	}
	else
	{
		// Handle special cases:
		// a) new string is prefix of another existing string and
		//    it is not on the tree
		if ((CTreeNode::NULL_DIGIT == nItemDigit) &&
			(CTreeNode::NULL_DIGIT != h->digit))
		{
			// If h->digit is wildcard, insert into h right sub-tree
			if (CTreeNode::WILDCARD_DIGIT == h->digit)
			{
				return InsertR(h->r, szItem, lpVoid);
			}
			// Else, insert into h left sub-tree
			else
			{
				return InsertR(h->l, szItem, lpVoid);
			}
		}

		// b) new string is prefix of another existing string except for the
		//    ending wildcard, and it is not on tree
		else if ((CTreeNode::WILDCARD_DIGIT == nItemDigit) &&
				 (CTreeNode::WILDCARD_DIGIT != h->digit))
		{
			// New wildcard node becomes h root (and h becomes its right sub-tree)
			// for faster string matching
			CTreeNode* pNode = new CTreeNode(nItemDigit);
			pNode->r = h;
			h = pNode;

			bNewString = TRUE;

			return InsertR(pNode->m, (szItem + 1), lpVoid);
		}
	}
	
	if (nItemDigit == CTreeNode::WILDCARD_DIGIT)
	{
		h->data = lpVoid;
	}

	// Reached string end
	if (CTreeNode::NULL_DIGIT == nItemDigit)
	{
		h->data = lpVoid;

		return bNewString;
	}	

	// Go left, middle or right depending on h->digit and nItemDigit
	// Only in case that h->digit is wildcard, don't let insert on its
	// left sub-tree
	if ((h->digit > nItemDigit) &&
		(h->digit != CTreeNode::WILDCARD_DIGIT))
	{
		return InsertR(h->l, szItem, lpVoid);
	}

	else if (h->digit == nItemDigit)
	{
		return InsertR(h->m, (szItem + 1), lpVoid);
	}

	// (h->digit < nItemDigit)
	else
	{
		return InsertR(h->r, szItem, lpVoid);
	}
}


LPVOID CTernarySearchTree::MatchR(CTreeNode* h, LPCTSTR szItem)
{
	_TUCHAR nItemDigit = (_TUCHAR) *szItem;

	if (NULL == h)
	{
		return NULL;
	}

	// Found '*', don't need to go deeper
	if (CTreeNode::WILDCARD_DIGIT == h->digit)
	{
		return h->data;
	}

	// If reached matching string end, check tree node digit
	if (CTreeNode::NULL_DIGIT == nItemDigit)
	{
		if (CTreeNode::NULL_DIGIT == h->digit)
		{
			return h->data;
		}
		else
		{
			// go left because h->digit is greater than NULL_DIGIT
			return MatchR(h->l, szItem);
		}
	}

	// Go left, middle or right depending on h->digit and nItemDigit
	if (h->digit > nItemDigit)
	{
		return MatchR(h->l, szItem);
	}

	else if (h->digit == nItemDigit)
	{
		return MatchR(h->m, (szItem+1));
	}

	// (h->digit < nItemDigit)
	else
	{
		return MatchR(h->r, szItem);
	}

	return NULL;
}


void CTernarySearchTree::SortStringsR(CTreeNode* h, CString& strResult, INT nInd, CTernarySearchTreeVisitor& visitor)
{
	if (NULL == h)
	{
		return;
	}

	// Process left sub-tree
	if (h->l != NULL)
	{
		SortStringsR(h->l, strResult, nInd, visitor);
	}

	// Process root
	INT nLength = strResult.GetLength();
	if (nInd >= nLength)
	{
		strResult += (_TUCHAR) h->digit;
	}
	else
	{
		strResult.SetAt(nInd, h->digit);
	}

	// Reached string end, call Visit function
	if (CTreeNode::NULL_DIGIT == h->digit)
	{
		visitor.Visit(strResult, h->data);
	}

	// Process middle sub-tree
	if (h->m != NULL)
	{
		SortStringsR(h->m, strResult, (nInd+1), visitor);
	}

	// Process right sub-tree
	if (h->r != NULL)
	{
		SortStringsR(h->r, strResult, nInd, visitor);
	}
}


BOOL CTernarySearchTree::RemoveR(CTreeNode* &h, LPCTSTR szItem)
{
	BOOL bRemoved = FALSE;
	_TUCHAR nItemDigit = (_TUCHAR) *szItem;

	if (NULL == h)
	{
		return bRemoved;
	}

	if (h->digit == nItemDigit)
	{
		// If its not a node with null terminator, continue searching and removing
		if (CTreeNode::NULL_DIGIT != h->digit)
		{
			bRemoved = RemoveR(h->m, (szItem+1));
		}

		// If there are no more nodes in the middle sub-tree, replace this node
		// for another node as root and delete it
		if (NULL == h->m)
		{
			CTreeNode* pNode = h;
			ReplaceRootNode(h);
			
			delete pNode;
			pNode = NULL;

			bRemoved = TRUE;
		}

		return bRemoved;
	}

	// (h->digit < nItemDigit) or 
	// h->digit is wildcard and there can be no nodes in h->l
	else if ((h->digit < nItemDigit) ||
			 (CTreeNode::WILDCARD_DIGIT == h->digit))
	{
		return RemoveR(h->r, szItem);
	}

	// (h->digit > nItemDigit)
	else
	{
		return RemoveR(h->l, szItem);
	}
}


void CTernarySearchTree::ReplaceRootNode(CTreeNode* &h)
{
	CTreeNode* pNewRootParent;
	CTreeNode* pNewRoot;

	// Try searching h->l greatest digit node
	if (NULL != h->l)
	{
		// Search on right sub-trees...
		pNewRootParent = h;
		pNewRoot = h->l;
		while (pNewRoot->r != NULL)
		{
			pNewRootParent = pNewRoot;
			pNewRoot = pNewRoot->r;
		}

		// Replace h with new root...
		// Check if new root is h->l to avoid circular reference
		if (pNewRoot != h->l)
		{
			pNewRootParent->r = pNewRoot->l;
			pNewRoot->l = h->l;
		}
		pNewRoot->r = h->r;
		h->l = NULL;
		h->r = NULL;
		h = pNewRoot;
	}

	// Try searching h->r lowest digit node
	else if (NULL != h->r)
	{
		// Search on left sub-trees...
		pNewRootParent = h;
		pNewRoot = h->r;
		while (pNewRoot->l != NULL)
		{
			pNewRootParent = pNewRoot;
			pNewRoot = pNewRoot->l;
		}

		// Replace h with new root...
		// Check if new root is h->r to avoid circular reference
		if (pNewRoot != h->r)
		{
			pNewRootParent->l = pNewRoot->r;
			pNewRoot->r = h->r;
		}
		pNewRoot->l = h->l;
		h->l = NULL;
		h->r = NULL;
		h = pNewRoot;
	}

	// h has no children to replace it
	else
	{
		h = NULL;
	}
}