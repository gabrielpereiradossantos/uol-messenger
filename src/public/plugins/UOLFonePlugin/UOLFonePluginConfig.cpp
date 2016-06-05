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
#include "resource.h"
#include ".\UOLFonePluginConfig.h"
#include ".\FacadeWrapper.h"


#define		CALL_ACCOUNT_TAG_NAME				"CallAccount"
#define		AUDIO_DEVICES_TAG_NAME				"AudioDevices"
#define		LINE_IN_TAG_NAME					"LineIn"
#define		LINE_OUT_TAG_NAME					"LineOut"
#define		CALL_HISTORY_TAG_NAME				"CallHistory"
#define		PHONE_CALL_NUMBER_TAG_NAME			"PhoneCallNumber"
#define		FORCE_TUNNEL_TAG_NAME				"ForceTunnel"
#define		PURGE_INTERVAL_TAG_NAME				"PurgeInterval"
#define		LAST_PURGE_TIME_TAG_NAME			"LastPurgeTime"
#define		REGISTER_ONLY_ON_CALL				"RegisterOnlyOnCall"
#define		SOUND_EVENTS_TAG_NAME				"SoundEvents"
#define		SOUND_EVENT_TAG_NAME				"SoundEvent"
#define		SOUND_EVENT_TYPE_TAG_NAME			"EventType"
#define		SOUND_EVENT_DESCRIPTION_TAG_NAME	"EventDescription"
#define		SOUND_EVENT_ENABLED_TAG_NAME		"EventEnabled"
#define		SOUND_EVENT_FILE_PATH_TAG_NAME		"FilePath"

#define		FORCE_TUNNEL_INIT_VALUE				"0"
#define		DEFAULT_PURGE_INTERVAL_VALUE		"5"
#define		MINIMMUM_PURGE_INTERVAL_VALUE		1
#define		REGISTER_ONLY_ON_CALL_INIT_VALUE	"0"

#define		PLUGIN_PROPERTY_LINE_IN					"Entrada de Áudio"
#define		PLUGIN_PROPERTY_LINE_OUT				"Saída de Áudio"
#define		PLUGIN_PROPERTY_FORCE_TUNNEL			"Conexão Via Porta 80"
#define		PLUGIN_PROPERTY_REGISTER_ONLY_ON_CALL	"Registro Somente Na Chamada"



CUOLFonePluginConfig::CUOLFonePluginConfig(void) : 
		m_initialized(FALSE), 
		m_callAccountNode(NULL), 
		m_lineInNode(NULL), 
		m_lineOutNode(NULL), 
		m_callHistoryNode(NULL), 
		m_forceTunnelNode(NULL),
		m_soundEventsNode(NULL),
		m_configLoaded(FALSE)
{
	if ( (!m_initialized) && Initialize() )
	{
		m_initialized = TRUE;
		
		PublishPluginProperties();
	}
}


CUOLFonePluginConfig::~CUOLFonePluginConfig(void)
{
	if (m_configLoaded)
	{
		SaveUOLFoneConfig();
	}
	
	if (m_callAccountNode != NULL)
	{
		delete m_callAccountNode;
		m_callAccountNode = NULL;
	}
	
	if (m_lineInNode != NULL)
	{
		delete m_lineInNode;
		m_lineInNode = NULL;
	}
	
	if (m_lineOutNode != NULL)
	{
		delete m_lineOutNode;
		m_lineOutNode = NULL;
	}
	
	if (m_callHistoryNode != NULL)
	{
		delete m_callHistoryNode;
		m_callHistoryNode = NULL;
	}

	if (m_soundEventsNode != NULL)
	{
		delete m_soundEventsNode;
		m_soundEventsNode = NULL;
	}

	if (m_forceTunnelNode != NULL)
	{
		delete m_forceTunnelNode;
		m_forceTunnelNode = NULL;
	}

	if (m_purgeIntervalNode != NULL)
	{
		delete m_purgeIntervalNode;
		m_purgeIntervalNode = NULL;
	}

	if (m_registerOnlyOnCall != NULL)
	{
		delete m_registerOnlyOnCall;
		m_registerOnlyOnCall = NULL;
	}
	
	m_historyPhoneNumbers.RemoveAll();
	m_soundEvents.RemoveAll();
}


