#include <stdio.h>
#include "Command.h"

Command::Command(const char* c):
    cmd(c)
{
}

Command::~Command() {
}

int Command::execute(const Path& path) {
    string c = "";
    int pos = 0;
    while (true) {
        int open = cmd.find_first_of('<', pos);
        if (open == string::npos) {
            c += cmd.substr(pos);
            break;
        }
        int close = cmd.find_first_of('>', open);
        if (close == string::npos) {
            c += cmd.substr(pos);
            break;
        }
        c += cmd.substr(pos, open - pos);
        string group = cmd.substr(open, close - open + 1);
        int from, to;
        int n = sscanf(group.c_str(), "<%d,%d>", &from, &to);
        if (n == 1) {
            c += path[from];
        } else if (n == 2) {
            c += path.range(from, to);
        }
        pos = close + 1;
    }

    return system(c.c_str());;
}
