#ifndef TABLET_BASE_H
#define TABLET_BASE_H

#include <iostream>
#include <map>

namespace scriber {

    // --- LOG LEVELS --- //
    enum LogLevel {NONE, DEBUG, INFO, WARNING, ERROR};
    static std::map<LogLevel, std::string> s_level {
        {LogLevel::NONE, "NONE"},
        {LogLevel::DEBUG, "DEBUG"},
        {LogLevel::INFO, "INFO"},
        {LogLevel::WARNING, "WARNING"},
        {LogLevel::ERROR, "ERROR"}
    };



    //better logging functions (log, logError, logWarning, logInfo, logDebug)
    void log(std::string msg, LogLevel msgLevel = LogLevel::NONE, bool override = false);
    void log(std::string msg, bool override, LogLevel msgLevel = LogLevel::NONE);
    void logError(std::string errorStr, bool override = false);
    void logWarning(std::string warningStr, bool override = false);
    void logInfo(std::string infoStr, bool override = false);
    void logDebug(std::string debugStr, bool override = false);

} //namespace scriber

#endif // TABLET_BASE_H