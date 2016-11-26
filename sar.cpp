/*
NAME
   sar - cria e extrai uma estrutura de diret�rios em um arquivo
SYNOPSIS
    sar -c diret�rio
    sar [-e|-l] arquivo_sar
DESCRIPTION
 -c diret�rio
   Com a op��o -c, especifica-se um diret�rio e o programa cria um arquivo
   chamado nome.sar, contendo todos os arquivos e sub-diret�rios contidos no diret�rio indicado.
 -e arquivo_sar
   A op��o -e indica a extra��o de arquivos e, neste caso, indica-se um arquivo 
   com extens�o .sar. Toda a hierarquia de diret�rios e arquivos contidos no arquivo
   s�o extra�dos.
 -l arquivo_sar
   A op��o de listagem, indicada por -l, usa um arquivo com extens�o .sar e
   apresenta na tela a lista de arquivos nele armazenados, incluindo a estrutura
   de diret�rios.
RETURN CODES
  sar produz os seguintes valores de retorno:
   0: execu��o bem sucedida
   1: o argumento passado na cria��o n�o � um diret�rio
   2: o argumento passado na listagem ou extra��o n�o � um arquivo .sar v�lido
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
diretorio_do arquivo_com_nome_e_extens�o
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

#define SUCESS 0                // execu��o bem sucedida
#define NOT_A_DIRECTORY 1       // argumento n�o � um diret�rio
#define NOT_A_VALID_SAR_FILE 2  // argumento n�o � um arquivo sar v�lido
#define FAILURE 3               // caso gen�rico para execu��o mal sucedida

#define BEGIN_DIR_AREA "<ldir?>"
#define END_DIR_AREA "<ldir!>"
#define BEGIN_DIR "<l?>"
#define END_DIR "<l!>"
#define BEGIN_FILE_AREA "<sarf?>"

struct header {
    char type[4];      // !SAR
    char name[120];    // filename.sar
    //char size[4];    // filesize
};

string path_list;

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
            // verificar se � um arquivo, se for, abrir e anexar ao destino 
            // e dar append no endere�o.
            
            stat(newpath.c_str(), &info);
            
            if (S_ISDIR(info.st_mode)) {
                listDirectories((char *)newpath.c_str());
            }
			else {
                append(entry->d_name);
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
    cout << "�oq? " << new1 << endl;



    return SUCESS;
}