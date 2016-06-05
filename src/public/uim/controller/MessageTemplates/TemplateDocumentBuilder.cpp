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
#include "TemplateDocumentBuilder.h"


#include "MessageTemplateManager.h"
#include "../../FileUtils.h"
#include "../UIMApplication.h"
#include "../../view/HTMLUtilitary.h"
#include "../../model/MessageTemplateRecord.h"


CTemplateDocumentBuilder::CTemplateDocumentBuilder(IUOLMessengerConversationPtr pConversation) : 
		m_bInitialized(FALSE), 
		m_bTemplateElementsLoaded(FALSE), 
		m_bReplacingTagsLoaded(FALSE)
{
	m_pConversation2 = ap_dynamic_cast<IUOLMessengerConversation2Ptr>(pConversation);
	
	LoadTemplateElements();
}


CTemplateDocumentBuilder::~CTemplateDocumentBuilder()
{
}


void CTemplateDocumentBuilder::AddMessageTemplateRecord(IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord)
{
	m_MessageTemplateRecordList.AddTail(pMessageTemplateRecord);
	
	if (m_bTemplateElementsLoaded)
	{
		if ( (m_bInitialized) || (InitializeMainBlocks()) )
		{
			IUOLMessengerMessageTemplatePtr	pCurrentMessageTemplate = 
					GetTemplateManager()->GetDefaultTemplate();
			
			if (m_pMessageTemplate == pCurrentMessageTemplate)
			{
				AppendMessageTemplateRecordToDocument(pMessageTemplateRecord);
			}
		}
	}
}


CString CTemplateDocumentBuilder::GetUpdatedDocument(BOOL bForceRebuild)
{
	CString strUpdatedDocument;
	
	if (m_bInitialized)
	{
		IUOLMessengerMessageTemplatePtr	pCurrentMessageTemplate = 
				GetTemplateManager()->GetDefaultTemplate();
		
		if ( (m_pMessageTemplate != pCurrentMessageTemplate) || (bForceRebuild) )
		{
			m_pMessageTemplate = pCurrentMessageTemplate;
			
			// We consider here that forcing is used always with the 
			// same template. (have a look on the other comparition)
			//
			return RebuildDocument(!bForceRebuild);
		}
		else
		{
			strUpdatedDocument.Format("%s%s%s%s", m_templateDocumentTop, 
					m_templateDocumentMessages, m_lastMessageBlock, 
					m_templateDocumentBottom);
		}
	}
	
	return strUpdatedDocument;
}


BOOL CTemplateDocumentBuilder::InitializeMainBlocks()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_bInitialized == [%d].", m_bInitialized);
	
	if (m_bInitialized)
	{
		return m_bInitialized;
	}
	
	CString messageTemplateReplaceInfo = GetReplacingTagInfo(m_strMainTemplate);
	CString strHeaderTemplate;
	CString strFooterTemplate;
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"messageTemplateReplaceInfo == [%s].", messageTemplateReplaceInfo);
	
	IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord = m_MessageTemplateRecordList.GetHead();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"pMessageTemplateRecord == [%x].", pMessageTemplateRecord.get());
	
	if ( (pMessageTemplateRecord) && (!m_strHeaderTemplate.IsEmpty()) )
	{
		strHeaderTemplate = ReplaceTagsOnTemplate(
				IUOLMessengerMessageTemplate::TEMPLATE_HEADER, pMessageTemplateRecord);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"strHeaderTemplate == [%s].", strHeaderTemplate);
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strHeaderTemplate == [%s].", m_strHeaderTemplate);
	}
	
	if ( (pMessageTemplateRecord) && (!m_strFooterTemplate.IsEmpty()) )
	{
		strFooterTemplate = ReplaceTagsOnTemplate(
				IUOLMessengerMessageTemplate::TEMPLATE_FOOTER, pMessageTemplateRecord);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"strFooterTemplate == [%s].", strFooterTemplate);
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_strFooterTemplate == [%s].", m_strFooterTemplate);
	}
	
	if (!messageTemplateReplaceInfo.IsEmpty())
	{
		// Remove the reference retrieved.
		//
		if (CHTMLUtilitary::Replace(m_strMainTemplate, 
				messageTemplateReplaceInfo, ""))
		{
			if (CHTMLUtilitary::Replace(m_strMainTemplate, 
					messageTemplateReplaceInfo, m_strCSSImport))
			{
				if (CHTMLUtilitary::Replace(m_strMainTemplate, 
						messageTemplateReplaceInfo, strHeaderTemplate))
				{
					if (CHTMLUtilitary::Replace(m_strMainTemplate, 
							messageTemplateReplaceInfo, strFooterTemplate))
					{
						int iTopTemplate = 0;
						
						iTopTemplate = m_strMainTemplate.Find(m_messageTemplateAditionPoint);
						iTopTemplate += m_messageTemplateAditionPoint.GetLength();
						
						while (m_strMainTemplate[iTopTemplate] != '<')
						{
							iTopTemplate++;
						}
						
						m_templateDocumentTop = m_strMainTemplate.Left(iTopTemplate);
						
						m_templateDocumentBottom = m_strMainTemplate.Right(
								m_strMainTemplate.GetLength() - iTopTemplate);
						
						m_templateDocumentMessages.Empty();
						m_lastMessageBlock.Empty();
						
						m_bInitialized = TRUE;
						
						static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
								GetLogger()->LogDebug(__FUNCTION__, 
								"m_messageTemplateAditionPoint = [%s], m_templateDocumentTop = [%s], m_templateDocumentBottom = [%s].", 
								m_messageTemplateAditionPoint, m_templateDocumentTop, m_templateDocumentBottom);
					}
					else
					{
						static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
								GetLogger()->LogError(__FUNCTION__, 
								"CHTMLUtilitary::Replace() when replacing [%s] from [%s] with [%s].", 
								messageTemplateReplaceInfo, m_strMainTemplate, strFooterTemplate);
					}
				}
				else
				{
					static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
							GetLogger()->LogError(__FUNCTION__, 
							"CHTMLUtilitary::Replace() when replacing [%s] from [%s] with [%s].", 
							messageTemplateReplaceInfo, m_strMainTemplate, strHeaderTemplate);
				}
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, 
						"CHTMLUtilitary::Replace() when replacing [%s] from [%s] with [%s].", 
						messageTemplateReplaceInfo, m_strMainTemplate, m_strCSSImport);
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"CHTMLUtilitary::Replace() when removing [%s] from [%s].", 
					messageTemplateReplaceInfo, m_strMainTemplate);
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_bInitialized = [%d]", m_bInitialized);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return m_bInitialized;
}


