//
// Created by hongchao1 on 2018/4/2.
//

#ifndef HLZO_HLZO_LZO_PARSER_H
#define HLZO_HLZO_LZO_PARSER_H

#include <string>

#include "hlzo_lzo_file.h"

namespace HLZO {

    class HLZOLzoHeader;

    typedef struct {
        unsigned char *mb_mem;
        u_int32_t      mb_size;

        unsigned char *mb_mem_alloc;
        u_int32_t      mb_size_alloc;
        u_int32_t      mb_align;

        u_int32_t      mb_flags;
        u_int32_t      mb_id;
        u_int32_t      mb_len;
        u_int32_t      mb_adler32;
        u_int32_t      mb_crc32;
    } mblock_t;

    class HLZOLzoparser {
    public:
        HLZOLzoparser() : _readFile(nullptr) {
            int r = alloc_mem(0, MAX_COMPRESSED_SIZE(block_size), 0);
            if (!r)
                throw "memory leak";
        }

        ~HLZOLzoparser() {
            free_mem();
        }

        bool decompressFile(const std::string &filename);
        int x_decompress(HLZOLzofile *ft, HLZOLzofile *fo, HLZOLzoHeader *header);

        void setHdfsReadFile(hdfsFile readFile) {
            _readFile = readFile;
        }

    private:
        bool alloc_mem(u_int32_t s1, u_int32_t s2, u_int32_t w) {
            bool r = true;

            r &= mb_alloc(&blocks_[0], s1, ALIGN_SIZE);
            r &= mb_alloc(&blocks_[1], s2, ALIGN_SIZE);
            if (!r)
                free_mem();
            return r;
        }

        void free_mem() {
            mb_free(&blocks_[1]);
            mb_free(&blocks_[0]);
        }

        bool mb_alloc(mblock_t *m, u_int32_t size, u_int64_t align);

        void mb_free(mblock_t *m);

        void do_init(mblock_t *m, u_int32_t size, u_int64_t align);
    private:

        mblock_t blocks_[2];

        HLZOLzofile fi_;  // 输入文件
        HLZOLzofile fo_;  // 输出文件
    };

}
#endif //HLZO_HLZO_LZO_PARSER_H
