#include "GL_shader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>

GL_shader::GL_shader() 
{
    Init();
}

GL_shader::~GL_shader()
{
    DeInitGL();
}

GL_shader::GL_shader(GL_shader&& s)
{
    this->m_ID = s.m_ID;
    this->m_bIsInit = s.m_bIsInit;

    s.m_ID = 0;
    s.m_bIsInit = false;
}

GL_shader& GL_shader::operator=(GL_shader&& s)
{
    if (&s != this)
    {
        this->m_ID = s.m_ID;
        this->m_bIsInit = s.m_bIsInit;

        s.m_ID = 0;
        s.m_bIsInit = false;       
    }

    return *this;
}

std::string GL_shader::ReadShaderCode(const std::string& codeFilePath)
{
    std::string code;
    std::ifstream shaderFile;
    // ensure ifstream objects can throw exceptions:
    shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        shaderFile.open(codeFilePath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        code = shaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout 
            << "Shader code read error! \n\t[path]: " << codeFilePath
            << "\n\t[error mmessage]: " << e.what() 
            << std::endl;
    }

    return code;
}

void GL_shader::Init()
{
    if (m_bIsInit) {
        std::cout << "The shader is already Init" << std::endl;
        return;
    }

    m_bIsInit = true;
    m_ID = glCreateProgram();
}

void GL_shader::AttachShaderFromFile(const std::string& path, GLenum type)
{
    if (!m_bIsInit) {
        std::cout 
            << "Shader must be initialized befor AttachShader call \n\t[path]: " << path 
            << "\n\t[type]: " << type 
            << std::endl;
        return;
    }

    GLuint sh = glCreateShader(type);
    std::string source = ReadShaderCode(path);
    const char* source_c = source.c_str();
    glShaderSource(sh, 1, &source_c, NULL);
    glCompileShader(sh);

    GLint success;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint log_length;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<GLchar> message(log_length);
        glGetShaderInfoLog(sh, log_length, nullptr, message.data());
        
        std::cout 
            << "AttachShader error! \n\t[path]: " << path 
            << "\n\t[type]: " << type 
            << "\n\t[error message]: \n" << message.data() << std::endl;
        return;
    }

    glAttachShader(m_ID, sh);
    glDeleteShader(sh);
}

void GL_shader::AttachShaderCode(const std::string& code, GLenum type)
{
    // TO WRITE
}

void GL_shader::Link()
{
    if (!m_bIsInit) {
        std::cout << "Shader must be initialized befor linking" << std::endl;
        return;
    }

    glLinkProgram(m_ID);

    GLint success; 
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLint log_length;
        glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &log_length);
        std::vector<GLchar> message(log_length);
        glGetProgramInfoLog(m_ID, log_length, nullptr, message.data());

        std::cout << "LinkProgram error! \n\t[error message]: \n" << message.data() << std::endl;
        return;
    }
}

GLuint GL_shader::GetID()
{
    return m_ID;
}

void GL_shader::DeInitGL()
{
    if (!m_bIsInit) {
        std::cout << "Can not delete shader which wasn't initialized" << std::endl;
        return;
    }

    glDeleteShader(m_ID);
}

void GL_shader::Use()
{
    glUseProgram(m_ID);
}

    // *=*=*=*=*=*=*=*=*=*=*=*
    //    UNIFORM SETTERS
    // *=*=*=*=*=*=*=*=*=*=*=*

void GL_shader::set1b(const char* name, GLboolean value)
{
    glUniform1i(glGetUniformLocation(m_ID, name), (int)value); 
}

void GL_shader::set1i(const char* name, GLint v0)
{
    glUniform1i(glGetUniformLocation(m_ID, name), v0);
}

void GL_shader::set1f(const char* name, GLfloat v0)
{
    glUniform1f(glGetUniformLocation(m_ID, name), v0);
}

void GL_shader::set1ui(const char* name, GLuint v0)
{
    glUniform1ui(glGetUniformLocation(m_ID, name), v0);
}

void GL_shader::set2i(const char* name, GLint v0, GLint v1)
{
    glUniform2i(glGetUniformLocation(m_ID, name), v0, v1);
}

void GL_shader::set2fv(const char* name, const glm::vec2& value)
{
    glUniform2fv(glGetUniformLocation(m_ID, name), 1, (float*)&value);
}

void GL_shader::set3f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2)
{
    glUniform3f(glGetUniformLocation(m_ID, name), v0, v1, v2);
}

void GL_shader::set3fv(const char* name, const glm::vec3& value)
{
    glUniform3fv(glGetUniformLocation(m_ID, name), 1, (float*)&value);
}

void GL_shader::set4fv(const char* name, const glm::vec4& value)
{
    glUniform4fv(glGetUniformLocation(m_ID, name), 1, (float*)&value);
}

void GL_shader::setM4fv(const char* name, GLboolean transpose, const glm::mat4& matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(m_ID, name), 1, transpose, (float*)&matrix);
}

GLuint GL_shader::GetUniformBlockIndex(const char* uniformBlockName) const
{
    GLuint result = glGetUniformBlockIndex(m_ID, uniformBlockName);
    if (result == GL_INVALID_INDEX) {
        std::cout << "Could not get index of uniform block [" << uniformBlockName << "], check if suck uniform block really exists" << std::endl;  
    }

    return result;
}

void GL_shader::BindUniformBlockToBindingPoint(const char* uniformBlockName, const GLuint bindingPoint) const
{
    const GLuint blockIndex = GetUniformBlockIndex(uniformBlockName);
    if (blockIndex != GL_INVALID_INDEX) {
        glUniformBlockBinding(m_ID, blockIndex, bindingPoint);
    } 
}

