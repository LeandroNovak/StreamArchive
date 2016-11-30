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
#endif // _WIN32

///////////////////////////////////////////////////////////////////////////////
/// DEFINES
///////////////////////////////////////////////////////////////////////////////
#define SUCCESS 0                   // execução bem sucedida
#define NOT_A_DIRECTORY 1           // argumento não é um diretório
#define NOT_A_SAR_FILE 2            // argumento não é um arquivo sar válido
#define FAILURE 3                   // caso genérico para execução mal sucedida

#define DIR_NAME "<!dir>"           // nome do arquivo
#define BIN_AREA "<!bin>"           // arquivo compactado
#define END_FILE "<!end>"           // final do arquivo sar

#define byte char                   // tipo de dado byte é um char

///////////////////////////////////////////////////////////////////////////////
/// GLOBAL DATA AREA
///////////////////////////////////////////////////////////////////////////////
char current_directory[256];        // diretório do programa sar
std::string work_directory;         // diretório do programa sar em string

std::vector<std::string> path_list; // lista de diretórios

std::ifstream in_file;              // arquivo de entrada
std::ofstream out_file;             // arquivo de saída

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
// Returns true if the path is a directory, otherwise returns false
///////////////////////////////////////////////////////////////////////////////
int is_dir(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return (S_ISDIR(path_stat.st_mode));
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the path is a file, otherwise returns false
///////////////////////////////////////////////////////////////////////////////
int is_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return (S_ISREG(path_stat.st_mode));
}

///////////////////////////////////////////////////////////////////////////////
/// Returns true if the file is a sar file, otherwise returns false
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
        return (str == "SAR") ? true : false;
    }

    return false;
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
        return true;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] != '.')
        {
            std::string newpath = std::string(path) + "/" + std::string(entry->d_name);

            path_list.push_back(newpath);

            stat(newpath.c_str(), &info);

            if (S_ISDIR(info.st_mode))
            {
                get_dir((char *)newpath.c_str());
            }
        }
    }

    closedir(dir);
    return false;
}

