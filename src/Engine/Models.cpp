#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// #define TINYGLTF_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
// #define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

#include <Models.hpp>

using namespace VulkanSetup;

namespace Application_Model
{
    VkDevice device;

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
    VkDescriptorSetLayout uboDescriptorSetLayout;
    VkDescriptorSetLayout textureDescriptorSetLayout;

    std::string path;

    bool Application_Model::loadModel(tinygltf::Model &model, const char *filename)
    {
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
        if (!warn.empty())
        {
            std::cout << "WARN: " << warn << std::endl;
        }

        if (!err.empty())
        {
            std::cout << "ERR: " << err << std::endl;
        }

        if (!res)
            std::cout << "Failed to load glTF: " << filename << std::endl;
        else
            std::cout << "Loaded glTF: " << filename << std::endl;

        return res;
    }

    void createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &uboDescriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 0;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo textureLayoutInfo{};
        textureLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        textureLayoutInfo.bindingCount = 1;
        textureLayoutInfo.pBindings = &samplerLayoutBinding;

        if (vkCreateDescriptorSetLayout(device, &textureLayoutInfo, nullptr, &textureDescriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture descriptor set layout!");
        }
    }

    void Application_Model::createDescriptorPools()
    {
        uint64_t allocationSize = MAX_FRAMES_IN_FLIGHT + textureImages.size();

        // UBO pool size
        VkDescriptorPoolSize uboPoolSize{};
        uboPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboPoolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT; // One UBO per frame.

        // Combined image sampler pool size
        VkDescriptorPoolSize samplerPoolSize{};
        samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerPoolSize.descriptorCount = textureImages.size();

        std::vector<VkDescriptorPoolSize> poolSizes = {uboPoolSize, samplerPoolSize};

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = allocationSize; // 1 UBO + textures.

        std::cout << "Max sets: " << poolInfo.maxSets << std::endl;

        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }

