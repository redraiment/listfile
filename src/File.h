#ifndef __LISTFILE_FILE_H_
#define __LISTFILE_FILE_H_

#include <string>
#include <vector>
using namespace std;

#ifdef WIN32
  #include <windows.h>
#else
  #error "This section will only work on Microsoft Windows OS!"
#endif

#include "common.h"

/**
 * I create this class just because I familiar
 * with the interface of java.io.File.
 */
class File {
private:
    string pathname;
    DWORD attribute;
public:
    const static char separator = '\\';

public:
    File();
    File(const char* pathname IN);
    File(const File& file IN);
    void assign(const File& file IN);
    File& operator=(const File& file IN);
    ~File();

    bool exists();

    bool isDirectory();
    bool isExecutable();
    bool isHidden();
    bool isLink();
    bool isReadonly();
    bool isSystem();

    DWORD getAttribute();
    string getAbsolutePath();
    string getName();
    string getExtension();

    // list all child files & direcories
    bool list(vector<string>& filenames OUT);
    bool listFiles(vector<File>& files OUT);
};

#endif
