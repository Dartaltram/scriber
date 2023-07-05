#include "../headers/tablet.h"

// --- DECLARATIONS --- //
scriber::ScribeLogger* scriber::ScribeLogger::instance = nullptr;
scriber::ScribeLogger* scriber::tablet() { return scriber::ScribeLogger::getInstance(); }


// --- LOGGING FUNCTIONS --- //

// better logging functions (log, logError, logWarning, logInfo, logDebug)
void scriber::log(string msg, LogLevel msgLevel, bool override) {


    scriber::tablet()->setLogLevel(msgLevel);
    scriber::tablet()->override(override);

    std::cout << msg << endl;

    scriber::tablet()->override(false);
    scriber::tablet()->setLogLevel(LogLevel::NONE);
}
void scriber::log(string msg, bool override, LogLevel msgLevel) { log(msg, msgLevel, override); }
void scriber::logError(string errorStr, bool override) { log(errorStr, LogLevel::ERROR, override); }
void scriber::logWarning(string warningStr, bool override) { log(warningStr, LogLevel::WARNING, override); }
void scriber::logInfo(string infoStr, bool override) { log(infoStr, LogLevel::INFO, override); }
void scriber::logDebug(string debugStr, bool override) { log(debugStr, LogLevel::DEBUG, override); }

//logs in the chosen file
void scriber::ScribeLogger::logToFile(string log) {

    // printf("logToFile(string log) : \n");
    // printf("\t- override: %s\n", (m_override ? "TRUE" : "FALSE"));
    // printf("\t- logFile_enabled: %s\n", (m_logFile_enabled ? "TRUE" : "FALSE"));
    // printf("\t- logFileEnabled_level(%s): %s\n", s_level[m_logLvl].c_str(), (isLogFileEnabled_level(m_logLvl) ? "TRUE" : "FALSE"));
    // printf("RESULT: %s\n", (!m_override && (!m_logFile_enabled || !isLogFileEnabled_level(m_logLvl))) ? "return" : "logging");

    if (!m_override && (!m_logFile_enabled || !isLogFileEnabled_level(m_logLvl)))
        return;

    ofstream logFile;
    logFile.open(m_logPath.c_str(), ios_base::app);
    logFile << log;
    logFile.close();
}

void scriber::ScribeLogger::logToTerminal(string log) {

    if (!m_override && (!m_logTerminal_enabled || !isLogTerminalEnabled_level(m_logLvl)))
        return;

    switch (m_logLvl) {

        case LogLevel::ERROR: { printf("\033[31m"); break; }    //red
        case LogLevel::WARNING: { printf("\033[33m"); break; }
        case LogLevel::INFO: { printf("\033[32m"); break; }
        case LogLevel::DEBUG: { printf("\033[36m"); break; }
        
        default:
            break;
    }
    printf("%s", log.c_str());
    printf("\033[0m");  //reset colour to current default
}


void scriber::ScribeLogger::loadParameters() {

    if (m_data != nullptr) {
        logWarning("scriber::loadParameters: data already present will be ovewritten");
        delete m_data;
        m_data = nullptr;
    }

    size_t length = sizeof(defaultLoggerParam) / sizeof(defaultLoggerParam[0]);
    std::string filePath = _utils::getPrgPath() + "config_" + _utils::getPrgName() + "/scriber_init.json";
    m_data = new jconfigloader::parameterObj(filePath, defaultLoggerParam, length);

    m_logTerminal_enabled = m_data->get_value<bool>({"Terminal-logging", "Enable"});

    m_logTerminal_filter.set_level( LogLevel::NONE, m_data->get_value<bool>({"Terminal-logging", "enable none_level"}) );
    m_logTerminal_filter.set_level( LogLevel::DEBUG, m_data->get_value<bool>({"Terminal-logging", "enable debug_level"}) );
    m_logTerminal_filter.set_level( LogLevel::INFO, m_data->get_value<bool>({"Terminal-logging", "enable info_level"}) );
    m_logTerminal_filter.set_level( LogLevel::WARNING, m_data->get_value<bool>({"Terminal-logging", "enable warning_level"}) );
    m_logTerminal_filter.set_level( LogLevel::ERROR, m_data->get_value<bool>({"Terminal-logging", "enable error_level"}) );


    m_logPath = scriber::_utils::getSavePath(
        m_data->get_value<string>({"File-logging", "path"}), 
        m_data->get_value<string>({"File-logging", "fileName"})
    );

    m_logFile_enabled = m_data->get_value<bool>({"File-logging", "Enable"});

    m_logFile_filter.set_level( LogLevel::NONE, m_data->get_value<bool>({"File-logging", "enable none_level"}) );
    m_logFile_filter.set_level( LogLevel::DEBUG, m_data->get_value<bool>({"File-logging", "enable debug_level"}) );
    m_logFile_filter.set_level( LogLevel::INFO, m_data->get_value<bool>({"File-logging", "enable info_level"}) );
    m_logFile_filter.set_level( LogLevel::WARNING, m_data->get_value<bool>({"File-logging", "enable warning_level"}) );
    m_logFile_filter.set_level( LogLevel::ERROR, m_data->get_value<bool>({"File-logging", "enable error_level"}) );
}




// --- CALLBACK --- //
// called each time a std::cout operator << is called instead of the normal function
void scriber::cb_log(const char* charPtr, streamsize count) {

    // if it's an empty log -> bail out
    if (*charPtr == '\n' || *charPtr == '\0') {
        return;
    }

    // build log level as string
    string lvl_str = 
        "(" + to_string( int(*scriber::tablet()->logLevel()) ) + ") " +
        s_level[*scriber::tablet()->logLevel()];
        
    string lvl_padding = "";
    for (size_t i = 0; i <= 12 - lvl_str.length(); i++) { lvl_padding += " "; }
    

    std::string msg = 
        "[" + _utils::getLogDate() + "] " +
        lvl_str + lvl_padding + "| " + charPtr + "\n";

    scriber::tablet()->logToTerminal(msg);
    scriber::tablet()->logToFile(msg);
}

