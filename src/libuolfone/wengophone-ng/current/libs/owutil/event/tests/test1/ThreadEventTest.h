#include <event/Trackable.h>

#include <QtGui/QtGui>

#include <string>

class Model;
class Message;

class Presentation : public QPushButton, public Trackable {
	Q_OBJECT
public:

	Presentation(Model & model);

	void messageSentEventHandler();

	void messageSentEventHandler2(const std::string & message) const;

	void messageSentEventHandler3(const Message * message) const;

	void destroyEventHandler();
};
