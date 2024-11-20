#include <Models.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// #define TINYGLTF_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
// #define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

namespace Application_Model
{
    VkDevice device;

    bool loadModel(tinygltf::Model &model, const char *filename)
    {
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        bool res = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
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

    void load(std::string Path, tinygltf::Model &model, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices)
    {
        bool res = loadModel(model, Path.c_str());

        if (!res)
        {
            throw std::runtime_error("Failed to load model!");
        }

        // load the first mesh, it's first primitive
        // only the vertices and indices are loaded

        tinygltf::Mesh mesh = model.meshes[0];
        tinygltf::Primitive primitive = mesh.primitives[0];

        // load the vertex data

        tinygltf::Accessor positionAccessor = model.accessors[primitive.attributes["POSITION"]];
        tinygltf::BufferView positionBufferView = model.bufferViews[positionAccessor.bufferView];
        tinygltf::Buffer positionBuffer = model.buffers[positionBufferView.buffer];

        // load the index data

        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
        tinygltf::BufferView indexBufferView = model.bufferViews[indexAccessor.bufferView];
        tinygltf::Buffer indexBuffer = model.buffers[indexBufferView.buffer];

        // load the vertices

        for (size_t i = 0; i < positionAccessor.count; i++)
        {
            Vertex vertex{};

            float *position = (float *)&positionBuffer.data[positionAccessor.byteOffset + positionBufferView.byteOffset + i * positionAccessor.ByteStride(positionBufferView)];
            vertex.pos = glm::vec3(position[0], position[1], position[2]);

            vertices.push_back(vertex);
        }

        // load the indices

        for (size_t i = 0; i < indexAccessor.count; i++)
        {
            uint32_t index = 0;

            if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT)
            {
                index = ((uint32_t *)&indexBuffer.data[indexAccessor.byteOffset + indexBufferView.byteOffset])[i];
            }
            else if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT)
            {
                index = ((uint16_t *)&indexBuffer.data[indexAccessor.byteOffset + indexBufferView.byteOffset])[i];
            }
            else if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE)
            {
                index = ((uint8_t *)&indexBuffer.data[indexAccessor.byteOffset + indexBufferView.byteOffset])[i];
            }

            indices.push_back(index);
        }
    }
}