BOOL CUOLFonePluginConfig::Initialize()
{
	BOOL result = FALSE;
	CString currentUserProfilePath;
	CString configFileStub;
	CXmlNodeWrapper* pAudioDevicesNode = NULL;
	CXmlNodeWrapper* pHistoryNode = NULL;
	CXmlNodeListWrapper* pPhoneCallNumbersList = NULL;
	MSXML2::IXMLDOMNode* pPhoneCallNumber = NULL;
	CXmlNodeListWrapper* pSoundEventsList = NULL;
	MSXML2::IXMLDOMNode* pSoundEventsDOMNode = NULL;
	
	if (CFacadeWrapper::GetInstance()->GetCurrentUserProfilePath(currentUserProfilePath))
	{
		m_uolfoneConfigFilePath.Format("%s%s", currentUserProfilePath, 
				"\\UOLFonePluginConfig.xml");
		
		if (PathFileExists(m_uolfoneConfigFilePath))
		{
			result = m_uolfonePluginConfigDoc.Load(m_uolfoneConfigFilePath);
		}
		else if ((result = CFacadeWrapper::GetInstance()->LoadConfigFileStub(
				MAKEINTRESOURCE(IDR_XML_CONFIG_STUB), configFileStub)) == TRUE)
		{
			if (m_uolfonePluginConfigDoc.LoadXML(configFileStub))
			{
				result = m_uolfonePluginConfigDoc.Save(m_uolfoneConfigFilePath);
			}
		}
		
		if (result)
		{
			m_configLoaded = TRUE;
			
			CXmlNodeWrapper sourceNode(m_uolfonePluginConfigDoc.AsNode());
			
			m_callAccountNode = new CXmlNodeWrapper(
					sourceNode.GetNode(CALL_ACCOUNT_TAG_NAME));
			
			pAudioDevicesNode = new CXmlNodeWrapper(
					sourceNode.GetNode(AUDIO_DEVICES_TAG_NAME));
			
			if (pAudioDevicesNode != NULL)
			{
				m_lineInNode = new CXmlNodeWrapper(
						pAudioDevicesNode->GetNode(LINE_IN_TAG_NAME));
				
				m_lineOutNode = new CXmlNodeWrapper(
						pAudioDevicesNode->GetNode(LINE_OUT_TAG_NAME));
			}
			
			m_callHistoryNode = new CXmlNodeWrapper(
					sourceNode.GetNode(CALL_HISTORY_TAG_NAME));
			
			if (m_callHistoryNode != NULL)
			{
				pPhoneCallNumbersList = new CXmlNodeListWrapper(
						m_callHistoryNode->FindNodes(PHONE_CALL_NUMBER_TAG_NAME));
				
				if ( (pPhoneCallNumbersList != NULL) && (pPhoneCallNumbersList->Count() > 0) )
				{
					pPhoneCallNumbersList->Start();
					
					while ((pPhoneCallNumber = pPhoneCallNumbersList->Next()) != NULL)
					{
						CXmlNodeWrapper phoneCallNumber(pPhoneCallNumber);
						
						m_historyPhoneNumbers.AddHead(phoneCallNumber.GetText());
					}
				}
				
				delete pPhoneCallNumbersList;
				pPhoneCallNumbersList = NULL;
			}
			
			MSXML2::IXMLDOMNode* pXMLForceTunnelNode = 
					sourceNode.GetNode(FORCE_TUNNEL_TAG_NAME);
			
			if (pXMLForceTunnelNode)
			{
				m_forceTunnelNode = new CXmlNodeWrapper(pXMLForceTunnelNode);
			}
			else
			{
				m_forceTunnelNode = new CXmlNodeWrapper(
						sourceNode.InsertNode(0, FORCE_TUNNEL_TAG_NAME));
				
				m_forceTunnelNode->SetText(FORCE_TUNNEL_INIT_VALUE);
			}
			
			MSXML2::IXMLDOMNode* pXMLPurgeIntervalNode = 
					sourceNode.GetNode(PURGE_INTERVAL_TAG_NAME);
			
			if (pXMLPurgeIntervalNode)
			{
				m_purgeIntervalNode = new CXmlNodeWrapper(pXMLPurgeIntervalNode);
			}
			else
			{
				m_purgeIntervalNode = new CXmlNodeWrapper(
						sourceNode.InsertNode(0, PURGE_INTERVAL_TAG_NAME));
				
				m_purgeIntervalNode->SetText(DEFAULT_PURGE_INTERVAL_VALUE);
			}
			
			MSXML2::IXMLDOMNode* pXMLRegisterOnlyOnCallNode = 
					sourceNode.GetNode(REGISTER_ONLY_ON_CALL);
			
			if (pXMLRegisterOnlyOnCallNode)
			{
				m_registerOnlyOnCall = new CXmlNodeWrapper(pXMLRegisterOnlyOnCallNode);
			}
			else
			{
				m_registerOnlyOnCall = new CXmlNodeWrapper(
						sourceNode.InsertNode(0, REGISTER_ONLY_ON_CALL));
				
				m_registerOnlyOnCall->SetText(REGISTER_ONLY_ON_CALL_INIT_VALUE);
			}
			
			if ( (m_callAccountNode == NULL) || (pAudioDevicesNode == NULL) || 
					(m_callHistoryNode == NULL) || (m_forceTunnelNode == NULL) || 
					(m_purgeIntervalNode == NULL) || (m_registerOnlyOnCall == NULL) )
			{
				result = FALSE;
			}
			
			if (pAudioDevicesNode != NULL)
			{
				delete pAudioDevicesNode;
				pAudioDevicesNode = NULL;
			}

			// Sound Events List
			MSXML2::IXMLDOMNode* pXMLSoundEventsNode = 
					sourceNode.GetNode(SOUND_EVENTS_TAG_NAME);
			
			if (pXMLSoundEventsNode)
			{
				m_soundEventsNode = new CXmlNodeWrapper(pXMLSoundEventsNode);
			}
			else
			{
				m_soundEventsNode = new CXmlNodeWrapper(
						sourceNode.InsertNode(0, SOUND_EVENTS_TAG_NAME));
			}
			
			if (m_soundEventsNode != NULL)
			{
				pSoundEventsList = new CXmlNodeListWrapper(
						m_soundEventsNode->FindNodes(SOUND_EVENT_TAG_NAME));
				
				if ( (pSoundEventsList != NULL) && (pSoundEventsList->Count() > 0) )
				{
					pSoundEventsList->Start();
					
					while ((pSoundEventsDOMNode = pSoundEventsList->Next()) != NULL)
					{
						CXmlNodeWrapper soundEventNode(pSoundEventsDOMNode);
						
						CXmlNodeWrapper soundEventTypeNode(
							soundEventNode.GetNode(SOUND_EVENT_TYPE_TAG_NAME));
						CXmlNodeWrapper soundEventDescriptionNode(
							soundEventNode.GetNode(SOUND_EVENT_DESCRIPTION_TAG_NAME));
						CXmlNodeWrapper soundFilePathNode(
							soundEventNode.GetNode(SOUND_EVENT_FILE_PATH_TAG_NAME));
						CXmlNodeWrapper soundEventEnabledNode(
							soundEventNode.GetNode(SOUND_EVENT_ENABLED_TAG_NAME));

						CSoundEventPtr pSoundEvent = new CSoundEvent(soundEventTypeNode.GetText(),
							soundEventDescriptionNode.GetText(),
							soundFilePathNode.GetText(),
							soundEventEnabledNode.GetText().Compare("1") == 0);
						
						if (pSoundEvent)
						{
							m_soundEvents.AddHead(pSoundEvent);
						}
					}
				}
				
				delete pSoundEventsList;
				pSoundEventsList = NULL;

				// Poulate default sound events list for the first time
				if (m_soundEvents.GetCount() == 0)
				{
					SetDefaultSoundEvents();
				}
			}
		}
	}
	
	return result;
}


