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

#ifndef OWFILE_H
#define OWFILE_H

#include <util/owutildll.h>
#include <util/Interface.h>
#include <util/StringList.h>

#include <string>
#include <fstream>

/**
 * File interface.
 *
 * An abstract representation of file and directory pathnames.
 *
 * TODO use FileNotFoundException?
 *
 * @see QFile
 * @see java.io.*
 * @author Tanguy Krotoff
 */
class File : NonCopyable {
public:

	/**
	 * Filename encoding.
	 */
	enum Encoding {
		EncodingDefault,
		EncodingUTF8
	};

	OWUTIL_API File(const std::string & filename, Encoding encoding = File::EncodingDefault);

	OWUTIL_API File(const File & file);

	OWUTIL_API virtual ~File() {
	}

	OWUTIL_API File & operator=(const File & file);

	/**
	 * Gets the file extension.
	 *
	 * @return the file extension or empty string if there's no extension
	 */
	OWUTIL_API std::string getExtension() const;

	/**
	 * Moves a file.
	 *
	 * @param newPath the path we want to move the file to
	 * @param overwrite true to overwrite the file if it exists.
	 *
	 * @return true if the operation succeeds otherwise false.
	 */
	OWUTIL_API bool move(const std::string & newPath, bool overwrite = false);

	/**
	 * Removes a file or a directory recursively.
	 *
	 * @return true if the operation succeeds otherwise false.
	 */
	OWUTIL_API bool remove();

	/**
	 * Copies a file or a directory recursively.
	 *
	 * @param dstPath destination path. Will be created if does not exist
	 * @return true if operation successful
	 */
	OWUTIL_API bool copy(const std::string & path);

	/**
	 * Copy a single file.
	 *
	 * @param dst destination path or file
	 * @param src source
	 * @return true if successful
	 */
	OWUTIL_API static bool copyFile(const std::string & dst, const std::string & src);

	/**
	 * Gets the path to the file.
	 *
	 * The path does not contain the filename, it stops at the last /
	 *
	 * @return path to the file
	 */
	OWUTIL_API std::string getPath() const;

	/**
	 * Gets the full path to the file.
	 *
	 * @return path to the file
	 */
	OWUTIL_API std::string getFullPath() const;

	/**
	* Gets the name of the file without its path.
	*
	* @return the name of the file without its path
	*/
	OWUTIL_API std::string getFileName() const;

	/**
	 * Gets directory list.
	 *
	 * @return a list of directories contained in 'this' directory
	 */
	OWUTIL_API StringList getDirectoryList() const;

	/**
	 * Gets file list.
	 *
	 * TODO Not implemented yet.
	 *
	 * @return a list of files contained in 'this' directory
	 */
	OWUTIL_API StringList getFileList() const;

	/**
	 * Gets the file size.
	 *
	 * @return the file size in bytes.
	 */
	OWUTIL_API unsigned getSize() const;

	/**
	 * Gets pathName with the '/' separators converted to separators that are appropriate for the underlying operating system.
	 *
	 * On Windows, convertPathSeparators("c:/winnt/system32") returns "c:\winnt\system32".
	 *
	 * @param path path to convert
	 * @return path converted
	 */
	OWUTIL_API static std::string convertPathSeparators(const std::string & path);

	/**
	 * Gets the native directory separator: "/" under Unix (including Mac OS X) and "\" under Windows.
	 *
	 * @return native system path separator
	 */
	OWUTIL_API static std::string getPathSeparator();

	/**
	 * Creates directories recursively if the path does not exist.
	 *
	 * If path exists, nothing happends.
	 *
	 * @param path the path to create
	 */
	OWUTIL_API static void createPath(const std::string & path);

	/**
	 * Creates a temporary file.
	 *
	 * @return the temporary file
	 */
	OWUTIL_API static File createTemporaryFile();

	/**
	 * @return true if the given path exists.
	 */
	OWUTIL_API static bool exists(const std::string & path);

	/**
	 * @return true if the file is a directory.
	 */
	OWUTIL_API static bool isDirectory(const std::string & filename);

protected:

	std::string _filename;

	File::Encoding _encoding;
};


/**
 * Interface for FileReader and FileWriter.
 *
 * Code factorization.
 *
 * @author Tanguy Krotoff
 */
class IFile : Interface {
public:

	/**
	 * Closes the file.
	 */
	virtual void close() = 0;

protected:

	/**
	 * Opens the file for writing or reading.
	 *
	 * @return true if success; false otherwise
	 */
	virtual bool open() = 0;

	/**
	 * Gets if the file is open or not.
	 *
	 * @return true if file open; false otherwise
	 */
	virtual bool isOpen() /*const*/ = 0;
};


/**
 * Reads from a file.
 *
 * @author Tanguy Krotoff
 */
class FileReader : public File, public IFile {
public:

	OWUTIL_API FileReader(const std::string & filename);

	OWUTIL_API FileReader(const FileReader & fileReader);

	OWUTIL_API FileReader(const File & file);

	OWUTIL_API ~FileReader();

	OWUTIL_API bool open();

	/**
	 * Reads data from the file.
	 *
	 * You must call open() first and check the returned value otherwise
	 * it will make an assertion (a crash).
	 *
	 * @return data read from the file
	 */
	OWUTIL_API std::string read();

	OWUTIL_API void close();

private:

	bool isOpen();

	std::ifstream _file;
};


/**
 * Writes to a file.
 *
 * @author Tanguy Krotoff
 */
class FileWriter : public File, public IFile {
public:

	OWUTIL_API FileWriter(const std::string & filename, bool binaryMode = true);

	OWUTIL_API FileWriter(const FileWriter & fileWriter, bool binaryMode = true);

	OWUTIL_API FileWriter(const File & file, bool binaryMode = true);

	OWUTIL_API ~FileWriter();

	/**
	 * Writes data to the file.
	 *
	 * You must call open() first and check the returned value otherwise
	 * it will make an assertion (a crash).
	 *
	 * @param data data to write to the file
	 */
	OWUTIL_API void write(const std::string & data);

	OWUTIL_API void close();

	/**
	 * Set a value to _appendMode
	 *
	 * WARNING : can NOT be used when the file is already open !!
	 *
	 * WHY THIS IS NOT INSIDE THE CONSTRUCTOR SINCE IT CANNOT BE CHANGED LATER ON???
	 *
	 * @param newAppendMode : value wanted
	 * @return value obtained
	 */
	OWUTIL_API bool setAppendMode(bool appendMode);

private:

	bool open();

	bool isOpen();

	std::ofstream _file;

	bool _fileOpen;

	bool _binaryMode;

	bool _appendMode;
};

#endif	//OWFILE_H
