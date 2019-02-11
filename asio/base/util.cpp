#include "util.h"
#include "desc.h"
#include <direct.h>
#include "boost/date_time.hpp"

static const boost::posix_time::ptime EPOCH(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
static int s_enableCompressSize = 1024;

void failureWriter(const char *data, int size) {
    LOG(ERROR) << std::string(data, size);
}

namespace util {

void initlog(std::string path, bool console) {
    google::InstallFailureWriter(failureWriter);
    const std::string fileNamePrefix = "log";
    std::replace(path.begin(), path.end(), '\\', '/');
    google::InitGoogleLogging(path.c_str());
    std::string dir = path.substr(0, path.find_last_of('/') + 1) + "log/";
    _mkdir(dir.c_str());
    google::SetLogDestination(google::GLOG_INFO, (dir + fileNamePrefix).c_str());
    FLAGS_alsologtostderr = console;
}

void initEnableCompressSize(int size) {
    s_enableCompressSize = size;
}

int enableCompressSize() {
    return s_enableCompressSize;
}

time_t currentMillisecond() {
    return (boost::posix_time::microsec_clock::universal_time() - EPOCH).total_milliseconds();
}

std::string getFormatTime(time_t t) {
    if (t < 0) {
        return "";
    }

    if (t > 1528959332000) {
        t /= 1000;
    }
    struct tm *local;
#ifdef WIN32
    struct tm tmpLocal;
    localtime_s(&tmpLocal, &t);
    local = &tmpLocal;
#else
    local = localtime_r(&t);
#endif
    char timeChars[50];
    strftime(timeChars, 50, "%Y-%m-%d %H:%M:%S", local);
    return std::string(timeChars);
}

}  // namespace util
