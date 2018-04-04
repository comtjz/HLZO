//
// Created by hongchao1 on 2018/4/2.
//

#ifndef HLZO_HLZO_LZO_FILE_H
#define HLZO_HLZO_LZO_FILE_H

#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#include "hlzo_def.h"
//#include "hlzo_lzo_header.h"

namespace HLZO {
    class HLZOLzofile {
    public:
        int          fd_;
        int          open_flags_;
        struct stat  st_;
        u_int32_t    f_adler32_;
        u_int32_t    f_crc32_;

        //int          opt_name_;
        u_int64_t    part_;
        u_int64_t    bytes_read_;
        u_int64_t    bytes_written_;
        bool         warn_multipart_;
        bool         warn_unknown_suffix_;

        u_int64_t    bytes_processed_;

#define FILE_T_NAME_LEN    ( 2 * ((PATH_MAX)+1) + SUFFIX_MAX + 1)
        char name_[FILE_T_NAME_LEN];

        HLZOLzofile() {
            memset(this, 0, sizeof(*this));
            fd_ = -1;
        }

        // 打开、关闭文件
        bool open_fi(const std::string& in_name);
        bool open_fo(const std::string& out_name, bool force);
        void f_close();

        // 正确magic返回0,结束返回-1,错误返回对应正值
        int magic();

    private:
        int check_magic(const unsigned char *magic);
    };


}

#if 0
struct HLZOLzofile {


public:
    bool open_file(const std::string& filename);

private:
    void reset();

    bool magic();
    int check_magic(const unsigned char *magic);

    bool p_header(HLZOLzoheader *h);

public:

    //static const unsigned char lzop_magic[9];

    int                fd_;    // 文件描述符
    int                open_flags_;
    struct stat        st_;
    unsigned int       f_adler32_;
    unsigned int       f_crc32_;
    int                opt_name_;
    unsigned long      part_;
    //std::atomic_ulong  bytes_read_;
    u_int64_t          bytes_read_;
    //std::atomic_ulong  bytes_written_;
    u_int64_t          bytes_written_;
    bool               warn_multipart_;
    bool               warn_unknown_suffix_;

    //std::atomic_ulong  bytes_processed;
    u_int64_t          bytes_processed;

#define FILE_T_NAME_LEN  ( 2*((PATH_MAX)+1) + SUFFIX_MAX + 1)
    char name_[FILE_T_NAME_LEN];
};
#endif
#endif //HLZO_HLZO_LZO_FILE_H
