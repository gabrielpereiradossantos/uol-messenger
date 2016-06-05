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


[	
	export
]
enum EnumConfFailureCode
{
	UFC_CONF_FAILURE_CODE_NOT_CREATED = 700,
	UFC_CONF_FAILURE_CODE_ALREADY_STARTED = 701,
	UFC_CONF_FAILURE_CODE_TOO_MANY_PARTICIPANTS = 702
};


[
	export
]
enum EnumConfConstants
{
	UFC_CONF_CONST_INVALID_PARTICIPANT_INDEX = -1,
	UFC_CONF_CONST_INVALID_PHONE_CALL_ID = -1,
	UFC_CONF_CONST_DEFAULT_CONFERENCE_ID_NUMBER = 1,
	UFC_CONF_CONST_MAX_NUMBER_OF_PARTICIPANTS = 2
};


// IUOLFoneClientConferenceCall
[
	object,
	uuid("990862D7-2642-4e2c-8BBB-9CB9EE645BEC"),
	dual,
	helpstring("IUOLFoneClientConferenceCall Interface"),
	pointer_default(unique)
]
__interface IUOLFoneClientConferenceCall : IDispatch
{
	[id(1)] HRESULT GetConferenceId([out] LONG* plConferenceCallId);
	[id(2)] HRESULT GetNumberOfParticipants([out] LONG* plNumberOfParticipants);
	[id(3)] HRESULT GetParticipantCallId([in] LONG lParticipantIndex, [out] LONG* lpParticipantCallId);
};

