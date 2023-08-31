#include "stdafx.h"
#include "components/MeshRenderer.h"
#include "render/Renderer.h"

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
        m_Parts.emplace_back();
        return m_Parts.size() - 1;
    }

    MeshRenderer::Part* MeshRenderer::GetPart(int slot)
    {
        if (slot >= 0 && slot < m_Parts.size())
        {
            return &m_Parts[slot];
        }
        else
        {
            return nullptr;
        }
    }

}
