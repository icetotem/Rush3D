#include "stdafx.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"
#include <mio/single_include/mio/mio.hpp>

namespace rush
{
    class MemMap
    {
    public:
        MemMap() {}
        virtual ~MemMap() {}
    };

    class MemMapRead : public MemMap
    {
    public:
        mio::mmap_source mmap;
    };

    class MemMapWrite : public MemMap
    {
    public:
        mio::mmap_sink mmap;
    };

    File::File()
        : m_ReadOnly(true)
        , m_MemMap(nullptr)
    {
    }

    File::~File()
    {
        delete m_MemMap;
    }

    bool File::CreateDir(const std::string& dir)
    {
        return std::filesystem::create_directory(Path(dir));
    }

    bool File::New(const std::string& fileName, size_t initSize /*= 0*/, bool txt /*= false*/)
    {
        std::fstream file(fileName, std::ios::out | std::ios::trunc | (txt ? 0 : std::ios::binary));
        if (file)
        {
            if (initSize > 0)
            {
                // Seek to the desired size - 1
                file.seekp(initSize - 1, std::ios::beg);

                // Write a single byte at the end to make the file's size exactly 'desiredSize'
                file.write("", 1);

                file.close();
            }
            return true;
        }
        else
            return false;
    }

    void File::Del(const std::string& file)
    {
        std::remove(file.c_str());
    }

    void File::Copy(const std::string& srcFile, const std::string& descPath, const std::string& newName)
    {
        std::error_code err;
        std::filesystem::copy(srcFile, descPath, err);
        if (!err && newName != "")
        {
            auto fileName = Path(srcFile).filename().string();
            auto fileExt = Path(srcFile).extension().string();
            std::filesystem::rename(descPath + fileName, descPath + newName + fileExt);
        }
    }

    bool File::Open(const std::string& path, bool readOnly /*= true*/, size_t offset/* = 0*/, size_t length /*= 0*/)
    {
        std::error_code error;
        m_ReadOnly = readOnly;
        m_FullPath = std::filesystem::absolute(Path(path)).string();
        if (m_ReadOnly)
        {
            m_MemMap = new MemMapRead;
            static_cast<MemMapRead*>(m_MemMap)->mmap = mio::make_mmap_source(m_FullPath, offset, length, error);
            m_Length = static_cast<MemMapRead*>(m_MemMap)->mmap.size();
        }
        else
        {
            m_MemMap = new MemMapWrite;
            static_cast<MemMapWrite*>(m_MemMap)->mmap = mio::make_mmap_sink(m_FullPath, offset, length, error);
            m_Length = static_cast<MemMapWrite*>(m_MemMap)->mmap.size();
        }

        if (error)
        {
            //LOG_ERROR("FileMemMap {0} [errno = {1}]", m_FullPath.c_str(), error.message().c_str());
            return false;
        }

        m_Pointer = 0;

        return true;
    }

    bool File::IsOpen() const
    {
        if (m_ReadOnly)
        {
            return (static_cast<MemMapRead*>(m_MemMap)->mmap.data() != nullptr);
        }
        else
        {
            return (static_cast<MemMapWrite*>(m_MemMap)->mmap.data() != nullptr);
        }
    }

    void File::Flush()
    {
        RUSH_ASSERT(!m_ReadOnly);
        std::error_code error;
        static_cast<MemMapWrite*>(m_MemMap)->mmap.sync(error);
        if (error)
        {
            LOG_ERROR("Failed to flush FileMemMap {0} [errno = {1}]", m_FullPath.c_str(), error.message().c_str());
        }
    }

    void File::Close()
    {
        if (m_ReadOnly)
        {
            if (static_cast<MemMapRead*>(m_MemMap)->mmap.data() != nullptr)
                static_cast<MemMapRead*>(m_MemMap)->mmap.unmap();
        }
        else
        {
            if (static_cast<MemMapWrite*>(m_MemMap)->mmap.data() != nullptr)
                static_cast<MemMapWrite*>(m_MemMap)->mmap.unmap();
        }
    }

    const char* File::GetReadOnlyHeader() const
    {
        if (m_ReadOnly)
            return static_cast<MemMapRead*>(m_MemMap)->mmap.data();
        else
            return static_cast<MemMapWrite*>(m_MemMap)->mmap.data();
    }

    const char* File::GetReadOnlyCurrent() const
    {
        return GetReadOnlyHeader() + m_Pointer;
    }

