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
	echo -e "\tsrc/char.cpp\t\t->\t$bold_white$lib_dir""libchar.so"$normal
	g++ -c -fPIC src/char.cpp -o "$lib_dir""char.o"
	g++ -shared "$lib_dir""char.o" -o "$lib_dir""libchar.so"
	echo -e "\tsrc/config.cpp\t\t->\t$bold_white$lib_dir""libconfig.so"$normal
	g++ -c -fPIC src/config.cpp -o "$lib_dir""config.o"
	g++ -shared "$lib_dir""config.o" -o "$lib_dir""libconfig.so"
	echo -e "\tsrc/currency.cpp\t->\t$bold_white$lib_dir""libcurrency.so"$normal
	g++ -c -fPIC src/currency.cpp -o "$lib_dir""currency.o"
	g++ -shared "$lib_dir""currency.o" -o "$lib_dir""libcurrency.so"
	echo -e "\tsrc/dice.cpp\t\t->\t$bold_white$lib_dir""libdice.so"$normal
	g++ -c -fPIC src/dice.cpp -o "$lib_dir""dice.o"
	g++ -shared "$lib_dir""dice.o" -o "$lib_dir""libdice.so"
	echo -e "\tsrc/define.cpp\t\t->\t$bold_white$lib_dir""libdefine.so"$normal
	g++ -c -fPIC src/define.cpp -o "$lib_dir""define.o"
	g++ -shared "$lib_dir""define.o" -o "$lib_dir""libdefine.so"
	echo -e "\tsrc/functions.cpp\t->\t$bold_white$lib_dir""libfunctions.so"$normal
	g++ --std=c++20 -c -fPIC src/functions.cpp -o "$lib_dir""functions.o"
	g++ -shared "$lib_dir""functions.o" -o "$lib_dir""libfunctions.so"
	echo -e "\tsrc/output.cpp\t\t->\t$bold_white$lib_dir""liboutput.so"$normal
	g++ -c -fPIC src/output.cpp -o "$lib_dir""output.o"
	g++ -shared "$lib_dir""output.o" -o "$lib_dir""liboutput.so"
	echo -e "\tsrc/scope.cpp\t\t->\t$bold_white$lib_dir""libscope.so"$normal
	g++ -c -fPIC src/scope.cpp -o "$lib_dir""scope.o"
	g++ -shared "$lib_dir""scope.o" -o "$lib_dir""libscope.so"
	echo -e "\tsrc/var.cpp\t\t->\t$bold_white$lib_dir""libvar.so"$normal
	g++ -c -fPIC src/var.cpp -o "$lib_dir""var.o"
	g++ -shared "$lib_dir""var.o" -o "$lib_dir""libvar.so"

	echo

	echo -e "Running $bold_white""ldconfig $lib_dir""$normal to pull in the new shared library objects:"
	ldconfig $lib_dir

	echo

	echo "Compiling applications:"
	echo -e "\tsrc/main.cpp\t\t->\t$bold_white""/bin/rpgsh"$normal
	g++ src/main.cpp -L $lib_dir -ldefine -lscope -lvar -lfunctions -loutput -lconfig -ldice -lcurrency -lchar $fs $debug -o /bin/rpgsh
	echo -e "\tsrc/setname.cpp\t\t->\t$bold_white""/bin/rpgsh-setname"$normal
	g++ src/setname.cpp -L $lib_dir -ldefine -loutput -ldice -lcurrency -lconfig -lchar -lfunctions -lvar -lscope $fs $debug -o /bin/rpgsh-setname
	echo -e "\tsrc/variables.cpp\t->\t$bold_white""/bin/rpgsh-variables"$normal
	g++ src/variables.cpp -L $lib_dir -ldefine -lvar -lfunctions -loutput -lconfig -ldice -lcurrency -lchar -lscope $fs $debug -o /bin/rpgsh-variables
	echo -e "\tsrc/version.cpp\t\t->\t$bold_white""/bin/rpgsh-version"$normal
	g++ src/version.cpp $debug -o /bin/rpgsh-version
	echo -e "\tsrc/banner.cpp\t\t->\t$bold_white""/bin/rpgsh-banner"$normal
	g++ src/banner.cpp -L $lib_dir -ldefine $debug -o /bin/rpgsh-banner
	echo -e "\tsrc/roll.cpp\t\t->\t$bold_white""/bin/rpgsh-roll"$normal
	g++ src/roll.cpp -L $lib_dir -ldefine -lchar -lcurrency -lconfig -lvar -lfunctions -ldice -loutput -lscope $fs $debug -o /bin/rpgsh-roll
	echo -e "\tsrc/list.cpp\t\t->\t$bold_white""/bin/rpgsh-list"$normal
	g++ src/list.cpp -L $lib_dir -ldefine -lconfig -lchar -ldice -lvar -lfunctions -loutput -lcurrency -lscope $debug -o /bin/rpgsh-list
	echo -e "\tsrc/help.cpp\t\t->\t$bold_white""/bin/rpgsh-help"$normal
	g++ src/help.cpp -L $lib_dir -ldefine -lconfig -lchar -lcurrency -ldice -lvar -lfunctions -loutput -lscope $debug -o /bin/rpgsh-help

	echo "Done!"
else
	if [ ! -d "$root_dir" ]; then echo -e "Creating root directory at:\t\t$bold_white""$root_dir$normal"; mkdir "$root_dir"; fi
	if [ ! -d "$campaigns_dir" ]; then echo -e "Creating campaigns directory at:\t$bold_white""$campaigns_dir$normal"; mkdir "$campaigns_dir"; fi
	if [ ! -d "$default_campaign_dir" ]; then echo -e "Creating default campaign at \t\t$bold_white""$default_campaign_dir$normal";
		mkdir "$default_campaign_dir"; mkdir "$default_campaign_dir""characters/"; touch "$default_campaign_dir"".vars"; fi
	if [ ! -d "$templates_dir" ]; then echo -e "Creating templates directory at:\t$bold_white""$templates_dir$normal"; mkdir "$templates_dir"; fi
	if [ ! -d "$dice_lists_dir" ]; then echo -e "Creating dice lists directory at:\t$bold_white""$dice_lists_dir$normal"; mkdir "$dice_lists_dir"; fi

	echo

	echo "Copying templates:"
	for i in $(ls templates); do echo -e "\t$bold_white""$i"$normal; cp "templates/$i" "$templates_dir$i"; done

	echo

	echo "Copying dice lists:"
	for i in $(ls dice-lists); do echo -e "\t$bold_white""$i"$normal; cp "dice-lists/$i" "$dice_lists_dir$i"; done

	echo

	echo "Done!"
fi
