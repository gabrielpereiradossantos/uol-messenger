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
#include ".\WTLWndComponentsOrganizer.h"
#include ".\AutoResizeStatic.h"


CWTLWndComponentsOrganizer::CWTLWndComponentsOrganizer() : 
		m_containerWindow(NULL), 
		m_leftShift(0), 
		m_topShift(0), 
		m_rightShift(0), 
		m_bottomShift(0), 
		m_verticalAlignment(WINDOW_ALIGNMENT_TOP), 
		m_lastLine(NULL), 
		m_hasOrganizedOnce(FALSE), 
		m_hasFillUpLine(FALSE), 
		m_lastLinePos(NULL), 
		m_lastComponentPos(NULL), 
		m_changedVisibility(FALSE), 
		m_bIsTopMostOrganizer(FALSE)
{
}


CWTLWndComponentsOrganizer::~CWTLWndComponentsOrganizer(void)
{
	// Clear the line's component list.
	LineInfo*		pLineInfo = NULL;
	ComponentInfo*	pComponentInfo = NULL;
	
	for (POSITION linePos = m_linesList.GetHeadPosition(); linePos; )
	{
		pLineInfo = m_linesList.GetNext(linePos);
		
		for (POSITION componentPos = pLineInfo->componentsList.GetHeadPosition(); componentPos; )
		{
			pComponentInfo = pLineInfo->componentsList.GetNext(componentPos);
			
			delete pComponentInfo;
		}
		
		pLineInfo->componentsList.RemoveAll();
		
		delete pLineInfo;
	}
	
	// Now clear the list of lines.
	m_linesList.RemoveAll();
	
	m_containerWindow = NULL;
	m_lastLine = NULL;
}


/////////////////////////////////////////////////////////////////////
// Public members
/////////////////////////////////////////////////////////////////////


void CWTLWndComponentsOrganizer::Initialize(CWindow* containerWindow, int leftShift, 
											int topShift, int rightShift, 
											int bottomShift, int verticalAlignment, 
											BOOL bIsTopMostOrganizer)
{
	m_containerWindow = containerWindow;
	
	m_leftShift = leftShift;
	m_topShift = topShift;
	m_rightShift = rightShift;
	m_bottomShift = bottomShift;
	m_verticalAlignment = verticalAlignment;
	
	m_bIsTopMostOrganizer = bIsTopMostOrganizer;
}


void CWTLWndComponentsOrganizer::SetBoundaries(int leftShift, int topShift, 
		int rightShift, int bottomShift)
{
	m_leftShift = leftShift;
	m_topShift = topShift;
	m_rightShift = rightShift;
	m_bottomShift = bottomShift;
}


void CWTLWndComponentsOrganizer::SetLeftShift(int leftShift)
{
	m_leftShift = leftShift;
}


void CWTLWndComponentsOrganizer::SetTopShift(int topShift)
{
	m_topShift = topShift;
}


void CWTLWndComponentsOrganizer::SetRightShift(int rightShift)
{
	m_rightShift = rightShift;
}


void CWTLWndComponentsOrganizer::SetBottomShift(int bottomShift)
{
	m_bottomShift = bottomShift;
}


void CWTLWndComponentsOrganizer::SetVerticalAlignment(int verticalAlignment)
{
	m_verticalAlignment = verticalAlignment;
}


/**
 * AddComponent()
 * 
 * Adiciona uma linha ao organizador.
 * 
 * @param	componentSpace		Specify the space between the components added 
 *								to this line.
 * 
 * @param	orientation			indicates whether the line will have a left, 
 *								centered, or right alignment. When the right 
 *								align is used, the components must be added 
 *								in the inversed order, considering the default 
 *								left to right orientation.
 * 
 * @param	verticalOrientation	indicates whether the line will have a bottom, 
 *								centered, or top vertical alignment.
 * 
 * @param	fillUp				whether the element will fillUp the remaining 
 *								space, i.e., the space not occupied by other 
 *								lines that belong to this organizator.
 * 
 * @param   fillUpFactor		the factor to be used when calculating the 
 *								amount of remaining space that will be used 
 *								by this line.
 * 
 * @return ERROR_INVALID_PARAMETER Se a orientacao informada nao for valida.
 * 
 */
INT CWTLWndComponentsOrganizer::AddLine(int componentSpace, int orientation, 
										int verticalOrientation, 
										BOOL fillUp, int fillUpFactor)
{
	if ( (orientation < LINE_ORIENTATION_LEFT) || 
			(orientation > LINE_ORIENTATION_CENTERED) || 
			(verticalOrientation < LINE_VORIENTATION_BOTTOM) || 
			(verticalOrientation > LINE_VORIENTATION_CENTERED) || 
			( (verticalOrientation == LINE_VORIENTATION_CENTERED) && (fillUp) ) )
	{
		return ERROR_INVALID_PARAMETER;
	}
	
	m_lastLine = CreateAndInitializeLineInfo();
	
	if (m_lastLine == NULL)
	{
		return ERROR_NOT_ENOUGH_MEMORY;
	}
	
	m_lastLine->lineType = LINE_TYPE_COMPONENT;
	m_lastLine->componentSpace = componentSpace;
	m_lastLine->lineOrientation = orientation;
	m_lastLine->lineVOrientation = verticalOrientation;
	m_lastLine->fillUpLine = fillUp;
	m_lastLine->fillUpFactor = fillUpFactor;
	
	m_hasFillUpLine = m_hasFillUpLine || fillUp;
	
	m_lastLinePos = m_linesList.AddTail(m_lastLine);
	
	return ERROR_SUCCESS;
}


