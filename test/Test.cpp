//COMPILER: g++ -std=c++17 -o test_result ../headers/tablet_base.h ../src/tablet_utils.cpp ../src/tablet_filter.cpp ../src/tablet.cpp Test.cpp -lgtest -lgtest_main -pthread -ljconfigloader

#include <iostream>
#include <regex>
#include <string>
#include <chrono>
#include <map>
#include "../headers/tablet.h"

#include <gtest/gtest.h>

#define FILE_NAME "log_test.txt"

std::map<std::string, std::string> days_per_month = {
    {"01", "31"},
    {"02", "28"},
    {"03", "31"},
    {"04", "30"},
    {"05", "31"},
    {"06", "30"},
    {"07", "31"},
    {"08", "31"},
    {"09", "30"},
    {"10", "31"},
    {"11", "30"},
    {"12", "31"},
};

std::string getLastLine(std::string fileName) {

    //open file and get last line
    std::ifstream file(FILE_NAME);
    std::string lastLine;

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            lastLine = line;
        }

        file.close();
    }

    return lastLine;
}

//CONSTRUCTORS AND INSTANCE
TEST(SingletonTest, instance) {

    scriber::ScribeLogger* logger_1 = scriber::tablet();
    scriber::ScribeLogger* logger_2 = scriber::ScribeLogger::getInstance();

    ASSERT_EQ(logger_1, logger_2);
}
TEST(SingletonTest, value) {

    scriber::ScribeLogger* logger_1 = scriber::tablet();
    scriber::ScribeLogger* logger_2 = scriber::ScribeLogger::getInstance();

    logger_1->setLogLevel(scriber::LogLevel::ERROR);

    ASSERT_EQ(logger_1->logLevel(), logger_2->logLevel());

    logger_1->setLogLevel(scriber::LogLevel::NONE);
}

