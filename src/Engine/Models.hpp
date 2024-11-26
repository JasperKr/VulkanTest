#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <Vertex.hpp>

#include "tiny_gltf.h"
#include "stb_image.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <filesystem>

namespace Application_Model
{
    extern VkDevice device;
    extern VkDescriptorSetLayout uboDescriptorSetLayout;
    extern VkDescriptorSetLayout textureDescriptorSetLayout;
    void createDescriptorSetLayout();

    class Application_Model
    {
    public:
        bool loadModel(tinygltf::Model &model, const char *filename);
        void createModelDescriptorSets();
        void createImage(stbi_uc *pixels, VkImage &textureImage, VkDeviceMemory &textureImageMemory, uint32_t &mipLevels, int texWidth, int texHeight);
        void createTextureImageView(uint32_t i, uint32_t mipLevels);
        void createVertexBuffers();
        void createIndexBuffers();
        void createTextureSamplers();
        void createDescriptorPools();

        Application_Model(
            std::string Path,
            tinygltf::Model &model);

        std::vector<VkDescriptorSet> uboDescriptorSets;
        std::vector<VkDescriptorSet> textureDescriptorSets;
        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        std::vector<void *> uniformBuffersMapped;

        uint32_t mipLevels;
        std::vector<VkImage> textureImages;
        std::vector<VkDeviceMemory> textureImagesMemory;
        std::vector<VkImageView> textureImageViews;
        std::vector<VkSampler> textureSamplers;

        std::vector<std::vector<Vertex>> Vertices;
        std::vector<std::vector<uint32_t>> Indices;
        std::vector<VkBuffer> vertexBuffers;
        std::vector<VkDeviceMemory> vertexBuffersMemory;
        std::vector<VkBuffer> indexBuffers;
        std::vector<VkDeviceMemory> indexBuffersMemory;

        VkDescriptorPool descriptorPool;

        std::string path;
    };
}

#include <VulkanSetup.hpp>

#endif