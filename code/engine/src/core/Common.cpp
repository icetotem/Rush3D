#include "stdafx.h"
#include "Core/Common.h"
#include "stduuid/include/uuid.h"
#include "xxHash/xxhash.h"
#include <locale.h>

#ifdef RUSH_PLATFORM_WINDOWS
#include <Windows.h>
#else
#include <locale.h>
#endif

namespace rush
{
    using UUID = uuids::uuid;

    UUID GenUUID()
    {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size> {};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::mt19937 generator(seq);
        uuids::uuid_random_generator gen{ generator };

        uuids::uuid const id = gen();
        RUSH_ASSERT(!id.is_nil());
        RUSH_ASSERT(id.version() == uuids::uuid_version::random_number_based);
        RUSH_ASSERT(id.variant() == uuids::uuid_variant::rfc);
        return id;
    }

    std::string Uuid2Str(const UUID& uuid)
    {
        return uuids::to_string(uuid);
    }
        
    inline uint64_t Uuid2Hash(const UUID& uuid)
    {
        auto h = std::hash<UUID>{};
        return h(uuid);
    }

    std::string GenerateUUIDStr()
    {
        return Uuid2Str(GenUUID());
    }

    uint64_t GenerateUUID64()
    {
        return Uuid2Hash(GenUUID());
    }


    /* POINTER defines a generic pointer type */
    typedef unsigned char* POINTER;

    /* UINT2 defines a two byte word */
    typedef unsigned short int UINT2;

    /* UINT4 defines a four byte word */
    typedef unsigned long int UINT4;

    /* MD5 context. */
    typedef struct {
        UINT4 state[4];                                   /* state (ABCD) */
        UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
        unsigned char buffer[64];                         /* input buffer */
    } MD5_CTX;

    /* Constants for MD5Transform routine.
    */

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

    static void MD5Transform(UINT4[4], unsigned char[64]);
    static void Encode(unsigned char*, UINT4*, unsigned int);
    static void Decode(UINT4*, unsigned char*, unsigned int);
    static void MD5_memcpy(POINTER, POINTER, unsigned int);
    static void MD5_memset(POINTER, int, unsigned int);

    static unsigned char PADDING[64] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    /* F, G, H and I are basic MD5 functions.
    */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

    /* ROTATE_LEFT rotates x left n bits.
    */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

