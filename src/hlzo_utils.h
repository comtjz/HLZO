//
// Created by hongchao1 on 2018/4/3.
//

#ifndef HLZO_HLZO_UTILS_H
#define HLZO_HLZO_UTILS_H

#include "hlzo_lzo_file.h"

namespace HLZO {

    ssize_t read_buf(int fd, void *vptr, size_t cnt);
    ssize_t write_buf(int fd, const void *vptr, size_t cnt);

    u_int16_t get_be16(const unsigned char *b);
    u_int32_t get_be32(const unsigned char *b);

    void read32(int fd, u_int32_t *v);
    int f_read8(HLZOLzofile *ft, unsigned char *b);
    void f_read16(HLZOLzofile *ft, u_int16_t *v);
    void f_read32(HLZOLzofile *ft, u_int32_t *v);
    ssize_t f_read(HLZOLzofile *ft, void *buf, size_t cnt);
}

#endif //HLZO_HLZO_UTILS_H
