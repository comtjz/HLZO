//
// Created by hongchao1 on 2018/4/3.
//
#include <glog/logging.h>
#include <errno.h>

#include "hlzo_def.h"
#include "hlzo_utils.h"
#include "hlzo_lzo_header.h"

namespace HLZO {
    int HLZOLzoheader::p_header(HLZOLzofile *ft) {
        int r;
        int l;
        u_int32_t checksum;

        memset(this, 0, sizeof(*this));
        version_needed_to_extract_ = 0x0900;  /* first public lzop version */
        level_ = 0;
        method_name_ = "unknown";

        ft->f_adler32_ = ADLER32_INIT_VALUE;
        ft->f_crc32_ = CRC32_INIT_VALUE;

        f_read16(ft, &version_);
        LOG(INFO) << std::hex << version_;
        if (version_ < 0x0900)
            return 3;
        f_read16(ft, &lib_version_);
        LOG(INFO) << std::hex << lib_version_;
        if (version_ >= 0x0900) {
            f_read16(ft, &version_needed_to_extract_);
            LOG(INFO) << std::hex << version_needed_to_extract_;
            if (version_needed_to_extract_ > LZOP_VERSION)
                return 16;
            if (version_needed_to_extract_ < 0x0900)
                return 3;
        }
        f_read8(ft, &method_);
        LOG(INFO) << std::hex << (int)method_;
        if (version_ >= 0x0940) {
            f_read8(ft, &level_);
            LOG(INFO) << std::hex << (int)level_;
        }
        f_read32(ft, &flags_);
        LOG(INFO) << std::hex << flags_;
        if (flags_ & F_H_FILTER)
            f_read32(ft, &filter_);
        f_read32(ft, &mode_);
        LOG(INFO) << std::hex << "mode " << mode_;
        if (flags_ & F_STDIN)
            mode_ = 0;
        f_read32(ft, &mtime_low_);
        LOG(INFO) << std::hex << mtime_low_;
        if (version_ >= 0x0940) {
            f_read32(ft, &mtime_high_);
            LOG(INFO) << std::hex << mtime_high_;
        }
        if (version_ < 0x0120) {
            if (mtime_low_ == 0xffffffffUL)
                mtime_low_ = 0;
            mtime_high_ = 0;
        }

        l = f_read8(ft, NULL);
        LOG(INFO) << "l = " << l;
        if (l > 0) {
            char name[255+1];
            if (f_read(ft, name, l) != l) {
                LOG(ERROR) << "read error, " << strerror(errno);
                return -1;
            }
            name[l] = 0;
            LOG(INFO) << name;
            // TODO
        }

        checksum = (flags_ & F_H_CRC32) ? ft->f_crc32_ : ft->f_adler32_;
        LOG(INFO) << std::hex << checksum;
        f_read32(ft, &header_checksum_);
        LOG(INFO) << std::hex << header_checksum_;
        if (header_checksum_ != checksum)
            return 2;

        if (method_ <= 0)
            return 14;

        r = x_get_method();
        if (r != 0) {
            LOG(ERROR) << "get method fail, ret = " << r;
            return r;
        }

        if (flags_ & F_RESERVED)
            return 13;

        if (flags_ & F_H_EXTRA_FIELD) {
            u_int32_t k;

            ft->f_adler32_ = ADLER32_INIT_VALUE;
            ft->f_crc32_ = CRC32_INIT_VALUE;
            f_read32(ft, &extra_field_len_);
            for (k = 0; k < extra_field_len_; k++)
                (void) f_read8(ft, NULL);
            checksum = (flags_ & F_H_CRC32) ? ft->f_crc32_ : ft->f_adler32_;
            f_read32(ft, &extra_field_checksum_);
            if (extra_field_checksum_ != checksum)
                return 3;
        }

        return 0;
    }

    int HLZOLzoheader::x_get_method() {
        int r = -1;

        if (method_ == M_LZO1X_1) {
            method_name_ = "LZO1X-1";
            if (level_ == 0)
                level_ = 3;
        } else if (method_ == M_LZO1X_1_15) {
            method_name_ = "LZO1X_1(15)";
            if (level_ == 0)
                level_ = 1;
        } else if (method_ == M_LZO1X_999) {
            static char s[11+1] = "LZO1X-999  ";
            s[9] = 0;
            if (level_ == 0)
                level_ = 9;
            else if (version_ >= 0x0950 && version_ >= 0x1020) {
                s[9] = '/';
                s[10] = (char)(level_ + '0');
            }
            method_name_ = s;
        } else {
            return -1;
        }

        if (level_ < 1 || level_ > 9)
            return 15;

        return 0;
    }
}