BOOL CTemplateDocumentBuilder::LoadTemplateElements()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_bTemplateElementsLoaded == [%d].", m_bTemplateElementsLoaded);
	
	if (m_bTemplateElementsLoaded)
	{
		return m_bTemplateElementsLoaded;
	}
	
	m_pMessageTemplate = GetTemplateManager()->GetDefaultTemplate();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_pMessageTemplate == [%x].", m_pMessageTemplate.get());
	
	if ( (m_pMessageTemplate) && (InitializeReplacementTags()) )
	{
		m_messageDefaultTemplate = GetTemplateManager()->GetDefaultTemplateDocument();
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"m_messageDefaultTemplate == [%s].", m_messageDefaultTemplate);
		
		// Gets the main template and breaks it on the peaces we need 
		// replacing the css file and header and footer templates.
		//
		m_strMainTemplate = m_pMessageTemplate->GetTemplateElement(
				IUOLMessengerMessageTemplate::TEMPLATE_TEMPLATE);
		m_strCSSImport = m_pMessageTemplate->GetTemplateElement(
				IUOLMessengerMessageTemplate::TEMPLATE_CSS_IMPORT);
		m_strHeaderTemplate = m_pMessageTemplate->GetTemplateElement(
				IUOLMessengerMessageTemplate::TEMPLATE_HEADER);
		m_strFooterTemplate = m_pMessageTemplate->GetTemplateElement(
				IUOLMessengerMessageTemplate::TEMPLATE_FOOTER);
		
		m_strStatusTemplate = m_pMessageTemplate->GetTemplateElement(
				IUOLMessengerMessageTemplate::TEMPLATE_STATUS);
		m_strNextStatusTemplate = m_pMessageTemplate->GetTemplateElement(
				IUOLMessengerMessageTemplate::TEMPLATE_NEXT_STATUS);
		m_strIncomingContentTemplate = m_pMessageTemplate->GetTemplateElement(
				IUOLMessengerMessageTemplate::TEMPLATE_INCOMING_CONTENT);
		m_strIncomingNextContentTemplate = m_pMessageTemplate->GetTemplateElement(
				IUOLMessengerMessageTemplate::TEMPLATE_INCOMING_NEXT_CONTENT);
		m_strIncomingFileTransferTemplate = m_pMessageTemplate->GetTemplateElement(
				IUOLMessengerMessageTemplate::TEMPLATE_INCOMING_FILE_TRANSFER);
		m_strOutgoingContentTemplate = m_pMessageTemplate->GetTemplateElement(
				IUOLMessengerMessageTemplate::TEMPLATE_OUTGOING_CONTENT);
		m_strOutgoingNextContentTemplate = m_pMessageTemplate->GetTemplateElement(
				IUOLMessengerMessageTemplate::TEMPLATE_OUTGOING_NEXT_CONTENT);
		
		// For now, always use the default template, because normal templates 
		// have scripts that we can't support yet.
		// Except the templates created by UOL.
		//
		//if (m_strMainTemplate.IsEmpty())
		//{
			//m_strMainTemplate = m_messageDefaultTemplate;
		//}
		if (m_pMessageTemplate->GetTemplateName().CompareNoCase("UOLMessenger") != 0)
		{
			m_strMainTemplate = m_messageDefaultTemplate;
			
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogDebug(__FUNCTION__, "Template name = UOLMessenger");
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"Mandatory -> m_strCSSImport = [%s], m_strMainTemplate = [%s], "
				"m_strStatusTemplate = [%s], m_strIncomingContentTemplate = [%s], "
				"m_strIncomingNextContentTemplate = [%s], m_strOutgoingContentTemplate = [%s], "
				"m_strOutgoingNextContentTemplate = [%s].", 
				m_strCSSImport, m_strMainTemplate, m_strStatusTemplate, 
				m_strIncomingContentTemplate, m_strIncomingNextContentTemplate, 
				m_strOutgoingContentTemplate, m_strOutgoingNextContentTemplate);
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, 
				"Optional - > m_strHeaderTemplate = [%s], m_strFooterTemplate = [%s], "
				"m_strNextStatusTemplate = [%s], m_strIncomingFileTransferTemplate = ]%s].", 
				m_strHeaderTemplate, m_strFooterTemplate, m_strNextStatusTemplate, 
				m_strIncomingFileTransferTemplate);
		
		if ( (!m_strCSSImport.IsEmpty()) && (!m_strMainTemplate.IsEmpty()) && 
				(!m_strStatusTemplate.IsEmpty()) && 
				(!m_strIncomingContentTemplate.IsEmpty()) && 
				(!m_strIncomingNextContentTemplate.IsEmpty()) && 
				(!m_strOutgoingContentTemplate.IsEmpty()) && 
				(!m_strOutgoingNextContentTemplate.IsEmpty()) )
		{
			m_bTemplateElementsLoaded = TRUE;
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_bTemplateElementsLoaded = [%d]", m_bTemplateElementsLoaded);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return m_bTemplateElementsLoaded;
}


