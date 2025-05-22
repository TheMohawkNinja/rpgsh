#!/bin/bash

bold_white="\e[1m\e[97m"
normal="\e[0m"

lib_dir="/usr/lib/rpgsh/"
root_dir="/home/""$USER""/.rpgsh/"
campaigns_dir="$root_dir""campaigns/"
default_campaign_dir="$campaigns_dir""default/"
templates_dir="$root_dir""templates/"
dice_lists_dir="$root_dir""dice-lists/"

flags="-Wall -Wextra -Wpedantic"
libs="-loutput -lpretty -lvariable -lstring -lfunctions -ldefine -lscope -lvar -lconfig -ldice -lcurrency -lcolors"

#debug="-g -fsanitize=address"
debug="-g"
fs="-lstdc++fs -std=c++17"

if [[ $EUID -eq 0 ]]; then
	if [ ! -d "$lib_dir" ]; then echo -e "Creating lib directory at:\t$bold_white""$lib_dir$normal"; mkdir "$lib_dir"; fi

	echo "Compiling shared library objects:"
	echo -e "\tsrc/chkflag.cpp\t\t->\t$bold_white$lib_dir""libchkflag.so"$normal
	g++ $flags $debug $fs -c -fPIC src/chkflag.cpp -o "$lib_dir""chkflag.o"
	g++ $flags -shared "$lib_dir""chkflag.o" -o "$lib_dir""libchkflag.so"
	echo -e "\tsrc/colors.cpp\t\t->\t$bold_white$lib_dir""libcolors.so"$normal
	g++ $flags $debug $fs -c -fPIC src/colors.cpp -o "$lib_dir""colors.o"
	g++ $flags -shared "$lib_dir""colors.o" -o "$lib_dir""libcolors.so"
	echo -e "\tsrc/config.cpp\t\t->\t$bold_white$lib_dir""libconfig.so"$normal
	g++ $flags $debug $fs -c -fPIC src/config.cpp -o "$lib_dir""config.o"
	g++ $flags -shared "$lib_dir""config.o" -o "$lib_dir""libconfig.so"
	echo -e "\tsrc/currency.cpp\t->\t$bold_white$lib_dir""libcurrency.so"$normal
	g++ $flags $debug $fs -c -fPIC src/currency.cpp -o "$lib_dir""currency.o"
	g++ $flags -shared "$lib_dir""currency.o" -o "$lib_dir""libcurrency.so"
	echo -e "\tsrc/dice.cpp\t\t->\t$bold_white$lib_dir""libdice.so"$normal
	g++ $flags $debug $fs -c -fPIC src/dice.cpp -o "$lib_dir""dice.o"
	g++ $flags -shared "$lib_dir""dice.o" -o "$lib_dir""libdice.so"
	echo -e "\tsrc/define.cpp\t\t->\t$bold_white$lib_dir""libdefine.so"$normal
	g++ $flags $debug $fs -c -fPIC src/define.cpp -o "$lib_dir""define.o"
	g++ $flags -shared "$lib_dir""define.o" -o "$lib_dir""libdefine.so"
	echo -e "\tsrc/functions.cpp\t->\t$bold_white$lib_dir""libfunctions.so"$normal
	g++ $flags $debug -std=c++20 -c -fPIC src/functions.cpp -o "$lib_dir""functions.o"
	g++ $flags -shared "$lib_dir""functions.o" -o "$lib_dir""libfunctions.so"
	echo -e "\tsrc/output.cpp\t\t->\t$bold_white$lib_dir""liboutput.so"$normal
	g++ $flags $debug -c -fPIC src/output.cpp -o "$lib_dir""output.o"
	g++ $flags -shared "$lib_dir""output.o" -o "$lib_dir""liboutput.so"
	echo -e "\tsrc/pretty.cpp\t\t->\t$bold_white$lib_dir""libpretty.so"$normal
	g++ $flags $debug -c -fPIC src/pretty.cpp -o "$lib_dir""pretty.o"
	g++ $flags -shared "$lib_dir""pretty.o" -o "$lib_dir""libpretty.so"
	echo -e "\tsrc/scope.cpp\t\t->\t$bold_white$lib_dir""libscope.so"$normal
	g++ $flags $debug $fs -c -fPIC src/scope.cpp -o "$lib_dir""scope.o"
	g++ $flags -shared "$lib_dir""scope.o" -o "$lib_dir""libscope.so"
	echo -e "\tsrc/string.cpp\t\t->\t$bold_white$lib_dir""libstring.so"$normal
	g++ $flags $debug -c -fPIC src/string.cpp -o "$lib_dir""string.o"
	g++ $flags -shared "$lib_dir""string.o" -o "$lib_dir""libstring.so"
	echo -e "\tsrc/var.cpp\t\t->\t$bold_white$lib_dir""libvar.so"$normal
	g++ $flags $debug -c -fPIC src/var.cpp -o "$lib_dir""var.o"
	g++ $flags -shared "$lib_dir""var.o" -o "$lib_dir""libvar.so"
	echo -e "\tsrc/variable.cpp\t->\t$bold_white$lib_dir""libvariable.so"$normal
	g++ $flags $debug -c -fPIC src/variable.cpp -o "$lib_dir""variable.o"
	g++ $flags -shared "$lib_dir""variable.o" -o "$lib_dir""libvariable.so"

	echo

	echo -e "Running $bold_white""ldconfig $lib_dir""$normal to pull in the new shared library objects:"
	ldconfig $lib_dir

	echo

	echo "Compiling applications:"
	echo -e "\tsrc/main.cpp\t\t->\t$bold_white""/bin/rpgsh"$normal
	g++ $flags src/main.cpp -L $lib_dir -lchkflag -lcolors -lconfig -lcurrency -ldefine -ldice -lfunctions -loutput -lpretty -lscope -lstring -lvar -lvariable $fs $debug -o /bin/rpgsh
	echo -e "\tsrc/autorun.cpp\t\t->\t$bold_white""/bin/rpgsh-autorun"$normal
	g++ $flags src/autorun.cpp -L $lib_dir -lchkflag -lconfig -lcurrency -ldefine -ldice -lfunctions -loutput -lscope -lstring -lvar -lvariable $fs $debug -o /bin/rpgsh-autorun
	echo -e "\tsrc/banner.cpp\t\t->\t$bold_white""/bin/rpgsh-banner"$normal
	g++ $flags src/banner.cpp -L $lib_dir -lchkflag -loutput $debug -o /bin/rpgsh-banner
	echo -e "\tsrc/clear.cpp\t\t->\t$bold_white""/bin/rpgsh-clear"$normal
	g++ $flags src/clear.cpp -L $lib_dir -lchkflag -loutput $debug -o /bin/rpgsh-clear
	echo -e "\tsrc/copy.cpp\t\t->\t$bold_white""/bin/rpgsh-copy"$normal
	g++ $flags src/copy.cpp -L $lib_dir -lchkflag -lconfig -lcurrency -ldefine -ldice -lfunctions -loutput -lscope -lstring -lvar -lvariable $fs $debug -o /bin/rpgsh-copy
	echo -e "\tsrc/del.cpp\t\t->\t$bold_white""/bin/rpgsh-del"$normal
	g++ $flags src/del.cpp -L $lib_dir -lchkflag -lconfig -lcurrency -ldefine -ldice -lfunctions -loutput -lscope -lstring -lvar -lvariable $fs $debug -o /bin/rpgsh-del
	echo -e "\tsrc/edit.cpp\t\t->\t$bold_white""/bin/rpgsh-edit"$normal
	g++ $flags src/edit.cpp -L $lib_dir -lchkflag -lcolors -lconfig -lcurrency -ldefine -ldice -lfunctions -loutput -lpretty -lscope -lstring -lvar -lvariable $fs $debug -o /bin/rpgsh-edit
	echo -e "\tsrc/eval.cpp\t\t->\t$bold_white""/bin/rpgsh-eval"$normal
	g++ $flags src/eval.cpp -L $lib_dir -lchkflag -lconfig -lcurrency -ldefine -ldice -lfunctions -loutput -lscope -lstring -lvar -lvariable $fs $debug -o /bin/rpgsh-eval
	echo -e "\tsrc/help.cpp\t\t->\t$bold_white""/bin/rpgsh-help"$normal
	g++ $flags src/help.cpp -L $lib_dir $libs $fs $debug -o /bin/rpgsh-help
	echo -e "\tsrc/history.cpp\t\t->\t$bold_white""/bin/rpgsh-history"$normal
	g++ $flags src/history.cpp -L $lib_dir $libs $fs $debug -o /bin/rpgsh-history
	echo -e "\tsrc/list.cpp\t\t->\t$bold_white""/bin/rpgsh-list"$normal
	g++ $flags src/list.cpp -L $lib_dir $libs $fs $debug -o /bin/rpgsh-list
	echo -e "\tsrc/load.cpp\t\t->\t$bold_white""/bin/rpgsh-load"$normal
	g++ $flags src/load.cpp -L $lib_dir $libs $fs $debug -o /bin/rpgsh-load
	echo -e "\tsrc/new.cpp\t\t->\t$bold_white""/bin/rpgsh-new"$normal
	g++ $flags src/new.cpp -L $lib_dir $libs $fs $debug -o /bin/rpgsh-new
	echo -e "\tsrc/print.cpp\t\t->\t$bold_white""/bin/rpgsh-print"$normal
	g++ $flags src/print.cpp -L $lib_dir $libs $fs $debug -o /bin/rpgsh-print
	echo -e "\tsrc/roll.cpp\t\t->\t$bold_white""/bin/rpgsh-roll"$normal
	g++ $flags src/roll.cpp -L $lib_dir $libs $fs $debug -o /bin/rpgsh-roll
	echo -e "\tsrc/setname.cpp\t\t->\t$bold_white""/bin/rpgsh-setname"$normal
	g++ $flags src/setname.cpp -L $lib_dir $libs $fs $debug -o /bin/rpgsh-setname
	echo -e "\tsrc/valueof.cpp\t\t->\t$bold_white""/bin/rpgsh-valueof"$normal
	g++ $flags src/valueof.cpp -L $lib_dir $libs $fs $debug -o /bin/rpgsh-valueof
	echo -e "\tsrc/version.cpp\t\t->\t$bold_white""/bin/rpgsh-version"$normal
	g++ $flags src/version.cpp -L $lib_dir $libs $fs $debug -o /bin/rpgsh-version

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
