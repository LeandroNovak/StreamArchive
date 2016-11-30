# **sar**
----

#### **1. NAME**  

sar - cria e extrai uma estrutura de diretórios em um arquivo  

#### **2. SYNOPSIS**

    sar -c diretório  
    sar [-e|-l] arquivo_sar  

#### **3. DESCRIPTION**  

-c diretório  

    Com a opção -c, especifica-se um diretório e o programa cria um arquivo chamado nome.sar, contendo todos os arquivos e sub-diretórios contidos no diretório indicado.  

-e arquivo_sar  

    A opção -e indica a extração de arquivos e, neste caso, indica-se um arquivo com extensão .sar.  
    Toda a hierarquia de diretórios e arquivos contidos no arquivo são extraídos.  

-l arquivo_sar  

    A opção de listagem, indicada por -l, usa um arquivo com extensão .sar e apresenta na tela a lista de arquivos nele armazenados, incluindo a estrutura de diretórios.  

#### **4. RETURN CODES**  
    
Sar produz os seguintes valores de retorno:  

    0: execução bem sucedida  
    1: o argumento passado na criação não é um diretório  
    2: o argumento passado na listagem ou extração não é um arquivo .sar válido  


#### **5. SAR FILE STRUCTURE**  

O arquivo sar é composto por 
Cada uma das tags representadas acima são, assim como a tag SAR, encerradas com o caractere de fim de linha.  

Os primeiros 4 bytes são o tipo do arquivo seguido de um caractere de fim de linha  

    !SAR

Em seguida temos a primeira grande área onde encontramos a relação de diretórios e arquivos seguidos de uma tag de encerramento:

    lista de diretórios
    <dir!>  

A segunda grande área contém os arquivos compactados, seguindo a estrutura abaixo e se repetindo para todos os aquivos:  

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

#### **8. SEE ALSO**  
tar(5)  


