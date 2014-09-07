Introduction
One of the key aspects of the UNIX programming environment is the tools concept: a lot of little programs that do one thing but can be combined together. The act of combining programs means that the output of one program is used as the input to another program. The mechanism that accomplishes this is the pipe. In the shell, it is denoted by a vertical bar (|).

For example, the ls command lists all the files in a directory. The wc command counts lines, words, and characters. The number of files and directories in my current directory is:


ls | wc -l
To find the top ten words in Herman Melville's Moby Dick and a count of their occurrance, we can run:


cat moby.txt |tr 'A-Z' 'a-z'|tr -C 'a-z' '\n' |sed  '/^$/d' |sort|uniq -c|sort -nr|sed 10q
Where:

cat moby.txt puts the contents of the file moby.txt onto the standard output stream.
tr 'A-Z' 'a-z' translates all uppercase letters to lowercase ones. This ensures that mixed-case words all get converted to lower case and will therefore look the same when we compare them.
tr -C 'a-z' '\n' translates anything that is not a lowercase letter into a newline (\n). This gives us one word per line as well as a lot of extra blank lines.
sed '/^$/d' runs sed, the stream editor, to delete all empty lines (i.e., lines that match the regular expression ^$).
sort sorts the output. Now all of our words are sorted.
uniq -c combines adjacent lines (words) that are identical and writes out a count of the number of duplicates followed by the line.
sort -nr sorts the output of uniq by the count of duplicate lines. The sorting is in inverse numeric order (-nr). The output from this is a frequency-ordered list of unique words.
sed 10q tells the stream editor to quit after reading ten lines. The user will see the top ten lines of the output of sort -nr.
In the shell, four kernel mechanisms are crucial to accomplishing this:

fork
The fork system call is used to create new processes. Each command in the pipeline has to run as a separate process. The result of a fork is a cloned copy of the parent process.
execve
The execve system call overlays a new program onto the current process. Without execve, we would never be able to run new programs. When the shell runs a command, it forks itself and the child then loads and runs the new program via execve. There are several variations of execve written as library functions: execl, execle, execlp, execv, execvp, execvP. All of them end up calling execve.
dup2
Dup2 simply duplicates one open file descriptor (the handle that is used to read and write files) onto another file descriptor number. This is the basis of I/O redirection. Under UNIX, programs that may interact with the terminal expect to have three file descriptors open and ready for use. File descriptor 0 is the standard input, file descriptor 1 is the standard output, and file descriptor 2 is the standard error. Typically, standard input comes from the keyboard and standard output and error go to the virtual terminal (terminal window running the shell). However, any or all of these can be redirected to other files (or devices; there is usually no distinction in UNIX). As an example of how this is done, consider redirecting the standard output of a program. Before calling execve, the child process opens the desired output file. The open system call returns a file descriptor, which is a small integer. The child then calls dup2 to duplicate this new file descriptor onto file descriptor 1, the standard output. dup2 causes the current file descriptor 1 to be closed and the new one is duplicated onto 1. All further output to file descriptor 1 (standard output) will now go to this newly-opened file. After this, the child process calls execve to run the desired program, which will simply write to file descriptor 1 for its output.
pipe
The pipe system call sets up a unidirectional communication channel using file descriptors. It creates two file descriptors: any data written to the second file descriptor will be read from the first file descriptor. Coupled with fork, execve, and dup2, it allows one process to have a communication stream to another process. See pge 142 of the text for a detailed description.
Objective
Your assignment will be a simple shell that will run one command or a pipeline of commands. When each process terminates, the shell will print the exit status for the process.

Your goal in this assignment is to become familiar with the basic set of system calls that let you create processes, establish pipes between them, and detect when a child process has died. This assignment will use the fork, execve, wait, pipe, dup2, chdir, and exit system calls.

Don't be intimidated by the length of this write-up. The HTML for this page is far longer the C code for the assignment! I hope to give you enough guidance with this assignment so that you will know exactly what to do. My version of this is well under 300 lines of code, including a fair number of comments and blank lines.

Team
The assignment should not take a lot of effort. However, since the class is huge, you may work with up to four partners on it. Be sure to identify the group members in your writeup and in a comment in the assignment.

Languages
This assignment must be done in C.

Specifications
Your assignment is to write a rudimentary shell, or command interpreter, with very specific capabilities. Here's what it will do, step by step:

