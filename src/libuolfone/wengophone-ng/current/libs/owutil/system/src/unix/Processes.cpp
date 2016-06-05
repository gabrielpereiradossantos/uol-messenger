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

#include <system/Processes.h>

#include <dirent.h>
#include <fcntl.h>

#define BUFFERSIZE 1024
#define FILENAMESIZE 128
#define PROCESSNAMESIZE 512

bool Processes::isRunning(const std::string & searchName) {
	struct dirent * ent;
	DIR * proc;

	proc = opendir("/proc");
	int count = 0;
	while ((ent = readdir (proc))) {

		if (isdigit(ent->d_name[0])) {
			int pid = atoi(ent->d_name);
			char statusFilename[FILENAMESIZE], processName[PROCESSNAMESIZE], buffer[BUFFERSIZE];
			int fd;

			sprintf(statusFilename, "/proc/%d/status", pid);
			fd = open(statusFilename, O_RDONLY, 0);
			if (fd < 0) {
				continue;
			}
			int nread = read(fd, buffer, BUFFERSIZE - 1);
			close(fd);
			buffer[nread] = '\0';
			sscanf(buffer, "%s %s", processName, processName);

			//printf("%d - %s\n", pid, processName);
			if (std::string(processName) == searchName) {
				count++;
				if (count > 1) {
					return true;
				}
			}
		}
	}
	closedir(proc);

	return false;
}

bool Processes::killProcess(const std::string & processName) {
	return false;
}
