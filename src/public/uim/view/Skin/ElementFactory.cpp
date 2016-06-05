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
#include "elementfactory.h"

#include "PositionElement.h"
#include "skinelement.h"
#include "solidbackgroundelement.h"
#include "./bitmapbackgroundelement.h"
#include "../ContactList/ContactListCtrl.h"
#include "GroupElement.h"
#include "ContactElement.h"
#include "PainterElement.h"
#include "StaticIconElement.h"
#include "../userstatuspanel.h"
#include "../uoltoolbarbuilder.h"
#include "../ImageButton.h"
#include "../AppWindow.h"
#include "../windowcaptionBuilder.h"
#include "../messagewindowbuilder.h"
#include "../uimstatusbarbuilder.h"
#include "FontElement.h"
#include "borderelement.h"
#include "../Preferences/PreferencesPropertySheetBuilder.h"
#include "../HistoryFrameBuilder.h"
#include "../MessageEditPanelBuilder.h"
#include "../InfoFrameBuilder.h"
#include "iconelement.h"
#include "menuelement.h"
#include "../TabViewBuilder.h"
#include "TabViewElement.h"
#include "PropPageElement.h"
#include "../Preferences/AccountsPropPage.h"
#include "../Preferences/ButtonTabCtrlBuilder.h"
#include "../skin/ProtocolImageElement.h"
#include "../NotifyPopUp/NotifyPopUpBuilder.h"
#include "../NotifyPopUp/NotifyPopUpCaptionBuilder.h"
#include "../NotifyPopUp/NotifyPopUpImages.h"
#include "../PluginCustomList/PluginCustomListCtrl.h"
#include "PluginCustomEntryElement.h"
#include "PluginCustomGroupElement.h"
#include "../ListSplitPanel.h"
#include "../FindContactPanel.h"
#include "../AccountsPanel.h"
#include "../Tooltip/ContactTooltipBuilder.h"
#include "../Tooltip/PluginTooltipBuilder.h"
#include "../FrameWindowBuilder.h"
#include "../MessageEdit/EmoticonsFrameBuilder.h"
#include "../MultiStateImageButtonBuilder.h"
#include "../ButtonStateInfoBuilder.h"
#include "TabCtrlElement.h"
#include "AppDefaultIconElement.h"
#include "AppAboutBoxElement.h"
#include "AppCloseAlertElement.h"
#include "StatusTrayIconElement.h"
#include "SystemTrayIconListElement.h"
#include "../ContactListTab.h"
#include "../AccountConnection/ConnectionErrorWindowBuilder.h"
#include "../AccountConnection/AccountListElement.h"
#include "../Wizard/WizardInitDialog.h"
#include "../Wizard/WizardProfileDialog.h"
#include "../Wizard/WizardMSNAccountDialog.h"
#include "../Wizard/WizardUOLAccountDialog.h"
#include "../Wizard/WizardICQAccountDialog.h"
#include "../Wizard/WizardJabberAccountDialog.h"
#include "../Wizard/WizardYahooAccountDialog.h"
#include "../Wizard/WizardPreferencesDialog.h"
#include "../Wizard/WizardPersonalizeAccountsDialog.h"
#include "../Wizard/WizardFinishDialog.h"
#include "../Wizard/WizardSplashDialog.h"
#include "BitmapElement.h"
#include "ContactNodeLayoutBitmapElement.h"
#include "MainDialogElement.h"
#include "DefaultEmoticonImageElement.h"
#include "../ChatList/ChatListCtrlBuilder.h"
#include "../ChatList/ReadyForChatListCtrlBuilder.h"
#include "../ChatList/ChatInviteListCtrlBuilder.h"
#include "../ConversationWindowSplitterBuilder.h"
#include "../TitlePanelBuilder.h"
#include "../FileTransfer/FileTransferFrameBuilder.h"
#include "../FileTransfer/FileTransferListCtrl.h"
#include "../PluginList/PluginListCtrl.h"
#include "../FindContactPanelBuilder.h"
#include "./ProgressBarElement.h"
#include "../FindPanelButtonBuilder.h"



CElementFactory::CString2AllocatorMap CElementFactory::ms_mapElements(136);


CElementFactory::CElementFactory(void)
{
}

CElementFactory::~CElementFactory(void)
{
}

IElementPtr CElementFactory::GetElement(const CString& strName, CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (ms_mapElements.IsEmpty())
	{
		InitializeMapElements();	
	}

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "Looking for the element [%s].", strName);
	
	CElementPtr pElement;
	const CString2AllocatorMap::CPair* pPair = ms_mapElements.Lookup(CString(strName).MakeLower());

	if (pPair && (pPair->m_value))
	{
		pElement = ap_static_cast<CElementPtr>(pPair->m_value());
	
		if (pElement)
		{
			pElement->SetParameters(pMapParams, pElementTable);
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"Failure creating the element. (pElement == NULL)");
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"Element not found or allocation method not registered.");
	}

	ATLASSERT(pElement.get());
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return pElement;
}

