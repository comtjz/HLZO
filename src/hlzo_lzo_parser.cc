//
// Created by hongchao1 on 2018/4/2.
//

#include <glog/logging.h>

#include <lzo/lzo1x.h>
#include <sys/types.h>

#include "hlzo_utils.h"
#include "hlzo_lzo_parser.h"
#include "hlzo_lzo_header.h"

namespace HLZO {

    bool HLZOLzoparser::decompressFile(const std::string &filename) {
        if (!fi_.open_fi(filename)) {
            LOG(ERROR) << "fail to open file";
            return false;
        }

        /* 解析文件名,根据文件名判断lzo文件中是否有magic和header */
        u_int32_t start = 0, length = 0;
        //fi_.get_start_and_length(start, length);

        bool ok = true;
        int r;

            r = fi_.magic();
            if (r > 0)
                return false;
            if (r < 0)
                return false;

            HLZOLzoheader lzoheader;
            r = lzoheader.p_header(&fi_);
            if (r != HLZO_OK) {
                LOG(ERROR) << "lzo header fail, ret = " << r;
                return false;
            }

            fo_.open_fo("./data/out_test", true);
            r = x_decompress(&fi_, &fo_, &lzoheader);


        return true;
    }

    int HLZOLzoparser::x_decompress(HLZOLzofile *ft, HLZOLzofile *fo, HLZOLzoheader *header) {
        int r;
        u_int32_t src_len, dst_len;
        u_int32_t c_adler32 = ADLER32_INIT_VALUE, d_adler32 = ADLER32_INIT_VALUE;
        u_int32_t c_crc32 = CRC32_INIT_VALUE, d_crc32 = CRC32_INIT_VALUE;
        mblock_t * const block = &blocks_[1];
        unsigned char *b1;
        lzo_bytep const b2 = block->mb_mem;
        for (;;) {
            unsigned char *dst;
            LOG(INFO) << "1111";
            f_read32(ft, &dst_len);
            LOG(INFO) << "dst_len = " << std::hex << dst_len;
            if (dst_len == 0)
                break;

            if (dst_len == 0xffffffffUL) {
                break;
            }

            if (dst_len > MAX_BLOCK_SIZE) {
                break;
            }

            read32(ft->fd_, &src_len);
            LOG(INFO) << "src_len = " << std::hex << src_len;
            if (src_len <= 0 || src_len > dst_len) {
                break;
            }

            if (dst_len > BLOCK_SIZE) {
                break;
            }

            if (dst_len > block_size) {
                break;
            }

            if (header->flags_ & F_ADLER32_D)
                read32(ft->fd_, &d_adler32);
            if (header->flags_ & F_CRC32_D)
                read32(ft->fd_, &d_crc32);

            if (header->flags_ & F_ADLER32_C) {
                if (src_len < dst_len)
                    read32(ft->fd_, &c_adler32);
                else {
                    c_adler32 = d_adler32;
                }
            }

            if (header->flags_ & F_CRC32_C) {
                if (src_len < dst_len)
                    read32(ft->fd_, &c_crc32);
                else {
                    c_crc32 = d_crc32;
                }
            }

            /* read the block */
            b1 = block->mb_mem + block->mb_size - src_len;
            if (read_buf(ft->fd_, b1, src_len) != src_len) {
                LOG(INFO) << "开玩笑";
                break;
            }

            ft->bytes_processed_ += src_len;

            u_long d = dst_len;
            LOG(INFO) << "start decompress";
            r = lzo1x_decompress(b1, src_len, b2, &d, NULL);
            dst = b2;
            LOG(INFO) << "d = " << std::hex << d;
            r = write_buf(fo->fd_, dst, dst_len);
            LOG(INFO) << "w = " << r;
        }
    }

    void HLZOLzoparser::do_init(mblock_t *m, u_int32_t size, u_int64_t align) {
        memset(m,0,sizeof(*m));
        m->mb_size = size;
        m->mb_align = (align > 1) ? align : 1;
        m->mb_adler32 = ADLER32_INIT_VALUE;
        m->mb_crc32 = CRC32_INIT_VALUE;
    }

