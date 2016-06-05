/* UOL Fone
 * Copyright (c) 2006 Universo Online S/A
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
 * Universo Online S/A - A/C: UOL Fone 5o. Andar
 * Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
 * São Paulo SP - CEP 01452-002 - BRASIL  */


#pragma once
#include "IUOLFoneClientPhoneLine.h"



[
	export
]
enum EnumPhoneCallState
{
	UFC_PHONE_CALL_STATE_UNKNOWN,
	UFC_PHONE_CALL_STATE_ERROR,
	UFC_PHONE_CALL_STATE_RESUMED,
	UFC_PHONE_CALL_STATE_TALKING,
	UFC_PHONE_CALL_STATE_DIALING,
	UFC_PHONE_CALL_STATE_RINGING,
	UFC_PHONE_CALL_STATE_CLOSED,
	UFC_PHONE_CALL_STATE_INCOMING,
	UFC_PHONE_CALL_STATE_HOLD,
	UFC_PHONE_CALL_STATE_BUSY,
	UFC_PHONE_CALL_STATE_NO_ANSWER,
	UFC_PHONE_CALL_STATE_REDIRECTED
};


[	
	export
]
enum EnumTone
{
	UFC_TONE_0 = 0,		// DTMF 0
	UFC_TONE_1,		// DTMF 1	
	UFC_TONE_2,		// DTMF 2
	UFC_TONE_3,		// DTMF 3
	UFC_TONE_4,		// DTMF 4
	UFC_TONE_5,		// DTMF 5
	UFC_TONE_6,		// DTMF 6
	UFC_TONE_7,		// DTMF 7
	UFC_TONE_8,		// DTMF 8
	UFC_TONE_9,		// DTMF 9
	UFC_TONE_STAR,	// DTMF *
	UFC_TONE_POUND	// DTMF #
	//UFC_TONE_DIAL_TONE,
	//UFC_TONE_BUSY,
	//UFC_TONE_RING_BACK,
	//UFC_TONE_RING_TONE,
	//UFC_TONE_CALL_FAILED,
	//UFC_TONE_SILENCE,
	//UFC_TONE_BACKSPACE,
	//UFC_TONE_CALL_WAITING,
	//UFC_TONE_CALL_HELD,
	//UFC_TONE_LOUD_FAST_BUSY
};


[	
	export
]
enum EnumFailureCode
{
	UFC_FAILURE_CODE_BAD_REQUEST = 400,
	UFC_FAILURE_CODE_UNAUTHORIZED = 401,
	UFC_FAILURE_CODE_PAYMENT_REQUIRED = 402,
	UFC_FAILURE_CODE_FORBIDDEN = 403,
	UFC_FAILURE_CODE_NOT_FOUND = 404,
	UFC_FAILURE_CODE_METHOD_NOT_ALLOWED = 405,
	UFC_FAILURE_CODE_NOT_ACCEPTABLE = 406,
	UFC_FAILURE_CODE_PROXY_AUTHENTICATION_REQUIRED = 407,
	UFC_FAILURE_CODE_TIMEOUT = 408,
	UFC_FAILURE_CODE_GONE = 410,
	UFC_FAILURE_CODE_REQUEST_ENTITY_TOO_LARGE = 413,
	UFC_FAILURE_CODE_REQUEST_URI_TOO_LONG = 414,
	UFC_FAILURE_CODE_UNSUPORTED_MEDIA = 415,
	UFC_FAILURE_CODE_UNSUPORTED_URI_SCHEME = 416,
	UFC_FAILURE_CODE_BAD_EXTENSION = 420,
	UFC_FAILURE_CODE_EXTENSION_REQUIRED = 421,
	UFC_FAILURE_CODE_INTERVAL_TOO_BRIEF = 423,
	UFC_FAILURE_CODE_TEMPORARILY_UNAVAILABLE = 480,
	UFC_FAILURE_CODE_TRANSACTION_NOT_EXIST = 481,
	UFC_FAILURE_CODE_LOOP_DETECTED = 482,
	UFC_FAILURE_CODE_TOO_MANY_HOPS = 483,
	UFC_FAILURE_CODE_ADDRESS_INCOMPLETE = 484,
	UFC_FAILURE_CODE_AMBIGUOUS = 485,
	UFC_FAILURE_CODE_BUSY = 486,
	UFC_FAILURE_CODE_REQUEST_TERMINATED = 487,
	UFC_FAILURE_CODE_NOT_ACCEPTABLE_HERE = 488,
	UFC_FAILURE_CODE_REQUEST_PENDING = 491,
	UFC_FAILURE_CODE_UNDECIPHERABLE = 493
};


// IUOLFoneClientPhoneCall
[
	object,
	uuid("B47AC133-41B8-48f5-8E10-D259E4F4E215"),
	dual,
	helpstring("IUOLFoneClientPhoneCall Interface"),
	pointer_default(unique)
]
__interface IUOLFoneClientPhoneCall : IDispatch
{
	[id(1)] HRESULT GetPhoneLine([out] IUOLFoneClientPhoneLine** ppPhoneLine);
	[id(2)] HRESULT GetPhoneURI([out] BSTR* pbstrPhoneURI);
	[id(3)] HRESULT IsVideoEnabled([out] BOOL* pbEnabled);
	[id(4)] HRESULT GetPhoneCallId([out] LONG* plCallId);
	[id(5)] HRESULT GetPhoneCallState([out] LONG* plCallState);
};