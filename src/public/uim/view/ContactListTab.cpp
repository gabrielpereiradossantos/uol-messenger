#include "stdafx.h"
#include "ContactListTab.h"

#include "UOLToolbarBuilder.h"
#include "NotifyPopUp/ContactNotifyPopUpHyperLinkHandler.h"
#include "NotifyPopUp/NotifyPopUpHandler.h"
#include "GroupListCtrl/GroupListCtrl.h"
#include "GroupListCtrl/GroupListCtrlItem.h"

#include "../controller/UIMApplication.h"
#include "../controller/uimanager.h"
#include "../controller/HistoryRecordMessage.h"
#include "../controller/NotifyWindow/NotifyWindowController.h"

#include "../HTMLFilter.h"


#include <commands/launchmessagewindowcommand.h>
#include <commands/sendfilecommand.h>
#include <commands/showprivacydialogcommand.h>
#include <commands/launchinfowindowcommand.h>
#include <commands/OpenBugReportCommand.h>



class CContactListTabCommandFactory
{
public:
	static IUOLMessengerCommandPtr GetCommand(const CString& strCommand, CWindow* pWindow);
};



CContactListTab::~CContactListTab()
{
}


LRESULT CContactListTab::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());

	m_pListSplitPanel = ap_dynamic_cast<CListSplitPanelPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__ListSplitPanelInstance"));
	m_pPluginCustomListCtrl = ap_dynamic_cast<CPluginCustomListCtrlPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__PluginCustomListInstance"));
    
    m_pOrderTraits = new CSettingsContactListOrderTraits();    
    m_pContactListContainer = new CContactListContainer();
    
	CreateToolbar();
	CreateListSplitPanel();
    CreateContactListContainer();
	CreatePluginCustomListCtrl();
	CreatePluginCustomPanelContainerView();

    InitListSplitPanel();

    ATLASSERT(m_pContactListContainer->GetContactListCtrl().get());
	ATLASSERT(m_pContactListContainer->GetFindContactPanel().get());

    if (m_pContactListContainer->GetContactListCtrl() != NULL)
	{
        IUOLMessengerFacade* pApp = CUIMApplication::GetApplication();

        //Set ContactList order traits
        m_pContactListContainer->GetContactListCtrl()->SetOrderTraits(m_pOrderTraits);
	    m_pOrderTraits->SetSortType(pApp->GetSettingsManager()->GetContactListSettings()->GetSortType());

        // Register as contact list settings event listener        
	    pApp->GetSettingsManager()->GetContactListSettings()->AddEventListener(m_pContactListContainer->GetContactListCtrl().get());
    }
	
	CUIMApplication::GetApplication()->GetRemoteContactList()->RegisterObserver(this);
	CUIMApplication::GetApplication()->GetConversationManager()->RegisterObserver(this);
	CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->AddEventListener(this);

	pUIManagerPtr->GetMainFrameFocusNotifier()->RegisterObserver(this);
    m_pContactListContainer->GetFindContactPanel()->RegisterObserver(this);
    m_pContactListContainer->GetContactListCtrl()->RegisterObserver(this);

	return 0L;
}


void CContactListTab::OnDestroy()
{
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	pUIManagerPtr->GetMainFrameFocusNotifier()->UnregisterObserver(this);

	CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->RemoveEventListener(this);
	CUIMApplication::GetApplication()->GetConversationManager()->UnregisterObserver(this);
	CUIMApplication::GetApplication()->GetRemoteContactList()->UnregisterObserver(this);
    m_pContactListContainer->GetFindContactPanel()->UnregisterObserver(this);
    m_pContactListContainer->GetContactListCtrl()->UnregisterObserver(this);

	if (m_pButtonImageSwitcher.get())
	{
		m_pButtonImageSwitcher->Finalize();
	}

	if (m_pPluginCustomPanelContainerView.get())
	{
		m_pPluginCustomPanelContainerView->Finalize();
		m_pPluginCustomPanelContainerView->DestroyWindow();
	}

	if (m_pContactListContainer->GetContactListCtrl().get())
	{
		// Unregister as contact list settings event listener
		CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->RemoveEventListener(m_pContactListContainer->GetContactListCtrl().get());

		m_pContactListContainer->GetContactListCtrl()->Finalize();
	}

	if (m_pPluginCustomListCtrl.get())
	{
		m_pPluginCustomListCtrl->Finalize();
	}
}


