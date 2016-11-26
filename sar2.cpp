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
header

<ldir?>
<l?>
diretorios
<l!>
<ldir!>

<sarf?>
<sard?>
diretorio_do arquivo_com_nome_e_extensão
<sard!>
<file?>
conteudo_do_arquivo
<file!>
<sarf!>
*/

#include <iostream>
#include <fstream>
//#include <string>
#include <cstring>
//#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <vector>
using namespace std;

/////////////////////////////////////////////////////////////////////
/// DEFINES
/////////////////////////////////////////////////////////////////////
#define SUCCESS 0               // execução bem sucedida
#define NOT_A_DIRECTORY 1       // argumento não é um diretório
#define NOT_A_SAR_FILE 2        // argumento não é um arquivo sar válido
#define FAILURE 3               // caso genérico para execução mal sucedida

#define TRUE 1
#define FALSE 0

#define BEGIN_DIR_AREA "<ldir?>"
#define END_DIR_AREA "<ldir!>"
#define BEGIN_DIR "<l?>"
#define END_DIR "<l!>"
#define BEGIN_FILE_AREA "<sarf?>"


/////////////////////////////////////////////////////////////////////
/// GLOBAL DATA AREA
/////////////////////////////////////////////////////////////////////
vector<string> path_list;
std::ifstream inputFile;
std::ofstream outputFile;


/////////////////////////////////////////////////////////////////////
/// Inserts the file at the end of the sar file
/////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////
// Returns true if the path is a directory, otherwise returns false
/////////////////////////////////////////////////////////////////////
int is_dir(const char *path) 
{
    struct stat path_stat;
    stat(path, &path_stat);
    return (S_ISDIR(path_stat.st_mode));
}

/////////////////////////////////////////////////////////////////////
/// Returns true if the file is a sar file, otherwise returns false
/////////////////////////////////////////////////////////////////////
int is_sar(const char *path)
{
    inputFile.open(path, ios::in | std::ofstream::binary);
    inputFile.seekg (0, ios::beg);
    string str;

    if (inputFile.is_open())
    {    
        inputFile >> str;
        cout << str << endl;
        inputFile.close();
        return (str == "SAR\n") ? TRUE : FALSE;
    }

    return FALSE;
}

/////////////////////////////////////////////////////////////////////
/// Generates the directories list and saves them
/////////////////////////////////////////////////////////////////////
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
        if (entry->d_name[0] != '.') {
            string newpath = string(path) + "/" + string(entry->d_name);
            
            path_list.push_back(newpath);
            
            stat(newpath.c_str(), &info);
            
            if (S_ISDIR(info.st_mode)) {
                get_dir((char *)newpath.c_str());
            }
        }
    }

    closedir(dir);
    return FALSE;
}

/////////////////////////////////////////////////////////////////////
/// compresses the files in a new sar file
/////////////////////////////////////////////////////////////////////
int compress_files(const char *path)
{
    return FALSE; 
}

/////////////////////////////////////////////////////////////////////
/// extract files from sar file
/////////////////////////////////////////////////////////////////////
int extract_files(const char *path)
{

    return FALSE;
}

/////////////////////////////////////////////////////////////////////
/// list the files within the sar file
/////////////////////////////////////////////////////////////////////
int list_files(const char *path)
{

    return FALSE; 
}

/////////////////////////////////////////////////////////////////////
/// Check arguments
/////////////////////////////////////////////////////////////////////
int check_args(int argc, char* argv[])
{    
    if (argc == 3)
    {
        if (strcmp("-c", argv[1]) == 0) {
            // cout << "argumento -c passado" << endl;
            // cout << "diretorio: " << argv[2] << endl;
            if (!is_dir(argv[2]))
                return NOT_A_DIRECTORY;
            else {
                get_dir(argv[2]);
                compress_files(argv[2]);
                return SUCCESS;
            }
        }

        else if (strcmp("-e", argv[1]) == 0) {
            // cout << "argumento -e passado" << endl;
            // cout << "arquivo: " << argv[2] << endl;
            if (!is_sar(argv[2]))
                return  NOT_A_SAR_FILE;
            else {
                return SUCCESS;
            }
        }
        
        else if (strcmp("-l", argv[1]) == 0) {
            // cout << "argumento -l passado" << endl;
            // cout << "arquivo: " << argv[2] << endl;
            if (!is_sar(argv[2]))
                return  NOT_A_SAR_FILE;
            else {
                list_files(argv[2]);
                return SUCCESS;
            }
            
        }
    }

    if (argc == 2)
    {
        if (strcmp("-h", argv[1]) == 0) {
            cout << "Usage: sar [option] file" << endl;
            cout << "Options:" << endl; 
            cout << "    -c            Compress the directory into a new sar file" << endl;
            cout << "    -e            Extract the sar file in the current directory" << endl;
            cout << "    -l            List all compressed files in a sar file" << endl; 
            return SUCCESS;
        }
    }

    cout << "error: missing argument" << endl;
    cout << "execution terminated" << endl;

    return FAILURE;
}

int main(int argc, char** argv)
{
    //cout << check_args(argc, argv) << endl;
    cout << FALSE << endl;
    
    // cout << "oloco" << check_args(argc, argv) << endl;
    // cout << endl;
    // cout << "============ exibe conteudo do vector ============" << endl;
    // for (vector<string>::const_iterator i = path_list.begin(); i != path_list.end(); i++)
    //     cout << *i << endl;
    // cout << endl << endl;

    // cout << "============ abre arquivo de output ==============" << endl;
    // outputFile.open("arquivo.bin", ios::out | std::ofstream::binary);

    // if (outputFile.is_open())
    // {
    //     cout << "arquivo aberto com sucesso" << endl;
    //     char offset[] = "ABCD";
    //     outputFile << "!SAR" << std::endl;
    //     for (vector<string>::const_iterator i = path_list.begin(); i != path_list.end(); i++)
    //         outputFile << *i << "\n";
    // }
    // outputFile.close();

    // cout << "============ abre arquivo de input ===============" << endl;
    // inputFile.open("arquivo.bin", ios::in | std::ofstream::binary);

    // inputFile.seekg (0, ios::beg);

    // if (inputFile.is_open())
    // {
    //     string str;
        
    //     while(!inputFile.eof())
    //     {
    //         //getline(inputFile, str);
    //         inputFile >> str;
    //         //if ()
    //         cout << str << endl;
    //     }
    // }
    // inputFile.close();
    return SUCCESS;
}
