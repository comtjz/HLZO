//
// Created by hongchao1 on 2018/4/9.
//

#include <glog/logging.h>

#include "lzo/lzo1x.h"
#include "hlzo_def.h"
#include "hlzo_lzo_context.h"

namespace HLZO {

    ssize_t HLZOLzoContext::parse_context(HLZOLzoHeader *lzoheader, HLZOFile *ft, string& out) {
        u_int32_t src_len, dst_len;
        u_int32_t c_adler32 = ADLER32_INIT_VALUE, d_adler32 = ADLER32_INIT_VALUE;
        u_int32_t c_crc32 = CRC32_INIT_VALUE, d_crc32 = CRC32_INIT_VALUE;
        u_char *dst;
        ssize_t length = 0;

        /* read uncompressed block size */
        ft->read32(&dst_len);
        length += 4;
        LOG(INFO) << "dst_len = " << dst_len;

        /*  exit if last block */
        if (dst_len == 0) {
            LOG(INFO) << "lzo file, last block";
            return 0;
        }

        /* error if split file */
        if (dst_len == 0xffffffffUL) {
            /* 不应该有分隔 */
            return HLZO_ERR_LZO_SPLIT;
        }

        if (dst_len > _lzo_block2.mb_size) {
            LOG(ERROR) << "file corrupted, abnormal decompressed block size";
            return HLZO_ERR_LZO_CORRUPTED;
        }

        /* read compressed block size */
        ft->read32(&src_len);
        length += 4;
        LOG(INFO) << "src_len = " << src_len;
        if (src_len <= 0 || src_len > dst_len) {
            return 0;
        }

        // TODO
        if (src_len > _lzo_block1.mb_size) {
            LOG(ERROR) << "file corrupted, abnormal compressed block size";
            return HLZO_ERR_LZO_CORRUPTED;
        }

        /* read checksum of uncompressed block */
        if (lzoheader->_flags & F_ADLER32_D) {
            ft->read32(&d_adler32);
            length += 4;
        }
        if (lzoheader->_flags & F_CRC32_D) {
            ft->read32(&d_crc32);
            length += 4;
        }

        /* read checksum of compressed block */
        if (lzoheader->_flags & F_ADLER32_C) {
            if (src_len < dst_len) {
                ft->read32(&c_adler32);
                length += 4;
            } else {
                c_adler32 = d_adler32;
            }
        }

        if (lzoheader->_flags & F_CRC32_C) {
            if (src_len < dst_len) {
                ft->read32(&c_crc32);
                length += 4;
            } else {
                c_crc32 = d_crc32;
            }
        }

        u_char *b1 = _lzo_block1.mb_mem + _lzo_block1.mb_size - src_len;
        u_char *b2 = _lzo_block2.mb_mem;
        if (ft->read_buf(b1, src_len) != (lzo_int)src_len) {
            LOG(ERROR) << "read src compress data fail";
            return -1;
        }
        length += src_len;

        if (src_len < dst_len) {
            unsigned long d = dst_len;

            int r = lzo1x_decompress(b1, src_len, b2, &d, NULL);
            if (r != HLZO_OK || dst_len != d) {
                LOG(ERROR) << "decompress fail";
                return -1;
            }

            dst = b2;

        } else {
            dst = b1;
        }

        out = string((char *)dst, dst_len);

        LOG(INFO) << "length = " << length;
        return length;
    }
}