static IElementPtr AllocPositionElement()
{
	return new CPositionElement();
}

static IElementPtr AllocSkinElement()
{
	return new CSkinElement();
}

static IElementPtr AllocSolidBackgroundElement()
{
	return new CSolidBackgroundElement();
}

static IElementPtr AllocBitmapBackgroundElement()
{
	return new CBitmapBackgroundElement();
}

static IElementPtr AllocContactListCtrl()
{
	return new CContactListCtrl();
}

static IElementPtr AllocGroupElement()
{
	return new CGroupElement();
}

static IElementPtr AllocContactElement()
{
	return new CContactElement();
}

static IElementPtr AllocGroupDefaultPainterElement()
{
	return new CGroupDefaultPainterElement();
}

static IElementPtr AllocGroupSelectionPainterElement()
{
	return new CGroupSelectionPainterElement();
}

static IElementPtr AllocContactDefaultPainterElement()
{
	return new CContactDefaultPainterElement();
}

static IElementPtr AllocContactSelectionPainterElement()
{
	return new CContactSelectionPainterElement();
}

static IElementPtr AllocOnlinePainterElement()
{
	return new COnlinePainterElement();
}

static IElementPtr AllocStaticIconElement()
{
	return new CStaticIconElement();
}

static IElementPtr AllocOfflinePainterElement()
{
	return new COfflinePainterElement();
}

static IElementPtr AllocTypingPainterElement()
{
	return new CTypingPainterElement();
}

static IElementPtr AllocNewMessagePainterElement()
{
	return new CNewMessagePainterElement();
}

static IElementPtr AllocUserStatusPanel()
{
	return new CUserStatusPanel();
}

static IElementPtr AllocUOLToolBarBuilder()
{
	return new CUOLToolBarBuilder();
}

static IElementPtr AllocImageButtonBuilder()
{
	return new CImageButtonBuilder();
}

static IElementPtr AllocAppWindow()
{
	return new CAppWindow();
}

static IElementPtr AllocWindowCaptionBuilder()
{
	return new CWindowCaptionBuilder();
}

static IElementPtr AllocUIMStatusBarBuilder()
{
	return new CUIMStatusBarBuilder();
}

static IElementPtr AllocFontElement()
{
	return new CFontElement();
}

static IElementPtr AllocBorderElement()
{
	return new CBorderElement();
}

static IElementPtr AllocPreferencesPropertySheetBuilder()
{
	return new CPreferencesPropertySheetBuilder();
}

static IElementPtr AllocMessageWindowBuilder()
{
	return new CMessageWindowBuilder();
}

static IElementPtr AllocHistoryFrameBuilder()
{
	return new CHistoryFrameBuilder();
}

static IElementPtr AllocInfoFrameBuilder()
{
	return new CInfoFrameBuilder();
}

static IElementPtr AllocIconElement()
{
	return new CIconElement();
}

static IElementPtr AllocMenuElement()
{
	return new CMenuElement();
}

static IElementPtr AllocSubMenuElement()
{
	return new CSubMenuElement();
}

static IElementPtr AllocMenuItemElement()
{
	return new CMenuItemElement();
}

static IElementPtr AllocMenuSeparator()
{
	return new CMenuSeparator();
}

static IElementPtr AllocMessageEditPanelBuilder()
{
	return new CMessageEditPanelBuilder();
}

static IElementPtr AllocTabViewBuilder()
{
	return new CTabViewBuilder();
}

static IElementPtr AllocTabButtonBuilder()
{
	return new CTabViewButtonElement();
}

static IElementPtr AllocTabViewLeftImageElement()
{
	return new CTabViewLeftImageElement();
}

static IElementPtr AllocTabViewFillImageElement()
{
	return new CTabViewFillImageElement();
}

static IElementPtr AllocTabViewRightImageElement()
{
	return new CTabViewRightImageElement();
}

static IElementPtr AllocAccountsPropPageBuilder()
{
	return new CAccountsPropPageBuilder();
}

static IElementPtr AllocContactsPropPageBuilder()
{
	return new CContactsPropPageBuilder();
}

static IElementPtr AllocMessagesPropPageBuilder()
{
	return new CMessagesPropPageBuilder();
}

static IElementPtr AllocStatusPropPageBuilder()
{
	return new CStatusPropPageBuilder();
}

static IElementPtr AllocSoundsPropPageBuilder()
{
	return new CSoundsPropPageBuilder();
}

static IElementPtr AllocEmoticonsPropPageBuilder()
{
	return new CEmoticonsPropPageBuilder();
}

static IElementPtr AllocNetworkPropPageBuilder()
{
	return new CNetworkPropPageBuilder();
}

static IElementPtr AllocGeneralPropPageBuilder()
{
	return new CGeneralPropPageBuilder();
}

static IElementPtr AllocProtocolBagListElement()
{
	return new CProtocolBagListElement();
}

static IElementPtr AllocProtocolBagElement()
{
	return new CProtocolBagElement();
}

