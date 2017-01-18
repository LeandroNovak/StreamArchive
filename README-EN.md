# **sar - stream archive**
----

#### **1. NAME**  

sar - create and extract a directory structure in a file

#### **2. SYNOPSIS**

    sar -c directory
    sar [-e|-l] sar_file    

#### **3. DESCRIPTION**  

**_-c directory_**  

Com a opção -c, especifica-se um diretório e o programa cria um arquivo chamado nome.sar, contendo todos os arquivos e sub-diretórios contidos no diretório indicado.  

**_-e sar_file_**  

The option -e indicates a file extraction and, in that case, a file with the .sar extension is indicated as argument.
The entire hierarchy of directories and files contained in the .sar file is extracted.

**_-l sar_file_**  

The listing option, indicated by -l, receives a file with .sar extension and shows in the screen the entire hierarchy of directories and files contained in the .sar file.

#### **4. RETURN CODES**  
    
Sar produces the following return values: 

    0: successful execution 
    1: the argument passed in the criation is not a directory 
    2: the argument passed in the listing or extraction is not a valid .sar file 

#### **5. SAR FILE STRUCTURE**  

O arquivo sar é composto por 
Cada uma das tags representadas acima são, assim como a tag SAR, encerradas com o caractere de fim de linha.  

Os primeiros 4 bytes são o tipo do arquivo seguido de um caractere de fim de linha  

    SAR

Em seguida temos a primeira área de dados, onde encontramos a relação de diretórios e arquivos separados por um caractere de fim de linha.
Na segunda área encontramos os arquivos compactados, seguindo a estrutura abaixo e se repetindo para todos os aquivos:  

    <!dir>  
    nome do arquivo  
    <!bin>  
    conteúdo do arquivo  

E por último o final do arquivo é indicado pela tag de encerramento:  

    <!end>  

#### **6. COMPILE**  

Tanto no Linux quanto no Windows a compilação é feita pelo g++ (MinGW no Windows).  

No Linux:  

    g++ -o sar sar.cpp  

No Windows:  

    g++ -o sar sar.cpp -static-libstdc++  

#### **7. RUN**  

No Linux:  

    ./sar [opção] [diretório ou arquivo]  

No Windows:  

    sar.exe [opção] [diretório ou arquivo]  

#### **8. TO DO**  

Translate README, comments and change the tags.
