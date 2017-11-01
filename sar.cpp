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

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif // _WIN32

///////////////////////////////////////////////////////////////////////////////
/// DEFINITIONS
///////////////////////////////////////////////////////////////////////////////
#define SUCCESS 0               // successfull execution
#define NOT_A_DIRECTORY 1       // argument is not a directory
#define NOT_A_SAR_FILE 2        // argument is not a valid sar file
#define FAILURE 3               // code for generic errors

#define SAR_TAG "SAR"
#define DIR_NAME "<!dir>"       // Tag of begin of directory
#define BIN_AREA "<!bin>"       // Tag of begin of file area
#define END_FILE "<!end>"       // Tag of end of file

#define byte char

///////////////////////////////////////////////////////////////////////////////
/// GLOBAL VARIABLES
///////////////////////////////////////////////////////////////////////////////
char current_directory[256];        // Current directory saved in a char array
std::string work_directory;         // Current directory saved in a string

std::vector<std::string> path_list; // Vector to directories list

std::ifstream in_file;              // Input stream of files
std::ofstream out_file;             // Output stream of files

///////////////////////////////////////////////////////////////////////////////
/// FUNCTIONS PROTOTYPES
///////////////////////////////////////////////////////////////////////////////
int is_dir(const char *path);
int is_sar(const char *path);
int get_dir(const char *path);
int create_directory(std::string path);
int compress_files(const char *path);
int extract_files(const char *path);
int list_files(const char *filename);
int check_args(int argc, char* argv[]);

///////////////////////////////////////////////////////////////////////////////
// Returns true if the path is a directory
///////////////////////////////////////////////////////////////////////////////
int is_dir(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return (S_ISDIR(path_stat.st_mode));
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the path is a file
///////////////////////////////////////////////////////////////////////////////
int is_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return (S_ISREG(path_stat.st_mode));
}

