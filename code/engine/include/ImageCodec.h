#ifndef ImageCodec_h__
#define ImageCodec_h__

#include "core/Core.h"
#include "render/RDefines.h"

namespace rush
{

    enum class ImageQuality
    {
        Default,
        Highest,
        Fastest,
        NormalDefault,
        NormalHighest,
        NormalFastest,
        Count
    };

    enum class EncodingType
    {
        Unorm,
        Int,
        Uint,
        Float,
        Snorm,
        Count
    };

    struct ImageBlockInfo
    {
        uint8_t bitsPerPixel;
        uint8_t blockWidth;
        uint8_t blockHeight;
        uint8_t blockSize;
        uint8_t minBlockX;
        uint8_t minBlockY;
        uint8_t depthBits;
        uint8_t stencilBits;
        uint8_t rBits;
        uint8_t gBits;
        uint8_t bBits;
        uint8_t aBits;
        uint8_t encoding;
    };

    struct TextureInfo
    {
        TextureFormat format;       //!< Texture format.
        uint32_t storageSize;       //!< Total amount of bytes required to store texture.
        uint16_t width;             //!< Texture width.
        uint16_t height;            //!< Texture height.
        uint16_t depth;             //!< Texture depth.
        uint16_t numLayers;         //!< Number of layers in texture array.
        uint8_t numMips;            //!< Number of MIP maps.
        uint8_t bitsPerPixel;       //!< Format bits per pixel.
        bool    cubeMap;            //!< Texture is cubemap.
    };

    struct RawImageData
    {
        ~RawImageData();
        uint8_t* data = nullptr;
        uint64_t size = 0;
        int width = 0;
        int height = 0;
        uint32_t channels = 0;
    };

    class ImageCodec
    {
    public:
        static bool LoadImage2D(RawImageData& data, const uint8_t* src, uint64_t dataSize, bool flip_y, String& error);

        static const ImageBlockInfo& GetBlockInfo(TextureFormat _format);
        static uint8_t GetBitsPerPixel(TextureFormat _format);
        static uint8_t GetBytesPerPixel(TextureFormat _format);
        static uint8_t GetBlockSize(TextureFormat _format);
        static uint8_t ImageCodec::CalcNumMips(bool _hasMips, uint16_t _width, uint16_t _height, uint16_t _depth /*= 1*/);

        static uint32_t GetTextureInfo(TextureInfo* _info, uint16_t _width, uint16_t _height, uint16_t _depth, bool _cubeMap, bool _hasMips, uint16_t _numLayers, TextureFormat _format);

        static void EncodeDTX(TextureFormat format, uint8_t* dest, const uint8_t* src, uint32_t width, uint32_t height, uint32_t depth, ImageQuality quality);

    };

}

#endif // ImageCodec_h__