INT CWTLWndComponentsOrganizer::AddSeparatorLine(int height)
{
	if (height <= 0) {
		
		return ERROR_INVALID_PARAMETER;
	}
	
	m_lastLine = CreateAndInitializeLineInfo();
	
	if (m_lastLine == NULL) {
		
		return ERROR_NOT_ENOUGH_MEMORY;
	}
	
	m_lastLine->lineType = LINE_TYPE_SEPARATOR;
	m_lastLine->lineHeight = height;
	
	m_lastLinePos = m_linesList.AddTail(m_lastLine);
	
	return ERROR_SUCCESS;
}


/**
 * AddComponent()
 * 
 * Adiciona um componente aa ultima linha adicionada ao organizador.
 * Componentes NAO podem ser adicionados a separadores.
 * 
 * Caso os parametros "maximmumHeight" e "minimmumHeight" sejam iguais a 
 * "0", prevalecerah a altura padrao para o componente, de acordo com a 
 * sua classe informada pelo parametro "wndClassName".
 * 
 * Caso o componente adicionado seja um botao, e ele possua um bitmap, 
 * as dimensoes do botao serao dadas pelas dimensoes do bitmap.
 * 
 * @param	component			ATL::CWindow of the component being added.
 * 
 * @param	wndClassName		class name of the component being added and 
 * 								retrived by a call to ATL::Window member 
 *								method GetWndClassName().
 * 
 * @param	width				width of the component.
 * 
 * @param	maximmumHeight		"0" or the height to be confronted with the 
 *								default height of the component. The lower 
 *								value will be used. It's used to limit the 
 *								height that is internally calculated.
 * 
 * @param	minimmumHeight		"0" or the height to be confronted with the 
 *								default height of the component. The higher 
 *								value will be used. It's used to define a 
 *								minimum height to the component.
 * 
 * @param	fillUp				whether the element will fillUp the remaining 
 *								space, i.e., the space not occupied by other 
 *								components that belong to the same line.
 *								Just one per line is accepted.
 * 
 * @return ERROR_INVALID_PARAMETER Se o componente nao eh uma window; se o 
 *			parametro "width" eh menor ou igual a zero; se o parametro 
 *			"fillUp" eh "TRUE", e a linha foi configurada com orientacao 
 *			centralizada; ou, se o parametro "fillUp" eh "TRUE", e a linha 
 *			jah possui um elemento fillUp.
 * 
 */
INT CWTLWndComponentsOrganizer::AddComponent(CWindow *component, 
											 LPCSTR wndClassName, int width, 
											 int maximmumHeight, int minimmumHeight, 
											 BOOL fillUp, 
											 COrganizerWindow* pOrganizerWindow)
{
	CBitmapHandle bitmapHandle;
	SIZE bitmapSize;
	
	if ( (!component->IsWindow()) || 
			(m_lastLine->lineType == LINE_TYPE_SEPARATOR) || 
			( (fillUp) && (m_lastLine->lineOrientation == LINE_ORIENTATION_CENTERED) ) || 
			( (maximmumHeight != 0) && (minimmumHeight > maximmumHeight) ) || 
			( (fillUp) && (m_lastLine->hasFillUp) ) )
	{
		return ERROR_INVALID_PARAMETER;
	}
	
	ComponentInfo* newComponentInfo = CreateAndInitializeComponentInfo();
	
	if (newComponentInfo == NULL)
	{
		return ERROR_NOT_ENOUGH_MEMORY;
	}
	
	newComponentInfo->component = component;
	newComponentInfo->hWndComponent = component->m_hWnd;
	newComponentInfo->fillUp = fillUp;
	lstrcpyn(newComponentInfo->wndClassName, wndClassName, WND_CLASS_NAME_SIZE - 1);
	
	if (lstrcmpi(wndClassName, _T("BUTTON")) == 0)
	{
		bitmapHandle = ((WTL::CButton*)component)->GetBitmap();
	}
	else if (IsStaticCtrlType(wndClassName))
	{
		// Lets check first whether it is a bitmap container.
		bitmapHandle = ((WTL::CStatic*)component)->GetBitmap();
		
		// If not. I'll check wether it has some text, and try 
		// to calculate the size it demands to paint that text.
		// If it don't have any text information, I'll consider 
		// it's a container for other components.
		if (bitmapHandle == NULL)
		{
			CString staticText;
			int textLength = component->GetWindowText(staticText);
			
			if (textLength > 0)
			{
				CPaintDC paintDC(component->m_hWnd);
				paintDC.SelectFont(component->GetFont());
				CSize textSize;
				
				::GetTextExtentPoint32(paintDC.m_ps.hdc, staticText, textLength, &textSize);
				
				if (width == 0)
				{
					width = textSize.cx;
				}
				else 
				{
					width = __max(width, textSize.cx);
				}
				
				minimmumHeight = textSize.cy;
			}
		}
	}
	
	if (bitmapHandle != NULL)
	{
		bitmapHandle.GetSize(bitmapSize);
		newComponentInfo->width = bitmapSize.cx;
		newComponentInfo->height = bitmapSize.cy;
	}
	else
	{
		newComponentInfo->width = width;
		newComponentInfo->height = CalculateComponentHeight(wndClassName, maximmumHeight, 
				minimmumHeight);
	}
	
	m_lastLine->hasFillUp = m_lastLine->hasFillUp || fillUp;
	m_lastLine->lineHeight = __max(m_lastLine->lineHeight, newComponentInfo->height);
	m_lastLine->totalWidth += newComponentInfo->width;
	
	m_lastComponentPos = m_lastLine->componentsList.AddTail(newComponentInfo);
	
	if (m_lastLine->componentsList.GetCount() > 1)
	{
		m_lastLine->totalWidth += m_lastLine->componentSpace;
	}
	
	if (pOrganizerWindow)
	{
		newComponentInfo->isOrganizerWindow = TRUE;
		newComponentInfo->pOrganizerWindow = pOrganizerWindow;
	}
	
	return ERROR_SUCCESS;
}


