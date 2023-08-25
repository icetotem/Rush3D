#include "stdafx.h"

#include <dawn/webgpu_cpp.h>

#include "render/RBuffer.h"
#include "RContex.h"

namespace rush
{

    RVertexBuffer::RVertexBuffer(Ref<RContex> contex, uint32_t stride, uint64_t size, const char* lable)
    {
        m_Contex = contex;
        m_Size = size;
        m_Stride = stride;
        m_Count = m_Size / m_Stride;
        RUSH_ASSERT(m_Size % 4 == 0);
        wgpu::BufferDescriptor descriptor;
        descriptor.label = lable;
        descriptor.size = m_Size;
        descriptor.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
        m_Buffer = CreateRef<wgpu::Buffer>(contex->device.CreateBuffer(&descriptor));
    }

    void RVertexBuffer::UpdateData(const void* data, uint64_t size, uint64_t startVertex)
    {
        uint64_t offset = startVertex * m_Stride;
        if (data != nullptr && size > 0 && offset + size <= m_Size)
        {
            m_Contex->queue.WriteBuffer(*m_Buffer.get(), offset, data, size);
        }
        else
        {
            LOG_ERROR("RVertexBuffer::UpdateData, size error {} {}", size, offset);
        }
    }

    //////////////////////////////////////////////////////////////////////////

    RIndexBuffer::RIndexBuffer(Ref<RContex> contex, uint64_t count, bool use32bits /*= false*/, const char* lable)
    {
        m_Contex = contex;
        m_Use32Bits = use32bits;
        m_Count = count;
        m_Size = m_Count * (use32bits ? sizeof(uint32_t) : sizeof(uint16_t));
        RUSH_ASSERT(m_Size % 4 == 0);
        wgpu::BufferDescriptor descriptor;
        descriptor.label = lable;
        descriptor.size = m_Size;
        descriptor.usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
        m_Buffer = CreateRef<wgpu::Buffer>(contex->device.CreateBuffer(&descriptor));
    }

    void RIndexBuffer::UpdateData(const void* data, uint64_t size, uint32_t startIndex /*= 0*/)
    {
        uint64_t offset = startIndex * (m_Use32Bits ? sizeof(uint32_t) : sizeof(uint16_t));
        if (data != nullptr && size > 0 && offset + size <= m_Size)
        {
            m_Contex->queue.WriteBuffer(*m_Buffer.get(), offset, data, size);
        }
        else
        {
            LOG_ERROR("RVertexBuffer::UpdateData, size error {} {}", size, offset);
        }
    }


}