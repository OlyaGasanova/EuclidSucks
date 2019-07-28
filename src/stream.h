#ifndef H_STREAM
#define H_STREAM

#include "utils.h"

struct Stream {
    int pos, size;

    Stream() : pos(0), size(0) {}
    virtual ~Stream() {}
    virtual void setPos(int newPos) {}
    virtual void read(void *data, int size) {}
    virtual void write(void *data, int size) {}
    
    void skip(int size) {
        setPos(pos + size);
    }
    
    void readStr(char *buffer) {
        uint8 length;
        read(&length, sizeof(length));
        read(buffer, length);
        buffer[length] = '\0';
    }
};


struct FileStream : Stream {

    enum Mode {
        MODE_READ,
        MODE__WRITE
    };

    FILE *f;

    FileStream(const char *name, Mode mode) : Stream() {
        f = fopen(name, (mode == MODE_READ) ? "rb" : "wb");
        if (!f) {
            ASSERT(false);
            return;
        }
        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);
    }

    virtual ~FileStream() {
        if (f) {
            fclose(f);
        }
    }

    virtual void setPos(int newPos) override {
        fseek(f, newPos, SEEK_SET);
        pos = newPos;
    }

    virtual void read(void *data, int size) override {
        fread(data, size, 1, f);
        pos += size;
    }

    virtual void write(void *data, int size) override {
        fwrite(data, size, 1, f);
        pos += size;
    }

};


struct MemoryStream : Stream {
    uint8* data;

    MemoryStream() : Stream(), data(NULL) {}

    MemoryStream(uint8* data, uint32 size) : Stream(), data(data) {
        this->size = size;
    }

    virtual void setPos(int newPos) override {
        pos = newPos;
    }

    virtual void read(void *data, int size) override {
        memcpy(data, this->data + pos, size);
        pos += size;
    }

    virtual void write(void *data, int size) override {
        memcpy(this->data + pos, data, size);
        pos += size;
    }

};

#endif
