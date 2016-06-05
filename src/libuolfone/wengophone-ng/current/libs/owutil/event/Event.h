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

#ifndef OWEVENT_H
#define OWEVENT_H

#include <event/IEvent.h>
#include <event/IPostEvent.h>
#include <event/ThreadEvent.h>
#include <event/Slot.h>
#include <event/Trackable.h>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <list>
#include <iostream>

/**
 * Delegates/Design Pattern Observer.
 *
 * This class helps to implement the subject part of the observer design pattern.
 *
 * A good pratice is to always have the sender as the first parameter of the Event.
 *
 * @author Tanguy Krotoff
 */
template<typename Signature>
class Event : public IEvent, public Trackable {
public:

	Event() {
	}

	~Event() {
		_slotList.clear();
		_eventList.clear();
	}

	/**
	 * Adds/attaches a slot.
	 *
	 * Two identical slots cannot be attached, only one will be:
	 * it checks first if the same slot was not attached already.
	 *
	 * Equivalent to boost::signal::connect()
	 *
	 * @param slot callback function that will observe this object
	 * @return slot ID
	 */
	template<typename T>
	int connect(Trackable * trackable, const T & slot, IPostEvent * postEvent) {
		disconnect(trackable, slot, postEvent);
		return connect(Slot<Signature>(trackable, slot, postEvent));
	}

	int connect(const Slot<Signature> & slot) {
		_slotList.push_back(slot);
		if (slot.trackable) {
			slot.trackable->addEvent(this);
		}
		return slot.id;
	}

	void connect(const Event & event) {
		_eventList.push_back(&event);
		event.addEvent(this);
	}

	template<typename T>
	bool disconnect(Trackable * trackable, const T & slot, IPostEvent * postEvent) {
		for (typename SlotList::iterator it = _slotList.begin();
			it != _slotList.end(); ++it) {

			if (it->trackable == trackable && it->slot == slot && it->postEvent == postEvent) {
				_slotList.erase(it);
				return true;
			}
		}
		return false;
	}

	bool disconnect(Trackable * trackable) {
		bool ret = false;
		if (trackable) {
			for (typename SlotList::iterator it = _slotList.begin();
				it != _slotList.end(); ++it) {

				if (it->trackable == trackable) {
					_slotList.erase(it);
					ret = true;
				}
			}
		}
		return ret;
	}

	bool disconnect(int slotId) {
		for (typename SlotList::iterator it = _slotList.begin();
			it != _slotList.end(); ++it) {

			if (it->id == slotId) {
				_slotList.erase(it);
				return true;
			}
		}
		return false;
	}

	/**
	 * Notifies all the observer from the list that the Subject has been changed.
	 */
	void operator()() const {
		notify();
	}

	template<typename Arg1>
	void operator()(Arg1 arg1) const {
		notify(arg1);
	}

	template<typename Arg1, typename Arg2>
	void operator()(Arg1 arg1, Arg2 arg2) const {
		notify(arg1, arg2);
	}

	template<typename Arg1, typename Arg2, typename Arg3>
	void operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3) const {
		notify(arg1, arg2, arg3);
	}

	template<typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) const {
		notify(arg1, arg2, arg3, arg4);
	}

	template<typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	void operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) const {
		notify(arg1, arg2, arg3, arg4, arg5);
	}

private:

	/** No argument. */
	void notify() const {
		for (typename SlotList::const_iterator it = _slotList.begin();
			it != _slotList.end(); ++it) {

			if (it->postEvent) {
				typedef ThreadEvent1<void (boost::function<Signature>), boost::function<Signature> > MyThreadEvent;
				MyThreadEvent * event = new MyThreadEvent(boost::bind(&Event::notifyThreadSafe, this, _1), it->slot);
				it->postEvent->postEvent(event);
			} else {
				it->slot();
			}

		}

		for (typename EventList::const_iterator it = _eventList.begin();
			it != _eventList.end(); ++it) {
				(*it)->notify();
		}
	}

	void notifyThreadSafe(boost::function<Signature> slot) const {
		slot();
	}


	/** 1 argument. */
	template<typename Arg1>
	void notify(Arg1 arg1) const {
		for (typename SlotList::const_iterator it = _slotList.begin();
			it != _slotList.end(); ++it) {

			if (it->postEvent) {
				typedef ThreadEvent2<void (boost::function<Signature>, Arg1), boost::function<Signature>, Arg1> MyThreadEvent;
				MyThreadEvent * event = new MyThreadEvent(boost::bind((void (Event::*) (boost::function<Signature>, Arg1) const) &Event::notifyThreadSafe, this, _1, _2),
								it->slot, arg1);
				it->postEvent->postEvent(event);

			} else {
				it->slot(arg1);
			}

		}

		for (typename EventList::const_iterator it = _eventList.begin();
			it != _eventList.end(); ++it) {
				(*it)->notify(arg1);
		}
	}

	template<typename Arg1>
	void notifyThreadSafe(boost::function<Signature> slot, Arg1 arg1) const {
		slot(arg1);
	}


	/** 2 arguments. */
	template<typename Arg1, typename Arg2>
	void notify(Arg1 arg1, Arg2 arg2) const {
		for (typename SlotList::const_iterator it = _slotList.begin();
			it != _slotList.end(); ++it) {
				it->slot(arg1, arg2);
		}

		for (typename EventList::const_iterator it = _eventList.begin();
			it != _eventList.end(); ++it) {
				(*it)->notify(arg1, arg2);
		}
	}


	/** 3 arguments. */
	template<typename Arg1, typename Arg2, typename Arg3>
	void notify(Arg1 arg1, Arg2 arg2, Arg3 arg3) const {
		for (typename SlotList::const_iterator it = _slotList.begin();
			it != _slotList.end(); ++it) {
				(*it)(arg1, arg2, arg3);
		}

		for (typename EventList::const_iterator it = _eventList.begin();
			it != _eventList.end(); ++it) {
				(*it)->notify(arg1, arg2, arg3);
		}
	}


	/** 4 arguments. */
	template<typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void notify(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) const {
		for (typename SlotList::const_iterator it = _slotList.begin();
			it != _slotList.end(); ++it) {
				(*it)(arg1, arg2, arg3, arg4);
		}

		for (typename EventList::const_iterator it = _eventList.begin();
			it != _eventList.end(); ++it) {
				(*it)->notify(arg1, arg2, arg3, arg4);
		}
	}


	/** 5 arguments. */
	template<typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	void notify(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) const {
		for (typename SlotList::const_iterator it = _slotList.begin();
			it != _slotList.end(); ++it) {
				(*it)(arg1, arg2, arg3, arg4, arg5);
		}

		for (typename EventList::const_iterator it = _eventList.begin();
			it != _eventList.end(); ++it) {
				(*it)->notify(arg1, arg2, arg3, arg4, arg5);
		}
	}


	typedef std::list<Slot<Signature> > SlotList;

	/** The vector/collection/list of slots. */
	SlotList _slotList;


	typedef std::list<Event<Signature> * > EventList;

	/** Second observer list that contains Event rather than boost::function. */
	EventList _eventList;
};

#endif	//OWEVENT_H
