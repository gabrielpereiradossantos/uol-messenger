#include "ThreadEventTest.h"

#include <event/Event.h>

#include <util/Logger.h>

#include <thread/Thread.h>

#include <qtutil/QtThreadEvent.h>
#include <qtutil/ThreadEventFilter.h>

#include <QtGui/QtGui>

class Message {
public:

	Message(int i) {
		_i = i;
	}

	std::string toString() const {
		return std::string("message=" + String::fromNumber(_i));
	}

private:

	int _i;
};

class Model : public Thread {
public:

	Event<void ()> messageSentEvent;

	Event<void (const std::string & message)> messageSentEvent2;

	Event<void (const Message * message)> messageSentEvent3;

	Event<void ()> destroyPresentationEvent;

	Model() {
	}

	void setPresentation(Presentation * presentation) {
		_presentation = presentation;
	}

	void run() {
		static int i = 0;
		while (i < 10) {
			LOG_DEBUG("messageSentEvent");
			messageSentEvent();
			messageSentEvent2("message=" + String::fromNumber(i));
			messageSentEvent3(&Message(i));
			msleep(300);
			i++;
		}
		destroyPresentationEvent();
		msleep(300);
	}

private:

	Presentation * _presentation;
};

class QtPostEvent : public IPostEvent {
public:

	QtPostEvent(QObject * receiver) {
		_receiver = receiver;
	}

	void postEvent(IThreadEvent * threadEvent) {
		LOG_DEBUG("postEvent");
		QCoreApplication::postEvent(_receiver, new QtThreadEvent(threadEvent));
	}

private:

	QObject * _receiver;
};

Presentation::Presentation(Model & model)
	: QPushButton("Hello world!") {

	//Filter for post event (=thread event)
	ThreadEventFilter * threadEventFilter = new ThreadEventFilter();
	QCoreApplication::instance()->installEventFilter(threadEventFilter);

	QtPostEvent * postEvent = new QtPostEvent(this);
	model.messageSentEvent.connect(this, boost::bind(&Presentation::messageSentEventHandler, this), postEvent);
	//model.destroyPresentationEvent.connect(this, boost::bind(&Presentation::destroyEventHandler, this), postEvent);
	model.messageSentEvent2.connect(this, boost::bind(&Presentation::messageSentEventHandler2, this, _1), postEvent);
	/*model.messageSentEvent3.connect(boost::bind(&Presentation::messageSentEventHandler3, this, _1), postEvent);
	model.messageSentEvent3.connect(boost::bind(&Presentation::messageSentEventHandler3, this, _1), postEvent);
	model.messageSentEvent3.connect(boost::bind(&Presentation::messageSentEventHandler3, this, _1), postEvent);
	model.messageSentEvent3.connect(boost::bind(&Presentation::messageSentEventHandler3, this, _1), postEvent);*/
}

void Presentation::messageSentEventHandler() {
	LOG_DEBUG("messageSentEventHandler");
	static int i = 0;
	setText("event posted " + QString::number(i++));

	QMainWindow * window = new QMainWindow(this);
	window->show();
}

void Presentation::messageSentEventHandler2(const std::string & message) const {
	LOG_DEBUG(message);
}

void Presentation::messageSentEventHandler3(const Message * message) const {
	LOG_DEBUG(message->toString());
}

void Presentation::destroyEventHandler() {
	LOG_DEBUG("QObject::deleteLater()");
	deleteLater();
}

int main(int argc, char * argv[]) {
	Model model;
	model.start();

	QApplication app(argc, argv);

	Presentation presentation(model);
	presentation.resize(100, 30);
	presentation.show();

	model.setPresentation(&presentation);

	return app.exec();
}
