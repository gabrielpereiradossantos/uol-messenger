#include "StdAfx.h"
#include ".\tabctrlelement.h"

//class CDummyTab;
//typedef CWindowElement<CDummyTab, CDummyWindow<CWindowImpl<CDummyTab> > > CTabCtrlItemBase;
////typedef CTabCtrlItem<CTabCtrlItemBase> CDummyTabBase;
//typedef CTabCtrlItemBase CDummyTabBase;
//
//class CDummyTab : public CDummyTabBase
//{
//public:
//	virtual void DoPaint(CDCHandle& dc)
//	{
//		CRect rect;
//		GetClientRect(&rect);
//		CSize size(rect.Width(), rect.Height());
//
//		dc.FillRect(&rect, 2);
//		dc.TextOut(20, 60, "Dummy");
//	}
//};
//MAKEAUTOPTR(CDummyTab);
//





CTabCtrlElement::CTabCtrlElement(void) :
	m_nHeight(20), 
	m_pActiveTab(NULL)
{
}

CTabCtrlElement::~CTabCtrlElement(void)
{
}

BOOL CTabCtrlElement::AddTab(ITabCtrlItemPtr pTab)
{
	m_listTabs.AddTail(pTab);

	return TRUE;
}

void CTabCtrlElement::DoPaint(CDCHandle& dc)
{
	DrawTabs(dc);	
}

void CTabCtrlElement::UpdateLayout(BOOL /*bResizeBars*/)
{
	CRect rect;
	GetClientRect(&rect);
	CSize size(rect.Width(), rect.Height());

	if (m_pActiveTab)
	{
		m_pActiveTab->SetWindowPos(0, m_nHeight, 
								   size.cx, size.cy - m_nHeight);			
	}	
}


LRESULT CTabCtrlElement::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CImageButtonBuilderPtr pBuilder = FindElement<CImageButtonBuilderPtr>();
	ATLASSERT(pBuilder.get());

	if (pBuilder.get())
	{
		SetWindowPos(NULL, 
					 0, 0, 
					 lpCreateStruct->cx, m_nHeight, 
					 SWP_NOZORDER);			

		for (POSITION pos = m_listChilds.GetHeadPosition(); pos; )
		{
			POSITION currentPosition = pos;
			ITabCtrlItemPtr pTab = ap_dynamic_cast<ITabCtrlItemPtr>(m_listChilds.GetNext(pos));

			if (pTab.get() == NULL) 
			{
				continue;
			}       

			CWindow window = pTab->Create(m_hWnd);

			if (window.IsWindow())
			{
				CImageButtonPtr pBtnTabImage = ap_static_cast<CImageButtonPtr>(pBuilder->CreateElement());
				ATLASSERT(pBtnTabImage.get());
				pBtnTabImage->AddEventListener(this);

				pTab->SetButton(pBtnTabImage);				
				pBtnTabImage->SetAttachment(pTab.get());

				AddTab(pTab);
			}
			else
			{
				m_listChilds.RemoveAt(currentPosition);
			}
		}

	//// start debug
	//CImageButtonPtr pBtnTabImage = ap_static_cast<CImageButtonPtr>(pBuilder->CreateElement());
	//ATLASSERT(pBtnTabImage.get());
	//pBtnTabImage->AddEventListener(this);

	//ITabCtrlItemPtr pTab = new CDummyTab();
	//
	//pTab->Create(m_hWnd);
	//pTab->SetButton(pBtnTabImage);
	//pBtnTabImage->SetAttachment(pTab.get());

	//AddTab(pTab);
	//// end debug

	}

	//m_pActiveTab = m_listTabs.GetHead().get();	
	m_pActiveTab = m_listTabs.GetTail().get();

	return 0L;
}

void CTabCtrlElement::OnDestroy()
{
	SetMsgHandled(FALSE);
}

void CTabCtrlElement::OnLButtonDown(UINT, const CPoint& ptClick)
{
	ITabCtrlItemPtr pTabCtrlItem = Pt2Tab(ptClick);

	if (pTabCtrlItem.get())
	{
		pTabCtrlItem->GetButton()->OnLButtonDown(ptClick);
	}
}

