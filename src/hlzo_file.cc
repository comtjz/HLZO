//
// Created by hongchao1 on 2018/4/9.
//

#include <glog/logging.h>

#include "lzo/lzo1.h"
#include "hlzo_file.h"
#include "hlzo_utils.h"

namespace HLZO {
    void HLZOFile::read32(u_int32_t *v) {
        unsigned char b[4];
        if (read_buf(b, 4) != 4) {
            LOG(ERROR) << "read32 fail, error:" << strerror(errno);
            throw "Read Fail";
        }

        *v = get_be32(b);
    }

    int HLZOFile::f_read8(unsigned char *b) {
        unsigned char bb;
        if (read_buf(&bb, 1) != 1) {
            LOG(ERROR) << "f_read8 fail, error:" << strerror(errno);
            throw "Read Fail";
        }
        _f_adler32 = lzo_adler32(_f_adler32, &bb, 1);
        _f_crc32 = lzo_crc32(_f_crc32, &bb, 1);
        if (b)
            *b = bb;
        return bb;
    }

    void HLZOFile::f_read16(u_int16_t *v) {
        unsigned char b[2];
        if (read_buf(b, 2) != 2) {
            LOG(ERROR) << "f_read16 fail, error:" << strerror(errno);
            throw "Read fail";
        }
        _f_adler32 = lzo_adler32(_f_adler32, b, 2);
        _f_crc32 = lzo_crc32(_f_crc32, b, 2);
        *v = get_be16(b);
    }

    void HLZOFile::f_read32(u_int32_t *v) {
        unsigned char b[4];
        if (read_buf(b, 4) != 4) {
            LOG(ERROR) << "f_read32 fail, error:" << strerror(errno);
            throw "Read fail";
        }
        _f_adler32 = lzo_adler32(_f_adler32, b, 4);
        _f_crc32 = lzo_crc32(_f_crc32, b, 4);
        *v = get_be32(b);
    }

    ssize_t HLZOFile::f_read(void *buf, size_t cnt) {
        cnt = read_buf(buf, cnt);
        if (cnt > 0) {
            _f_adler32 = lzo_adler32(_f_adler32, (const unsigned char *)buf, cnt);
            _f_crc32 = lzo_crc32(_f_crc32, (const unsigned char *)buf, cnt);
        }
        return cnt;
    }
}