#include "stdafx.h"
#include "render/RGeometry.h"

namespace rush
{

    RGeometry::RGeometry(PrimitiveTopology type, uint32_t bufferCount, const VertexLayout* layouts, uint64_t vertCount, uint64_t indexCount, const StringView& lable)
    {
        m_Type = type;
        m_BufferCount = bufferCount;
        memcpy(m_Layouts, layouts, sizeof(VertexLayout) * m_BufferCount);
        for (uint32_t i = 0; i < m_BufferCount; ++i)
        {
            auto vb = CreateRef<RVertexBuffer>(m_Layouts[i].stride, vertCount * m_Layouts[i].stride);
            m_VertexBuffers.push_back(vb);
        }
        if (indexCount > 0)
        {
            RUSH_ASSERT(type == PrimitiveTopology::LineList || type == PrimitiveTopology::TriangleList);
            auto idxFormat = indexCount >= (uint64_t)0xffffffff ? IndexFormat::Uint32 : IndexFormat::Uint16;
            m_IndexBuffer = CreateRef<RIndexBuffer>(indexCount, idxFormat);
        }
        else
        {
            RUSH_ASSERT(type == PrimitiveTopology::LineStrip || type == PrimitiveTopology::PointList || type == PrimitiveTopology::TriangleStrip);
        }

        // hash
        for (int i = 0; i < m_BufferCount; ++i)
        {
            hashCombine(m_LayoutHash, m_Layouts[i].stride);
            for (int j = 0; j < m_Layouts[i].attributeCount; ++j)
            {
                hashCombine(m_LayoutHash, m_Layouts[i].attributes[j].format, m_Layouts[i].attributes[j].offset, m_Layouts[i].attributes[j].location);
            }
        }
    }

    void RGeometry::UpdateVertexBuffer(uint32_t index, const void* data, uint64_t size, uint64_t offset /*= 0*/)
    {
        if (index >= 0 && index < m_BufferCount)
        {
            m_VertexBuffers[index]->UpdateData(data, size, offset);
        }
    }

    void RGeometry::UpdateIndexBuffer(const void* data, uint64_t size, uint64_t offset /*= 0*/)
    {
        if (m_IndexBuffer)
        {
            m_IndexBuffer->UpdateData(data, size, offset);
        }
    }

}