void CTabCtrlElement::OnLButtonUp(UINT, const CPoint& ptClick)
{
	ITabCtrlItemPtr pTabCtrlItem = Pt2Tab(ptClick);

	if (pTabCtrlItem.get())
	{
		pTabCtrlItem->GetButton()->OnLButtonUp(ptClick);
	}
}

void CTabCtrlElement::OnLButtonDblClick(UINT, const CPoint& ptClick)
{
	ITabCtrlItemPtr pTabCtrlItem = Pt2Tab(ptClick);

	if (pTabCtrlItem.get())
	{
		pTabCtrlItem->GetButton()->OnLButtonDblClick(ptClick);
	}
}

void CTabCtrlElement::OnRButtonDown(UINT, const CPoint& ptClick)
{
	ITabCtrlItemPtr pTabCtrlItem = Pt2Tab(ptClick);

	if (pTabCtrlItem.get())
	{
		pTabCtrlItem->GetButton()->OnRButtonDown(ptClick);
	}
}

void CTabCtrlElement::OnMouseMove(const CPoint& ptClick)
{
	ITabCtrlItemPtr pTabCtrlItem = Pt2Tab(ptClick);

	if (pTabCtrlItem.get())
	{
		pTabCtrlItem->GetButton()->OnMouseMove(ptClick);
	}
}

void CTabCtrlElement::OnRButtonDblClick(UINT, const CPoint& ptClick)
{
	ITabCtrlItemPtr pTabCtrlItem = Pt2Tab(ptClick);

	if (pTabCtrlItem.get())
	{
		pTabCtrlItem->GetButton()->OnRButtonDblClick(ptClick);
	}
}

void CTabCtrlElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
	//__super::SetParameters(pMapParams, pElementTable);		

	try
	{
		GetParameterValue(m_nHeight, pMapParams, "height");
	}
	catch(const CSkinException&)
	{
	}
}


void CTabCtrlElement::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint& ptClick)
{
	ITabCtrlItem* pTab = reinterpret_cast<ITabCtrlItem*>(static_cast<IUOLMessengerImageButton2*>(pButton)->GetAttachment());

	m_pActiveTab->HideWindow();

	m_pActiveTab = pTab;

	m_pActiveTab->BringWindowToTop();

	UpdateLayout();
}

void CTabCtrlElement::OnMouseMove(IUOLMessengerImageButton* pButton, const CPoint& ptClick)
{
}


void CTabCtrlElement::DrawTabs(CDCHandle& dc)
{
	const int nNumberOftabs = m_listTabs.GetCount();

	if (nNumberOftabs <= 0)
	{
		return;
	}

	CRect rect;
	GetClientRect(&rect);
	CSize size(rect.Width(), rect.Height());

	int x = 0;
	const int cx =  size.cx / nNumberOftabs;

	for (POSITION pos = m_listTabs.GetHeadPosition(); pos; )
	{
		POSITION currentPosition = pos;
		ITabCtrlItemPtr pTab = ap_static_cast<ITabCtrlItemPtr>(m_listTabs.GetNext(pos));		

		CImageButtonPtr pButton = pTab->GetButton();

		pButton->Draw(dc, x, 0, cx, m_nHeight, CImageButton::MouseOut);		
	}
}

ITabCtrlItemPtr CTabCtrlElement::Pt2Tab(const CPoint& ptClick)
{	
	CRect rect;
	GetClientRect(&rect);
	CSize size(rect.Width(), rect.Height());

	size_t nCount = m_listTabs.GetCount();

	if ((nCount) && (ptClick.x > 0))
	{
		size_t nTabSize = size.cx / nCount;
		size_t nInd = ptClick.x / nTabSize;

		nInd = (nInd < nCount) ? nInd : 0;
		
		return m_listTabs.GetAt(m_listTabs.FindIndex(nInd));
	}

	return ITabCtrlItemPtr();
}