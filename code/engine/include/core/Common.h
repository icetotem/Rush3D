#ifndef Common_h__
#define Common_h__

#ifdef _DEBUG
    #define RUSH_DEBUG
#else
	#define RUSH_RELEASE
#endif

#if defined(_WIN32) || defined(_WIN64)
    #define RUSH_PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #if defined(TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR || TARGET_OS_EMBEDDED)
        #define RUSH_PLATFORM_IOS
    #elif defined(TARGET_OS_MAC)
        #define RUSH_PLATFORM_MAC
    #endif
#elif defined(__linux__)
    #define RUSH_PLATFORM_LINUX
#elif defined(__ANDROID__)
    #define RUSH_PLATFORM_ANDROID
#endif

#ifdef RUSH_PLATFORM_WINDOWS
#pragma warning(disable:4996 4244 4267 4100)
#endif


#define RUSH_EXPAND_MACRO(x) x
#define RUSH_STRINGIFY_MACRO(x) #x

#define RUSH_BIT(x) (1 << x)
#define RUSH_MAGIC_NUM(_a, _b, _c, _d) ( ( (uint32_t)(_a) | ( (uint32_t)(_b) << 8) | ( (uint32_t)(_c) << 16) | ( (uint32_t)(_d) << 24) ) )
#define RUSH_MAGIC_NUM_STR(str) ( ( (uint32_t)(str[0]) | ( (uint32_t)(str[1]) << 8) | ( (uint32_t)(str[2]) << 16) | ( (uint32_t)(str[3]) << 24) ) )


#define RUSH_UNUSED_VAR( exp ) ( (void) (exp))

#ifdef RUSH_DEBUG
#if defined(RUSH_PLATFORM_WINDOWS)
#define RUSH_DEBUGBREAK() __debugbreak()
#else
#define RUSH_DEBUGBREAK()
#endif
#define RUSH_ENABLE_ASSERTS
#else
#define RUSH_DEBUGBREAK()
#endif

#ifdef RUSH_ENABLE_ASSERTS
#define RUSH_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { LOG##type##ERROR(msg, __VA_ARGS__); RUSH_DEBUGBREAK(); } }
#define RUSH_INTERNAL_ASSERT_WITH_MSG(type, check, ...) RUSH_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define RUSH_INTERNAL_ASSERT_NO_MSG(type, check) RUSH_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", RUSH_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define RUSH_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define RUSH_INTERNAL_ASSERT_GET_MACRO(...) RUSH_EXPAND_MACRO( RUSH_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, RUSH_INTERNAL_ASSERT_WITH_MSG, RUSH_INTERNAL_ASSERT_NO_MSG) )

#define RUSH_ASSERT(...) RUSH_EXPAND_MACRO( RUSH_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#else
#define RUSH_ASSERT(...) void(0);
#endif

// Object deletion macro
#define SAFE_DELETE(x) \
    { \
        delete x; \
        x = NULL; \
    }

// Array deletion macro
#define SAFE_DELETE_ARRAY(x) \
    { \
        delete[] x; \
        x = NULL; \
    }

#define ARRAY_SIZE(Array) (sizeof(Array) / sizeof((Array)[0]))

#include <memory>
#include <utility>
#include <functional>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <thread>
#include <sstream>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <map>
#include <vector>
#include <string>
#include <set>
#include <deque>
#include <algorithm>
#include <string_view>
#include "Logger.h"

namespace rush
{ 

    typedef std::string String;
    typedef std::string_view StringView;

    namespace FS = std::filesystem;
    using Path = std::filesystem::path;
    
    template<typename T>
    using List = std::list<T>;

    template<typename T>
    using DArray = std::vector<T>;

    template<typename T, size_t N>
    using SArray = std::array<T, N>;

    template<typename Key, typename Value>
    using Map = std::map<Key, Value>;

    template<typename Key, typename Value>
    using HMap = std::unordered_map<Key, Value>;

    template<typename T>
    using HSet = std::unordered_set<T>;