BOOL CTemplateDocumentBuilder::InitializeReplacementTags()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_bReplacingTagsLoaded == [%d].", m_bReplacingTagsLoaded);
	
	if (m_bReplacingTagsLoaded)
	{
		return m_bReplacingTagsLoaded;
	}
	
	// Initialize the constant elements.
	//
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(
			CUIMApplication::GetApplication())->GetStringLoader();
	
	ATLASSERT(pStringLoader);
	
	if (pStringLoader)
	{
		m_messageTemplateAditionPoint = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_ADITION_POINT");
		
		m_messageTemplateReplaceInfoTag = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_REPLACE_INFO_TAG");
		
		m_messageTemplateAppendingInfoTag = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_APPENDING_INFO_TAG");
		
		m_strTemplateTagChatName = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_CHAT_NAME");
		
		m_strTemplateTagSourceName = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_SOURCE_NAME");
		
		m_strTemplateTagDestinationName = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_DESTINATION_NAME");
		
		m_strTemplateTagDestinationDisplayName = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_DESTINATION_DISPLAY_NAME");
		
		m_strTemplateTagIncomingIconPath = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_INCOMING_ICON_PATH");
		
		m_strTemplateTagOutgoingIconPath = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_OUTGOING_ICON_PATH");
		
		m_strTemplateTagTimeOpened = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_TIME_OPENED");
		
		m_strTemplateTagUserIconPath = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_USER_ICON_PATH");
		
		m_strTemplateTagSenderScreenName = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_SENDER_SCREEN_NAME");
		
		m_strTemplateTagSender = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_SENDER");
		
		m_strTemplateTagService = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_SERVICE");
		
		m_strTemplateTagMessage = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_MESSAGE");
		
		m_strTemplateTagTime = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_TIME");
		
		m_strTemplateTagFileIconPath = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_FILE_ICON_PATH");
		
		m_strTemplateTagFileName = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_FILE_NAME");
		
		m_strTemplateTagSaveFileHandler = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_SAVE_FILE_HANDLER");
		
		m_strTemplateTagSaveAsFileHandler = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_SAVE_AS_FILE_HANDLER");
		
		m_strTemplateTagCancelRequestHandler = pStringLoader->GetValue(
				"uol.resource.IDS_MESSAGE_TEMPLATE_TAG_CANCEL_REQUEST_HANDLER");
		
		// Load file transfer formating strings too.
		//
		m_fileTransferStatusMessageFormat = pStringLoader->GetValue(
				"uol.resource.IDS_HISTORY_FILE_TRANSFER_STATUS_MESSAGE_FORMAT");
		
		m_fileTransferSaveFormat = pStringLoader->GetValue(
				"uol.resource.IDS_HISTORY_FILE_TRANSFER_SAVE_FORMAT");
		
		m_fileTransferSaveAsFormat = pStringLoader->GetValue(
				"uol.resource.IDS_HISTORY_FILE_TRANSFER_SAVE_AS_FORMAT");
		
		m_fileTransferCancelFormat = pStringLoader->GetValue(
				"uol.resource.IDS_HISTORY_FILE_TRANSFER_CANCEL_FORMAT");
		
		// File transfer alternate messages formating too.
		//
		m_strFileTransferAcceptedMessageFormat = pStringLoader->GetValue(
				"uol.resource.IDS_HISTORY_FILE_TRANSFER_ACEPTED_MESSAGE_FORMAT");
		
		m_strFileTransferRejectedMessageFormat = pStringLoader->GetValue(
				"uol.resource.IDS_HISTORY_FILE_TRANSFER_REFUSED_MESSAGE_FORMAT");
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Replacing tags -> m_messageTemplateAditionPoint = [%s],  "
			"m_messageTemplateReplaceInfoTag = [%s], m_strTemplateTagChatName = [%s], "
			"m_strTemplateTagSourceName = [%s], m_strTemplateTagDestinationName = [%s], "
			"m_strTemplateTagDestinationDisplayName = [%s], m_strTemplateTagIncomingIconPath = [%s], "
			"m_strTemplateTagOutgoingIconPath = [%s], "
			"m_strTemplateTagTimeOpened = [%s], m_strTemplateTagUserIconPath = [%s], "
			"m_strTemplateTagSenderScreenName = [%s], m_strTemplateTagSender = [%s], "
			"m_strTemplateTagService = [%s], m_strTemplateTagMessage = [%s], "
			"m_strTemplateTagTime = [%s], m_strTemplateTagFileIconPath = [%s], "
			"m_strTemplateTagFileName = [%s], m_strTemplateTagSaveFileHandler = [%s], "
			"m_strTemplateTagSaveAsFileHandler = [%s], m_strTemplateTagCancelRequestHandler = [%s], "
			"m_fileTransferStatusMessageFormat = [%s], m_fileTransferSaveFormat = [%s], "
			"m_fileTransferSaveAsFormat = [%s], m_fileTransferCancelFormat = [%s], "
			"m_strFileTransferAcceptedMessageFormat = [%s], m_strFileTransferRejectedMessageFormat = [%s].", 
			m_messageTemplateAditionPoint, m_messageTemplateReplaceInfoTag, 
			m_strTemplateTagChatName, m_strTemplateTagSourceName, 
			m_strTemplateTagDestinationName, m_strTemplateTagDestinationDisplayName, 
			m_strTemplateTagIncomingIconPath, m_strTemplateTagOutgoingIconPath, 
			m_strTemplateTagTimeOpened, m_strTemplateTagUserIconPath, 
			m_strTemplateTagSenderScreenName, m_strTemplateTagSender, 
			m_strTemplateTagService, m_strTemplateTagMessage, 
			m_strTemplateTagTime, m_strTemplateTagFileIconPath, 
			m_strTemplateTagFileName, m_strTemplateTagSaveFileHandler, 
			m_strTemplateTagSaveAsFileHandler, m_strTemplateTagCancelRequestHandler, 
			m_fileTransferStatusMessageFormat, m_fileTransferSaveFormat, 
			m_fileTransferSaveAsFormat, m_fileTransferCancelFormat, 
			m_strFileTransferAcceptedMessageFormat, m_strFileTransferRejectedMessageFormat);
	
	if ( (!m_messageTemplateAditionPoint.IsEmpty()) && 
			(!m_messageTemplateReplaceInfoTag.IsEmpty()) && 
			(!m_strTemplateTagChatName.IsEmpty()) && 
			(!m_strTemplateTagSourceName.IsEmpty()) && 
			(!m_strTemplateTagDestinationName.IsEmpty()) && 
			(!m_strTemplateTagDestinationDisplayName.IsEmpty()) && 
			(!m_strTemplateTagIncomingIconPath.IsEmpty()) && 
			(!m_strTemplateTagOutgoingIconPath.IsEmpty()) && 
			(!m_strTemplateTagTimeOpened.IsEmpty()) && 
			(!m_strTemplateTagUserIconPath.IsEmpty()) && 
			(!m_strTemplateTagSenderScreenName.IsEmpty()) && 
			(!m_strTemplateTagSender.IsEmpty()) && 
			(!m_strTemplateTagService.IsEmpty()) && 
			(!m_strTemplateTagMessage.IsEmpty()) && 
			(!m_strTemplateTagTime.IsEmpty()) && 
			(!m_strTemplateTagFileIconPath.IsEmpty()) && 
			(!m_strTemplateTagFileName.IsEmpty()) && 
			(!m_strTemplateTagSaveFileHandler.IsEmpty()) && 
			(!m_strTemplateTagSaveAsFileHandler.IsEmpty()) && 
			(!m_strTemplateTagCancelRequestHandler.IsEmpty()) && 
			(!m_fileTransferStatusMessageFormat.IsEmpty()) && 
			(!m_fileTransferSaveFormat.IsEmpty()) && 
			(!m_fileTransferSaveAsFormat.IsEmpty()) && 
			(!m_fileTransferCancelFormat.IsEmpty()) && 
			(!m_strFileTransferAcceptedMessageFormat.IsEmpty()) && 
			(!m_strFileTransferRejectedMessageFormat.IsEmpty()) )
	{
		m_bReplacingTagsLoaded = TRUE;
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_bTemplateElementsLoaded = [%d]", m_bTemplateElementsLoaded);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return m_bReplacingTagsLoaded;
}