CUOLFonePluginConfig* CUOLFonePluginConfig::GetInstance()
{
	static CUOLFonePluginConfig configInstance;
	
	return &configInstance;
}


CString CUOLFonePluginConfig::GetCallAccount()
{
	if (!m_initialized)
	{
		return "";
	}
	else
	{
		return m_callAccountNode->GetText();
	}
}


CString CUOLFonePluginConfig::GetLineInDevice()
{
	if (!m_initialized)
	{
		return "";
	}
	else
	{
		return m_lineInNode->GetText();
	}
}


CString CUOLFonePluginConfig::GetLineOutDevice()
{
	if (!m_initialized)
	{
		return "";
	}
	else
	{
		return m_lineOutNode->GetText();
	}
}


BOOL CUOLFonePluginConfig::GetForceTunnel()
{
	if (!m_initialized)
	{
		return FALSE;
	}
	else
	{
		return (m_forceTunnelNode->GetText().Compare("1") == 0);
	}
}


int CUOLFonePluginConfig::GetPurgeInterval()
{
	if (!m_initialized)
	{
		return ::_tstoi(DEFAULT_PURGE_INTERVAL_VALUE);
	}
	else
	{
		return ::_tstoi(m_purgeIntervalNode->GetText());
	}
}


int CUOLFonePluginConfig::GetMinimmumPurgeInterval()
{
	return MINIMMUM_PURGE_INTERVAL_VALUE;
}


