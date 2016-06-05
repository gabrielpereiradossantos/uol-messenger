

#pragma once


#include "WTLWndComponentsOrganizer.h"
#include "UOLFoneStatic.h"


class CUOLFoneOrganizedColoredPanel : public CUOLFoneStatic
{
public:
	
	CUOLFoneOrganizedColoredPanel(void);
	virtual ~CUOLFoneOrganizedColoredPanel(void);

	void Initialize(int leftShift = 0, int topShift = 0, 
			int rightShift = 0, int bottomShift = 0, 
			int verticalAlignment = CWTLWndComponentsOrganizer::WINDOW_ALIGNMENT_TOP);
	
	void SetLeftShift(int leftShift);
	void SetTopShift(int topShift);
	void SetRightShift(int rightShift);
	void SetBottomShift(int bottomShift);
	void SetVerticalAlignment(int verticalAlignment);
	
	INT  AddLine(int componentSpace = 0, 
			int orientation = CWTLWndComponentsOrganizer::LINE_ORIENTATION_LEFT, 
			int vOrientation = CWTLWndComponentsOrganizer::LINE_VORIENTATION_BOTTOM, 
			BOOL fillUp = FALSE);
	INT  AddSeparatorLine(int height);
	INT  AddComponent(CWindow *component, LPCSTR wndClassName, int width = 0, 
			int maximmumHeight = 0, int minimmumHeight = 0, BOOL fillUp = FALSE);
	
	void ShowLineComponents(BOOL visible, POSITION linePos);
	void OrganizeComponents(BOOL forceLineTopsRecalculation = FALSE);
	void RecalculateLinesHeightsAndWidths();
	
	POSITION GetLastAddedLinePosition();
	
	void GetComponentClientPosition(CWindow component, CRect& clientPosition);
	
	int	GetMinimumWindowWidth();
	int	GetMinimumWindowHeight(BOOL bConsiderHideElements = TRUE);
	
	int GetLastLineHeight();
	
protected:
	
	BEGIN_MSG_MAP(CUOLFoneOrganizedColoredPanel)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		CHAIN_MSG_MAP(CUOLFoneStatic)
	END_MSG_MAP()
	
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
private:
	
	CWTLWndComponentsOrganizer			m_componentsOrganizer;
};
