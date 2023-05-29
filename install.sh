echo "Compiling main.cpp..."
g++ main.cpp text.h classes.h -o dndsh
echo "Compiling version.cpp..."
g++ version.cpp -o dndsh-version
echo "Compiling banner.cpp..."
g++ banner.cpp -o dndsh-banner
echo "Compiling roll.cpp..."
g++ roll.cpp -o dndsh-roll
