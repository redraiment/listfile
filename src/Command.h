#ifndef __FILELIST_COMMAND_H_
#define __FILELIST_COMMAND_H_

#include "Path.h"

class Command {
private:
    string cmd;
public:
    Command(const char*);
    ~Command();

    int execute(const Path&);
};

#endif
