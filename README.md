# Custom Unix-like Shell Implementation

## Overview
A C-based command-line shell supporting basic Unix operations including piping, redirection, and compound commands. Designed for educational purposes to demonstrate core shell concepts.

## Features
- **Command Execution**
  - Basic command execution (`ls`, `pwd`, `echo`, etc.)
  - Background process support (using `&`)
- **I/O Redirection**
  - Output truncation (`>`)
  - Output appending (`>>`)
- **Piping** (`|`)
- **Compound Commands**
  - Sequential execution (`;`)
  - Conditional execution (`&&`)
- **Interactive Interface**
  - Dynamic prompt with user/host/cwd info
  - Clean screen initialization
  - Built-in `exit` command
 ## Usage Examples
 ```
 # Basic command
[user@host:~/projects]$ ls -l

# Output redirection
[user@host:~/projects]$ echo "Hello" > output.txt

# Pipe commands
[user@host:~/projects]$ ls | grep .c

# Compound commands
[user@host:~/projects]$ make && ./program
[user@host:~/projects]$ git status; git log

# Append output
[user@host:~/projects]$ date >> log.txt
```
