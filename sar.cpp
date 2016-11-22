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

#include <iostream>
#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
using namespace std;

<<<<<<< HEAD
#define SUCESS 0
#define NOT_A_DIRECTORY 1
#define NOT_A_VALID_SAR_FILE 2
#define FAILURE 3
#define SEPARATOR 0x444C

struct header {
    // File prefix
    char[] type = "SAR";
    // Size in bytes
    unsigned long int size;
};

=======
#define SUCESS 0                // execução bem sucedida
#define NOT_A_DIRECTORY 1       // argumento não é um diretório
#define NOT_A_VALID_SAR_FILE 2  // argumento não é um arquivo sar válido
#define FAILURE 3               // caso genérico para execução mal sucedida
>>>>>>> 890ea92c3ab74a9ce2ac7249e62158a328f400de

struct header_sar {
    char type[4];               // %SAR
    char name[120];             // filename.sar
    char size[4];               // filesize
} sar_header;

int isRegularFile(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int listDirectories(const char *caminho)
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir(caminho);
    if (dir != NULL)
    {
        while (entry = readdir(dir))
        {
            puts(entry->d_name);
        }
        closedir(dir);
    }
    else
        return 1;
    return 0;
}

int main(int argc, char** argv)
{

    cout << argc << endl;
    
    if (argc == 3)
    {
        if (strcmp("-c", argv[1]) == 0) {
            cout << "argumento -c passado" << endl;
            cout << "diretorio: " << argv[2] << endl;
        }

        else if (strcmp("-e", argv[1]) == 0) {
            cout << "argumento -e passado" << endl;
            cout << "arquivo: " << argv[2] << endl;
        }
        
        else if (strcmp("-l", argv[1]) == 0) {
            cout << "argumento -l passado" << endl;
            cout << "arquivo: " << argv[2] << endl;
        }
        
        if (listDirectories(argv[2]) != 0)
            return NOT_A_DIRECTORY;
        cout << isRegularFile(argv[2]) << endl;
    }

    else if (argc == 2)
    {
        if (strcmp("-h", argv[1]) == 0) {
            cout << "Aqui vai a ajuda" << endl;
        }
    }

    else 
        return FAILURE;

    cout << endl << endl;
    
    return SUCESS;
}