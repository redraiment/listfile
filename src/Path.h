#ifndef __LISTFILE_PATH_H_
#define __LISTFILE_PATH_H_

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "common.h"

typedef enum {
    DEFAULT = 0,
    ABSOLUTE_PATH,
    RELATIVE_PATH
} OUTPUT_TYPE;

typedef enum {
    COLOR_READONLY = 0x8,
    COLOR_DIRECTORY,
    COLOR_EXECUTEABLE,
    COLOR_LINK,
    COLOR_SYSTEM
} OUTPUT_COLOR;

class Path {
private:
    string metapath;
    OUTPUT_TYPE type;
    vector<string> nodes;
private:
    void split();
public:
    Path(const char* path IN, const OUTPUT_TYPE t = DEFAULT IN);
    Path(const vector<string> n IN, const OUTPUT_TYPE t = DEFAULT IN);
    ~Path();

    int count() const;
    string operator[](const size_t index) const;
    string range(int from, int to) const;

    string getAbsolutePath() const;
    string getMetaPath() const;
    string getRelativePath(const char* path = "." IN) const;
    friend ostream& operator<<(ostream& out IN, const Path& path);
};

#endif
