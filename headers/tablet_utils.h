#ifndef TABLET_UTILS_H
#define TABLET_UTILS_H

#include "tablet_base.h"

#include <iostream>
#include <chrono>

namespace scriber {

    namespace _utils {



        std::string getTimestampDate(std::chrono::time_point<std::chrono::system_clock> now);
        std::string getTimestampTime(std::chrono::time_point<std::chrono::system_clock> now);
        std::string getTimestampMs(std::chrono::time_point<std::chrono::system_clock> now);

        std::string getLogDate();

        std::string getPrgName();

        std::string getSavePath(std::string default_path, std::string default_name);

    }
}

#endif // TABLET_UTILS_H