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
 

#include "purplebuddylisthelper.h"

#include <vector>


// Purple headers
#include <util.h>


using namespace std;

//const char* CPurpleBuddyListHelper::BUDDY_LIST_OPERATION_SETTING = "__hlb_operation";
//const char* CPurpleBuddyListHelper::BUDDY_LIST_OLD_GROUP_NAME_SETTING = "__blh_old_group_name";


void CPurpleBuddyListHelper::AddPurpleGroup(PurpleGroup* pGroup)
{
	PurpleBuddyList* pPurpleBuddyList = purple_get_blist();

	PurpleBlistNode* pGroupNode = (PurpleBlistNode*) pGroup;

	if (pPurpleBuddyList->root == pGroupNode)
	{
		// Group is already buddy list root
		return;
	}

	// Add group to root
	if (pPurpleBuddyList->root)
	{
		pPurpleBuddyList->root->prev = pGroupNode;
	}
	
	pGroupNode->next = pPurpleBuddyList->root;
	pGroupNode->prev = NULL;
	pPurpleBuddyList->root = pGroupNode;
}


void CPurpleBuddyListHelper::RemovePurpleEmptyGroup(PurpleGroup* pGroup)
{
	PurpleBlistNode* pNode = (PurpleBlistNode *) pGroup;

	if (pNode->child == NULL)
	{
		PurpleBuddyList* pBuddyList = purple_get_blist();

		// Remove the node from its parent
		if (pBuddyList->root == pNode)
		{
			pBuddyList->root = pNode->next;
		}
		if (pNode->prev != NULL)
		{
			pNode->prev->next = pNode->next;
		}
		if (pNode->next != NULL)
		{
			pNode->next->prev = pNode->prev;
		}

		purple_blist_schedule_save();
		
		// Update the UI
		PurpleBlistUiOps* pOps = purple_blist_get_ui_ops();
		if ((pOps != NULL) && (pOps->remove != NULL))
		{
			pOps->remove(pBuddyList, pNode);
		}

		// Delete the node
		g_hash_table_destroy(pGroup->node.settings);
		g_free(pGroup->name);
		g_free(pGroup);
	}
}


void CPurpleBuddyListHelper::AddPurpleBuddy(PurpleGroup* pGroup, PurpleBuddy* pBuddy)
{
	PurpleBlistNode* pGroupNode;
	PurpleBlistNode* pPreviousContactNode;
	PurpleBlistNode* pBuddyNode;
	PurpleContact* pContact;

	pGroupNode = (PurpleBlistNode*) pGroup;
	pPreviousContactNode = pGroupNode->child;
	pBuddyNode = (PurpleBlistNode*) pBuddy;
	pContact = purple_contact_new();

	if (pContact != NULL)
	{
		// Initialize contact...
		if (PURPLE_BUDDY_IS_ONLINE(pBuddy))
		{
			pContact->online++;
		}

		if (purple_account_is_connected(pBuddy->account))
		{
			pContact->currentsize++;
		}

		pContact->totalsize++;
		pContact->alias = (pBuddy->alias != NULL) ? g_strdup(pBuddy->alias) : NULL;
		pContact->priority = pBuddy;

		// Initialize contact node...
		PurpleBlistNode* pContactNode = (PurpleBlistNode*) pContact;
		
		pContactNode->child = pBuddyNode;
		pBuddyNode->parent = pContactNode;

		pContactNode->prev = NULL;

		if (pPreviousContactNode != NULL)
		{
			pPreviousContactNode->prev = pContactNode;
			pContactNode->next = pPreviousContactNode;
		}
		else
		{
			pContactNode->next = NULL;
		}

		pContactNode->parent = pGroupNode;
			
		// Update group...
		pGroupNode->child = pContactNode;

		if (pContact->online > 0)
		{
			pGroup->online++;
		}

		if (pContact->currentsize > 0)
		{
			pGroup->currentsize++;
		}

		pGroup->totalsize++;

		// Add to buddy list hash table
		_purple_hbuddy* hb = g_new(struct _purple_hbuddy, 1);
		hb->name = g_strdup(purple_normalize(pBuddy->account, pBuddy->name));
		hb->account = pBuddy->account;
		hb->group = (PurpleBlistNode*) pGroup;
							
		g_hash_table_replace(purple_get_blist()->buddies, hb, pBuddy);
	}
}