CString CTemplateDocumentBuilder::GetReplacingTagInfo(CString strMainTemplate)
{
	CString strReplaceInfo;
	int iTagStart = 0;
	int iTagEnd = 0;
	
	if ( (!strMainTemplate.IsEmpty()) && (!m_messageTemplateReplaceInfoTag.IsEmpty()) )
	{
		iTagStart = strMainTemplate.Find(m_messageTemplateReplaceInfoTag);
		
		if (iTagStart)
		{
			iTagStart += m_messageTemplateReplaceInfoTag.GetLength() + 1;
			
			iTagEnd = strMainTemplate.Find("\"", iTagStart);
			
			if (iTagEnd)
			{
				strReplaceInfo = strMainTemplate.Mid(iTagStart, iTagEnd - iTagStart);
			}
		}
	}
	
	return strReplaceInfo;
}


IUOLMessengerMessageTemplateManagerPtr CTemplateDocumentBuilder::GetTemplateManager()
{
	return CUIMApplication::GetApplication()->GetMessageTemplateManager();
}


void CTemplateDocumentBuilder::AppendMessageTemplateRecordToDocument(
		IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord)
{
	CString strMessageBlock;
	
	int nLastRecordType = IUOLMessengerConversation::MT_SYSTEM;
	CString strLastAuthorId;
	
	if (m_pLastMessageTemplateRecord != NULL)
	{
		nLastRecordType = m_pLastMessageTemplateRecord->GetRecordType();
		strLastAuthorId = m_pLastMessageTemplateRecord->GetAuthorId();
	}
	
	if ( (pMessageTemplateRecord->GetRecordType() == IUOLMessengerConversation::MT_SYSTEM) && 
			(ProcessSystemMessage(pMessageTemplateRecord)) )
	{
		m_pLastMessageTemplateRecord = pMessageTemplateRecord;
	}
	else if ( (pMessageTemplateRecord->GetRecordType() == IUOLMessengerConversation::MT_FILE_TRANSFER) && 
			(ProcessFileTransferMessage(pMessageTemplateRecord)) )
	{
		m_pLastMessageTemplateRecord = pMessageTemplateRecord;
	}
	else
	{
		if ( (nLastRecordType == IUOLMessengerConversation::MT_SYSTEM) || 
				(nLastRecordType == IUOLMessengerConversation::MT_FILE_TRANSFER) || 
				(nLastRecordType != pMessageTemplateRecord->GetRecordType()) || 
				(0 != strLastAuthorId.Compare(pMessageTemplateRecord->GetAuthorId())) )
		{
			if (ProcessNewMessageBlock(pMessageTemplateRecord))
			{
				m_pLastMessageTemplateRecord = pMessageTemplateRecord;
			}
		}
		else
		{
			ProcessMessageBlockAppend(pMessageTemplateRecord);
		}
	}
}