    template<typename T>
    using Unique = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Unique<T> CreateUnique(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Weak = std::weak_ptr<T>;

    inline std::string str_tolower(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return std::tolower(c); }
        );
        return s;
    }

    inline std::string str_toupper(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return std::toupper(c); }
        );
        return s;
    }


    // Only defined for unsigned integers because that is all that is
    // needed at the time of writing.
    template <typename Dst, typename Src, typename = std::enable_if_t<std::is_unsigned_v<Src>>>
    inline Dst checked_cast(const Src& value) {
        RUSH_ASSERT(value <= std::numeric_limits<Dst>::max());
        return static_cast<Dst>(value);
    }

    /// <summary>
    /// FreeList
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template<class T>
    class FreeList
    {
    public:
        // count: 初始分配的数量
        // fixMaxCount: 是否固定最大数量，如果为true，那么最大数量为count，如果为false，那么最大数量为无限
        void Init(int count, bool limitMax = true)
        {
            m_LimitMax = limitMax;
            m_Objects.resize(count);
            m_FreeList.resize(count);
            m_ActiveList.clear();
            for (int i = 0; i < count; ++i)
            {
                m_FreeList[i] = i;
            }
        }

        // handle 从1开始为有效
        T* Add(uint32_t& handle)
        {
            if (!m_FreeList.empty())
            {
                uint32_t index = m_FreeList.back();
                m_FreeList.pop_back();
                handle = index + 1;
                m_ActiveList.insert(handle);
                m_MaxHandle = std::max(m_MaxHandle, handle);
                return &m_Objects[index];
            }
            else if (!m_LimitMax)
            {
                auto& obj = m_Objects.emplace_back();
                handle = (uint32_t)m_Objects.size();
                m_MaxHandle = std::max(m_MaxHandle, handle);
                m_ActiveList.insert(handle);
                return &obj;
            }
            else
            {
                handle = 0;
                return nullptr;
            }
        }

        uint32_t GetMaxHandle() const { return m_MaxHandle; }

        void Remove(uint32_t handle)
        {
            RUSH_ASSERT(handle > 0 && handle <= m_Objects.size());
            RUSH_ASSERT(std::find(m_FreeList.begin(), m_FreeList.end(), handle - 1) == m_FreeList.end());

            //m_Objects[handle - 1] = T();  // Destruct and replace with default object
            m_FreeList.push_back(handle - 1);
            m_ActiveList.erase(handle);
        }

        T& GetRef(uint32_t handle)
        {
            RUSH_ASSERT(handle > 0 && handle <= m_Objects.size());
            return m_Objects[handle - 1];
        }

        const T& GetRef(uint32_t handle) const
        {
            RUSH_ASSERT(handle > 0 && handle <= m_Objects.size());
            return m_Objects[handle - 1];
        }

        void Clear()
        {
            m_Objects.clear();
            m_FreeList.clear();
            m_ActiveList.clear();
        }

        const std::vector<T>& GetAllObjects() const
        {
            return m_Objects;
        }

        std::vector<T>& GetAllObjects()
        {
            return m_Objects;
        }

        const std::set<uint32_t>& GetActiveList() const
        {
            return m_ActiveList;
        }

    private:
        std::vector<T> m_Objects;
        std::vector<uint32_t> m_FreeList;
        std::set<uint32_t> m_ActiveList;
        bool m_LimitMax = false;
        uint32_t m_MaxHandle = 0;
    };


    /// <summary>
    /// Delegate
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template <typename T>
    class Delegate {};

    template <typename R, typename... Params>
    class Delegate< R(Params...) >
    {
        typedef void* instancePtr;
        typedef R(*internalFunction) (void* instance, Params...);
        typedef std::pair< void*, internalFunction > methodStub;

        template <class C, R(C::* func)(Params...) >
        static inline R classMethodStub(void* instance, Params... args)
        {
            return (static_cast<C*>(instance)->*func)(args...);
        }

    public:

        Delegate() : stub(nullptr, nullptr)
        {}

        template <class C, R(C::* func)(Params...) >
        void bind(C* instance)
        {
            stub.first = instance;
            stub.second = &classMethodStub< C, func >;
        }

        bool valid()
        {
            return stub.first && stub.second;
        }

        R invoke(Params... args)
        {
            return stub.second(stub.first, args...);
        }

        const R invoke(Params... args) const
        {
            return stub.second(stub.first, args...);
        }
    private:

        methodStub stub;
    };


    template<typename T>
    class Singleton
    {
    public:
        Singleton() { }

        Singleton(const Singleton&) = delete;

        Singleton& operator=(const Singleton&) = delete;

        static T& instance()
        {
            return *s_Inst;
        }

    protected:
        static T* s_Inst;
    };

    std::string GenerateUUIDStr();

    uint64_t GenerateUUID64();

    template<class T>
    inline void hash_combine(uint64_t& seed, const T& val)
    {
        seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template<class T>
    inline void hash_val(uint64_t& seed, const T& val)
    {
        hash_combine(seed, val);
    }

    template<class T, class ...Types>
    inline void hash_val(uint64_t& seed, const T& val, const Types & ...args)
    {
        hash_combine(seed, val);
        hash_val(seed, args...);
    }

    template<class ...Types>
    inline uint64_t hash_val(const Types & ...args)
    {
        uint64_t  seed = 0;
        hash_val(seed, args...);
        return seed;
    }

    bool GetFileMD5(const std::string& filename, uint8_t md5[16]);

    std::string Md5_Str(uint8_t md5[16]);

    extern "C"
    {
        extern uint64_t Hash64(const std::string& value);
    }

    std::string base64_encode(std::string const& s, bool url = false);
    std::string base64_encode_pem(std::string const& s);
    std::string base64_encode_mime(std::string const& s);

    std::string base64_decode(std::string const& s, bool remove_linebreaks = false);
    std::string base64_encode(unsigned char const*, size_t len, bool url = false);

    std::string ws2s(const std::wstring& ws);
    std::wstring s2ws(const std::string& s);

    int splitStr(std::string pszSrc, const char* flag, std::vector<std::string>& vecDat);

#ifdef RUSH_PLATFORM_WINDOWS
    bool RunExe(const std::string& fileName, const std::string& cmdLine);
#endif

    //////////////////////////////////////////////////////////////////////////

    class PointI
    {
    public:
        PointI() : xp(0), yp(0) {}
        PointI(int x, int y) : xp(x), yp(y) {}

        int x() const { return xp; }
        int y() const { return yp; }

    private:
        int xp, yp;
    };

    //////////////////////////////////////////////////////////////////////////


    class Rect
    {
    public:
        Rect() : x1(0), y1(0), x2(-1), y2(-1) {}

        Rect(const PointI& leftTop, const PointI& rightBottom)
            : x1(leftTop.x()), y1(leftTop.y()), x2(rightBottom.x()), y2(rightBottom.y()) {}

        Rect(int aleft, int atop, int awidth, int aheight)
            : x1(aleft), y1(atop), x2(aleft + awidth - 1), y2(atop + aheight - 1) {}

        int width() const { return  x2 - x1 + 1; }
        int height() const { return  y2 - y1 + 1; }
        int x() const { return x1; }
        int y() const { return y1; }
        int left() const { return x1; }
        int right() const { return x2; }
        int top() const { return y1; }
        int bottom() const { return y2; }

        void setX(int x)
        {
            x1 = x;
        }

        void setY(int y)
        {
            y1 = y;
        }

        void setLeft(int pos)
        {
            x1 = pos;
        }

        void setTop(int pos)
        {
            y1 = pos;
        }

        void setRight(int pos)
        {
            x2 = pos;
        }

        void setBottom(int pos)
        {
            y2 = pos;
        }

        Rect adjusted(int xp1, int yp1, int xp2, int yp2) const
        {
            return Rect(PointI(x1 + xp1, y1 + yp1), PointI(x2 + xp2, y2 + yp2));
        }

        Rect intersected(const Rect& other) const
        {
            return *this & other;
        }

        bool contains(int ax, int ay) const
        {
            return contains(PointI(ax, ay), false);
        }

        bool contains(const PointI& p, bool proper) const
        {
            int l, r;
            if (x2 < x1 - 1) {
                l = x2;
                r = x1;
            }
            else {
                l = x1;
                r = x2;
            }
            if (proper) {
                if (p.x() <= l || p.x() >= r)
                    return false;
            }
            else {
                if (p.x() < l || p.x() > r)
                    return false;
            }
            int t, b;
            if (y2 < y1 - 1) {
                t = y2;
                b = y1;
            }
            else {
                t = y1;
                b = y2;
            }
            if (proper) {
                if (p.y() <= t || p.y() >= b)
                    return false;
            }
            else {
                if (p.y() < t || p.y() > b)
                    return false;
            }
            return true;
        }

        bool isNull() const
        {
            return x2 == x1 - 1 && y2 == y1 - 1;
        }

        Rect operator&(const Rect& r) const
        {
            if (isNull() || r.isNull())
                return Rect();
            int l1 = x1;
            int r1 = x1;
            if (x2 - x1 + 1 < 0)
                l1 = x2;
            else
                r1 = x2;
            int l2 = r.x1;
            int r2 = r.x1;
            if (r.x2 - r.x1 + 1 < 0)
                l2 = r.x2;
            else
                r2 = r.x2;
            if (l1 > r2 || l2 > r1)
                return Rect();
            int t1 = y1;
            int b1 = y1;
            if (y2 - y1 + 1 < 0)
                t1 = y2;
            else
                b1 = y2;
            int t2 = r.y1;
            int b2 = r.y1;
            if (r.y2 - r.y1 + 1 < 0)
                t2 = r.y2;
            else
                b2 = r.y2;
            if (t1 > b2 || t2 > b1)
                return Rect();
            Rect tmp;
            tmp.x1 = std::max(l1, l2);
            tmp.x2 = std::min(r1, r2);
            tmp.y1 = std::max(t1, t2);
            tmp.y2 = std::min(b1, b2);
            return tmp;
        }

        inline Rect united(const Rect& r) const
        {
            return *this | r;
        }

        Rect operator|(const Rect& r) const
        {
            if (isNull())
                return r;
            if (r.isNull())
                return *this;
            int l1 = x1;
            int r1 = x1;
            if (x2 - x1 + 1 < 0)
                l1 = x2;
            else
                r1 = x2;
            int l2 = r.x1;
            int r2 = r.x1;
            if (r.x2 - r.x1 + 1 < 0)
                l2 = r.x2;
            else
                r2 = r.x2;
            int t1 = y1;
            int b1 = y1;
            if (y2 - y1 + 1 < 0)
                t1 = y2;
            else
                b1 = y2;
            int t2 = r.y1;
            int b2 = r.y1;
            if (r.y2 - r.y1 + 1 < 0)
                t2 = r.y2;
            else
                b2 = r.y2;
            Rect tmp;
            tmp.x1 = std::min(l1, l2);
            tmp.x2 = std::max(r1, r2);
            tmp.y1 = std::min(t1, t2);
            tmp.y2 = std::max(b1, b2);
            return tmp;
        }

    protected:
        int x1;
        int y1;
        int x2;
        int y2;
    };

} // namespace rush

#endif // Common_h__
