#include <iostream>
#include <cassert>
#include <zlib.h>
#include "MnistLoader.hpp"

#define CHUNK 163840

static int InfFile(FILE *source, FILE *dest)
{
    int ret;
    size_t have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    ret = inflateInit2(&strm, (16+MAX_WBITS));
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

         /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;     /* and fall through */
                case Z_DATA_ERROR:
                    return ret;
                case Z_MEM_ERROR:
                    (void)inflateEnd(&strm);
                    return ret;
                case Z_ERRNO:
                    return ret;
                case Z_STREAM_ERROR:
                    return ret;
                case Z_BUF_ERROR:
                    return ret;
                case Z_VERSION_ERROR:
                    return ret;
            }
            have = CHUNK - strm.avail_out;

            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }

        } while (strm.avail_out == 0);
    }  while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

static void InfMemory(FILE *source, unsigned char** memptr, size_t *memsize)
{
    int ret;
    unsigned char* ptr = nullptr;
    size_t length = 0;
    size_t have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    ret = inflateInit2(&strm, (16+MAX_WBITS));
    if (ret != Z_OK)
        return;

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);

        if (ferror(source)) {
            (void)inflateEnd(&strm);
            if (ptr)
                free(ptr);
            return;
        }

        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

         /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);

            switch (ret) {
                case Z_NEED_DICT:
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                case Z_ERRNO:
                case Z_STREAM_ERROR:
                case Z_BUF_ERROR:
                case Z_VERSION_ERROR:
                    (void)inflateEnd(&strm);
                    if (ptr)
                        free(ptr);
                    return;
            }

            have = CHUNK - strm.avail_out;

            ptr = (unsigned char*)realloc(ptr, length+have);
            memcpy(&ptr[length], out, have);
            length += have;

        } while (strm.avail_out == 0);
    }  while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    
    *memptr = ptr;
    *memsize = length;
}

int MnistToFile(const char* srcFileName, const char* destFileName)
{
    FILE* source = std::fopen(srcFileName, "rb");
    FILE* dest = std::fopen(destFileName, "w");

    assert(source && dest);
    int ret = InfFile(source, dest);
    fclose(source);
    fclose(dest);
    
    return ret;
}

void MnistToMemory(const char* srcFileName, unsigned char** ptr, size_t* length)
{
    FILE* source = std::fopen(srcFileName, "rb");
    assert(source && ptr && length);

    *ptr = nullptr;
    *length = 0;

    InfMemory(source, ptr, length);
    fclose(source);
    
}

