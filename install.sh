echo "Compiling main.cpp..."
g++ main.cpp -lstdc++fs -std=c++17 -o rpgsh
echo "Compiling variables.cpp..."
g++ variables.cpp -lstdc++fs -std=c++17 -o rpgsh-variables
echo "Compiling version.cpp..."
g++ version.cpp -o rpgsh-version
echo "Compiling banner.cpp..."
g++ banner.cpp -o rpgsh-banner
echo "Compiling roll.cpp..."
g++ roll.cpp -lstdc++fs -std=c++17 -o rpgsh-roll
