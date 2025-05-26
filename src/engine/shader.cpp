#include "engine/shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace zuul {

Shader::Shader() : mProgram(0) {}

Shader::~Shader() {
    if (mProgram != 0) {
        glDeleteProgram(mProgram);
    }
}

bool Shader::loadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
    std::cout << "Loading vertex shader from: " << vertexPath << std::endl;
    // Read vertex shader
    std::string vertexCode;
    std::ifstream vShaderFile(vertexPath);
    if (vShaderFile.is_open()) {
        std::stringstream vShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        vertexCode = vShaderStream.str();
        vShaderFile.close();
        std::cout << "Successfully loaded vertex shader" << std::endl;
    } else {
        std::cerr << "Failed to open vertex shader file: " << vertexPath << std::endl;
        return false;
    }

    std::cout << "Loading fragment shader from: " << fragmentPath << std::endl;
    // Read fragment shader
    std::string fragmentCode;
    std::ifstream fShaderFile(fragmentPath);
    if (fShaderFile.is_open()) {
        std::stringstream fShaderStream;
        fShaderStream << fShaderFile.rdbuf();
        fragmentCode = fShaderStream.str();
        fShaderFile.close();
        std::cout << "Successfully loaded fragment shader" << std::endl;
    } else {
        std::cerr << "Failed to open fragment shader file: " << fragmentPath << std::endl;
        return false;
    }

    std::cout << "Compiling vertex shader..." << std::endl;
    // Compile shaders
    GLuint vertexShader, fragmentShader;
    if (!compileShader(vertexShader, GL_VERTEX_SHADER, vertexCode)) {
        return false;
    }
    std::cout << "Vertex shader compiled successfully" << std::endl;

    std::cout << "Compiling fragment shader..." << std::endl;
    if (!compileShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentCode)) {
        glDeleteShader(vertexShader);
        return false;
    }
    std::cout << "Fragment shader compiled successfully" << std::endl;

    std::cout << "Creating shader program..." << std::endl;
    // Create shader program
    mProgram = glCreateProgram();
    glAttachShader(mProgram, vertexShader);
    glAttachShader(mProgram, fragmentShader);
    
    if (!linkProgram()) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    std::cout << "Shader program created and linked successfully" << std::endl;

    // Clean up
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

void Shader::use() {
    glUseProgram(mProgram);
}

void Shader::setUniform(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(mProgram, name.c_str()), value);
}

void Shader::setUniform(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(mProgram, name.c_str()), value);
}

void Shader::setUniform(const std::string& name, float x, float y) {
    glUniform2f(glGetUniformLocation(mProgram, name.c_str()), x, y);
}

void Shader::setUniform(const std::string& name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(mProgram, name.c_str()), x, y, z);
}

void Shader::setUniform(const std::string& name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(mProgram, name.c_str()), x, y, z, w);
}

bool Shader::compileShader(GLuint& shader, GLenum type, const std::string& source) {
    shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
        return false;
    }
    return true;
}

bool Shader::linkProgram() {
    glLinkProgram(mProgram);

    // Check for linking errors
    GLint success;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(mProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
        return false;
    }
    return true;
}

// ShaderManager implementation
ShaderManager& ShaderManager::getInstance() {
    static ShaderManager instance;
    return instance;
}

bool ShaderManager::initialize() {
    return true;
}

void ShaderManager::cleanup() {
    mShaders.clear();
}

std::shared_ptr<Shader> ShaderManager::loadShader(const std::string& name,
                                                const std::string& vertexPath,
                                                const std::string& fragmentPath) {
    auto shader = std::make_shared<Shader>();
    if (shader->loadFromFile(vertexPath, fragmentPath)) {
        mShaders[name] = shader;
        return shader;
    }
    return nullptr;
}

std::shared_ptr<Shader> ShaderManager::getShader(const std::string& name) {
    auto it = mShaders.find(name);
    if (it != mShaders.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace zuul 