BOOL CUOLFonePluginConfig::RegisterOnlyOnCall()
{
	if (!m_initialized)
	{
		return FALSE;
	}
	else
	{
		return (m_registerOnlyOnCall->GetText().Compare("1") == 0);
	}
}


void CUOLFonePluginConfig::SetCallAccount(CString callAccount)
{
	if (!m_initialized)
	{
		return;
	}
	else if (!callAccount.IsEmpty())
	{
		m_callAccountNode->SetText(callAccount);
	}
}


void CUOLFonePluginConfig::SetLineInDevice(CString lineInDeviceName)
{
	if (!m_initialized)
	{
		return;
	}
	else if (!lineInDeviceName.IsEmpty())
	{
		m_lineInNode->SetText(lineInDeviceName);
	}
}


void CUOLFonePluginConfig::SetLineOutDevice(CString lineOutDeviceName)
{
	if (!m_initialized)
	{
		return;
	}
	else if (!lineOutDeviceName.IsEmpty())
	{
		m_lineOutNode->SetText(lineOutDeviceName);
	}
}


void CUOLFonePluginConfig::SetForceTunnel(BOOL bForceTunnel)
{
	if (!m_initialized)
	{
		return;
	}
	else
	{
		CString strForceTunnel;
		strForceTunnel.Format("%d", bForceTunnel);
		
		m_forceTunnelNode->SetText(strForceTunnel);
	}
}


void CUOLFonePluginConfig::SetPurgeInterval(int iPurgeInterval)
{
	if (!m_initialized)
	{
		return;
	}
	else
	{
		CString strPurgeInterval;
		strPurgeInterval.Format("%d", iPurgeInterval);
		
		m_purgeIntervalNode->SetText(strPurgeInterval);
	}
}


void CUOLFonePluginConfig::SetRegisterOnlyOnCall(BOOL bRegisterOnlyOnCall)
{
	if (!m_initialized)
	{
		return;
	}
	else
	{
		CString strRegisterOnlyOnCall;
		strRegisterOnlyOnCall.Format("%d", bRegisterOnlyOnCall);
		
		m_registerOnlyOnCall->SetText(strRegisterOnlyOnCall);
	}
}


void CUOLFonePluginConfig::GetCallHistory(CAtlList<CString>& historyPhoneNumbers)
{
	historyPhoneNumbers.AddHeadList(&m_historyPhoneNumbers);
}


void CUOLFonePluginConfig::AddCallNumberToHistory(CString callNumber)
{
	if (!m_initialized)
	{
		return;
	}
	else if ( !callNumber.IsEmpty() )
	{
		//Remove if exists
		POSITION pos = NULL;
		while ( pos = m_historyPhoneNumbers.Find(callNumber) )
		{
			m_historyPhoneNumbers.RemoveAt(pos);
		}
		
		//Add entry
		m_historyPhoneNumbers.AddTail(callNumber);
	
		//Limit the history
		while ( m_historyPhoneNumbers.GetCount() > CPhoneNumberComboBox::MAX_HISTORY_ITEMS )
		{
			m_historyPhoneNumbers.RemoveHead();
		}		
	}
}


