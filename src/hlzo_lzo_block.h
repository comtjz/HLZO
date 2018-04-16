//
// Created by hongchao1 on 2018/4/9.
//

#ifndef HLZO_HLZO_LZO_BLOCK_H
#define HLZO_HLZO_LZO_BLOCK_H

#include "lzo/lzoconf.h"

namespace HLZO {

    struct HLZOLzoBlock {
        /* public */
        lzo_bytep       mb_mem;
        lzo_uint32      mb_size;
        /* private */
        lzo_bytep   mb_mem_alloc;
        lzo_uint32  mb_size_alloc;
        lzo_uint32  mb_align;
        /* the following fields are not yet used but may prove useful for
         * adding new algorithms */
        lzo_uint32  mb_flags;
        lzo_uint32  mb_id;
        lzo_uint32  mb_len;
        lzo_uint32  mb_adler32;
        lzo_uint32  mb_crc32;

        lzo_bool mb_alloc(lzo_uint32 size, lzo_uint align);
        void mb_free();

        void do_init(lzo_uint32 size, lzo_uint align);
    };
}

#endif //HLZO_HLZO_LZO_BLOCK_H
