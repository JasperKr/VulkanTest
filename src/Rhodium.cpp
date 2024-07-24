#include "src/Graphics.cpp"

class Rhodium
{
public:
    Shader newShader(const char *vertexPath, const char *fragmentPath)
    {
        return Shader(vertexPath, fragmentPath);
    }
};