BOOL CTemplateDocumentBuilder::ProcessSystemMessage(
		IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord)
{
	BOOL bProcessed = FALSE;
	
	if (!m_pLastMessageTemplateRecord)
	{
		m_lastMessageBlock = ReplaceTagsOnTemplate(
				IUOLMessengerMessageTemplate::TEMPLATE_STATUS, pMessageTemplateRecord);
		
		bProcessed = !m_lastMessageBlock.IsEmpty();
	}
	else
	{
		int iLastRecordType = m_pLastMessageTemplateRecord->GetRecordType();
		
		if ( (iLastRecordType == IUOLMessengerConversation::MT_SYSTEM) && 
				(!m_strNextStatusTemplate.IsEmpty()) && (!m_lastMessageBlock.IsEmpty()) )
		{
			bProcessed = AppendToLastBlock(ReplaceTagsOnTemplate(
					IUOLMessengerMessageTemplate::TEMPLATE_NEXT_STATUS, pMessageTemplateRecord));
		}
		else
		{
			if (!m_lastMessageBlock.IsEmpty())
			{
				m_templateDocumentMessages += m_lastMessageBlock;
			}
			
			m_lastMessageBlock = ReplaceTagsOnTemplate(
					IUOLMessengerMessageTemplate::TEMPLATE_STATUS, pMessageTemplateRecord);
			
			bProcessed = !m_lastMessageBlock.IsEmpty();
		}
	}
	
	return bProcessed;
}


BOOL CTemplateDocumentBuilder::ProcessFileTransferMessage(
		IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord)
{
	BOOL bProcessed = FALSE;
	
	if (!m_lastMessageBlock.IsEmpty())
	{
		m_templateDocumentMessages += m_lastMessageBlock;
	}
	
	CMessageTemplateRecordPtr pFileTransferRecord = 
			ap_dynamic_cast<CMessageTemplateRecordPtr>(pMessageTemplateRecord);
	
	// If we can't make the cast, let's it work as usually.
	//
	if ( (pFileTransferRecord == NULL) || 
			(pFileTransferRecord->GetRequestFileTransferAsyncInfo()->IsWaitingDecision()) )
	{
		if (!m_strIncomingFileTransferTemplate.IsEmpty())
		{
			m_lastMessageBlock = ReplaceTagsOnTemplate(
					IUOLMessengerMessageTemplate::TEMPLATE_INCOMING_FILE_TRANSFER, 
					pMessageTemplateRecord);
		}
		else
		{
			// Especific file transfer template was not provided.
			// Built one using STATUS block.
			//
			CString strTemplateUpdated = m_strStatusTemplate;
			
			if ( (!strTemplateUpdated.IsEmpty()) && 
					(!m_fileTransferStatusMessageFormat.IsEmpty()) )
			{
				CString newMessageValue = m_fileTransferStatusMessageFormat;
				
				if ( (CHTMLUtilitary::Replace(newMessageValue, "%s", pMessageTemplateRecord->GetContactAlias())) && 
						(CHTMLUtilitary::Replace(newMessageValue, "%s", pMessageTemplateRecord->GetFileTransferName())) )
				{
					strTemplateUpdated.Replace(m_strTemplateTagMessage, newMessageValue);
					
					ReplaceTagsOnTemplate(strTemplateUpdated, pMessageTemplateRecord, FALSE, FALSE);
					
					m_lastMessageBlock = strTemplateUpdated;
				}
			}
		}
	}
	else if (pFileTransferRecord)
	{
		CString strFileTransferMessage;
		CString strStatusMessageBlock;
		
		// For now we will only consider the user options when accepting 
		// or refusing the file transfer initiation.
		// Canceling during file transfer will be handled on future.
		//
		
		// First we build the message used on the place of "%message%" 
		// replacing tag.
		//
		if (pFileTransferRecord->GetRequestFileTransferAsyncInfo()->WasAccepted())
		{
			strFileTransferMessage.Format(
					m_strFileTransferAcceptedMessageFormat, 
					pMessageTemplateRecord->GetFileTransferName());
		}
		else if (pFileTransferRecord->GetRequestFileTransferAsyncInfo()->WasRejected())
		{
			strFileTransferMessage.Format(
					m_strFileTransferRejectedMessageFormat, 
					pMessageTemplateRecord->GetFileTransferName());
		}
		
		// Now we get the Status template that will be used to 
		// present the file transfer message on the document.
		//
		strStatusMessageBlock = m_strStatusTemplate;
		
		strStatusMessageBlock.Replace(m_strTemplateTagMessage, 
				strFileTransferMessage);
		
		// Call the replacement routine to update the other 
		// replacing tags.
		//
		ReplaceTagsOnTemplate(strStatusMessageBlock, pMessageTemplateRecord, FALSE, FALSE);
		
		m_lastMessageBlock = strStatusMessageBlock;
	}
	
	bProcessed = !m_lastMessageBlock.IsEmpty();
	
	return bProcessed;
}


