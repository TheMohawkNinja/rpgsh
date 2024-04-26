#!/bin/bash

bold_white="\e[1m\e[97m"
normal="\e[0m"

lib_dir="/usr/lib/rpgsh/"
root_dir="/home/""$USER""/.rpgsh/"
campaigns_dir="$root_dir""campaigns/"
default_campaign_dir="$campaigns_dir""default/"
templates_dir="$root_dir""templates/"
dice_lists_dir="$root_dir""dice-lists/"

#debug="-g -fsanitize=address"
debug="-g"
fs="-lstdc++fs -std=c++17"

if [[ $EUID -eq 0 ]]; then
	if [ ! -d "$lib_dir" ]; then echo -e "Creating lib directory at:\t$bold_white""$lib_dir$normal"; mkdir "$lib_dir"; fi

	echo "Compiling shared library objects:"
	echo -e "\tsrc/scope.cpp\t\t->\t$bold_white$lib_dir""libscope.so"$normal
	g++ -c -fPIC src/scope.cpp -o "$lib_dir""scope.o"
	g++ -shared "$lib_dir""scope.o" -o "$lib_dir""libscope.so"

	echo -e "Running $bold_white""ldconfig $lib_dir""$normal to pull in the new shared library objects:"
	ldconfig $lib_dir

	echo

	#echo "Compiling applications:"
	#echo -e "\tsrc/main.cpp\t\t->\t$bold_white""/bin/rpgsh"$normal
	#g++ src/main.cpp -L $lib_dir -ldefine -lvar -lfunctions -loutput -lconfig -ldice -lcurrency -lchar $fs $debug -o /bin/rpgsh
	#echo -e "\tsrc/setname.cpp\t\t->\t$bold_white""/bin/rpgsh-setname"$normal
	#g++ src/setname.cpp -L $lib_dir -ldefine -loutput -ldice -lcurrency -lconfig -lchar -lfunctions -lvar $fs $debug -o /bin/rpgsh-setname
	#echo -e "\tsrc/variables.cpp\t->\t$bold_white""/bin/rpgsh-variables"$normal
	#g++ src/variables.cpp -L $lib_dir -ldefine -lvar -lfunctions -loutput -lconfig -ldice -lcurrency -lchar $fs $debug -o /bin/rpgsh-variables
	#echo -e "\tsrc/version.cpp\t\t->\t$bold_white""/bin/rpgsh-version"$normal
	#g++ src/version.cpp $debug -o /bin/rpgsh-version
	#echo -e "\tsrc/banner.cpp\t\t->\t$bold_white""/bin/rpgsh-banner"$normal
	#g++ src/banner.cpp -L $lib_dir -ldefine $debug -o /bin/rpgsh-banner
	#echo -e "\tsrc/roll.cpp\t\t->\t$bold_white""/bin/rpgsh-roll"$normal
	#g++ src/roll.cpp -L $lib_dir -ldefine -lchar -lcurrency -lconfig -lvar -lfunctions -ldice -loutput $fs $debug -o /bin/rpgsh-roll
	#echo -e "\tsrc/list.cpp\t\t->\t$bold_white""/bin/rpgsh-list"$normal
	#g++ src/list.cpp -L $lib_dir -ldefine -lconfig -lchar -ldice -lvar -lfunctions -loutput -lcurrency $debug -o /bin/rpgsh-list
	#echo -e "\tsrc/help.cpp\t\t->\t$bold_white""/bin/rpgsh-help"$normal
	#g++ src/help.cpp -L $lib_dir -ldefine -lconfig -lchar -lcurrency -ldice -lvar -lfunctions -loutput $debug -o /bin/rpgsh-help

	echo "Done!"
fi
