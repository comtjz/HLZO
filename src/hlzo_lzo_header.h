//
// Created by hongchao1 on 2018/4/3.
//

#ifndef HLZO_HLZO_LZO_HEADER_H
#define HLZO_HLZO_LZO_HEADER_H

#include <sys/types.h>

namespace HLZO {

    class HLZOLzofile;

    class HLZOLzoheader {
    public:
        u_int16_t version_;
        u_int16_t lib_version_;
        u_int16_t version_needed_to_extract_;
        u_char method_;
        u_char level_;
        u_int32_t flags_;
        u_int32_t filter_;
        u_int32_t mode_;
        u_int32_t mtime_low_;
        u_int32_t mtime_high_;
        u_int32_t header_checksum_;

        u_int32_t extra_field_len_;
        u_int32_t extra_field_checksum_;

        const char *method_name_;

        char name_[255 + 1];

        int p_header(HLZOLzofile *ft);
        int x_get_method();
    };

}

#endif //HLZO_HLZO_LZO_HEADER_H
