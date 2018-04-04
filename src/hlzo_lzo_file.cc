//
// Created by hongchao1 on 2018/4/2.
//

#include <glog/logging.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "hlzo_lzo_file.h"
#include "hlzo_utils.h"

namespace HLZO {

    bool HLZOLzofile::open_fi(const std::string &in_name) {
        int r;
        if (fd_ != -1) {
            LOG(ERROR) << "lzo file no duplicate open";
            return false;
        }

        if (in_name.length() >= PATH_MAX) {
            LOG(ERROR) << "filename too long, name:" << in_name;
            return false;
        }

        strcpy(name_, in_name.c_str());

        r = stat(name_, &st_);
        if (r != 0)
            memset(&st_, 0, sizeof(st_));
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
            LOG(ERROR) << "lzo file no duplicate open";
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
                LOG(ERROR) << strerror(errno);
                LOG(ERROR) << "can't open input file";
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
        if (part_ > 0 && l <= 0)
            return -1;
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

}

#if 0
bool HLZOLzofile::open_file(const std::string& filename) {
    if (fd_ != -1) {
        LOG(ERROR) << "lzo file fd is exist";
        return false;
    }

    reset();
    if (filename.length() >= PATH_MAX) {
        LOG(ERROR) << "name too long";
        return false;
    }

    strcpy(name_, name);

    /* open file */
    errno = 0;
    int r = stat(name_, &st_);
    if (r != 0)
        memset(&st_, 0, sizeof(st_));

    if (r == 0 && !S_ISREG(st_.st_mode)) {
        LOG(WARN) << filename << " not a regular file -- skipped";
        return false;
    }

    open_flags_ = O_RDONLY;

    if (r)
        fd_ = open(name_, open_flags_, 0);

    if (fd_ >= 0 && (fd_ == STDIN_FILENO || fd_ == STDOUT_FILENO || fd_ == STDERR_FILENO)) {
        LOG(ERROR) << filename << " sanity check failed";
        fd_ = -1;
    }

    if (fd_ < 0) {
        LOG(ERROR) << filename << " can't open input file";
        return false;
    }

    return true;
}

void HLZOLzofile::reset() {
    opt_name_            = -1;
    part_                = 0;
    bytes_read_          = 0;
    bytes_written_       = 0;
    warn_multipart_      = false;
    warn_unknown_suffix_ = false;
}

bool HLZOLzofile::magic() {
    int r;
    int l;
    unsigned char magic[sizeof(lzop_magic)];

    l = read_buf(magic, sizeof(magic));
    if (part_ > 0 && l <= 0)
        return false;

    if (l == (int)sizeof(magic))
        r = check_magic(magic);
    else
        r = 1;

    if (part_ > 0 && r == 1) {

    }

    if (r != 0) {
        LOG(ERROR) << name_ << " check magic fail";
    }

    return r;
}


int HLZOLzofile::read_header(HLZOLzoheader *h) {
    int r;
    int l;
    unsigned int checksum;

    memset(h, 0 sizeof(HLZOLzoheader));
    h->version_need_to_extract = 0x0900; /* first public lzop version */
    h->level = 0;
    h->method_name = "unknown";

    f_adler32_ = ADLER32_INIT_VALUE;
    f_crc32_ = CRC32_INIT_VALUE;

    f_read16(&h->version);
    if (h->version < 0x0900)
        return 3;
    f_read16(&h->lib_version);
    if (h->version >= 0x0940) {
        f_read16(&h->version_needed_to_extract);
        if (h->version_needed_to_extract > LZOP_VERSION)
            return 16;
        if (h->version_needed_to_extract < 0x0900)
            return 3;
    }
    f_read8(&h->method);
    if (h->version >= 0x0940)
        f_read8(&h->level);
    f_read32(&h->flags);
    if (h->flags & F_H_FILTER)
        f_read32(&h->filter);
    f_read32(&h->mode);

    if (h->flags & F_STDIN)
        h->mode = 0;

    f_read32(&h->mtime_low);
    if (h->version >= 0x0940)
        f_read32(&h->mtime_high);
    if (h->version < 0x0120) {
        if (h->mtime_low == 0xffffffffUL)
            h->mtime_low = 0;
        h->mtime_high = 0;
    }

    l = f_read8(ft, NULL);
    if (l > 0) {
        char name[255+1];
        if (f_read(name, l) != l)
            throw std::exception{};
        name[l] = 0;
        // TODO
    }

    checksum = (h->flags & F_H_CRC32) ? f_crc32_ : f_adler32_;
    f_read32(&h->header_checksum);
    if (h->header_checksum != checksum)
        return 2;

    if (h->method <= 0)
        return 14;
    r = x_get_method(h);
    if (r != 0)
        return r;

    return 0;
}

bool HLZOLzoheader::p_header(HLZOLzoheader *h) {
    int r;

    r = read_header(h);
    if (r == 0)
        return 1;
    if (r < 0) {
        r = -r;

    }
}
#endif
