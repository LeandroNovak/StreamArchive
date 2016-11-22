#include <iostream>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <string>
using namespace std;

int total = 0;
string allfiles;
string startdelimiter = "@";
string enddelimiter = "%";
string endstring = "$";

bool explore(char* dirname) 
{
    DIR *dir;
    struct dirent *entry;
    struct stat info;
    dir = opendir(dirname);

    if (!dir) {
        return false;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') {
            string path = string(dirname) + "/" + string(entry->d_name);
            cout << path << endl;
            allfiles += startdelimiter;
            allfiles += path + enddelimiter;
            ++total;
            stat(path.c_str(), &info);

            if (S_ISDIR(info.st_mode)) {
                explore((char *)path.c_str());
            }
        }
    }

    closedir(dir);
    return true;
}

int main(int argc, char** argv)
{
    cout << argv[1] << endl;
    char name1[] = "olá";
    teststring(name1);
    explore(argv[1]);
    cout << total << endl;
    allfiles += endstring;
    cout << allfiles << endl;

    unsigned first = allfiles.find(startdelimiter);
    unsigned last = allfiles.find(enddelimiter);
    string strnew = allfiles.substr(first + 1, last - first - 1);
    cout << allfiles[first + 1] << endl;
    cout << endl << endl << strnew << endl;
    cout << first << " " << last << " " << allfiles.size() << endl;
    return 0;
}