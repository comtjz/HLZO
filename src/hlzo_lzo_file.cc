//
// Created by hongchao1 on 2018/4/2.
//

#include <glog/logging.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <vector>

#include "hlzo_lzo_file.h"
#include "hlzo_utils.h"

namespace HLZO {

    bool HLZOLzofile::open_fi(const std::string &in_name) {
        int r;
        if (fd_ != -1) {
            LOG(ERROR) << "fi can't open again";
            return false;
        }

        if (in_name.length() >= FILE_NAME_LEN) {
            LOG(ERROR) << "filename too long, name:" << in_name;
            return false;
        }

        strcpy(name_, in_name.c_str());

        r = stat(name_, &st_);
        if (r != 0) {
            LOG(WARNING) << "file can stat";
            memset(&st_, 0, sizeof(st_));
        }
        if (r == 0 && !S_ISREG(st_.st_mode)) {
            LOG(WARNING) << "not a regular file";
            return false;
        }
        open_flags_ = O_RDONLY;

        /* 打开文件 */
        fd_ = open(name_, open_flags_, 0);
        if (fd_ < 0) {
            LOG(ERROR) << "can't open input file";
            return false;
        }

        return true;
    }

    bool HLZOLzofile::open_fo(const std::string &out_name, bool force) {
        if (fd_ != -1) {
            LOG(ERROR) << "fo can't open again";
            return false;
        }

        if (out_name.length() >= FILE_NAME_LEN) {
            LOG(ERROR) << "filename too long, name:" << out_name;
            return false;
        }
        strcpy(name_, out_name.c_str());

        open_flags_ = O_CREAT | O_WRONLY;
        if (force)
            open_flags_ |= O_TRUNC;
        else
            open_flags_ |= O_EXCL;

        /* 打开文件 */
        fd_ = open(name_, open_flags_, 0644);
        if (fd_ < 0) {
            if ((open_flags_ & O_EXCL) && errno == EEXIST)
                LOG(ERROR) << "already exists; not overwritten";
            else {
                LOG(ERROR) << "can't open input file, error:" << strerror(errno);
            }
            return false;
        }

        return true;
    }

    void HLZOLzofile::f_close() {
        if (fd_ < 0)
            return;
        close(fd_);
        fd_ = -1;
        return;
    }

    int HLZOLzofile::magic() {
        int r;
        u_int32_t l;
        unsigned char magic[sizeof(lzop_magic)];

        l = read_buf(fd_, magic, sizeof(magic));
        if (l == (u_int32_t)sizeof(magic))
            r = check_magic(magic);
        else
            r = 1;

        if (r != 0) {
            LOG(ERROR) << "wrong lzo magic, reason:" << r;
        }
        return r;
    }

    int HLZOLzofile::check_magic(const unsigned char *magic) {
        if (memcmp(magic, lzop_magic, sizeof(lzop_magic)) == 0)
            return 0;

        /* We have a bad magic signature. Try to figure what possibly
         * could have gone wrong. */

        /* look at bytes 1-3: "LZO" in hex and local text format */
        if (memcmp((void *)&magic[1], (const void *)&lzop_magic[1], 3) != 0 &&
            memcmp((void *)&magic[1], (const void *)"LZO", 3) != 0)
            return 1;

        /* look at byte 4 */
        if (magic[4] != lzop_magic[4])
            return 1;

        void *m;
        /* look at bytes 5-8 */
        m = (void *)&magic[5];
        if (memcmp(m, "\012\012\032", 3) == 0)
            return 7;
        if (memcmp(m, "\012\012", 2) == 0)
            return 4;
        if (memcmp(m, "\012\032", 2) == 0)
            return 4;
        if (memcmp(m, "\015\012\012", 3) == 0)
            return 10;
        if (memcmp(m, "\015\012\032\012", 4) == 0)
            return 9;
        if (memcmp(m, "\015\012\032\015", 4) == 0)
            return 8;
        if (memcmp(m, "\015\015\012\032", 4) == 0)
            return 8;
        if (memcmp(m, "\015\015\032", 3) == 0)
            return 6;
        if (memcmp(m, "\015\032", 2) == 0)
            return 5;
        if (memcmp(m, &lzop_magic[5], 4) != 0)
            return 12;

        /* look at byte 0 */
        if (magic[0] == (unsigned char)(lzop_magic[0] & 0x7f))
            return 11;
        if (magic[0] != lzop_magic[0])
            return 12;

        return 3;
    }

    void HLZOLzofile::get_start_and_length(u_int32_t &start, u_int32_t &length) {
        std::vector<std::string> ret;
        std::string delim = "/";
        split(std::string(name_), delim, ret);
        LOG(INFO) << "filename = " << ret[ret.size() - 1];

        delim = "_";
        std::vector<std::string> ret1;
        split(ret[ret.size()-1], delim, ret1);
    }
}

