
mkdir build

del ./build/main

g++ -I./include -o ./build/main ./src/main.cpp -L./lib -lglfw3 -lgdi32 -lvulkan-1