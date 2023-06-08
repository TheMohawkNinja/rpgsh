# dndsh

`dndsh` is an interactive and extensible shell purpose-built for table-top RPGs like Dungeons and Dragons©, Pathfinder©, and more!

## Disclaimer

This project is entirely developed independantly. I am not associated in anyway with Wizards of the Coast, Paizo, or any other company. To any lawyers eyeing me up, I will not be adding information that would negate the need for players to puchase your products (e.g. spell descriptions).

## How to use

When initially launching the program, you will be met with the `dndsh` shell, which looks similar to the following:

![prompt](https://imgur.com/YY66YSC.png)

The prompt displays the currently loaded character name, along with their heath (**current/max** *(temp)*).

Just like any other shell (sh, bash, cmd, etc.), you interact with it by typing a command along with any associated parameters and then executing the desired command by pressing the Enter key. You may exit the shell at any time by running the "exit" command, or by pressing ctrl+c.

## Installation instructions

Run `./install.sh` as root

## Writing programs for dndsh

<ins>***Please do not create programs until after 1.0.0 is released, as I want to make sure all intended basic functionality is added first***</ins>

The program *must* adhere to the following standards in order to function correctly when being called by the `dndsh` shell:
- The name of the executable must start with "dndsh-"
- The program will be given the following argv parameters when called by `dndsh`:
  - [0] The full path to the the called program
  - [1] The name of the currently selected character (for programs which will need to be aware of the currently selected character)
  - [2] - [(n-1)]. All additional arguments given by the user for the program
  - [n] NULL (per `posix_spawn()` requirements).

The program *should* adhere to the following standards in order to maintain continuity of appearances during runtime:
 - Error, Warning, and Info text should use the standarized `DNDSH_OUTPUT()` call
 - Error text should be follwed by `exit()`
 - Warning text should allow for program continuation
 - Info text should only be used when character attributes, character currency, or character spells change
   - Info text should also state both the old and new values to keep the user well-informed

Newlines are automatically placed before and after the output to reduce on-screen clutter, so this does not need to be added to the individual programs unless desired.

## Known Issues

`roll`
 - High values for `-r|--repeat` when using a custom list may result in program crashing. During limited testing, this behavior was observed when the value was >90