/**
 * OrganizeComponents()
 * 
 * Na primeira execucao, este metodo monta o layout da interface, 
 * considerando a organizacao informada pelo usuario.
 * Jah na segunda iteracao, ele reorganiza os componentes dinamicos 
 * da tela.
 * 
 * Os componentes dinamicos sao: 
 * 
 *   - Componentes que foram adicionados com a opcao "fillUp"; e, 
 *   - Linhas que devem ser centralizadas.
 * 
 * Esse metodo deve ser invocado depois que o layout da janela tiver 
 * sido totalmente definido. Ou seja, depois de todos as linhas, 
 * separadores e componentes tiverem sido adicionados.
 * 
 * Esse metodo deve ser invocado a partir do metodo OnSize da janela 
 * visando reposicionar os componentes dinamicos.
 *
 */
void CWTLWndComponentsOrganizer::OrganizeComponents(BOOL forceLineTopsRecalculation)
{
	CRect clientRect;
	BOOL linesRecalculated = FALSE;
	
	::GetClientRect(m_containerWindow->m_hWnd, &clientRect);
	
	if ( (clientRect.Width() == 0) || (clientRect.Height() == 0) )
	{
		return;
	}
	
	// Se necessario, recalcula a posicao das linhas.
	if ( (!m_hasOrganizedOnce) || 
			(forceLineTopsRecalculation) || 
			(m_hasFillUpLine) || 
			(m_changedVisibility) || 
			(m_verticalAlignment == WINDOW_ALIGNMENT_BOTTOM) || 
			(m_verticalAlignment == WINDOW_ALIGNMENT_CENTER) )
	{
		CalculateLinesTop();
		
		linesRecalculated = TRUE;
		m_changedVisibility = FALSE;
		
		if (forceLineTopsRecalculation)
		{
			PropagateChangeToParentOrganizer();
		}
	}
	
	LineInfo* pLineInfo = NULL;
	
	for (POSITION pos = m_linesList.GetHeadPosition(); pos; )
	{
		pLineInfo = m_linesList.GetNext(pos);
		
		if ( (pLineInfo->lineType == LINE_TYPE_COMPONENT) && (pLineInfo->isLineVisible) )
		{
			CalculateComponentsPosition(pLineInfo, linesRecalculated);
		}
	}
	
	m_hasOrganizedOnce = TRUE;
}


/**
 * GetMinimumWindowWidth()
 * 
 * Retorna a largura minima que a interface deve ter para que os 
 * componentes possam ser apresentados sem a necessidade de uma 
 * barra de rolagem.
 * 
 */
int	CWTLWndComponentsOrganizer::GetMinimumWindowWidth()
{
	int maxLineWidth = 0;
	
	LineInfo* pLineInfo = NULL;
	
	for (POSITION pos = m_linesList.GetHeadPosition(); pos; )
	{
		pLineInfo = m_linesList.GetNext(pos);
		
		if (pLineInfo->isLineVisible)
		{
			maxLineWidth = __max(maxLineWidth, pLineInfo->totalWidth);
		}
	}
	
	maxLineWidth += m_leftShift + m_rightShift;
	
	return maxLineWidth;
}


/**
 * GetMinimumWindowHeight()
 * 
 * Retorna a altura minima que a interface deve ter para que os 
 * componentes possam ser apresentados sem a necessidade de uma 
 * barra de rolagem.
 * 
 */
int CWTLWndComponentsOrganizer::GetMinimumWindowHeight(BOOL bConsiderHideElements)
{
	return CalculateLinesTotalHeight(bConsiderHideElements) + m_topShift + m_bottomShift;
}


int CWTLWndComponentsOrganizer::GetLastLineHeight()
{
	if (m_lastLine == NULL) {
		
		return 0;
	}
	else {
		
		return m_lastLine->lineHeight;
	}
}


POSITION CWTLWndComponentsOrganizer::GetLastAddedLinePosition()
{
	return m_lastLinePos;
}


POSITION CWTLWndComponentsOrganizer::GetLastAddedComponentPosition()
{
	return m_lastComponentPos;
}