void CContactListTab::OnSetFocus(HWND /*lostFocusWnd*/)
{
	if (m_pContactListContainer->GetContactListCtrl().get() && m_pContactListContainer->GetContactListCtrl()->IsWindow() && m_pContactListContainer->GetContactListCtrl()->IsWindowVisible())
	{
		m_pContactListContainer->GetContactListCtrl()->SetFocus();

        m_pContactListContainer->UpdateLayout();

        GroupListCtrl::CGroupListCtrlItemPtr pItem;
        pItem = m_pContactListContainer->GetContactListCtrl()->GetSelectedItem(GroupListCtrl::CGroupListCtrl::IterateOptions::ioALL);
        if (!pItem)
        {
            m_pContactListContainer->GetContactListCtrl()->SelectItem(0);
        }
	}
}


LRESULT CContactListTab::OnShowAlert(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	LPSHOWALERTPARAM pAlert = (LPSHOWALERTPARAM) wParam;

	if (pAlert != NULL)
	{
		IUOLMessengerNotifyPopUpManagerPtr pNotifyPopUpManager = CUIMApplication::GetApplication()->GetNotifyPopUpManager();

		if (pNotifyPopUpManager.get())
		{
			UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
			CString strTitle;
			strTitle = pStringLoader->GetValue("uol.resource.IDS_APP_NAME");

			CAtlList<CString> listTextLines;

			if (!(pAlert->strLine1).IsEmpty())
			{
				listTextLines.AddTail(pAlert->strLine1);
			}

			if (!(pAlert->strLine2).IsEmpty())
			{
				listTextLines.AddTail(pAlert->strLine2);
			}

			IUOLMessengerUserIconPtr pUserIcon = pAlert->pContact->GetUserIcon();
			IUOLMessengerImagePtr pImage;

			if (pUserIcon != NULL)
			{
				pImage = pUserIcon->GetImage();
			}
			else
			{
				pImage = CUIMApplication::GetApplication()->GetUIManager()->GetImagesProtocol()->GetContactImage(pAlert->pContact->GetAccount()->GetProtocolId(), 1);
			}
			
			CNotifyPopUpHandlerPtr pNotifyPopUpHandler = new CNotifyPopUpHandler();

			pNotifyPopUpHandler->SetTitle(strTitle);
			pNotifyPopUpHandler->SetTextLines(listTextLines);
			pNotifyPopUpHandler->SetMessageIcon(pImage);
			pNotifyPopUpHandler->SetOptionsHyperLinkHandler(new COptionsHyperLinkHandler());
			pNotifyPopUpHandler->SetTextHyperLinkHandler(new CTextHyperLinkHandler(pAlert->pContact));

			IUOLMessengerNotifyPopUpResourcePtr pNotifyPopUpResource = CUIMApplication::GetApplication()->GetUIManager()->GetNotifyPopUpResource();
			pNotifyPopUpHandler->SetWindowIcon(pNotifyPopUpResource->GetImage(pAlert->resource));

			pNotifyPopUpManager->ShowAlertWindow(pNotifyPopUpHandler);
		}
	}

	return 0L;
}


LRESULT CContactListTab::OnNotifyAddedMessage(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	LPNOTIFYADDEDPARAM pNap = (LPNOTIFYADDEDPARAM) wParam;

	if (pNap != NULL)
	{
		CNotifyAddedDialogPtr pNotifyAddedDialog = 
			CNotifyWindowController::GetInstance()->GetWindow(pNap->pContact, pNap->bHasAlreadyAdded);

		if (pNotifyAddedDialog)
		{
			pNotifyAddedDialog->ShowWindow(SW_NORMAL);
		}
	}

	return 0L;
}


void CContactListTab::DoPaint(CDCHandle& dc)
{
	CRect rect;
	GetClientRect(&rect);

	dc.FillRect(&rect, 3);
}

