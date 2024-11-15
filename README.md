# POSIX Shell Implementation

## 1. Display requirement

### Description

Created C++ method that displays a command prompt similar to what you see in a Unix-like terminal. It includes the username, hostname, and current working directory. The prompt format is: username@hostname:~[relativePath]>, with [relativePath] being optional.

Infinite loop calls prompt() method each time to display a prompt.

## 2. cd, echo, pwd:

### Description

- I implemented cd command which works similar to the cd command in Unix-like shells. It allows you to change the current working directory within a program. The function takes a vector of strings args as input and handles various directory navigation scenarios. implemented all the basic functionality, also implement the flags “.”, “..”, “-” and “~.
- Implemented echo which works similar to the echo command in Unix-like shells. If string is enclosed into "" then it will print as it is otherwise spaces and tabs are handled.
- Implemented a "Print Working Directory" (PWD) function that retrieves and displays the current working directory. The PWD function is similar to the pwd command in Unix-like systems.

## 3. ls:

### Description

- Implemented "ls" command, which is commonly used to list directory contents in Unix-like operating systems. The code offers features for listing directory contents with various options, including showing hidden files (-a) and displaying detailed file information (-l). handled All the mentioned cases.

## 4. System commands (background/fg), with and without arguments:

### Description

- Implemented executeBackgroundProcess(), for executing processes in the background or foreground, similar to how commands are run in a Unix-like shell. The code handles process creation, execution, and optionally running processes in the background.

## 5. pinfo

- Implemented printProcessInfo() method which utilizes proc filesystem to extracts and displays various pieces of information about a specified process.
- pinfo(id) will print details of process with mentioned id.
- Implemented pinfo will not run on mac as mac not supports proc filesystem.

## 6. search

- My search method searches for a specified file or folder under the current directory recursively. It returns True if the item exists and False if it doesn't.

## 7. I/O redirection

- Implemented input and output redirection using the symbols <, >, and >>. It allows you to redirect the output of commands to a file, read input from a file, or both.
- ex: cat < example.txt

## Approach

- The shell parses the command line arguments to identify input and output redirection requests.
- For input redirection (<), the shell reads from the specified file as input for the command.
- For output redirection (> and >>), the shell creates or appends to the specified file and directs the command's output to that file.
- The shell handles errors appropriately:
  - Displays an error message if the input file does not exist.
  - Creates the output file with permissions 0644 if it doesn't exist for > and appends to it for >>.

## 8. pipeline

- Implemented pipeline for execution commands that are present in pipe command.

## Approach

- executeCommand(): Executes a single shell command. It handles forking a child process, setting up input/output redirection, and executing the command. It also supports optional input/output redirection.

- executePipes(): Executes a sequence of shell commands separated by pipes ('|'). It creates pipes to connect the commands, tokenizes each command, and invokes executeCommand for each command in the sequence, handling input/output redirection and piping between them.
- ex: cat sample2.txt | head -7 | tail -5

- Overall, these functions enable the execution of a series of shell commands with pipes and optional input/output redirection in a C++ program.

## 10. Simple signals

- handled CTRL-D, CTRL-Z and CTRL-C using signal command.

## 12. BONUS: history

- Implemented history functionality for the commands. To maintain history across the different sessions I stored it in file.

## Exit command

- exit command will terminate the shell.

## Compile the program:

```sh
make
```

## Run the Program:

```sh
./a.out
```
