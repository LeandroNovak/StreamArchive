# **sar - stream archive**
----

#### **1. NAME**  

sar - cria e extrai uma estrutura de diretórios em um arquivo  

#### **2. SYNOPSIS**

    sar -c diretório  
    sar [-e|-l] arquivo_sar  

#### **3. DESCRIPTION**  

**_-c diretório_**  

Com a opção -c, especifica-se um diretório e o programa cria um arquivo chamado nome.sar, contendo todos os arquivos e sub-diretórios contidos no diretório indicado.  

**_-e arquivo_sar_**  

A opção -e indica uma extração de arquivos e, nesse caso, um arquivo com a extensão .sar é indicado como argumento.  
Toda a hierarquia de diretórios e arquivos contidos no arquivo são extraídos.  

**_-l arquivo_sar_**  

A opção de listagem, indicada por -l, usa um arquivo com extensão .sar e apresenta na tela toda a hierarquia de diretórios e arquivos contidos no arquivo .sar.  

#### **4. RETURN CODES**  
    
Sar produz os seguintes valores de retorno:  

    0: execução bem sucedida  
    1: o argumento passado na criação não é um diretório  
    2: o argumento passado na listagem ou extração não é um arquivo .sar válido  


#### **5. SAR FILE STRUCTURE**  

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

- [x] Traduzir README
- [ ] Traduzir comentários
- [ ] Trocar as tags delimitadoras