///////////////////////////////////////////////////////////////////////////////
/// Create a directory
///////////////////////////////////////////////////////////////////////////////
int create_directory(std::string path)
{
    int first = path.find_first_of("/");

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

        return create_directory(aux);
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
/// compresses the files in a new sar file
///////////////////////////////////////////////////////////////////////////////
int compress_files(const char *path)
{
    std::string filename(path);
    int begin = filename.find_last_of("/");
    int end = filename.size();

    if (begin != -1 && begin != end - 1)
        filename = filename.substr(begin + 1, end - begin);
    else if (begin == end - 1)
        filename = filename.substr(0, begin);

    filename.append(".sar");
    out_file.open(filename.c_str(), std::ios::out | std::ofstream::binary);

    if (out_file.is_open())
    {
        out_file << "SAR" << std::endl;

        for (std::vector<std::string>::const_iterator i = path_list.begin(); i != path_list.end(); i++)
        {
            out_file << *i << "\n";
        }

        // string teste;
        // for (int i = 0; i < path_list.size(); i++)
        // {
        //     teste = path_list[i];
        //     out_file.write(teste, teste.size());
        //     out_file
        //     std::cout << i << ": " << path_list[i] << std::endl; 
        // }

        //std::cout << "Compactando arquivos" << std::endl;
        std::cout << "Compressing files" << std::endl;
        for (std::vector<std::string>::const_iterator i = path_list.begin(); i != path_list.end(); i++)
        {
            filename = *i;
            if (!is_dir(filename.c_str()))
            {
                out_file << DIR_NAME << "\n";
                out_file << filename << "\n";
                out_file << BIN_AREA << "\n";

                in_file.open(filename.c_str(), std::ios::in | std::ofstream::binary);

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
        out_file << END_FILE << "\n";

        out_file.close();

        // std::cout << "Todos os arquivos foram compactados" << std::endl;
        std::cout << "All files were compressed" << std::endl;
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
/// extract files from sar file
///////////////////////////////////////////////////////////////////////////////
int extract_files(const char *path)
{
    std::string filename(path);
    in_file.open(path, std::ios::in | std::ofstream::binary);

    if (in_file.is_open())
    {
        in_file.seekg (0, std::ios::beg);
        in_file.seekg (4, std::ios::cur);

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

            std::string temp(current_directory);
            work_directory = temp;
            create_directory(filename);

            out_file.open(filename.c_str(), std::ios::out | std::ofstream::binary);

            if (out_file.is_open())
            {
                //std::cout << "Extraindo arquivos" << std::endl;
                std::cout << "Extracting files" << std::endl;
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
                                                std::string temp(current_directory);
                                                work_directory = temp;
                                                create_directory(filename);
                                                out_file.close();

                                                out_file.open(filename.c_str(), std::ios::out | std::ofstream::binary);

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
    //std::cout << "Todos os arquivos foram extraidos" << std::endl;
    std::cout << "All files have been extracted" << std::endl;
    return false;
}

///////////////////////////////////////////////////////////////////////////////
/// list the files within the sar file
///////////////////////////////////////////////////////////////////////////////
int list_files(const char *filename)
{
    in_file.open(filename, std::ios::out | std::ofstream::binary);

    std::cout << "." << std::endl;
    if (in_file.is_open())
    {
        in_file.seekg (0, std::ios::beg);
        in_file.seekg (4, std::ios::cur);
        std::string path;

        std::getline(in_file, path, (char)0x0A);
        
        while (1)
        {
            if (path == DIR_NAME)
                break;
            
            std::cout << "|";

            int pos = 0;
            if ((pos = path.find("/", pos + 1)) != -1)
            {
                std::cout << "--";
            }

            while ((pos = path.find("/", pos + 1)) != -1)
            {
                std::cout << "---";
            }

            pos = path.find_last_of("/");

            
            // for (int i = 0 ; i < (pos / 4) - 1; i++)
            // {
            //     std::cout << "-";
            // }

            // std::cout << pos << ": ";
            std::cout << " " << path.substr(pos + 1, path.size());
            std::getline(in_file, path, (char)0x0A);
            if (path == DIR_NAME)
            {
                std::cout << "\r" << "+";//(char)192;
            }

            std::cout << std::endl;
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
            std::cout << "Options:" << std::endl;
            //std::cout << "Opcoes:" << std::endl;

            #ifdef _WIN32
            std::cout << "Usage: sar.exe [option] file" << std::endl;
            //std::cout << "Modo de uso: sar.exe [opcao] [arquivo ou diretorio]" << std::endl;
            #else
            std::cout << "Usage: sar [option] file" << std::endl;
            //std::cout << "Modo de uso: sar [opcao] [arquivo ou diretorio]" << std::endl;
            #endif // _WIN32

            //std::cout << "    -c          Comprime o diretorio em um novo arquivo sar" << std::endl;
            std::cout << "    -c          Compress the directory into a new sar file" << std::endl;
            //std::cout << "    -e          Extrai o arquivo sar no diretorio atual" << std::endl;
            std::cout << "    -e          Extract the sar file in the current directory" << std::endl;
            //std::cout << "    -l          Lista todos os arquivo compactados em um arquivo sar" << std::endl;
            std::cout << "    -l          List all compressed files in a sar file" << std::endl;
            return SUCCESS;
        }
    }

    //std::cout << "erro: faltando argumento" << std::endl;
    std::cout << "error: missing argument" << std::endl;
    //std::cout << "execucao encerrada" << std::endl;
    std::cout << "execution terminated" << std::endl;

    return FAILURE;
}

//int main(int argc, char* argv[])
int main(int argc, char* argv[])
{
	getcwd(current_directory, sizeof(current_directory));
	return check_args(argc, argv);
}