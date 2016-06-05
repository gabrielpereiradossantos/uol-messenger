#pragma once

#include <atlstr.h>
#include <atltypes.h>
#include "image/image.h"
#include "image/imagebuilder.h"

#include "interfaces/IUOLMessengerImagesProtocol.h"

template <typename TBase>
class CUOLOwnerDraw : 
	public CWindowImpl<CUOLOwnerDraw<TBase>, TBase>,
	public COwnerDraw<CUOLOwnerDraw<TBase> >
{
	BEGIN_MSG_MAP(CUOLOwnerDraw)
		CHAIN_MSG_MAP_ALT(COwnerDraw<CUOLOwnerDraw>, 1)
	END_MSG_MAP()

	CUOLOwnerDraw()
	{
		m_pImagesProtocol = CUIMApplication::GetApplication()->GetUIManager()->GetImagesProtocol();
	}

	CUOLOwnerDraw & operator = (HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}	

	void DrawItem(LPDRAWITEMSTRUCT lpdis)
	{
		CDCHandle dc = lpdis->hDC;
		CDC dcMem;

		dcMem.CreateCompatibleDC ( dc );
		dc.SaveDC();
		dcMem.SaveDC();

		CString text = "camp",str;

		TBase* T = static_cast<TBase*>(this);
		{
			int n = static_cast<TBase*>(this)->GetLBTextLen( lpdis->itemID );
			static_cast<TBase*>(this)->GetLBText( lpdis->itemID , str.GetBuffer(n) );
			text = str;
			str.ReleaseBuffer();
		}

		CIMProtocol* pProtocol;
		pProtocol = (CIMProtocol*) lpdis->itemData;

		CRect lineRect (lpdis->rcItem);
		CRect picRect (lineRect);

		picRect.right = picRect.left+16;
		
		if (lpdis->itemState & ODS_SELECTED) 
		{
			COLORREF bkColor = GetSysColor(COLOR_HIGHLIGHT);
			COLORREF txtColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
			dc.SetBkColor(bkColor);
			dc.SetTextColor(txtColor);
		}

		if ( lpdis->itemState & ODS_SELECTED ) 
		{
			dc.BitBlt ( 1, 1, 80, 80, dcMem, 0, 0, SRCCOPY );
		}
		else
		{
			dc.BitBlt ( 0, 0, 80, 80, dcMem, 0, 0, SRCCOPY );
		}
		
		CRect tmpRect(lineRect);
		tmpRect.left+=20;
		dc.DrawText(text,text.GetLength(),tmpRect,DT_LEFT|DT_END_ELLIPSIS|DT_VCENTER);

		IUOLMessengerImagePtr m_pImage = m_pImagesProtocol->GetUserStatusImage(pProtocol->GetId().c_str(), 0);

		if (m_pImage.get()) 				
		{
			int nPicLength = min(picRect.Width(), picRect.Height());

			//ATLTRACE(_T("%s, pictRet = [%d,%d]x[%d,%d] (%d x %d)\n"), __FUNCTION__, picRect.left, picRect.right, picRect.top, picRect.bottom, picRect.Width(), picRect.Height());

			IUOLMessengerImageDrawStrategyPtr pStrategy = 
				new UOLMessengerImage::CDrawStretchTransparentStrategy();

			m_pImage->Draw(dc, picRect.left, picRect.top, nPicLength, nPicLength, pStrategy);	
		} 
		else
		{
			ATLTRACE(_T("CHistoryLogsView::DrawItem WARNING !NOT FOUND PROTOCOL!\n"));	
		}

		dcMem.RestoreDC(-1);
		dc.RestoreDC(-1);
	}

private:
	IUOLMessengerImagesProtocolPtr m_pImagesProtocol;
};



typedef class CUOLOwnerDraw<CListBox> CUOLListBox;
typedef class CUOLOwnerDraw<CComboBox> CUOLComboBox;