    /* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
    Rotation is separate from addition to prevent recomputation.
    */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
(a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

    /* MD5 initialization. Begins an MD5 operation, writing a new context.
    */
    static void MD5Init(MD5_CTX* context) /* context */
    {
        context->count[0] = context->count[1] = 0;
        /* Load magic initialization constants.
        */
        context->state[0] = 0x67452301;
        context->state[1] = 0xefcdab89;
        context->state[2] = 0x98badcfe;
        context->state[3] = 0x10325476;
    }

    /* MD5 block update operation. Continues an MD5 message-digest
    operation, processing another message block, and updating the
    context.
    */
    static void MD5Update(MD5_CTX* context, unsigned char* input, unsigned int inputLen)
    {
        unsigned int i, index, partLen;

        /* Compute number of bytes mod 64 */
        index = (unsigned int)((context->count[0] >> 3) & 0x3F);

        /* Update number of bits */
        if ((context->count[0] += ((UINT4)inputLen << 3))
            < ((UINT4)inputLen << 3))
            context->count[1]++;
        context->count[1] += ((UINT4)inputLen >> 29);

        partLen = 64 - index;

        /* Transform as many times as possible.
        */
        if (inputLen >= partLen) {
            MD5_memcpy
            ((POINTER)&context->buffer[index], (POINTER)input, partLen);
            MD5Transform(context->state, context->buffer);

            for (i = partLen; i + 63 < inputLen; i += 64)
                MD5Transform(context->state, &input[i]);

            index = 0;
        }
        else
            i = 0;

        /* Buffer remaining input */
        MD5_memcpy
        ((POINTER)&context->buffer[index], (POINTER)&input[i],
            inputLen - i);
    }

    /* MD5 finalization. Ends an MD5 message-digest operation, writing the
    the message digest and zeroizing the context.
    */
    static void MD5Final(unsigned char digest[16], MD5_CTX* context)
    {
        unsigned char bits[8];
        unsigned int index, padLen;

        /* Save number of bits */
        Encode(bits, context->count, 8);

        /* Pad out to 56 mod 64.
        */
        index = (unsigned int)((context->count[0] >> 3) & 0x3f);
        padLen = (index < 56) ? (56 - index) : (120 - index);
        MD5Update(context, PADDING, padLen);

        /* Append length (before padding) */
        MD5Update(context, bits, 8);

        /* Store state in digest */
        Encode(digest, context->state, 16);

        /* Zeroize sensitive information.
        */
        MD5_memset((POINTER)context, 0, sizeof(*context));
    }

    /* MD5 basic transformation. Transforms state based on block.
    */
    static void MD5Transform(UINT4 state[4], unsigned char block[64])
    {
        UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

        Decode(x, block, 64);

        /* Round 1 */
        FF(a, b, c, d, x[0], S11, 0xd76aa478); /* 1 */
        FF(d, a, b, c, x[1], S12, 0xe8c7b756); /* 2 */
        FF(c, d, a, b, x[2], S13, 0x242070db); /* 3 */
        FF(b, c, d, a, x[3], S14, 0xc1bdceee); /* 4 */
        FF(a, b, c, d, x[4], S11, 0xf57c0faf); /* 5 */
        FF(d, a, b, c, x[5], S12, 0x4787c62a); /* 6 */
        FF(c, d, a, b, x[6], S13, 0xa8304613); /* 7 */
        FF(b, c, d, a, x[7], S14, 0xfd469501); /* 8 */
        FF(a, b, c, d, x[8], S11, 0x698098d8); /* 9 */
        FF(d, a, b, c, x[9], S12, 0x8b44f7af); /* 10 */
        FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
        FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
        FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
        FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
        FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
        FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

                                                /* Round 2 */
        GG(a, b, c, d, x[1], S21, 0xf61e2562); /* 17 */
        GG(d, a, b, c, x[6], S22, 0xc040b340); /* 18 */
        GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
        GG(b, c, d, a, x[0], S24, 0xe9b6c7aa); /* 20 */
        GG(a, b, c, d, x[5], S21, 0xd62f105d); /* 21 */
        GG(d, a, b, c, x[10], S22, 0x2441453); /* 22 */
        GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
        GG(b, c, d, a, x[4], S24, 0xe7d3fbc8); /* 24 */
        GG(a, b, c, d, x[9], S21, 0x21e1cde6); /* 25 */
        GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
        GG(c, d, a, b, x[3], S23, 0xf4d50d87); /* 27 */

        GG(b, c, d, a, x[8], S24, 0x455a14ed); /* 28 */
        GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
        GG(d, a, b, c, x[2], S22, 0xfcefa3f8); /* 30 */
        GG(c, d, a, b, x[7], S23, 0x676f02d9); /* 31 */
        GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

                                                /* Round 3 */
        HH(a, b, c, d, x[5], S31, 0xfffa3942); /* 33 */
        HH(d, a, b, c, x[8], S32, 0x8771f681); /* 34 */
        HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
        HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
        HH(a, b, c, d, x[1], S31, 0xa4beea44); /* 37 */
        HH(d, a, b, c, x[4], S32, 0x4bdecfa9); /* 38 */
        HH(c, d, a, b, x[7], S33, 0xf6bb4b60); /* 39 */
        HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
        HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
        HH(d, a, b, c, x[0], S32, 0xeaa127fa); /* 42 */
        HH(c, d, a, b, x[3], S33, 0xd4ef3085); /* 43 */
        HH(b, c, d, a, x[6], S34, 0x4881d05); /* 44 */
        HH(a, b, c, d, x[9], S31, 0xd9d4d039); /* 45 */
        HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
        HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
        HH(b, c, d, a, x[2], S34, 0xc4ac5665); /* 48 */

                                               /* Round 4 */
        II(a, b, c, d, x[0], S41, 0xf4292244); /* 49 */
        II(d, a, b, c, x[7], S42, 0x432aff97); /* 50 */
        II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
        II(b, c, d, a, x[5], S44, 0xfc93a039); /* 52 */
        II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
        II(d, a, b, c, x[3], S42, 0x8f0ccc92); /* 54 */
        II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
        II(b, c, d, a, x[1], S44, 0x85845dd1); /* 56 */
        II(a, b, c, d, x[8], S41, 0x6fa87e4f); /* 57 */
        II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
        II(c, d, a, b, x[6], S43, 0xa3014314); /* 59 */
        II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
        II(a, b, c, d, x[4], S41, 0xf7537e82); /* 61 */
        II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
        II(c, d, a, b, x[2], S43, 0x2ad7d2bb); /* 63 */
        II(b, c, d, a, x[9], S44, 0xeb86d391); /* 64 */

        state[0] += a;
        state[1] += b;
        state[2] += c;
        state[3] += d;

        /* Zeroize sensitive information.
        */
        MD5_memset((POINTER)x, 0, sizeof(x));
    }

    /* Encodes input (UINT4) into output (unsigned char). Assumes len is
    a multiple of 4.
    */
    static void Encode(unsigned char* output, UINT4* input, unsigned int len)
    {
        unsigned int i, j;

        for (i = 0, j = 0; j < len; i++, j += 4) {
            output[j] = (unsigned char)(input[i] & 0xff);
            output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
            output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
            output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
        }
    }

    /* Decodes input (unsigned char) into output (UINT4). Assumes len is
    a multiple of 4.
    */
    static void Decode(UINT4* output, unsigned char* input, unsigned int len)
    {
        unsigned int i, j;

        for (i = 0, j = 0; j < len; i++, j += 4)
            output[i] = ((UINT4)input[j]) | (((UINT4)input[j + 1]) << 8) |
            (((UINT4)input[j + 2]) << 16) | (((UINT4)input[j + 3]) << 24);
    }

    /* Note: Replace "for loop" with standard memcpy if possible.
    */

    static void MD5_memcpy(POINTER output, POINTER input, unsigned int len)
    {
        unsigned int i;

        for (i = 0; i < len; i++)
            output[i] = input[i];
    }

    /* Note: Replace "for loop" with standard memset if possible.
    */
    static void MD5_memset(POINTER output, int value, unsigned int len)
    {
        unsigned int i;

        for (i = 0; i < len; i++)
            ((char*)output)[i] = (char)value;
    }

    bool GetFileMD5(const std::string& filename, uint8_t md5[16])
    {
        std::ifstream fin(filename.c_str(), std::ifstream::in | std::ifstream::binary);
        if (fin)
        {
            MD5_CTX context;
            MD5Init(&context);

            fin.seekg(0, fin.end);
            const auto fileLength = fin.tellg();
            fin.seekg(0, fin.beg);

            const int bufferLen = 8192;
            std::unique_ptr<unsigned char[]> buffer{ new unsigned char[bufferLen] {} };
            unsigned long long totalReadCount = 0;
            decltype(fin.gcount()) readCount = 0;
            // 读取文件内容，调用MD5Update()更新MD5值
            while (fin.read(reinterpret_cast<char*>(buffer.get()), bufferLen))
            {
                readCount = fin.gcount();
                totalReadCount += readCount;
                MD5Update(&context, buffer.get(), static_cast<unsigned int>(readCount));
            }
            // 处理最后一次读到的数据
            readCount = fin.gcount();
            if (readCount > 0)
            {
                totalReadCount += readCount;
                MD5Update(&context, buffer.get(), static_cast<unsigned int>(readCount));
            }
            fin.close();

            // 数据完整性检查
            if (totalReadCount != fileLength)
            {
                LOG_ERROR("Failed reading file MD5 {0}", filename.c_str());
            }

            MD5Final(md5, &context);

            return true;
            //             // 获取MD5
            //             std::ostringstream oss;
            //             for (int i = 0; i < 16; ++i)
            //             {
            //                 oss << std::hex << std::setw(1) << std::setfill('0') << static_cast<unsigned int>(digest[i]);
            //             }
            //             oss << std::ends;
            // 
            //             return std::move(oss.str());
        }
        else
        {
            LOG_ERROR("Failed reading file MD5 {0}", filename.c_str());
            return false;
        }
    }

    std::string Md5_Str(uint8_t md5[16])
    {
        std::string md5String;
        char hexBuffer[3];
        for (int i = 0; i < 16; ++i)
        {
            if (md5[i] < 16)
            {
                sprintf(hexBuffer, "0%x", md5[i]);
            }
            else
            {
                sprintf(hexBuffer, "%x", md5[i]);
            }
            md5String.append(hexBuffer);
        }
        return md5String;
    }



    extern "C"
    {

        uint64_t Hash64(const std::string& value)
        {
            XXH64_hash_t const seed = 0;
            return XXH64(value.c_str(), value.size(), seed);
        }

    }

    //
// Depending on the url parameter in base64_chars, one of
// two sets of base64 characters needs to be chosen.
// They differ in their last two characters.
//
    static const char* base64_chars[2] = {
                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                 "abcdefghijklmnopqrstuvwxyz"
                 "0123456789"
                 "+/",

                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                 "abcdefghijklmnopqrstuvwxyz"
                 "0123456789"
                 "-_" };

    static unsigned int pos_of_char(const unsigned char chr) {
        //
        // Return the position of chr within base64_encode()
        //

        if (chr >= 'A' && chr <= 'Z') return chr - 'A';
        else if (chr >= 'a' && chr <= 'z') return chr - 'a' + ('Z' - 'A') + 1;
        else if (chr >= '0' && chr <= '9') return chr - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
        else if (chr == '+' || chr == '-') return 62; // Be liberal with input and accept both url ('-') and non-url ('+') base 64 characters (
        else if (chr == '/' || chr == '_') return 63; // Ditto for '/' and '_'
        else
            //
            // 2020-10-23: Throw std::exception rather than const char*
            //(Pablo Martin-Gomez, https://github.com/Bouska)
            //
            throw std::runtime_error("Input is not valid base64-encoded data.");
    }

    static std::string insert_linebreaks(std::string str, size_t distance) {
        //
        // Provided by https://github.com/JomaCorpFX, adapted by me.
        //
        if (!str.length()) {
            return "";
        }

        size_t pos = distance;

        while (pos < str.size()) {
            str.insert(pos, "\n");
            pos += distance + 1;
        }

        return str;
    }

    template <typename String, unsigned int line_length>
    static std::string encode_with_line_breaks(String s) {
        return insert_linebreaks(base64_encode(s, false), line_length);
    }

    template <typename String>
    static std::string encode_pem(String s) {
        return encode_with_line_breaks<String, 64>(s);
    }

    template <typename String>
    static std::string encode_mime(String s) {
        return encode_with_line_breaks<String, 76>(s);
    }

    template <typename String>
    static std::string encode(String s, bool url) {
        return base64_encode(reinterpret_cast<const unsigned char*>(s.data()), s.length(), url);
    }

    std::string base64_encode(unsigned char const* bytes_to_encode, size_t in_len, bool url) {

        size_t len_encoded = (in_len + 2) / 3 * 4;

        unsigned char trailing_char = url ? '.' : '=';

        //
        // Choose set of base64 characters. They differ
        // for the last two positions, depending on the url
        // parameter.
        // A bool (as is the parameter url) is guaranteed
        // to evaluate to either 0 or 1 in C++ therefore,
        // the correct character set is chosen by subscripting
        // base64_chars with url.
        //
        const char* base64_chars_ = base64_chars[url];

        std::string ret;
        ret.reserve(len_encoded);

        unsigned int pos = 0;

        while (pos < in_len) {
            ret.push_back(base64_chars_[(bytes_to_encode[pos + 0] & 0xfc) >> 2]);

            if (pos + 1 < in_len) {
                ret.push_back(base64_chars_[((bytes_to_encode[pos + 0] & 0x03) << 4) + ((bytes_to_encode[pos + 1] & 0xf0) >> 4)]);

                if (pos + 2 < in_len) {
                    ret.push_back(base64_chars_[((bytes_to_encode[pos + 1] & 0x0f) << 2) + ((bytes_to_encode[pos + 2] & 0xc0) >> 6)]);
                    ret.push_back(base64_chars_[bytes_to_encode[pos + 2] & 0x3f]);
                }
                else {
                    ret.push_back(base64_chars_[(bytes_to_encode[pos + 1] & 0x0f) << 2]);
                    ret.push_back(trailing_char);
                }
            }
            else {

                ret.push_back(base64_chars_[(bytes_to_encode[pos + 0] & 0x03) << 4]);
                ret.push_back(trailing_char);
                ret.push_back(trailing_char);
            }

            pos += 3;
        }


        return ret;
    }

    template <typename String>
    static std::string decode(String encoded_string, bool remove_linebreaks) {
        //
        // decode(…) is templated so that it can be used with String = const std::string&
        // or std::string_view (requires at least C++17)
        //

        if (encoded_string.empty()) return std::string();

        if (remove_linebreaks) {

            std::string copy(encoded_string);

            copy.erase(std::remove(copy.begin(), copy.end(), '\n'), copy.end());

            return base64_decode(copy, false);
        }

        size_t length_of_string = encoded_string.length();
        size_t pos = 0;

        //
        // The approximate length (bytes) of the decoded string might be one or
        // two bytes smaller, depending on the amount of trailing equal signs
        // in the encoded string. This approximation is needed to reserve
        // enough space in the string to be returned.
        //
        size_t approx_length_of_decoded_string = length_of_string / 4 * 3;
        std::string ret;
        ret.reserve(approx_length_of_decoded_string);

        while (pos < length_of_string) {
            //
            // Iterate over encoded input string in chunks. The size of all
            // chunks except the last one is 4 bytes.
            //
            // The last chunk might be padded with equal signs or dots
            // in order to make it 4 bytes in size as well, but this
            // is not required as per RFC 2045.
            //
            // All chunks except the last one produce three output bytes.
            //
            // The last chunk produces at least one and up to three bytes.
            //

            size_t pos_of_char_1 = pos_of_char(encoded_string[pos + 1]);

            //
            // Emit the first output byte that is produced in each chunk:
            //
            ret.push_back(static_cast<std::string::value_type>(((pos_of_char(encoded_string[pos + 0])) << 2) + ((pos_of_char_1 & 0x30) >> 4)));

            if ((pos + 2 < length_of_string) &&  // Check for data that is not padded with equal signs (which is allowed by RFC 2045)
                encoded_string[pos + 2] != '=' &&
                encoded_string[pos + 2] != '.'            // accept URL-safe base 64 strings, too, so check for '.' also.
                )
            {
                //
                // Emit a chunk's second byte (which might not be produced in the last chunk).
                //
                unsigned int pos_of_char_2 = pos_of_char(encoded_string[pos + 2]);
                ret.push_back(static_cast<std::string::value_type>(((pos_of_char_1 & 0x0f) << 4) + ((pos_of_char_2 & 0x3c) >> 2)));

                if ((pos + 3 < length_of_string) &&
                    encoded_string[pos + 3] != '=' &&
                    encoded_string[pos + 3] != '.'
                    )
                {
                    //
                    // Emit a chunk's third byte (which might not be produced in the last chunk).
                    //
                    ret.push_back(static_cast<std::string::value_type>(((pos_of_char_2 & 0x03) << 6) + pos_of_char(encoded_string[pos + 3])));
                }
            }

            pos += 4;
        }

        return ret;
    }

    std::string base64_decode(std::string const& s, bool remove_linebreaks) {
        return decode(s, remove_linebreaks);
    }

    std::string base64_encode(std::string const& s, bool url) {
        return encode(s, url);
    }

    std::string base64_encode_pem(std::string const& s) {
        return encode_pem(s);
    }

    std::string base64_encode_mime(std::string const& s) {
        return encode_mime(s);
    }

    std::string ws2s(const std::wstring& ws)
    {
#ifdef RUSH_PLATFORM_WINDOWS
        std::string result;
        //获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
        int len = WideCharToMultiByte(CP_ACP, 0, ws.c_str(), ws.size(), NULL, 0, NULL, NULL);
        char* buffer = new char[len + 1];
        //宽字节编码转换成多字节编码  
        WideCharToMultiByte(CP_ACP, 0, ws.c_str(), ws.size(), buffer, len, NULL, NULL);
        buffer[len] = '\0';
        //删除缓冲区并返回值  
        result.append(buffer);
        delete[] buffer;
        return result;
#else
        //string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
        //setlocale(LC_ALL, "chs");

        const wchar_t* _Source = ws.c_str();
        size_t _Dsize = 2 * ws.size() + 1;
        char* _Dest = new char[_Dsize];
        memset(_Dest, 0, _Dsize);
        wcstombs(_Dest, _Source, _Dsize);
        std::string result = _Dest;
        delete[]_Dest;

        //setlocale(LC_ALL, curLocale.c_str());

        return result;
#endif
    }

    std::wstring s2ws(const std::string& s)
    {
#ifdef RUSH_PLATFORM_WINDOWS
        std::wstring result;
        //获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
        int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), s.size(), NULL, 0);
        wchar_t* buffer = new wchar_t[len + 1];
        //多字节编码转换成宽字节编码  
        MultiByteToWideChar(CP_ACP, 0, s.c_str(), s.size(), buffer, len);
        buffer[len] = '\0';             //添加字符串结尾  
        //删除缓冲区并返回值  
        result.append(buffer);
        delete[] buffer;
        return result;
#else
        //setlocale(LC_ALL, "chs");

