echo "Compiling main.cpp..."
g++ main.cpp text.h classes.h -lstdc++fs -std=c++17 -o dndsh
echo "Compiling variables.cpp..."
g++ variables.cpp text.h classes.h -lstdc++fs -std=c++17 -o dndsh-variables
echo "Compiling version.cpp..."
g++ version.cpp -o dndsh-version
echo "Compiling banner.cpp..."
g++ banner.cpp -o dndsh-banner
echo "Compiling roll.cpp..."
g++ roll.cpp text.h classes.h -lstdc++fs -std=c++17 -o dndsh-roll
