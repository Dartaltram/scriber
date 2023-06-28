#ifndef TABLET_DEFAULTPARAM_H
#define TABLET_DEFAULTPARAM_H

#include <iostream>

static std::string defaultLoggerParam[] = {

    "{\n",
    "\t\"Version\" : \"1.0\",\n",
    "\n",
    "\t\"Terminal-logging\" : {\n",
    "\t\t\"Enable\" : true,\n",
    "\t\t\"enable none_level\" : true,\n",
    "\t\t\"enable debug_level\" : true,\n",
    "\t\t\"enable info_level\" : true,\n",
    "\t\t\"enable warning_level\" : true,\n",
    "\t\t\"enable error_level\" : true\n",
    "\t\t},\n",
    "\n",
    "\t\"File-logging\" : {\n",
    "\t\t\"path\" : \"\",\n",
    "\t\t\"fileName\" : \"\",\n",
    "\t\t\"Enable\" : true,\n",
    "\t\t\"enable none_level\" : true,\n",
    "\t\t\"enable debug_level\" : true,\n",
    "\t\t\"enable info_level\" : true,\n",
    "\t\t\"enable warning_level\" : true,\n",
    "\t\t\"enable error_level\" : true\n",
    "\t}\n",
    "}",
};

#endif // TABLET_DEFAULTPARAM_H