# MySH - A Simple Linux Shell  

## Description  
MySH is a simple shell implemented in C for Linux environments. It allows users to create and manage processes, execute system programs, and supports features like I/O redirection, piping, background execution, and signal handling.  

## Features  
✅ Supports input/output redirection (`>`, `<`, `>>`)  
✅ Allows piping between commands (`|`)  
✅ Supports background execution (`command &`)  
✅ Handles signals (`SIGINT`, `SIGTSTP`)  
✅ Provides wildcard support (`*`, `?`) for file matching  

## Installation  

1. Clone the repository:  
   ```sh
   git clone https://github.com/yourusername/mysh.git
   cd mysh
   ```  
2. Compile the shell:  
   ```sh
   make
   ```  

## Usage  

Run the shell:  
```sh
./mysh
```  
Example commands:  
```sh
ls -l > output.txt    # Redirect output to a file  
cat < file.txt        # Redirect input from a file  
ls | grep ".c"        # Pipe output to another command  
sleep 10 &            # Run command in background  
```  

## License  
This project is licensed under the MIT License.  
