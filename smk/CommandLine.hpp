#pragma once

#include <string>

namespace smk {

    bool findArg(int argc, char *argv[], std::string arg);
    int findArgInt(int argc, char **argv, std::string arg, int def);
    float findArgFloat(int argc, char **argv, std::string arg, float def);
    std::string findArgStr(int argc, char **argv, std::string arg, std::string def);
}
