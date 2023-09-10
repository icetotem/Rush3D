#ifndef PassNode_h__
#define PassNode_h__

#include "render/fg/GraphNode.hpp"
#include "render/fg/PassEntry.hpp"
#include "render/fg/FrameGraphResource.hpp"
#include <memory>
#include <vector>

namespace rush
{

    class PassNode final : public GraphNode
    {
        friend class FrameGraph;

    public:
        struct AccessDeclaration {
            FrameGraphResource id;
            uint32_t flags;

            AccessDeclaration(FrameGraphResource _id, uint32_t _flags) : id(_id), flags(_flags)
            {
            }

            bool operator==(const AccessDeclaration& rhs) const
            {
                return id == rhs.id && flags == rhs.id;
            }
        };

        [[nodiscard]] bool creates(FrameGraphResource id) const;
        [[nodiscard]] bool reads(FrameGraphResource id) const;
        [[nodiscard]] bool writes(FrameGraphResource id) const;

        [[nodiscard]] bool hasSideEffect() const;
        [[nodiscard]] bool canExecute() const;

    private:
        PassNode(const std::string_view name, uint32_t id,
            std::unique_ptr<FrameGraphPassConcept>&&);

        FrameGraphResource _read(FrameGraphResource id, uint32_t flags);
        [[nodiscard]] FrameGraphResource _write(FrameGraphResource id,
            uint32_t flags);

    private:
        std::unique_ptr<FrameGraphPassConcept> m_exec;

        std::vector<FrameGraphResource> m_creates;

        std::vector<AccessDeclaration> m_reads;
        std::vector<AccessDeclaration> m_writes;

        bool m_hasSideEffect{ false };
    };

}

#endif // PassNode_h__
