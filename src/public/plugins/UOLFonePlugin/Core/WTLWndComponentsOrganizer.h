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


#pragma once


#define		WND_CLASS_NAME_SIZE					30
#define		AUTO_RESIZE_STATIC_SET_MAX_WIDTH	RegisterWindowMessage("AUTO_RESIZE_STATIC_SET_MAX_WIDTH")
#define		AUTO_RESIZE_STATIC_GET_SIZE			RegisterWindowMessage("AUTO_RESIZE_STATIC_GET_SIZE")


class COrganizerWindow;


/**
 * CWTLWndComponentsOrganizer
 * 
 * Essa eh uma classe facilitadora para a organizacao de componentes 
 * em uma janela WTL.
 * Ela tambem possibilita o redimensionamento e reposicionamento 
 * dinamico destes componentes.
 * 
 * Para utiliza-la, basta adicionar os elementos (linhas, separadores, 
 * e componentes) de forma a se obter o layout desejado para a tela e 
 * obedecendo aas limitacoes desta implementacao.
 * Esse processo geralmente eh feito durante a criacao da tela associada 
 * (metodo OnCreate()).
 * 
 * O metodo "SetVerticalAlignment()" permite estabelecer o layout de 
 * exibicao dos componentes dentro da janela.
 * Informando "WINDOW_ALIGNMENT_TOP", a tela apresentarah uma orientacao 
 * tradicional, com os elementos organizados de forma top-down.
 * Informando "WINDOW_ALIGNMENT_BOTTOM", a tela apresentarah uma orientacao 
 * invertida, com os elementos organizados de forma bottom-up.
 * Informando "WINDOW_ALIGNMENT_CENTER", a tela apresentarah uma orientacao 
 * centralizada, com os elementos organizados verticalmente no centro da 
 * tela.
 * 
 * Importante: as linhas e separadores devem ser adicionados na ordem 
 * inversa quando se desejar uma orientacao bottom-up no layout dos 
 * componentes para a janela (WINDOW_ALIGNMENT_BOTTOM).
 * 
 * Para que os componentes sejam visualizados e reposicionados, o metodo 
 * "OrganizeComponents()" deve ser invocado durante os eventos WM_SIZE 
 * direcionados para a janela.
 * 
 * Caso se deseje saber a largura minima necessaria para a apresentacao 
 * dos componentes sem a necessidade de scrollbars, basta invodar o 
 * metodo "GetMinimumWindowWidth()".
 * 
 */
class CWTLWndComponentsOrganizer
{
public:
	
	enum LineType
	{
		LINE_TYPE_COMPONENT = 1, 
		LINE_TYPE_SEPARATOR
	};
	
	enum LineHorizontalAlign
	{
		LINE_ORIENTATION_LEFT = 1, 
		LINE_ORIENTATION_RIGHT,
		LINE_ORIENTATION_CENTERED
	};
	
	enum LineVerticalAlign
	{
		LINE_VORIENTATION_BOTTOM = 1, 
		LINE_VORIENTATION_TOP,
		LINE_VORIENTATION_CENTERED
	};
	
	enum WindowVerticalAlign
	{
		WINDOW_ALIGNMENT_TOP = 1,
		WINDOW_ALIGNMENT_BOTTOM,
		WINDOW_ALIGNMENT_CENTER
	};
	
	enum OrganizerMessage
	{
		WM_INNER_ORGANIZER_CHANGE = WM_USER + 152
	};
	
private:
	
	typedef struct ComponentInfo
	{
		ATL::CWindow		*component;
		HWND				hWndComponent;
		int					width;
		int					fillUp;
		int					height;
		CRect				clientRect;
		char				wndClassName[WND_CLASS_NAME_SIZE];
		BOOL				isOrganizerWindow;
		COrganizerWindow*	pOrganizerWindow;
	};
	
	typedef struct LineInfo
	{
		int			lineType;
		int			lineHeight;
		int			totalWidth;			// total of components widths. do 
										// not consider fillUp of a component
		int			componentSpace;
		int			lineOrientation;
		int			lineVOrientation;
		BOOL		hasFillUp;			// only acceps one fillUp component per line
		int			lineTop;
		int			lastLineTop;
		BOOL		fillUpLine;
		int			fillUpSize;
		int			fillUpFactor;
		BOOL		isLineVisible;
		CAtlList<ComponentInfo*> componentsList;
	};
	
public:
	
	CWTLWndComponentsOrganizer();
	~CWTLWndComponentsOrganizer(void);
	
	void Initialize(CWindow* containerWindow, int leftShift = 0, int topShift = 0, 
			int rightShift = 0, int bottomShift = 0, 
			int verticalAlignment = WINDOW_ALIGNMENT_TOP, 
			BOOL bIsTopMostOrganizer = FALSE);
	
	void SetBoundaries(int leftShift = 0, int topShift = 0, 
			int rightShift = 0, int bottomShift = 0);
	
	void SetLeftShift(int leftShift);
	void SetTopShift(int topShift);
	void SetRightShift(int rightShift);
	void SetBottomShift(int bottomShift);
	void SetVerticalAlignment(int verticalAlignment);
	
	INT  AddLine(int componentSpace = 0, int orientation = LINE_ORIENTATION_LEFT, 
			int verticalOrientation = LINE_VORIENTATION_BOTTOM, 
			BOOL fillUp = FALSE, int fillUpFactor = 1);
	INT  AddSeparatorLine(int height);
	INT  AddComponent(CWindow *component, LPCSTR wndClassName, int width = 0, 
			int maximmumHeight = 0, int minimmumHeight = 0, BOOL fillUp = FALSE, 
			COrganizerWindow* pOrganizerWindow = NULL);
	
