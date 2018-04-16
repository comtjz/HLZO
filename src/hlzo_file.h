//
// Created by hongchao1 on 2018/4/9.
//

#ifndef HLZO_HLZOFILE_H
#define HLZO_HLZOFILE_H

#include <string>

using std::string;

namespace HLZO {

    class HLZOFile {
    public:
        HLZOFile() {};
        virtual ~HLZOFile() {};

        virtual int openFile(const string& file, size_t pos, size_t length) = 0;
        virtual void closeFile() = 0;

        virtual ssize_t tell() = 0;

        virtual ssize_t checkMagic() = 0;
        virtual ssize_t checkHead() = 0;
        virtual ssize_t readContext(string& out) = 0;

        virtual ssize_t read_buf(void *vptr, size_t cnt) = 0;

        void read32(u_int32_t *v);
        int f_read8(unsigned char *b);
        void f_read16(u_int16_t *v);
        void f_read32(u_int32_t *v);
        ssize_t f_read(void *buf, size_t cnt);

    public:
        u_int32_t _f_adler32;
        u_int32_t _f_crc32;
    };
}

#endif //HLZO_HLZOFILE_H