INT CWTLWndComponentsOrganizer::ChangeComponent(
		CWindow *newComponent, POSITION linePos, 
		POSITION componentPos, LPCSTR wndClassName, int width, 
		int maximmumHeight, int minimmumHeight, BOOL fillUp)
{
	CBitmapHandle bitmapHandle;
	SIZE bitmapSize;
	
	if ( (!newComponent->IsWindow()) || (width <= 0) || 
			( (maximmumHeight != 0) && (minimmumHeight > maximmumHeight) ) )
	{
		return ERROR_INVALID_PARAMETER;
	}
	
	LineInfo* changingLine = m_linesList.GetAt(linePos);
	ComponentInfo* changingComponent = changingLine->componentsList.GetAt(componentPos);
	
	if ( (changingLine == NULL) || (changingComponent == NULL) || 
			(changingLine->lineType == LINE_TYPE_SEPARATOR) || 
			( (fillUp) && (changingLine->lineOrientation == LINE_ORIENTATION_CENTERED) ) || 
			( (fillUp) && (changingLine->hasFillUp) && (!changingComponent->fillUp) ) ) 
	{
		return ERROR_INVALID_PARAMETER;
	}
	
	ComponentInfo* newComponentInfo = CreateAndInitializeComponentInfo();
	
	if (newComponentInfo == NULL) 
	{
		return ERROR_NOT_ENOUGH_MEMORY;
	}
	
	newComponentInfo->component = newComponent;
	newComponentInfo->hWndComponent = newComponent->m_hWnd;
	newComponentInfo->fillUp = fillUp;
	
	if (lstrcmpi(wndClassName, _T("BUTTON")) == 0) 
	{
		bitmapHandle = ((WTL::CButton*)newComponent)->GetBitmap();
	}
	else if (IsStaticCtrlType(wndClassName))
	{
		bitmapHandle = ((WTL::CStatic*)newComponent)->GetBitmap();
	}
	
	if (bitmapHandle != NULL) 
	{
		bitmapHandle.GetSize(bitmapSize);
		newComponentInfo->width = bitmapSize.cx;
		newComponentInfo->height = bitmapSize.cy;
	}
	else 
	{
		newComponentInfo->width = width;
		newComponentInfo->height = CalculateComponentHeight(wndClassName, maximmumHeight, 
				minimmumHeight);
	}
	
	changingLine->hasFillUp = changingLine->hasFillUp || fillUp;
	changingLine->totalWidth += (newComponentInfo->width - changingComponent->width);
	changingLine->componentsList.SetAt(componentPos, newComponentInfo);
	
	// All done with the old component info, let's dealock it.
	delete changingComponent;
	
	RecalculateLineHeight(changingLine);
	
	RecalculateAll();
	
	return ERROR_SUCCESS;
}


void CWTLWndComponentsOrganizer::GetComponentClientPosition(
		CWindow component, CRect& clientPosition)
{
	LineInfo*		pLineInfo = NULL;
	ComponentInfo*	pComponentInfo = NULL;
	
	for (POSITION linePos = m_linesList.GetHeadPosition(); linePos; )
	{
		pLineInfo = m_linesList.GetNext(linePos);
		
		for (POSITION componentPos = pLineInfo->componentsList.GetHeadPosition(); componentPos; )
		{
			pComponentInfo = pLineInfo->componentsList.GetNext(componentPos);
			
			if (pComponentInfo->component->m_hWnd == component.m_hWnd)
			{
				clientPosition.SetRect(pComponentInfo->clientRect.TopLeft(), 
					pComponentInfo->clientRect.BottomRight());
				
				return;
			}
		}
	}
}


BOOL CWTLWndComponentsOrganizer::RecalculateLinesHeightsAndWidths()
{
	LineInfo*		pLineInfo = NULL;
	ComponentInfo*	pComponentInfo = NULL;
	CBitmapHandle	bitmapHandle;
	BOOL			lineRecalculated = FALSE;
	int				lineHeight = 0;
	int				numAutoResizeComponents = 0;
	
	// Firstly we recalculate all lines.
	//
	for (POSITION linePos = m_linesList.GetHeadPosition(); linePos; )
	{
		pLineInfo = m_linesList.GetNext(linePos);
		lineHeight = 0;
		
		for (POSITION componentPos = pLineInfo->componentsList.GetHeadPosition(); componentPos; )
		{
			pComponentInfo = pLineInfo->componentsList.GetNext(componentPos);
			
			if (IsStaticCtrlType(pComponentInfo->wndClassName))
			{
				// Lets check first whether it is a bitmap container.
				bitmapHandle = ((WTL::CStatic*)pComponentInfo->component)->GetBitmap();
				
				// If not. I'll check wether it has some text, and try 
				// to calculate the size it demands to paint that text.
				// If it don't have any text information, I'll consider 
				// it's a container for other components.
				if (bitmapHandle == NULL)
				{
					CString staticText;
					int textLength = pComponentInfo->component->GetWindowText(staticText);
					
					if (textLength > 0)
					{
						CPaintDC paintDC(pComponentInfo->component->m_hWnd);
						paintDC.SelectFont(pComponentInfo->component->GetFont());
						CSize textSize;
						
						::GetTextExtentPoint32(paintDC.m_ps.hdc, staticText, textLength, &textSize);
						
						pLineInfo->totalWidth += (textSize.cx - pComponentInfo->width);
						
						pComponentInfo->width = textSize.cx;
						pComponentInfo->height = textSize.cy;
					}
				}
			}
			else if (lstrcmpi(pComponentInfo->wndClassName, _T("CAutoResizeStatic")) == 0)
			{
				numAutoResizeComponents++;
				
				pComponentInfo->height = 
						((CAutoResizeStatic*)pComponentInfo->component)->GetHeight();
			}
			
			lineHeight = __max(lineHeight, pComponentInfo->height);
		}
		
		if ( (pLineInfo->lineHeight != lineHeight) && 
				(pLineInfo->lineType == LINE_TYPE_COMPONENT) )
		{
			lineRecalculated = TRUE;
			
			pLineInfo->lineHeight = lineHeight;
		}
	}
	
	// Right after, we check out whether exists CAutoResizeStatic 
	// components, and whether the total lines size exceeds the 
	// client area.
	CRect clientRect;
	int totalNeededHeight = 0;
	int subtractingSlice = 0;
	
	if (numAutoResizeComponents > 0)
	{
		::GetClientRect(m_containerWindow->m_hWnd, &clientRect);
		
		totalNeededHeight = CalculateLinesTotalHeight() + m_topShift + m_bottomShift;
		
		if (totalNeededHeight > clientRect.Height())
		{
			// Problemas...
			//
			// Os componentes ocuparam mais espaco do que deveriam, e o unico 
			// que pode causar isso eh o nosso amigo CAutoResizeStatic.
			// Temos de especificar a sua altura maxima!!!
			// 
			subtractingSlice = (totalNeededHeight - clientRect.Height()) / numAutoResizeComponents;
		}
		
		for (POSITION linePos = m_linesList.GetHeadPosition(); linePos; )
		{
			pLineInfo = m_linesList.GetNext(linePos);
			lineHeight = 0;
			
			for (POSITION componentPos = pLineInfo->componentsList.GetHeadPosition(); componentPos; )
			{
				pComponentInfo = pLineInfo->componentsList.GetNext(componentPos);
				
				if (lstrcmpi(pComponentInfo->wndClassName, _T("CAutoResizeStatic")) == 0)
				{
					pComponentInfo->height -= subtractingSlice;
					
					((CAutoResizeStatic*)pComponentInfo->component)->SetMaxHeight(
							pComponentInfo->height);
					
					// Apos um "SetMaxHeight", o componente recalcula a sua altura 
					// em termos de multiplos da altura de uma linha.
					// Temos de pegar o valor atualizado de novo.
					pComponentInfo->height = 
							((CAutoResizeStatic*)pComponentInfo->component)->GetHeight();
				}
				
				lineHeight = __max(lineHeight, pComponentInfo->height);
			}
			
			if ( (pLineInfo->lineHeight != lineHeight) && 
					(pLineInfo->lineType == LINE_TYPE_COMPONENT) )
			{
				lineRecalculated = TRUE;
				
				pLineInfo->lineHeight = lineHeight;
			}
		}
	}
	
	return lineRecalculated;
}


