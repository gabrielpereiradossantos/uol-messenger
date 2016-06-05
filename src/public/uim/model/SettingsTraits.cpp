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
#include "SettingsTraits.h"

BOOL CSettingsTraits::Load(MSXML::IXMLDOMNodePtr pNode, CSettingsMap& mapSettings)
{
	BOOL bSuccess = FALSE;

	if (pNode)
	{
		if (pNode->hasChildNodes())
		{
			MSXML::IXMLDOMNodeListPtr pNodeList = pNode->GetchildNodes();
			if (NULL != pNodeList)
			{
				long lSize = pNodeList->length;

				MSXML::IXMLDOMNodePtr pChild;

				for (long lInd = 0; lInd < lSize; lInd++)
				{
					pChild = pNodeList->Getitem(lInd);

					CComBSTR bstrNodeName;
					pChild->get_nodeName(&bstrNodeName);

					CSettingsMap::CPair* pPair = mapSettings.Lookup((CString) bstrNodeName);

					if (pPair)
					{
						(pPair->m_value)->Load(pChild, pPair->m_key);
					}
				}
				pNodeList.Release();
			}
		}

		MSXML::IXMLDOMNamedNodeMapPtr pAttrList = pNode->Getattributes();

		if (NULL != pAttrList)
		{
			long lSize = pAttrList->length;

			MSXML::IXMLDOMNodePtr pAttr;

			for (long lInd = 0; lInd < lSize; lInd++)
			{
				pAttr = pAttrList->Getitem(lInd);

				CComBSTR bstrNodeName;
				pAttr->get_nodeName(&bstrNodeName);

				CSettingsMap::CPair* pPair = mapSettings.Lookup((CString) bstrNodeName);

				if (pPair)
				{
					(pPair->m_value)->Load(pAttr, pPair->m_key);
				}
			}
			pAttrList.Release();
		}

		bSuccess = TRUE;
	}

	return bSuccess;
}

BOOL CSettingsTraits::Load(MSXML::IXMLDOMNodePtr pNode, CProcessMap& mapProcess, ISettings* pSettings)
{
	BOOL bSuccess = FALSE;

	if (pNode)
	{
		MSXML::IXMLDOMNamedNodeMapPtr pAttrList = pNode->Getattributes();

		if (NULL != pAttrList)
		{
			long lSize = pAttrList->length;

			MSXML::IXMLDOMNodePtr pAttr;

			for (long lInd = 0; lInd < lSize; lInd++)
			{
				pAttr = pAttrList->Getitem(lInd);

				CComBSTR bstrNodeName;
				pAttr->get_nodeName(&bstrNodeName);

				CProcessMap::CPair* pPair = mapProcess.Lookup((CString) bstrNodeName);

				if (pPair)
				{
					//(pSettings->*(pPair->m_value))(pAttr, pPair->m_key, FALSE);
					pPair->m_value->Execute(pAttr, pPair->m_key, FALSE);
				}
			}
			pAttrList.Release();
		}

		if (pNode->hasChildNodes())
		{
			MSXML::IXMLDOMNodeListPtr pNodeList = pNode->GetchildNodes();
			if (NULL != pNodeList)
			{
				long lSize = pNodeList->length;

				MSXML::IXMLDOMNodePtr pChild;

				for (long lInd = 0; lInd < lSize; lInd++)
				{
					pChild = pNodeList->Getitem(lInd);

					CComBSTR bstrNodeName;
					pChild->get_nodeName(&bstrNodeName);

					CProcessMap::CPair* pPair = mapProcess.Lookup((CString) bstrNodeName);

					if (pPair)
					{
						//(pSettings->*(pPair->m_value))(pChild, pPair->m_key, FALSE);
						pPair->m_value->Execute(pChild, pPair->m_key, FALSE);
					}
				}
				pNodeList.Release();
			}
		}

		bSuccess = TRUE;
	}

	return bSuccess;
}

BOOL CSettingsTraits::Save(MSXML::IXMLDOMNodePtr pNode, CSettingsMap& mapSettings)
{
	BOOL bSuccess = FALSE;

	if (pNode)
	{
		CSettingsMap::CPair* pPair;

		for (POSITION pos = mapSettings.GetStartPosition(); pos;)
		{
			pPair = mapSettings.GetNext(pos);

			if (pPair)
			{
				MSXML::IXMLDOMNodePtr pChild = CSettingsTraits::OpenGroup(pNode, pPair->m_key);
				(pPair->m_value)->Save(pChild, pPair->m_key);
			}
		}

		bSuccess = TRUE;
	}

	return bSuccess;
}

