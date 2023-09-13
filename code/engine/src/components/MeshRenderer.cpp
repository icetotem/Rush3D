#include "stdafx.h"
#include "components/MeshRenderer.h"
#include "render/RenderContext.h"

namespace rush
{  

    MeshRenderer::MeshRenderer(Entity owner)
        : Component(owner)
    {
    }

    MeshRenderer::~MeshRenderer()
    {
    }

    int MeshRenderer::AddPart()
    {
        m_Primitives.emplace_back();
        return m_Primitives.size() - 1;
    }

    MeshRenderer::Primitive* MeshRenderer::GetPart(int slot)
    {
        if (slot >= 0 && slot < m_Primitives.size())
        {
            return &m_Primitives[slot];
        }
        else
        {
            return nullptr;
        }
    }

}
