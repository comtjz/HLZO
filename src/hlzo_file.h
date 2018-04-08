//
// Created by hongchao1 on 2018/4/8.
//

#ifndef HLZO_HLZO_FILE_H
#define HLZO_HLZO_FILE_H

namespace HLZO {
    class HLZOFile {
    public:
        HLZOFile();

        virtual ~HLZOFile();

        virtual ssize_t read_buf(void *vptr, size_t cnt);

        void read32(u_int32_t *v);
        int f_read8(unsigned char *b);
        void f_read16(u_int16_t *v);
        void f_read32(u_int32_t *v);
        ssize_t f_read(void *buf, size_t cnt);

    public:
        u_int32_t    _f_adler32;
        u_int32_t    _f_crc32;
    };
}

#endif //HLZO_HLZO_FILE_H
