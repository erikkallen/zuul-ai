#pragma once

#include <string>
#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <memory>
#include <unordered_map>

namespace zuul {

class Shader {
public:
    Shader();
    ~Shader();

    bool loadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
    void use();
    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, int value);
    void setUniform(const std::string& name, float x, float y);
    void setUniform(const std::string& name, float x, float y, float z);
    void setUniform(const std::string& name, float x, float y, float z, float w);

private:
    GLuint mProgram;
    bool compileShader(GLuint& shader, GLenum type, const std::string& source);
    bool linkProgram();
};

class ShaderManager {
public:
    static ShaderManager& getInstance();
    
    bool initialize();
    void cleanup();
    
    std::shared_ptr<Shader> loadShader(const std::string& name, 
                                      const std::string& vertexPath, 
                                      const std::string& fragmentPath);
    std::shared_ptr<Shader> getShader(const std::string& name);

private:
    ShaderManager() = default;
    ~ShaderManager() = default;
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    std::unordered_map<std::string, std::shared_ptr<Shader>> mShaders;
};

} // namespace zuul 