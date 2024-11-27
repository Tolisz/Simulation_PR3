#include "GL_UBO.hpp"

#include <iostream>

GL_UBO::~GL_UBO()
{}

void GL_UBO::CreateUBO(const size_t byteSize, GLenum usageHint)
{
    if (m_isInit) {
        std::cout << "The UBO is already created. You must to delete UBO before subsequent creation" << std::endl;
        return;
    }

    glGenBuffers(1, &m_ID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
    glBufferData(GL_UNIFORM_BUFFER, byteSize, nullptr, usageHint);
    
    m_isInit = true;
    m_byteSize = byteSize;
}

void GL_UBO::DeleteUBO() 
{
    if (!m_isInit) {
        std::cout << "Can not delete UBO which was not created" << std::endl;
        return;
    }

    glDeleteBuffers(1, &m_ID);
    
    m_ID = 0;
    m_isInit = false;
    m_byteSize = 0;
}

void GL_UBO::BindUBO() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
}

void GL_UBO::SetBufferData(const size_t offset, const void* ptrData, const size_t dataSize) const
{
    glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, ptrData);
}

void GL_UBO::BindBufferBaseToBindingPoint(const GLuint bindingPoint) const
{
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_ID);
}

GLuint GL_UBO::GetID() const
{
    return m_ID;
}
