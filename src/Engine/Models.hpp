#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <Vertex.hpp>

#include "tiny_gltf.h"

namespace Application_Model
{
    extern VkDevice device;

    bool loadModel(tinygltf::Model &model, const char *filename);
    void load(std::string Path, tinygltf::Model &model, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);
}

#endif