BOOL CTemplateDocumentBuilder::ProcessNewMessageBlock(
		IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord)
{
	BOOL bProcessed = FALSE;
	BOOL bIsFromUser = (pMessageTemplateRecord->GetRecordType() == IUOLMessengerConversation::MT_ACCOUNT_USER);
	
	if (!m_lastMessageBlock.IsEmpty())
	{
		m_templateDocumentMessages += m_lastMessageBlock;
	}
	
	if (bIsFromUser)
	{
		m_lastMessageBlock = ReplaceTagsOnTemplate(
				IUOLMessengerMessageTemplate::TEMPLATE_OUTGOING_CONTENT, pMessageTemplateRecord);
	}
	else
	{
		m_lastMessageBlock = ReplaceTagsOnTemplate(
				IUOLMessengerMessageTemplate::TEMPLATE_INCOMING_CONTENT, pMessageTemplateRecord);
	}
	
	bProcessed = !m_lastMessageBlock.IsEmpty();
	
	return bProcessed;
}


BOOL CTemplateDocumentBuilder::ProcessMessageBlockAppend(
		IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord)
{
	BOOL bProcessed = FALSE;
	BOOL bIsFromUser = (pMessageTemplateRecord->GetRecordType() == IUOLMessengerConversation::MT_ACCOUNT_USER);
	
	if (bIsFromUser)
	{
		bProcessed = AppendToLastBlock(ReplaceTagsOnTemplate(
				IUOLMessengerMessageTemplate::TEMPLATE_OUTGOING_NEXT_CONTENT, pMessageTemplateRecord));
	}
	else
	{
		bProcessed = AppendToLastBlock(ReplaceTagsOnTemplate(
				IUOLMessengerMessageTemplate::TEMPLATE_INCOMING_NEXT_CONTENT, pMessageTemplateRecord));
	}
	
	return bProcessed;
}


