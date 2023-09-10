#include "stdafx.h"
#include "render/fg/GraphNode.hpp"

namespace rush
{

    GraphNode::GraphNode(const std::string_view name, uint32_t id)
        : m_name{ name }, m_id{ id } {}

}


