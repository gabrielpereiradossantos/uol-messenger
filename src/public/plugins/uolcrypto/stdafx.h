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

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

#include <atlbase.h>
#include <atlstr.h>
#include <atlapp.h>

#include <atlwin.h>
#include <atltypes.h>
#include <atlcoll.h>
#include <atlcomtime.h>
#include <atlctrls.h>
#include <atlcrack.h>
#include <atltime.h>
#include <atlddx.h>
#include <atlutil.h>
#include <atlfile.h>
#include <atlenc.h>

#include <iostream>
#include <strstream>
#include <string>

#include <util/util.h>
#include <util/atllistiterator.h>

#include <util/smartpointer.h>
#include <util/appversion.h>

#include <interfaces/iuolmessengerfacade.h>
#include <interfaces/uolmessengerplugin.h>


#include <cryptopp552/cryptlib.h>
#include <cryptopp552/default.h>
#include <cryptopp552/rsa.h>
#include <cryptopp552/randpool.h>
#include <cryptopp552/osrng.h>
#include <cryptopp552/elgamal.h>
#include <cryptopp552/hex.h>	
#include <cryptopp552/base64.h>
#include <cryptopp552/blowfish.h>
#include <cryptopp552/aes.h>


