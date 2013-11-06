#include <windows.h>
#include <cstring>
#include <cctype>
#include "FileList.h"

FileList::FileList(OUTPUT_TYPE t, bool a, bool ignore):
    type(t), all(a), flag(ignore? NOCASE: NOFLAGS) {
}

FileList::~FileList() {
}

bool FileList::isListAll() {
    return all;
}

bool FileList::isIgnoreCase() {
    return flag == NOCASE;
}

OUTPUT_TYPE FileList::getOutputType() {
    return type;
}

void FileList::setListHidden(bool a) {
    all = a;
}

void FileList::setIgnoreCase(bool ignore) {
    flag = ignore? NOCASE: NOFLAGS;
}

void FileList::setOutputType(OUTPUT_TYPE t) {
    type = t;
}

FileList::iterator FileList::begin() {
    return path.begin();
}

FileList::iterator FileList::end() {
    return path.end();
}

string FileList::getRootPath(string& p) {
    string root;

    node.clear();
    // find the root directory
    if (p.length() >= 3 && p.substr(1, 2) == ":\\" && isalpha(p[0])) {
        // driver
        root = p.substr(0, 3);
        node.push_back(root);
        p.erase(0, 3);
    } else if (p[0] == '~') {
        // home directory
        char home[MAX_PATH] = {0};
        GetEnvironmentVariable("userprofile", home, MAX_PATH);
        char* username = strrchr(home, File::separator);
        *username++ = 0;
        root = home;
        node.push_back(root);

        p.erase(0, 1);
        if (p.length() == 0 || p[0] == File::separator) {
            p.insert(0, username);
        }
    } else {
        // current directory
        root = ".";
    }

    return root;
}

void FileList::parse(const char* rpath) {
    pattern.clear();
    while (*rpath) {
        char item[MAX_PATH] = "^";
        int i = 1;
        while (*rpath && *rpath != File::separator) {
            switch (*rpath) {
            case '<': {
                // regular expressions
                item[i++] = '(';
                int count = 1;
                rpath++;
                while (count > 0) {
                    switch (*rpath) {
                    case '\\': {
                        item[i++] = *rpath++;
                    }   break;
                    case '<': {
                        count++;
                    }   break;
                    case '>': {
                        count--;
                    }   break;
                    default:
                        break;
                    }
                    item[i++] = *rpath++;
                }
                item[i - 1] = ')';  // remove the last '}'
            }   break;
            case '.':
            case '{':
            case '[':
            case '(':
            case ')':
            case '^':
            case '$':
            case '+': {
                item[i++] = '\\';
                item[i++] = *rpath++;
            }   break;
            case '?': {
                item[i++] = '.';
                rpath++;
            }   break;
            case '*': {
                item[i++] = '.';
                item[i++] = *rpath++;
            }   break;
            default: {
                item[i++] = *rpath++;
            }   break;
            }
        }
        if (*rpath) {
            rpath++;
        }
        if (i > 1) {
            item[i++] = '$';
            pattern.push_back(string(item));
        }
    }
}

void FileList::list(File& f, const int n) {
    if (n >= pattern.size()) {
        path.push_back(Path(node, type));
        return;
    }

    if (pattern[n] == "^\\.$") {
        // current directory
        node.push_back(".");
        list(f, n + 1);
    } else if (pattern[n] == "^\\.\\.$") {
        // parent directory
        node.push_back("..");
        string pname = f.getAbsolutePath() + File::separator;
        pname += "..";
        File parent(pname.c_str());
        list(parent, n + 1);
    } else {
        rpattern p(pattern[n].c_str(), flag);
        match_results results;
        match_results::backref_type br;

        vector<File> files;
        f.listFiles(files);
        for (int i = 0; i < files.size(); i++) {
            if (files[i].isHidden() && !all) {
                continue;
            }
            br = p.match(files[i].getName(), results);

            if (br.matched) {
                node.push_back(br.str());
                list(files[i], n + 1);
                node.pop_back();
            }
        }
    }
}

void FileList::push(const char* p) {
    string rpath(p);
    string root = getRootPath(rpath);
    parse(rpath.c_str());

    File f(root.c_str());
    list(f, 0);
}