void CWTLWndComponentsOrganizer::ShowAllComponents(BOOL visible)
{
	int cmdShow = (visible) ? SW_SHOW : SW_HIDE;
	
	LineInfo*		pLineInfo = NULL;
	ComponentInfo*	pComponentInfo = NULL;
	
	for (POSITION linePos = m_linesList.GetHeadPosition(); linePos; )
	{
		pLineInfo = m_linesList.GetNext(linePos);
		
		pLineInfo->isLineVisible = visible;
		
		for (POSITION componentPos = pLineInfo->componentsList.GetHeadPosition(); componentPos; )
		{
			pComponentInfo = pLineInfo->componentsList.GetNext(componentPos);
			
			pComponentInfo->component->ShowWindow(cmdShow);
		}
	}
	
	m_changedVisibility = TRUE;
}


void CWTLWndComponentsOrganizer::ShowLineComponents(BOOL visible, POSITION linePos)
{
	if (linePos)
	{
		int cmdShow = (visible) ? SW_SHOW : SW_HIDE;
		
		LineInfo*		pLineInfo = m_linesList.GetAt(linePos);
		ComponentInfo*	pComponentInfo = NULL;
		
		if (pLineInfo != NULL)
		{
			pLineInfo->isLineVisible = visible;
			
			for (POSITION componentPos = pLineInfo->componentsList.GetHeadPosition(); componentPos; )
			{
				pComponentInfo = pLineInfo->componentsList.GetNext(componentPos);
				
				pComponentInfo->component->ShowWindow(cmdShow);
			}
		}
		
		m_changedVisibility = TRUE;
	}
}


INT CWTLWndComponentsOrganizer::UpdateComponent(POSITION linePos, POSITION componentPos, 
		int width, int height, BOOL fillUp)
{
	LineInfo* changingLine = m_linesList.GetAt(linePos);
	ComponentInfo* changingComponent = changingLine->componentsList.GetAt(componentPos);
	BOOL bComponentDimentionsChanged = FALSE;
	
	if ( (width <= 0) || (height <= 0) || 
			(changingLine == NULL) || (changingComponent == NULL) || 
			(changingLine->lineType == LINE_TYPE_SEPARATOR) || 
			( (fillUp) && (changingLine->lineOrientation == LINE_ORIENTATION_CENTERED) ) || 
			( (fillUp) && (changingLine->hasFillUp) && (!changingComponent->fillUp) ) ) 
	{
		return ERROR_INVALID_PARAMETER;
	}
	
	changingLine->hasFillUp = changingLine->hasFillUp || fillUp;
	
	if (changingLine->lineHeight != height)
	{
		changingComponent->height = height;
		
		RecalculateLineHeight(changingLine);
		
		bComponentDimentionsChanged = TRUE;
	}
	
	if (changingComponent->width != width)
	{
		changingLine->totalWidth += (width - changingComponent->width);
		
		bComponentDimentionsChanged = TRUE;
	}
	
	changingComponent->fillUp = fillUp;
	changingComponent->width = width;
	
	if (bComponentDimentionsChanged)
	{
		RecalculateAll();
		
		PropagateChangeToParentOrganizer();
	}
	
	return ERROR_SUCCESS;
}