//TERMINAL LOG
TEST(TerminalTest, log_timestamp) {

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::log("TerminalTest, log_timestamp");

    std::string out = testing::internal::GetCapturedStdout();

    std::regex pattern(R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2}).(\d{3})\] \((\d+)\) (\w+) *\| (\w+))");
    std::smatch matches;
    
    bool validLog = std::regex_search(out, matches, pattern);
    ASSERT_TRUE(validLog);

    //extract data in string form
    std::string d_year = matches[1];
    std::string d_month = matches[2];
    std::string d_day = matches[3];
    std::string t_hour = matches[4];
    std::string t_min = matches[5];
    std::string t_sec = matches[6];
    
    auto now = std::chrono::system_clock::now();
    std::string date = scriber::_utils::getTimestampDate(now);
    std::string time = scriber::_utils::getTimestampTime(now);

    //check year
    if(date.substr(4,2) == "12" && date.substr(6,2) == "31" && time.substr(0,2) == "23" && time.substr(2,2) == "59" && time.substr(4,2) == "59" && d_year != date.substr(0,4)) {
        EXPECT_EQ(d_year, to_string( stoi( date.substr(0,4) ) +1 ));
    }
    else EXPECT_EQ(d_year, date.substr(0,4));

    //check month
    if((date.substr(6,2) == days_per_month[date.substr(4,2)] || date.substr(4,2) == "02" && date.substr(6,2) == "29") && time.substr(0,2) == "23" && time.substr(2,2) == "59" && time.substr(4,2) == "59" && d_month != date.substr(4,2)) {
        if (date.substr(4,2) == "12")
            EXPECT_EQ(d_month, "01");
        else
            EXPECT_EQ(d_month, (stoi(date.substr(4,2)) > 9 ? "" : "0") + to_string( stoi( date.substr(4,2) ) +1 ));
    }
    else EXPECT_EQ(d_month, date.substr(4,2));

    //check day
    if(time.substr(0,2) == "23" && time.substr(2,2) == "59" && time.substr(4,2) == "59" && d_day != date.substr(6,2)) {
        if (date.substr(6,2) == days_per_month[date.substr(4,2)] || date.substr(4,2) == "02" && date.substr(6,2) == "29")
            EXPECT_EQ(d_day, "01");
        else
            EXPECT_EQ(d_day, (stoi(date.substr(6,2)) > 9 ? "" : "0") + to_string( stoi( date.substr(6,2) ) +1 ));
    }
    else EXPECT_EQ(d_day, date.substr(6,2));

    //check hour
    if(time.substr(2,2) == "59" && time.substr(4,2) == "59" && t_hour != time.substr(0,2)) {
        if (time.substr(0,2) == "23")
            EXPECT_EQ(t_hour, "00");
        else
            EXPECT_EQ(t_hour, (stoi(time.substr(0,2)) > 9 ? "" : "0") + to_string( stoi( time.substr(0,2) ) +1 ));
    }
    else EXPECT_EQ(t_hour, time.substr(0,2));

    //check minute
    if(time.substr(4,2) == "59" && t_min != time.substr(2,2)) {
        if (time.substr(2,2) == "59")
            EXPECT_EQ(t_min, "00");
        else
            EXPECT_EQ(t_min, (stoi(time.substr(2,2)) > 9 ? "" : "0") + to_string( stoi( time.substr(2,2) ) +1 ));
    }
    else EXPECT_EQ(t_min, time.substr(2,2));

    //check second
    if (t_sec != time.substr(4,2)) {
        if (time.substr(4,2) == "59")
            EXPECT_EQ(t_sec, "00");
        else
            EXPECT_EQ(t_sec, (stoi(time.substr(4,2)) > 9 ? "" : "0") + to_string( stoi( time.substr(4,2) ) +1 ));
    }
    else EXPECT_EQ(t_sec, time.substr(4,2));

}
TEST(TerminalTest, logCOUT_lvl) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::NONE, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::NONE) );

    //capture log from std out
    testing::internal::CaptureStdout();

        std::cout << "TerminalTest, logCOUT_lvl" << endl;

    std::string out = testing::internal::GetCapturedStdout();

    std::regex pattern(R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2}).(\d{3})\] \((\d+)\) (\w+) *\| (\w+))");
    std::smatch matches;
    
    bool validLog = std::regex_search(out, matches, pattern);
    ASSERT_TRUE(validLog);

    //extract data in string form
    std::string lvlNum = matches[8];
    std::string status = matches[9];

    EXPECT_EQ(lvlNum, to_string( int(scriber::LogLevel::NONE) ));
    EXPECT_EQ(status, scriber::s_level[scriber::LogLevel::NONE]);
}
TEST(TerminalTest, logNONE_lvl) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::NONE, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::NONE) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::log("TerminalTest, logNONE_lvl");

    std::string out = testing::internal::GetCapturedStdout();

    std::regex pattern(R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2}).(\d{3})\] \((\d+)\) (\w+) *\| (\w+))");
    std::smatch matches;
    
    bool validLog = std::regex_search(out, matches, pattern);
    ASSERT_TRUE(validLog);

    //extract data in string form
    std::string lvlNum = matches[8];
    std::string status = matches[9];

    EXPECT_EQ(lvlNum, to_string( int(scriber::LogLevel::NONE) ));
    EXPECT_EQ(status, scriber::s_level[scriber::LogLevel::NONE]);
}
TEST(TerminalTest, logDEBUG_lvl) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::DEBUG, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::DEBUG) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logDebug("TerminalTest, logDEBUG_lvl");

    std::string out = testing::internal::GetCapturedStdout();


    std::regex pattern(R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2}).(\d{3})\] \((\d+)\) (\w+) *\| (\w+))");
    std::smatch matches;
    
    bool validLog = std::regex_search(out, matches, pattern);
    ASSERT_TRUE(validLog);

    //extract data in string form
    std::string lvlNum = matches[8];
    std::string status = matches[9];

    EXPECT_EQ(lvlNum, to_string( int(scriber::LogLevel::DEBUG) ));
    EXPECT_EQ(status, scriber::s_level[scriber::LogLevel::DEBUG]);
}
TEST(TerminalTest, logINFO_lvl) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::INFO, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::INFO) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logInfo("TerminalTest, logINFO_lvl");

    std::string out = testing::internal::GetCapturedStdout();

    std::regex pattern(R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2}).(\d{3})\] \((\d+)\) (\w+) *\| (\w+))");
    std::smatch matches;
    
    bool validLog = std::regex_search(out, matches, pattern);
    ASSERT_TRUE(validLog);

    //extract data in string form
    std::string lvlNum = matches[8];
    std::string status = matches[9];

    EXPECT_EQ(lvlNum, to_string( int(scriber::LogLevel::INFO) ));
    EXPECT_EQ(status, scriber::s_level[scriber::LogLevel::INFO]);
}
TEST(TerminalTest, logWARNING_lvl) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::WARNING, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::WARNING) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logWarning("TerminalTest, logWARNING_lvl");

    std::string out = testing::internal::GetCapturedStdout();

    std::regex pattern(R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2}).(\d{3})\] \((\d+)\) (\w+) *\| (\w+))");
    std::smatch matches;
    
    bool validLog = std::regex_search(out, matches, pattern);
    ASSERT_TRUE(validLog);

    //extract data in string form
    std::string lvlNum = matches[8];
    std::string status = matches[9];

    EXPECT_EQ(lvlNum, to_string( int(scriber::LogLevel::WARNING) ));
    EXPECT_EQ(status, scriber::s_level[scriber::LogLevel::WARNING]);
}
TEST(TerminalTest, logERROR_lvl) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::ERROR, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::ERROR) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logError("TerminalTest, logERROR_lvl");

    std::string out = testing::internal::GetCapturedStdout();

    std::regex pattern(R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2}).(\d{3})\] \((\d+)\) (\w+) *\| (\w+))");
    std::smatch matches;
    
    bool validLog = std::regex_search(out, matches, pattern);
    ASSERT_TRUE(validLog);

    //extract data in string form
    std::string lvlNum = matches[8];
    std::string status = matches[9];

    EXPECT_EQ(lvlNum, to_string( int(scriber::LogLevel::ERROR) ));
    EXPECT_EQ(status, scriber::s_level[scriber::LogLevel::ERROR]);
}