If the standard input is coming from a terminal, then print a prompt ("$ " or anything else you'd like; preferably something different than your normal shell prompt so that you'll remember when you're running this shell). You can detect if the standard input to the shell is a terminal or not via the isatty() library function (see here).
Get a command line from the user. You may assume that the entire command is on one line. This line may be a pipeline of one or more commands.
Parse the command line. Each command is a sequence of one or more tokens separated by whitespace (spaces or tabs). Tokens containing spaces must be quoted with either single or double quotes. Commands may be piped together by separating them with a vertical bar (|). There need not be a space on either side of the vertical bar. Examples of valid input lines are:


echo I "am a command"
ls|wc -l
"ls" | wc '-l'
The last two commands are identical.

Break the command and arguments into an argument list (an array of char *). You may assume that no command will have more than 50 arguments.

The result of your parsing will be a list of piped commands. Each command will be parsed into an its arguments.

Execute the pipeline of commands. If one command has its output going to another command, then create a pipe and set the standard output of the command to the pipe. If the command is reading input from a pipe then set the standard input to the pipe. See the Hints section for more advice.

Execute each command using fork and execve. Using the execvp variant of execve will probably be the easiest since it handles searching your search path (PATH environment variable) for the command — just pass it the command name and the full argument list. You may not use the system library function. If you do, your grade for the assignment will be 0. If the child cannot execute the desired command (execvp fails), then print an error message (see perror) and exit the child with an exit code of 1.

Check if the command is one of two special built-in commands. If so, then call local functions to execute them and do not fork a new process. Built-in commands must be implemented as a table of pointers to functions. Each build-in command accepts the same parameters that any external program will get: (int argc, char **argv).

cd [dirname]
The cd command changes the current working directory to that specified by the first and only argument. An error is printed if more than one arguments are present or if changing the directory fails. Otherwise, the full pathname of the new directory is printed. See the chdir system call man page and the getcwd library function. If there is no argument given to the cd command then change to the user's home directory. You can get this by reading the envrionment variable HOME via getenv("HOME").
exit [value]
The exit command causes the shell process to exit by calling the exit library function (which, in turn, calls the _exit system call). If no parameter is provided, then exit returns with an exit code of 0. Otherwise, exit returns with the error code provided by value. Note that the exit code will be truncated to eight bits (0..255). No need to print an error here since chances are that we really want to exit the process, even if the syntax is wrong.
Neither of the built-in commands have to function in a pipeline of commands.

The parent will now wait for all the child processes to terminate before going back to step 1. To do this, loop on wait until wait returns a value of -1. For each process that terminates, print that child's exit status. See here for how to do this. Do not be concerned if some of the termination messages are interspersed with some output from the commands themselves.
Hints
Develop and test incrementally!
Develop and test your code incrementally. Write a 10-30 lines of code at a time and then test it. For example, try this sequence:

Prompt for a command, get the line, print it, and repeat.
Parse the commmand line into tokens. A token is either a quoted string, a set of non-space characters, or a | character. Test this extensively to ensure that this is robust. If a string starts with a single quote, it must end with a single quote. If it starts with a double quote (") then it must end with a double quote. For example, the command:


'abc'   "de f'g"  hij|  k "lm | no"
must parse into the tokens:

token 1: "abc"
token 2: "de f'g"
token 3: "hij"
token 4: "|"
token 5: "k"
token 6: "lm | no"
Build up a list of commands. Each command is a sequence of arguments. A pipe separates commands. From the above example, the list will be:

command 1: "abc", "de f'g", "hij"
command 2: "k", "lm | no"
Check for built-in commands. If found, call the corresponding function to execute them. Built-in commands must be implemented as a table of pointers to functions.
Set up a pipe (if necessary) and execute a command using fork, dup2, and exec.
Loop and wait for all processes to die, printing their status. Make sure that all processes do indeed terminate.
Malloc and strcpy
If you use malloc for dynamic memory allocation, be sure to use free to clean it up. Your program does not need to use any dynamic memory allocation or string copying for parsing the command line. All you need to do is keep track of pointers to the start of each token and terminate the end of the token within the command line buffer. Unnecessary memory copying is your enemy!

Getting lines
fgets is a fine function to use for reading a line. Don't use gets since it doesn't allow you to specify the size of the buffer and is a target for buffer overflow attacks.

Creating an argument list
Your parsing function will split a line containing a command into an array of pointers to characters.

You have to write this yourself since you need to parse tokens that might be quoted with single or double quotes and may therefore contain whitespace or pipe characters within the quoted string. You also have to account for the pipe character possibly not having any whitespace before or after surrounding tokens. Using the strtok library will not work. You may assume a maximum number of arguments to be a static amount of at least 50.

Keep your functions small
Advice: resist the temptation to put a lot into any one function. Keep the purpose of each function highly focused. For example, create separate functions to read lines, parse lines, and fork & exec. Err on the side of more functions.
Running built-in commands
The shell has to support two built-in commands: exit and cd. The trivial but uninspiring way to do this is simply via a few if statements with strcmp. Don't do this! Instead, use a table that maps a built-in function name to the function and search the table. One possible definition for such a table structure is:


	struct builtins {
		char *name;	/* name of function */
		int (*f)();	/* function to execute for the built-in command */
	}
So now we run through the table of builtins, searching for a match for the first argument (arg[0]). If there is one, we call the corresponding function and return its value back to parse, which returns the value back to main. This is overkill for just two commands but it's really handy if the list grows to a dozen or more built-in commands.

Running external programs
If the function is not a built-in then we have to execute it as a command, so we fork().

The child does an execvp(argv[0], argv). This is a library function over execve that saves us the trouble of searching through the PATH environment variable.

Handling pipe and dup2
See the tutorial here. Note that it is crucial that you close the file descriptors of the pipe in the parent. Otherwise, any processes reading from pipes will never exit. Also, you'll be building up more and more open but unusued file descriptors in your parent.

See these brief tutorials
how to fork, exec, and wait: fork, execve, fork+exec, wait
dup2
pipe and dup2
isatty
The main page for tutorials is here.

Testing
Check the guidelines to testing the assignment.

Submission
Be sure to indicate all members of your team in the opening comments in your code. If it takes us effort to figure out whose program this is, you will lose points. Only one team member needs to submit the assignment but be sure that the comments make it clear who the members are.

You may submit a Makefile so that I can run make to compile your program (see some basic notes on makefiles). Otherwise, I expect to be able to compile your program via:

cc -o shell shell.c
Submit only what is required. Do not submit object files, executables, or temporary editor files.

Hand the assignment in via sakai. Only one group member needs to submit the assignment.

Before sending the file to me, make sure that all the components are there. If I can't compile any part, you will get no credit.