BOOL CWTLWndComponentsOrganizer::VerifyAndRecalculateInnerOrganizers()
{
	LineInfo* pLineInfo = NULL;
	ComponentInfo* pComponentInfo = NULL;
	BOOL bParentNeedToRecalculate = FALSE;
	
	for (POSITION pos = m_linesList.GetHeadPosition(); pos; )
	{
		pLineInfo = m_linesList.GetNext(pos);
		
		for (POSITION pos = pLineInfo->componentsList.GetHeadPosition(); pos; )
		{
			pComponentInfo = pLineInfo->componentsList.GetNext(pos);
			
			if ( (pComponentInfo->isOrganizerWindow) && 
					(pComponentInfo->pOrganizerWindow != NULL) )
			{
				CSize componentSize;
				pComponentInfo->pOrganizerWindow->GetMinSize(componentSize);
				
				if (pLineInfo->lineHeight != componentSize.cy)
				{
					pComponentInfo->height = componentSize.cy;
					
					RecalculateLineHeight(pLineInfo);
					
					bParentNeedToRecalculate = TRUE;
				}
				
				if (pComponentInfo->width != componentSize.cx)
				{
					pLineInfo->totalWidth += 
							(componentSize.cx - pComponentInfo->width);
					
					bParentNeedToRecalculate = TRUE;
				}
				
				pComponentInfo->width = componentSize.cx;
			}
		}
	}
	
	return bParentNeedToRecalculate;
}


CWindow* CWTLWndComponentsOrganizer::GetContainerWindow()
{
	return m_containerWindow;
}


void CWTLWndComponentsOrganizer::PropagateChangeToParentOrganizer()
{
	HWND hParentWnd = GetParent(m_containerWindow->m_hWnd);
	
	if (hParentWnd)
	{
		::SendMessage(hParentWnd, WM_INNER_ORGANIZER_CHANGE, 0, 0);
	}
}


INT CWTLWndComponentsOrganizer::GetNumberOfLines()
{
	return m_linesList.GetCount();
}


BOOL CWTLWndComponentsOrganizer::GetIsTopMostOrganizer()
{
	return m_bIsTopMostOrganizer;
}



/////////////////////////////////////////////////////////////////////
// Protected members
/////////////////////////////////////////////////////////////////////


CWTLWndComponentsOrganizer::LineInfo* CWTLWndComponentsOrganizer::CreateAndInitializeLineInfo()
{
	LineInfo* newLineInfo = new LineInfo();
	
	if (newLineInfo != NULL) {
		
		newLineInfo->hasFillUp = FALSE;
		newLineInfo->componentSpace = 0;
		newLineInfo->lineHeight = 0;
		newLineInfo->totalWidth = 0;
		newLineInfo->lineTop = 0;
		newLineInfo->lastLineTop = 0;
		newLineInfo->fillUpLine = FALSE;
		newLineInfo->fillUpSize = 0;
		newLineInfo->fillUpFactor = 1;
		newLineInfo->isLineVisible = TRUE;
		newLineInfo->lineOrientation = LINE_ORIENTATION_LEFT;
		newLineInfo->lineVOrientation = LINE_VORIENTATION_BOTTOM;
	}
	
	return newLineInfo;
}


CWTLWndComponentsOrganizer::ComponentInfo* CWTLWndComponentsOrganizer::CreateAndInitializeComponentInfo()
{
	ComponentInfo* newComponentInfo = new ComponentInfo();
	
	if (newComponentInfo != NULL)
	{
		newComponentInfo->component = NULL;
		newComponentInfo->fillUp = FALSE;
		newComponentInfo->width = 0;
		newComponentInfo->hWndComponent = 0;
		newComponentInfo->component = NULL;
		newComponentInfo->clientRect.SetRect(0, 0, 0, 0);
		memset(newComponentInfo->wndClassName, 0, WND_CLASS_NAME_SIZE);
		newComponentInfo->isOrganizerWindow = FALSE;
		newComponentInfo->pOrganizerWindow = NULL;
	}
	
	return newComponentInfo;
}


int	CWTLWndComponentsOrganizer::CalculateComponentHeight(LPCSTR wndClassName, 
														 int maximmumHeight, 
														 int minimmumHeight)
{
	int expectedHeight = GetComponentExpectedHeight(wndClassName);
	
	if (maximmumHeight != 0) {
		
		return __min(maximmumHeight, expectedHeight);
	}
	else if (minimmumHeight != 0) {
		
		return __max(minimmumHeight, expectedHeight);
	}
	else if (expectedHeight == 0) {
		
		return 25;
	}
	else {
		
		return expectedHeight;
	}
}


int CWTLWndComponentsOrganizer::GetComponentExpectedHeight(LPCSTR wndClassName)
{
	int expectedHeight = 0;
	
	if (IsStaticCtrlType(wndClassName))
	{
		expectedHeight = 17;
	}
	else if (lstrcmpi(wndClassName, _T("BUTTON")) == 0)
	{
		expectedHeight = 25;
	}
	else if (lstrcmpi(wndClassName, _T("COMBOBOX")) == 0)
	{
		expectedHeight = 25;
	}
	else if (lstrcmpi(wndClassName, _T("EDIT")) == 0)
	{
		expectedHeight = 25;
	}
	else if (lstrcmpi(wndClassName, TRACKBAR_CLASS) == 0)
	{
		expectedHeight = 35;
	}
	else if (lstrcmpi(wndClassName, "CUOLFoneSlider") == 0)
	{
		expectedHeight = 35;
	}
	else if (lstrcmpi(wndClassName, "CUOLFonePromoAdPanel") == 0)
	{
		expectedHeight = 45;
	}
	
	return expectedHeight;
}


