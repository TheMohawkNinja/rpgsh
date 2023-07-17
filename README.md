# rpgsh

`rpgsh` is an interactive and extensible shell purpose-built for table-top RPGs like Dungeons and Dragons©, Pathfinder©, and more!

## Disclaimer

This project is entirely developed independantly. I am not associated in anyway with Wizards of the Coast, Paizo, or any other company. To any lawyers eyeing me up, I will not be adding information that would negate the need for players to puchase your products (e.g. spell descriptions).

## How to use

When initially launching the program, you will be met with the `rpgsh` shell, which looks similar to the following:

![prompt](https://i.imgur.com/XTBgRw9.png)

The prompt displays the currently loaded character name, along with their heath (**current/max** *(temp)*).

Just like any other shell (sh, bash, cmd, etc.), you interact with `rpgsh` by typing a command follwed by any associated parameters. You then execute the desired command by pressing the Enter key. You may exit the shell at any time by running the "exit" command, or by pressing ctrl+c.

When first launching `rpgsh`, you will see `[INFO]` text appear stating that the root directory for the program has been created at `~/.rpgsh/`. In addition, a placeholder character file `<NO_NAME>.char` is created to give the prompt something to work with until you create a character of your own. (NOTE: This file can be safely deleted if the placeholder character is no longer being used).

Whenever the currently loaded character is modifed by the shell or a program called by the shell, it will create a `.char.bak` file which contains the character information prior to the last save of the character (which *should* be the last command run). This is mainly for instances whereby the player accidentally sets a prompt value to something which would crash the prompt due to a data type mis-match (e.g. setting their current HP to "FULL" instead of "20"). In the event this happens, the prompt will attempt to load the backup. If loading the backup fails, the player will need to open up their character file and attempt to manually edit the file to resolve the issue.

## Variables

`rpgsh` contains two types of variables:
 - Shell variables, prefixed by a `$`
   - General-use variables for things not meant for your character sheet, but that you would still want to keep track of. These are global in scope (that is to say, independant of what character is currently loaded) and are stored in `~/.rpgsh/.shell`
 - Character attributes, prefixed by a `%`
   - Values which will appear on your character sheet (excluding things like hit dice and currency, which have additional mechanics). The scope of these variables are thus limited to the currently loaded character. If you are playing a game which is known to `rpgsh`, many of these attributes will have been generated for you as part of the character creation process. The default `<NO_NAME>` character contains such pre-generated attributes as a placeholder for the prompt.

All variables are lazily evaluated, meaning that they can either store numerical values or strings of characters, with operations handling them accordingly. Currently supported operators are `=`, `+`, `-`, `*`, `/`, `+=`,`-=`, `*=`, `/=`, `++`, `--`. The `+` and `+=` operators will assume addition if both sides are numbers, or concatenation if both sides are strings. An error will be thrown if both sides are not of the same apparent data type. Variables can be used in the shell with any program, as any variable arguments passed to a program will be replaced with the variable's value prior to the program's execution. The only exception to this is when one is modifying the value of a variable, in which case the variable in question is not swapped out for its' value.

Operators and associated values must be entered in a space-delimited format. (e.g. `$four = 2 + 2`, **NOT** `$four=2+2`)

Additionally, the following variables are built-in and should not be directly modified:
 - `$.CURRENT_USER`
   - This is used to load the currently loaded user, including the last loaded user from the previous session. If modified to a value that isn't the name of a `.char` file, it will reset to the default `rpgsh` character name.
 - `%.NAME`
   - This holds the name of the variable to be referenced when displaying the character's name. It should be modified via the `setname` command.

## Dependancies

`g++` should be all that is needed, as nothing outside of the C++ standard library is used.

## Installation instructions

Run `./install.sh` as root.

## Current programs

`rpgsh` comes with a number of core programs built-in for use in its' environment. These are:

 - `banner`	Displays pretty banner art
 - `version`	Prints version info
 - `roll`	A dice-rolling program
 - `setname`	Sets which character attribute is used for displaying the character's name
 - `variables`	Not to be explicitly called, but is used when the user enters a variable as the first parameter in the buffer

All core programs that expect a parameter (excluding `variables`) has help text with examples which can be called with the `-?` or `--help` flags.

## Writing programs for rpgsh

<ins>***Please do not create programs until after 1.0.0 is released, as I want to make sure all intended basic functionality is added first***</ins>

The program *must* adhere to the following standards in order to function correctly when being called by the `rpgsh` shell:
- The name of the executable must start with "rpgsh-"
- The program will be given the following argv parameters when called by `rpgsh`:
  - **[0]** The full path to the the called program
  - **[1]** - **[n-1]**. All additional arguments given by the user for the program
  - **[n]** NULL (per `posix_spawn()` requirements).

The program *should* adhere to the following standards in order to maintain continuity of appearances during runtime:
 - Error, Warning, and Info text should use the standarized `RPGSH_OUTPUT()` call
 - Error text should be follwed by `exit(-1)`
 - Warning text should allow for program continuation
 - Info text should only be used when character attributes, character currency, or character spells change
   - Info text should also state both the old and new values to keep the user well-informed

Newlines are automatically placed before and after the output to reduce on-screen clutter, so this does not need to be added to the individual programs unless desired.

## Known Issues
`roll`
 - High values for `-r|--repeat` when using a custom list may result in program crashing. During limited testing, this behavior was observed when the value was >90
