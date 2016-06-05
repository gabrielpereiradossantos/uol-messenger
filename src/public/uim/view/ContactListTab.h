#pragma once

#include <interfaces/IUOLMessengerFacade.h>
#include <interfaces/IUOLMessengerContact.h>
#include <interfaces/IUOLMessengerContactListCtrl.h>
#include <interfaces/IUOLMessengerContactListTab.h>
#include <interfaces/IUOLMessengerSettingsEventListener.h>

#include "DummyWindow.h"
#include "skin/TabCtrlElement.h"

#include "UOLToolbar.h"
#include "ListSplitPanel.h"
#include "PluginCustomPanelContainerView.h"
#include "UOLButtonImageSwitcher.h"
#include "userMessages.h"
#include "ContactList/ContactListCtrl.h"
#include "SettingsContactListOrderTraits.h"
#include "PluginCustomList/PluginCustomListCtrl.h"
#include "TabView.h"
#include "FindContactPanel.h"
#include "ContactListContainer.h"


//class CContactListTab;
//typedef CWindowElement<CContactListTab, CDummyWindow<CWindowImpl<CContactListTab> > > CTabCtrlItemBase;
//typedef CTabCtrlItem<CTabCtrlItemBase> CContactListTabBase;

class CContactListTab;
typedef CWindowElement<CContactListTab, CDummyWindow<CWindowImpl<CContactListTab> > > CContactListTabBase;

class CContactListTab : public CContactListTabBase, 
						public CUOLMessengerImageButtonEventListener2, 
						public CUOLMessengerRemoteContactListObserver,
						public CUOLMessengerConversationManagerObserver, 
						public IUOLMessengerSettingsEventListener,
						public IUOLMessengerContactListTab,
						public CUOLMessengerMainFrameFocusObserver,
	                    public CUOLMessengerAccountConnectionManagerObserver,
	                    public CUOLMessengerAccountListEventListener,
                        public CFindPanelObserver2,
                        public CUOLMessengerContactListCtrlObserver2
{
public:
	DECLARE_ELEMENT_NAME("ContactListTab");

	virtual ~CContactListTab();

	IUOLMessengerContactPtr GetSelectedContact();
	void Init(CTabViewPtr pTabViewCtrl);
    void SetFindContactPanelFocus();
	CFindContactPanelPtr GetFindContactPanel();

    //CFindPanelObserver method
    virtual void OnFindPanelTextChanged(CString strFindContactText);
    virtual void OnFindPanelArrowKeyPressed();
	virtual void OnFindPanelReturnKeyPressed();

    //CUOLMessengerContactListCtrlObserver2 methods
    virtual void OnContactListChar(TCHAR ch);


protected:
	BEGIN_MSG_MAP(CContactListTab)			
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SETFOCUS(OnSetFocus)
		MESSAGE_HANDLER(WM_USER_SHOW_ALERT, OnShowAlert)
		MESSAGE_HANDLER(WM_USER_NOTIFY_ADDED, OnNotifyAddedMessage)
		MESSAGE_HANDLER(WM_USER_UPDATE_LAYOUT, OnUpdateLayout)
		CHAIN_MSG_MAP(CContactListTabBase)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);
	void	OnDestroy();

	LRESULT OnShowAlert(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNotifyAddedMessage(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnUpdateLayout(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);


	virtual void DoPaint(CDCHandle& dc);
	virtual void UpdateLayout(BOOL bResizeBars = TRUE);
	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	virtual void GetMinSize(CSize& size);

	
	// CImageButtonEventListener overridden functions
	void OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick);


	// CUOLMessengerRemoteContactListObserver overriden functions
	virtual void OnAddContactDenied(IUOLMessengerAccountPtr pAccount, const CString& strContact, const CString& strMessage);
	virtual void OnContactSignedOn(IUOLMessengerContactPtr pContact);
	virtual void OnContactSignedOff(IUOLMessengerContactPtr pContact);
	virtual void OnNotifyContactAction(IUOLMessengerContactPtr pContact, NotifyReason reason);
	virtual void OnNotifyAdded(IUOLMessengerContactPtr pContact, BOOL bOnContactList);


	// CUOLMessengerConversationManagerObserver overriden functions
	virtual void OnContactTyping(IUOLMessengerConversationPtr pConversation, TypingState state);
	virtual void OnIMReceived(IUOLMessengerConversationPtr pConversation, const CString& strText, DWORD dwMessageReceivingStatus);
	virtual void OnNudgeReceived(IUOLMessengerConversationPtr pConversation, const CString& strText, DWORD dwNudgeReceivingStatus);


	// IUOLMessengerEventListener overriden functions
	void OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId);

	void ShowContactListTab();
	void HideContactListTab();

	//CUOLMessengerMainFrameFocusObserver overriden functions
	virtual void OnSetFocus(HWND lostFocusWnd);


private:
	void CreateToolbar();
	void CreateUOLButtonImageSwitcher();
	void CreatePluginCustomPanelContainerView();
	void CreatePluginCustomTabPanelContainerView();
	void CreateListSplitPanel();	
    void CreateContactListContainer();
	void CreatePluginCustomListCtrl();
	void InitListSplitPanel();

	void NotifyContactStatusChange(IUOLMessengerContactPtr pContact);
	void NotifyContactMessage(IUOLMessengerContactPtr pContact);
	
	IUOLMessengerCommandPtr GetCommand(const CString& strCommand);

private:
	CUOLToolBarPtr						m_pUolToolbar;
	CListSplitPanelPtr					m_pListSplitPanel;
	CPluginCustomPanelContainerViewPtr	m_pPluginCustomPanelContainerView;
	IUOLMessengerAccountPtr				m_pCurrentAccount;
	CUOLButtonImageSwitcherPtr			m_pButtonImageSwitcher;
	CSettingsContactListOrderTraitsPtr	m_pOrderTraits;
	CPluginCustomListCtrlPtr			m_pPluginCustomListCtrl;
    CTabViewPtr							m_pTabViewCtrl;
    CContactListContainerPtr            m_pContactListContainer;
};

MAKEAUTOPTR(CContactListTab);

