echo "Clearing out old files"
rm -rf build
cd generated
rm cipher-C++.exe
cd ../
mkdir build
cd build
echo "Compiling"
g++ -c -O3 -std=c++23 ../*.cpp
echo "Linking"
g++ -o cipher-C++.exe *.o
chmod +x cipher-C++.exe
mv cipher-C++.exe ../generated
cd ../
echo "Done"
