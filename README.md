# sar

1. NAME  
            sar - cria e extrai uma estrutura de diretórios em um arquivo  

2. SYNOPSIS  
            sar -c diretório  
            sar [-e|-l] arquivo_sar  

3. DESCRIPTION  
    * -c diretório  
        Com a opção -c, especifica-se um diretório e o programa cria um arquivo chamado nome.sar, contendo todos os arquivos e sub-diretórios contidos no diretório indicado.  
    * -e arquivo_sar  
        A opção -e indica a extração de arquivos e, neste caso, indica-se um arquivo com extensão .sar.  
        Toda a hierarquia de diretórios e arquivos contidos no arquivo são extraídos.  

    * -l arquivo_sar  
        A opção de listagem, indicada por -l, usa um arquivo com extensão .sar e apresenta na tela a lista de arquivos nele armazenados, incluindo a estrutura de diretórios.  

4. RETURN CODES  
    * sar produz os seguintes valores de retorno:  
        0: execução bem sucedida  
        1: o argumento passado na criação não é um diretório  
        2: o argumento passado na listagem ou extração não é um arquivo .sar válido  

5. SEE ALSO  
        tar(5) 
        https://www.youtube.com/watch?v=w9l8kLPQ39c

6. RUN and MAKE files
    * MAKE: compiles the cpp file.
    * RUN: compiles and executes the cpp file.

