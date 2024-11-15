mkdir build

del ./build\main.exe

cd shaders
compile.bat

cd ..
g++ -I./include -I./src -o ./build\main.exe ./src\main.cpp -L./lib -lglfw3 -lgdi32 -lvulkan-1
