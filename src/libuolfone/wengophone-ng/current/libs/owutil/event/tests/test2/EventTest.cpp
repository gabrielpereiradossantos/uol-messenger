/**
 * This program is a simple and small example of what can be achieved
 * with custom events.
 *
 * These events are wrappers around boost signals, they are defined in
 * Event.h.
 *
 * Commented lines are here to show the old syntax.
 *
 * You can build this example on GNU/Linux with the following command line in this directory:
 * g++ -I ../include/ -lboost_signals EventTest.cpp -o event_test
 */

#include <event/Event.h>

#include <util/Logger.h>

#include <string>
#include <iostream>

#include <cstdio>

class MyTimer {
public:

	Event<void (MyTimer *, const std::string &)> secondEvent;

	Event<void (MyTimer *, const std::string &)> minuteEvent;

	void tick() {
		//Sends the signal, updates the observers
		secondEvent(this, "Second Event");
		minuteEvent(this, "Minute Event");
	}
};

void showTime(MyTimer * sender, const std::string & time) {
	std::cout << time << std::endl;
}

class Clock : public Trackable {
public:

	Clock() {
		_crashVariable = 1;
	}

	~Clock() {
		_crashVariable = -1;
	}

	void showTime(MyTimer * sender, const std::string & time) const {
		std::cout << "Clock: " << time << _crashVariable << std::endl;
	}

private:

	int _crashVariable;
};


int main(void) {
	MyTimer timer;
	Clock * clock = new Clock();

	timer.secondEvent.connect(NULL, &showTime, NULL);
	//timer.secondEvent.connect(NULL, &showTime, NULL);

	timer.secondEvent.connect(clock, boost::bind(&Clock::showTime, clock, _1, _2), NULL);
	timer.secondEvent.connect(clock, boost::bind(&Clock::showTime, clock, _1, _2), NULL);

	std::string slotId = timer.minuteEvent.connect(clock, boost::bind(&Clock::showTime, clock, _1, _2), NULL);

	timer.tick();

	std::cout << "kikoo" << std::endl;

	//delete clock;

	std::cout << "kikoo" << std::endl;

	//Print: Second Event
	//       Clock: Second Event
	//       Clock: Minute Event

	//timer.minuteEvent -= boost::bind(&Clock::showTime, clock, _1, _2);
	//timer.minuteEvent.disconnect(&Clock::showTime);
	timer.minuteEvent.disconnect(slotId);
	timer.tick();
	//Print: Second Event
	//       Clock: Second Event

	//timer.secondEvent += timer.minuteEvent;

	getchar();
}
