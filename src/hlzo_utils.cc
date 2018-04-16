//
// Created by hongchao1 on 2018/4/3.
//

#include "hlzo_utils.h"

namespace HLZO {

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

}
