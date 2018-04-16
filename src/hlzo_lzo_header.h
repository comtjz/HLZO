//
// Created by hongchao1 on 2018/4/3.
//

#ifndef HLZO_HLZO_LZO_HEADER_H
#define HLZO_HLZO_LZO_HEADER_H

#include <sys/types.h>

#include "hlzo_file.h"

namespace HLZO {

    class HLZOLzoHeader {
    public:
        u_int16_t _version;
        u_int16_t _lib_version;
        u_int16_t _version_needed_to_extract;

        u_char _method;
        u_char _level;

        u_int32_t _flags;
        u_int32_t _filter;
        u_int32_t _mode;
        u_int32_t _mtime_low;
        u_int32_t _mtime_high;
        u_int32_t _header_checksum;

        u_int32_t _extra_field_len;
        u_int32_t _extra_field_checksum;

        const char *_method_name;

        char _name[255 + 1];

        ssize_t parse_header(HLZOFile *reafFile);
        int x_get_method();
    };
}

#endif //HLZO_HLZO_LZO_HEADER_H