	void OrganizeComponents(BOOL forceLineTopsRecalculation = FALSE);
	
	int	GetMinimumWindowWidth();
	int	GetMinimumWindowHeight(BOOL bConsiderHideElements = TRUE);
	
	int GetLastLineHeight();
	
	POSITION GetLastAddedLinePosition();
	
	POSITION GetLastAddedComponentPosition();
	
	INT ChangeComponent(CWindow *newComponent, POSITION linePos, 
			POSITION componentPos, LPCSTR wndClassName, int width, 
			int maximmumHeight = 0, int minimmumHeight = 0, BOOL fillUp = FALSE);
	
	void GetComponentClientPosition(CWindow component, CRect& clientPosition);
	
	BOOL RecalculateLinesHeightsAndWidths();
	
	void ShowAllComponents(BOOL visible);
	void ShowLineComponents(BOOL visible, POSITION linePos);
	
	INT UpdateComponent(POSITION linePos, POSITION componentPos, 
			int width, int height, BOOL fillUp = FALSE);
	
	BOOL VerifyAndRecalculateInnerOrganizers();
	
	CWindow* GetContainerWindow();
	
	void PropagateChangeToParentOrganizer();
	
	INT GetNumberOfLines();
	
	BOOL GetIsTopMostOrganizer();
	
private:
	
	LineInfo*		CreateAndInitializeLineInfo();
	ComponentInfo*	CreateAndInitializeComponentInfo();
	int				CalculateComponentHeight(LPCSTR wndClassName, int maximmumHeight, 
											 int minimmumHeight);
	int				GetComponentExpectedHeight(LPCSTR wndClassName);
	void			CalculateLinesTop();
	int				CalculateLinesTotalHeight(BOOL bConsiderHideElements = FALSE);
	void			CalculateComponentsPosition(LineInfo *pLineInfo, BOOL linesRecalculated);
	
	void			RecalculateAll();
	
	void			RecalculateLineHeight(LineInfo* calculatingLine);
	
	int				CalculateLinesFillUpTotalFactor();
	
	BOOL			IsStaticCtrlType(LPCSTR wndClassName);
	
	/*
	LineInfo*		GetLineByIndex(int lineIndex);
	ComponentInfo*	GetComponentByIndex(LineInfo* searchingLine, int componentIndex, 
			POSITION &componentPos);
	*/
	
private:
	
	CAtlList<LineInfo*> m_linesList;
	
	CWindow*	m_containerWindow;
	
	int			m_leftShift;
	int			m_topShift;
	int			m_rightShift;
	int			m_bottomShift;
	int			m_verticalAlignment;
	
	LineInfo*	m_lastLine;

	BOOL		m_hasOrganizedOnce;
	
	BOOL		m_hasFillUpLine;
	
	POSITION	m_lastLinePos;
	POSITION	m_lastComponentPos;
	
	BOOL		m_changedVisibility;
	
	BOOL		m_bIsTopMostOrganizer;
};


class COrganizerWindow
{
public:
	
	COrganizerWindow()
	{
	}
	
	virtual void GetMinSize(CSize& size) = 0;
	
protected:
	
	BEGIN_MSG_MAP(COrganizerWindow)
		MESSAGE_HANDLER(CWTLWndComponentsOrganizer::WM_INNER_ORGANIZER_CHANGE, OnInnerOrganizerChange)
	END_MSG_MAP()
	
	inline LRESULT OnInnerOrganizerChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		BOOL bHasInnerOrganizerChanged = m_componentsOrganizer.VerifyAndRecalculateInnerOrganizers();
		
		ATLTRACE(_T("COrganizerWindow::OnInnerOrganizerChange()\n"));
		
		// A number of lines equal to "0" indicates a window that doesn't uses the 
		// WTLWndComponentsOrganizer class to organize itself, but that is a 
		// OrganizerWindow anyway.
		// This could be a reflection of the limited possibilities of this class on 
		// the beginning, what leads to another organization alternatives.
		//
		if ( (m_componentsOrganizer.GetIsTopMostOrganizer()) && (bHasInnerOrganizerChanged) )
		{
			// Find the top most window.
			//
			HWND parent = GetParent(m_componentsOrganizer.GetContainerWindow()->m_hWnd);
			HWND topMostWnd = NULL;
			
			while (parent)
			{
				topMostWnd = parent;
				parent = GetParent(topMostWnd);
			}
			
			// Now, forces it to call the WM_GETMINMAXINFO message to reorganize 
			// correctly by changing the size of the window.
			// 
			WINDOWPLACEMENT windowPlacement = {0};
			::GetWindowPlacement(topMostWnd, &windowPlacement);
			//windowPlacement.rcNormalPosition.right += 1;
			//windowPlacement.rcNormalPosition.bottom += 1;
			::SetWindowPlacement(topMostWnd, &windowPlacement);
		}
		else if ( (m_componentsOrganizer.GetNumberOfLines() == 0) || (bHasInnerOrganizerChanged) )
		{
			m_componentsOrganizer.PropagateChangeToParentOrganizer();
		}
		
		return 0L;
	}
	
protected:
	
	CWTLWndComponentsOrganizer		m_componentsOrganizer;
};


