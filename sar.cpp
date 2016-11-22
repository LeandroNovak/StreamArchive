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
#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
using namespace std;

#define SUCESS 0                // execução bem sucedida
#define NOT_A_DIRECTORY 1       // argumento não é um diretório
#define NOT_A_VALID_SAR_FILE 2  // argumento não é um arquivo sar válido
#define FAILURE 3               // caso genérico para execução mal sucedida

#define BEGIN_DIR_AREA "<ldir?>"
#define END_DIR_AREA "<ldir!>"
#define BEGIN_DIR "<l?>"
#define END_DIR "<l!>"
#define BEGIN_FILE_AREA "<sarf?>"

struct header {
    char type[4];               // !SAR
    char name[120];             // filename.sar
    //char size[4];               // filesize
};

string path_list;

int isDirectory(const char *path) 
{
    struct stat path_stat;
    stat(path, &path_stat);
    return !(S_ISDIR(path_stat.st_mode));
}

int listDirectories(const char *path)
{
    DIR *dir;
    struct dirent *entry;
    struct stat info;

    dir = opendir(path);
    if (!dir)
        return 1;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] != '.') {
            string newpath = string(path) + "/" + string(entry->d_name);
            
            path_list += "@";
            path_list += newpath + "#";
            
            cout << newpath << endl;
            // verificar se é um arquivo, se for, abrir e anexar ao destino 
            // e dar append no endereço.
            
            stat(newpath.c_str(), &info);
            
            if (S_ISDIR(info.st_mode)) {
                listDirectories((char *)newpath.c_str());
            }
        }
    }

    closedir(dir);
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
        
        if (isDirectory(argv[2]))
            return NOT_A_DIRECTORY;
        else
            listDirectories(argv[2]);
        
    }

    else if (argc == 2)
    {
        if (strcmp("-h", argv[1]) == 0) {
            cout << "Aqui vai a ajuda" << endl;
        }
    }

    else 
        return FAILURE;

    cout << endl;
    cout << path_list << endl << endl;
    cout << endl << endl;

    string bel = "belzebu";
    string pastel = "O_pastel_do_belzebu_eh_mais_barato";
    unsigned position = pastel.find("belzebu");
    cout << "aqui: " << position << endl;
    string new1 = pastel.substr(position + bel.size(), pastel.size());
    cout << "éoq? " << new1 << endl;



    return SUCESS;
}