void CContactListTab::UpdateLayout(BOOL bResizeBars)
{
	//__super::UpdateLayout(bResizeBars);
	
	CRect rect;
	GetClientRect(&rect);
	CSize size(rect.Width(), rect.Height());
    
	int y = 0;	

	if ((m_pUolToolbar.get()) && (m_pUolToolbar->IsWindow()))
	{
		CSize sizeElement;
		m_pUolToolbar->GetMinSize(sizeElement);

		m_pUolToolbar->SetWindowPos(NULL, 0, y, size.cx, sizeElement.cy,  SWP_NOZORDER);
		y += sizeElement.cy;
	}

    if ((m_pPluginCustomPanelContainerView) && (m_pPluginCustomPanelContainerView->IsWindow()))
	{
		CSize sizePluginPanelContainer;
		m_pPluginCustomPanelContainerView->GetMinSize(sizePluginPanelContainer);

		m_pPluginCustomPanelContainerView->SetWindowPos(NULL, 0, size.cy - sizePluginPanelContainer.cy, size.cx, sizePluginPanelContainer.cy,  SWP_NOZORDER);
		size.cy -= sizePluginPanelContainer.cy;        
	}

	if ((m_pListSplitPanel) && (m_pListSplitPanel->IsWindow()))
	{
		m_pListSplitPanel->SetWindowPos(NULL, 0, y, size.cx, (size.cy > y) ? (size.cy - y) : 0,  SWP_NOZORDER);
	}      
}

void CContactListTab::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	//__super::SetParameters(pMapParams, pElementTable);
}

void CContactListTab::GetMinSize(CSize& size)
{
	int cx = 0;
	int cy = 0;

	if (m_pUolToolbar.get() && (m_pUolToolbar->GetWindow()))
	{
		CSize sizeUolToolbar;
		m_pUolToolbar->GetMinSize(sizeUolToolbar);

		cy += sizeUolToolbar.cy;
		cx = sizeUolToolbar.cx;
	}

    if (m_pPluginCustomPanelContainerView.get() && (m_pPluginCustomPanelContainerView->IsWindow()))
	{
		CSize sizePluginCustomPanelContainer;
		m_pPluginCustomPanelContainerView->GetMinSize(sizePluginCustomPanelContainer);

		cy += sizePluginCustomPanelContainer.cy;
	}

	size.cx = cx;
	size.cy = cy;
}



void CContactListTab::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick)
{
	IUOLMessengerCommandPtr pCommand = GetCommand(pButton->GetCommandName());

	if (pCommand)
	{
		CUIMApplication::GetApplication()->ExecuteCommand(pCommand);
	}
}
	

void CContactListTab::OnAddContactDenied(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strMessage)
{
	ATLASSERT(pAccount != NULL);

	if (pAccount != NULL)
	{
		UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
		CString strMsgText;

		CString strFormat;
		if (strMessage.IsEmpty())
		{
			strFormat = pStringLoader->GetValue("uol.resource.error.IDS_JABBER_ADD_CONTACT_DENIED");
			strMsgText.Format(strFormat, strContact);
		}
		else
		{
			strFormat = pStringLoader->GetValue("uol.resource.error.IDS_JABBER_ADD_CONTACT_DENIED_REASON");
			strMsgText.Format(strFormat, strContact, strMessage);
		}
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogWarning(__FUNCTION__, 
				"A adição do contato foi negada retornando: [%s].", strMsgText);
		
		CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMsgText, MB_OK | MB_ICONERROR);
	}
}


void CContactListTab::OnContactSignedOn(IUOLMessengerContactPtr pContact)
{
	// Change contact background
	m_pContactListContainer->GetContactListCtrl()->ChangeContactPainter(pContact.get(), IPainterRepository::PR_CONTACT_ONLINE_PAINTER);

	COleDateTimeSpan tsElapsedTime = pContact->GetSignedOnTime() - pContact->GetAddedTime();
	COleDateTimeSpan tsNotifyTimeout(0, 0, 0, 60);

	if (tsElapsedTime > tsNotifyTimeout)
	{
		NotifyContactStatusChange(pContact);
	}
}


