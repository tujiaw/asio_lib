#include "Connector.h"
#include <QtWidgets/QApplication>
#include "asio/base/util.h"
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>

using namespace boost::asio;

int main(int argc, char *argv[])
{
	util::initlog(argv[0], true);
    //util::initEnableCompressSize(-1);
	QApplication a(argc, argv);
	Connector w;
	w.show();
	return a.exec();
}
