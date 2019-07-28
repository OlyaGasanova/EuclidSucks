#ifndef H_TGA
#define H_TGA

#include "utils.h"
#include "context.h"

// only 32-bit non-RLE
bool loadTGA(Stream *stream, Texture::Desc &desc) {
    struct Header {
        uint8   idlength;
        uint8   colourmaptype;
        uint8   datatypecode;
        uint8   colourmaporigin[2];
        uint8   colourmaplength[2];
        uint8   colourmapdepth;
        uint16  x_origin;
        uint16  y_origin;
        uint16  width;
        uint16  height;
        uint8   bitsperpixel;
        uint8   imagedescriptor;
    } header;

    ASSERT(sizeof(header) == 18);
    stream->read(&header, sizeof(header));
    stream->skip(header.idlength);

    desc.flags   = 0;
    desc.width   = header.width;
    desc.height  = header.height;
    desc.depth   = 1;
    desc.slices  = 1;
    desc.levels  = 1;
    desc.format  = Texture::RGBA8;
    desc.data    = new uint8[desc.width * desc.height * 4];

    stream->read(desc.data, desc.width * desc.height * 4);

    // BGRA -> RGBA
    for (int i = 0; i < desc.width * desc.height; i++) {
        Color32 &c = ((Color32*)desc.data)[i];
        swap(c.r, c.b);
    }

    return true;
}

#endif
