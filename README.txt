Drashti Patel
Sarid Shinwari
Bhargav Patel
Joe Komosinski
Operating Systems Design
Professor Paul Krzyzanowski
2 March 2014

Shell program that takes input and creates child processes to according to the command. Also contains built-in change directory and exit functions.

Basic Functionality
The program starts with initializing the table of built-in function pointers, the garbage tracker, and the word buffer array. After the initializations, it takes in the input from the standard input with fgets. Then the input is parsed into the command triple pointer. The structure of the triple pointer is that of having the input split into each pipe and the command of each pipe further split into each word that was tokenized. The parser will exit the program if there are mismatched quotes. After parsing, the command triple pointer is added into the garbage_tracker quadruple pointer. The garbage_collector keeps track of every command input into the terminal. If the parsed command is not blank, the command is sent to the forking function. In the forking function, there is a check for too many arguments, if there are too many arguments, the program exits. Then is a check for whether the command is a built in function. If it is a change directory function, the cd function does so error checking and changes the directory to the appropriate function. If it is an exit command, the function checks for the exit status and exits. Otherwise, for a different command, the program forks into a child process and parent process. The child process lays the appropriate piping and then execs. The parent process waits for the child process to finish, then loops to prepare to fork for the next command. Whenever the program exits, it runs the free_data function that takes whatever commands are in the garbage_tracker pointer and frees them.

Testing (Done on iLabs)
1. Empty command #1
New prompt
Success

2. Empty command #2
New prompt
Success

3. Exit on end of file
Clean exit
Success

4. A shell prompt for each line of input
New prompt
Success

5. No shell prompt if the standard input is not a terminal
IN:
(echo echo abc;echo echo def;echo echo ghi)|./shell
OUT:
abc
process 1361 exits with 0
def
process 1362 exits with 0
ghi
process 1363 exits with 0

6. The shell should run a simple command with arguments
IN:
<3: echo hello, world
OUT:
hello, world
process 3125 exits with 0

7. The shell should print an exit code for each process that terminates.
IN:
<3: echo hello, world
OUT:
hello, world
process 3201 exits with 0

8. Handle too many arguments gracefully
IN:
<3: echo a b c d e f g h i j k l m n o p q r s t u v w x y z a b c d e f g h i j k l m n o p q r s t u v w x y z a b c d e f g h i j k l m n o p q r s t u v w x y z a b c d e f g h i j k l m n o p q r s t u v w x y z
OUT:
Too many arguments
exit
Success

9. Quotes should be handled properly
IN:
<3: echo "abc" "def" 'ghi' 'jkl'
OUT:
abc def ghi jkl
process 3386 exits with 0

10. Spaces within quotes should be parsed correctly.
IN:
<3: ./echoargs 'abc def ghi' "j     k   l"
OUT:
argc = 3
arg[0] = "./echoargs"
arg[1] = "abc def ghi"
arg[2] = "j     k   l"
process 3512 exits with 3

11. Mismatched quotes should be detected and handled gracefully #1.
IN:
<3: echo 'abc
OUT:
Quote Mismatch

12. Mismatched quotes should be detected and handled gracefully #2.
IN:
<3: echo 'abc"
OUT:
Quote Mismatch

