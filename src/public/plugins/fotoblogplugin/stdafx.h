/* UOL Messenger
* Copyright (c) 2005 Universo Online S/A
*
* Direitos Autorais Reservados
* All rights reserved
*
* Este programa � software livre; voc� pode redistribu�-lo e/ou modific�-lo
* sob os termos da Licen�a P�blica Geral GNU conforme publicada pela Free
* Software Foundation; tanto a vers�o 2 da Licen�a, como (a seu crit�rio)
* qualquer vers�o posterior.
* Este programa � distribu�do na expectativa de que seja �til, por�m,
* SEM NENHUMA GARANTIA; nem mesmo a garantia impl�cita de COMERCIABILIDADE
* OU ADEQUA��O A UMA FINALIDADE ESPEC�FICA. Consulte a Licen�a P�blica Geral
* do GNU para mais detalhes. 
* Voc� deve ter recebido uma c�pia da Licen�a P�blica Geral do GNU junto
* com este programa; se n�o, escreva para a Free Software Foundation, Inc.,
* no endere�o 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA. 
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
* S�o Paulo SP - CEP 01452-002 - BRASIL  */

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
// Change these values to use different versions
#define WINVER			0x0500
#define _WIN32_WINNT	0x0500
#define _WIN32_WINDOWS	0x0500
#define _WIN32_IE		0x0500
#define _RICHEDIT_VER	0x0300	

#include <windows.h>
#include <winbase.h>
#define _WTL_NO_CSTRING				// ADDED MINIMUN PLUGIN
#define _ATL_USE_CSTRING_FLOAT		// ADDED MINIMUN PLUGIN

#include <atlbase.h>

#include <atlstr.h>
#include <atlapp.h>


//////////////////////////////////////////////////////////////////////////
//  ADDED MINIMUN PLUGIN - START
//////////////////////////////////////////////////////////////////////////

#include <atlwin.h>
#include <atltypes.h>
#include <atlcoll.h>
#include <atltime.h>
#include <atlcomtime.h>

const int UTIL_WTL_VER = _WTL_VER;

#include <util/util.h>
#include <util/atllistiterator.h>

#include <util/smartpointer.h>
#include <util/appversion.h>

#include <interfaces/iuolmessengerfacade.h>
#include <interfaces/uolmessengerplugin.h>

//////////////////////////////////////////////////////////////////////////
// // ADDED MINIMUN PLUGIN - END
//////////////////////////////////////////////////////////////////////////

#include <atlcom.h>
#include <comdef.h>
#include <atlcrack.h> 
#include <WinInet.h>
#include <atlutil.h>



extern IUOLMessengerFacade * g_pUOLMessengerFacade;