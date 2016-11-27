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
*/

/*
__Ideia de estrutura de um arquivo sar__
!SAR

diretorios
<dir!>
diretorio_do arquivo_com_nome_e_extensão
<bin!>
conteudo_do_arquivo
<!SAR>
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
//#include <cstdio>
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

#define BEGIN_DIR_AREA "<dir?>"
#define END_DIR_AREA "<dir!>"
#define BEGIN_DIR "<l?>"
#define END_DIR "<l!>"
#define BEGIN_FILE_AREA "<sarf?>"
#define END_FILE_AREA "<sarf!>"
#define BEGIN_FILE "<file?>"
#define END_FILE "<file!>"

#define DIR_NAME "<dir!>"
#define BIN_AREA "<dir!>"

#define byte char


///////////////////////////////////////////////////////////////////////////////
/// GLOBAL DATA AREA
///////////////////////////////////////////////////////////////////////////////
vector<string> path_list;
std::ifstream in_file;
std::ofstream out_file;

///////////////////////////////////////////////////////////////////////////////
/// Inserts the file at the end of the sar file
///////////////////////////////////////////////////////////////////////////////
void append(const char *source) 
{
    FILE* dest_file_sar = fopen("arquivo.sar", "ab");
    FILE* source_file   = fopen(source, "rb");
    
    char letter;

    while (letter != EOF);
    {
        letter = fgetc(source_file);
        fputc(letter, dest_file_sar);
    }

    // while(!feof(source_file))
    // {
    //     fread(&letter, sizeof(char), 1, source_file);
    //     fwrite(&letter, sizeof(char), 1, dest_file_sar);
    // }

    fclose(dest_file_sar);
    fclose(source_file);	
}

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
        in_file >> str;
        //cout << str << endl;
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

///////////////////////////////////////////////////////////////////////////////
/// compresses the files in a new sar file
///////////////////////////////////////////////////////////////////////////////
int compress_files(const char *path)
{
    string filename(path);
    int begin = filename.find_last_of("/");
    int end = filename.size();

    if (begin != -1)
        filename = filename.substr(begin + 1, end - begin);
    
    filename.append(".sar");
    cout << filename << endl;
    out_file.open(filename.c_str(), ios::out | std::ofstream::binary);

    if (out_file.is_open())
    {
        out_file << "!SAR" << std::endl;

        for (vector<string>::const_iterator i = path_list.begin(); i != path_list.end(); i++)
            out_file << *i << "\n";

        cout << "================ compactando ================" << endl;
        for (vector<string>::const_iterator i = path_list.begin(); i != path_list.end(); i++)
        {
            filename = *i;
            if (!is_dir(filename.c_str()))
            {
                out_file << DIR_NAME;
                out_file << filename;
                out_file << BIN_AREA;
                
                cout << filename << endl;
                in_file.open(filename.c_str(), ios::in | std::ofstream::binary);
                in_file.seekg (0, ios::beg);

                if (in_file.is_open())
                {
                    byte data[1];
                    
                    while(!in_file.eof())
                    {
                        in_file.read(data, 1);
                        out_file.write(data, 1);
                    }
                }
                in_file.close();
            }
        }
        out_file.close();
    }
    
    return FALSE; 
}

///////////////////////////////////////////////////////////////////////////////
/// extract files from sar file
///////////////////////////////////////////////////////////////////////////////
int extract_files(const char *path)
{

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// list the files within the sar file
///
///////////////////////////////////////////////////////////////////////////////
int list_files(const char *filename)
{
    in_file.open(filename, ios::out | std::ofstream::binary);
    in_file.seekg (0, ios::beg);
    in_file.seekg (5, ios::cur);

    if (in_file.is_open())
    {
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
            // cout << "argumento -c passado" << endl;
            // cout << "diretorio: " << argv[2] << endl;
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
            // cout << "argumento -e passado" << endl;
            // cout << "arquivo: " << argv[2] << endl;
            
            if (!is_sar(argv[2]))
            {
                return  NOT_A_SAR_FILE;
            }
            else 
            {
                return SUCCESS;
            }
        }
        
        else if (strcmp("-l", argv[1]) == 0) 
        {
            // cout << "argumento -l passado" << endl;
            // cout << "arquivo: " << argv[2] << endl;
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
        if (strcmp("-h", argv[1]) == 0) 
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
    check_args(argc, argv);
    
    // cout << "oloco" << check_args(argc, argv) << endl;
    // cout << endl;
    // cout << "============ exibe conteudo do vector ============" << endl;
    // for (vector<string>::const_iterator i = path_list.begin(); i != path_list.end(); i++)
    //     cout << *i << endl;
    // cout << endl << endl;

    // cout << "============ abre arquivo de output ==============" << endl;
    // out_file.open("arquivo.bin", ios::out | std::ofstream::binary);

    // if (out_file.is_open())
    // {
    //     cout << "arquivo aberto com sucesso" << endl;
    //     char offset[] = "ABCD";
    //     out_file << "!SAR" << std::endl;
    //     for (vector<string>::const_iterator i = path_list.begin(); i != path_list.end(); i++)
    //         out_file << *i << "\n";
    // }
    // out_file.close();

    // cout << "============ abre arquivo de input ===============" << endl;
    // in_file.open("arquivo.bin", ios::in | std::ofstream::binary);

    // in_file.seekg (0, ios::beg);

    // if (in_file.is_open())
    // {
    //     string str;
        
    //     while(!in_file.eof())
    //     {
    //         //getline(in_file, str);
    //         in_file >> str;
    //         //if ()
    //         cout << str << endl;
    //     }
    // }
    // in_file.close();
    return SUCCESS;
}
