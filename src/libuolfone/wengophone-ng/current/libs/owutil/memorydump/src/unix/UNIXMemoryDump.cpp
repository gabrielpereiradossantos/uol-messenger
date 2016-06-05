/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2007  Wengo
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

#include <memorydump/UNIXMemoryDump.h>

#include <google/coredumper.h>

#include <util/Logger.h>
#include <util/Path.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/resource.h>

UNIXMemoryDump::UNIXMemoryDump(const std::string & applicationName, const std::string & revision)
	: MemoryDump(applicationName, revision) {

	doJob();

	// avoid an additional core dump
	struct rlimit rlim = {0, 0};
	setrlimit(RLIMIT_CORE, &rlim);
	abort();
}

UNIXMemoryDump::~UNIXMemoryDump() {
}

void UNIXMemoryDump::doJob() {

	std::string memoryDumpName;
	memoryDumpName += _applicationName;
	memoryDumpName += "-rev";
	memoryDumpName += _revision;
	memoryDumpName += "-";
	memoryDumpName += getCurrentDateTime();
	memoryDumpName += ".core";

	const char * coreDir = getenv("WENGOPHONE_COREDUMP_DIR");
	if(!coreDir) {
		coreDir = getenv("TMPDIR");
	}
	if(!coreDir) {
		coreDir = "/tmp";
	}
	std::string memoryDumpFile(coreDir);
	memoryDumpFile += '/' + memoryDumpName;

	WriteCoreDump(memoryDumpFile.c_str());

	//Flushes the logger file
	Logger::logger.flush();

	std::string command = Path::getApplicationDirPath() + "owcrashreport";
	if (!fork()) {
		execlp(command.c_str(), command.c_str(), "-d",
			memoryDumpFile.c_str(), "-n", _applicationName.c_str(), NULL
		);
	}
}

