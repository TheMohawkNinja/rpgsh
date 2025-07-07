<p align="center">
<img src="banner.png" width="500" height="350" border="10"/>
</p>

# rpgsh

`rpgsh` is an interactive and extensible shell purpose-built for table-top RPGs like Dungeons and Dragons®, Pathfinder®, and more!

Features include:
- Tab completion

  <img src="tab_completion.gif" width="320" height="180" border="10"/>
- Four different variable data types: Var, Dice, Currency, and Wallet
	- Support for common operations with similar and differing data types
- Templates to allow support for new games to be added easily and by the user
- Automation via scripting

## Disclaimer

This project is entirely developed independantly. I am not associated in anyway with Wizards of the Coast, Paizo, or any other game development company.

## Install

Run `sudo ./install.sh; ./install.sh`

## How to Use

Once the project is installed, run the program with the `rpgsh` command.

See `docs/rpgsh-userguide.pdf` for detailed instructions on how everything works.

## Note To Developers

This project is designed to be modular, with each program acting as its' own standalone binary. Anyone is more than welcome to create their own programs for use with rpgsh.
