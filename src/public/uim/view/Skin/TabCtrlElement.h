#pragma once
#include "../DummyWindow.h"
#include "windowelement.h"
 
#include "ElementBuilder.h"
#include "../ImageButton.h"
#include "../ImageButtonContainer.h"


class __declspec(novtable) ITabCtrlItem 
{
public:
	virtual ~ITabCtrlItem() {}

	virtual HWND Create(HWND hwndParent) = 0;
	virtual void SetWindowPos(int x, int y, int cx, int cy);
	virtual void BringWindowToTop() = 0;
	virtual void HideWindow() = 0;
	
	virtual CImageButtonPtr GetButton() const = 0;
	virtual CImageButtonPtr SetButton(CImageButtonPtr pButton) = 0;

};

MAKEAUTOPTR(ITabCtrlItem);

class CTabCtrlItem;
typedef CImageButtonContainer<CTabCtrlItem, CDummyWindow<CWindowImpl<CTabCtrlItem> > > _CTabCtrlItemBase;

//template <typename TBase>
class CTabCtrlItem : public _CTabCtrlItemBase, 
					 public ITabCtrlItem
{
public:
	
protected:
	BEGIN_MSG_MAP(CTabCtrlItem)								
		CHAIN_MSG_MAP(_CTabCtrlItemBase)
	END_MSG_MAP()


	virtual HWND Create(HWND hwndParent)
	{	
		CRect rect = CRect(CPoint(0, 0), CSize(0, 0));

		return __super::Create(hwndParent, rect);
	}

	virtual void SetWindowPos(int x, int y, int cx, int cy)
	{
		__super::SetWindowPos(NULL, 
							  x, y, 
							  cx, cy, 
							  SWP_NOZORDER);
	}

	virtual void BringWindowToTop()
	{
		_CTabCtrlItemBase::BringWindowToTop();
		_CTabCtrlItemBase::ShowWindow(SW_SHOW);
	}

	virtual void HideWindow()
	{
		_CTabCtrlItemBase::ShowWindow(SW_HIDE);
	}

	virtual CImageButtonPtr GetButton() const
	{
		return m_pButton;
	}

	virtual CImageButtonPtr SetButton(CImageButtonPtr pButton)
	{
		CImageButtonPtr pTemp = m_pButton;

		m_pButton = pButton;

		return pTemp;
	}


private:
	CImageButton m_button;
	CImageButtonPtr m_pButton;
};












class CTabCtrlElement;
typedef CWindowElement<CTabCtrlElement, CDummyWindow<CWindowImpl<CTabCtrlElement> > > CTabCtrlElementBase;

class CTabCtrlElement :	public CTabCtrlElementBase, 
					    public CUOLMessengerImageButtonEventListener2

{
public:	
	DECLARE_ELEMENT_NAME("TabCtrl");

	CTabCtrlElement(void);
	virtual ~CTabCtrlElement(void);

	BOOL AddTab(ITabCtrlItemPtr pTab);

	virtual void DoPaint(CDCHandle& dc);
	virtual void UpdateLayout(BOOL bResizeBars = TRUE);

protected:
	BEGIN_MSG_MAP(CTabCtrlElement)			
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)		
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClick)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		MSG_WM_RBUTTONDBLCLK(OnRButtonDblClick)
		MSG_WM_MOVE(OnMouseMove)
		CHAIN_MSG_MAP(CTabCtrlElementBase)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnLButtonDown(UINT, const CPoint& ptClick);
	void OnLButtonUp(UINT, const CPoint& ptClick);
	void OnLButtonDblClick(UINT, const CPoint& ptClick);
	void OnRButtonDown(UINT, const CPoint& ptClick);
	void OnRButtonDblClick(UINT, const CPoint& ptClick);
	void OnMouseMove(const CPoint& ptClick);

	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);

	// CUOLMessengerImageButtonEventListener2
	virtual void OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint& ptClick);
	virtual void OnMouseMove(IUOLMessengerImageButton* pButton, const CPoint& ptClick);

private:
	void DrawTabs(CDCHandle& dc);
	ITabCtrlItemPtr Pt2Tab(const CPoint& ptClick);

private:	
	ITabCtrlItem*			  m_pActiveTab;

	CAtlList<ITabCtrlItemPtr> m_listTabs;
	int m_nHeight;

};

MAKEAUTOPTR(CTabCtrlElement);