static IElementPtr AllocProtocolImageListElement()
{
	return new CProtocolImageListElement();
}

static IElementPtr AllocProtocolUserIconElement()
{
	return new CProtocolUserIconElement();
}

static IElementPtr AllocProtocolUserStatusElement()
{
	return new CProtocolUserStatusElement();
}

static IElementPtr AllocProtocolContactIconElement()
{
	return new CProtocolContactIconElement();
}

static IElementPtr AllocProtocolContactStatusElement()
{
	return new CProtocolContactStatusElement();
}

static IElementPtr AllocProtocolStatusButtonElement()
{
	return new CProtocolStatusButtonElement();
}

static IElementPtr AllocProtocolContactBlockedElement()
{
	return new CProtocolContactBlockedElement();
}

static IElementPtr AllocProtocolContactAuthorizationElement()
{
	return new CProtocolContactAuthorizationElement();
}

static IElementPtr AllocNotifyPopUpBuilder()
{
	return new CNotifyPopUpBuilder();
}

static IElementPtr AllocNotifyPopUpCaptionBuilder()
{
	return new CNotifyPopUpCaptionBuilder();
}

static IElementPtr AllocNotifyPopUpImages()
{
	return new CNotifyPopUpImages();
}

static IElementPtr AllocButtonTabCtrlBuilder()
{
	return new CButtonTabCtrlBuilder();
}

static IElementPtr AllocPluginsPropPageBuilder()
{
	return new CPluginsPropPageBuilder();
}

static IElementPtr AllocPluginCustomListCtrl()
{
	return new CPluginCustomListCtrl();
}

static IElementPtr AllocPluginCustomEntryElement()
{
	return new CPluginCustomEntryElement();
}

static IElementPtr AllocPluginCustomGroupElement()
{
	return new CPluginCustomGroupElement();
}

static IElementPtr AllocListSplitPanel()
{
	return new CListSplitPanel();
}

static IElementPtr AllocAccountsPanel()
{
	return new CAccountsPanel();
}

static IElementPtr AllocContactTooltipBuilder()
{
	return new CContactTooltipBuilder();
}

static IElementPtr AllocPluginTooltipBuilder()
{
	return new CPluginTooltipBuilder();
}

static IElementPtr AllocFrameWindowBuilder()
{
	return new CFrameWindowBuilder();
}

static IElementPtr AllocEmoticonsFrameBuilder()
{
	return new CEmoticonsFrameBuilder();
}

static IElementPtr AllocMultiStateImageButtonBuilder()
{
	return new CMultiStateImageButtonBuilder();
}

static IElementPtr AllocButtonStateInfoBuilder()
{
	return new CButtonStateInfoBuilder();
}

static IElementPtr AllocTabCtrlElement()
{
	return new CTabCtrlElement();
}

static IElementPtr AllocContactListTabElement()
{
	return new CContactListTab();
}

static IElementPtr AllocAppDefaultIconElement()
{
	return new CAppDefaultIconElement();
}

static IElementPtr AllocAppAboutBoxElement()
{
	return new CAppAboutBoxElement();
}

static IElementPtr AllocAppCloseAlertElement()
{
	return new CAppCloseAlertElement();
}

static IElementPtr AllocOfflineStatusTrayIconElement()
{
	return new COfflineStatusTrayIconElement();
}

static IElementPtr AllocOnlineStatusTrayIconElement()
{
	return new COnlineStatusTrayIconElement();
}

static IElementPtr AllocAwayStatusTrayIconElement()
{
	return new CAwayStatusTrayIconElement();
}

static IElementPtr AllocBusyStatusTrayIconElement()
{
	return new CBusyStatusTrayIconElement();
}

static IElementPtr AllocIdleStatusTrayIconElement()
{
	return new CIdleStatusTrayIconElement();
}

static IElementPtr AllocInvisibleStatusTrayIconElement()
{
	return new CInvisibleStatusTrayIconElement();
}

static IElementPtr AllocCustomStatusTrayIconElement()
{
	return new CCustomStatusTrayIconElement();
}

static IElementPtr AllocSystemTrayIconListElement()
{
	return new CSystemTrayIconListElement();
}

static IElementPtr AllocConnectionErrorWindowBuilderElement()
{
	return new CConnectionErrorWindowBuilder();
}

static IElementPtr AllocAccountListElement()
{
	return new CAccountListElement();
}

static IElementPtr AllocWizardInitDialog()
{
	return new CWizardInitDialog();
}

static IElementPtr AllocWizardProfileDialog()
{
	return new CWizardProfileDialog();
}

static IElementPtr AllocWizardMSNAccountDialog()
{
	return new CWizardMSNAccountDialog();
}

static IElementPtr AllocWizardUOLAccountDialog()
{
	return new CWizardUOLAccountDialog();
}

static IElementPtr AllocWizardICQAccountDialog()
{
	return new CWizardICQAccountDialog();
}

static IElementPtr AllocWizardJabberAccountDialog()
{
	return new CWizardJabberAccountDialog();
}

