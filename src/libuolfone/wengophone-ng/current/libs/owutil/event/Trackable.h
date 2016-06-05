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

#ifndef OWTRACKABLE_H
#define OWTRACKABLE_H

#include <event/IEvent.h>

#include <util/NonCopyable.h>

#include <list>

class Trackable : NonCopyable {
public:

	Trackable();

	~Trackable();

public:

	void addEvent(IEvent * event);

	void removeEvent(IEvent * event);

	void removeAllEvents();

	typedef std::list<IEvent * > EventList;

	/** Event list. */
	EventList _eventList;
};

#endif	//OWTRACKABLE_H
