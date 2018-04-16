//
// Created by hongchao1 on 2018/4/3.
//

#ifndef HLZO_HLZO_UTILS_H
#define HLZO_HLZO_UTILS_H

#include <sys/types.h>

namespace HLZO {

    u_int16_t get_be16(const unsigned char *b);
    u_int32_t get_be32(const unsigned char *b);

}
#endif //HLZO_HLZO_UTILS_H
