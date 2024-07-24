cmake_minimum_required(VERSION 3.10)

project(VulkanProject)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

add_executable(VulkanProject main.cpp)

target_include_directories(VulkanProject PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(VulkanProject PRIVATE ${Vulkan_LIBRARIES})