CString CTemplateDocumentBuilder::ReplaceTagsOnTemplate(
		IUOLMessengerMessageTemplate::MessageTemplateElementId nElementId, 
		IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord)
{
	CString strTemplateUpdated;
	BOOL bIsIncomingMessage = FALSE;
	
	// First let's get the desired template.
	//
	switch (nElementId)
	{
		case IUOLMessengerMessageTemplate::TEMPLATE_HEADER:
			strTemplateUpdated = m_strHeaderTemplate;
		break;
		
		case IUOLMessengerMessageTemplate::TEMPLATE_FOOTER:
			strTemplateUpdated = m_strFooterTemplate;
		break;
		
		case IUOLMessengerMessageTemplate::TEMPLATE_STATUS:
			strTemplateUpdated = m_strStatusTemplate;
		break;
		
		case IUOLMessengerMessageTemplate::TEMPLATE_NEXT_STATUS:
			strTemplateUpdated = m_strNextStatusTemplate;
		break;
		
		case IUOLMessengerMessageTemplate::TEMPLATE_INCOMING_CONTENT:
			strTemplateUpdated = m_strIncomingContentTemplate;
			bIsIncomingMessage = TRUE;
		break;
		
		case IUOLMessengerMessageTemplate::TEMPLATE_INCOMING_NEXT_CONTENT:
			strTemplateUpdated = m_strIncomingNextContentTemplate;
			bIsIncomingMessage = TRUE;
		break;
		
		case IUOLMessengerMessageTemplate::TEMPLATE_OUTGOING_CONTENT:
			strTemplateUpdated = m_strOutgoingContentTemplate;
		break;
		
		case IUOLMessengerMessageTemplate::TEMPLATE_OUTGOING_NEXT_CONTENT:
			strTemplateUpdated = m_strOutgoingNextContentTemplate;
		break;
		
		case IUOLMessengerMessageTemplate::TEMPLATE_INCOMING_FILE_TRANSFER:
			strTemplateUpdated = m_strIncomingFileTransferTemplate;
		break;
		
		default:
		break;
	};
	
	// Now we replace the existent tags with the correct information.
	//
	switch (nElementId)
	{
		case IUOLMessengerMessageTemplate::TEMPLATE_HEADER:
		case IUOLMessengerMessageTemplate::TEMPLATE_FOOTER:
			
			ReplaceTagsOnTemplate(strTemplateUpdated, pMessageTemplateRecord, TRUE, bIsIncomingMessage);
			
		break;
		
		case IUOLMessengerMessageTemplate::TEMPLATE_STATUS:
		case IUOLMessengerMessageTemplate::TEMPLATE_NEXT_STATUS:
		case IUOLMessengerMessageTemplate::TEMPLATE_INCOMING_CONTENT:
		case IUOLMessengerMessageTemplate::TEMPLATE_INCOMING_NEXT_CONTENT:
		case IUOLMessengerMessageTemplate::TEMPLATE_OUTGOING_CONTENT:
		case IUOLMessengerMessageTemplate::TEMPLATE_OUTGOING_NEXT_CONTENT:
		case IUOLMessengerMessageTemplate::TEMPLATE_INCOMING_FILE_TRANSFER:
			
			ReplaceTagsOnTemplate(strTemplateUpdated, pMessageTemplateRecord, FALSE, bIsIncomingMessage);
			
		break;
		
		default:
		break;
	};
	
	return strTemplateUpdated;
}


void CTemplateDocumentBuilder::ReplaceTagsOnTemplate(CString& strTemplateUpdated, 
		IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord, 
		BOOL bIsHeaderOrFooter, BOOL bIsIncomingMessage)
{
	CString strSaveFileHandler;
	CString strSaveFileAsHandler;
	CString strCancelFileHandler;
	
	if (bIsHeaderOrFooter)
	{
		// We don't consider chat name element, but put some 
		// usefull information.
		strTemplateUpdated.Replace(
				m_strTemplateTagChatName, 
				pMessageTemplateRecord->GetContactAlias());
		
		strTemplateUpdated.Replace(
				m_strTemplateTagSourceName, 
				pMessageTemplateRecord->GetAccountNickName());
		
		strTemplateUpdated.Replace(
				m_strTemplateTagDestinationName, 
				pMessageTemplateRecord->GetContactAlias());
		
		strTemplateUpdated.Replace(
				m_strTemplateTagDestinationDisplayName, 
				pMessageTemplateRecord->GetContactAlias());
		
		strTemplateUpdated.Replace(
				m_strTemplateTagIncomingIconPath, 
				pMessageTemplateRecord->GetContactIconPath());
		
		strTemplateUpdated.Replace(
				m_strTemplateTagOutgoingIconPath, 
				pMessageTemplateRecord->GetAccountIconPath());
		
		if (strTemplateUpdated.Find(m_strTemplateTagTimeOpened) != -1)
		{
			CString strTimeEntireTag;
			CString strReplacingTime;
			
			if (GenerateTimeInfo(pMessageTemplateRecord,
					strTemplateUpdated, m_strTemplateTagTimeOpened, 
					strTimeEntireTag, strReplacingTime))
			{
				strTemplateUpdated.Replace(
						strTimeEntireTag, strReplacingTime);
			}
		}
	}
	else
	{
		strTemplateUpdated.Replace(
				m_strTemplateTagUserIconPath, 
				pMessageTemplateRecord->GetAuthorIconPath());
		
		strTemplateUpdated.Replace(
				m_strTemplateTagSenderScreenName, 
				pMessageTemplateRecord->GetAuthor());
		
		strTemplateUpdated.Replace(
				m_strTemplateTagSender, 
				pMessageTemplateRecord->GetAuthorId());
		
		strTemplateUpdated.Replace(
				m_strTemplateTagService, 
				pMessageTemplateRecord->GetService());
		
		strTemplateUpdated.Replace(
				m_strTemplateTagMessage, 
				pMessageTemplateRecord->GetMessageParsed());
		
		// We don't consider file icon path element.
		strTemplateUpdated.Replace(
				m_strTemplateTagFileIconPath, "");
		
		strTemplateUpdated.Replace(
				m_strTemplateTagFileName, 
				pMessageTemplateRecord->GetFileTransferName());
		
		strSaveFileHandler.Format(m_fileTransferSaveFormat, 
				pMessageTemplateRecord->GetFileTransferId());
		
		strTemplateUpdated.Replace(
				m_strTemplateTagSaveFileHandler, 
				strSaveFileHandler);
		
		strSaveFileAsHandler.Format(m_fileTransferSaveAsFormat, 
				pMessageTemplateRecord->GetFileTransferId());
		
		strTemplateUpdated.Replace(
				m_strTemplateTagSaveAsFileHandler, 
				strSaveFileAsHandler);
		
		strCancelFileHandler.Format(m_fileTransferCancelFormat, 
				pMessageTemplateRecord->GetFileTransferId());
		
		strTemplateUpdated.Replace(
				m_strTemplateTagCancelRequestHandler, 
				strCancelFileHandler);
		
		if (strTemplateUpdated.Find(m_strTemplateTagTime) != -1)
		{
			CString strTimeEntireTag;
			CString strReplacingTime;
			
			if (GenerateTimeInfo(pMessageTemplateRecord,
					strTemplateUpdated, m_strTemplateTagTime, 
					strTimeEntireTag, strReplacingTime))
			{
				strTemplateUpdated.Replace(
						strTimeEntireTag, strReplacingTime);
			}
		}
	}
}