void CPurpleBuddyListHelper::RemovePurpleBuddy(PurpleGroup* pGroup, PurpleBuddy* pBuddy)
{
	PurpleBlistNode* pBuddyNode = (PurpleBlistNode*) pBuddy;
	PurpleBlistNode* pContactNode = pBuddyNode->parent;

	// Remove from contact node
	if (pContactNode->child == pBuddyNode)
	{
		pContactNode->child = pBuddyNode->next;
	}
	
	if (pBuddyNode->prev != NULL)
	{
		pBuddyNode->prev->next = pBuddyNode->next;
	}

	if (pBuddyNode->next != NULL)
	{
		pBuddyNode->next->prev = pBuddyNode->prev;
	}

	pBuddyNode->prev = NULL;
	pBuddyNode->next = NULL;

	// Adjust sizes...
	PurpleContact* pContact = (PurpleContact*) pContactNode;
	if (PURPLE_BUDDY_IS_ONLINE(pBuddy))
	{
		pContact->online--;
		if (pContact->online == 0)
		{
			pGroup->online--;
		}
	}

	if (purple_account_is_connected(pBuddy->account))
	{
		pContact->currentsize--;
		if (pContact->currentsize == 0)
		{
			pGroup->currentsize--;
		}
	}
	pContact->totalsize--;

	// Remove from buddy list hash table
	_purple_hbuddy* hb = g_new(struct _purple_hbuddy, 1);
	hb->name = g_strdup(purple_normalize(pBuddy->account, pBuddy->name));
	hb->account = pBuddy->account;
	hb->group = (PurpleBlistNode*) pGroup;

	g_hash_table_remove(purple_get_blist()->buddies, hb);
	g_free(hb->name);
	g_free(hb);

	// Contact has no more buddies, remove from group
	if (pContactNode->child == NULL)
	{
		purple_blist_remove_contact(pContact);
	}
}


void CPurpleBuddyListHelper::AddPurpleBuddies(PurpleGroup* pGroup, GList* pListBuddies)
{
	PurpleBlistNode* pGroupNode;
	PurpleBlistNode* pContactNode;
	PurpleBlistNode* pPreviousContactNode;
	PurpleBlistNode* pBuddyNode;
	PurpleBuddy* pBuddy;	
	PurpleContact* pContact;
	GList* pList;

	pGroupNode = (PurpleBlistNode*) pGroup;
	pPreviousContactNode = pGroupNode->child;

	for (pList = pListBuddies; pListBuddies != NULL; pListBuddies = pListBuddies->next)
	{
		pBuddyNode = (PurpleBlistNode*) pListBuddies->data;

		pContact = purple_contact_new();
		if (pContact != NULL)
		{
			pBuddy = (PurpleBuddy*) pBuddyNode;

			// Initialize contact...
			if (PURPLE_BUDDY_IS_ONLINE(pBuddy))
			{
				pContact->online++;
			}

			if (purple_account_is_connected(pBuddy->account))
			{
				pContact->currentsize++;
			}

			pContact->totalsize++;
			pContact->alias = (pBuddy->alias != NULL) ? g_strdup(pBuddy->alias) : NULL;
			pContact->priority = pBuddy;

			// Initialize contact node...
			pContactNode = (PurpleBlistNode*) pContact;
			pContactNode->child = pBuddyNode;			
			pContactNode->prev = pPreviousContactNode;

			pBuddyNode->parent = pContactNode;
			
			if (pPreviousContactNode != NULL)
			{
				pPreviousContactNode->next = pContactNode;
			}
			pPreviousContactNode = pContactNode;

			pContactNode->parent = pGroupNode;
			
			// Update group...
			if (!pGroupNode->child)
			{
				pGroupNode->child = pContactNode;
			}

			if (pContact->online > 0)
			{
				pGroup->online++;
			}

			if (pContact->currentsize > 0)
			{
				pGroup->currentsize++;
			}

			pGroup->totalsize++;

            // Add to buddy list hash table
			_purple_hbuddy* hb = g_new(struct _purple_hbuddy, 1);
			hb->name = g_strdup(purple_normalize(pBuddy->account, pBuddy->name));
			hb->account = pBuddy->account;
			hb->group = (PurpleBlistNode*) pGroup;
							
			g_hash_table_replace(purple_get_blist()->buddies, hb, pBuddy);
		}
	}
}


void CPurpleBuddyListHelper::RemovePurpleBuddies(PurpleAccount* pAccount, PurpleGroup* pGroup, GList*& pListBuddies)
{
	PurpleBlistNode* pGroupNode;
	PurpleBlistNode* pContactNode;
	PurpleBlistNode* pBuddyNode;
	PurpleBuddy* pBuddy;	
	PurpleContact* pContact;
	PurpleBlistNode* pTmpContactNode;

	pGroupNode = (PurpleBlistNode*) pGroup;
	pContactNode = pGroupNode->child;
	while (pContactNode != NULL)
	{
		if (PURPLE_BLIST_NODE_IS_CONTACT(pContactNode))
		{
			pContact = (PurpleContact*) pContactNode;
			for (pBuddyNode = pContactNode->child; pBuddyNode != NULL; pBuddyNode = pBuddyNode->next)
			{
				if (PURPLE_BLIST_NODE_IS_BUDDY(pBuddyNode))
				{
					pBuddy = (PurpleBuddy*) pBuddyNode;
					if (pBuddy->account == pAccount)
					{
						// Remove from contact node
						if (pContactNode->child == pBuddyNode)
						{
							pContactNode->child = pBuddyNode->next;
						}
						if (pBuddyNode->prev != NULL)
						{
							pBuddyNode->prev->next = pBuddyNode->next;
						}
						if (pBuddyNode->next != NULL)
						{
							pBuddyNode->next->prev = pBuddyNode->prev;
						}

						// Adjust sizes
						if (PURPLE_BUDDY_IS_ONLINE(pBuddy))
						{
							pContact->online--;
							if (pContact->online == 0)
							{
								pGroup->online--;
							}
						}

						if (purple_account_is_connected(pBuddy->account))
						{
							pContact->currentsize--;
							if (pContact->currentsize == 0)
							{
								pGroup->currentsize--;
							}
						}
						pContact->totalsize--;

						// Remove from buddy list hash table
						_purple_hbuddy* hb = g_new(struct _purple_hbuddy, 1);
						hb->name = g_strdup(purple_normalize(pBuddy->account, pBuddy->name));
						hb->account = pBuddy->account;
						hb->group = (PurpleBlistNode*) pGroup;
						
						g_hash_table_remove(purple_get_blist()->buddies, hb);
						g_free(hb->name);
						g_free(hb);

						// Append to list
						pListBuddies = g_list_append(pListBuddies, pBuddyNode);
					}
				}
			}

			pTmpContactNode = pContactNode;
			pContactNode = pContactNode->next;

			// Contact has no more buddies, remove from group
			if (pTmpContactNode->child == NULL)
			{
				purple_blist_remove_contact(pContact);
			}
		}
		else
		{
			pContactNode = pContactNode->next;
		}
	}
}


