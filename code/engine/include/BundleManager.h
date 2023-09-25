#ifndef BundleManager_h__
#define BundleManager_h__

#include "core/Core.h"

namespace rush
{

    class MemStream
    {
    public:
        MemStream(const uint8_t* buffer, uint64_t size);

        bool IsEmpty() const { return m_Buffer == nullptr || m_Size == 0; }

        const uint8_t* GetData() const { return m_Buffer; }

        uint64_t GetSize() const { return m_Size; }

    protected:
        const uint8_t* m_Buffer;
        uint64_t m_Size;
    };
    
    class BundleManager : public Singleton<BundleManager>
    {
    public:
        BundleManager();
        ~BundleManager();

        void AddFolder(StringView path);
        void AddBundle(StringView path);

        Ref<MemStream> Get(StringView assetPath) const;

        static void TraverseFunction(StringView fileHash, StringView filePath);

        List<String> GetFileNamesByExt(StringView ext);

        void LoadSingleFile(StringView filePath);

        struct File
        {
            Ref<MemStream> stream;
        };

    protected:
        HMap<String, Ref<File>> m_Files;
        HMap<String, List<String>> m_FileNamesByExt;
    };

}

#endif // BundleManager_h__
