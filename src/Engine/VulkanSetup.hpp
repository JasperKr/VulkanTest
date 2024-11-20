#ifndef VULKANSETUP_HPP
#define VULKANSETUP_HPP

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

#include <Ubo.hpp>
#include <Vertex.hpp>
#include <Filesystem.hpp>
#include "tiny_gltf.h"

#include <Models.hpp>

using namespace tinygltf;

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

namespace VulkanSetup
{
    void init();
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createCommandPool();
    void createDepthResources();
    void createFramebuffers();
    void createTextureImage();
    void createTextureImageView();
    void createTextureSampler();
    void loadModel();
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
    VkShaderModule createShaderModule(const std::vector<char> &code);
    VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);
    bool checkValidationLayerSupport();
    std::vector<const char *> getRequiredExtensions();
    bool isDeviceSuitable(VkPhysicalDevice device);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    VkFormat findDepthFormat();
    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void loadModel();
    void createVertexBuffer();
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkCommandBuffer beginSingleTimeCommands();
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    extern GLFWwindow *window;

    extern VkInstance instance;
    extern VkDebugUtilsMessengerEXT debugMessenger;
    extern VkSurfaceKHR surface;

    extern VkPhysicalDevice physicalDevice;
    extern VkDevice device;

    extern VkQueue graphicsQueue;
    extern VkQueue presentQueue;

    extern VkSwapchainKHR swapChain;
    extern std::vector<VkImage> swapChainImages;
    extern VkFormat swapChainImageFormat;
    extern VkExtent2D swapChainExtent;
    extern std::vector<VkImageView> swapChainImageViews;
    extern std::vector<VkFramebuffer> swapChainFramebuffers;

    extern VkRenderPass renderPass;
    extern VkDescriptorSetLayout descriptorSetLayout;
    extern VkPipelineLayout pipelineLayout;
    extern VkPipeline graphicsPipeline;

    extern VkCommandPool commandPool;

    extern VkImage depthImage;
    extern VkDeviceMemory depthImageMemory;
    extern VkImageView depthImageView;

    extern uint32_t mipLevels;
    extern VkImage textureImage;
    extern VkDeviceMemory textureImageMemory;
    extern VkImageView textureImageView;
    extern VkSampler textureSampler;

    extern std::vector<Vertex> vertices;
    extern std::vector<uint32_t> indices;
    extern VkBuffer vertexBuffer;
    extern VkDeviceMemory vertexBufferMemory;
    extern VkBuffer indexBuffer;
    extern VkDeviceMemory indexBufferMemory;

    extern std::vector<VkBuffer> uniformBuffers;
    extern std::vector<VkDeviceMemory> uniformBuffersMemory;
    extern std::vector<void *> uniformBuffersMapped;

    extern VkDescriptorPool descriptorPool;
    extern std::vector<VkDescriptorSet> descriptorSets;

    extern std::vector<VkCommandBuffer> commandBuffers;

    extern std::vector<VkSemaphore> imageAvailableSemaphores;
    extern std::vector<VkSemaphore> renderFinishedSemaphores;
    extern std::vector<VkFence> inFlightFences;
    extern uint32_t currentFrame;
}

#endif