void CUOLFonePluginConfig::ClearCallHistory()
{
	if (!m_initialized)
	{
		return;
	}
	else 
	{
		m_callHistoryNode->RemoveNodes(PHONE_CALL_NUMBER_TAG_NAME);
		m_historyPhoneNumbers.RemoveAll();
	}
}


void CUOLFonePluginConfig::SaveUOLFoneConfig()
{
	if (!m_initialized)
	{
		return;
	}
	else 
	{
		// Call History
		m_callHistoryNode->RemoveNodes(PHONE_CALL_NUMBER_TAG_NAME);

		POSITION pos = m_historyPhoneNumbers.GetHeadPosition();
		CString callNumber;
		while (pos)
		{
			callNumber = m_historyPhoneNumbers.GetNext(pos);

			MSXML2::IXMLDOMNodePtr pNewNode = 
				m_callHistoryNode->InsertNode(0, PHONE_CALL_NUMBER_TAG_NAME);

			if (pNewNode != NULL)
			{
				CXmlNodeWrapper newNodeWrapper(pNewNode);
				newNodeWrapper.SetText(callNumber);
			}
		}

		// Sound Events
		m_soundEventsNode->RemoveNodes(SOUND_EVENT_TAG_NAME);

		POSITION posSnd = m_soundEvents.GetHeadPosition();
		CSoundEventPtr pSoundEvent;
		while (posSnd)
		{
			pSoundEvent = m_soundEvents.GetNext(posSnd);

			MSXML2::IXMLDOMNodePtr pSoundEventDOMNode = 
				m_soundEventsNode->InsertNode(0, SOUND_EVENT_TAG_NAME);

			if (pSoundEventDOMNode != NULL)
			{
				CString strSoundEventEnabled;
				strSoundEventEnabled.Format("%d", pSoundEvent->IsEnabled());

				CXmlNodeWrapper soundEventNode(pSoundEventDOMNode);

				// Sound event type tag
				MSXML2::IXMLDOMNodePtr pSoundEventTypeDOMNode = 
				soundEventNode.InsertNode(0, SOUND_EVENT_TYPE_TAG_NAME);

				if (pSoundEventTypeDOMNode != NULL)
				{
					CXmlNodeWrapper soundEventTypeNode(pSoundEventTypeDOMNode);
					soundEventTypeNode.SetText(pSoundEvent->GetEventType());
				}

				// Sound event description tag
				MSXML2::IXMLDOMNodePtr pSoundEventDescriptionDOMNode = 
				soundEventNode.InsertNode(0, SOUND_EVENT_DESCRIPTION_TAG_NAME);

				if (pSoundEventDescriptionDOMNode != NULL)
				{
					CXmlNodeWrapper soundEventDescriptionNode(pSoundEventDescriptionDOMNode);
					soundEventDescriptionNode.SetText(pSoundEvent->GetDescription());
				}

				// Sound event enabled tag
				MSXML2::IXMLDOMNodePtr pSoundEventEnabledDOMNode = 
				soundEventNode.InsertNode(0, SOUND_EVENT_ENABLED_TAG_NAME);

				if (pSoundEventEnabledDOMNode != NULL)
				{
					CXmlNodeWrapper soundEventEnabledNode(pSoundEventEnabledDOMNode);
					soundEventEnabledNode.SetText(strSoundEventEnabled);
				}

				// Sound event file path tag
				MSXML2::IXMLDOMNodePtr pSoundEventFilePathDOMNode = 
				soundEventNode.InsertNode(0, SOUND_EVENT_FILE_PATH_TAG_NAME);

				if (pSoundEventFilePathDOMNode != NULL)
				{
					CXmlNodeWrapper soundFilePathNode(pSoundEventFilePathDOMNode);
					soundFilePathNode.SetText(pSoundEvent->GetFilePath());
				}	
			}
		}

		m_uolfonePluginConfigDoc.Save(m_uolfoneConfigFilePath);
		
		PublishPluginProperties();
	}
}