static IElementPtr AllocWizardYahooAccountDialog()
{
	return new CWizardYahooAccountDialog();
}

static IElementPtr AllocWizardPreferencesDialog()
{
	return new CWizardPreferencesDialog();
}

static IElementPtr AllocWizardPersonalizeAccountsDialog()
{
	return new CWizardPersonalizeAccountsDialog();
}

static IElementPtr AllocWizardFinishDialog()
{
	return new CWizardFinishDialog();
}

static IElementPtr AllocWizardSplashDialog()
{
	return new CWizardSplashDialog();
}

static IElementPtr AllocBitmapElement()
{
	return new CBitmapElement();
}

static IElementPtr AllocContactNodeLayoutBitmapElement()
{
	return new CContactNodeLayoutBitmapElement();
}

static IElementPtr AllocMessageBoxBuilder()
{
	return new CMessageBoxBuilder();
}

static IElementPtr AllocDefaultEmoticonImageElement()
{
	return new CDefaultEmoticonImageElement();
}

static IElementPtr AllocFindContactPanel()
{
	return new CFindContactPanel();
}

static IElementPtr AllocAddCustomEmoticonDialogBuilder()
{
	return new CAddCustomEmoticonDialogBuilder();
}

static IElementPtr AllocChatListCtrlBuilder()
{
	return new CChatListCtrlBuilder();
}

static IElementPtr AllocReadyForChatListCtrlBuilder()
{
	return new CReadyForChatListCtrlBuilder();
}

static IElementPtr AllocConversationWindowVerticalSplitterBuilder()
{
	return new CConversationWindowVerticalSplitterBuilder();
}

static IElementPtr AllocConversationWindowHorizontalSplitterBuilder()
{
	return new CConversationWindowHorizontalSplitterBuilder();
}

static IElementPtr AllocTitlePanelBuilder()
{
	return new CTitlePanelBuilder();
}

static IElementPtr AllocFileTransferFrameBuilder()
{
	return new CFileTransferFrameBuilder();
}

static IElementPtr AllocFileTransferListCtrl()
{
	return new CFileTransferListCtrl();
}

static IElementPtr AllocFileTransferEntryElement()
{
	return new CFileTransferEntryElement();
}

static IElementPtr AllocFileTransferGroupElement()
{
	return new CFileTransferGroupElement();
}

static IElementPtr AllocFileTransferDefaultPainterElement()
{
	return new CFileTransferDefaultPainterElement();
}

static IElementPtr AllocFileTransferSelectionPainterElement()
{
	return new CFileTransferSelectionPainterElement();
}

static IElementPtr AllocPluginListCtrl()
{
	return new CPluginListCtrl();
}

static IElementPtr AllocPluginEntryElement()
{
	return new CPluginEntryElement();
}

static IElementPtr AllocPluginGroupElement()
{
	return new CPluginGroupElement();
}

static IElementPtr AllocPluginDefaultPainterElement()
{
	return new CPluginDefaultPainterElement();
}

static IElementPtr AllocPluginSelectionPainterElement()
{
	return new CPluginSelectionPainterElement();
}

static IElementPtr AllocChatInviteDialogBuilder()
{
	return new CChatInviteDialogBuilder();
}

static IElementPtr AllocChatInviteListCtrlBuilder()
{
	return new CChatInviteListCtrlBuilder();
}

static IElementPtr AllocFindContactPanelBuilder()
{
	return new CFindContactPanelBuilder();
}

static IElementPtr AllocProgressBarElement()
{
	return new CProgressBarElement();
}

static IElementPtr AllocFindPanelButtonBuilder()
{
	return new CFindPanelButtonBuilder();
}

static IElementPtr AllocContactDialogBuilder()
{
	return new CContactDialogBuilder();
}

static IElementPtr AllocInputDialogBuilder()
{
	return new CInputDialogBuilder();
}

static IElementPtr AllocPrivacyDialogBuilder()
{
	return new CPrivacyDialogBuilder();
}

static IElementPtr AllocMessageReceivingDialogBuilder()
{
	return new CMessageReceivingDialogBuilder();
}

static IElementPtr AllocAuthorizeDialogBuilder()
{
	return new CAuthorizeDialogBuilder();
}

static IElementPtr AllocConfirmDlgBuilder()
{
	return new CConfirmDlgBuilder();
}

static IElementPtr AllocPasswordChangeDlgBuilder()
{
	return new CPasswordChangeDlgBuilder();
}

static IElementPtr AllocAlertDialogBuilder()
{
	return new CAlertDialogBuilder();
}

static IElementPtr AllocAboutDlgBuilder()
{
	return new CAboutDlgBuilder();
}

static IElementPtr AllocHotKeysDlgBuilder()
{
	return new CHotKeysDlgBuilder();
}

