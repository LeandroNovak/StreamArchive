 **sar - stream archive**
----  
#### **1. Alunos**  

O trabalho foi desenvolvido em conjunto pelos seguintes alunos:

    Diogo Kionori     - 586854  
    João G. Marins    - 586919  
    Leandro Novak     - 586927  

#### **2. Estrutura do arquivo SAR**  

Durante o planejamento do software chegou-se a um consenso de que seria mais vantajoso separar o arquivo 
em sub-áreas e o armazenamento da lista de diretórios e arquivos armazenados visando facilitar a listagem 
desses arquivos e diretórios. Além disso, decidiu-se que o arquivo seria estruturado de maneira sequencial, uma 
vez que não seria implementada a operação de busca e que todos os arquivos seriam extraídos de uma única vez.  

O arquivo é organizado da seguinte forma:  

Os primeiros 4 bytes são o tipo do arquivo seguido de um caractere de fim de linha  

    SAR\n  

Em seguida temos a primeira área de dados, onde encontramos a relação de diretórios e arquivos, cada um deles 
separado por um caractere de fim de linha.  

    nome do arquivo ou diretório\n  

Na segunda área encontramos os arquivos compactados, seguindo a estrutura abaixo e se repetindo para todos os aquivos.
Vale ressaltar que cada uma das tags e sucedida pelo caractere de fim de linha.  

    <!dir>\n  
    nome do arquivo\n  
    <!bin>\n  
    conteúdo do arquivo\n  

E por último o final do arquivo é indicado pela tag de encerramento:  

    <!end>\n  

#### **3. Como compilar**  

Tanto no Linux quanto no Windows a compilação é feita pelo g++ (MinGW no Windows).  

No Linux:  

    g++ -o sar sar.cpp  

No Windows:  

    g++ -o sar sar.cpp -static-libstdc++  

#### **4. Como executar**  

No Linux:  

    ./sar [opção] [diretório ou arquivo]  

No Windows:  

    sar.exe [opção] [diretório ou arquivo]  

Uma seção de ajuda pode ser exibida utilizando-se as opções _-h_ ou _-a_.


