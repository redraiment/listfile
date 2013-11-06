#ifndef __FILELIST_CONFIG_H_
#define __FILELIST_CONFIG_H_

#include <string>
#include <vector>
using namespace std;

#include <greta/regexpr2.h>
using namespace regex;

#include "Path.h"
#include "File.h"
#include "common.h"

class FileList {
private:
    bool all;
    REGEX_FLAGS flag;
    OUTPUT_TYPE type;
    vector<string> pattern;
    vector<string> node;
    vector<Path> path;
public:
    typedef vector<Path>::iterator iterator;

private:
    string getRootPath(string& IN OUT);
    void list(File& f IN, const int n IN);
    void parse(const char* IN);

public:
    FileList(OUTPUT_TYPE t = DEFAULT, bool a = false, bool ignore = false);
    ~FileList();

    bool isListAll();
    bool isIgnoreCase();
    OUTPUT_TYPE getOutputType();

    void setListHidden(bool);
    void setIgnoreCase(bool);
    void setOutputType(OUTPUT_TYPE);

    iterator begin();
    iterator end();
    void push(const char* p);
};

#endif