void CElementFactory::InitializeMapElements()
{
	ms_mapElements.SetAt(CString(CPositionElement::GetClassName()).MakeLower(), AllocPositionElement);
	ms_mapElements.SetAt(CString(CSkinElement::GetClassName()).MakeLower(), AllocSkinElement);
	ms_mapElements.SetAt(CString(CSolidBackgroundElement::GetClassName()).MakeLower(), AllocSolidBackgroundElement);
	ms_mapElements.SetAt(CString(CBitmapBackgroundElement::GetClassName()).MakeLower(), AllocBitmapBackgroundElement);
	ms_mapElements.SetAt(CString(CContactListCtrl::GetClassName()).MakeLower(), AllocContactListCtrl);
	ms_mapElements.SetAt(CString(CGroupElement::GetClassName()).MakeLower(), AllocGroupElement);
	ms_mapElements.SetAt(CString(CContactElement::GetClassName()).MakeLower(), AllocContactElement);
	ms_mapElements.SetAt(CString(CGroupDefaultPainterElement::GetClassName()).MakeLower(), AllocGroupDefaultPainterElement);
	ms_mapElements.SetAt(CString(CGroupSelectionPainterElement::GetClassName()).MakeLower(), AllocGroupSelectionPainterElement);
	ms_mapElements.SetAt(CString(CContactDefaultPainterElement::GetClassName()).MakeLower(), AllocContactDefaultPainterElement);
	ms_mapElements.SetAt(CString(CContactSelectionPainterElement::GetClassName()).MakeLower(), AllocContactSelectionPainterElement);
	ms_mapElements.SetAt(CString(COnlinePainterElement::GetClassName()).MakeLower(), AllocOnlinePainterElement);
	ms_mapElements.SetAt(CString(COfflinePainterElement::GetClassName()).MakeLower(), AllocOfflinePainterElement);
	ms_mapElements.SetAt(CString(CTypingPainterElement::GetClassName()).MakeLower(), AllocTypingPainterElement);
	ms_mapElements.SetAt(CString(CNewMessagePainterElement::GetClassName()).MakeLower(), AllocNewMessagePainterElement);
	ms_mapElements.SetAt(CString(CStaticIconElement::GetClassName()).MakeLower(), AllocStaticIconElement);
	ms_mapElements.SetAt(CString(CUserStatusPanel::GetClassName()).MakeLower(), AllocUserStatusPanel);
	ms_mapElements.SetAt(CString(CUOLToolBarBuilder::GetClassName()).MakeLower(), AllocUOLToolBarBuilder);
	ms_mapElements.SetAt(CString(CImageButtonBuilder::GetClassName()).MakeLower(), AllocImageButtonBuilder);
	ms_mapElements.SetAt(CString(CAppWindow::GetClassName()).MakeLower(), AllocAppWindow);
	ms_mapElements.SetAt(CString(CWindowCaptionBuilder::GetClassName()).MakeLower(), AllocWindowCaptionBuilder);
	ms_mapElements.SetAt(CString(CUIMStatusBarBuilder::GetClassName()).MakeLower(), AllocUIMStatusBarBuilder);
	ms_mapElements.SetAt(CString(CFontElement::GetClassName()).MakeLower(), AllocFontElement);
	ms_mapElements.SetAt(CString(CBorderElement::GetClassName()).MakeLower(), AllocBorderElement);
	ms_mapElements.SetAt(CString(CPreferencesPropertySheetBuilder::GetClassName()).MakeLower(), AllocPreferencesPropertySheetBuilder);
	ms_mapElements.SetAt(CString(CMessageWindowBuilder::GetClassName()).MakeLower(), AllocMessageWindowBuilder);
	ms_mapElements.SetAt(CString(CHistoryFrameBuilder::GetClassName()).MakeLower(), AllocHistoryFrameBuilder);
	ms_mapElements.SetAt(CString(CInfoFrameBuilder::GetClassName()).MakeLower(), AllocInfoFrameBuilder);
	ms_mapElements.SetAt(CString(CIconElement::GetClassName()).MakeLower(), AllocIconElement);
	ms_mapElements.SetAt(CString(CMenuElement::GetClassName()).MakeLower(), AllocMenuElement);
	ms_mapElements.SetAt(CString(CSubMenuElement::GetClassName()).MakeLower(), AllocSubMenuElement);
	ms_mapElements.SetAt(CString(CMenuItemElement::GetClassName()).MakeLower(), AllocMenuItemElement);
	ms_mapElements.SetAt(CString(CMenuSeparator::GetClassName()).MakeLower(), AllocMenuSeparator);
	ms_mapElements.SetAt(CString(CMessageEditPanelBuilder::GetClassName()).MakeLower(), AllocMessageEditPanelBuilder);
	ms_mapElements.SetAt(CString(CTabViewBuilder::GetClassName()).MakeLower(), AllocTabViewBuilder);
	ms_mapElements.SetAt(CString(CTabViewButtonElement::GetClassName()).MakeLower(), AllocTabButtonBuilder);
	ms_mapElements.SetAt(CString(CTabViewLeftImageElement::GetClassName()).MakeLower(), AllocTabViewLeftImageElement);
	ms_mapElements.SetAt(CString(CTabViewFillImageElement::GetClassName()).MakeLower(), AllocTabViewFillImageElement);
	ms_mapElements.SetAt(CString(CTabViewRightImageElement::GetClassName()).MakeLower(), AllocTabViewRightImageElement);
	ms_mapElements.SetAt(CString(CAccountsPropPageBuilder::GetClassName()).MakeLower(), AllocAccountsPropPageBuilder);
	ms_mapElements.SetAt(CString(CContactsPropPageBuilder::GetClassName()).MakeLower(), AllocContactsPropPageBuilder);
	ms_mapElements.SetAt(CString(CMessagesPropPageBuilder::GetClassName()).MakeLower(), AllocMessagesPropPageBuilder);
	ms_mapElements.SetAt(CString(CStatusPropPageBuilder::GetClassName()).MakeLower(), AllocStatusPropPageBuilder);
	ms_mapElements.SetAt(CString(CSoundsPropPageBuilder::GetClassName()).MakeLower(), AllocSoundsPropPageBuilder);
	ms_mapElements.SetAt(CString(CEmoticonsPropPageBuilder::GetClassName()).MakeLower(), AllocEmoticonsPropPageBuilder);
	ms_mapElements.SetAt(CString(CNetworkPropPageBuilder::GetClassName()).MakeLower(), AllocNetworkPropPageBuilder);
	ms_mapElements.SetAt(CString(CGeneralPropPageBuilder::GetClassName()).MakeLower(), AllocGeneralPropPageBuilder);
	ms_mapElements.SetAt(CString(CProtocolBagListElement::GetClassName()).MakeLower(), AllocProtocolBagListElement);
	ms_mapElements.SetAt(CString(CProtocolBagElement::GetClassName()).MakeLower(), AllocProtocolBagElement);
	ms_mapElements.SetAt(CString(CProtocolImageListElement::GetClassName()).MakeLower(), AllocProtocolImageListElement);
	ms_mapElements.SetAt(CString(CProtocolUserIconElement::GetClassName()).MakeLower(), AllocProtocolUserIconElement);
	ms_mapElements.SetAt(CString(CProtocolUserStatusElement::GetClassName()).MakeLower(), AllocProtocolUserStatusElement);
	ms_mapElements.SetAt(CString(CProtocolContactIconElement::GetClassName()).MakeLower(), AllocProtocolContactIconElement);
	ms_mapElements.SetAt(CString(CProtocolContactStatusElement::GetClassName()).MakeLower(), AllocProtocolContactStatusElement);
	ms_mapElements.SetAt(CString(CProtocolStatusButtonElement::GetClassName()).MakeLower(), AllocProtocolStatusButtonElement);
	ms_mapElements.SetAt(CString(CProtocolContactBlockedElement::GetClassName()).MakeLower(), AllocProtocolContactBlockedElement);
	ms_mapElements.SetAt(CString(CProtocolContactAuthorizationElement::GetClassName()).MakeLower(), AllocProtocolContactAuthorizationElement);
	ms_mapElements.SetAt(CString(CNotifyPopUpBuilder::GetClassName()).MakeLower(), AllocNotifyPopUpBuilder);
	ms_mapElements.SetAt(CString(CNotifyPopUpCaptionBuilder::GetClassName()).MakeLower(), AllocNotifyPopUpCaptionBuilder);
	ms_mapElements.SetAt(CString(CNotifyPopUpImages::GetClassName()).MakeLower(), AllocNotifyPopUpImages);
	ms_mapElements.SetAt(CString(CButtonTabCtrlBuilder::GetClassName()).MakeLower(), AllocButtonTabCtrlBuilder);
	ms_mapElements.SetAt(CString(CPluginsPropPageBuilder::GetClassName()).MakeLower(), AllocPluginsPropPageBuilder);
	ms_mapElements.SetAt(CString(CPluginCustomListCtrl::GetClassName()).MakeLower(), AllocPluginCustomListCtrl);
	ms_mapElements.SetAt(CString(CPluginCustomEntryElement::GetClassName()).MakeLower(), AllocPluginCustomEntryElement);
	ms_mapElements.SetAt(CString(CPluginCustomGroupElement::GetClassName()).MakeLower(), AllocPluginCustomGroupElement);
	ms_mapElements.SetAt(CString(CListSplitPanel::GetClassName()).MakeLower(), AllocListSplitPanel);
	ms_mapElements.SetAt(CString(CAccountsPanel::GetClassName()).MakeLower(), AllocAccountsPanel);
	ms_mapElements.SetAt(CString(CContactTooltipBuilder::GetClassName()).MakeLower(), AllocContactTooltipBuilder);
	ms_mapElements.SetAt(CString(CPluginTooltipBuilder::GetClassName()).MakeLower(), AllocPluginTooltipBuilder);
	ms_mapElements.SetAt(CString(CFrameWindowBuilder::GetClassName()).MakeLower(), AllocFrameWindowBuilder);
	ms_mapElements.SetAt(CString(CEmoticonsFrameBuilder::GetClassName()).MakeLower(), AllocEmoticonsFrameBuilder);
	ms_mapElements.SetAt(CString(CMultiStateImageButtonBuilder::GetClassName()).MakeLower(), AllocMultiStateImageButtonBuilder);
	ms_mapElements.SetAt(CString(CButtonStateInfoBuilder::GetClassName()).MakeLower(), AllocButtonStateInfoBuilder);
	ms_mapElements.SetAt(CString(CTabCtrlElement::GetClassName()).MakeLower(), AllocTabCtrlElement);
	ms_mapElements.SetAt(CString(CContactListTab::GetClassName()).MakeLower(), AllocContactListTabElement);
	ms_mapElements.SetAt(CString(CAppDefaultIconElement::GetClassName()).MakeLower(), AllocAppDefaultIconElement);
	ms_mapElements.SetAt(CString(CAppAboutBoxElement::GetClassName()).MakeLower(), AllocAppAboutBoxElement);
	ms_mapElements.SetAt(CString(CAppCloseAlertElement::GetClassName()).MakeLower(), AllocAppCloseAlertElement);
	ms_mapElements.SetAt(CString(COfflineStatusTrayIconElement::GetClassName()).MakeLower(), AllocOfflineStatusTrayIconElement);
	ms_mapElements.SetAt(CString(COnlineStatusTrayIconElement::GetClassName()).MakeLower(), AllocOnlineStatusTrayIconElement);
	ms_mapElements.SetAt(CString(CAwayStatusTrayIconElement::GetClassName()).MakeLower(), AllocAwayStatusTrayIconElement);
	ms_mapElements.SetAt(CString(CBusyStatusTrayIconElement::GetClassName()).MakeLower(), AllocBusyStatusTrayIconElement);
	ms_mapElements.SetAt(CString(CIdleStatusTrayIconElement::GetClassName()).MakeLower(), AllocIdleStatusTrayIconElement);
	ms_mapElements.SetAt(CString(CInvisibleStatusTrayIconElement::GetClassName()).MakeLower(), AllocInvisibleStatusTrayIconElement);
	ms_mapElements.SetAt(CString(CCustomStatusTrayIconElement::GetClassName()).MakeLower(), AllocCustomStatusTrayIconElement);
	ms_mapElements.SetAt(CString(CSystemTrayIconListElement::GetClassName()).MakeLower(), AllocSystemTrayIconListElement);
	ms_mapElements.SetAt(CString(CConnectionErrorWindowBuilder::GetClassName()).MakeLower(), AllocConnectionErrorWindowBuilderElement);
	ms_mapElements.SetAt(CString(CAccountListElement::GetClassName()).MakeLower(), AllocAccountListElement);
	ms_mapElements.SetAt(CString(CWizardInitDialog::GetClassName()).MakeLower(), AllocWizardInitDialog);
	ms_mapElements.SetAt(CString(CWizardProfileDialog::GetClassName()).MakeLower(), AllocWizardProfileDialog);
	ms_mapElements.SetAt(CString(CWizardMSNAccountDialog::GetClassName()).MakeLower(), AllocWizardMSNAccountDialog);
	ms_mapElements.SetAt(CString(CWizardUOLAccountDialog::GetClassName()).MakeLower(), AllocWizardUOLAccountDialog);
	ms_mapElements.SetAt(CString(CWizardICQAccountDialog::GetClassName()).MakeLower(), AllocWizardICQAccountDialog);
	ms_mapElements.SetAt(CString(CWizardJabberAccountDialog::GetClassName()).MakeLower(), AllocWizardJabberAccountDialog);
	ms_mapElements.SetAt(CString(CWizardYahooAccountDialog::GetClassName()).MakeLower(), AllocWizardYahooAccountDialog);
	ms_mapElements.SetAt(CString(CWizardPreferencesDialog::GetClassName()).MakeLower(), AllocWizardPreferencesDialog);
	ms_mapElements.SetAt(CString(CWizardPersonalizeAccountsDialog::GetClassName()).MakeLower(), AllocWizardPersonalizeAccountsDialog);
	ms_mapElements.SetAt(CString(CWizardFinishDialog::GetClassName()).MakeLower(), AllocWizardFinishDialog);
	ms_mapElements.SetAt(CString(CWizardSplashDialog::GetClassName()).MakeLower(), AllocWizardSplashDialog);
    ms_mapElements.SetAt(CString(CBitmapElement::GetClassName()).MakeLower(), AllocBitmapElement);
    ms_mapElements.SetAt(CString(CContactNodeLayoutBitmapElement::GetClassName()).MakeLower(), AllocContactNodeLayoutBitmapElement);
    ms_mapElements.SetAt(CString(CMessageBoxBuilder::GetClassName()).MakeLower(), AllocMessageBoxBuilder);
    ms_mapElements.SetAt(CString(CDefaultEmoticonImageElement::GetClassName()).MakeLower(), AllocDefaultEmoticonImageElement);
    ms_mapElements.SetAt(CString(CFindContactPanel::GetClassName()).MakeLower(), AllocFindContactPanel);
    ms_mapElements.SetAt(CString(CAddCustomEmoticonDialogBuilder::GetClassName()).MakeLower(), AllocAddCustomEmoticonDialogBuilder);
    ms_mapElements.SetAt(CString(CReadyForChatListCtrlBuilder::GetClassName()).MakeLower(), AllocReadyForChatListCtrlBuilder);
    ms_mapElements.SetAt(CString(CChatListCtrlBuilder::GetClassName()).MakeLower(), AllocChatListCtrlBuilder);
    ms_mapElements.SetAt(CString(CConversationWindowVerticalSplitterBuilder::GetClassName()).MakeLower(), AllocConversationWindowVerticalSplitterBuilder);
    ms_mapElements.SetAt(CString(CConversationWindowHorizontalSplitterBuilder::GetClassName()).MakeLower(), AllocConversationWindowHorizontalSplitterBuilder);
    ms_mapElements.SetAt(CString(CTitlePanelBuilder::GetClassName()).MakeLower(), AllocTitlePanelBuilder);
	ms_mapElements.SetAt(CString(CFileTransferFrameBuilder::GetClassName()).MakeLower(), AllocFileTransferFrameBuilder);
	ms_mapElements.SetAt(CString(CFileTransferListCtrl::GetClassName()).MakeLower(), AllocFileTransferListCtrl);
	ms_mapElements.SetAt(CString(CFileTransferEntryElement::GetClassName()).MakeLower(), AllocFileTransferEntryElement);
	ms_mapElements.SetAt(CString(CFileTransferGroupElement::GetClassName()).MakeLower(), AllocFileTransferGroupElement);
	ms_mapElements.SetAt(CString(CFileTransferDefaultPainterElement::GetClassName()).MakeLower(), AllocFileTransferDefaultPainterElement);
	ms_mapElements.SetAt(CString(CFileTransferSelectionPainterElement::GetClassName()).MakeLower(), AllocFileTransferSelectionPainterElement);
	ms_mapElements.SetAt(CString(CPluginListCtrl::GetClassName()).MakeLower(), AllocPluginListCtrl);
	ms_mapElements.SetAt(CString(CPluginEntryElement::GetClassName()).MakeLower(), AllocPluginEntryElement);
	ms_mapElements.SetAt(CString(CPluginGroupElement::GetClassName()).MakeLower(), AllocPluginGroupElement);
	ms_mapElements.SetAt(CString(CPluginDefaultPainterElement::GetClassName()).MakeLower(), AllocPluginDefaultPainterElement);
	ms_mapElements.SetAt(CString(CPluginSelectionPainterElement::GetClassName()).MakeLower(), AllocPluginSelectionPainterElement);
	ms_mapElements.SetAt(CString(CChatInviteDialogBuilder::GetClassName()).MakeLower(), AllocChatInviteDialogBuilder);
	ms_mapElements.SetAt(CString(CChatInviteListCtrlBuilder::GetClassName()).MakeLower(), AllocChatInviteListCtrlBuilder);
	ms_mapElements.SetAt(CString(CFindContactPanelBuilder::GetClassName()).MakeLower(), AllocFindContactPanelBuilder);
	ms_mapElements.SetAt(CString(CProgressBarElement::GetClassName()).MakeLower(), AllocProgressBarElement);
	ms_mapElements.SetAt(CString(CFindPanelButtonBuilder::GetClassName()).MakeLower(), AllocFindPanelButtonBuilder);
	ms_mapElements.SetAt(CString(CContactDialogBuilder::GetClassName()).MakeLower(), AllocContactDialogBuilder);
	ms_mapElements.SetAt(CString(CInputDialogBuilder::GetClassName()).MakeLower(), AllocInputDialogBuilder);
	ms_mapElements.SetAt(CString(CPrivacyDialogBuilder::GetClassName()).MakeLower(), AllocPrivacyDialogBuilder);
	ms_mapElements.SetAt(CString(CMessageReceivingDialogBuilder::GetClassName()).MakeLower(), AllocMessageReceivingDialogBuilder);
	ms_mapElements.SetAt(CString(CAuthorizeDialogBuilder::GetClassName()).MakeLower(), AllocAuthorizeDialogBuilder);
	ms_mapElements.SetAt(CString(CConfirmDlgBuilder::GetClassName()).MakeLower(), AllocConfirmDlgBuilder);
	ms_mapElements.SetAt(CString(CPasswordChangeDlgBuilder::GetClassName()).MakeLower(), AllocPasswordChangeDlgBuilder);
	ms_mapElements.SetAt(CString(CAlertDialogBuilder::GetClassName()).MakeLower(), AllocAlertDialogBuilder);
	ms_mapElements.SetAt(CString(CAboutDlgBuilder::GetClassName()).MakeLower(), AllocAboutDlgBuilder);
	ms_mapElements.SetAt(CString(CHotKeysDlgBuilder::GetClassName()).MakeLower(), AllocHotKeysDlgBuilder);
}
