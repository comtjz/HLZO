//
// Created by hongchao1 on 2018/4/3.
//

#include <errno.h>
#include <sys/types.h>
#include <glog/logging.h>

#include "lzo/lzo1.h"

#include "hlzo_utils.h"

namespace HLZO {

    ssize_t read_buf(int fd, void *vptr, size_t cnt) {
        size_t nleft = cnt;
        char *ptr = (char *)vptr;
        ssize_t nread;

        while (nleft > 0) {
            if ((nread = read(fd, ptr, nleft)) < 0) {
                if (errno == EINTR)
                    nread = 0;
                else
                    return -1;
            } else if (nread == 0) {
                LOG(ERROR) << "nread = 0";
                break;
            }

            nleft -= nread;
            ptr += nread;
        }

        return (cnt - nleft);
    }

    ssize_t write_buf(int fd, const void *vptr, size_t cnt) {
        size_t  nleft = cnt;
        const char *ptr = (const char *)vptr;
        ssize_t nwritten;

        while (nleft > 0) {
            if ((nwritten = write(fd, ptr, nleft)) <= 0) {
                if (nwritten < 0 && errno == EINTR)
                    nwritten = 0;
                else {
                    LOG(ERROR) << strerror(errno);
                    return -1;
                }

            }

            nleft -= nwritten;
            ptr += nwritten;
        }

        return cnt;
    }

    void read32(int fd, u_int32_t *v) {
        unsigned char b[4];
        if (read_buf(fd, b, 4) != 4) {
            LOG(ERROR) << "read32 fail, error:" << strerror(errno);
            throw "Internal Wrong";
        }
        *v = get_be32(b);
    }

    u_int16_t get_be16(const unsigned char *b) {
        u_int16_t v;

        v  = (u_int16_t) b[1] << 0;
        v |= (u_int16_t) b[0] << 8;
        return v;
    }

    u_int32_t get_be32(const unsigned char *b) {
        u_int32_t v;

        v  = (u_int32_t) b[3] <<  0;
        v |= (u_int32_t) b[2] <<  8;
        v |= (u_int32_t) b[1] << 16;
        v |= (u_int32_t) b[0] << 24;
        return v;
    }

    int f_read8(HLZOLzofile *ft, unsigned char *b) {
        unsigned char bb;
        if (read_buf(ft->fd_, &bb, 1) != 1) {
            LOG(ERROR) << "f_read8 fail, error:" << strerror(errno);
            throw "Internal Wrong";
        }
        ft->f_adler32_ = lzo_adler32(ft->f_adler32_, &bb, 1);
        ft->f_crc32_ = lzo_crc32(ft->f_crc32_, &bb, 1);
        if (b)
            *b = bb;
        return bb;
    }

    void f_read16(HLZOLzofile *ft, u_int16_t *v) {
        unsigned char b[2];
        if (read_buf(ft->fd_, b, 2) != 2) {
            LOG(ERROR) << "f_read16 fail, error:" << strerror(errno);
            throw "Internal Wrong";
        }
        ft->f_adler32_ = lzo_adler32(ft->f_adler32_, b, 2);
        ft->f_crc32_ = lzo_crc32(ft->f_crc32_, b, 2);
        *v = get_be16(b);
    }

    void f_read32(HLZOLzofile *ft, u_int32_t *v) {
        unsigned char b[4];
        if (read_buf(ft->fd_, b, 4) != 4) {
            LOG(ERROR) << "f_read32 fail, error:" << strerror(errno);
            throw "Internal Wrong";
        }
        ft->f_adler32_ = lzo_adler32(ft->f_adler32_, b, 4);
        ft->f_crc32_ = lzo_crc32(ft->f_crc32_, b, 4);
        *v = get_be32(b);
    }

    ssize_t f_read(HLZOLzofile *ft, void *buf, size_t cnt) {
        cnt = read_buf(ft->fd_, buf, cnt);
        if (cnt > 0) {
            ft->f_adler32_ = lzo_adler32(ft->f_adler32_, (const unsigned char *)buf, cnt);
            ft->f_crc32_ = lzo_crc32(ft->f_crc32_, (const unsigned char *)buf, cnt);
        }
        return cnt;
    }
}
