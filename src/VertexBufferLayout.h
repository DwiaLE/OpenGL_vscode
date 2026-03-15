#pragma once
#include "vector"
#include "glew.h"
#include "Renderer.h"
#include "iostream"
struct VertexBufferElement
{
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int GetsizeOfType(unsigned int type)
    {
        switch (type)
        {
            case GL_FLOAT: 
                return 4;
            case GL_UNSIGNED_INT: 
                return 4;
            case GL_UNSIGNED_BYTE: 
                return 1;
        }
        ASSERT(false);
        return 0;
    }
};

class VertexBufferLayout
{
private:
    std::vector<VertexBufferElement> m_Element;
    unsigned int m_Stride;

public:
    VertexBufferLayout()
        : m_Stride(0){};

    template<typename T>
    void Push(unsigned int count)
    {
        static_assert(false);
    }

    template<>
    void Push<float>(unsigned int count)
    {
        m_Element.push_back({GL_FLOAT, count,GL_FALSE});
        m_Stride += count* VertexBufferElement::GetsizeOfType(GL_FLOAT);
    }

    template<>
    void Push<unsigned int>(unsigned int count)      //int存放索引或者id所以不用归一化
    {
        m_Element.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
        m_Stride += count* VertexBufferElement::GetsizeOfType(GL_UNSIGNED_INT);
    }

    template<>
    void Push<unsigned char>(unsigned int count)     //char 类型一般用于存颜色，因为他不是float类型，
    {                                       //所以需要进行映射操作，如果直接用255会导致颜色爆表
        m_Element.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
        m_Stride += count* VertexBufferElement::GetsizeOfType(GL_UNSIGNED_BYTE);
    }

    inline const std::vector<VertexBufferElement>& GetElements() const {return m_Element; }
    inline unsigned int GetStride()const {return m_Stride;}
};