CSoundEventPtr CUOLFonePluginConfig::GetSoundEvent(const CString& strEventType)
{
	POSITION posSnd = m_soundEvents.GetHeadPosition();
	CSoundEventPtr pSoundEvent;
	while (posSnd)
	{
		pSoundEvent = m_soundEvents.GetNext(posSnd);
		if (pSoundEvent->GetEventType() == strEventType)
		{
			return pSoundEvent;
		}
	}

	return NULL;
}


void CUOLFonePluginConfig::SetDefaultSoundEvents()
{
	ATLASSERT(m_soundEvents.GetCount() == 0);
	
	CString strEventType;
	CString strDescription;
	CString strFileName;
	CString strFilePath;

	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY0);
	strDescription.LoadString(IDS_SOUND_EVENT_KEY0_DESCRIPTION);
	strFileName.LoadString(IDS_SOUND_EVENT_KEY0_FILENAME);
	strFilePath = CreateWaveFileFromResource(IDR_KEY_0, strFileName);
	CSoundEventPtr pSoundEventKey0 = new CSoundEvent(strEventType, strDescription, strFilePath, TRUE);

	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY1);
	strDescription.LoadString(IDS_SOUND_EVENT_KEY1_DESCRIPTION);
	strFileName.LoadString(IDS_SOUND_EVENT_KEY1_FILENAME);
	strFilePath = CreateWaveFileFromResource(IDR_KEY_1, strFileName);
	CSoundEventPtr pSoundEventKey1 = new CSoundEvent(strEventType, strDescription, strFilePath, TRUE); 

	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY2);
	strDescription.LoadString(IDS_SOUND_EVENT_KEY2_DESCRIPTION);
	strFileName.LoadString(IDS_SOUND_EVENT_KEY2_FILENAME);
	strFilePath = CreateWaveFileFromResource(IDR_KEY_2, strFileName);
	CSoundEventPtr pSoundEventKey2 = new CSoundEvent(strEventType, strDescription, strFilePath, TRUE);  

	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY3);
	strDescription.LoadString(IDS_SOUND_EVENT_KEY3_DESCRIPTION);
	strFileName.LoadString(IDS_SOUND_EVENT_KEY3_FILENAME);
	strFilePath = CreateWaveFileFromResource(IDR_KEY_3, strFileName);
	CSoundEventPtr pSoundEventKey3 = new CSoundEvent(strEventType, strDescription, strFilePath, TRUE);  

	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY4);
	strDescription.LoadString(IDS_SOUND_EVENT_KEY4_DESCRIPTION);
	strFileName.LoadString(IDS_SOUND_EVENT_KEY4_FILENAME);
	strFilePath = CreateWaveFileFromResource(IDR_KEY_4, strFileName);
	CSoundEventPtr pSoundEventKey4 = new CSoundEvent(strEventType, strDescription, strFilePath, TRUE);  

	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY5);
	strDescription.LoadString(IDS_SOUND_EVENT_KEY5_DESCRIPTION);
	strFileName.LoadString(IDS_SOUND_EVENT_KEY5_FILENAME);
	strFilePath = CreateWaveFileFromResource(IDR_KEY_5, strFileName);
	CSoundEventPtr pSoundEventKey5 = new CSoundEvent(strEventType, strDescription, strFilePath, TRUE);  

	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY6);
	strDescription.LoadString(IDS_SOUND_EVENT_KEY6_DESCRIPTION);
	strFileName.LoadString(IDS_SOUND_EVENT_KEY6_FILENAME);
	strFilePath = CreateWaveFileFromResource(IDR_KEY_6, strFileName);
	CSoundEventPtr pSoundEventKey6 = new CSoundEvent(strEventType, strDescription, strFilePath, TRUE);  

	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY7);
	strDescription.LoadString(IDS_SOUND_EVENT_KEY7_DESCRIPTION);
	strFileName.LoadString(IDS_SOUND_EVENT_KEY7_FILENAME);
	strFilePath = CreateWaveFileFromResource(IDR_KEY_7, strFileName);
	CSoundEventPtr pSoundEventKey7 = new CSoundEvent(strEventType, strDescription, strFilePath, TRUE);  

	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY8);
	strDescription.LoadString(IDS_SOUND_EVENT_KEY8_DESCRIPTION);
	strFileName.LoadString(IDS_SOUND_EVENT_KEY8_FILENAME);
	strFilePath = CreateWaveFileFromResource(IDR_KEY_8, strFileName);
	CSoundEventPtr pSoundEventKey8 = new CSoundEvent(strEventType, strDescription, strFilePath, TRUE);  

	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_KEY9);
	strDescription.LoadString(IDS_SOUND_EVENT_KEY9_DESCRIPTION);
	strFileName.LoadString(IDS_SOUND_EVENT_KEY9_FILENAME);
	strFilePath = CreateWaveFileFromResource(IDR_KEY_9, strFileName);
	CSoundEventPtr pSoundEventKey9 = new CSoundEvent(strEventType, strDescription, strFilePath, TRUE);  

	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_ASTERISK);
	strDescription.LoadString(IDS_SOUND_EVENT_ASTERISK_DESCRIPTION);
	strFileName.LoadString(IDS_SOUND_EVENT_ASTERISK_FILENAME);
	strFilePath = CreateWaveFileFromResource(IDR_KEY_ASTERISK, strFileName);
	CSoundEventPtr pSoundEventAsterisk = new CSoundEvent(strEventType, strDescription, strFilePath, TRUE);  

	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_POUND);
	strDescription.LoadString(IDS_SOUND_EVENT_POUND_DESCRIPTION);
	strFileName.LoadString(IDS_SOUND_EVENT_POUND_FILENAME);
	strFilePath = CreateWaveFileFromResource(IDR_KEY_POUND, strFileName);
	CSoundEventPtr pSoundEventPound = new CSoundEvent(strEventType, strDescription, strFilePath, TRUE);  

	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_CALL_CALLING);
	strDescription.LoadString(IDS_SOUND_EVENT_CALL_CALLING_DESCRIPTION);
	strFileName.LoadString(IDS_SOUND_EVENT_CALL_CALLING_FILENAME);
	strFilePath = CreateWaveFileFromResource(IDR_WAVE_CALL_CALLING, strFileName);
	CSoundEventPtr pSoundEventCallCalling = new CSoundEvent(strEventType, strDescription, strFilePath, TRUE);  

	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_CALL_CLOSED);
	strDescription.LoadString(IDS_SOUND_EVENT_CALL_CLOSED_DESCRIPTION);
	strFileName.LoadString(IDS_SOUND_EVENT_CALL_CLOSED_FILENAME);
	strFilePath = CreateWaveFileFromResource(IDR_WAVE_CALL_CLOSED, strFileName);
	CSoundEventPtr pSoundEventCallClosed = new CSoundEvent(strEventType, strDescription, strFilePath, TRUE);  

	strEventType.LoadString(IDS_SOUND_EVENT_TYPE_CALL_RINGING);
	strDescription.LoadString(IDS_SOUND_EVENT_CALL_RINGING_DESCRIPTION);
	strFileName.LoadString(IDS_SOUND_EVENT_CALL_RINGING_FILENAME);
	strFilePath = CreateWaveFileFromResource(IDR_WAVE_CALL_RINGING, strFileName);
	CSoundEventPtr pSoundEventCallRinging = new CSoundEvent(strEventType, strDescription, strFilePath, TRUE);  


	m_soundEvents.AddHead(pSoundEventKey0);
	m_soundEvents.AddHead(pSoundEventKey1);
	m_soundEvents.AddHead(pSoundEventKey2);
	m_soundEvents.AddHead(pSoundEventKey3);
	m_soundEvents.AddHead(pSoundEventKey4);
	m_soundEvents.AddHead(pSoundEventKey5);
	m_soundEvents.AddHead(pSoundEventKey6);
	m_soundEvents.AddHead(pSoundEventKey7);
	m_soundEvents.AddHead(pSoundEventKey8);
	m_soundEvents.AddHead(pSoundEventKey9);
	m_soundEvents.AddHead(pSoundEventAsterisk);
	m_soundEvents.AddHead(pSoundEventPound);
	m_soundEvents.AddHead(pSoundEventCallCalling);
	m_soundEvents.AddHead(pSoundEventCallClosed);
	m_soundEvents.AddHead(pSoundEventCallRinging);

}


