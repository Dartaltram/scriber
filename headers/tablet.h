#ifndef TABLET_H
#define TABLET_H

#include <iostream>
#include <vector>
#include <map>
#include <streambuf>
#include <ostream>
#include <fstream>

#include <chrono>

#include <filesystem>   //need compilation in c++17

#include <jconfigloader/jconfigloader.h>

#include "tablet_defaultParam.h"

#include "tablet_base.h"
#include "tablet_utils.h"
#include "tablet_filter.h"


/* TERMINAL LOGGER v1.2
 *
 * Creation date: 15-05-2023 - Last update: 13-06-2023
 * Author: Tobia Cancelliere
 * 
 * This header library offers logging functionality both on the terminal and on txt file.
 * Including this header in your program will offer logging functions as well as re-rout
 *  all cout calls through its own callback and showing them as TL entries.
 * 
 * The tl_settings.json file contains several parameters giving the logger its base values
 *  when the program is loaded. If the file is not present, the logger will provide one
 *  of its own with standard values for their parameters.
 * 
 * On-Terminal and On-File logging can be enabled separately via enable flag, just as
 *  the single log levels can be enabled separately via the bitwise filter
 *  The bitwise filters (on-terminal and on-file) are two 1-byte variables allowing or
 *   blocking the logging of a particular level on terminal and/or on file.
 *   Each bit corresponds to a logging level. If the nth bit (starting from lsb) is
 *    set to 1, the nth log level is going to be shown on the log.
 * 
 * On-Terminal logs are coloured and labled for better readability, the colours being
 *  determined by the tl_settings.json file
 * 
 * The On-File logs are saved on a file named "<TO BE DETERMINED>.txt". Its path is set
 *  by the tl_settings.json file as well
 */


// --- LOG EXAMPLES --- //
// [2023-05-15 10:08:57.280] (0) NONE     | Cout
// [2023-05-15 10:09:07.003] (0) NONE     | Normal
// [2023-05-15 10:11:34.237] (4) ERROR    | Error
// [2023-05-15 10:31:66.458] (3) WARNING  | Warning
// [2023-05-15 10:12:04.079] (2) INFO     | Info
// [2023-05-15 10:11:59.351] (1) DEBUG    | Debug

using namespace std;

namespace scriber {

    //callback landing for the cout function - each time << is called, the callback is called as well
    void cb_log(const char* charPtr, streamsize count);

    class ScribeLogger : public std::basic_streambuf<char, char_traits<char>>
    {
        typedef void(*pfncb)(const char*, std::streamsize _count);


        // --- DATA --- //
        private:
            static ScribeLogger* instance;

            std::basic_ostream<char, char_traits<char>> & m_stream;     //output stream
            std::streambuf * m_buffer;                                  //stream buffer
            pfncb m_callback;                                           //logging callback

            //parameters
            jconfigloader::parameterObj* m_data;    //parameter file

            //terminal
            bool m_logTerminal_enabled = true;      //flag enabling live log in terminal
            filter m_logTerminal_filter;            //bit-wise filter for logs on terminal

            //file
            string m_logPath = "";                  //saved log path
            bool m_logFile_enabled = false;         //flag enabling log-to-file
            filter m_logFile_filter;                //bit-wise filter for logs on file

            LogLevel m_logLvl = LogLevel::NONE;     //current log level - remains at NONE unless actively logging
            bool m_override = false;                //to make tests sometimes it is needed for everything to be off BUT for some comments

            //returns true if path exists (except for file name)
            bool pathExists(string pathString) {

                if (pathString.find('/') == std::string::npos)
                    return true;

                std::filesystem::path pathObj(pathString.substr(0, pathString.find_last_of('/')));
                return std::filesystem::exists(pathObj);
            }


        // --- LOGGING FUNCTIONS --- //
        public:

            //logs in the chosen file
            void logToFile(string log);

            //logs directly on terminal
            void logToTerminal(string log);



