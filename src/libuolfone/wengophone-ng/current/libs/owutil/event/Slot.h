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

#ifndef OWSLOT_H
#define OWSLOT_H

#include <util/NonCopyable.h>

#include <util/Logger.h>

#include <boost/function.hpp>

class IPostEvent;
class Trackable;

/**
 * Associates a slot with a IPostEvent.
 *
 * @author Tanguy Krotoff
 */
template<typename Signature>
class Slot : NonCopyable {
public:

	/**
	 * Constructs a slot.
	 *
	 * @param trackable object associated with this slot
	 * @param slot boost::function, slot callback
	 * @param postEvent thread where the slot will be executed
	 */
	Slot(Trackable * trackable, const boost::function<Signature> & slot, IPostEvent * postEvent) {
		this->trackable = trackable;
		this->slot = slot;
		this->postEvent = postEvent;
		id++;

		LOG_DEBUG("slot created=" + String::fromNumber(id));
	}

	Slot(const Slot<Signature> & slot) {
		this->trackable = slot.trackable;
		this->slot = slot.slot;
		this->postEvent = slot.postEvent;
		this->id = slot.id;

		LOG_DEBUG("slot copied=" + id);
	}

	Slot<Signature> & operator=(const Slot<Signature> & slot) {
		this->trackable = slot.trackable;
		this->slot = slot.slot;
		this->postEvent = slot.postEvent;
		this->id = slot.id;

		LOG_DEBUG("slot copied=" + String::fromNumber(id));

		return *this;
	}

	~Slot() {
		LOG_DEBUG("slot destroyed=" + String::fromNumber(id));
	}

public:

	/** Object associated with this slot. */
	Trackable * trackable;

	/** Slot/observer/event handler. */
	boost::function<Signature> slot;

	/** Destination threads of the event. */
	IPostEvent * postEvent;

	/** Unique ID. */
	static int id;
};

template<typename Signature> int Slot<Signature>::id = 0;

#endif	//OWSLOT_H
