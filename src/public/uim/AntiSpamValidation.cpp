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

// AntiSpamValidation.cpp: implementation of the CAntiSpamValidation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "AntiSpamValidation.h"


CString CAntiSpamValidation::strValidDomainChars="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-";
CString CAntiSpamValidation::strValidEmailChars="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-@_&";



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAntiSpamValidation::CAntiSpamValidation()
{

}

CAntiSpamValidation::~CAntiSpamValidation()
{

}


CAntiSpamValidation::ValidationStatus CAntiSpamValidation::checkDomain(const CString& refstrDomain)
{
	// Check for not null.
	if (refstrDomain.IsEmpty()) {
		return CAntiSpamValidation::AS_NULL_DOMAIN;
	}
	
	// Validate size.
	int length = refstrDomain.GetLength();
	if (length < CAntiSpamValidation::MIN_DOMAIN_LENGTH) {
		return CAntiSpamValidation::AS_DOMAIN_TOO_SHORT;
	}

	if (length > CAntiSpamValidation::MAX_DOMAIN_LENGTH) {
		return CAntiSpamValidation::AS_DOMAIN_TOO_LONG;
	}
	
	bool foundLetterOrDigit = false;
	bool foundDot = false;

	int iIndex=0;
//	CharacterIterator iter = new StringCharacterIterator(domain);
	TCHAR chData = refstrDomain.GetAt(iIndex); //iter.first();
	if (chData == '.')
	{
		return  CAntiSpamValidation::AS_DOT_IN_FIRST_CHAR;
	}
	
	TCHAR chLastChar = chData;
	for(iIndex=0 ; iIndex < refstrDomain.GetLength() ; iIndex++) {
		chData = refstrDomain.GetAt(iIndex);
		if (!isValidDomainCharacter(chData)) {
			return CAntiSpamValidation::AS_INVALID_CHARACTER;
		}
		if (isNonAccentedLetter(chData) || ::isdigit((int)chData)) {
			foundLetterOrDigit = true;
		}
		else if (chData == '.')
		{
			if (chLastChar == '.')
			{
				return CAntiSpamValidation::AS_CONSECUTIVE_DOTS_IN_DOMAIN;
			}
			foundDot = true;
		}
		
		chLastChar = chData;
	}
	
	if (chLastChar == '.')
	{
		return CAntiSpamValidation::AS_DOT_IN_LAST_CHAR;
	}
	
	if (!foundLetterOrDigit)
	{
		return CAntiSpamValidation::AS_NO_LETTER_OR_DIGIT_IN_DOMAIN;
	}
	
	if (!foundDot)
	{
		return CAntiSpamValidation::AS_NO_DOT_IN_DOMAIN;
	}
	
	// All OK.
	return CAntiSpamValidation::AS_NO_ERROR;
}

/*
CAntiSpamValidation::ValidationStatus CAntiSpamValidation::checkForumName(const CString& refstrName)
{
	// Check for a null forum name. Its OK, since it can be null. Return NO_ERROR.
	if (refstrName.IsEmpty()) {
		return CAntiSpamValidation::AS_NO_ERROR;
	}
	
	// Validate size.
	int length = refstrName.GetLength();
	if (length < CAntiSpamValidation::MIN_FORUM_NAME_LENGTH) {
		return CAntiSpamValidation::AS_FORUM_NAME_TOO_SHORT;
	}
	if (length > CAntiSpamValidation::MAX_FORUM_NAME_LENGTH) {
		return CAntiSpamValidation::AS_FORUM_NAME_TOO_LONG;
	}

	TCHAR chData;
	
	for(int iIndex = 0 ; iIndex < refstrName.GetLength() ; iIndex++) {
		chData = refstrName.GetAt(iIndex);
		if (!isValidForumNameCharacter(chData)) {
			return CAntiSpamValidation::AS_INVALID_CHARACTER;
		}
	}
	
	// All OK.
	return CAntiSpamValidation::AS_NO_ERROR;
}
*/


CAntiSpamValidation::ValidationStatus CAntiSpamValidation::checkEMail(CString& refstrEmail)
{
	CAntiSpamValidation::ValidationStatus result = checkEMailImpl(refstrEmail);
	
	return result;
}



CAntiSpamValidation::ValidationStatus CAntiSpamValidation::checkEMailImpl(CString& refstrEmail)
{
	refstrEmail.TrimLeft(";, \t\n\r\"'");
	refstrEmail.TrimRight(";, \t\n\r\"'");

	// Check for not null.
	if (refstrEmail.IsEmpty()) {
		return CAntiSpamValidation::AS_NULL_EMAIL;
	}	
	
	// Validate size.
	int length = refstrEmail.GetLength();
	if (length < CAntiSpamValidation::MIN_EMAIL_LENGTH) {
		return CAntiSpamValidation::AS_EMAIL_TOO_SHORT;
	}
	if (length > CAntiSpamValidation::MAX_EMAIL_LENGTH)
	{
		return CAntiSpamValidation::AS_EMAIL_TOO_LONG;
	}
	
	int atPos = refstrEmail.Find('@');
	// Assures that there is at least one at sign (@).
	if (atPos == -1)
	{
		return CAntiSpamValidation::AS_NO_AT_SIGN;
	}
	if (atPos == 0)
	{
		return CAntiSpamValidation::AS_NO_CHAR_BEFORE_AT;
	}	

	// Check that all characters are in the valid set.
	TCHAR chData;

	//CharacterIterator iter = new StringCharacterIterator(email.substring(0, atPos));
	bool foundLetterOrDigit = false;
	for(int iIndex = 0 ; iIndex < atPos ; iIndex++) {
		chData=refstrEmail.GetAt(iIndex);
		if (!isValidEMailCharacter(chData)) {
			return CAntiSpamValidation::AS_INVALID_CHARACTER;
		}
		if (isNonAccentedLetter(chData) || ::isdigit((int)chData)) {
			foundLetterOrDigit = true;
		}
	}
	
	if (!foundLetterOrDigit) {
		return CAntiSpamValidation::AS_NO_LETTER_OR_DIGIT_BEFORE_AT;
	}
	
	// Assures the string after the at sign has at least one letter or digit, at least one
	// dot (but not in the beginning or in the end of the segment) and that there is no other
	// at sign.
	return checkDomain( refstrEmail.Mid(atPos + 1,refstrEmail.GetLength() ) );

}


BOOL  CAntiSpamValidation::isValidDomainCharacter(TCHAR chData)
{
	return CAntiSpamValidation::strValidDomainChars.Find(chData) != -1;
}

BOOL  CAntiSpamValidation::isNonAccentedLetter(TCHAR chData)
{
	return (chData >= 'A' && chData <= 'Z') || (chData >= 'a' && chData <= 'z');
}


BOOL  CAntiSpamValidation::isValidForumNameCharacter(TCHAR chData)
{
	return chData != '[' && chData != ']';
}

BOOL  CAntiSpamValidation::isValidEMailCharacter(TCHAR chData)
{
	return CAntiSpamValidation::strValidEmailChars.Find(chData) != -1;
}

