mkdir build

cd build

del main.exe

cd ..

echo Compiling shaders

cd shaders
cmd /C compile.bat

cd ..

echo Compiling

g++ -I./include -I./src -I./src/Engine -o ./build/main.exe ^
    ./src/Engine/Filesystem.cpp ^
    ./src/Engine/Models.cpp ^
    ./src/Engine/VulkanSetup.cpp ^
    ./src/main.cpp ^
    -L./lib ^
    -lglfw3 ^
    -lgdi32 ^
    -lvulkan-1