//TERMINAL FILTERS
TEST(TerminalFilterTest, enable) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::log("TerminalFilterTest, enable_1");

    std::string out_1 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(!out_1.empty());


    scriber::tablet()->logToTerminal_setEnable(false);
    ASSERT_FALSE( scriber::tablet()->isLogTerminalEnabled() );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::log("TerminalFilterTest, enable_2");

    std::string out_2 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(out_2.empty());


    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::log("TerminalFilterTest, enable_3", true);

    std::string out_3 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(!out_3.empty());
}
TEST(TerminalFilterTest, filter_lvlNONE) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::NONE, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::NONE) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::log("TerminalFilterTest, filter_lvlNONE_1");

    std::string out_1 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(!out_1.empty());


    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::NONE, false);
    ASSERT_FALSE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::NONE) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::log("TerminalFilterTest, filter_lvlNONE_2");

    std::string out_2 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(out_2.empty());


    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::log("TerminalFilterTest, filter_lvlNONE_3", true);

    std::string out_3 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(!out_3.empty());
}
TEST(TerminalFilterTest, filter_lvlDEBUG) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::DEBUG, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::DEBUG) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logDebug("TerminalFilterTest, filter_lvlDEBUG_1");

    std::string out_1 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(!out_1.empty());


    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::DEBUG, false);
    ASSERT_FALSE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::DEBUG) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logDebug("TerminalFilterTest, filter_lvlDEBUG_2");

    std::string out_2 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(out_2.empty());


    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logDebug("TerminalFilterTest, filter_lvlDEBUG_3", true);

    std::string out_3 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(!out_3.empty());
}
TEST(TerminalFilterTest, filter_lvlINFO) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::INFO, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::INFO) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logInfo("TerminalFilterTest, filter_lvlINFO_1");

    std::string out_1 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(!out_1.empty());


    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::INFO, false);
    ASSERT_FALSE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::INFO) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logInfo("TerminalFilterTest, filter_lvlINFO_2");

    std::string out_2 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(out_2.empty());


    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logInfo("TerminalFilterTest, filter_lvlINFO_3", true);

    std::string out_3 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(!out_3.empty());
}
TEST(TerminalFilterTest, filter_lvlWARNING) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::WARNING, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::WARNING) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logWarning("TerminalFilterTest, filter_lvlWARNING_1");

    std::string out_1 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(!out_1.empty());


    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::WARNING, false);
    ASSERT_FALSE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::WARNING) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logWarning("TerminalFilterTest, filter_lvlWARNING_2");

    std::string out_2 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(out_2.empty());


    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logWarning("TerminalFilterTest, filter_lvlWARNING_3", true);

    std::string out_3 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(!out_3.empty());
}
TEST(TerminalFilterTest, filter_lvlERROR) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::ERROR, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::ERROR) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logError("TerminalFilterTest, filter_lvlERROR_1");

    std::string out_1 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(!out_1.empty());


    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::ERROR, false);
    ASSERT_FALSE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::ERROR) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logError("TerminalFilterTest, filter_lvlERROR_2");

    std::string out_2 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(out_2.empty());


    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logError("TerminalFilterTest, filter_lvlERROR_3", true);

    std::string out_3 = testing::internal::GetCapturedStdout();
    
    ASSERT_TRUE(!out_3.empty());
}
TEST(TerminalFilterTest, filter_ALL) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel_all(true);

    EXPECT_TRUE(scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::NONE));
    EXPECT_TRUE(scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::DEBUG));
    EXPECT_TRUE(scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::INFO));
    EXPECT_TRUE(scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::WARNING));
    EXPECT_TRUE(scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::ERROR));

    scriber::tablet()->logToTerminal_setLevel_all(false);

    EXPECT_FALSE(scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::NONE));
    EXPECT_FALSE(scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::DEBUG));
    EXPECT_FALSE(scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::INFO));
    EXPECT_FALSE(scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::WARNING));
    EXPECT_FALSE(scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::ERROR));
}

//FILE PATH
TEST(FilePathTest, pathChange) {

    scriber::tablet()->logPath("MyFile.txt");
    std::string path_1 = scriber::tablet()->logPath();

    scriber::tablet()->logPath(FILE_NAME);
    std::string path_2 = scriber::tablet()->logPath();

    ASSERT_FALSE(path_1 == path_2);
}