BOOL CTemplateDocumentBuilder::AppendToLastBlock(CString strAppendingBlock)
{
	BOOL bAppended = FALSE;
	
	int iAppendingTagStart = 0;
	int iAppendingTagEnd = 0;
	
	// Find appending tag.
	iAppendingTagStart = m_lastMessageBlock.Find(m_messageTemplateAppendingInfoTag);
	
	// Maybe the "insert" element is not between double quotation, 
	// but just single one.
	if (iAppendingTagStart == -1)
	{
		CString strSingleQuotingAppendingInfoTag = m_messageTemplateAppendingInfoTag;
		
		strSingleQuotingAppendingInfoTag.Replace("\"", "\'");
		
		iAppendingTagStart = m_lastMessageBlock.Find(strSingleQuotingAppendingInfoTag);
	}
	
	// Not found neither?... give up...
	if (iAppendingTagStart != -1)
	{
		// Get to this tag begin.
		while (m_lastMessageBlock[iAppendingTagStart] != '<')
		{
			iAppendingTagStart--;
		}
		
		// Find appending tag end.
		iAppendingTagEnd = m_lastMessageBlock.Find(">", iAppendingTagStart);
		iAppendingTagEnd++;
		
		// Find appending tag close.
		iAppendingTagEnd = m_lastMessageBlock.Find(">", iAppendingTagEnd);
		iAppendingTagEnd++;
		
		if ( (iAppendingTagStart > 0) && (iAppendingTagEnd > 0) )
		{
			m_lastMessageBlock.Replace(
					m_lastMessageBlock.Mid(iAppendingTagStart, iAppendingTagEnd - iAppendingTagStart), 
					strAppendingBlock);
			
			bAppended = TRUE;
		}
	}
	
	return bAppended;
}


BOOL CTemplateDocumentBuilder::GenerateTimeInfo(
	IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord,
	const CString& strTemplate, const CString& strTimeInitialTag, 
	CString& strTimeCompleteTag, CString& strReplacingTime)
{
	BOOL bGenerated = FALSE;
	int iTimeTagStart = 0;
	int iTimeTagEnd = 0;
	CString strTimeFormat;
	int iTimeFormatStart = 0;
	int iTimeFormatEnd = 0;
	
	iTimeTagStart = strTemplate.Find(strTimeInitialTag);
	
	iTimeTagEnd = iTimeTagStart + strTimeInitialTag.GetLength();
	
	CTime timeEvent = pMessageTemplateRecord->GetTime();
	
	if (strTemplate[iTimeTagEnd] == '%')
	{
		iTimeTagEnd++;
		strTimeCompleteTag = strTemplate.Mid(iTimeTagStart, iTimeTagEnd - iTimeTagStart);
		strReplacingTime = timeEvent.Format("%H:%M");
		
		bGenerated = TRUE;
	}
	else if (strTemplate[iTimeTagEnd] == '{')
	{
		iTimeFormatStart = iTimeTagEnd + 1;
		iTimeFormatEnd = strTemplate.Find("}", iTimeFormatEnd);
		
		strTimeFormat = strTemplate.Mid(iTimeFormatStart, iTimeFormatEnd - iTimeFormatStart);
		iTimeTagEnd = iTimeFormatEnd + 2;
		strTimeCompleteTag = strTemplate.Mid(iTimeTagStart, iTimeTagEnd - iTimeTagStart);
		
		strReplacingTime = timeEvent.Format(strTimeFormat);
		
		bGenerated = TRUE;
	}
	
	return bGenerated;
}


CString CTemplateDocumentBuilder::RebuildDocument(BOOL bNeedReloadTemplateElements)
{
	CString strUpdatedDocument;
	
	if (bNeedReloadTemplateElements)
	{
		m_bInitialized = FALSE;
		m_bTemplateElementsLoaded = FALSE;
	}
	else
	{
		m_templateDocumentMessages.Empty();
		m_lastMessageBlock.Empty();
	}
	
	m_pLastMessageTemplateRecord = IUOLMessengerMessageTemplateRecordPtr();
	
	if ( (LoadTemplateElements()) && (InitializeMainBlocks()) )
	{
		for(POSITION recordPos = m_MessageTemplateRecordList.GetHeadPosition(); recordPos; )
		{
			AppendMessageTemplateRecordToDocument(m_MessageTemplateRecordList.GetNext(recordPos));
		}
		
		strUpdatedDocument.Format("%s%s%s%s", m_templateDocumentTop, 
				m_templateDocumentMessages, m_lastMessageBlock, 
				m_templateDocumentBottom);
	}
	
	return strUpdatedDocument;
}

