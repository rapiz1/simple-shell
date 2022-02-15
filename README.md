# simple-shell

A simple shell implementation with built-in commands and coreutils.

## Motivation

I did the project for learning Linux programming years ago. I think making it public may help those who's interested in making a shell and other command line gadgets.

This project did a little more than what you can generally find in a "create your own shell" tutorial.

## Features

### Built-in Commands

Built-in commands are placed in `builtin/`.

Supported built-in are:

- cd
- echo
- export
- kill
- pwd

coreutils are placed in `utils/`.

### coreutils

Supported coreutils are:

- cp
- mv
- rm
- ls
- touch
- mkdir
- ln
- cat
- xargs
- more

### Redirection and Pipes

`>`, `<`, `|` are supported.

### Background Jobs

`&` is supported.

### Configuration

The shell will read and execute `./.rshrc` at startup.