void CWTLWndComponentsOrganizer::CalculateLinesTop()
{
	CRect rectClient;
	::GetClientRect(m_containerWindow->m_hWnd, &rectClient);
	
	int linesTotalHeight = CalculateLinesTotalHeight();
	int totalFillUpSize = 0;
	float fillUpSlice = 0;
	
	if (m_hasFillUpLine) {
		
		totalFillUpSize = __max(0, (rectClient.Height() - linesTotalHeight - m_topShift - m_bottomShift));
		
		fillUpSlice = ( static_cast<float>(totalFillUpSize) / 
				static_cast<float>(CalculateLinesFillUpTotalFactor()) );
	}
	
	int yCoord;
	
	if (m_verticalAlignment == WINDOW_ALIGNMENT_TOP)
	{
		yCoord = m_topShift;
	}
	else if (m_verticalAlignment == WINDOW_ALIGNMENT_BOTTOM)
	{
		yCoord = rectClient.bottom - m_bottomShift;
	}
	else if (m_verticalAlignment == WINDOW_ALIGNMENT_CENTER)
	{
		int availableHeight = rectClient.Height() - m_topShift - m_bottomShift;
		
		// Caso a altura total das linhas supere a altura da area cliente, 
		// trabalharemos como se fosse orientacao TOP.
		yCoord = m_topShift + 
				__max(0, ( (availableHeight - linesTotalHeight) / 2));
	}
	
	LineInfo* pLineInfo = NULL;
	
	for (POSITION pos = m_linesList.GetHeadPosition(); pos; )
	{
		pLineInfo = m_linesList.GetNext(pos);
		
		if (pLineInfo->isLineVisible)
		{
			if (pLineInfo->fillUpLine)
			{
				pLineInfo->fillUpSize = static_cast<int>(fillUpSlice * pLineInfo->fillUpFactor);
			}
			
			// Salvando a posicao anterior.
			pLineInfo->lastLineTop = pLineInfo->lineTop;
			
			if (m_verticalAlignment == WINDOW_ALIGNMENT_BOTTOM)
			{
				// Como a orientacao neste caso eh invertida, eu tenho 
				// que subtrair a altura da linha atual antes de 
				// calcular a posicao do seu topo.
				if (pLineInfo->fillUpLine)
				{
					yCoord -= pLineInfo->fillUpSize;
				}
				
				yCoord -= pLineInfo->lineHeight;
				pLineInfo->lineTop = yCoord;
			}
			else
			{
				// Para os demais casos, o yCoord jah aponta para o topo 
				// da linha. Portanto, a atualizacao do seu valor para o 
				// recalculo da proxima linha deve ocorrer depois da 
				// atribuicao.
				pLineInfo->lineTop = yCoord;
				yCoord += pLineInfo->lineHeight;
				
				if (pLineInfo->fillUpLine)
				{
					yCoord += pLineInfo->fillUpSize;
				}
			}
		}
	}
}


int CWTLWndComponentsOrganizer::CalculateLinesTotalHeight(BOOL bConsiderHideElements)
{
	int linesTotalHeight = 0;
	
	LineInfo* pLineInfo = NULL;
	
	for (POSITION pos = m_linesList.GetHeadPosition(); pos; )
	{
		pLineInfo = m_linesList.GetNext(pos);
		
		if ( (pLineInfo->isLineVisible) || (bConsiderHideElements) )
		{
			linesTotalHeight += pLineInfo->lineHeight;
		}
	}
	
	return linesTotalHeight;
}


void CWTLWndComponentsOrganizer::CalculateComponentsPosition(LineInfo *pLineInfo, BOOL linesRecalculated)
{
	CRect rectClient;
	BOOL  lineRecalculated = FALSE;
	::GetClientRect(m_containerWindow->m_hWnd, &rectClient);
	
	ComponentInfo* pComponentInfo = NULL;
	int xCoord = 0;
	int yCoord = 0;
	int width = 0;
	int height = 0;
	int fillUpSize = 0;
	
	if (pLineInfo->hasFillUp) {
		
		fillUpSize = __max(0, (rectClient.Width() - pLineInfo->totalWidth - m_leftShift - m_rightShift));
	}
	
	if (pLineInfo->lineOrientation == LINE_ORIENTATION_LEFT) {
		
		xCoord = m_leftShift;
	}
	else if (pLineInfo->lineOrientation == LINE_ORIENTATION_RIGHT) {
		
		xCoord = rectClient.right - m_rightShift;
	}
	else if (pLineInfo->lineOrientation == LINE_ORIENTATION_CENTERED) {
		
		int availableWidth = rectClient.Width() - m_leftShift - m_rightShift;
		
		// Caso a largura total da linha supere a largura da area cliente, 
		// trabalharemos como se fosse orientacao LEFT.
		xCoord += m_leftShift + 
				__max(0, ( (availableWidth - pLineInfo->totalWidth) / 2));
	}
	
	for (POSITION pos = pLineInfo->componentsList.GetHeadPosition(); pos; )
	{
		pComponentInfo = pLineInfo->componentsList.GetNext(pos);
		
		// Calculate the width, considering the fillUp attribute.
		width = pComponentInfo->width;
		
		if (pComponentInfo->fillUp) {
			
			width += fillUpSize;
		}
		
		// Este componente precisa se recalcular, e caso apresente mudancas, 
		// realizaremos o recalculo dos topos de todas as linhas subsequentes.
		if (lstrcmpi(pComponentInfo->wndClassName, _T("CAutoResizeStatic")) == 0)
		{
			((CAutoResizeStatic*)pComponentInfo->component)->SetMaxWidth(width);
			
			lineRecalculated = RecalculateLinesHeightsAndWidths();
			
			if (lineRecalculated)
			{
				// Recalcula o topo das linhas, visto que o componente teve 
				// alteracoes.
				// Nao precisamos nos preocupar com os componentes que jah 
				// foram posicionados, pois somente os componentes em linhas 
				// posteriores sofreram o efeito do redimensionamento deste 
				// componente.
				CalculateLinesTop();
			}
		}
		
		// Consideraremos que uma linha fillUp possuirah somente o componente 
		// filler pra ser redimensionado.
		// Linhas normais destinanas aa apresentacao de componentes nunca 
		// serao criadas com essa opcao. Sendo tratadas assim, na clausula 
		// "else" abaixo.
		if (pLineInfo->fillUpLine) {
			
			// Todos os componentes que pertencem a uma linha que seja fillUp 
			// devem ter a sua altura redimensionada para a altura da linha, 
			// acrescida do fillUpSize.
			// Alem disso, o seu topo serah exatamente o topo da linha.
			yCoord = pLineInfo->lineTop;
			
			height = pLineInfo->lineHeight + pLineInfo->fillUpSize;
		}
		else {
			
			if (pLineInfo->lineVOrientation == LINE_VORIENTATION_BOTTOM)
			{
				// A coordenada y para o componente deve ser o topo da linha, acrescido 
				// da diferenca que possa haver entre a altura dele e a altura total da 
				// linha.
				// Isso garante que os componentes estejam alinhados pela sua base.
				yCoord = pLineInfo->lineTop + (pLineInfo->lineHeight - pComponentInfo->height);
			}
			else if (pLineInfo->lineVOrientation == LINE_VORIENTATION_TOP)
			{
				// Nada a fazer.
				// Basta atribuir o topo da linha aa variavel "yCoord".
				yCoord = pLineInfo->lineTop;
			}
			else if (pLineInfo->lineVOrientation == LINE_VORIENTATION_CENTERED)
			{
				// A coordenada y para o componente deve ser o topo da linha, acrescido 
				// da metade da diferenca que possa haver entre a altura dele e a altura 
				// total da linha.
				// Isso garante que os componentes estejam centralizados na vertical.
				yCoord = pLineInfo->lineTop + ( (pLineInfo->lineHeight - pComponentInfo->height) / 2);
			}
			
			height = pComponentInfo->height;
		}
		
		if (pLineInfo->lineOrientation == LINE_ORIENTATION_RIGHT) {
			
			xCoord -= width;
			
			pComponentInfo->clientRect.SetRect(
					xCoord, yCoord, xCoord + width, yCoord + height);
			
			pComponentInfo->component->SetWindowPos(NULL, 
					pComponentInfo->clientRect, 
					SWP_NOACTIVATE | SWP_NOZORDER);
			
			xCoord -= pLineInfo->componentSpace;
		}
		else {
			
			pComponentInfo->clientRect.SetRect(
					xCoord, yCoord, xCoord + width, yCoord + height);
			
			pComponentInfo->component->SetWindowPos(NULL, 
					pComponentInfo->clientRect, 
					SWP_NOACTIVATE | SWP_NOZORDER);
			
			xCoord += width + pLineInfo->componentSpace;
		}
	}
}