//FILE LOG
TEST(FileTest, log_timestamp) {

    scriber::tablet()->logToFile_setEnable(true);
    scriber::tablet()->logToFile_setLevel_all(true);

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::log("FileTest, log_timestamp");

    std::string out = testing::internal::GetCapturedStdout();

    std::string lastLine = getLastLine(FILE_NAME);

    std::regex pattern(R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2}).(\d{3})\] \((\d+)\) (\w+) *\| (\w+))");
    std::smatch matches;
    
    bool validLog = std::regex_search(lastLine, matches, pattern);
    ASSERT_TRUE(validLog);

    //extract data in string form
    std::string d_year = matches[1];
    std::string d_month = matches[2];
    std::string d_day = matches[3];
    std::string t_hour = matches[4];
    std::string t_min = matches[5];
    std::string t_sec = matches[6];
    
    auto now = std::chrono::system_clock::now();
    std::string date = scriber::_utils::getTimestampDate(now);
    std::string time = scriber::_utils::getTimestampTime(now);

    //check year
    if(date.substr(4,2) == "12" && date.substr(6,2) == "31" && time.substr(0,2) == "23" && time.substr(2,2) == "59" && time.substr(4,2) == "59" && d_year != date.substr(0,4)) {
        EXPECT_EQ(d_year, to_string( stoi( date.substr(0,4) ) +1 ));
    }
    else EXPECT_EQ(d_year, date.substr(0,4));

    //check month
    if((date.substr(6,2) == days_per_month[date.substr(4,2)] || date.substr(4,2) == "02" && date.substr(6,2) == "29") && time.substr(0,2) == "23" && time.substr(2,2) == "59" && time.substr(4,2) == "59" && d_month != date.substr(4,2)) {
        if (date.substr(4,2) == "12")
            EXPECT_EQ(d_month, "01");
        else
            EXPECT_EQ(d_month, (stoi(date.substr(4,2)) > 9 ? "" : "0") + to_string( stoi( date.substr(4,2) ) +1 ));
    }
    else EXPECT_EQ(d_month, date.substr(4,2));

    //check day
    if(time.substr(0,2) == "23" && time.substr(2,2) == "59" && time.substr(4,2) == "59" && d_day != date.substr(6,2)) {
        if (date.substr(6,2) == days_per_month[date.substr(4,2)] || date.substr(4,2) == "02" && date.substr(6,2) == "29")
            EXPECT_EQ(d_day, "01");
        else
            EXPECT_EQ(d_day, (stoi(date.substr(6,2)) > 9 ? "" : "0") + to_string( stoi( date.substr(6,2) ) +1 ));
    }
    else EXPECT_EQ(d_day, date.substr(6,2));

    //check hour
    if(time.substr(2,2) == "59" && time.substr(4,2) == "59" && t_hour != time.substr(0,2)) {
        if (time.substr(0,2) == "23")
            EXPECT_EQ(t_hour, "00");
        else
            EXPECT_EQ(t_hour, (stoi(time.substr(0,2)) > 9 ? "" : "0") + to_string( stoi( time.substr(0,2) ) +1 ));
    }
    else EXPECT_EQ(t_hour, time.substr(0,2));

    //check minute
    if(time.substr(4,2) == "59" && t_min != time.substr(2,2)) {
        if (time.substr(2,2) == "59")
            EXPECT_EQ(t_min, "00");
        else
            EXPECT_EQ(t_min, (stoi(time.substr(2,2)) > 9 ? "" : "0") + to_string( stoi( time.substr(2,2) ) +1 ));
    }
    else EXPECT_EQ(t_min, time.substr(2,2));

    //check second
    if (t_sec != time.substr(4,2)) {
        if (time.substr(4,2) == "59")
            EXPECT_EQ(t_sec, "00");
        else
            EXPECT_EQ(t_sec, (stoi(time.substr(4,2)) > 9 ? "" : "0") + to_string( stoi( time.substr(4,2) ) +1 ));
    }
    else EXPECT_EQ(t_sec, time.substr(4,2));

    std::remove(FILE_NAME);
}
TEST(FileTest, logCOUT_lvl) {

    scriber::tablet()->logToFile_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled() );

    scriber::tablet()->logToFile_setLevel(scriber::LogLevel::NONE, true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::NONE) );

    //capture log from std out
    testing::internal::CaptureStdout();

        std::cout << "FileTest, logCOUT_lvl" << endl;

    std::string out = testing::internal::GetCapturedStdout();

    std::string lastLine = getLastLine(FILE_NAME);

    std::regex pattern(R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2}).(\d{3})\] \((\d+)\) (\w+) *\| (\w+))");
    std::smatch matches;
    
    bool validLog = std::regex_search(lastLine, matches, pattern);
    ASSERT_TRUE(validLog);

    //extract data in string form
    std::string lvlNum = matches[8];
    std::string status = matches[9];

    EXPECT_EQ(lvlNum, to_string( int(scriber::LogLevel::NONE) ));
    EXPECT_EQ(status, scriber::s_level[scriber::LogLevel::NONE]);

    std::remove(FILE_NAME);
}
TEST(FileTest, logNONE_lvl) {

    scriber::tablet()->logToFile_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled() );

    scriber::tablet()->logToFile_setLevel(scriber::LogLevel::NONE, true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::NONE) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::log("FileTest, logNONE_lvl");

    std::string out = testing::internal::GetCapturedStdout();

    std::string lastLine = getLastLine(FILE_NAME);

    std::regex pattern(R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2}).(\d{3})\] \((\d+)\) (\w+) *\| (\w+))");
    std::smatch matches;
    
    bool validLog = std::regex_search(lastLine, matches, pattern);
    ASSERT_TRUE(validLog);

    //extract data in string form
    std::string lvlNum = matches[8];
    std::string status = matches[9];

    EXPECT_EQ(lvlNum, to_string( int(scriber::LogLevel::NONE) ));
    EXPECT_EQ(status, scriber::s_level[scriber::LogLevel::NONE]);

    std::remove(FILE_NAME);
}
TEST(FileTest, logDEBUG_lvl) {

    scriber::tablet()->logToFile_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled() );

    scriber::tablet()->logToFile_setLevel(scriber::LogLevel::DEBUG, true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::DEBUG) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logDebug("FileTest, logDEBUG_lvl");

    std::string out = testing::internal::GetCapturedStdout();

    std::string lastLine = getLastLine(FILE_NAME);


    std::regex pattern(R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2}).(\d{3})\] \((\d+)\) (\w+) *\| (\w+))");
    std::smatch matches;
    
    bool validLog = std::regex_search(lastLine, matches, pattern);
    ASSERT_TRUE(validLog);

    //extract data in string form
    std::string lvlNum = matches[8];
    std::string status = matches[9];

    EXPECT_EQ(lvlNum, to_string( int(scriber::LogLevel::DEBUG) ));
    EXPECT_EQ(status, scriber::s_level[scriber::LogLevel::DEBUG]);

    std::remove(FILE_NAME);
}
TEST(FileTest, logINFO_lvl) {

    scriber::tablet()->logToFile_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled() );

    scriber::tablet()->logToFile_setLevel(scriber::LogLevel::INFO, true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::INFO) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logInfo("FileTest, logINFO_lvl");

    std::string out = testing::internal::GetCapturedStdout();

    std::string lastLine = getLastLine(FILE_NAME);

    std::regex pattern(R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2}).(\d{3})\] \((\d+)\) (\w+) *\| (\w+))");
    std::smatch matches;
    
    bool validLog = std::regex_search(lastLine, matches, pattern);
    ASSERT_TRUE(validLog);

    //extract data in string form
    std::string lvlNum = matches[8];
    std::string status = matches[9];

    EXPECT_EQ(lvlNum, to_string( int(scriber::LogLevel::INFO) ));
    EXPECT_EQ(status, scriber::s_level[scriber::LogLevel::INFO]);

    std::remove(FILE_NAME);
}
TEST(FileTest, logWARNING_lvl) {

    scriber::tablet()->logToFile_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled() );

    scriber::tablet()->logToFile_setLevel(scriber::LogLevel::WARNING, true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::WARNING) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logWarning("FileTest, logWARNING_lvl");

    std::string out = testing::internal::GetCapturedStdout();

    std::string lastLine = getLastLine(FILE_NAME);

    std::regex pattern(R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2}).(\d{3})\] \((\d+)\) (\w+) *\| (\w+))");
    std::smatch matches;
    
    bool validLog = std::regex_search(lastLine, matches, pattern);
    ASSERT_TRUE(validLog);

    //extract data in string form
    std::string lvlNum = matches[8];
    std::string status = matches[9];

    EXPECT_EQ(lvlNum, to_string( int(scriber::LogLevel::WARNING) ));
    EXPECT_EQ(status, scriber::s_level[scriber::LogLevel::WARNING]);

    std::remove(FILE_NAME);
}
TEST(FileTest, logERROR_lvl) {

    scriber::tablet()->logToFile_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled() );

    scriber::tablet()->logToFile_setLevel(scriber::LogLevel::ERROR, true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::ERROR) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logError("FileTest, logERROR_lvl");

    std::string out = testing::internal::GetCapturedStdout();

    std::string lastLine = getLastLine(FILE_NAME);

    std::regex pattern(R"(\[(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2}).(\d{3})\] \((\d+)\) (\w+) *\| (\w+))");
    std::smatch matches;
    
    bool validLog = std::regex_search(lastLine, matches, pattern);
    ASSERT_TRUE(validLog);

    //extract data in string form
    std::string lvlNum = matches[8];
    std::string status = matches[9];

    EXPECT_EQ(lvlNum, to_string( int(scriber::LogLevel::ERROR) ));
    EXPECT_EQ(status, scriber::s_level[scriber::LogLevel::ERROR]);

    std::remove(FILE_NAME);
}