void CUOLFonePluginConfig::GetSoundEvents(CAtlList<CSoundEventPtr>& soundEvents)
{
	soundEvents.AddHeadList(&m_soundEvents);
}


CString CUOLFonePluginConfig::CreateWaveFileFromResource(const int &nResourceID, const CString &strFileName)
{
	CString strCurrentUserProfilePath;
	CString strWaveFilePath;
	CString strSoundsDir;
	strSoundsDir.LoadString(IDS_SOUND_DIR);
	strSoundsDir.Insert(0, "\\");

	CFacadeWrapper::GetInstance()->GetCurrentUserProfilePath(strCurrentUserProfilePath);
	strCurrentUserProfilePath.Append(strSoundsDir);

	// Create directory if necessary
	BOOL bPathExists = TRUE;

	bPathExists = ::PathFileExists(strCurrentUserProfilePath);
	if (!bPathExists)
	{
		bPathExists = ::CreateDirectory(strCurrentUserProfilePath, NULL);
	}

	strWaveFilePath.Format("%s\\%s", strCurrentUserProfilePath, strFileName);
	
	HRSRC hResource;
	hResource = FindResource(ATL::_AtlBaseModule.GetResourceInstance(),
		MAKEINTRESOURCE(nResourceID), "WAVE");

	if (hResource) 
	{
		HGLOBAL hGlobal = LoadResource(ATL::_AtlBaseModule.GetResourceInstance(), hResource);
		CAtlFile file;

		BOOL bFileOK = (S_OK == file.Create(strWaveFilePath, GENERIC_READ | GENERIC_WRITE, 0, CREATE_ALWAYS));
		if (bFileOK) 
		{
			DWORD dwSize = SizeofResource(ATL::_AtlBaseModule.GetResourceInstance(), hResource);
			file.Write(LockResource(hGlobal), dwSize);
			file.Close();
		}
	}

	return strWaveFilePath;
}