BOOL CSettingsTraits::Save(MSXML::IXMLDOMNodePtr pNode, CProcessMap& mapProcess, ISettings* pSettings)
{
	BOOL bSuccess = FALSE;

	if (pNode)
	{
		CProcessMap::CPair* pPair;

		for (POSITION pos = mapProcess.GetStartPosition(); pos;)
		{
			pPair = mapProcess.GetNext(pos);

			if (pPair)
			{
				//(pSettings->*(pPair->m_value))(pNode, pPair->m_key, TRUE);
				pPair->m_value->Execute(pNode, pPair->m_key, TRUE);
			}
		}

		bSuccess = TRUE;
	}

	return bSuccess;
}
// Value methods
BOOL CSettingsTraits::GetIntValue(MSXML::IXMLDOMNodePtr pNode, const CString& strKey, int& dwValue)
{
	if (NULL != pNode)
	{
		CComVariant variant;
		variant.ChangeType(VT_I4, &pNode->GetnodeValue());

		if (variant.vt == VT_EMPTY)
		{
			return FALSE;
		}

		dwValue = variant.intVal;
	}

	return (NULL != pNode);
}


BOOL CSettingsTraits::GetDWORDValue(MSXML::IXMLDOMNodePtr pNode, const CString& strKey, DWORD& dwValue)
{
	if (NULL != pNode)
	{
		CComVariant variant;
		variant.ChangeType(VT_UI4, &pNode->GetnodeValue());
		
		if (variant.vt == VT_EMPTY)
		{
			return FALSE;
		}

		dwValue = variant.intVal;
	}

	return (NULL != pNode);
}

BOOL CSettingsTraits::GetBOOLValue(MSXML::IXMLDOMNodePtr pNode, const CString& strKey, BOOL& bValue)
{
	if (NULL != pNode)
	{
		CComVariant variant;
		variant.ChangeType(VT_INT, &pNode->GetnodeValue());

		if (variant.vt == VT_EMPTY)
		{
			return FALSE;
		}

		bValue = variant.intVal;
	}

	return (NULL != pNode);
}

BOOL CSettingsTraits::GetStringAttibuteValue(MSXML::IXMLDOMNodePtr pNode, const CString& strKey, CString& strValue)
{
	if (NULL != pNode)
	{
		CComVariant variant;
		variant.ChangeType(VT_BSTR, &pNode->GetnodeValue());
		
		if (variant.vt == VT_EMPTY)
		{
			return FALSE;
		}
		
		strValue = variant.bstrVal;
	}
	
	return (NULL != pNode);
}

BOOL CSettingsTraits::GetStringValue(MSXML::IXMLDOMNodePtr pNode, const CString& strKey, CString& strValue)
{
	MSXML::IXMLDOMNodePtr pChild = NULL;

	if (NULL != pNode && pNode->hasChildNodes())
	{
		pChild = pNode->GetfirstChild();

		if ((NODE_TEXT == pChild->GetnodeType()) || 
			(NODE_CDATA_SECTION == pChild->GetnodeType()))
		{
			CComVariant variant =  pChild->GetnodeValue();

			if (variant.vt == VT_EMPTY)
			{
				return FALSE;
			}

			strValue = variant.bstrVal;
		}
	}

	return (NULL != pChild);
}

BOOL CSettingsTraits::GetStringListValue(MSXML::IXMLDOMNodePtr pNode, const CString& strKey, CAtlList<CString>& listValue)
{
	MSXML::IXMLDOMNodePtr pChild = NULL;
	MSXML::IXMLDOMNodePtr pText = NULL;

		if (NULL != pNode)
		{
			if (pNode->hasChildNodes())
			{
				pChild = pNode->GetfirstChild();

				while (NULL != pChild)
				{
					pText = pChild->GetfirstChild();

					if (NULL != pText && NODE_TEXT == pText->GetnodeType())
					{
						CComVariant variant =  pText->GetnodeValue();
						CString strValue(variant);
						listValue.AddTail(strValue);
					}

					pChild = pChild->GetnextSibling();
				}
			}
		}

	return (NULL != pNode);
}

BOOL CSettingsTraits::SetIntValue(MSXML::IXMLDOMNodePtr pNode, const CString& strKey, int nValue)
{
	BOOL bSuccess = FALSE;

	MSXML::IXMLDOMDocumentPtr pDoc;
	MSXML::IXMLDOMNamedNodeMapPtr pAttributes;
	MSXML::IXMLDOMNodePtr pAttr;

	CComVariant variant(nValue);

	if (NULL != pNode)
	{
		pDoc = pNode->GetownerDocument();

		if (pDoc)
		{
			pAttributes = pNode->Getattributes();

			if (NULL != pAttributes)
			{
				// Create attribute
				pAttr = pDoc->createAttribute(strKey.AllocSysString());

				if (NULL != pAttr)
				{
					pAttr->PutnodeValue(variant);
					pAttributes->setNamedItem(pAttr);
					pAttr.Release();
					bSuccess = TRUE;
				}
			}
		}
	}

	return (bSuccess);
}

