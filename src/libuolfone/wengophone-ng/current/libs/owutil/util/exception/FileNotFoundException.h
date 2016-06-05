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

#ifndef OWFILENOTFOUNDEXCEPTION_H
#define OWFILENOTFOUNDEXCEPTION_H

#include <util/owutildll.h>
#include <util/exception/Exception.h>

/**
 * Signals that an attempt to open the file
 * denoted by a specified pathname has failed.
 *
 * @see Exception
 * @see java.io.FileNotFoundException
 * @author Tanguy Krotoff
 */
class OWUTIL_API FileNotFoundException : public Exception {
public:

	FileNotFoundException(const std::string & message)
	: Exception("FileNotFoundException: " + message) {
	}

private:

	FileNotFoundException & operator=(const FileNotFoundException &);
};

#endif	//OWFILENOTFOUNDEXCEPTION_H
