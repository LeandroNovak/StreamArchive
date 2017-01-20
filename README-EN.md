# **sar - stream archive**
----

#### **1. NAME**  

sar - create and extract a directory structure in a file

#### **2. SYNOPSIS**

    sar -c directory
    sar [-e|-l] sar_file    

#### **3. DESCRIPTION**  

**_-c directory_**  

With the -c option, specifies a directory and the program creates one file called name.sar, containing all the files and sub-directories contained on the indicated directory.  

**_-e sar_file_**  

The -e option indicates a file extraction and, in that case, a file with the .sar extension is indicated as argument.
The entire hierarchy of directories and files contained in the .sar file is extracted.  

**_-l sar_file_**  

The listing option, indicated by -l, receives a file with .sar extension and shows in the screen the entire hierarchy of directories and files contained in the .sar file.  

#### **4. RETURN CODES**  
    
Sar produces the following return values:  

    0: successful execution 
    1: the argument passed in the criation is not a directory 
    2: the argument passed in the listing or extraction is not a valid .sar file 

#### **5. SAR FILE STRUCTURE**  

The first 4 bytes are the file type followed by an end-of-line character.  

    SAR

Then whe have the first data area, where we find the relation of files and directories separated by an end-of-file character.
In the second area we find the compressed files, following the structure below and repeating for all files:  

    <!dir>  
    file name  
    <!bin>  
    file content 

And finally the end of the file is indicated by the closure tag:    

    <!end>  

#### **6. COMPILE**  

Both in Linux and Windows the compilation is done by g++ (MinGW in Windows).  

On Linux:  

    g++ -o sar sar.cpp  

On Windows:  

    g++ -o sar sar.cpp -static-libstdc++  

#### **7. RUN**  

On Linux:  

    ./sar [option] [directory or file]  

On Windows:  

    sar.exe [option] [directory or file]   

#### **8. TO DO**  

Translate comments and change the tags.
