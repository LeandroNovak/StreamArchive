/*
NAME
   sar - cria e extrai uma estrutura de diretórios em um arquivo

SYNOPSIS
    sar -c diretório
    sar [-e|-l] arquivo_sar

DESCRIPTION
 -c diretório
   Com a opção -c, especifica-se um diretório e o programa cria um arquivo
   chamado nome.sar, contendo todos os arquivos e sub-diretórios contidos no diretório indicado.

 -e arquivo_sar
   A opção -e indica a extração de arquivos e, neste caso, indica-se um arquivo 
   com extensão .sar. Toda a hierarquia de diretórios e arquivos contidos no arquivo
   são extraídos.

 -l arquivo_sar
   A opção de listagem, indicada por -l, usa um arquivo com extensão .sar e
   apresenta na tela a lista de arquivos nele armazenados, incluindo a estrutura
   de diretórios.

RETURN CODES
  sar produz os seguintes valores de retorno:
   0: execução bem sucedida
   1: o argumento passado na criação não é um diretório
   2: o argumento passado na listagem ou extração não é um arquivo .sar válido

SEE ALSO
   tar(5)

Estrutura de um arquivo sar:

!SAR
diretorios
<dir!>
diretorio_do arquivo_com_nome_e_extensão
<bin!>
conteudo_do_arquivo
...
<!end>
*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <clocale>
#include <wchar.h>
using namespace std;

///////////////////////////////////////////////////////////////////////////////
/// DEFINES
///////////////////////////////////////////////////////////////////////////////
#define SUCCESS 0               // execução bem sucedida
#define NOT_A_DIRECTORY 1       // argumento não é um diretório
#define NOT_A_SAR_FILE 2        // argumento não é um arquivo sar válido
#define FAILURE 3               // caso genérico para execução mal sucedida

#define TRUE 1
#define FALSE 0

#define DIR_NAME "<!dir>"
#define BIN_AREA "<!bin>"
#define END_FILE "<!end>"

#define byte char

///////////////////////////////////////////////////////////////////////////////
/// GLOBAL DATA AREA
///////////////////////////////////////////////////////////////////////////////
char current_directory[128];
string work_directory;

vector<string> path_list;

std::ifstream in_file;
std::ofstream out_file;

///////////////////////////////////////////////////////////////////////////////
/// FUNCTIONS PROTOTYPES
///////////////////////////////////////////////////////////////////////////////
int is_dir(const char *path);
int is_sar(const char *path);
int get_dir(const char *path);
int create_directory(string path);
int compress_files(const char *path);
int extract_files(const char *path);
int list_files(const char *filename);
int check_args(int argc, char* argv[]);

///////////////////////////////////////////////////////////////////////////////
// Returns true if the path is a directory, otherwise returns false
///////////////////////////////////////////////////////////////////////////////
int is_dir(const char *path) 
{
    struct stat path_stat;
    stat(path, &path_stat);
    return (S_ISDIR(path_stat.st_mode));
}

///////////////////////////////////////////////////////////////////////////////
/// Returns true if the file is a sar file, otherwise returns false
///////////////////////////////////////////////////////////////////////////////
int is_sar(const char *path)
{
    if (is_dir(path))
        return FALSE;
    
    in_file.open(path, ios::in | std::ofstream::binary);
    in_file.seekg (0, ios::beg);
    string str;

    if (in_file.is_open())
    {    
        getline(in_file, str);
        in_file.close();
        return (str == "!SAR") ? TRUE : FALSE;
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// Generates the directories list and saves them
///////////////////////////////////////////////////////////////////////////////
int get_dir(const char *path)
{
    DIR *dir;
    struct dirent *entry;
    struct stat info;

    dir = opendir(path);
    if (!dir)
        return TRUE;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] != '.') 
        {
            string newpath = string(path) + "/" + string(entry->d_name);
            
            path_list.push_back(newpath);
            
            stat(newpath.c_str(), &info);
            
            if (S_ISDIR(info.st_mode)) 
            {
                get_dir((char *)newpath.c_str());
            }
        }
    }

    closedir(dir);
    return FALSE;
}

int create_directory(string path)
{
    int first = path.find_first_of("/");

    if (first != -1)
    {   
        string aux = path.substr(first + 1, path.size());
        string dir = path.substr(0, first);
        work_directory.append("/" + dir);
        mkdir(work_directory.c_str(), ACCESSPERMS);
        return create_directory(aux);
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// compresses the files in a new sar file
///////////////////////////////////////////////////////////////////////////////
int compress_files(const char *path)
{
    string filename(path);
    int begin = filename.find_last_of("/");
    int end = filename.size();

    if (begin != -1 && begin != end - 1)
        filename = filename.substr(begin + 1, end - begin);
    else if (begin == end - 1)
        filename = filename.substr(0, begin);
    
    filename.append(".sar");
    out_file.open(filename.c_str(), ios::out | std::ofstream::binary);

    if (out_file.is_open())
    {
        out_file << "!SAR" << std::endl;

        for (vector<string>::const_iterator i = path_list.begin(); i != path_list.end(); i++)
            out_file << *i << "\n";

        cout << "Compressing files" << endl;
        for (vector<string>::const_iterator i = path_list.begin(); i != path_list.end(); i++)
        {
            filename = *i;
            if (!is_dir(filename.c_str()))
            {
                out_file << DIR_NAME << "\n";
                out_file << filename << "\n";
                out_file << BIN_AREA << "\n";

                in_file.open(filename.c_str(), ios::in | std::ofstream::binary);

                if (in_file.is_open())
                {
                    in_file.seekg (0, ios::beg);
                    byte data[1];
                    
                    in_file.read(data, sizeof(data));
                    while(!in_file.eof())
                    {
                        out_file.write(data, sizeof(data));
                        in_file.read(data, sizeof(data));
                    }
                    in_file.close();
                }
                
            }
        }
        out_file << END_FILE << "\n";
        
        out_file.close();
        
        cout << "All files were compressed" << endl;
        return TRUE;
    }
    
    return FALSE; 
}

///////////////////////////////////////////////////////////////////////////////
/// extract files from sar file
///////////////////////////////////////////////////////////////////////////////
int extract_files(const char *path)
{
    string filename(path);
    in_file.open(path, ios::in | std::ofstream::binary);
    unsigned total_files = 0;
    unsigned current_file = 0;

    if (in_file.is_open())
    {
        in_file.seekg (0, ios::beg);
        in_file.seekg (5, ios::cur);

        byte data[1];
        byte data_aux[6];
        
        // Go to begin of file area
        while (1)
        {
            //in_file >> filename;
            getline(in_file, filename);
            
            if (filename == DIR_NAME)
                break;
        }

        // File area
        while (1)
        {
            //in_file >> filename;
            getline(in_file, filename);

            string temp(current_directory);
            work_directory = temp;
            create_directory(filename);

            out_file.open(filename.c_str(), ios::out | std::ofstream::binary);
            
            if (out_file.is_open())
            {
                cout << "Extracting files" << endl;
                in_file >> filename;                // read <!bin>
                in_file.read(data, sizeof(data));   //Remove \n after <!bin>
                while(1)
                {
                    in_file.read(data, sizeof(data));
                    if (data[0] == '<')
                    {
                        in_file.read(&data_aux[0], 1);
                        
                        if (data_aux[0] == '!')
                        {
                            in_file.read(&data_aux[1], 1);

                            if (data_aux[1] == 'd')
                            {
                                in_file.read(&data_aux[2], 1);

                                if (data_aux[2] == 'i')
                                {
                                    in_file.read(&data_aux[3], 1);

                                    if (data_aux[3] == 'r')
                                    {
                                        in_file.read(&data_aux[4], 1);
                                        if (data_aux[4] == '>')
                                        {
                                            in_file.read(&data_aux[5], 1);

                                            if (data_aux[5] == '\n')
                                            {
                                                getline(in_file, filename);
                                                string temp(current_directory);
                                                work_directory = temp;
                                                create_directory(filename);
                                                out_file.close();

                                                out_file.open(filename.c_str(), ios::out | std::ofstream::binary);

                                                if (in_file.is_open())
                                                {
                                                    in_file >> filename;                //Read <!bin>
                                                    in_file.read(data, sizeof(data));   //Remove \n after <!bin>
                                                }
                                                else
                                                {
                                                    break;
                                                }
                                                
                                            }
                                            else
                                            {
                                                out_file.write(data, 1);
                                                out_file.write(&data_aux[0], 1);
                                                out_file.write(&data_aux[1], 1);
                                                out_file.write(&data_aux[2], 1);
                                                out_file.write(&data_aux[3], 1);
                                                out_file.write(&data_aux[4], 1);
                                                out_file.write(&data_aux[5], 1);
                                            }
                                        }
                                        else
                                        {
                                            out_file.write(data, 1);
                                            out_file.write(&data_aux[0], 1);
                                            out_file.write(&data_aux[1], 1);
                                            out_file.write(&data_aux[2], 1);
                                            out_file.write(&data_aux[3], 1);
                                            out_file.write(&data_aux[4], 1);
                                        }
                                    }
                                    else
                                    {
                                        out_file.write(data, 1);
                                        out_file.write(&data_aux[0], 1);
                                        out_file.write(&data_aux[1], 1);
                                        out_file.write(&data_aux[2], 1);
                                        out_file.write(&data_aux[3], 1);
                                    }
                                }
                                else
                                {
                                    out_file.write(data, 1);
                                    out_file.write(&data_aux[0], 1);
                                    out_file.write(&data_aux[1], 1);
                                    out_file.write(&data_aux[2], 1);
                                }
                            }

                            else if (data_aux[1] == 'e')
                            {
                                in_file.read(&data_aux[2], 1);

                                if (data_aux[2] == 'n')
                                {
                                    in_file.read(&data_aux[3], 1);

                                    if (data_aux[3] == 'd')
                                    {
                                        in_file.read(&data_aux[4], 1);
                                        if (data_aux[4] == '>')
                                        {
                                            in_file.read(&data_aux[5], 1);

                                            if (data_aux[5] == '\n')
                                            {
                                                in_file.close();
                                                break;
                                            }
                                            else
                                            {
                                                out_file.write(data, 1);
                                                out_file.write(&data_aux[0], 1);
                                                out_file.write(&data_aux[1], 1);
                                                out_file.write(&data_aux[2], 1);
                                                out_file.write(&data_aux[3], 1);
                                                out_file.write(&data_aux[4], 1);
                                                out_file.write(&data_aux[5], 1);
                                            }
                                        }
                                        else
                                        {
                                            out_file.write(data, 1);
                                            out_file.write(&data_aux[0], 1);
                                            out_file.write(&data_aux[1], 1);
                                            out_file.write(&data_aux[2], 1);
                                            out_file.write(&data_aux[3], 1);
                                            out_file.write(&data_aux[4], 1);
                                        }
                                    }
                                    else
                                    {
                                        out_file.write(data, 1);
                                        out_file.write(&data_aux[0], 1);
                                        out_file.write(&data_aux[1], 1);
                                        out_file.write(&data_aux[2], 1);
                                        out_file.write(&data_aux[3], 1);
                                    }
                                }
                                else
                                {
                                    out_file.write(data, 1);
                                    out_file.write(&data_aux[0], 1);
                                    out_file.write(&data_aux[1], 1);
                                    out_file.write(&data_aux[2], 1);
                                }
                            }

                            else
                            {
                                out_file.write(data, 1);
                                out_file.write(&data_aux[0], 1);
                                out_file.write(&data_aux[1], 1);
                            }
                        }
                        else
                        {
                            out_file.write(data, 1);
                            out_file.write(&data_aux[0], 1);
                        }
                    }

                    else
                    {
                        out_file.write(data, 1);
                    }
                }
                out_file.close();
            }
            break;
        }
    }
    cout << "All files have been extracted" << endl;
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// list the files within the sar file
///////////////////////////////////////////////////////////////////////////////
int list_files(const char *filename)
{
    in_file.open(filename, ios::out | std::ofstream::binary);

    if (in_file.is_open())
    {
        in_file.seekg (0, ios::beg);
        in_file.seekg (5, ios::cur);
        string path;
        while (1)
        {
            in_file >> path;
            if (path == DIR_NAME)
                break;
            cout << path << endl;
        } 

        in_file.close();
    }
    return FALSE; 
}

///////////////////////////////////////////////////////////////////////////////
/// Check arguments
///////////////////////////////////////////////////////////////////////////////
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

    if (argc == 2)
    {
        if ((strcmp("-h", argv[1]) == 0) || (strcmp("-a", argv[1]) == 0))  
        {
            cout << "Usage: sar [option] file" << endl;
            cout << "Options:" << endl; 
            cout << "    -c          Compress the directory into a new sar file" << endl;
            cout << "    -e          Extract the sar file in the current directory" << endl;
            cout << "    -l          List all compressed files in a sar file" << endl; 
            return SUCCESS;
        }
    }

    cout << "error: missing argument" << endl;
    cout << "execution terminated" << endl;

    return FAILURE;
}

///////////////////////////////////////////////////////////////////////////////
/// MAIN
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    getcwd(current_directory, sizeof(current_directory));
    return check_args(argc, argv);
}