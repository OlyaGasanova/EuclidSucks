#ifndef H_DDS
#define H_DDS

#include "utils.h"
#include "context.h"

enum DXGI_FORMAT {
  DXGI_FORMAT_UNKNOWN,
  DXGI_FORMAT_R32G32B32A32_TYPELESS,
  DXGI_FORMAT_R32G32B32A32_FLOAT,
  DXGI_FORMAT_R32G32B32A32_UINT,
  DXGI_FORMAT_R32G32B32A32_SINT,
  DXGI_FORMAT_R32G32B32_TYPELESS,
  DXGI_FORMAT_R32G32B32_FLOAT,
  DXGI_FORMAT_R32G32B32_UINT,
  DXGI_FORMAT_R32G32B32_SINT,
  DXGI_FORMAT_R16G16B16A16_TYPELESS,
  DXGI_FORMAT_R16G16B16A16_FLOAT,
  DXGI_FORMAT_R16G16B16A16_UNORM,
  DXGI_FORMAT_R16G16B16A16_UINT,
  DXGI_FORMAT_R16G16B16A16_SNORM,
  DXGI_FORMAT_R16G16B16A16_SINT,
  DXGI_FORMAT_R32G32_TYPELESS,
  DXGI_FORMAT_R32G32_FLOAT,
  DXGI_FORMAT_R32G32_UINT,
  DXGI_FORMAT_R32G32_SINT,
  DXGI_FORMAT_R32G8X24_TYPELESS,
  DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
  DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,
  DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,
  DXGI_FORMAT_R10G10B10A2_TYPELESS,
  DXGI_FORMAT_R10G10B10A2_UNORM,
  DXGI_FORMAT_R10G10B10A2_UINT,
  DXGI_FORMAT_R11G11B10_FLOAT,
  DXGI_FORMAT_R8G8B8A8_TYPELESS,
  DXGI_FORMAT_R8G8B8A8_UNORM,
  DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
  DXGI_FORMAT_R8G8B8A8_UINT,
  DXGI_FORMAT_R8G8B8A8_SNORM,
  DXGI_FORMAT_R8G8B8A8_SINT,
  DXGI_FORMAT_R16G16_TYPELESS,
  DXGI_FORMAT_R16G16_FLOAT,
  DXGI_FORMAT_R16G16_UNORM,
  DXGI_FORMAT_R16G16_UINT,
  DXGI_FORMAT_R16G16_SNORM,
  DXGI_FORMAT_R16G16_SINT,
  DXGI_FORMAT_R32_TYPELESS,
  DXGI_FORMAT_D32_FLOAT,
  DXGI_FORMAT_R32_FLOAT,
  DXGI_FORMAT_R32_UINT,
  DXGI_FORMAT_R32_SINT,
  DXGI_FORMAT_R24G8_TYPELESS,
  DXGI_FORMAT_D24_UNORM_S8_UINT,
  DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
  DXGI_FORMAT_X24_TYPELESS_G8_UINT,
  DXGI_FORMAT_R8G8_TYPELESS,
  DXGI_FORMAT_R8G8_UNORM,
  DXGI_FORMAT_R8G8_UINT,
  DXGI_FORMAT_R8G8_SNORM,
  DXGI_FORMAT_R8G8_SINT,
  DXGI_FORMAT_R16_TYPELESS,
  DXGI_FORMAT_R16_FLOAT,
  DXGI_FORMAT_D16_UNORM,
  DXGI_FORMAT_R16_UNORM,
  DXGI_FORMAT_R16_UINT,
  DXGI_FORMAT_R16_SNORM,
  DXGI_FORMAT_R16_SINT,
  DXGI_FORMAT_R8_TYPELESS,
  DXGI_FORMAT_R8_UNORM,
  DXGI_FORMAT_R8_UINT,
  DXGI_FORMAT_R8_SNORM,
  DXGI_FORMAT_R8_SINT,
  DXGI_FORMAT_A8_UNORM,
  DXGI_FORMAT_R1_UNORM,
  DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
  DXGI_FORMAT_R8G8_B8G8_UNORM,
  DXGI_FORMAT_G8R8_G8B8_UNORM,
  DXGI_FORMAT_BC1_TYPELESS,
  DXGI_FORMAT_BC1_UNORM,
  DXGI_FORMAT_BC1_UNORM_SRGB,
  DXGI_FORMAT_BC2_TYPELESS,
  DXGI_FORMAT_BC2_UNORM,
  DXGI_FORMAT_BC2_UNORM_SRGB,
  DXGI_FORMAT_BC3_TYPELESS,
  DXGI_FORMAT_BC3_UNORM,
  DXGI_FORMAT_BC3_UNORM_SRGB,
  DXGI_FORMAT_BC4_TYPELESS,
  DXGI_FORMAT_BC4_UNORM,
  DXGI_FORMAT_BC4_SNORM,
  DXGI_FORMAT_BC5_TYPELESS,
  DXGI_FORMAT_BC5_UNORM,
  DXGI_FORMAT_BC5_SNORM,
  DXGI_FORMAT_B5G6R5_UNORM,
  DXGI_FORMAT_B5G5R5A1_UNORM,
  DXGI_FORMAT_B8G8R8A8_UNORM,
  DXGI_FORMAT_B8G8R8X8_UNORM,
  DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
  DXGI_FORMAT_B8G8R8A8_TYPELESS,
  DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
  DXGI_FORMAT_B8G8R8X8_TYPELESS,
  DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
  DXGI_FORMAT_BC6H_TYPELESS,
  DXGI_FORMAT_BC6H_UF16,
  DXGI_FORMAT_BC6H_SF16,
  DXGI_FORMAT_BC7_TYPELESS,
  DXGI_FORMAT_BC7_UNORM,
  DXGI_FORMAT_BC7_UNORM_SRGB,
  DXGI_FORMAT_AYUV,
  DXGI_FORMAT_Y410,
  DXGI_FORMAT_Y416,
  DXGI_FORMAT_NV12,
  DXGI_FORMAT_P010,
  DXGI_FORMAT_P016,
  DXGI_FORMAT_420_OPAQUE,
  DXGI_FORMAT_YUY2,
  DXGI_FORMAT_Y210,
  DXGI_FORMAT_Y216,
  DXGI_FORMAT_NV11,
  DXGI_FORMAT_AI44,
  DXGI_FORMAT_IA44,
  DXGI_FORMAT_P8,
  DXGI_FORMAT_A8P8,
  DXGI_FORMAT_B4G4R4A4_UNORM,
  DXGI_FORMAT_P208,
  DXGI_FORMAT_V208,
  DXGI_FORMAT_V408,
  DXGI_FORMAT_FORCE_UINT
} ;