        const char* _Source = s.c_str();
        size_t _Dsize = s.size() + 1;
        wchar_t* _Dest = new wchar_t[_Dsize];
        wmemset(_Dest, 0, _Dsize);
        mbstowcs(_Dest, _Source, _Dsize);
        std::wstring result = _Dest;
        delete[]_Dest;

        //setlocale(LC_ALL, "C");

        return result;
#endif
    }

    int splitStr(std::string pszSrc, const char* flag, std::vector<std::string>& vecDat)
    {
        if (pszSrc.empty() || !flag)
            return -1;

        std::string strContent, strTemp;
        strContent = pszSrc;
        std::string::size_type nBeginPos = 0, nEndPos = 0;
        while (true)
        {
            nEndPos = strContent.find(flag, nBeginPos);
            if (nEndPos == std::string::npos)
            {
                strTemp = strContent.substr(nBeginPos, strContent.length());
                if (!strTemp.empty())
                {
                    vecDat.push_back(strTemp);
                }
                break;
            }
            strTemp = strContent.substr(nBeginPos, nEndPos - nBeginPos);
            nBeginPos = nEndPos + strlen(flag);
            vecDat.push_back(strTemp);
        }
        return vecDat.size();
    }

#ifdef RUSH_PLATFORM_WINDOWS
    bool RunExe(const std::string& fileName, const std::string& cmdLine)
    {
        DWORD    dwExitCode = -1;
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        //所要执行的命令
        char msg[1000] = { 0 };
        strcpy(msg, cmdLine.c_str());

        // Start the child process.创建子进程   
        if (!CreateProcessA(fileName.c_str(), // an exe file.   
            msg,        // parameter for your exe file.   
            NULL,             // Process handle not inheritable.   
            NULL,             // Thread handle not inheritable.   
            FALSE,            // Set handle inheritance to FALSE.   
            0,                // No creation flags.   
            NULL,             // Use parent's environment block.   
            NULL,             // Use parent's starting directory.  
                              // 传null默认为使用父目录启动子进程，如要改变子进程运行目录可以传相应路径
            &si,              // Pointer to STARTUPINFO structure.  
            &pi)             // Pointer to PROCESS_INFORMATION structure.  
            )
        {
            return false;
        }

        // Wait until child process exits.  等待子进程执行结束返回
        WaitForSingleObject(pi.hProcess, INFINITE);
        GetExitCodeProcess(pi.hProcess, &dwExitCode);

        // Close process and thread handles.   
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        if (0 != dwExitCode)
            return false;

        return true;
    }
#endif
}