void CWTLWndComponentsOrganizer::RecalculateAll()
{
	m_hasOrganizedOnce = FALSE;
	
	OrganizeComponents();
}


void CWTLWndComponentsOrganizer::RecalculateLineHeight(LineInfo* calculatingLine)
{
	int lineHeight = 0;
	
	ComponentInfo* pComponentInfo = NULL;
	
	for (POSITION pos = calculatingLine->componentsList.GetHeadPosition(); pos; )
	{
		pComponentInfo = calculatingLine->componentsList.GetNext(pos);
		
		lineHeight = __max(lineHeight, pComponentInfo->height);
	}
	
	calculatingLine->lineHeight = lineHeight;
}


int CWTLWndComponentsOrganizer::CalculateLinesFillUpTotalFactor()
{
	int fillUpTotalFactor = 0;
	LineInfo* pLineInfo = NULL;
	
	for (POSITION pos = m_linesList.GetHeadPosition(); pos; )
	{
		pLineInfo = m_linesList.GetNext(pos);
		
		if ( (pLineInfo->isLineVisible) && (pLineInfo->fillUpLine) )
		{
			fillUpTotalFactor += pLineInfo->fillUpFactor;
		}
	}
	
	return fillUpTotalFactor;
}


BOOL CWTLWndComponentsOrganizer::IsStaticCtrlType(LPCSTR wndClassName)
{
	BOOL result = FALSE;
	
	if ( (lstrcmpi(wndClassName, _T("STATIC")) == 0) || 
			(lstrcmpi(wndClassName, "CUOLFoneStatic") == 0) || 
			(lstrcmpi(wndClassName, "CUOLFoneTooltipLabel") == 0) )
	{
		result = TRUE;
	}
	
	return result;
}


/*
LineInfo* CWTLWndComponentsOrganizer::GetLineByIndex(int lineIndex)
{
	int lineCount = 0;
	LineInfo* pLineInfo = NULL;
	
	if (lineIndex >= m_linesList.GetCount())
	{
		for (POSITION pos = m_linesList.GetHeadPosition(); pos; lineCount++)
		{
			pLineInfo = m_linesList.GetNext(pos);
			
			if (lineCount == lineIndex)
			{
				break;
			}
		}
	}
	
	return pLineInfo;
}


ComponentInfo* CWTLWndComponentsOrganizer::GetComponentByIndex(
		LineInfo* searchingLine, int componentIndex, POSITION &componentPos)
{
	int componentCount = 0;
	ComponentInfo* pComponentInfo = NULL;
	
	if (componentIndex >= searchingLine->componentsList.GetCount())
	{
		for (componentPos = searchingLine->componentsList.GetHeadPosition(); componentPos; 
				componentCount++)
		{
			pComponentInfo = searchingLine->componentsList.GetNext(componentPos);
			
			if (componentCount == componentIndex)
			{
				break;
			}
		}
	}
	
	return pComponentInfo;
}
*/