    char* File::GetWriteableHeader()
    {
        RUSH_ASSERT(!m_ReadOnly);
        return static_cast<MemMapWrite*>(m_MemMap)->mmap.data();
    }

    char* File::GetWriteableCurrent()
    {
        return GetWriteableHeader() + m_Pointer;
    }

    char* File::ReadLine(char* str, int num)
    {
        if (num <= 0)
            return NULL;
        char c = 0;
        size_t maxCharsToRead = num - 1;
        for (size_t i = 0; i < maxCharsToRead; ++i)
        {
            bool result = Read(c);
            if (!result)
            {
                str[i] = '\0';
                break;
            }
            if (c == '\n')
            {
                str[i] = c;
                str[i + 1] = '\0';
                break;
            }
            else if (c == '\r')
            {
                str[i] = c;
                // next may be '\n'
                size_t pos = Tell();

                char nextChar = 0;
                if (Read(nextChar))
                {
                    // no more characters
                    str[i + 1] = '\0';
                    break;
                }
                if (nextChar == '\n')
                {
                    if (i == maxCharsToRead - 1)
                    {
                        str[i + 1] = '\0';
                        break;
                    }
                    else
                    {
                        str[i + 1] = nextChar;
                        str[i + 2] = '\0';
                        break;
                    }
                }
                else
                {
                    Seek(pos, true);
                    str[i + 1] = '\0';
                    break;
                }
            }
            str[i] = c;
        }
        return str; // what if first read failed?
    }


    std::string FileSystem::s_WorkingDir;

    void FileSystem::SetWorkingDir(const std::string& relToRuntimPath)
    {
        s_WorkingDir = FS::absolute(FS::current_path() / relToRuntimPath).string();
        std::replace(s_WorkingDir.begin(), s_WorkingDir.end(), '\\', '/');
    }

    bool FileSystem::FindFilesInFolder(std::string strPath, std::vector<std::string>& vecFiles, const std::set<std::string>& exts)
    {
        char cEnd = *strPath.rbegin();
        if (cEnd == '\\' || cEnd == '/')
        {
            strPath = strPath.substr(0, strPath.length() - 1);
        }

        if (strPath.empty() || strPath == (".") || strPath == (".."))
            return false;

        std::error_code ec;
        std::filesystem::path fsPath(strPath);
        if (!std::filesystem::exists(strPath, ec)) {
            return false;
        }
        //  const std::string str1 = str.path().filename().string();
        //  std::cout << std::filesystem::absolute(str.path()) << '\n'; //绝对路径
        //  std::cout << std::filesystem::absolute(str.path()).string() << '\n';
        for (auto& iter : std::filesystem::directory_iterator(fsPath))
        {
            if (std::filesystem::is_directory(iter.status()))
            {
                FindFilesInFolder(iter.path().string(), vecFiles, exts);
            }
            else
            {
                //if (std::regex_match(filename.string(), fileSuffix))
                //std::filesystem::remove_all(version_dir, ec);
                //std::filesystem::copy_file(from_file, to_file, std::filesystem::copy_options::skip_existing, ec))
                std::string ext = iter.path().extension().string();
                if (exts.find(ext) != exts.end())
                {
                    std::string filePath = iter.path().string();
                    std::replace(filePath.begin(), filePath.end(), '\\', '/');
                    vecFiles.push_back(filePath);
                }
            }
        }

        return true;
    } 

    void FileSystem::TraverseFolder(std::string strPath, FileTraverse traverseFunction, void* param)
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
        if (!std::filesystem::exists(strPath, ec)) {
            return;
        }
        //  const std::string str1 = str.path().filename().string();
        //  std::cout << std::filesystem::absolute(str.path()) << '\n'; //绝对路径
        //  std::cout << std::filesystem::absolute(str.path()).string() << '\n';
        for (auto& iter : std::filesystem::directory_iterator(fsPath))
        {
            std::string filePath = iter.path().string();
            std::replace(filePath.begin(), filePath.end(), '\\', '/');

            if (std::filesystem::is_directory(iter.status()))
            {
                traverseFunction(true, filePath, param);
                TraverseFolder(iter.path().string(), traverseFunction, param);
            }
            else
            {

                traverseFunction(false, filePath, param);
            }
        }
    }

    std::string FileSystem::GetSubPath(const std::string& package, const std::string& fullPath)
    {
        auto temp = fullPath.find(package, package.size());
        return fullPath.substr(temp + package.length() + 1, fullPath.length() - temp);
    }


}