void CContactListTab::OnContactSignedOff(IUOLMessengerContactPtr pContact)
{
	// Change contact background
	m_pContactListContainer->GetContactListCtrl()->ChangeContactPainter(pContact.get(), IPainterRepository::PR_CONTACT_OFFLINE_PAINTER);

	NotifyContactStatusChange(pContact);
}


void CContactListTab::OnNotifyContactAction(IUOLMessengerContactPtr pContact, NotifyReason reason)
{
	// Ignore
}


void CContactListTab::OnNotifyAdded(IUOLMessengerContactPtr pContact, BOOL bOnContactList)
{
	// Ignore

	//NOTIFYADDEDPARAM nap;

	//nap.bHasAlreadyAdded = bOnContactList;
	//nap.pContact = pContact;

	//SendMessage(WM_USER_NOTIFY_ADDED, (WPARAM) &nap, NULL);
}


void CContactListTab::OnContactTyping(IUOLMessengerConversationPtr pConversation, TypingState state)
{
	if (state == TYPING)
	{
		// Change contact background
		CAtlList<IUOLMessengerContactPtr> listContacts;

		CUIMApplication::GetApplication()->GetContactList()->FindContacts(pConversation->GetAccount()->GetUserId(), pConversation->GetAccount()->GetProtocolId(), pConversation->GetContactName(), listContacts);

		for (POSITION pos = listContacts.GetHeadPosition(); pos; )
		{
			IUOLMessengerContactPtr pContact = listContacts.GetNext(pos);

			m_pContactListContainer->GetContactListCtrl()->ChangeContactPainter(pContact.get(), IPainterRepository::PR_CONTACT_TYPING_PAINTER);
		}
	}
}


void CContactListTab::OnIMReceived(IUOLMessengerConversationPtr pConversation, const CString& strText, DWORD dwMessageReceivingStatus)
{
	if (dwMessageReceivingStatus != IUOLMessengerConversationManager::MRS_DENIED)
	{
		IUOLMessengerContactPtr pContact = pConversation->GetContact();

		if (pContact)
		{
			if (dwMessageReceivingStatus == IUOLMessengerConversationManager::MRS_AUTHORIZED)
			{
				// Change contact background
				m_pContactListContainer->GetContactListCtrl()->ChangeContactPainter(pContact.get(), IPainterRepository::PR_CONTACT_NEW_MESSAGE_PAINTER);

				// Show alert window...
				if (CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->IsTrayNotificationEnabled())
				{
					SHOWALERTPARAM alert;

					alert.pContact = pContact;
					alert.strLine1.Format(IDS_CONTACT_SAYS, pContact->GetAlias());
					alert.strLine2 = CHTMLFilter::HTMLToText(strText);
					alert.resource = IUOLMessengerNotifyPopUpResource::NPR_NEW_MESSAGE;

					SendMessage(WM_USER_SHOW_ALERT, (WPARAM) &alert);
				}
			}
		}

		// Play sound...
		NotifyContactMessage(pContact);
	}
}


void CContactListTab::OnNudgeReceived(IUOLMessengerConversationPtr pConversation, const CString& strText, DWORD dwNudgeReceivingStatus)
{
	if (dwNudgeReceivingStatus != IUOLMessengerConversationManager::MRS_DENIED)
	{
		IUOLMessengerContactPtr pContact = pConversation->GetContact();
		
		if (pContact)
		{
			if (dwNudgeReceivingStatus == IUOLMessengerConversationManager::MRS_AUTHORIZED)
			{
				// Change contact background
				m_pContactListContainer->GetContactListCtrl()->ChangeContactPainter(pContact.get(), IPainterRepository::PR_CONTACT_NEW_MESSAGE_PAINTER);
				
				// Show alert window...
				SHOWALERTPARAM alert;
				
				alert.pContact = pContact;
				alert.strLine1 = CHTMLFilter::HTMLToText(strText);
				alert.resource = IUOLMessengerNotifyPopUpResource::NPR_NUDGE_RECEIVED;
				
				SendMessage(WM_USER_SHOW_ALERT, (WPARAM) &alert);
			}
		}
		
		// Play sound...
		CUIMApplication::GetApplication()->PlayNudgeSound(pContact);
	}
}


