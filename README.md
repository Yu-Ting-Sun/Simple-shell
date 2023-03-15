# Simple-shell
## This is a simple shell which can do the following things.
1. Run built-in command
2. Run single process command
3. Run two-process pipelines
4. Handle input and output redirection
5. Execute commands in the background
6. Run multi-pipelines

## The built-in command
1. help: **Print how to use the built-in functions.**
2. cd: **Change the working directory.**
3. echo: **Print a line of text to stdout** If “-n” flag is set, “echo” will not output the trailing newline.
4. exit: **Terminate your shell.** This command will not run in the backgound. Print some goodbye-message before terminate.
5. record: **Your shell will always record the last-16 commands that user used in the shell.** When user type the “record” command, the shell will print the last-16 commands to stdout, including “record” itself. The biggest number indicate the latest command being used (i.e., “record” itself). If the command is not a legal command (e.g., “recorf” in p.17), that command will still be recorded. The only exception is the “replay” command, which itself will not be recorded.
6. replay: **Re-execute the command that is listed in record.** User should use the “replay” command with a number. If the number is in legal range, the shell should re-execute the command according to the number listed in the “record” command. If “replay” is used with wrong argument (not a legal number), your shell will output an error message: “replay: wrong args”. No other command will be executed, and “record” will not update. IMPORTANT: The “replay” command itself will not be recorded in the shell. Instead, the command which is actually “replayed” is recorded. If “replay” is used with pipeline, the command being “replayed” will be recorded along with other commands in the pipeline. For example, if “replay 3” will run “echo 300”, “replay 3 | grep 3” will run “echo 300 | grep 3”, and “echo 300 | grep 3” will be recorded. 
7. mypid: **Show the related pids about the process** Depend on the flag used with the command, mypid will list the related process’ pid(s). -i: print process’ pid, which execute “mypid”. (ignore [number]) -p: print process’ parent’s pid (i.e., who has child [number]) -c: print process’ child’s pid (i.e., whose parent is [number]) You must implement this command through parsing information in the /proc directory. (except for implementing the “-i” option).