//FILE FILTERS
TEST(FileFilterTest, enable) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::NONE, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::NONE) );

    scriber::tablet()->logToFile_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled() );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::log("FileFilterTest, enable_1");

    std::string out_1 = testing::internal::GetCapturedStdout();
    std::string file_1 = getLastLine(FILE_NAME);

    out_1 = out_1.substr(out_1.find_first_of('[') , out_1.find('\n'));
    if (out_1[3] < 48 || out_1[3] > 57) {
        out_1 = out_1.substr(1);
        out_1 = out_1.substr(out_1.find_first_of('[') , out_1.find('\n'));
    }
    ASSERT_STREQ(out_1.c_str(), file_1.c_str());


    scriber::tablet()->logToFile_setEnable(false);
    ASSERT_FALSE( scriber::tablet()->isLogFileEnabled() );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::log("FileFilterTest, enable_2");

    std::string out_2 = testing::internal::GetCapturedStdout();
    std::string file_2 = getLastLine(FILE_NAME);

    out_2 = out_2.substr(out_2.find_first_of('[') , out_2.find('\n'));
    if (out_2[3] < 48 || out_2[3] > 57) {
        out_2 = out_2.substr(1);
        out_2 = out_2.substr(out_2.find_first_of('[') , out_2.find('\n'));
    }
    ASSERT_STRNE(out_2.c_str(), file_2.c_str());


    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::log("FileFilterTest, enable_3", true);

    std::string out_3 = testing::internal::GetCapturedStdout();
    std::string file_3 = getLastLine(FILE_NAME);

    out_3 = out_3.substr(out_3.find_first_of('[') , out_3.find('\n'));
    if (out_3[3] < 48 || out_3[3] > 57) {
        out_3 = out_3.substr(1);
        out_3 = out_3.substr(out_3.find_first_of('[') , out_3.find('\n'));
    }
    ASSERT_STREQ(out_3.c_str(), file_3.c_str());

    std::remove(FILE_NAME);
}
TEST(FileFilterTest, filter_lvlNONE) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::NONE, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::NONE) );

    scriber::tablet()->logToFile_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled() );
    
    scriber::tablet()->logToFile_setLevel(scriber::LogLevel::NONE, true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::NONE) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::log("FileFilterTest, filter_lvlNONE_1");

    std::string out_1 = testing::internal::GetCapturedStdout();
    std::string file_1 = getLastLine(FILE_NAME);

    out_1 = out_1.substr(out_1.find_first_of('[') , out_1.find('\n'));
    try { stoi(out_1.substr(1,4)); }
    catch (...) {
        out_1 = out_1.substr(out_1.find_first_of('[') , out_1.find('\n'));
    }
    ASSERT_STREQ(out_1.c_str(), file_1.c_str());

    scriber::tablet()->logToFile_setLevel(scriber::LogLevel::NONE, false);
    ASSERT_FALSE( scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::NONE) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::log("FileFilterTest, filter_lvlNONE_2");

    std::string out_2 = testing::internal::GetCapturedStdout();
    std::string file_2 = getLastLine(FILE_NAME);

    out_2 = out_2.substr(out_2.find_first_of('[') , out_2.find('\n'));
    if (out_2[3] < 48 || out_2[3] > 57) {
        out_2 = out_2.substr(1);
        out_2 = out_2.substr(out_2.find_first_of('[') , out_2.find('\n'));
    }
    ASSERT_STRNE(out_2.c_str(), file_2.c_str());


    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::log("FileFilterTest, filter_lvlNONE_3", true);

    std::string out_3 = testing::internal::GetCapturedStdout();
    std::string file_3 = getLastLine(FILE_NAME);

    out_3 = out_3.substr(out_3.find_first_of('[') , out_3.find('\n'));
    if (out_3[3] < 48 || out_3[3] > 57) {
        out_3 = out_3.substr(1);
        out_3 = out_3.substr(out_3.find_first_of('[') , out_3.find('\n'));
    }
    ASSERT_STREQ(out_3.c_str(), file_3.c_str());

    std::remove(FILE_NAME);
}
TEST(FileFilterTest, filter_lvlDEBUG) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::DEBUG, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::DEBUG) );

    scriber::tablet()->logToFile_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled() );
    
    scriber::tablet()->logToFile_setLevel(scriber::LogLevel::DEBUG, true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::DEBUG) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logDebug("FileFilterTest, filter_lvlDEBUG_1");

    std::string out_1 = testing::internal::GetCapturedStdout();
    std::string file_1 = getLastLine(FILE_NAME);

    out_1 = out_1.substr(out_1.find_first_of('[') , out_1.find('\n'));
    if (out_1[3] < 48 || out_1[3] > 57) {
        out_1 = out_1.substr(1);
        out_1 = out_1.substr(out_1.find_first_of('[') , out_1.find('\n'));
    }
    ASSERT_STREQ(out_1.c_str(), (file_1.append("\n")).c_str());

    scriber::tablet()->logToFile_setLevel(scriber::LogLevel::DEBUG, false);
    ASSERT_FALSE( scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::DEBUG) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logDebug("FileFilterTest, filter_lvlDEBUG_2");

    std::string out_2 = testing::internal::GetCapturedStdout();
    std::string file_2 = getLastLine(FILE_NAME);

    out_2 = out_2.substr(out_2.find_first_of('[') , out_2.find('\n'));
    if (out_2[3] < 48 || out_2[3] > 57) {
        out_2 = out_2.substr(1);
        out_2 = out_2.substr(out_2.find_first_of('[') , out_2.find('\n'));
    }
    ASSERT_STRNE(out_2.c_str(), (file_2.append("\n")).c_str());


    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logDebug("FileFilterTest, filter_lvlDEBUG_3", true);

    std::string out_3 = testing::internal::GetCapturedStdout();
    std::string file_3 = getLastLine(FILE_NAME);

    out_3 = out_3.substr(out_3.find_first_of('[') , out_3.find('\n'));
    if (out_3[3] < 48 || out_3[3] > 57) {
        out_3 = out_3.substr(1);
        out_3 = out_3.substr(out_3.find_first_of('[') , out_3.find('\n'));
    }
    ASSERT_STREQ(out_3.c_str(), (file_3.append("\n")).c_str());

    std::remove(FILE_NAME);
}
TEST(FileFilterTest, filter_lvlINFO) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::INFO, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::INFO) );

    scriber::tablet()->logToFile_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled() );
    
    scriber::tablet()->logToFile_setLevel(scriber::LogLevel::INFO, true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::INFO) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logInfo("FileFilterTest, filter_lvlINFO_1");

    std::string out_1 = testing::internal::GetCapturedStdout();
    std::string file_1 = getLastLine(FILE_NAME);

    out_1 = out_1.substr(out_1.find_first_of('[') , out_1.find('\n'));
    if (out_1[3] < 48 || out_1[3] > 57) {
        out_1 = out_1.substr(1);
        out_1 = out_1.substr(out_1.find_first_of('[') , out_1.find('\n'));
    }
    ASSERT_STREQ(out_1.c_str(), (file_1.append("\n")).c_str());

    scriber::tablet()->logToFile_setLevel(scriber::LogLevel::INFO, false);
    ASSERT_FALSE( scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::INFO) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logInfo("FileFilterTest, filter_lvlINFO_2");

    std::string out_2 = testing::internal::GetCapturedStdout();
    std::string file_2 = getLastLine(FILE_NAME);

    out_2 = out_2.substr(out_2.find_first_of('[') , out_2.find('\n'));
    if (out_2[3] < 48 || out_2[3] > 57) {
        out_2 = out_2.substr(1);
        out_2 = out_2.substr(out_2.find_first_of('[') , out_2.find('\n'));
    }
    ASSERT_STRNE(out_2.c_str(), (file_2.append("\n")).c_str());


    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logInfo("FileFilterTest, filter_lvlINFO_3", true);

    std::string out_3 = testing::internal::GetCapturedStdout();
    std::string file_3 = getLastLine(FILE_NAME);

    out_3 = out_3.substr(out_3.find_first_of('[') , out_3.find('\n'));
    if (out_3[3] < 48 || out_3[3] > 57) {
        out_3 = out_3.substr(1);
        out_3 = out_3.substr(out_3.find_first_of('[') , out_3.find('\n'));
    }
    ASSERT_STREQ(out_3.c_str(), (file_3.append("\n")).c_str());

    std::remove(FILE_NAME);
}
TEST(FileFilterTest, filter_lvlWARNING) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::WARNING, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::WARNING) );

    scriber::tablet()->logToFile_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled() );
    
    scriber::tablet()->logToFile_setLevel(scriber::LogLevel::WARNING, true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::WARNING) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logWarning("FileFilterTest, filter_lvlWARNING_1");

    std::string out_1 = testing::internal::GetCapturedStdout();
    std::string file_1 = getLastLine(FILE_NAME);

    out_1 = out_1.substr(out_1.find_first_of('[') , out_1.find('\n'));
    if (out_1[3] < 48 || out_1[3] > 57) {
        out_1 = out_1.substr(1);
        out_1 = out_1.substr(out_1.find_first_of('[') , out_1.find('\n'));
    }
    ASSERT_STREQ(out_1.c_str(), (file_1.append("\n")).c_str());

    scriber::tablet()->logToFile_setLevel(scriber::LogLevel::WARNING, false);
    ASSERT_FALSE( scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::WARNING) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logWarning("FileFilterTest, filter_lvlWARNING_2");

    std::string out_2 = testing::internal::GetCapturedStdout();
    std::string file_2 = getLastLine(FILE_NAME);

    out_2 = out_2.substr(out_2.find_first_of('[') , out_2.find('\n'));
    if (out_2[3] < 48 || out_2[3] > 57) {
        out_2 = out_2.substr(1);
        out_2 = out_2.substr(out_2.find_first_of('[') , out_2.find('\n'));
    }
    ASSERT_STRNE(out_2.c_str(), (file_2.append("\n")).c_str());


    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logWarning("FileFilterTest, filter_lvlWARNING_3", true);

    std::string out_3 = testing::internal::GetCapturedStdout();
    std::string file_3 = getLastLine(FILE_NAME);

    out_3 = out_3.substr(out_3.find_first_of('[') , out_3.find('\n'));
    if (out_3[3] < 48 || out_3[3] > 57) {
        out_3 = out_3.substr(1);
        out_3 = out_3.substr(out_3.find_first_of('[') , out_3.find('\n'));
    }
    ASSERT_STREQ(out_3.c_str(), (file_3.append("\n")).c_str());

    std::remove(FILE_NAME);
}
TEST(FileFilterTest, filter_lvlERROR) {

    scriber::tablet()->logToTerminal_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled() );

    scriber::tablet()->logToTerminal_setLevel(scriber::LogLevel::ERROR, true);
    ASSERT_TRUE( scriber::tablet()->isLogTerminalEnabled_level(scriber::LogLevel::ERROR) );

    scriber::tablet()->logToFile_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled() );
    
    scriber::tablet()->logToFile_setLevel(scriber::LogLevel::ERROR, true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::ERROR) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logError("FileFilterTest, filter_lvlERROR_1");

    std::string out_1 = testing::internal::GetCapturedStdout();
    std::string file_1 = getLastLine(FILE_NAME);

    out_1 = out_1.substr(out_1.find_first_of('[') , out_1.find('\n'));
    if (out_1[3] < 48 || out_1[3] > 57) {
        out_1 = out_1.substr(1);
        out_1 = out_1.substr(out_1.find_first_of('[') , out_1.find('\n'));
    }
    ASSERT_STREQ(out_1.c_str(), (file_1.append("\n")).c_str());

    scriber::tablet()->logToFile_setLevel(scriber::LogLevel::ERROR, false);
    ASSERT_FALSE( scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::ERROR) );

    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logError("FileFilterTest, filter_lvlERROR_2");

    std::string out_2 = testing::internal::GetCapturedStdout();
    std::string file_2 = getLastLine(FILE_NAME);

    out_2 = out_2.substr(out_2.find_first_of('[') , out_2.find('\n'));
    if (out_2[3] < 48 || out_2[3] > 57) {
        out_2 = out_2.substr(1);
        out_2 = out_2.substr(out_2.find_first_of('[') , out_2.find('\n'));
    }
    ASSERT_STRNE(out_2.c_str(), (file_2.append("\n")).c_str());


    //capture log from std out
    testing::internal::CaptureStdout();

        scriber::logError("FileFilterTest, filter_lvlERROR_3", true);

    std::string out_3 = testing::internal::GetCapturedStdout();
    std::string file_3 = getLastLine(FILE_NAME);

    out_3 = out_3.substr(out_3.find_first_of('[') , out_3.find('\n'));
    if (out_3[3] < 48 || out_3[3] > 57) {
        out_3 = out_3.substr(1);
        out_3 = out_3.substr(out_3.find_first_of('[') , out_3.find('\n'));
    }
    ASSERT_STREQ(out_3.c_str(), (file_3.append("\n")).c_str());

    std::remove(FILE_NAME);
}
TEST(FileFilterTest, filter_ALL) {

    scriber::tablet()->logToFile_setEnable(true);
    ASSERT_TRUE( scriber::tablet()->isLogFileEnabled() );

    scriber::tablet()->logToFile_setLevel_all(true);

    EXPECT_TRUE(scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::NONE));
    EXPECT_TRUE(scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::DEBUG));
    EXPECT_TRUE(scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::INFO));
    EXPECT_TRUE(scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::WARNING));
    EXPECT_TRUE(scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::ERROR));

    scriber::tablet()->logToFile_setLevel_all(false);

    EXPECT_FALSE(scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::NONE));
    EXPECT_FALSE(scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::DEBUG));
    EXPECT_FALSE(scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::INFO));
    EXPECT_FALSE(scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::WARNING));
    EXPECT_FALSE(scriber::tablet()->isLogFileEnabled_level(scriber::LogLevel::ERROR));
}


int main(int argc, char** argv)
{    
    scriber::tablet()->logPath(FILE_NAME);

    testing::InitGoogleTest(&argc, argv);
    return  RUN_ALL_TESTS();
}
