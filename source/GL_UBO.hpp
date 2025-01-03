#pragma once 

#include <glad/glad.h>

#include <cstddef>

class GL_UBO 
{
private:

    GLuint m_ID{0};
    bool m_isInit = false;
    size_t m_byteSize;

public:

    ~GL_UBO();

    void CreateUBO(const size_t byteSize, GLenum usageHint = GL_STREAM_DRAW);
    void DeleteUBO();

    void BindUBO() const;
    void SetBufferData(const size_t offset, const void* ptrData, const size_t dataSize) const;
    void BindBufferBaseToBindingPoint(const GLuint bindingPoint) const;
    GLuint GetID() const;

};