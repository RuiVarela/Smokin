#include "CommandLine.hpp"

namespace smk
{
    static void del_arg(int argc, char **argv, int index) {
        int i;
        for(i = index; i < argc-1; ++i)
            argv[i] = argv[i+1];
        argv[i] = nullptr;
    }

    bool findArg(int argc, char *argv[], std::string arg) {
        for (int i = 0; i < argc; ++i) {
            if (!argv[i])
                continue;

            if (arg == argv[i]) {
                del_arg(argc, argv, i);
                return true;
            }
        }

        return false;
    }

    int findArgInt(int argc, char **argv, std::string arg, int def) {
        for (int i = 0; i < argc; ++i) {
            if (!argv[i])
                continue;

            if (arg == argv[i])
            {
                def = atoi(argv[i + 1]);
                del_arg(argc, argv, i);
                del_arg(argc, argv, i);
                break;
            }
        }

        return def;
    }

    float findArgFloat(int argc, char **argv, std::string arg, float def) {
        for (int i = 0; i < argc; ++i) {
            if (!argv[i])
                continue;

            if (arg == argv[i])
            {
                def = float(atof(argv[i + 1]));
                del_arg(argc, argv, i);
                del_arg(argc, argv, i);
                break;
            }
        }

        return def;
    }

    std::string findArgStr(int argc, char **argv, std::string arg, std::string def) {
        for (int i = 0; i < argc; ++i) {
            if (!argv[i])
                continue;

            if (arg == argv[i])
            {
                def = argv[i + 1];
                del_arg(argc, argv, i);
                del_arg(argc, argv, i);
                break;
            }
        }

        return def;
    }
}
