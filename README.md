# rpgsh

`rpgsh` is an interactive and extensible shell purpose-built for table-top RPGs like Dungeons and Dragons©, Pathfinder©, and more!

## Disclaimer

This project is entirely developed independantly. I am not associated in anyway with Wizards of the Coast, Paizo, or any other company. To any lawyers eyeing me up, I will not be adding information that would negate the need for players to puchase your products (e.g. spell descriptions).

## How to use

When initially launching the program, you will be met with the `rpgsh` shell, which looks similar to the following:

![prompt](https://i.imgur.com/XTBgRw9.png)

The prompt displays the currently loaded character name, along with their heath (**current/max** *(temp)*).

Just like any other shell (sh, bash, cmd, etc.), you interact with 'rpgsh' by typing a command follwed by any associated parameters. You then execute the desired command by pressing the Enter key. You may exit the shell at any time by running the "exit" command, or by pressing ctrl+c.

When first launching `rpgsh`, you will see `[INFO]` text appear stating that the root directory for the program has been created at `~/.rpgsh/`. In addition, a placeholder character `<NO_NAME>.char` is created to give the prompt something to work with until you create a character of your own. This file can be safely deleted if the placeholder character is no longer being used. Whenever the currently loaded character is modifed by the shell or a program called by the shell, it will create a `.char.bak` file which contains the character information prior to the last save of the character (which *should* be the last command run). This is mainly for instances whereby the player accidentally sets a prompt value to something non-sensical (e.g. setting their current HP to "FULL" instead of "20"), as this would cause the prompt to crash. In the event this happens, the prompt will attempt to load the backup. If loading the backup fails, the player will need to open up their character file and attempt to manually edit the file to resolve the issue.

## Installation instructions

Run `./install.sh` as root

## Writing programs for rpgsh

<ins>***Please do not create programs until after 1.0.0 is released, as I want to make sure all intended basic functionality is added first***</ins>

The program *must* adhere to the following standards in order to function correctly when being called by the `rpgsh` shell:
- The name of the executable must start with "rpgsh-"
- The program will be given the following argv parameters when called by `rpgsh`:
  - **[0]** The full path to the the called program
  - **[1]** The name of the currently selected character (for programs which will need to be aware of the currently selected character)
  - **[2]** - **[(n-1)]**. All additional arguments given by the user for the program
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
