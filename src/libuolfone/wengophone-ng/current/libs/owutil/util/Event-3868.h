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

#include <util/NonCopyable.h>

#include <util/IPostEvent.h>
#include <util/ThreadEvent.h>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <list>
#include <iostream>

/**
 * Delegates/Design Pattern Observer.
 *
 * This class helps to implement the subject part of the observer design pattern.
 *
 * Uses boost::function and boost::signal in order to simplify the use of the pattern.
 * Works a bit like the C# observer pattern that uses delegates and events.
 *
 * You can also connect an Event to another.
 *
 * An class Trackable is available for automatic disconnection when an object is being destroyed.
 * boost::signal are not thread safe:
 * http://boost.org/doc/html/signals/s04.html#id2738867
 *
 * Example (the boost::signal syntax is commented so that one can compare both):
 * <pre>
 * void showTime(MyTimer * sender, const std::string & time) {
 *     std::cout << time << std::endl;
 * }
 *
 * class Clock {
 * public:
 *     void showTime(MyTimer * sender, const std::string & time) const {
 *         std::cout << "Clock: " << time << std::endl;
 *     }
 * };
 *
 * class MyTimer {
 * public:
 *
 *     Event<void (MyTimer *, const std::string &)> secondEvent;
 *     //boost::signal<void (const std::string &)> secondEvent;
 *
 *     Event<void (MyTimer *, const std::string &)> minuteEvent;
 *     //boost::signal<void (const std::string &)> minuteEvent;
 *
 *     void tick() {
 *         //Sends the signal, updates the observers
 *         secondEvent(this, "Second Event");
 *         minuteEvent(this, "Minute Event");
 *     }
 * };
 *
 * MyTimer timer;
 * Clock clock;
 *
 * timer.secondEvent += &showTime;
 * //timer.secondEvent.connect(&showTime);
 * timer.secondEvent += boost::bind(&Clock::showTime, &clock, _1, _2);
 * //timer.secondEvent.connect(boost::bind(&Clock::showTime, &clock, _1);
 * timer.minuteEvent += boost::bind(&Clock::showTime, &clock, _1, _2);
 * //timer.minuteEvent.connect(boost::bind(&Clock::showTime, &clock, _1);
 * timer.tick();
 *    //Print: Second Event
 *    //       Clock: Second Event
 *    //       Clock: Minute Event
 *
 * timer.minuteEvent -= boost::bind(&Clock::showTime, &clock, _1, _2);
 * //timer.minuteEvent.disconnect(boost::bind(&Clock::showTime, &clock, _1));
 * timer.tick();
 *    //Print: Second Event
 *    //       Clock: Second Event
 * </pre>
 *
 * A good pratice is to always have the sender as the first parameter of the Event.
 *
 * @author Tanguy Krotoff
 */
template<typename Signature>
class Event : NonCopyable {
public:

	Event() {
	}

	~Event() {
		for (typename SlotList::iterator it = _slotList.begin();
			it != _slotList.end(); ++it) {
				disconnect(it->slot);
		}
		_slotList.clear();

		for (typename EventList::iterator it = _eventList.begin();
			it != _eventList.end(); ++it) {
				disconnect(**it);
		}
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
	 */
	void connect(const boost::function<Signature> & slot, IPostEvent * postEvent) {
		disconnect(slot);
		_slotList.push_back(Slot(slot, postEvent));
	}

	/**
	 * Removes/detaches a slot.
	 *
	 * Equivalent to boost::signal::disconnect()
	 *
	 * @param slot callback function to be removed
	 * @return true if the slot has been removed from the list; false otherwise
	 */
	bool disconnect(const boost::function<Signature> & slot) {
		for (typename SlotList::iterator it = _slotList.begin();
			it != _slotList.end(); ++it) {
			if (functionEqual(slot, it->slot)) {
				_slotList.erase(it);
				return true;
			}
		}
		return false;
	}

	void connect(const Event & event) {
		disconnect(event);
		_eventList.push_back(&event);
	}

	bool disconnect(const Event & event) {
		typename EventList::iterator it = std::find(_eventList.begin(), _eventList.end(), &event);
		if (it != _eventList.end()) {
			_eventList.erase(it);
			return true;
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

	/**
	 * Compares 2 boost::function together.
	 *
	 * Not safe but still usefull.
	 *
	 * @param slot1 first boost::function to compare
	 * @param slot2 second boost::function to compare
	 * @return true if equal; false otherwise
	 */
	static bool functionEqual(const boost::function<Signature> & slot1, const boost::function<Signature> & slot2) {
		std::cout << sizeof(slot1) << " " << sizeof(slot2) << std::endl;
		return sizeof(slot1) == sizeof(slot2);
	}


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


	/** Associates a slot with a IPostEvent. */
	struct Slot {

		Slot(const boost::function<Signature> & _slot, IPostEvent * _postEvent) {
			slot = _slot;
			postEvent = _postEvent;
		}

		/** Slot/observer/event handler. */
		boost::function<Signature> slot;

		/** Destination threads of the event. */
		IPostEvent * postEvent;
	};


	typedef std::list<Slot> SlotList;

	/** The vector/collection/list of slots. */
	SlotList _slotList;


	typedef std::list<Event<Signature> * > EventList;

	/** Second observer list that contains Event rather than boost::function. */
	EventList _eventList;
};

#endif	//OWEVENT_H
