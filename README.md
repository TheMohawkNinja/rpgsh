# dndsh

dndsh is an interactive and extensible shell purpose-built for the Dungeons and Dragons© tabletop RPG by Wizards of the Coast.

## How to use

When initially launching the program, you will be met with the dndsh shell, which looks similar to the following:

![prompt](https://imgur.com/YY66YSC.png)

The prompt displays the currently loaded character name, along with their heath (**current/max** *(temp)*).

Just like any other shell, you interact with it by sending commands and any associated parameters, executing the desired command by pressing the enter key. You may exit the shell at any time by running the "exit" command.

## Installation instructions

Run `./install.sh` as root

## Writing programs for dndsh

<ins>***Please do not create programs until after 1.0.0 is released, as I want to make sure all intended basic functionality is added first***</ins>

When writing programs which may be called within the `dndsh` shell, the program *must* adhere to the following standard:
- The name of the executable must start with "dndsh-"
- The program will be given the following argv parameters:
  - [0] The full path to the the called program (like any other program being called by a shell)
  - [1] The name of the currently selected character
  - [2] - [(n-1)]. All additional arguments given by the user for the program
  - [n] NULL (per posix_spawn() requirements).

Newlines are automatically placed before and after the output to reduce on-screen clutter, so this does not need to be added to the individual programs unless desired.
