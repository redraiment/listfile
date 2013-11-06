#include <iostream>
#include <cstdlib>
using namespace std;

#include <getopt.h>

#include "FileList.h"
#include "Command.h"

const static struct option long_options[] = {
    {"all", no_argument, NULL, 'a'},
    {"absolute-path", no_argument, NULL, 'A'},
    {"exec", required_argument, NULL, 'e'},
    {"help", no_argument, NULL, 'h'},
    {"ignore-case", no_argument, NULL, 'i'},
    {"relative-path", no_argument, NULL, 'r'},
    {NULL, 0, NULL, '\0'}
};

void usage(int status) {
    FILE* fout = status? stderr: stdout;
    fprintf(fout, "NAME\n\
  lf - list files matching a regular expression\n\n");

    fprintf(fout, "SYNOPSIS\n\
  lf [OPTIONS] PATTERN...\n\n");

    fprintf(fout, "DESCRIPTION\n\
  lf lists the file and directory that matching the given pattern.\n\
  It likes find (1) on UNIX like system, but find (1) use wildcard\n\
  charcacter substitute for any other character or  characters  in\n\
  file name, like the asterisk character (\"*\") substitutes for any\n\
  zero or more characters, and the question mark (\"?\") substitutes\n\
  for any one character. But lf use regular expression, it provide\n\
  more concise and flexible means for matching strings of path and\n\
  file name.\n\n");

    fprintf(fout, "OPTIONS\n\
  -a, --all\n\
        Do not ignore hidden file.\n\
  -A, --absolute-path\n\
        Always show absolute path.\n\
  -e, --exec command\n\
        Execute command. The file/director names held can be\n\
        replayed by the sequences <1> .. <n>, <0> is replaced\n\
        with the full path name by the pattern. A range of\n\
        consecutive names can be specified by separating the\n\
        first and last numbers in the range with a hyphen. For\n\
        example, <1-n> is the same with <0>.\n\
  -h, --help\n\
        Display this help and exit.\n\
  -i, --ignore-case\n\
        The regular expression will ignore the case.\n\
  -r, --relative-path\n\
        Always show relative path.\n\n");

    fprintf(fout, "PATTERN\n\
  The pattern expressions support both wildcard character and\n\
  regular expressions, the regexp enclosed in angle brackets\n\
  (\"<\" and \">\").\n\n");

    fprintf(fout, "ENVIROMENT VARIABLES\n\
  USERPROFILE          The %UserProfile% variable is  a  special\n\
                       system-wide environment variable found on\n\
                       Microsoft Windows NT and its derivatives.\n\
                       Its value is the location of the  current\n\
                       user's profile directory, in which is found\n\
                       that user's HKCU registry hive (NTUSER).\n\n");

    fprintf(fout, "EXIT STATUS\n\
  lf exits with status 0 if all files are processed successfully,\n\
  greater than 0 if errors occur.\n\n");

    fprintf(fout, "EXAMPLES\n\
  lf \"workspaces\\project<[1-9]>\\src\\*.cpp\" -e \"copy <0> backup\\<4>\"\n\
        Copy all c plus plus files in src under project1 .. project9\n\
        to backup folder, and save with the same file name.\n\n");

    fprintf(fout, "AUTHOR\n\
  Written by Zhang, Zepeng (Joe).\n\n");

    fprintf(fout, "COPYRIGHT\n\
  lf uses Microsoft regular expression library, GRETA 2.6.4, you\n\
  should follow the related license for using greta. But lf self\n\
  is free software (of couse, open source), you may do anything\n\
  you want with the source code, you can even tell your friends\n\
  the author is you! I only hope this tool can save your time.\n\n");

    fprintf(fout, "BUGS\n\
  Email bug reports to <redraiment@gmail.com>. Thanks!\n");

    exit(status);
}

int main(int argc, char *argv[])
{
    int option_index = 0;
    FileList list;
    vector<Command> cmds;
    while (true) {
        int c = getopt_long(argc, argv, "aAe:hir", long_options, &option_index);
        if (c < 0) {
            break;
        }
        switch (c) {
        case 0:
            /* this option set a flag, do nothing else now. */
            break;
        case 'a':
            list.setListHidden(true);
            break;
        case 'A':
            list.setOutputType(ABSOLUTE_PATH);
            break;
        case 'r':
            list.setOutputType(RELATIVE_PATH);
            break;
        case 'i':
            list.setIgnoreCase(true);
            break;
        case 'e':
            cmds.push_back(Command(optarg));
            break;
        case 'h':
            usage(EXIT_SUCCESS);
            break;
        default:
            usage(EXIT_FAILURE);
            break;
        }
    }

    if (optind >= argc) {
        usage(EXIT_FAILURE);
    }

    for (int i = optind; i < argc; i++) {
        list.push(argv[i]);
    }

    FileList::iterator file = list.begin();
    while (file != list.end()) {
        if (cmds.size() == 0) {
            cout << *file << endl;
        } else {
            vector<Command>::iterator cmd = cmds.begin();
            while (cmd != cmds.end()) {
                cmd->execute(*file);
                cmd++;
            }
        }
        file++;
    }

    return EXIT_SUCCESS;
}
