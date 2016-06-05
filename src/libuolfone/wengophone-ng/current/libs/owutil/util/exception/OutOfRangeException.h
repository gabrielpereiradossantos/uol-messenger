/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef OWOUTOFRANGEEXCEPTION_H
#define OWOUTOFRANGEEXCEPTION_H

#include <util/owutildll.h>
#include <util/exception/Exception.h>

/**
 * Thrown to indicate that an index of some sort
 * (such as to an array, to a string, or to a vector) is out of range.
 *
 * @see Exception
 * @see java.lang.IndexOutOfBoundsException
 * @see std::out_of_range
 * @author Tanguy Krotoff
 */
class OWUTIL_API OutOfRangeException : public Exception {
public:

	OutOfRangeException(const std::string & message)
	: Exception("OutOfRangeException: " + message) {
	}

private:

	OutOfRangeException & operator=(const OutOfRangeException &);
};

#endif	//OWOUTOFRANGEEXCEPTION_H
