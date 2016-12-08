sar - stream archive
-----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// 1. Alunos
/////////////////////////////////////////////////////////////////////////////

O trabalho foi desenvolvido em conjunto pelos seguintes alunos:

    Diogo Kionori     - 586854  
    JoÃ£o G. Marins    - 586919  
    Leandro Novak     - 586927  

/////////////////////////////////////////////////////////////////////////////
// 2. Estrutura do arquivo SAR
/////////////////////////////////////////////////////////////////////////////

Durante o planejamento do software chegou-se a um consenso de que seria
mais vantajoso separar o arquivo em sub-Ã¡reas e o armazenamento da lista
de diretÃ³rios e arquivos armazenados visando facilitar a listagem desses
arquivos e diretÃ³rios. AlÃ©m disso, decidiu-se que o arquivo seria
estruturado de maneira sequencial, uma vez que nÃ£o seria implementada a
operaÃ§Ã£o de busca e que todos os arquivos seriam extraÃ­dos de uma Ãºnica
vez.

O arquivo Ã© organizado da seguinte forma:

Os primeiros 4 bytes sÃ£o o tipo do arquivo seguido de um caractere de
fim de linha

    SAR\n  

Em seguida temos a primeira Ã¡rea de dados, onde encontramos a relaÃ§Ã£o de
diretÃ³rios e arquivos, cada um deles separado por um caractere de fim de
linha.

    nome do arquivo ou diretÃ³rio\n  

Na segunda Ã¡rea encontramos os arquivos compactados, seguindo a
estrutura abaixo e se repetindo para todos os aquivos. Vale ressaltar
que cada uma das tags e sucedida pelo caractere de fim de linha.

    <!dir>\n  
    nome do arquivo\n  
    <!bin>\n  
    conteÃºdo do arquivo\n  

E por Ãºltimo o final do arquivo Ã© indicado pela tag de encerramento:

    <!end>\n  

#### **3. Como compilar**

Tanto no Linux quanto no Windows a compilaÃ§Ã£o Ã© feita pelo g++ (MinGW no
Windows).

No Linux:

    g++ -o sar sar.cpp  

No Windows:

    g++ -o sar sar.cpp -static-libstdc++  

#### **4. Como executar**

No Linux:

    ./sar [opÃ§Ã£o] [diretÃ³rio ou arquivo]  

No Windows:

    sar.exe [opÃ§Ã£o] [diretÃ³rio ou arquivo]  

Uma seÃ§Ã£o de ajuda pode ser exibida utilizando-se as opÃ§Ãµes *-h* ou
*-a*.