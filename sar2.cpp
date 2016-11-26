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

#define SUCESS 0                // execução bem sucedida
#define NOT_A_DIRECTORY 1       // argumento não é um diretório
#define NOT_A_VALID_SAR_FILE 2  // argumento não é um arquivo sar válido
#define FAILURE 3               // caso genérico para execução mal sucedida

#define BEGIN_DIR_AREA "<ldir?>"
#define END_DIR_AREA "<ldir!>"
#define BEGIN_DIR "<l?>"
#define END_DIR "<l!>"
#define BEGIN_FILE_AREA "<sarf?>"

// struct header {
//     char type[4];      // !SAR
//     char name[120];    // filename.sar
//     //char size[4];    // filesize
// };

//string path_list;
vector<string> path_list;
std::ifstream inputFile;
std::ofstream outputFile;

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
            
            path_list.push_back(newpath);
            // path_list += "@";
            // path_list += newpath + "#";
            
            //cout << newpath << endl;
            // verificar se é um arquivo, se for, abrir e anexar ao destino 
            // e dar append no endereço.
            
            stat(newpath.c_str(), &info);
            
            if (S_ISDIR(info.st_mode)) {
                listDirectories((char *)newpath.c_str());
            }
			// else {
            //     append(entry->d_name);
            // }
        }
    }

    closedir(dir);
    return 0;
}

int checkArgs()
{
    return 0;
}

int main(int argc, char** argv)
{
    
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
    for (vector<string>::const_iterator i = path_list.begin(); i != path_list.end(); i++)
        cout << *i << endl;
    cout << endl << endl;

    outputFile.open("arquivo.bin", ios::out | std::ofstream::binary);

    if (outputFile.is_open())
    {
        char offset[] = "ABCD";
        outputFile << "!SAR" << std::endl;
        for (vector<string>::const_iterator i = path_list.begin(); i != path_list.end(); i++)
            outputFile << *i << "\n";
    }
    outputFile.close();

    cout << "============= conteudo do arquivo =============" << endl;
    inputFile.open("arquivo.bin", ios::in | std::ofstream::binary);

    inputFile.seekg (0, ios::beg);

    if (inputFile.is_open())
    {
        string str;
        
        while(!inputFile.eof())
        {
            //getline(inputFile, str);
            inputFile >> str;
            //if ()
            cout << str << endl;
        }
    }
    inputFile.close();
    return SUCESS;
}