        std::cout << "descriptor pool created" << std::endl;
        std::cout << poolSizes.size() << " descriptor pool sizes" << std::endl;
    }

    void Application_Model::createModelDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> uboLayouts = {MAX_FRAMES_IN_FLIGHT, uboDescriptorSetLayout};

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        allocInfo.pSetLayouts = uboLayouts.data();

        uboDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

        std::cout << "allocating descriptor sets " << MAX_FRAMES_IN_FLIGHT << std::endl;

        VkResult result = vkAllocateDescriptorSets(device, &allocInfo, uboDescriptorSets.data());

        if (result != VK_SUCCESS)
        {
            std::cout << "result = " << result << std::endl;
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        std::cout << "descriptor sets allocated" << std::endl;

        std::vector<VkDescriptorSetLayout> textureLayouts = {textureImages.size(), textureDescriptorSetLayout};

        allocInfo.descriptorSetCount = textureImages.size();
        allocInfo.pSetLayouts = textureLayouts.data();

        textureDescriptorSets.resize(textureImages.size());

        std::cout << "allocating texture descriptor sets " << textureImages.size() << std::endl;

        result = vkAllocateDescriptorSets(device, &allocInfo, textureDescriptorSets.data());

        if (result != VK_SUCCESS)
        {
            std::cout << "result = " << result << std::endl;
            throw std::runtime_error("failed to allocate texture descriptor sets!");
        }

        std::cout << "allocating model matrix ubo descriptor set " << std::endl;
        std::cout << "uniformBuffers.size() = " << uniformBuffers.size() << std::endl;

        std::vector<VkWriteDescriptorSet> descriptorWrites;
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(ModelUBO);

            VkWriteDescriptorSet descriptorWrite{};

            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = uboDescriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;

            descriptorWrites.push_back(descriptorWrite);
        }
        vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

        std::cout << "model matrix ubo descriptor set allocated" << std::endl;

        // std::vector<VkWriteDescriptorSet> writeDescriptorSetsImage;

        for (size_t j = 0; j < textureImages.size(); j++)
        {
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            imageInfo.imageView = textureImageViews[j];
            imageInfo.sampler = textureSamplers[j];

            VkWriteDescriptorSet descriptorWrite{};

            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = textureDescriptorSets[j];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
            // writeDescriptorSetsImage.push_back(descriptorWrite);
        }

        std::cout << "texture image descriptor sets allocated" << std::endl;
    }

    void Application_Model::createImage(stbi_uc *pixels, VkImage &textureImage, VkDeviceMemory &textureImageMemory, uint32_t &mipLevels, int texWidth, int texHeight)
    {
        VkDeviceSize imageSize = texWidth * texHeight * 4;
        mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void *data;
        vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(device, stagingBufferMemory);

        VulkanSetup::createImage(texWidth, texHeight, mipLevels, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

        transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
        copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
        // transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);

        generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);
    }

    void Application_Model::createTextureImageView(uint32_t i, uint32_t mipLevels)
    {
        textureImageViews.push_back(createImageView(textureImages[i], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels));
    }

    void Application_Model::createVertexBuffers()
    {
        std::cout << "Vertices.size() = " << Vertices.size() << std::endl;
        for (uint64_t i = 0; i < Vertices.size(); i++)
        {
            std::vector<Vertex> vertices = Vertices[i];
            VkBuffer vertexBuffer = VkBuffer();
            VkDeviceMemory vertexBufferMemory = VkDeviceMemory();

            VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            void *data;
            vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, vertices.data(), (size_t)bufferSize);
            vkUnmapMemory(device, stagingBufferMemory);

            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

            copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);

            vertexBuffers.push_back(vertexBuffer);
            vertexBuffersMemory.push_back(vertexBufferMemory);
        }
    }

    void Application_Model::createIndexBuffers()
    {
        std::cout << "Indices.size() = " << Indices.size() << std::endl;
        for (uint64_t i = 0; i < Indices.size(); i++)
        {
            std::vector<uint32_t> indices = Indices[i];
            VkBuffer indexBuffer = VkBuffer();
            VkDeviceMemory indexBufferMemory = VkDeviceMemory();

            VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            void *data;
            vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, indices.data(), (size_t)bufferSize);
            vkUnmapMemory(device, stagingBufferMemory);

            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

            copyBuffer(stagingBuffer, indexBuffer, bufferSize);

            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);

            indexBuffers.push_back(indexBuffer);
            indexBuffersMemory.push_back(indexBufferMemory);
        }
    }

    void Application_Model::createTextureSamplers()
    {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        textureSamplers.resize(textureImages.size());

        for (uint64_t i = 0; i < textureImages.size(); i++)
        {
            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.anisotropyEnable = VK_TRUE;
            samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
            samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = static_cast<float>(mipLevels);
            samplerInfo.mipLodBias = 0.0f;

            if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSamplers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create texture sampler!");
            }
        }
    }

    Application_Model::Application_Model(
        std::string Path,
        tinygltf::Model &model)

    {
        bool res = loadModel(model, Path.c_str());

        path = Path;

        std::string directory = std::filesystem::path(Path).parent_path().string() + "/";

        if (!res)
        {
            throw std::runtime_error("Failed to load model!");
        }

        std::vector<stbi_uc> images;

        uint32_t texIndex = 0;

        for (const auto &mesh : model.meshes)
        {
            for (const auto &primitive : mesh.primitives)
            {
                std::vector<Vertex> vertices;
                std::vector<uint32_t> indices;

                // load the vertex data
                const auto &positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
                const auto &positionBufferView = model.bufferViews[positionAccessor.bufferView];
                const auto &positionBuffer = model.buffers[positionBufferView.buffer];

                const auto &uvAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                const auto &uvBufferView = model.bufferViews[uvAccessor.bufferView];
                const auto &uvBuffer = model.buffers[uvBufferView.buffer];

                // load the index data
                const auto &indexAccessor = model.accessors[primitive.indices];
                const auto &indexBufferView = model.bufferViews[indexAccessor.bufferView];
                const auto &indexBuffer = model.buffers[indexBufferView.buffer];

                // load the vertices
                vertices.reserve(positionAccessor.count);
                for (size_t i = 0; i < positionAccessor.count; i++)
                {
                    Vertex vertex{};

                    const float *position = reinterpret_cast<const float *>(
                        &positionBuffer.data[positionAccessor.byteOffset + positionBufferView.byteOffset + i * positionAccessor.ByteStride(positionBufferView)]);
                    vertex.pos = glm::vec3(position[0], position[1], position[2]);
                    vertex.color = glm::vec3(rand() / static_cast<float>(RAND_MAX), rand() / static_cast<float>(RAND_MAX), rand() / static_cast<float>(RAND_MAX));

                    const float *uv = reinterpret_cast<const float *>(
                        &uvBuffer.data[uvAccessor.byteOffset + uvBufferView.byteOffset + i * uvAccessor.ByteStride(uvBufferView)]);
                    vertex.texCoord = glm::vec2(uv[0], uv[1]);

                    vertices.push_back(vertex);
                }

                // load the indices
                indices.reserve(indexAccessor.count);
                for (size_t i = 0; i < indexAccessor.count; i++)
                {
                    uint32_t index = 0;

                    if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT)
                    {
                        index = reinterpret_cast<const uint32_t *>(
                            &indexBuffer.data[indexAccessor.byteOffset + indexBufferView.byteOffset])[i];
                    }
                    else if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT)
                    {
                        index = reinterpret_cast<const uint16_t *>(
                            &indexBuffer.data[indexAccessor.byteOffset + indexBufferView.byteOffset])[i];
                    }
                    else if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE)
                    {
                        index = reinterpret_cast<const uint8_t *>(
                            &indexBuffer.data[indexAccessor.byteOffset + indexBufferView.byteOffset])[i];
                    }

                    indices.push_back(index);
                }

                Vertices.push_back(std::move(vertices));
                Indices.push_back(std::move(indices));

                // load the albedo texture

                if (primitive.material >= 0)
                {
                    const auto &material = model.materials[primitive.material];
                    if (material.values.find("baseColorTexture") != material.values.end())
                    {
                        const int textureIndex = material.values.at("baseColorTexture").TextureIndex();
                        const auto &texture = model.textures[textureIndex];
                        const auto &image = model.images[texture.source];

                        // load the image data from the buffer view
                        int width, height, channels;
                        stbi_uc *imageData = stbi_load(
                            (directory + image.uri).c_str(),
                            &width,
                            &height,
                            &channels,
                            STBI_rgb_alpha);

                        if (!imageData)
                        {
                            throw std::runtime_error("failed to load texture image!");
                        }

                        textureImages.resize(texIndex + 1);
                        textureImagesMemory.resize(texIndex + 1);

                        createImage(imageData, textureImages[texIndex], textureImagesMemory[texIndex], mipLevels, width, height);

                        stbi_image_free(imageData);

                        this->createTextureImageView(texIndex, mipLevels);

                        texIndex++;
                    }
                }
            }
        }

        if (textureImages.size() == 0)
        {
            int texWidth = 1;
            int texHeight = 1;
            int texChannels = 4;
            stbi_uc *pixels = new stbi_uc[texWidth * texHeight * texChannels];
            for (int i = 0; i < texWidth * texHeight * texChannels; i++)
            {
                pixels[i] = 255;
            }

            textureImages.resize(1);
            textureImagesMemory.resize(1);

            createImage(pixels, textureImages[0], textureImagesMemory[0], mipLevels, texWidth, texHeight);

            // std::cout << "Freeing pixels" << std::endl;

            stbi_image_free(pixels);

            textureImageViews.resize(1);

            this->createTextureImageView(0, mipLevels);
        }

        uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        std::cout << "uniformBuffers.size() = " << uniformBuffers.size() << std::endl;

        uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        VkDeviceSize bufferSize = sizeof(ModelUBO);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            std::cout << "Creating buffer " << i << std::endl;
            createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

            std::cout << "Mapping buffer " << i << std::endl;
            vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);

            std ::cout << "Creating ubo" << std::endl;
            ModelUBO ubo{};
            ubo.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));

            memcpy(uniformBuffersMapped[i], &ubo, sizeof(ubo));
        }

        std::cout << "uniformBuffers.size() = " << uniformBuffers.size() << std::endl;

        std::cout << "Creating descriptor pools" << std::endl;

        this->createDescriptorPools();

        std::cout << "textureImages.size() = " << textureImages.size() << std::endl;
        this->createTextureSamplers();

        std::cout << "uniformBuffers.size() = " << uniformBuffers.size() << std::endl;

        this->createModelDescriptorSets();

        std::cout << "Vertices.size() = " << Vertices.size() << std::endl;

        this->createVertexBuffers();

        std::cout << "Indices.size() = " << Indices.size() << std::endl;
        this->createIndexBuffers();

        std::cout << "Done creating model" << std::endl;
    }
}