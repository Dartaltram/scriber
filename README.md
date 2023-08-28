# scriber
General log system on both file and terminal

## How to Install
Clone the repo on your computer and run the **INSTALL** shell script. You might need to up the script priviledges running this line before the install script.

```bash
$ git clone https://github.com/Dartaltram/scriber.git
$ cd scriber
$ chmod +x ./INSTALL.sh
$ sudo sh ./INSTALL.sh
```

The **INSTALL** script copies the library file (.a) from the build directory in /usr/local/lib and all needed headers (.h) from the headers directory to a new folder /usr/include/scriber

This can be done by hand, with no need for the INSTALL script

### Dependencies
The library depends on the **nlohmann** library for easy JSON manipulation. The other files included in the library are all part of the standard c++ fare, with the small addition that **filesystem** needs the program to be compiled in c++ 17 or later. More on that in the Use->Compiling section

This script also depends on the jconfigloader library that can be found at https://github.com/Dartaltram/jconfigloader


## How to Uninstall
To uninstall the library it is sufficient to delete the files the INSTALL script copied. To expedite the process, you can run the **UNINSTALL** shell script

## Use
To use the library you have to include the scriber/tablet.h header

### Functions and Objects
This library offers an agile method to log messages both on terminal and on a logfile. Additionally, it makes several log "levels" available to the user as to better define what must and must not be logged on either platform.

**LogLevel**: this enum class specifies the various levels available. Each of these can be disabled (and therefore not shown) on either terminal, logfile, or both.

- ERROR: to be used when something serious happened, the code should probably be stopped
- WARNING: less serious then the error, code can continue but it will likely not produce the desired result
- INFO: intended to give the reader data on the current status of the code
- DEBUG: to be used as a tool in finding coding errors and disabled in the final release of your code
- NONE: general communication

**scriber::LogX**: main function of the library, takes a log message in string form and treats it in the manner prescribed by the config file. Optional parameter is a boolean override to show the message even in case the config file says otherwise

**scriber_init.json**: config file, automatically created in a separate folder with the name config\_\<your_program_name\>. This file shows the standard settings for each log level, whether it is enabled or disabled on the terminal or on the file.

It also gives the logFile path and the name used for the file. If this last parameter is empty ("") it will automatically use the program name. To this it will always append an alphanumeric string showing the timestamp of the moment the program is opened.

This library makes use of the auto-generation feature of the jconfigloader to automatically produce a standard version of the config file if one is not found in the program folder.

**scriber::ScribeLogger::getInstance()->logToX_setLevel**: used to change the enable status of a level, either on terminal or on file. This function will NOT act on the overall settings described in the scriber_init.json file


### Compiling
To compile a program using this library, you will have to specify the c++ version to use to be 17 or higher and the library file to take from, as seen in the example below

- g++ -std=c++17 -o \<your_output\> \<your_program\> -lscriber -ljconfigloader

## TODO:
- add better methods to change log level enabling
- add color settings options
- prettify the README.md
