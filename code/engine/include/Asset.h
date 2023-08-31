#ifndef Asset_h__
#define Asset_h__

#include "core/Core.h"

namespace rush
{

    /// <summary>
    /// 
    /// </summary>
    class Asset
    {
    public:
        Asset() {}
        virtual ~Asset() {}

        virtual bool Load(const StringView& path) { return false; }

    protected:
        bool m_Loaded = false;
    };

}

#endif // Asset_h__
