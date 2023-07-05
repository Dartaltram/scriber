#include "../headers/tablet_utils.h"


std::string scriber::_utils::getTimestampDate(std::chrono::time_point<std::chrono::system_clock> now) {
            
    //get the current time
    // auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::tm* timeInfo = std::localtime(&time);

    //set it in the correct format
    char buffer[24];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d", timeInfo);

    std::string timestamp_date = buffer;

    return timestamp_date;
}
std::string scriber::_utils::getTimestampTime(std::chrono::time_point<std::chrono::system_clock> now) {

            
    //get the current time
    // auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::tm* timeInfo = std::localtime(&time);

    //set it in the correct format
    char buffer[24];
    std::strftime(buffer, sizeof(buffer), "%H%M%S", timeInfo);

    std::string timestamp_time = buffer;

    return timestamp_time;
}
std::string scriber::_utils::getTimestampMs(std::chrono::time_point<std::chrono::system_clock> now) {
            
    //get the current time
    // auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

    std::string timestamp_ms;
    timestamp_ms.append( (ms.count()%1000<100 ? "0" : "") );
    timestamp_ms.append( (ms.count()%1000<10 ? "0" : "") );
    timestamp_ms.append( std::to_string(ms.count()%1000) );

    return timestamp_ms;
}

std::string scriber::_utils::getLogDate() {

    //get the current time
    auto now = std::chrono::system_clock::now();

    std::string date = getTimestampDate(now);
    date.insert(4, "-");
    date.insert(7, "-");

    std::string time = getTimestampTime(now);
    time.insert(2, ":");
    time.insert(5, ":");

    std::string ms = getTimestampMs(now);

    std::string dateTime = date + " " + time + "." + ms;

    return dateTime;
}

std::string scriber::_utils::getPrgName() {

    std::string prgName = program_invocation_name;

    prgName = prgName.substr(prgName.find_last_of('/') + 1, prgName.size());
    prgName = prgName.substr(0, prgName.find_last_of('.'));

    return prgName;
}

std::string scriber::_utils::getPrgPath() {
    
    char c_path[ PATH_MAX ];
    ssize_t count = readlink("/proc/self/exe", c_path, PATH_MAX);
    std::string filePath(c_path, (count > 0) ? count : 0);
    filePath = filePath.substr(0, filePath.find_last_of("/\\") + 1);

    return filePath;
}

std::string scriber::_utils::getSavePath(std::string default_path, std::string default_name) {

    if (!default_path.empty() && default_path.back() != '/')
        default_path.append("/");

    if (default_name.empty())
        default_name = scriber::_utils::getPrgName();

    auto now = std::chrono::system_clock::now();
    
    std::string result_path;
    result_path = 
        default_path + 
        default_name + 
        "_" + 
        scriber::_utils::getTimestampDate(now) + 
        "_" +
        scriber::_utils::getTimestampTime(now) + 
        ".txt"
    ;

    return result_path;
}