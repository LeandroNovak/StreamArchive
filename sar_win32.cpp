#include <iostream>
#include <windows.h>
using namespace std;

char current_directory[MAX_PATH];

int is_dir(const char *path);
int is_sar(const char *path);
int get_dir(const char *path);
int create_directory(string path);
int compress_files(const char *path);
int extract_files(const char *path);
int list_files(const char *filename);
int check_args(int argc, char* argv[]);

int check_args(int argc, char* argv[])
{    
    if (argc == 3)
    {
        if (strcmp("-c", argv[1]) == 0) 
        {
            if (!is_dir(argv[2]))
            {
                return NOT_A_DIRECTORY;
            }
            else 
            {
                get_dir(argv[2]);
                compress_files(argv[2]);
                return SUCCESS;
            }
        }

        else if (strcmp("-e", argv[1]) == 0) 
        {
            if (!is_sar(argv[2]))
            {
                return  NOT_A_SAR_FILE;
            }
            else 
            {
                extract_files(argv[2]);
                return SUCCESS;
            }
        }
        
        else if (strcmp("-l", argv[1]) == 0) 
        {
            if (!is_sar(argv[2]))
            {
                cout << "f" << endl;
                return  NOT_A_SAR_FILE;
            }
            else 
            {
                list_files(argv[2]);
                return SUCCESS;
            }
        }
    }
}

int main(int argc, char* argv[])
{
	GetModuleFileName(NULL, current_directory, MAX_PATH);
	cout << current_directory << endl;


	return check_args(argc, argv);
}
