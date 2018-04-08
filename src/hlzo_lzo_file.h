//
// Created by hongchao1 on 2018/4/2.
//

#ifndef HLZO_HLZO_LZO_FILE_H
#define HLZO_HLZO_LZO_FILE_H

#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#include "hlzo_def.h"

namespace HLZO {
    class HLZOLzofile {
    public:
        int          fd_;
        int          open_flags_;
        struct stat  st_;

        u_int32_t    f_adler32_;
        u_int32_t    f_crc32_;

        u_int64_t    bytes_read_;
        u_int64_t    bytes_written_;

        u_int64_t    bytes_processed_;

        char name_[FILE_NAME_LEN];

        HLZOLzofile() {
            memset(this, 0, sizeof(*this));
            fd_ = -1;
        }

        ~HLZOLzofile() {
            f_close();
        }

        // 打开、关闭文件
        bool open_fi(const std::string& in_name);
        bool open_fo(const std::string& out_name, bool force = true);
        void f_close();

        // 正确magic返回0,结束返回-1,错误返回对应正值
        int magic();

        // 根据文件名得到起始点和长度
        void get_start_and_length(u_int32_t &start, u_int32_t &length);

    private:
        int check_magic(const unsigned char *magic);
    };
}

#endif //HLZO_HLZO_LZO_FILE_H