13. A command should work with a single pipe.
IN:
<3: ls -laF /|tr a-z A-Z
OUT:
process 3754 exits with 0
TOTAL 7504
DR-XR-XR-X.  36 ROOT ROOT    4096 MAR  2 02:07 ./
DR-XR-XR-X.  36 ROOT ROOT    4096 MAR  2 02:07 ../
DRWXR-XR-X.   9 ROOT ROOT    4096 AUG 30  2013 .AUTOFS/
-RW-R--R--    1 ROOT ROOT       0 DEC 18 13:30 .AUTOFSCK
-RW-R--R--.   1 ROOT ROOT       0 AUG 23  2012 .AUTORELABEL
DR-XR-XR-X.   2 ROOT ROOT    4096 FEB 21 04:12 BIN/
DR-XR-XR-X.   5 ROOT ROOT    4096 DEC  3 06:04 BOOT/
DRWXR-XR-X.   2 ROOT ROOT    4096 DEC  9 03:36 CGROUP/
DRWXR-XR-X   20 ROOT ROOT    4240 FEB 10 14:15 DEV/
DRWXR-XR-X. 203 ROOT ROOT   20480 MAR  2 17:22 ETC/
DRWXR-XR-X.   3 ROOT ROOT    4096 SEP 13  2012 FILER/
DRWXR-XR-X.   2 ROOT ROOT    4096 AUG 22  2013 FREESPACE/
DRWXRWXRWT.  11 ROOT ROOT    4096 JAN 29 17:48 .FREESPACE/
DRWXR-XR-X.   2 ROOT ROOT    4096 AUG 21  2012 HOME/
DRWXR-XR-X.   2 ROOT ROOT    4096 SEP 13  2012 ILAB/
DRWX------    3 ROOT ROOT    4096 NOV  7  2012 .KDE/
DRWXR-XR-X.   2 ROOT ROOT    4096 APR 17  2013 KOKO/
DRWXR-XR-X.   2 ROOT ROOT    4096 SEP  3  2012 LCSR/
DR-XR-XR-X.  13 ROOT ROOT    4096 FEB  8 03:55 LIB/
DR-XR-XR-X.  14 ROOT ROOT   12288 FEB  8 03:55 LIB64/
DRWX------.   2 ROOT ROOT   16384 AUG 21  2012 LOST+FOUND/
DRWXR-XR-X.   2 ROOT ROOT    4096 DEC  6 12:54 MEDIA/
DRWXR-XR-X.   2 ROOT ROOT    4096 NOV 23 09:07 MISC/
DRWXR-XR-X.   2 ROOT ROOT    4096 SEP 23  2011 MNT/
DRWXR-XR-X.   2 ROOT ROOT    4096 NOV 23 09:07 NET/
DRWXR-XR-X.   3 ROOT ROOT    4096 SEP  3  2012 .NFS4EXPORTS/
LRWXRWXRWX.   1 ROOT ROOT      14 SEP  4  2012 OPT -> /USR/LOCAL/OPT/
DR-XR-XR-X  473 ROOT ROOT       0 DEC 18 13:29 PROC/
-RW-R--R--    1 ROOT ROOT       0 NOV 27 03:27 .READAHEAD_COLLECT
-RW-------.   1 ROOT ROOT 1047112 SEP  4  2012 RESTORESYMTABLE
-RW-------    1 ROOT ROOT    1024 AUG 22  2013 .RND
DR-XR-X---.   4 ROOT ROOT    4096 AUG 30  2013 ROOT/
DRWXR-XR-X.   2 ROOT ROOT    4096 SEP  3  2012 RUTGERS/
DR-XR-XR-X.   2 ROOT ROOT   12288 FEB 26 04:05 SBIN/
DRWXR-XR-X.   2 ROOT ROOT    4096 AUG 21  2012 SELINUX/
DRWXR-XR-X.   2 ROOT ROOT    4096 SEP 23  2011 SRV/
DRWXR-XR-X   13 ROOT ROOT       0 DEC 18 13:29 SYS/
DRWXRWXRWT.   1 ROOT ROOT 6443008 MAR  2 17:23 TMP/
DRWXR-XR-X.   2 ROOT ROOT    4096 SEP 13  2012 UG/
DRWXR-XR-X.  16 ROOT ROOT    4096 SEP  3  2012 USR/
DRWXR-XR-X.  29 ROOT ROOT    4096 OCT 15 11:13 VAR/
DRWXR-XR-X.   4 ROOT ROOT    4096 SEP  3  2012 WWW/
process 3755 exits with 0

14. A pipeline of three commands should work.
IN:
<3: ls -alF  / | grep bin | cat -n
OUT:
process 3820 exits with 0
process 3821 exits with 0
     1  dr-xr-xr-x.   2 root root    4096 Feb 21 04:12 bin/
     2  dr-xr-xr-x.   2 root root   12288 Feb 26 04:05 sbin/
process 3822 exits with 0

15. A pipeline of more than three commands should work.
IN:
<3: ls -alF  / | grep bin | tr a-z 'A-Z' |rev|cat -n
OUT:
process 3951 exits with 0
process 3952 exits with 0
process 3953 exits with 0
process 3954 exits with 0
     1  /NIB 21:40 12 BEF 6904    TOOR TOOR 2   .X-RX-RX-RD
     2  /NIBS 50:40 62 BEF 88221   TOOR TOOR 2   .X-RX-RX-RD
process 3956 exits with 0

16. The cd command should work with one argument
<3: cd /etc
/etc
<3: pwd
/etc
process 4044 exits with 0

17. The cd command should work with no arguments
<3: cd
/.autofs/ilab/ilab_users/joeko
<3: pwd
/.autofs/ilab/ilab_users/joeko
process 4131 exits with 0

18. The cd command should barf when given more than one argument
IN:
<3: cd /etc /home
OUT:
cd: More than one argument is present

19. The exit command should exit with no arguments
<3: exit
-bash-4.1$ echo $?
0

20. The exit command should exit with an argument
<3: exit 123
-bash-4.1$ echo $?
123

21. Check that the shell is able to accept at least 1000 commands.
IN:
(typeset -i i;i=0;while [ $i -lt 1000 ];do echo echo $i;i=i+1;done)|./shell
OUT:
...
990
process 7110 exits with 0
991
process 7111 exits with 0
992
process 7112 exits with 0
993
process 7113 exits with 0
994
process 7114 exits with 0
995
process 7115 exits with 0
996
process 7116 exits with 0
997
process 7117 exits with 0
998
process 7118 exits with 0
999
process 7119 exits with 0