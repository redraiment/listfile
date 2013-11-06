#include <sstream>
#include <cstring>
#include "File.h"

File::File() {
    File(".");
}

File::File(const char* pname IN) {
    if (pname == NULL) {
        throw "Illegal Argument: the pathname must not be NULL";
    }

    int size = GetFullPathName(pname, 0, NULL, NULL);
    char* fullpath = new char[size];
    if (GetFullPathName(pname, size, fullpath, NULL)) {
        if (fullpath[size - 2] == separator && fullpath[size - 3] != ':') {
            fullpath[size - 2] = 0;
        }
        pathname = fullpath;
        attribute = GetFileAttributes(fullpath);
        if (attribute == INVALID_FILE_ATTRIBUTES) {
            attribute = FILE_ATTRIBUTE_SYSTEM;
        }
    } else {
        pathname = "";
        attribute = 0x0;
    }
    delete fullpath;
}

File::File(const File& file IN) {
    assign(file);
}

void File::assign(const File& file IN) {
    pathname = file.pathname;
    attribute = file.attribute;
}

File& File::operator=(const File& file IN) {
    assign(file);
    return *this;
}

File::~File() {
}

bool File::exists() {
    return pathname != "";
}

bool File::isExecutable() {
    const char* extension = getExtension().c_str();
    char pathext[MAX_PATH] = {0};
    GetEnvironmentVariable("PATHEXT", pathext, MAX_PATH);
    stringstream ss(pathext);
    char ext[MAX_PATH];
    while (ss.getline(ext, sizeof(ext), ';')) {
        if (!stricmp(extension, ext)) {
            return true;
        }
    }

    return false;
}

bool File::isLink() {
    return getExtension() == ".lnk";
}

bool File::isDirectory() {
    return (attribute & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool File::isHidden() {
    return (attribute & FILE_ATTRIBUTE_HIDDEN) != 0;
}

bool File::isReadonly() {
    return (attribute & FILE_ATTRIBUTE_READONLY) != 0;
}

bool File::isSystem() {
    return (attribute & FILE_ATTRIBUTE_SYSTEM) != 0;
}

DWORD File::getAttribute() {
    return attribute;
}

string File::getAbsolutePath() {
    return pathname;
}

string File::getName() {
    int pos = pathname.find_last_of(separator);
    return pos != string::npos? pathname.substr(pos + 1): "";
}

string File::getExtension() {
    string filename = getName();
    int pos = filename.find_last_of(".");
    return pos != string::npos? filename.substr(pos): "";
}

bool File::list(vector<string>& filenames OUT) {
    int sizeCurrentDir = GetCurrentDirectory(0, NULL);
    char* dirBackup = new char[sizeCurrentDir];
    GetCurrentDirectory(sizeCurrentDir, dirBackup);
    SetCurrentDirectory(pathname.c_str());

    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile("*", &ffd);
    if (hFind == INVALID_HANDLE_VALUE) {
        return false;
    }

    do {
        // ignore current directory & parent directory
        if (!strcmp(ffd.cFileName, ".") || !strcmp(ffd.cFileName, "..")) {
            continue;
        }
        char fullpath[MAX_PATH] = {0};
        GetFullPathName(ffd.cFileName, MAX_PATH, fullpath, NULL);
        filenames.push_back(fullpath);
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);

    SetCurrentDirectory(dirBackup);
    delete[] dirBackup;

    return true;
}

bool File::listFiles(vector<File>& files OUT) {
    vector<string> filenames;
    if (!list(filenames)) {
        return false;
    }

    vector<string>::iterator fn = filenames.begin();
    while (fn != filenames.end()) {
        files.push_back(File(fn->c_str()));
        fn++;
    }

    return true;
}