BOOL CSettingsTraits::SetDWORDValue(MSXML::IXMLDOMNodePtr pNode, const CString& strKey, DWORD dwValue)
{
	BOOL bSuccess = FALSE;

	MSXML::IXMLDOMDocumentPtr pDoc;
	MSXML::IXMLDOMNamedNodeMapPtr pAttributes;
	MSXML::IXMLDOMNodePtr pAttr;

	CComVariant variant(dwValue);

	if (NULL != pNode)
	{
		pDoc = pNode->GetownerDocument();

		if (pDoc)
		{
			pAttributes = pNode->Getattributes();

			if (NULL != pAttributes)
			{
				// Create attribute
				pAttr = pDoc->createAttribute(strKey.AllocSysString());

				if (NULL != pAttr)
				{
					pAttr->PutnodeValue(variant);
					pAttributes->setNamedItem(pAttr);
					pAttr.Release();
					bSuccess = TRUE;
				}
			}
		}
	}

	return (bSuccess);
}

BOOL CSettingsTraits::SetBOOLValue(MSXML::IXMLDOMNodePtr pNode, const CString& strKey, BOOL bValue)
{
	BOOL bSuccess = FALSE;

	MSXML::IXMLDOMDocumentPtr pDoc;
	MSXML::IXMLDOMNamedNodeMapPtr pAttributes;
	MSXML::IXMLDOMNodePtr pAttr;

	CComVariant variant(bValue);

	if (NULL != pNode)
	{
		pDoc = pNode->GetownerDocument();

		if (pDoc)
		{
			pAttributes = pNode->Getattributes();

			if (NULL != pAttributes)
			{
				// Create attribute
				pAttr = pDoc->createAttribute(strKey.AllocSysString());

				if (NULL != pAttr)
				{
					pAttr->PutnodeValue(variant);
					pAttributes->setNamedItem(pAttr);
					pAttr.Release();
					bSuccess = TRUE;
				}
			}
		}
	}

	return (bSuccess);
}

BOOL CSettingsTraits::SetStringAttributeValue(MSXML::IXMLDOMNodePtr pNode, const CString& strKey, const CString& strValue)
{
	BOOL bSuccess = FALSE;
	
	MSXML::IXMLDOMDocumentPtr pDoc;
	MSXML::IXMLDOMNamedNodeMapPtr pAttributes;
	MSXML::IXMLDOMNodePtr pAttr;
	
	CComVariant variant(strValue.AllocSysString());
	
	if (NULL != pNode)
	{
		pDoc = pNode->GetownerDocument();

		if (pDoc)
		{
			pAttributes = pNode->Getattributes();

			if (NULL != pAttributes)
			{
				// Create attribute
				pAttr = pDoc->createAttribute(strKey.AllocSysString());

				if (NULL != pAttr)
				{
					pAttr->PutnodeValue(variant);
					pAttributes->setNamedItem(pAttr);
					pAttr.Release();
					bSuccess = TRUE;
				}
			}
		}
	}

	return (bSuccess);
}

BOOL CSettingsTraits::SetStringValue(MSXML::IXMLDOMNodePtr pNode, const CString& strKey, const CString& strValue)
{
	BOOL bSuccess = FALSE;

	MSXML::IXMLDOMDocumentPtr pDoc;
	MSXML::IXMLDOMNodePtr pChild;
	MSXML::IXMLDOMCDATASectionPtr pCData;

	if (NULL != pNode)
	{
		pDoc = pNode->GetownerDocument();

		if (pDoc)
		{
			pChild = pDoc->createElement(strKey.AllocSysString());

			if (NULL != pChild)
			{
				pCData = pDoc->createCDATASection(strValue.AllocSysString());

				if (NULL != pCData)
				{
					pChild->appendChild(pCData);
					pCData.Release();
					bSuccess = TRUE;
				}

				pNode->appendChild(pChild);
				pChild.Release();
			}
		}
	}

	return (bSuccess);
}

BOOL CSettingsTraits::SetStringListValue(MSXML::IXMLDOMNodePtr pNode, const CString& strKey, CAtlList<CString>& listValue)
{
	BOOL bSuccess = FALSE;

	MSXML::IXMLDOMDocumentPtr pDoc;
	MSXML::IXMLDOMNodePtr pChild;
	MSXML::IXMLDOMTextPtr pText;

	CString strValue;

	if (NULL != pNode)
	{
		pDoc = pNode->GetownerDocument();

		if (pDoc)
		{
			for (POSITION pos = listValue.GetHeadPosition(); pos;)
			{
				strValue = listValue.GetNext(pos);

				pChild = pDoc->createElement(strKey.AllocSysString());

				if (NULL != pChild)
				{
					pText = pDoc->createTextNode(strValue.AllocSysString());

					if (NULL != pText)
					{
						pChild->appendChild(pText);
						pText.Release();
						bSuccess = TRUE;
					}

					pNode->appendChild(pChild);
					pChild.Release();
				}
			}
		}
	}

	return (bSuccess);
}

MSXML::IXMLDOMNodePtr CSettingsTraits::OpenGroup(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	BOOL bSuccess = FALSE;

	MSXML::IXMLDOMDocumentPtr pDoc;
	MSXML::IXMLDOMNodePtr pGroup;

	if (NULL != pNode)
	{
		pDoc = pNode->GetownerDocument();

		if (pDoc)
		{
			pGroup = pDoc->createElement(strKey.AllocSysString());

			if (NULL != pGroup)
			{
				pNode->appendChild(pGroup);
			}
		}
	}

	return pGroup;
}