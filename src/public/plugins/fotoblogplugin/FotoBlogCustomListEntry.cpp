#include "StdAfx.h"
#include ".\fotoblogcustomlistentry.h"
#include "FotoBlogDllWrapper.h"
//////////////////////////////////////////////////////////////////////////
// CPhotoBlogCustomListEntry
//////////////////////////////////////////////////////////////////////////

CPhotoBlogCustomListEntry::CPhotoBlogCustomListEntry(const CString& strPluginInternalName, const CString& strEntryName) :
m_strPluginInternalName(strPluginInternalName),
m_strEntryName(strEntryName)
{
	m_icon = g_pUOLMessengerFacade->GetUIManager()->LoadImage(_AtlBaseModule.GetModuleInstance(),(IDB_PHOTOBLOG) );
	
	;
	//m_listActions.AddTail(new CPhotoBlogAction(strEntryName + _T("/Abrir...")));
	//m_listActions.AddTail(new CPhotoBlogAction(strEntryName + _T("/Abrir...")));
	//m_listActions.AddTail(new CPhotoBlogAction(strEntryName + _T("/Configurar...")));
	//	m_listActions.AddTail(new CPhotoBlogAction(strEntryName + _T("/Atualizar")));
}


CPhotoBlogCustomListEntry::~CPhotoBlogCustomListEntry()
{
}


CString CPhotoBlogCustomListEntry::GetEntryName() const
{
	return m_strEntryName;
}


CString CPhotoBlogCustomListEntry::GetPluginInternalName() const
{
	return m_strPluginInternalName;
}


IUOLMessengerPainterPtr CPhotoBlogCustomListEntry::GetPainter()
{
	return IUOLMessengerPainterPtr();
}


IUOLMessengerImagePtr CPhotoBlogCustomListEntry::GetIcon(BOOL& bDrawTransparent)
{
	bDrawTransparent = TRUE;

	//return IUOLMessengerImagePtr();
	return m_icon;
}


CString CPhotoBlogCustomListEntry::GetDisplayName()
{
	return m_strEntryName;
}


void CPhotoBlogCustomListEntry::OnLButtonDoubleClick(const CPoint& ptClient)
{
	Show();
}


void CPhotoBlogCustomListEntry::OnReturnKeyPressed()
{
	Show();
}


void CPhotoBlogCustomListEntry::OnSelectionChanged(BOOL bSelected)
{
}


void CPhotoBlogCustomListEntry::GetContextMenuActions(CAtlList<IUOLMessengerPluginActionPtr>& listActions)
{
	listActions.RemoveAll();

	for (POSITION pos = m_listActions.GetHeadPosition(); pos; )
	{
		IUOLMessengerPluginActionPtr pAction = m_listActions.GetNext(pos);
		listActions.AddTail(pAction);
	}
}

void CPhotoBlogCustomListEntry::Show()
{
	g_fotoblog_dll.ShowFotoBlog(g_pUOLMessengerFacade->GetUIManager()->GetAppWindow());
	////if (1 || g_pUOLMessengerFacade->GetAuthenticationManager()->IsAuthenticated()==TRUE) {
	//	IUOLMessengerAuthenticationManagerPtr pAuth;
	//	pAuth = g_pUOLMessengerFacade->GetAuthenticationManager();
	//	if (pAuth.get()!=NULL) {
	//		CPhotoBlogController * pSimple;
	//		pSimple = CPhotoBlogController::GetInstance();
	//		CString cookie = pAuth->GetCookie();
	//		pSimple->SetCookie(cookie);
	//		pSimple->OnAuthenticated(TRUE);
	//		pSimple->Init();
	//		pSimple->DisplayBlogDialog(g_pUOLMessengerFacade->GetUIManager()->GetAppWindow());
	//		//pSimple->CloseBlogDialog();
	//		//pSimple->Finalize();
	//	}
	////}
}



//////////////////////////////////////////////////////////////////////////
// CPhotoBlogAction
//////////////////////////////////////////////////////////////////////////

CPhotoBlogAction::CPhotoBlogAction(const CString& strName) :
m_strName(strName)
{
}


CPhotoBlogAction::~CPhotoBlogAction()
{
}


CString CPhotoBlogAction::GetName() const
{
	return m_strName;
}


void CPhotoBlogAction::DoAction()
{

}
