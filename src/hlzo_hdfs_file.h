//
// Created by hongchao1 on 2018/4/8.
//

#ifndef HLZO_HLZO_HDFS_FILE_H
#define HLZO_HLZO_HDFS_FILE_H

#include <string>

using std::string;

namespace HLZO {
    class HLZOHdfsFile {
    public:
        HLZOHdfsFile();
        virtual ~HLZOHdfsFile();

        void setfs(hdfsFS fs) { _fs = fs; };

        int openHdfsFile(const string& file, size_t pos, size_t length);
        void closeHdfsFile();

        bool read_check_head();

    private:
        ssize_t read_buf(void *vptr, size_t cnt);
        void read32(u_int32_t *v);
        int f_read8(HLZOLzofile *ft, unsigned char *b);
        void f_read16(u_int16_t *v);
        void f_read32(HLZOLzofile *ft, u_int32_t *v);
        ssize_t f_read(HLZOLzofile *ft, void *buf, size_t cnt);

    private:
        hdfsFS   _fs;
        hdfsFile _readFile;

        u_int32_t    f_adler32_;
        u_int32_t    f_crc32_;
    };
}

#endif //HLZO_HLZO_HDFS_FILE_H