        // --- ACCESSORS --- //
        public:
            // returns current log level - always LogLevel::NONE apart from when logging
            LogLevel* logLevel() { return &m_logLvl; }

            // return status of logging-on-terminal flag
            bool isLogTerminalEnabled() { return m_logTerminal_enabled; }
            // return status of specific log-on-terminal level
            bool isLogTerminalEnabled_level(LogLevel level) { return m_logTerminal_filter.get_level(level); }
            // returns status of all log-on-terminal levels
            std::vector<bool> logTerminalFilter_status() { return m_logTerminal_filter.get_all(); }

            // return status of logging-on-file flag
            bool isLogFileEnabled() { return m_logFile_enabled; }
            // return status of specific log-on-file level
            bool isLogFileEnabled_level(LogLevel level) { return m_logFile_filter.get_level(level); }
            // returns status of all log-on-file levels
            std::vector<bool> logFilter_status() { return m_logFile_filter.get_all(); }

            //returns saved path of file
            string logPath() { return m_logPath; }
            bool override() { return m_override; }
            
        // --- SETTERS --- //
        public:
            void setLogLevel(LogLevel level) { m_logLvl = level; }

            // set log-to-terminal flag on or off
            void logToTerminal_setEnable(bool logEnabled) { m_logTerminal_enabled = logEnabled; }
            // toggle log-to-terminal
            void logToTerminal_toggle() { logToTerminal_setEnable(!m_logTerminal_enabled); }
            // set log-to-terminal level to a specified value
            void logToTerminal_setLevel(LogLevel level, bool value) { m_logTerminal_filter.set_level(level, value); }
            // set ALL log-to-terminal levels to a specified value
            void logToTerminal_setLevel_all(bool value) { m_logTerminal_filter.set_all(value); }

            // set log-to-file flag on or off
            void logToFile_setEnable(bool logEnabled) { m_logFile_enabled = logEnabled && pathExists(m_logPath); }
            // toggle log-to-file
            void logToFile_toggle() { logToFile_setEnable(!m_logFile_enabled); }
            // set log-to-file level to a specified value
            void logToFile_setLevel(LogLevel level, bool value) { m_logFile_filter.set_level(level, value); }
            // set ALL log-to-file levels to a specified value
            void logToFile_setLevel_all(bool value) { m_logFile_filter.set_all(value); }
            
            //sets new path
            string logPath(string newPath) { if (pathExists(newPath)) {m_logPath = newPath;} return m_logPath; }
            bool override(bool override) { m_override = override; return m_override;}


        // --- PARAMETER FILE --- //
        private:
            //loads parameters from file, creating a new one if needed
            void loadParameters();


        // --- SINGLETON --- //
        public:
            //get instance of the scriber
            static ScribeLogger* getInstance() {

                if (!instance)
                    instance = new scriber::ScribeLogger();

                return instance;
            }


            ScribeLogger(const ScribeLogger&) = delete;
            ScribeLogger& operator=(const ScribeLogger&) = delete;

        // --- CONSTRUCTORS --- //
        private:
            ScribeLogger(std::ostream& os = std::cout, pfncb callback = nullptr) : m_stream(os) {

                if (callback == nullptr)
                    m_callback = cb_log;

                m_buffer = m_stream.rdbuf(this);    //redirect stream

                loadParameters();
            }

        public:
            ~ScribeLogger() {
                m_stream.rdbuf(m_buffer);   //restore stream
            }

            //override xsputn and make it forward data to callback function
            std::streamsize xsputn(const char* _ptr, std::streamsize _count) { //may not be declared outside the class
                m_callback(_ptr, _count);
                return _count;
            }

            //overrride overflow and make it forward data to callback function
            typename char_traits<char>::int_type overflow(typename char_traits<char>::int_type v) {
                char charElement = char_traits<char>::to_char_type(v);
                m_callback(&charElement, 1);

                return char_traits<char>::not_eof(v);
            }
    };

    ScribeLogger* tablet();

} //namespace scriber



#endif //TABLET_H