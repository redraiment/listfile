#include <sstream>

#include <windows.h>
#include <shlwapi.h>

#include "Path.h"
#include "File.h"

Path::Path(const char* path IN, const OUTPUT_TYPE t IN):
    metapath(path), type(t)
{
    split();
}

Path::Path(const vector<string> n IN, const OUTPUT_TYPE t IN):
    type(t)
{
    metapath = n[0];
    for (int i = 1; i < n.size(); i++) {
        if (metapath[metapath.length() - 1] != File::separator) {
            metapath += File::separator;
        }
        metapath += n[i];
    }
    split();
}

Path::~Path() {
}

void Path::split() {
    string p;
    switch(type) {
    case DEFAULT:
        p = getMetaPath();
        break;
    case ABSOLUTE_PATH:
        p = getAbsolutePath();
        break;
    case RELATIVE_PATH:
        p = getRelativePath();
        break;
    default:
        break;
    }

    stringstream ss(p);
    char node[MAX_PATH];
    while (ss.getline(node, sizeof(node), File::separator)) {
        nodes.push_back(node);
    }
}

int Path::count() const {
    return nodes.size();
}

string Path::operator[](const size_t index) const {
    if (index == 0) {
        return range(1, count());
    } else if (0 < index && index <= count()) {
        return nodes[index - 1];
    } else {
        return "";
    }
}

string Path::range(int from, int to) const {
    string r = "";

    if (0 < from && from <= count()) {
        if (from <= to) {
            for (int i = from; 0 < i && i <= count() && i <= to; i++) {
                r += nodes[i - 1];
                if (r[r.length() - 1] != File::separator) {
                    r += File::separator;
                }
            }
        } else {
            for (int i = from; 0 < i && i <= count() && i >= to; i--) {
                r += nodes[i - 1];
                if (r[r.length() - 1] != File::separator) {
                    r += File::separator;
                }
            }
        }
        r.erase(r.length() - 1);
    }

    return r;
}

string Path::getAbsolutePath() const {
    return File(metapath.c_str()).getAbsolutePath();
}

string Path::getMetaPath() const {
    return metapath;
}

string Path::getRelativePath(const char* path IN) const {
    File from(path);
    File to(metapath.c_str());
    
    if (from.getAbsolutePath()[0] != to.getAbsolutePath()[0]) {
        // the driver is different
        return to.getAbsolutePath();
    }

    char relPath[MAX_PATH] = {0};
    PathRelativePathTo(relPath,
        from.getAbsolutePath().c_str(), from.getAttribute(),
        to.getAbsolutePath().c_str(), to.getAttribute());

    return string(relPath);
}

ostream& operator<<(ostream& out IN, const Path& path) {
    File file(path.metapath.c_str());

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(handle, &csbi);
    WORD colorOld = csbi.wAttributes;
    if (file.isReadonly()) {
        SetConsoleTextAttribute(handle, COLOR_READONLY);
    } else if (file.isDirectory()) {
        SetConsoleTextAttribute(handle, COLOR_DIRECTORY);
    } else if (file.isExecutable()) {
        SetConsoleTextAttribute(handle, COLOR_EXECUTEABLE);
    } else if (file.isLink()) {
        SetConsoleTextAttribute(handle, COLOR_LINK);
    } else if (file.isSystem()) {
        SetConsoleTextAttribute(handle, COLOR_SYSTEM);
    }

    switch(path.type) {
    case DEFAULT:
        out << path.getMetaPath();
        break;
    case ABSOLUTE_PATH:
        out << path.getAbsolutePath();
        break;
    case RELATIVE_PATH:
        out << path.getRelativePath();
        break;
    default:
        break;
    }

    SetConsoleTextAttribute(handle, colorOld);

    return out;
}
