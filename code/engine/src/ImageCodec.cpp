#include "stdafx.h"
#include "ImageCodec.h"

#include <libsquish/squish.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace rush
{

    static uint32_t s_squishQuality[] =
    {
        // Standard
        squish::kColourClusterFit,          // Default
        squish::kColourIterativeClusterFit, // Highest
        squish::kColourRangeFit,            // Fastest
        // Normal map
        squish::kColourClusterFit,          // Default
        squish::kColourIterativeClusterFit, // Highest
        squish::kColourRangeFit,            // Fastest
    };

    static const ImageBlockInfo s_imageBlockInfo[] =
    {
        //  +--------------------------------------------- bits per pixel
        //  |   +----------------------------------------- block width
        //  |   |  +-------------------------------------- block height
        //  |   |  |   +---------------------------------- block size
        //  |   |  |   |  +------------------------------- min blocks x
        //  |   |  |   |  |  +---------------------------- min blocks y
        //  |   |  |   |  |  |   +------------------------ depth bits
        //  |   |  |   |  |  |   |  +--------------------- stencil bits
        //  |   |  |   |  |  |   |  |   +---+---+---+----- r, g, b, a bits
        //  |   |  |   |  |  |   |  |   r   g   b   a  +-- encoding type
        //  |   |  |   |  |  |   |  |   |   |   |   |  |
        {   0,  0, 0,  0, 0, 0,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) },  // Undefined
        {   8,  1, 1,  1, 1, 1,  0, 0,  8,  0,  0,  0, uint8_t(EncodingType::Unorm) },  // R8Unorm
        {   8,  1, 1,  1, 1, 1,  0, 0,  8,  0,  0,  0, uint8_t(EncodingType::Snorm) },  // R8Snorm
        {   8,  1, 1,  1, 1, 1,  0, 0,  8,  0,  0,  0, uint8_t(EncodingType::Uint) },   // R8Uint
        {   8,  1, 1,  1, 1, 1,  0, 0,  8,  0,  0,  0, uint8_t(EncodingType::Int) },    // R8Sint
        {  16,  1, 1,  2, 1, 1,  0, 0, 16,  0,  0,  0, uint8_t(EncodingType::Uint) },   // R16Uint
        {  16,  1, 1,  2, 1, 1,  0, 0, 16,  0,  0,  0, uint8_t(EncodingType::Snorm) },  // R16Sint
        {  16,  1, 1,  2, 1, 1,  0, 0, 16,  0,  0,  0, uint8_t(EncodingType::Float) },  // R16Float
        {  16,  1, 1,  2, 1, 1,  0, 0,  8,  8,  0,  0, uint8_t(EncodingType::Unorm) },  // RG8Unorm
        {  16,  1, 1,  2, 1, 1,  0, 0,  8,  8,  0,  0, uint8_t(EncodingType::Snorm) },  // RG8Snorm
        {  16,  1, 1,  2, 1, 1,  0, 0,  8,  8,  0,  0, uint8_t(EncodingType::Uint) },   // RG8Uint
        {  16,  1, 1,  2, 1, 1,  0, 0,  8,  8,  0,  0, uint8_t(EncodingType::Int) },    // RG8Sint
        {  32,  1, 1,  4, 1, 1,  0, 0, 32,  0,  0,  0, uint8_t(EncodingType::Float) },  // R32Float
        {  32,  1, 1,  4, 1, 1,  0, 0, 32,  0,  0,  0, uint8_t(EncodingType::Uint) },   // R32Uint
        {  32,  1, 1,  4, 1, 1,  0, 0, 32,  0,  0,  0, uint8_t(EncodingType::Int) },    // R32Sint
        {  32,  1, 1,  4, 1, 1,  0, 0, 16, 16,  0,  0, uint8_t(EncodingType::Uint) },   // RG16Uint
        {  32,  1, 1,  4, 1, 1,  0, 0, 16, 16,  0,  0, uint8_t(EncodingType::Int) },    // RG16Sint
        {  32,  1, 1,  4, 1, 1,  0, 0, 16, 16,  0,  0, uint8_t(EncodingType::Float) },  // RG16Float
        {  32,  1, 1,  4, 1, 1,  0, 0,  8,  8,  8,  8, uint8_t(EncodingType::Unorm) },  // RGBA8Unorm
        {  32,  1, 1,  4, 1, 1,  0, 0,  8,  8,  8,  8, uint8_t(EncodingType::Unorm) },  // RGBA8UnormSrgb
        {  32,  1, 1,  4, 1, 1,  0, 0,  8,  8,  8,  8, uint8_t(EncodingType::Snorm) },  // RGBA8Snorm
        {  32,  1, 1,  4, 1, 1,  0, 0,  8,  8,  8,  8, uint8_t(EncodingType::Uint) },   // RGBA8Uint
        {  32,  1, 1,  4, 1, 1,  0, 0,  8,  8,  8,  8, uint8_t(EncodingType::Int) },    // RGBA8Sint
        {  32,  1, 1,  4, 1, 1,  0, 0,  8,  8,  8,  8, uint8_t(EncodingType::Unorm) },  // BGRA8
        {  32,  1, 1,  4, 1, 1,  0, 0,  8,  8,  8,  8, uint8_t(EncodingType::Unorm) },  // BGRA8UnormSrgb
        {  32,  1, 1,  4, 1, 1,  0, 0, 10, 10, 10,  2, uint8_t(EncodingType::Unorm) },  // RGB10A2Unorm
        {  32,  1, 1,  4, 1, 1,  0, 0, 11, 11, 10,  0, uint8_t(EncodingType::Unorm) },  // RG11B10Ufloat
        {  32,  1, 1,  4, 1, 1,  0, 0,  9,  9,  9,  5, uint8_t(EncodingType::Float) },  // RGB9E5Ufloat
        {  64,  1, 1,  8, 1, 1,  0, 0, 32, 32,  0,  0, uint8_t(EncodingType::Float) },  // RG32Float
        {  64,  1, 1,  8, 1, 1,  0, 0, 32, 32,  0,  0, uint8_t(EncodingType::Uint) },   // RG32Uint
        {  64,  1, 1,  8, 1, 1,  0, 0, 32, 32,  0,  0, uint8_t(EncodingType::Int) },    // RG32Sint
        {  64,  1, 1,  8, 1, 1,  0, 0, 16, 16, 16, 16, uint8_t(EncodingType::Uint) },   // RGBA16Uint
        {  64,  1, 1,  8, 1, 1,  0, 0, 16, 16, 16, 16, uint8_t(EncodingType::Int) },    // RGBA16Sint
        {  64,  1, 1,  8, 1, 1,  0, 0, 16, 16, 16, 16, uint8_t(EncodingType::Float) },  // RGBA16Float
        { 128,  1, 1, 16, 1, 1,  0, 0, 32, 32, 32, 32, uint8_t(EncodingType::Float) },  // RGBA32Float
        { 128,  1, 1, 16, 1, 1,  0, 0, 32, 32, 32, 32, uint8_t(EncodingType::Int) },    // RGBA32Uint
        { 128,  1, 1, 16, 1, 1,  0, 0, 32, 32, 32, 32, uint8_t(EncodingType::Uint) },   // RGBA32Sint
        {   8,  1, 1,  1, 1, 1,  0, 8,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) },  // Stencil8
        {  16,  1, 1,  2, 1, 1, 16, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) },  // Depth16Unorm
        {  24,  1, 1,  3, 1, 1, 24, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) },  // Depth24Plus
        {  32,  1, 1,  4, 1, 1, 24, 8,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) },  // Depth24PlusStencil8
        {  32,  1, 1,  4, 1, 1, 32, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) },  // Depth32Float
        {  32,  1, 1,  4, 1, 1, 32, 0,  0,  0,  0,  0, uint8_t(EncodingType::Float) },  // Depth32FloatStencil8
        {   4,  4, 4,  8, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // BC1RGBAUnorm
        {   4,  4, 4,  8, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // BC1RGBAUnormSrgb
        {   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // BC2RGBAUnorm
        {   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // BC2RGBAUnormSrgb
        {   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // BC3RGBAUnorm
        {   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // BC3RGBAUnormSrgb
        {   4,  4, 4,  8, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // BC4RUnorm
        {   4,  4, 4,  8, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Snorm) }, // BC4RSnorm
        {   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // BC5RGUnorm
        {   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Snorm) }, // BC5RGSnorm
        {   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Float) }, // BC6HRGBUfloat
        {   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Float) }, // BC6HRGBFloat
        {   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // BC7RGBAUnorm
        {   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // BC7RGBAUnormSrgb

//         {   4,  4, 4,  8, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // ETC2
//         {   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // ETC2A
//         {   4,  4, 4,  8, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // ETC2A1
//         {   2,  8, 4,  8, 2, 2,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // PTC12
//         {   4,  4, 4,  8, 2, 2,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // PTC14
//         {   2,  8, 4,  8, 2, 2,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // PTC12A
//         {   4,  4, 4,  8, 2, 2,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // PTC14A
//         {   2,  8, 4,  8, 2, 2,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // PTC22
//         {   4,  4, 4,  8, 2, 2,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // PTC24
//         {   4,  4, 4,  8, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // ATC
//         {   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // ATCE
//         {   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // ATCI
//         {   8,  4, 4, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // ASTC4x4
//         {   6,  5, 5, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // ASTC5x5
//         {   4,  6, 6, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // ASTC6x6
//         {   4,  8, 5, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // ASTC8x5
//         {   3,  8, 6, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // ASTC8x6
//         {   3, 10, 5, 16, 1, 1,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // ASTC10x5
//         {   0,  0, 0,  0, 0, 0,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Count) }, // Unknown
//         {   1,  8, 1,  1, 1, 1,  0, 0,  1,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // R1
//         {   8,  1, 1,  1, 1, 1,  0, 0,  0,  0,  0,  8, uint8_t(EncodingType::Unorm) }, // A8
//         {  16,  1, 1,  2, 1, 1,  0, 0, 16,  0,  0,  0, uint8_t(EncodingType::Unorm) }, // R16
//         {  16,  1, 1,  2, 1, 1,  0, 0, 16,  0,  0,  0, uint8_t(EncodingType::Int) }, // R16I
//         {  24,  1, 1,  3, 1, 1,  0, 0,  8,  8,  8,  0, uint8_t(EncodingType::Unorm) }, // RGB8
//         {  24,  1, 1,  3, 1, 1,  0, 0,  8,  8,  8,  0, uint8_t(EncodingType::Int) }, // RGB8I
//         {  24,  1, 1,  3, 1, 1,  0, 0,  8,  8,  8,  0, uint8_t(EncodingType::Uint) }, // RGB8U
//         {  24,  1, 1,  3, 1, 1,  0, 0,  8,  8,  8,  0, uint8_t(EncodingType::Snorm) }, // RGB8S
//         {  64,  1, 1,  8, 1, 1,  0, 0, 16, 16, 16, 16, uint8_t(EncodingType::Unorm) }, // RGBA16
//         {  64,  1, 1,  8, 1, 1,  0, 0, 16, 16, 16, 16, uint8_t(EncodingType::Snorm) }, // RGBA16S
//         {  16,  1, 1,  2, 1, 1,  0, 0,  5,  6,  5,  0, uint8_t(EncodingType::Unorm) }, // R5G6B5
//         {  16,  1, 1,  2, 1, 1,  0, 0,  4,  4,  4,  4, uint8_t(EncodingType::Unorm) }, // RGBA4
//         {  16,  1, 1,  2, 1, 1,  0, 0,  5,  5,  5,  1, uint8_t(EncodingType::Unorm) }, // RGB5A1
//         {   0,  0, 0,  0, 0, 0,  0, 0,  0,  0,  0,  0, uint8_t(EncodingType::Count) }, // UnknownDepth
//         {  16,  1, 1,  2, 1, 1, 16, 0,  0,  0,  0,  0, uint8_t(EncodingType::Float) }, // D16F
//         {  24,  1, 1,  3, 1, 1, 24, 0,  0,  0,  0,  0, uint8_t(EncodingType::Float) }, // D24F
    };

    uint8_t ImageCodec::GetBitsPerPixel(TextureFormat _format)
    {
        return s_imageBlockInfo[(int)_format].bitsPerPixel;
    }

    uint8_t ImageCodec::GetBytesPerPixel(TextureFormat _format)
    {
        return GetBitsPerPixel(_format) / 8;
    }

    RawImageData::~RawImageData()
    {
        stbi_image_free(data);
    }

    bool ImageCodec::LoadImage2D(RawImageData& data, const uint8_t* src, uint64_t dataSize, bool flip_y, String& error)
    {
        static const uint8_t comp_map[5] = {
            0, //
            1, //
            2, //
            4, //
            4  //
        };

        static const uint32_t channels[5] = {
            STBI_default,    // only used for req_comp
            STBI_grey,       //
            STBI_grey_alpha, //
            STBI_rgb_alpha,  //
            STBI_rgb_alpha   //
        };

        // Force loading 4 channel images to 3 channel by stb_image because Dawn doesn't
        // support 3 channel formats currently. The group is discussing on whether
        // WebGPU should support 3 channel format.
        // https://github.com/gpuweb/gpuweb/issues/66#issuecomment-410021505
        int read_comps = 4;
        stbi_set_flip_vertically_on_load(flip_y);
        stbi_uc* pixel_data = stbi_load_from_memory(src, dataSize,
            &data.width,
            &data.height,
            &read_comps,
            channels[read_comps]
        );
        if (pixel_data == nullptr)
        {
            error = stbi_failure_reason();
            return false;
        }
        data.data = pixel_data;
        data.channels = comp_map[read_comps];
        data.size = data.width * data.height * data.channels;
        return true;
    }

    const ImageBlockInfo& ImageCodec::GetBlockInfo(TextureFormat _format)
    {
        return s_imageBlockInfo[(int)_format];
    }

    uint8_t ImageCodec::GetBlockSize(TextureFormat _format)
    {
        return s_imageBlockInfo[(int)_format].blockSize;
    }

    uint8_t ImageCodec::CalcNumMips(bool _hasMips, uint16_t _width, uint16_t _height, uint16_t _depth /*= 1*/)
    {
        if (_hasMips)
        {
            const uint32_t max = std::max<uint16_t>(std::max<uint16_t>(_width, _height), _depth);
            const uint32_t num = 1 + uint32_t(std::log2((int32_t)max));
            return uint8_t(num);
        }
        return 1;
    }

    uint32_t ImageCodec::GetTextureInfo(TextureInfo* _info, uint16_t _width, uint16_t _height, uint16_t _depth, bool _cubeMap, bool _hasMips, uint16_t _numLayers, TextureFormat _format)
    {
        const ImageBlockInfo& blockInfo = GetBlockInfo(_format);
        const uint8_t  bpp = blockInfo.bitsPerPixel;
        const uint16_t blockWidth = blockInfo.blockWidth;
        const uint16_t blockHeight = blockInfo.blockHeight;
        const uint16_t minBlockX = blockInfo.minBlockX;
        const uint16_t minBlockY = blockInfo.minBlockY;
        const uint8_t  blockSize = blockInfo.blockSize;

        _width = std::max<uint16_t>(blockWidth * minBlockX, ((_width + blockWidth - 1) / blockWidth) * blockWidth);
        _height = std::max<uint16_t>(blockHeight * minBlockY, ((_height + blockHeight - 1) / blockHeight) * blockHeight);
        _depth = std::max<uint16_t>(1, _depth);
        const uint8_t  numMips = CalcNumMips(_hasMips, _width, _height, _depth);
        const uint32_t sides = _cubeMap ? 6 : 1;

        uint32_t width = _width;
        uint32_t height = _height;
        uint32_t depth = _depth;
        uint32_t size = 0;

        for (uint32_t lod = 0; lod < numMips; ++lod)
        {
            width = std::max<uint32_t>(blockWidth * minBlockX, ((width + blockWidth - 1) / blockWidth) * blockWidth);
            height = std::max<uint32_t>(blockHeight * minBlockY, ((height + blockHeight - 1) / blockHeight) * blockHeight);
            depth = std::max<uint32_t>(1, depth);

            size += uint32_t(uint64_t(width / blockWidth * height / blockHeight * depth) * blockSize * sides);

            width >>= 1;
            height >>= 1;
            depth >>= 1;
        }

        size *= _numLayers;

        if (NULL != _info)
        {
            _info->format = _format;
            _info->width = _width;
            _info->height = _height;
            _info->depth = _depth;
            _info->numMips = numMips;
            _info->numLayers = _numLayers;
            _info->cubeMap = _cubeMap;
            _info->storageSize = size;
            _info->bitsPerPixel = bpp;
        }

        return size;
    }

    void ImageCodec::EncodeDTX(TextureFormat _format, uint8_t* _dst, const uint8_t* _src, uint32_t _width, uint32_t _height, uint32_t _depth, ImageQuality quality)
    {
        const uint8_t* src = (const uint8_t*)_src;
        uint8_t* dst = (uint8_t*)_dst;

        const uint32_t srcPitch = _width * 4;
        const uint32_t srcSlice = _height * srcPitch;
        const uint32_t dstBpp = GetBitsPerPixel(_format);
        const uint32_t dstPitch = _width * dstBpp / 8;
        const uint32_t dstSlice = _height * dstPitch;

        for (uint32_t zz = 0; zz < _depth; ++zz, src += srcSlice, dst += dstSlice)
        {
            squish::CompressImage(src, _width, _height, _dst
                , s_squishQuality[(int)quality]
                | (_format == TextureFormat::BC2RGBAUnorm ? squish::kDxt3
                    : _format == TextureFormat::BC3RGBAUnorm ? squish::kDxt5
                    : _format == TextureFormat::BC4RUnorm ? squish::kBc4
                    : _format == TextureFormat::BC5RGSnorm ? squish::kBc5
                    : squish::kDxt1)
            );
        }
    }



}