void CContactListTab::OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId)
{
	IUOLMessengerFacade* pApplication = CUIMApplication::GetApplication();
	IUOLMessengerContactListSettingsPtr pContactListSettings = pApplication->GetSettingsManager()->GetContactListSettings();

	if (pSource == pContactListSettings.get())
	{
		if (IUOLMessengerContactListSettings::CL_SORT_TYPE == nSettingId)
		{
			// Update order traits sort method
			m_pOrderTraits->SetSortType(pContactListSettings->GetSortType());

			ATLASSERT(m_pContactListContainer->GetContactListCtrl() != NULL);
			m_pContactListContainer->GetContactListCtrl()->Sort();
		}
		else if (IUOLMessengerContactListSettings::CL_CONTACT_NODE_LAYOUT == nSettingId)
		{
			m_pContactListContainer->GetContactListCtrl()->RequestUpdateWindow();
		}
	}
}


void CContactListTab::CreateToolbar()
{
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());	
	CUOLToolBarBuilderPtr pBuilder = FindElement<CUOLToolBarBuilderPtr>();
	
	if (pBuilder)
	{
		m_pUolToolbar = ap_dynamic_cast<CUOLToolBarPtr>(pBuilder->CreateElement());
		ATLASSERT(m_pUolToolbar);
	}

	if (m_pUolToolbar.get())
	{
		CRect rect = CRect(CPoint(0, 0), CSize(0, 0));
		m_pUolToolbar->Create(m_hWnd, rect);			

		ATLASSERT(m_pUolToolbar.get());

		m_pUolToolbar->AddButtonsEventListener(this);
	}
}


void CContactListTab::CreateUOLButtonImageSwitcher()
{
	if (m_pUolToolbar.get())
	{
		CImageButtonBasePtr pButton = m_pUolToolbar->FindImageButton(_T("OpenUOLHome"));

		if (pButton)
		{
			m_pButtonImageSwitcher = new CUOLButtonImageSwitcher(pButton, *(m_pUolToolbar.get()));
			m_pButtonImageSwitcher->Init();
		}
	}
}


void CContactListTab::CreatePluginCustomPanelContainerView()
{
	m_pPluginCustomPanelContainerView = new CPluginCustomPanelContainerView();

	ATLASSERT(m_pPluginCustomPanelContainerView != NULL);
	if (m_pPluginCustomPanelContainerView != NULL)
	{
		m_pPluginCustomPanelContainerView->Init(CUIMApplication::GetApplication()->GetUIManager()->GetPluginCustomPanelContainer());

		CRect rect = CRect(CPoint(0, 0), CSize(0, 0));
		m_pPluginCustomPanelContainerView->Create(m_hWnd, rect);

		ATLASSERT(m_pPluginCustomPanelContainerView->IsWindow());

		// Debug code
		//m_pApplication->GetUIManager()->GetPluginCustomPanelContainer()->AddPanel(new CDummyPluginPanelBuilder("PluginSimples"));
		//m_pApplication->GetUIManager()->GetPluginCustomPanelContainer()->AddPanel(new CDummyPluginPanelBuilder("PluginRadio"));
		// Debug code
	}
}


void CContactListTab::CreateListSplitPanel()
{
	if (m_pListSplitPanel != NULL)
	{
		CRect rectPanel = CRect(CPoint(0, 0), CSize(0, 0));
		m_pListSplitPanel->Create(m_hWnd, rectPanel);
	}
}


