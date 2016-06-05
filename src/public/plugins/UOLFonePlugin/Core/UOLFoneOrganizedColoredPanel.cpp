

#include "StdAfx.h"
#include ".\UOLFoneOrganizedColoredPanel.h"


/////////////////////////////////////////////////////////////////////
// CUOLFoneOrganizedColoredPanel.
/////////////////////////////////////////////////////////////////////


CUOLFoneOrganizedColoredPanel::CUOLFoneOrganizedColoredPanel(void)
{
	m_componentsOrganizer.Initialize(this);
}


CUOLFoneOrganizedColoredPanel::~CUOLFoneOrganizedColoredPanel(void)
{
}


/////////////////////////////////////////////////////////////////////
// Organizing methods.
/////////////////////////////////////////////////////////////////////


void CUOLFoneOrganizedColoredPanel::Initialize(int leftShift, 
		int topShift, int rightShift, int bottomShift, int verticalAlignment)
{
	m_componentsOrganizer.Initialize(this, leftShift, topShift, 
			rightShift, bottomShift, verticalAlignment);
}


void CUOLFoneOrganizedColoredPanel::SetLeftShift(int leftShift)
{
	m_componentsOrganizer.SetLeftShift(leftShift);
}


void CUOLFoneOrganizedColoredPanel::SetTopShift(int topShift)
{
	m_componentsOrganizer.SetTopShift(topShift);
}


void CUOLFoneOrganizedColoredPanel::SetRightShift(int rightShift)
{
	m_componentsOrganizer.SetRightShift(rightShift);
}


void CUOLFoneOrganizedColoredPanel::SetBottomShift(int bottomShift)
{
	m_componentsOrganizer.SetBottomShift(bottomShift);
}


void CUOLFoneOrganizedColoredPanel::SetVerticalAlignment(int verticalAlignment)
{
	m_componentsOrganizer.SetVerticalAlignment(verticalAlignment);
}


INT CUOLFoneOrganizedColoredPanel::AddLine(int componentSpace, int orientation, 
		int vOrientation, BOOL fillUp)
{
	return m_componentsOrganizer.AddLine(componentSpace, orientation, vOrientation, fillUp);
}


INT CUOLFoneOrganizedColoredPanel::AddSeparatorLine(int height)
{
	return m_componentsOrganizer.AddSeparatorLine(height);
}


INT CUOLFoneOrganizedColoredPanel::AddComponent(
		CWindow* component, LPCSTR wndClassName, int width,
		int maximmumHeight, int minimmumHeight, BOOL fillUp)
{
	return m_componentsOrganizer.AddComponent(component, wndClassName, width, 
			maximmumHeight, minimmumHeight, fillUp);
}


void CUOLFoneOrganizedColoredPanel::ShowLineComponents(BOOL visible, POSITION linePos)
{
	m_componentsOrganizer.ShowLineComponents(visible, linePos);
}

void CUOLFoneOrganizedColoredPanel::OrganizeComponents(BOOL forceLineTopsRecalculation)
{
	m_componentsOrganizer.OrganizeComponents();
}


void CUOLFoneOrganizedColoredPanel::RecalculateLinesHeightsAndWidths()
{
	m_componentsOrganizer.RecalculateLinesHeightsAndWidths();
}

POSITION CUOLFoneOrganizedColoredPanel::GetLastAddedLinePosition()
{
	return m_componentsOrganizer.GetLastAddedLinePosition();
}

void CUOLFoneOrganizedColoredPanel::GetComponentClientPosition(
		CWindow component, CRect& clientPosition)
{
	m_componentsOrganizer.GetComponentClientPosition(component, clientPosition);
}


int	CUOLFoneOrganizedColoredPanel::GetMinimumWindowWidth()
{
	return m_componentsOrganizer.GetMinimumWindowWidth();
}


int	CUOLFoneOrganizedColoredPanel::GetMinimumWindowHeight(BOOL bConsiderHideElements)
{
	return m_componentsOrganizer.GetMinimumWindowHeight(bConsiderHideElements);
}


int CUOLFoneOrganizedColoredPanel::GetLastLineHeight()
{
	return m_componentsOrganizer.GetLastLineHeight();
}


/////////////////////////////////////////////////////////////////////
// Messages handling.
/////////////////////////////////////////////////////////////////////


LRESULT CUOLFoneOrganizedColoredPanel::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_componentsOrganizer.OrganizeComponents();
	
	return 0L;
}

