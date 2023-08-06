#!/bin/bash

bold_white="\e[1m\e[97m"
normal="\e[0m"

root_dir="/home/$USER/.rpgsh/"
characters_dir="$root_dir""characters/"
templates_dir="$root_dir""templates/"
dice_lists_dir="$root_dir""dice-lists/"

echo -e "Compiling:\tsrc/main.cpp\t\t->\t$bold_white""rpgsh"$normal
g++ src/main.cpp -o rpgsh
echo -e "Compiling:\tsrc/setname.cpp\t\t->\t$bold_white""rpgsh-setname"$normal
g++ src/setname.cpp -o rpgsh-setname
echo -e "Compiling:\tsrc/variables.cpp\t->\t$bold_white""rpgsh-variables"$normal
g++ src/variables.cpp -o rpgsh-variables
echo -e "Compiling:\tsrc/version.cpp\t\t->\t$bold_white""rpgsh-version"$normal
g++ src/version.cpp -o rpgsh-version
echo -e "Compiling:\tsrc/banner.cpp\t\t->\t$bold_white""rpgsh-banner"$normal
g++ src/banner.cpp -o rpgsh-banner
echo -e "Compiling:\tsrc/roll.cpp\t\t->\t$bold_white""rpgsh-roll"$normal
g++ src/roll.cpp -o rpgsh-roll

echo

if [ ! -d "$root_dir" ]; then echo -e "Creating root directory at:\t\t$bold_white""$root_dir$normal"; mkdir "$root_dir"; fi
if [ ! -d "$characters_dir" ]; then echo -e "Creating characters directory at:\t$bold_white""$characters_dir$normal"; mkdir "$characters_dir"; fi
if [ ! -d "$templates_dir" ]; then echo -e "Creating templates directory at:\t$bold_white""$templates_dir$normal"; mkdir "$templates_dir"; fi
if [ ! -d "$dice_lists_dir" ]; then echo -e "Creating dice lists directory at:\t$bold_white""$dice_lists_dir$normal"; mkdir "$dice_lists_dir"; fi

echo

echo "Copying templates..."
for i in $(ls templates); do echo -e "\t$bold_white""$i"$normal; cp "templates/$i" "$templates_dir$i"; done

echo

echo "Copying dice lists..."
for i in $(ls dice-lists); do echo -e "\t$bold_white""$i"$normal; cp "dice-lists/$i" "$dice_lists_dir$i"; done

echo

echo "Done!"