void CUOLFonePluginConfig::PublishPluginProperties()
{
	// Publish the plugin properties for further use on 
	// "Bug Report" info.
	IUOLMessengerFacade4* pFacade4 = 
			dynamic_cast<IUOLMessengerFacade4*>(
					CFacadeWrapper::GetInstance()->GetFacadeInstance());
	
	IUOLMessengerPluginPropertiesRegisterPtr pPluginPropertiesRegister;
	
	if (pFacade4 != NULL)
	{
		pPluginPropertiesRegister = 
				pFacade4->GetPluginPropertiesRegister();
	}
	
	if (pPluginPropertiesRegister != NULL)
	{
		CString strBooleanFlag;
		
		pPluginPropertiesRegister->RegisterPluginProperty(
				PLUGIN_INTERNAL_NAME, 
				PLUGIN_PROPERTY_LINE_IN, 
				GetLineInDevice());
		
		pPluginPropertiesRegister->RegisterPluginProperty(
				PLUGIN_INTERNAL_NAME, 
				PLUGIN_PROPERTY_LINE_OUT, 
				GetLineOutDevice());
		
		(GetForceTunnel()) ? strBooleanFlag = "Sim" : strBooleanFlag = "Não";
		pPluginPropertiesRegister->RegisterPluginProperty(
				PLUGIN_INTERNAL_NAME, 
				PLUGIN_PROPERTY_FORCE_TUNNEL, 
				strBooleanFlag);
		
		(RegisterOnlyOnCall()) ? strBooleanFlag = "Sim" : strBooleanFlag = "Não";
		pPluginPropertiesRegister->RegisterPluginProperty(
				PLUGIN_INTERNAL_NAME, 
				PLUGIN_PROPERTY_REGISTER_ONLY_ON_CALL, 
				strBooleanFlag);
	}
}