void CContactListTab::CreateContactListContainer()
{
    ATLASSERT(m_pContactListContainer != NULL);
	if (m_pContactListContainer != NULL)
	{
		ATLASSERT(m_pListSplitPanel != NULL);

		if (m_pListSplitPanel != NULL)
		{
			CWindow wndParent = (HWND) *m_pListSplitPanel.get();

			CRect rect = CRect(CPoint(0, 0), CSize(100, 50));
			m_pContactListContainer->Create(wndParent, rect, NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		}
	}
}


void CContactListTab::CreatePluginCustomListCtrl()
{
	ATLASSERT(m_pPluginCustomListCtrl != NULL);
	if (m_pPluginCustomListCtrl != NULL)
	{
		m_pPluginCustomListCtrl->Init(CUIMApplication::GetApplication()->GetUIManager()->GetPluginCustomList());

		ATLASSERT(m_pListSplitPanel != NULL);

		if (m_pListSplitPanel != NULL)
		{
			CWindow wndParent = (HWND) *m_pListSplitPanel.get();

			CRect rect = CRect(CPoint(0, 0), CSize(100, 50));
			m_pPluginCustomListCtrl->Create(wndParent, rect, NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		}
	}
}


void CContactListTab::InitListSplitPanel()
{
	ATLASSERT(m_pListSplitPanel.get());
	ATLASSERT(m_pContactListContainer.get());
	ATLASSERT(m_pPluginCustomListCtrl.get());

	m_pListSplitPanel->SetSplitterPanes(*(m_pContactListContainer.get()), *(m_pPluginCustomListCtrl.get()));
}


void CContactListTab::NotifyContactStatusChange(IUOLMessengerContactPtr pContact)
{
	IUOLMessengerFacade* pFacade = CUIMApplication::GetApplication();

	if (pContact->GetStatus() == IUOLMessengerContact::CS_ONLINE)
	{
		if (pFacade->GetSettingsManager()->GetContactListSettings()->GetNotifyOnline())
		{
			SHOWALERTPARAM alert;

			alert.pContact = pContact;
			alert.strLine1.Format(IDS_CONTACT_ONLINE, pContact->GetAlias());
			alert.resource = IUOLMessengerNotifyPopUpResource::NPR_CONTACT_ONLINE;

			SendMessage(WM_USER_SHOW_ALERT, (WPARAM) &alert);
		}

		// Contact became online
		
		BOOL bEnableSound = pFacade->GetSettingsManager()->GetGeneralSettings()->IsEnableSound();

		DWORD dwAccountStatus = CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetStatus(pContact->GetAccount());

		BOOL bAwayStatus = (dwAccountStatus == IUOLMessengerAccountConnectionManager::AS_AWAY);

		if (!bEnableSound || bAwayStatus &&  pFacade->GetSettingsManager()->GetGeneralSettings()->IsMuteWhileAwayEnabled())
		{
			// Do not play if mute when away
			return;
		}

		IUOLMessengerSettingsManager2Ptr pSettingsManager2 = ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(pFacade->GetSettingsManager());

		if (pSettingsManager2)
		{
			CString strSoundPath;
			
			if (pSettingsManager2->GetSoundSettings()->GetSoundEventFilePath(IUOLMessengerSoundSettings2::CONTACT_ONLINE_SOUND_EVENT, strSoundPath, TRUE))
			{
				pFacade->GetUIManager()->PlaySound(strSoundPath, FALSE);
			}
		}
	}
	else if (pContact->GetStatus() == IUOLMessengerContact::CS_OFFLINE)
	{
		if (pFacade->GetSettingsManager()->GetContactListSettings()->GetNotifyOffline())
		{
			SHOWALERTPARAM alert;

			alert.pContact = pContact;
			alert.strLine2.Format(IDS_CONTACT_OFFLINE, pContact->GetAlias());
			alert.resource = IUOLMessengerNotifyPopUpResource::NPR_CONTACT_OFFLINE;

			SendMessage(WM_USER_SHOW_ALERT, (WPARAM) &alert);
		}
	}
}


void CContactListTab::NotifyContactMessage(IUOLMessengerContactPtr pContact)
{
	IUOLMessengerFacade* pFacade = CUIMApplication::GetApplication();

	// Play sound...
	BOOL bEnableSound = pFacade->GetSettingsManager()->GetGeneralSettings()->IsEnableSound();
	DWORD dwAccountStatus = pFacade->GetAccountConnectionManager()->GetStatus(pContact->GetAccount());
	BOOL bAwayStatus = (dwAccountStatus == IUOLMessengerAccountConnectionManager::AS_AWAY);

	if (!bEnableSound || bAwayStatus &&  pFacade->GetSettingsManager()->GetGeneralSettings()->IsMuteWhileAwayEnabled())
	{
		// Do not play if mute when away
		return;
	}

	IUOLMessengerSettingsManager2Ptr pSettingsManager2 = ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(pFacade->GetSettingsManager());

	if (pSettingsManager2)
	{
		CString strSoundPath;
		
		if (pSettingsManager2->GetSoundSettings()->GetSoundEventFilePath(IUOLMessengerSoundSettings2::NEW_MESSAGE_SOUND_EVENT, strSoundPath, TRUE))
		{
			pFacade->GetUIManager()->PlaySound(strSoundPath, FALSE);
		}
	}
}


IUOLMessengerContactPtr CContactListTab::GetSelectedContact()
{ 
	if (m_pContactListContainer->GetContactListCtrl().get())
	{
		return m_pContactListContainer->GetContactListCtrl()->GetSelectedContact();
	}

	return IUOLMessengerContactPtr();
}


IUOLMessengerCommandPtr CContactListTab::GetCommand(const CString& strCommand)
{
	return CContactListTabCommandFactory::GetCommand(strCommand, this);
}


void CContactListTab::ShowContactListTab()
{
	if ((m_pTabViewCtrl.get()) && (m_pTabViewCtrl->IsWindow()))
	{
		m_pTabViewCtrl->SetActiveTab(0);
	}
	
	ShowWindow(SW_SHOW);
	SetFocus();
}


void CContactListTab::HideContactListTab()
{
	if ((m_pTabViewCtrl.get()) && (m_pTabViewCtrl->IsWindow()))
	{
		// Don't hide this window if there is no other tabs
		UINT nTabsCount = m_pTabViewCtrl->GetTabsCount();

		if (nTabsCount > 1)
		{
			ShowWindow(SW_HIDE);
		}
	}
}



void CContactListTab::Init(CTabViewPtr pTabViewCtrl)
{
	ATLASSERT(pTabViewCtrl);
	m_pTabViewCtrl = pTabViewCtrl;
	
	// TODO: Verificar com o cliente se isso ficarah hardcodec aqui.
	m_pTabViewCtrl->CreateTabButton("Contatos");
	m_pTabViewCtrl->SetActiveTab(0);
}


LRESULT CContactListTab::OnUpdateLayout(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	UpdateLayout();

	GetParent().SendMessage(WM_USER_PREFS_CHANGED);

	return 0L;
}


void CContactListTab::OnFindPanelTextChanged(CString strFindContactText)
{
    ATLTRACE(_T("%s - %s\n"), __FUNCTION__, strFindContactText);
    m_pContactListContainer->GetContactListCtrl()->Sort();
	m_pContactListContainer->GetContactListCtrl()->UpdateItemSelection();
    m_pContactListContainer->GetContactListCtrl()->UpdateScrollSize();
}


void CContactListTab::OnFindPanelArrowKeyPressed()
{
    if (m_pContactListContainer->GetContactListCtrl().get() && m_pContactListContainer->GetContactListCtrl()->IsWindow() && m_pContactListContainer->GetContactListCtrl()->IsWindowVisible())
	{
        m_pContactListContainer->GetContactListCtrl()->SetFocus();
        m_pContactListContainer->GetContactListCtrl()->SelectItem(1);
	}
}


void CContactListTab::OnFindPanelReturnKeyPressed()
{
    if (m_pContactListContainer->GetContactListCtrl().get() && m_pContactListContainer->GetContactListCtrl()->IsWindow() && m_pContactListContainer->GetContactListCtrl()->IsWindowVisible())
	{
        m_pContactListContainer->GetContactListCtrl()->SetFocus();
        m_pContactListContainer->GetContactListCtrl()->SelectItem(1);

		IUOLMessengerContactPtr pContact = GetSelectedContact();
		if (pContact)
		{
			IUOLMessengerCommandPtr pCommand = new CLaunchMessageWindowCommand(pContact);
			if (pCommand)
			{
				CUIMApplication::GetApplication()->ExecuteCommand(pCommand);
			}
		}
	}
}


void CContactListTab::OnContactListChar(TCHAR ch)
{
    char validChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%&*()[{}]/?\\|";

    if (strchr(validChars, ch))
    {
        CString strText(ch);

        if ((m_pContactListContainer->GetFindContactPanel()) && (m_pContactListContainer->GetFindContactPanel()->IsWindow()) && (m_pContactListContainer->GetFindContactPanel()->IsWindowVisible()))
        {
            m_pContactListContainer->GetFindContactPanel()->SetFocus();
			m_pContactListContainer->GetFindContactPanel()->SetText(strText);
        }
    }
}


void CContactListTab::SetFindContactPanelFocus()
{
    if ((m_pContactListContainer->GetFindContactPanel()) && (m_pContactListContainer->GetFindContactPanel()->IsWindow()) && (m_pContactListContainer->GetFindContactPanel()->IsWindowVisible()))
    {
        m_pContactListContainer->GetFindContactPanel()->SetFocus();
    }
}


CFindContactPanelPtr CContactListTab::GetFindContactPanel()
{
	if (m_pContactListContainer.get())
	{
		return m_pContactListContainer->GetFindContactPanel();
	}

	return NULL;
}










// --------------------
// CContactListTabCommandFactory

IUOLMessengerCommandPtr CContactListTabCommandFactory::GetCommand(const CString& strCommand, CWindow* pWindow)
{
	ATLASSERT(pWindow);

	IUOLMessengerContactPtr pContact = static_cast<CContactListTab*>(pWindow)->GetSelectedContact();
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	if (strCommand.CompareNoCase("SendMessage") == 0)
	{
		if (pContact)
		{
			return new CLaunchMessageWindowCommand(pContact);
		}
		else if (0 == CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectedAccountsCount())
		{
			CString strMessage;
			strMessage = pStringLoader->GetValue("uol.resource.error.IDS_SHOW_MESSAGE_WINDOW_NO_ONLINE_ACCOUNTS_MSG");

			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMessage, MB_OK | MB_ICONERROR);
		}
	}
	else if (strCommand.CompareNoCase("SendFile") == 0)
	{
		if (pContact)
		{
			return new CSendFileCommand(pContact, *pWindow);
		}
		else if (0 == CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectedAccountsCount())
		{
			CString strMessage;
			strMessage = pStringLoader->GetValue("uol.resource.error.IDS_SEND_FILE_NO_ONLINE_ACCOUNTS_MSG");

			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(*pWindow, strMessage, MB_OK | MB_ICONERROR);
		}
	}
	else if (strCommand.CompareNoCase("ShowPrivacy") == 0)
	{
		return new CShowPrivacyDialogCommand(*pWindow);		
	}
	else if (strCommand.CompareNoCase("ShowUserInfo") == 0)
	{
		if (pContact)
		{
			return new CLaunchInfoWindowCommand(pContact, *pWindow);
		}
		else if (0 == CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetConnectedAccountsCount())
		{
			CString strMessage;
			strMessage = pStringLoader->GetValue("uol.resource.error.IDS_SHOW_INFO_NO_ONLINE_ACCOUNTS_MSG");

			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMessage, MB_OK | MB_ICONERROR);
		}
	}
	else if (strCommand.CompareNoCase("ShowHistory") == 0)
	{
		CUIManagerPtr pUIManager = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
		if (pUIManager)
		{
			CString strCaption;
			CString strFormat;
			strFormat = pStringLoader->GetValue("uol.resource.IDS_HISTORY_WINDOW_MESSAGES_TITLE");
			strCaption.Format(strFormat, pContact->GetAlias());

			CHistoryRecordMessagePtr pHistoryRecordMessage;
			pHistoryRecordMessage = new CHistoryRecordMessage(pContact->GetAccount(), pContact->GetName());
			
			CHistoryRecordMessageListPtr pHistoryRecordMessageList;
			pHistoryRecordMessageList = new CHistoryRecordMessageList(pHistoryRecordMessage);
			pUIManager->ShowHistoryWindow(pHistoryRecordMessage, pHistoryRecordMessageList, strCaption);
		}
	}
	else if (strCommand.CompareNoCase("OpenBugReport") == 0)
	{
		return new COpenBugReportCommand(*pWindow);
	}
	else
	{
		return CUIMApplication::GetApplication()->GetCommandFactory()->GetCommand(strCommand);
	}

	return IUOLMessengerCommandPtr();
}
