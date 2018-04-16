//
// Created by hongchao1 on 2018/4/9.
//

#include <string.h>
#include <stdlib.h>

#include "hlzo_def.h"
#include "hlzo_lzo_block.h"

namespace HLZO {

    void HLZOLzoBlock::do_init(lzo_uint32 size, lzo_uint align) {
        memset(this, 0, sizeof(*this));
        mb_size = size;
        mb_align = (align > 1) ? align : 1;
        mb_adler32 = ADLER32_INIT_VALUE;
        mb_crc32 = CRC32_INIT_VALUE;
    }

    lzo_bool HLZOLzoBlock::mb_alloc(lzo_uint32 size, lzo_uint align) {
        do_init(size, align);
        if (mb_size == 0)
            return true;

        mb_size_alloc = mb_size + mb_align - 1;
        mb_mem_alloc = (lzo_bytep)malloc(mb_size_alloc);
        if (mb_mem_alloc == nullptr)
            return false;
        memset(mb_mem_alloc, 0, mb_size_alloc);

        mb_mem = LZO_PTR_ALIGN_UP(mb_mem_alloc, mb_align);
        return true;
    }

    void HLZOLzoBlock::mb_free() {
        free(mb_mem_alloc);
        memset(this, 0, sizeof(*this));
    }
}
