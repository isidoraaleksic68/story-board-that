#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>     // For glm::value_ptr


class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void use();
    void setVec3(const std::string& name, float x, float y, float z);
    GLuint getID();
    void setInt(const std::string& name, int value);
    void setBool(const std::string& name, bool value);
    void setMat4(const std::string& name, const glm::mat4& mat);
private:
    GLuint programID;
    std::string readFile(const std::string& filePath);
    void checkCompileErrors(GLuint shader, const std::string& type);
};

#endif
