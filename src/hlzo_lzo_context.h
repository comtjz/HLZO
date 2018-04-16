//
// Created by hongchao1 on 2018/4/9.
//

#ifndef HLZO_HLZO_LZO_CONTEXT_H
#define HLZO_HLZO_LZO_CONTEXT_H

#include <string>

#include "hlzo_lzo_header.h"
#include "hlzo_file.h"
#include "hlzo_lzo_block.h"
#include "hlzo_def.h"

using std::string;

namespace HLZO {

    class HLZOLzoContext {
    public:
        HLZOLzoContext() {
            _lzo_block1.mb_alloc(MAX_COMPRESSED_SIZE(block_size), 0);
            _lzo_block2.mb_alloc(MAX_COMPRESSED_SIZE(block_size), 0);
        }
        ~HLZOLzoContext() {
            _lzo_block2.mb_free();
            _lzo_block1.mb_free();
        }

        ssize_t parse_context(HLZOLzoHeader *lzoheader, HLZOFile *reafFile, string& out);

    private:
        HLZOLzoBlock _lzo_block1;
        HLZOLzoBlock _lzo_block2;
    };
}

#endif //HLZO_HLZO_LZO_CONTEXT_H