bool loadDDS(Stream *stream, Texture::Desc &desc) {
    enum FOURCC {
        FOURCC_DXT1    = 0x31545844,
        FOURCC_DXT3    = 0x33545844,
        FOURCC_DXT5    = 0x35545844,
        FOURCC_BC4U    = 0x55344342,
        FOURCC_BC5U    = 0x55354342,
        FOURCC_ATI1    = 0x31495441,
        FOURCC_ATI2    = 0x32495441,
        FOURCC_DXGI    = 0x30315844,
        FOURCC_R16F    = 0x0000006F,
        FOURCC_RG16F   = 0x00000070,
        FOURCC_RGBA16F = 0x00000071,
        FOURCC_R32F    = 0x00000072,
        FOURCC_RG32F   = 0x00000073,
        FOURCC_RGBA32F = 0x00000074,
    };

    enum DXGI {
        DXGI_FORMAT_R32G32B32A32_FLOAT  = 2,
        DXGI_FORMAT_R32G32B32_FLOAT     = 6,
        DXGI_FORMAT_R16G16B16A16_FLOAT  = 10,
        DXGI_FORMAT_R32G32_FLOAT        = 16,
        DXGI_FORMAT_R11G11B10_FLOAT     = 26,
        DXGI_FORMAT_R8G8B8A8_UNORM      = 28,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
        DXGI_FORMAT_R16G16_FLOAT        = 34,
        DXGI_FORMAT_R32_FLOAT           = 41,
        DXGI_FORMAT_R8G8_UNORM          = 49,
        DXGI_FORMAT_R16_FLOAT           = 54,
        DXGI_FORMAT_R8_UNORM            = 61,
        DXGI_FORMAT_BC1_UNORM           = 71,
        DXGI_FORMAT_BC1_UNORM_SRGB      = 72,
        DXGI_FORMAT_BC2_UNORM           = 74,
        DXGI_FORMAT_BC2_UNORM_SRGB      = 75,
        DXGI_FORMAT_BC3_UNORM           = 77,
        DXGI_FORMAT_BC3_UNORM_SRGB      = 78,
        DXGI_FORMAT_BC4_UNORM           = 80,
        DXGI_FORMAT_BC5_UNORM           = 83,
        DXGI_FORMAT_BC6H_UF16           = 95,
        DXGI_FORMAT_BC7_UNORM           = 98,
        DXGI_FORMAT_BC7_UNORM_SRGB      = 99,
    };

    enum FLAGS {
        DDPF_ALPHAPIXELS = 0x000001,
        DDPF_ALPHA       = 0x000002,
        DDPF_FOURCC      = 0x000004,
        DDPF_RGB         = 0x000040,
        DDSCAPS2_CUBEMAP = 0x000200,
        DDPF_LUMINANCE   = 0x020000,
        DDSD_MIPMAPCOUNT = 0x020000,
    };

    struct Header {
        uint32 dwMagic;
        uint32 dwSize;
        uint32 dwFlags;
        uint32 dwHeight;
        uint32 dwWidth;
        uint32 dwPOLSize;
        uint32 dwDepth;
        uint32 dwMipMapCount;
        uint32 SomeData1[12];
        uint32 pfFlags;
        uint32 pfFourCC;
        uint32 pfRGBbpp;
        uint32 pfRMask;
        uint32 pfGMask;
        uint32 pfBMask;
        uint32 pfAMask;
        uint32 dwCaps1;
        uint32 dwCaps2;
        uint32 SomeData3[3];
    } header;

    struct {
        uint32 format;
        uint32 dimension;
        uint32 flags1;
        uint32 slices;
        uint32 flags2;
    } header10;

    stream->read(&header, sizeof(Header));

    uint32 bpp = 8;

    if (header.pfFlags & DDPF_FOURCC) {
        switch (header.pfFourCC) {
        // Compressed
            case FOURCC_DXT1    : desc.format = Texture::FMT_BC1;     break;
            case FOURCC_DXT3    : desc.format = Texture::FMT_BC2;     break;
            case FOURCC_DXT5    : desc.format = Texture::FMT_BC3;     break;
            case FOURCC_BC4U    : desc.format = Texture::FMT_BC4;     break;
            case FOURCC_BC5U    : desc.format = Texture::FMT_BC5;     break;
            case FOURCC_ATI1    : desc.format = Texture::FMT_BC4;     break;
            case FOURCC_ATI2    : desc.format = Texture::FMT_BC5;     break;
            case FOURCC_R16F    : desc.format = Texture::FMT_R16F;    break;
            case FOURCC_RG16F   : desc.format = Texture::FMT_RG16F;   break;
            case FOURCC_RGBA16F : desc.format = Texture::FMT_RGBA16F; break;
            case FOURCC_R32F    : desc.format = Texture::FMT_R32F;    break;
            case FOURCC_RG32F   : desc.format = Texture::FMT_RG32F;   break;
            case FOURCC_RGBA32F : desc.format = Texture::FMT_RGBA32F; break;
            case FOURCC_DXGI : {
                stream->read(&header10, sizeof(header10));
                switch (header10.format) {
                    case DXGI_FORMAT_R32G32B32A32_FLOAT  : desc.format = Texture::FMT_RGBA32F;    break; 
                    case DXGI_FORMAT_R32G32B32_FLOAT     : desc.format = Texture::FMT_RGB32F;     break;
                    case DXGI_FORMAT_R16G16B16A16_FLOAT  : desc.format = Texture::FMT_RGBA16F;    break;
                    case DXGI_FORMAT_R32G32_FLOAT        : desc.format = Texture::FMT_RG32F;      break;
                    case DXGI_FORMAT_R11G11B10_FLOAT     : desc.format = Texture::FMT_R11G11B10F; break;
                    case DXGI_FORMAT_R8G8B8A8_UNORM      : desc.format = Texture::FMT_RGBA8;      break;
                    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : desc.format = Texture::FMT_RGBA8_SRGB; break;
                    case DXGI_FORMAT_R16G16_FLOAT        : desc.format = Texture::FMT_RG16F;      break;
                    case DXGI_FORMAT_R32_FLOAT           : desc.format = Texture::FMT_R32F;       break;
                    case DXGI_FORMAT_R8G8_UNORM          : desc.format = Texture::FMT_RG8;        break;
                    case DXGI_FORMAT_R16_FLOAT           : desc.format = Texture::FMT_R16F;       break;
                    case DXGI_FORMAT_R8_UNORM            : desc.format = Texture::FMT_R8;         break;
                    case DXGI_FORMAT_BC1_UNORM           : desc.format = Texture::FMT_BC1;        break;
                    case DXGI_FORMAT_BC1_UNORM_SRGB      : desc.format = Texture::FMT_BC1_SRGB;   break;
                    case DXGI_FORMAT_BC2_UNORM           : desc.format = Texture::FMT_BC2;        break;
                    case DXGI_FORMAT_BC2_UNORM_SRGB      : desc.format = Texture::FMT_BC2_SRGB;   break;
                    case DXGI_FORMAT_BC3_UNORM           : desc.format = Texture::FMT_BC3;        break;
                    case DXGI_FORMAT_BC3_UNORM_SRGB      : desc.format = Texture::FMT_BC3_SRGB;   break;
                    case DXGI_FORMAT_BC4_UNORM           : desc.format = Texture::FMT_BC4;        break;
                    case DXGI_FORMAT_BC5_UNORM           : desc.format = Texture::FMT_BC5;        break;
                    case DXGI_FORMAT_BC6H_UF16           : desc.format = Texture::FMT_BC6;        break;
                    case DXGI_FORMAT_BC7_UNORM           : desc.format = Texture::FMT_BC7;        break;
                    case DXGI_FORMAT_BC7_UNORM_SRGB      : desc.format = Texture::FMT_BC7_SRGB;   break;
                    default                              : ASSERT(false); return false;
                }
                break;
            }
            default : ASSERT(false); return false;
        }
    } else {
        switch (header.pfRGBbpp) {
            case 8  : desc.format = Texture::FMT_R8;    break;
            case 16 : desc.format = Texture::FMT_RG8;   break;
            case 32 : desc.format = Texture::FMT_RGBA8; break;
            default : ASSERT(false); return false;
        }
    }

    desc.width  = header.dwWidth;
    desc.height = header.dwHeight;
    desc.depth  = 1;
    desc.slices = 1;
    desc.levels = (header.dwFlags & DDSD_MIPMAPCOUNT) ? header.dwMipMapCount : 1;
    desc.flags  = (header.dwCaps2 & DDSCAPS2_CUBEMAP) ? Texture::FLAG_CUBEMAP : 0;

    int size = stream->size - stream->pos;
    desc.data = new char[size];
    stream->read(desc.data, size);

    return true;
}

#endif
