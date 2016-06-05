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

// AntiSpamValidation.h: interface for the CAntiSpamValidation class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


class CAntiSpamValidation  
{
public:
	enum ValidationStatus { AS_NO_ERROR = 0,AS_NULL_DOMAIN = 1, AS_DOMAIN_TOO_SHORT = 2,
	AS_DOMAIN_TOO_LONG = 3, AS_INVALID_CHARACTER = 4, AS_CONSECUTIVE_DOTS_IN_DOMAIN = 5,
	AS_DOT_IN_FIRST_CHAR = 6, AS_DOT_IN_LAST_CHAR = 7, AS_NO_LETTER_OR_DIGIT_IN_DOMAIN = 8,
	AS_NO_DOT_IN_DOMAIN = 9, AS_NULL_EMAIL = 10, AS_EMAIL_TOO_SHORT = 11, AS_EMAIL_TOO_LONG = 12,
	AS_NO_AT_SIGN = 13, AS_NO_CHAR_BEFORE_AT = 14, AS_NO_LETTER_OR_DIGIT_BEFORE_AT = 15,
	AS_NULL_FORUM_NAME = 20, AS_FORUM_NAME_TOO_SHORT = 21, AS_FORUM_NAME_TOO_LONG = 22 };
	


    static CAntiSpamValidation::ValidationStatus checkDomain(const CString& refstrDomain);
	//static CAntiSpamValidation::ValidationStatus checkForumName(const CString& refstrName);
    
	static CAntiSpamValidation::ValidationStatus checkEMail(CString& refstrEmail	);

	
protected:
    static CAntiSpamValidation::ValidationStatus checkEMailImpl(CString& refstrEmail);
	CAntiSpamValidation();
	virtual ~CAntiSpamValidation();

	static BOOL isValidDomainCharacter(TCHAR chData);
	static BOOL isNonAccentedLetter(TCHAR chData);
	static BOOL isDigit(TCHAR chData);
	static BOOL isValidForumNameCharacter(TCHAR chData);
	static BOOL isValidEMailCharacter(TCHAR chData);

	enum {MIN_DOMAIN_LENGTH = 4};
	enum {MAX_DOMAIN_LENGTH = 128};
	enum { MIN_EMAIL_LENGTH = 6};
	enum { MAX_EMAIL_LENGTH = 128};
	enum { MIN_FORUM_NAME_LENGTH = 2};
	enum { MAX_FORUM_NAME_LENGTH = 64};
	static CString strValidDomainChars;
	static CString strValidEmailChars;
};


   


