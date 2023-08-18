#pragma once


namespace rush
{
    
    class MemMap;

    class File
    {
    public:
        File();
        virtual ~File();

        static bool CreateDir(const std::string& dir);
        static bool New(const std::string& file, size_t initSize = 0, bool txt = false);
        static void Del(const std::string& file);
        static void Copy(const std::string& srcFile, const std::string& descPath, const std::string& newName = "");

        bool Open(const std::string& path, bool readOnly = true, size_t offset = 0, size_t length = 0);

        bool IsOpen() const;

        void Flush();

        void Close();

        size_t GetSize() const { return m_Length; }

        const std::string& GetFullPath() const { return m_FullPath; }

        const char* GetReadOnlyHeader() const;
        const char* GetReadOnlyCurrent() const;

        char* GetWriteableHeader();
        char* GetWriteableCurrent();

        char* ReadLine(char* str, int num);

        bool Seek(int pos, bool fromBegin = true)
        {
            if (fromBegin)
            {
                if (pos >= GetSize() || pos < 0)
                {
                    return false;
                }
                else
                {
                    m_Pointer = pos;
                    return true;
                }
            }
            else
            {
                auto newPos = m_Pointer + pos;
                if (newPos >= GetSize())
                {
                    return false;
                }
                else
                {
                    m_Pointer = newPos;
                    return true;
                }
            }
        }

        size_t Tell() const { return m_Pointer; }

        void Forward(size_t offset)
        {
            m_Pointer += offset;
        }

        void Backward(size_t offset)
        {
            m_Pointer -= offset;
        }

        bool Eof() const { return m_Pointer == GetSize(); }

        template<class T>
        bool Read(T& value)
        {
            if (m_Pointer + sizeof(T) > GetSize())
            {
                return false;
            }
            else
            {
                memcpy(&value, GetReadOnlyCurrent(), sizeof(T));
                Forward(sizeof(T));
                return true;
            }
        }

        bool ReadBuffer(uint8_t* value, size_t size)
        {
            if (m_Pointer + size > GetSize())
            {
                return false;
            }
            else
            {
                memcpy(value, GetReadOnlyCurrent(), size);
                Forward(size);
                return true;
            }
        }

        bool Read(std::string& value)
        {
            if (m_Pointer + value.size() + sizeof(uint32_t) > GetSize())
            {
                return false;
            }
            else
            {
                uint32_t len;
                Read(len);
                value.resize(len);
                memcpy(value.data(), GetReadOnlyCurrent(), len);
                Forward(len);
                return true;
            }
        }

        template<class T>
        void Write(const T& value)
        {
            RUSH_ASSERT(m_Pointer + sizeof(T) <= GetSize());
            memcpy(GetWriteableCurrent(), &value, sizeof(T));
            Forward(sizeof(T));
        }

        void WriteBuffer(const uint8_t* value, size_t size)
        {
            RUSH_ASSERT(m_Pointer + size <= GetSize());
            memcpy(GetWriteableCurrent(), value, size);
            Forward(size);
        }

        void Write(const std::string& value)
        {
            uint32_t len = value.length();
            RUSH_ASSERT(m_Pointer + len + sizeof(uint32_t) <= GetSize());
            Write(len);
            memcpy(GetWriteableCurrent(), value.data(), value.length());
            Forward(value.length());
        }

        template<class T>
        File& operator>> (T& value)
        {
            Read(value);
        }

        template<class T>
        File& operator<< (const T& value)
        {
            Write(value);
        }

    protected:
        MemMap* m_MemMap;
        bool m_ReadOnly;
        std::string m_FullPath;
        size_t m_Pointer = 0;
        size_t m_Length = 0;
    };

    namespace FS = std::filesystem;
    using Path = std::filesystem::path;

    typedef void (*FileTraverse)(bool isFolder, const std::string& fileName, void* param);

    class FileSystem
    {
    public:
        static void SetWorkingDir(const std::string& relToRuntimPath);

        static const std::string& GetWorkingDir() { return s_WorkingDir; }

        static bool FindFilesInFolder(std::string strPath, std::vector<std::string>& vecFiles, const std::set<std::string>& exts);

        static void TraverseFolder(std::string path, FileTraverse traverseFunction, void* param);

        static std::string GetSubPath(const std::string& package, const std::string& fullPath);

    protected:
        static std::string s_WorkingDir;
    };


}