    bool HLZOLzoparser::mb_alloc(mblock_t *m, u_int32_t size, u_int64_t align) {
        do_init(m, size, align);
        if (m->mb_size == 0)
            return true;

        m->mb_size_alloc = m->mb_size + m->mb_align - 1;
        m->mb_mem_alloc = (unsigned char *) malloc(m->mb_size_alloc);
        if (m->mb_mem_alloc == NULL)
            return false;
        memset(m->mb_mem_alloc, 0, m->mb_size_alloc);

        m->mb_mem = LZO_PTR_ALIGN_UP(m->mb_mem_alloc,m->mb_align);

        return true;
    }

    void HLZOLzoparser::mb_free(mblock_t *m) {
        free(m->mb_mem_alloc);
        memset(m, 0, sizeof(*m));
    }


#if 0
    bool HLZOLzoparser::parseLzoFile(const std::string &filename) {
        if (!fi_.f_open(filename)) {
            LOG(ERROR) << "parse lzo file stop, can't open file";
            return false;
        }

        int r;
        for (; ok; fi_.part++) {
            r = fi_.magic();
            if (r > 0)
                return false;

            if (r < 0)
                break;
        }
    }

    int HLZOLzoparser::x_decompress() {
        int ok = 0;

        if (hlzo_enter()) {
            LOG(ERROR) << "out of memory";
            return -1;
        }

        fi_.bytes_processed = fo_.bytes_processed = 0;

        switch (lzoHeader_.method) {
            case M_LZO1X_1:
            case M_LZO1X_1_15:
            case M_LZO1X_999:
                ok = decompress();
                break;
            default:
                LOG(ERROR) << "Internal error";
                ok = 0;
                break;
        }
    }

    int HLZOLzoparser::decompress() {
        int r;
        uint32_t src_len, dst_len;
        uint32_t c_adler32 = ADLER32_INIT_VALUE, d_adler32 = ADLER32_INIT_VALUE;
        uint32_t c_crc32 = CRC32_INIT_VALUE, d_crc32 = CRC32_INIT_VALUE;
        bool ok = true;
        mblock_t *const block = &blocks_[1];

        for (;;) {
            u_char *dst;

            /* read uncompressed block size */
            read32(&fi_, &dst_len);

            /* exit if last block */
            if (dst_len == 0)
                break;

            /* exit if last block */
            if (dst_len == 0xffffffffUL) {

            }

            if (dst_len > MAX_BLOCK_SIZE) {

            }

            /* read compressed block size */
            read32(&fi_, &src_len);
            if (src_len <= 0 || src_len > dst_len) {

            }

            if (dst_len > BLOCK_SIZE) {

            }

            if (dst_len > block_size) {

            }

            /* read checksum of uncompressed block */
            if (lzoHeader_.flags & F_ADLER32_D) {
                read32(&fi_, &d_adler32);
            }
            if (lzoHeader_.flags & F_CRC32_D) {
                read32(&fi_, &d_crc32);
            }

            /* read checksum of compressed block */
            if (h->flags & F_ADLER32_C) {

            }
            if (h->flags & F_F_CRC32_C) {

            }

            /* read the block */
            b1 = block->mb_memb + block->mb_size - src_len;
            if (read_buf(&fi, b1, src_len) != src_len) {

            }

            fi_.bytes_processed += src_len;

            if (src_len < dst_len) {
                u_int32_t d = dst_len;

                r = lzo1x_decompress(b1, src_len, b2, &d, NULL);

                if (r != LZO_E_OK || dst_len != d) {

                }

                dst = b2;
            } else {
                dst = b1;
            }

            /* write uncompressed blocks data */
            write_buf(&fo_, dst, dst_len);
            fo_.bytes_processed += dst_len;
        }

        return ok;
    }
#endif
}