///////////////////////////////////////////////////////////////////////////////
/// Returns true if the file is a SAR file
///////////////////////////////////////////////////////////////////////////////
int is_sar(const char *path)
{
    if (is_dir(path))
        return false;

    in_file.open(path, std::ios::in | std::ofstream::binary);
    in_file.seekg (0, std::ios::beg);
    std::string str;

    if (in_file.is_open()) 
    {
        getline(in_file, str);
        in_file.close();
        return (str == SAR_TAG) ? true : false;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
/// Generates the path list and saves into path_list
///////////////////////////////////////////////////////////////////////////////
int get_dir(const char *path)
{
    DIR *dir;
    struct dirent *entry;
    struct stat info;

    dir = opendir(path);

    // se dir for null
    if (!dir)
        return true;

    // enquanto houverem diretórios a serem lidos
    while ((entry = readdir(dir)) != NULL) 
    {
        // se o diretório lido não for o atual, nem o diretório pai
        if (entry->d_name[0] != '.') 
        {
            std::string newpath = std::string(path) + "/" + std::string(entry->d_name);

            // salva o diretório
            path_list.push_back(newpath);

            stat(newpath.c_str(), &info);

            if (S_ISDIR(info.st_mode)) 
            {
                // busca o próximo recursivamente
                get_dir((char *)newpath.c_str());
            }
        }
    }

    closedir(dir);
    return false;
}

///////////////////////////////////////////////////////////////////////////////
/// Creates a directory in the given path
///////////////////////////////////////////////////////////////////////////////
int create_directory(std::string path)
{
    int first = path.find_first_of("/");

    // If the string doesn't contains an '/', all the directories where created
    if (first != -1) 
    {
        std::string aux = path.substr(first + 1, path.size());
        std::string dir = path.substr(0, first);
        work_directory.append("/" + dir);

        #ifdef _WIN32
        CreateDirectory(work_directory.c_str(), NULL);
        #else
        mkdir(work_directory.c_str(), ACCESSPERMS);
        #endif // _WIN32

	// Calls itself recursively to create the next subdirectory
        return create_directory(aux);
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
/// Archieve the files in a new SAR file
///////////////////////////////////////////////////////////////////////////////
int compress_files(const char *path)
{
    std::string filename(path);
    int begin = filename.find_last_of("/");
    int end = filename.size();

    // Obtém o nome da pasta a ser comprimida para o nome do arquivo sar
    if (begin != -1 && begin != end - 1)
        filename = filename.substr(begin + 1, end - begin);
    else if (begin == end - 1)
        filename = filename.substr(0, begin);

    // Adiciona a extensão
    filename.append(".sar");

    out_file.open(filename.c_str(), std::ios::out | std::ofstream::binary);

    if (out_file.is_open()) 
    {
        // Adiciona o cabeçalho identificador de arquivo sar
        out_file << SAR_TAG << std::endl;

        for (std::vector<std::string>::const_iterator i = path_list.begin(); i != path_list.end(); i++)
            out_file << *i << "\n";
        
        std::cout << "Compressing files" << std::endl;
        
        for (std::vector<std::string>::const_iterator i = path_list.begin(); i != path_list.end(); i++) 
        {
            filename = *i;
            if (!is_dir(filename.c_str())) 
            {
                // Adiciona a tag de nome de diretório
                out_file << DIR_NAME << "\n";
                // O nome do diretório
                out_file << filename << "\n";
                // E a tag de conteúdo do arquivo
                out_file << BIN_AREA << "\n";

                in_file.open(filename.c_str(), std::ios::in | std::ofstream::binary);

                // Lê byte a byte dando copiando do arquivo original para o novo arquivo
                if (in_file.is_open()) 
                {
                    in_file.seekg (0, std::ios::beg);
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
        // Insere a tag de encerramento do arquivo sar
        out_file << END_FILE << "\n";

        out_file.close();

        std::cout << "All files were compressed" << std::endl;
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
/// Extrai os arquivos do arquivo sar
///////////////////////////////////////////////////////////////////////////////
int extract_files(const char *path)
{
    std::string filename(path);
    in_file.open(path, std::ios::in | std::ofstream::binary);

    if (in_file.is_open()) 
    {
        // Move o ponteiro de leitura de arquivo para o início da área de diretórios
        in_file.seekg (0, std::ios::beg);
        in_file.seekg (4, std::ios::cur);

        byte data[1];
        byte data_aux[6];

        // Busca a área de arquivos
        while (1) 
        {
            std::getline(in_file, filename, (char)0x0A);
            if (filename == DIR_NAME)
                break;
        }

        while (1) 
        {
            getline(in_file, filename);

            std::string temp(current_directory);
            work_directory = temp;
            create_directory(filename);

            out_file.open(filename.c_str(), std::ios::out | std::ofstream::binary);

            if (out_file.is_open()) 
            {
                std::cout << "Extracting files" << std::endl;

                // Lê o <!bin> e o \n depois dele
                in_file >> filename;
                in_file.read(data, sizeof(data));   
                
                while(1) 
                {
                    in_file.read(data, sizeof(data));

                    // Ao encontrar um caractere identificador, verifica se a sequência bate com a de alguma tag
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

                                            // Se a tag lida for a indicadora de nome de um novo arquivo
                                            if (data_aux[5] == '\n') 
                                            {
                                                getline(in_file, filename);

                                                // Cria o diretório para armazenar este arquivo
                                                std::string temp(current_directory);
                                                work_directory = temp;
                                                create_directory(filename);

                                                out_file.close();

                                                out_file.open(filename.c_str(), std::ios::out | std::ofstream::binary);

                                                if (in_file.is_open()) 
                                                {
                                                    // Lê o <!bin> e o \n depois dele
                                                    in_file >> filename;
                                                    in_file.read(data, sizeof(data));
                                                } 
                                                else
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

                                            // Caso a tag seja de final do arquivo sar, encerra-se a descompressão
                                            if (data_aux[5] == '\n') 
                                            {
                                                in_file.close();
                                                out_file.close();

                                                std::cout << "All files have been extracted" << std::endl;
                                                return true;
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
                        out_file.write(data, 1);
                }
                out_file.close();
            }
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
/// Lista os arquivos e diretórios compactados
///////////////////////////////////////////////////////////////////////////////
int list_files(const char *filename)
{
    in_file.open(filename, std::ios::out | std::ofstream::binary);

    if (in_file.is_open()) 
    {
        // Salta a leitura para o início da área de diretórios
        in_file.seekg (0, std::ios::beg);
        in_file.seekg (4, std::ios::cur);
        std::string path;

        // Lê uma string do arquivo tendo como caractere delimitador o \n
        std::getline(in_file, path, (char)0x0A);
        
        // Indicador de diretório pai
        std::cout << "." << std::endl; 
        while (1) 
        {
            if (path == DIR_NAME)
                break;
            
            // Imprime a lista de diretórios de forma estruturada com base em seu caminho
            int pos = path.find_last_of("/");

            std::cout << "|";
            for (int i = 0 ; i < pos / 3; i++)
                std::cout << "-";

            std::cout << " " << path.substr(pos + 1, path.size()) << std::endl;
            std::getline(in_file, path, (char)0x0A);
        }

        in_file.close();
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
/// Check arguments
///////////////////////////////////////////////////////////////////////////////
int check_args(int argc, char* argv[])
{
    if (argc == 3) 
    {
        // para o argumento -c
        if (strcmp("-c", argv[1]) == 0) 
        {
            if (!is_dir(argv[2]))
                return NOT_A_DIRECTORY;
            else 
            {
                get_dir(argv[2]);
                compress_files(argv[2]);
                return SUCCESS;
            }
        }

        // para o argumento -e
        else if (strcmp("-e", argv[1]) == 0) 
        {
            if (!is_sar(argv[2]))
                return  NOT_A_SAR_FILE;
            else 
            {
                extract_files(argv[2]);
                return SUCCESS;
            }
        }

        // para o argumento -l
        else if (strcmp("-l", argv[1]) == 0) 
        {
            if (!is_sar(argv[2]))
                return  NOT_A_SAR_FILE;
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
            std::cout << "Options:" << std::endl;

            #ifdef _WIN32
            std::cout << "Usage: sar.exe [option] file" << std::endl;
            #else
            std::cout << "Usage: sar [option] file" << std::endl;
            #endif // _WIN32

            std::cout << "    -c          Compress the directory into a new sar file" << std::endl;
            std::cout << "    -e          Extract the sar file in the current directory" << std::endl;
            std::cout << "    -l          List all compressed files in a sar file" << std::endl;
            return SUCCESS;
        }
    }

    std::cout << "error: missing argument" << std::endl;
    std::cout << "execution terminated" << std::endl;

    return FAILURE;
}

int main(int argc, char* argv[])
{
    // Salva o diretório onde foi executado o arquivo sar
	getcwd(current_directory, sizeof(current_directory));
	return check_args(argc, argv);
}
