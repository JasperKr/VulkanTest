#ifndef MAIN_HPP
#define MAIN_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <array>
#include <optional>
#include <set>
#include <unordered_map>

#include <Filesystem.hpp>

#include <VulkanSetup.hpp>

namespace Main
{
    void run();
    void mainLoop();
    void cleanupSwapChain();
    void cleanup();
    void drawFrame();
    void recreateSwapChain();
    void updateUniformBuffer(uint32_t currentImage);

    bool framebufferResized = false;
};

#endif