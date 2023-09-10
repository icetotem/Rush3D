#ifndef GraphNode_h__
#define GraphNode_h__

#include <string>

namespace rush
{

    class GraphNode
    {
    public:
        GraphNode() = delete;
        GraphNode(const GraphNode&) = delete;
        GraphNode(GraphNode&&) noexcept = default;
        virtual ~GraphNode() = default;

        GraphNode& operator=(const GraphNode&) = delete;
        GraphNode& operator=(GraphNode&&) noexcept = delete;

    protected:
        GraphNode(const std::string_view name, uint32_t id);

    protected:
        const std::string m_name;
        const uint32_t m_id; // Unique id, matches an array index in FrameGraph
        int32_t m_refCount{ 0 };
    };

}

#endif // GraphNode_h__
