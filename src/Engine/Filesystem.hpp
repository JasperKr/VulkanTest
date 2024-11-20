#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>
#include <vector>
#include <fstream>

namespace Filesystem
{
    std::vector<char> read(const std::string &filename);
}

#endif