int CPurpleBuddyListHelper::GetPurpleGroupBuddiesCount(PurpleAccount* pAccount, PurpleGroup* pGroup)
{
	int nBuddiesCount = 0;
	PurpleBlistNode* pChild;
	PurpleBlistNode* pContactChild;
	PurpleBuddy* pBuddy;

	PurpleBlistNode* pNode = (PurpleBlistNode *) pGroup;

	// Get account buddies count
	for (pChild = pNode->child; pChild != NULL; pChild = pChild->next)
	{
		if (PURPLE_BLIST_NODE_IS_CONTACT(pChild))
		{
			for (pContactChild = pChild->child; pContactChild != NULL; pContactChild = pContactChild->next)
			{
				if (PURPLE_BLIST_NODE_IS_BUDDY(pContactChild))
				{
					pBuddy = (PurpleBuddy*) pContactChild;
					if (pBuddy->account == pAccount)
					{
							nBuddiesCount++;
					}
				}
			}
		}
	}

	return nBuddiesCount;
}


void CPurpleBuddyListHelper::RemovePurpleAccountBuddies(PurpleAccount* pAccount)
{
	PurpleBlistNode *gnode, *cnode, *bnode;

	// Get online accounts count...
	int nOnlineAccountCount = 0;
		
	GList* pListConn;
	PurpleConnection* pPurpleConn;
	for (pListConn = purple_connections_get_all(); pListConn != NULL; pListConn = pListConn->next)
	{
		pPurpleConn = (PurpleConnection*) pListConn->data;

		if (PURPLE_CONNECTION_IS_CONNECTED(pPurpleConn))
		{
			nOnlineAccountCount++;
		}
	}

    PurpleBuddyList* pBuddyList = purple_get_blist();
	if (!pBuddyList)
	{
		return;
	}

	// Remove buddy list nodes...
	vector<PurpleGroup*> vEmptyGroups;

	for (gnode = pBuddyList->root; gnode != NULL; gnode = gnode->next)
	{
		if (!PURPLE_BLIST_NODE_IS_GROUP(gnode))
		{
			// Not group
			continue;
		}

		if (NULL != gnode->child)
		{
			// Remove children...
			cnode = gnode->child;
			while (cnode)
			{
				PurpleBlistNode *cnode_next = cnode->next;

				if (PURPLE_BLIST_NODE_IS_CONTACT(cnode))
				{
					// Remove contact node buddies...
					// Purple will automatically remove contact node after all buddies are removed
					bnode = cnode->child;
					while (bnode)
					{
						PurpleBlistNode *bnode_next = bnode->next;

						if (PURPLE_BLIST_NODE_IS_BUDDY(bnode))
						{
							PurpleBuddy* pBuddy = (PurpleBuddy*) bnode;

							if (pBuddy->account == pAccount)
							{
								purple_blist_remove_buddy(pBuddy);
							}
						}
						bnode = bnode_next;
					}
				} 
				else if (PURPLE_BLIST_NODE_IS_CHAT(cnode))
				{
					// Remove chat
					PurpleChat* pChat = (PurpleChat*) cnode;

					if (pChat->account == pAccount)
					{
						purple_blist_remove_chat(pChat);
					}
				}

				cnode = cnode_next;
			}
		}
		else
		{
			// Empty group, mark for removal if no online accounts
			if (0 == nOnlineAccountCount)
			{
				PurpleGroup* pGroup = (PurpleGroup*) gnode;
				vEmptyGroups.push_back(pGroup);
			}
		}
	}

	// Remove empty groups
	vector<PurpleGroup*>::iterator it;
	for (it = vEmptyGroups.begin(); it != vEmptyGroups.end(); it++)
	{
		RemovePurpleEmptyGroup(*it);
	}
}