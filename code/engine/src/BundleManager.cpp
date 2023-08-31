#include "stdafx.h"
#include "BundleManager.h"
#include "mio/single_include/mio/mio.hpp"

namespace rush
{

    MemStream::MemStream(const uint8_t* buffer, uint64_t size)
        : m_Buffer(buffer)
        , m_Size(size)
    {
    }

    //////////////////////////////////////////////////////////////////////////

    BundleManager* BundleManager::s_Inst = nullptr;

    typedef void (*FileTraverse)(StringView fileHash, StringView filePath);

    struct SingleFile : public BundleManager::File
    {
        mio::mmap_source mmap;
    };

    void TraverseFolder(StringView bundleName, StringView strPath, FileTraverse traverseImpl)
    {
        char cEnd = *strPath.rbegin();
        if (cEnd == '\\' || cEnd == '/')
        {
            strPath = strPath.substr(0, strPath.length() - 1);
        }

        if (strPath.empty() || strPath == (".") || strPath == (".."))
            return;

        std::error_code ec;
        std::filesystem::path fsPath(strPath);
        if (!std::filesystem::exists(fsPath, ec)) {
            return;
        }
        //  const std::string str1 = str.path().filename().string();
        //  std::cout << std::filesystem::absolute(str.path()) << '\n'; //¾ø¶ÔÂ·¾¶
        //  std::cout << std::filesystem::absolute(str.path()).string() << '\n';
        for (auto& iter : std::filesystem::directory_iterator(fsPath))
        {
            std::string filePath = iter.path().string();
            filePath = str_tolower(filePath);
            std::replace(filePath.begin(), filePath.end(), '\\', '/');

            if (std::filesystem::is_directory(iter.status()))
            {
                TraverseFolder(bundleName, iter.path().string(), traverseImpl);
            }
            else
            {
                auto s = str_tolower(String(bundleName));
                traverseImpl(filePath.substr(filePath.find(s), filePath.length() - s.length()), filePath);
            }
        }
    }

    void BundleManager::TraverseFunction(StringView fileNameHash, StringView filePath)
    {
        auto& resMgr = BundleManager::instance();

        Path path(fileNameHash);
        const auto& ext = path.extension();
        //auto type = resMgrGetExtType(str_tolower(ext.string()));

        auto key = String(fileNameHash);
        Ref<SingleFile> file = CreateRef<SingleFile>();
        std::error_code error;
        if (!error)
        {
            file->mmap = mio::make_mmap_source(String(filePath), 0, 0, error);
            file->stream = CreateRef<MemStream>((const uint8_t*)file->mmap.data(), file->mmap.size());
            resMgr.m_Files.insert({ key, std::static_pointer_cast<File>(file) });
        }
        else
        {
            LOG_ERROR("Failed to load file {}", fileNameHash);
        }
    }

    BundleManager::BundleManager()
    {
        s_Inst = this;
    }

    BundleManager::~BundleManager()
    {
        
    }

    void BundleManager::AddFolder(StringView path)
    {
        if (path == "")
        {
            return;
        }

        if (path[path.length() - 1] == '/' || path[path.length() - 1] == '\\')
        {
            path = path.substr(0, path.length() - 1);
        }
        auto bundleName = Path(String(path)).filename().string();
        TraverseFolder(bundleName, path, &BundleManager::TraverseFunction);
    }

    void BundleManager::AddBundle(StringView path)
    {
        
    }
    
    Ref<MemStream> BundleManager::Get(StringView assetPath) const
    {
        auto key = String(assetPath);
        key = str_tolower(key);

        auto iter = m_Files.find(key);
        if (iter != m_Files.end())
        {
            return iter->second->stream;
        }
        else
        {
            return CreateRef<MemStream>(nullptr, 0);
        }
    }

}