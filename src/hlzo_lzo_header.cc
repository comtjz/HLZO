//
// Created by hongchao1 on 2018/4/3.
//

#include <glog/logging.h>

#include "hlzo_def.h"
#include "hlzo_lzo_header.h"

namespace HLZO {

    ssize_t HLZOLzoHeader::parse_header(HLZOFile* ft) {
        int r;
        int l;
        u_int32_t checksum;
        ssize_t length = 0;

        memset(this, 0, sizeof(*this));
        _version_needed_to_extract = 0x0900; /* first public lzop version */
        _level = 0;
        _method_name = "unknown";

        ft->_f_adler32 = ADLER32_INIT_VALUE;
        ft->_f_crc32 = CRC32_INIT_VALUE;

        ft->f_read16(&_version);
        length += 2;
        LOG(INFO) << "version = " << std::hex << _version;
        if (_version < 0x0900)
            return HLZO_ERROR;
        ft->f_read16(&_lib_version);
        length += 2;
        LOG(INFO) << "lib_version = " << std::hex << _lib_version;
        if (_version >= 0x0940) {
            ft->f_read16(&_version_needed_to_extract);
            length += 2;
            LOG(INFO) << "version_needed_to_extract = " << std::hex << _version_needed_to_extract;
            if (_version_needed_to_extract > LZOP_VERSION)
                return HLZO_ERROR;
            if (_version_needed_to_extract < 0x0900)
                return HLZO_ERROR;
        }
        ft->f_read8(&_method);
        length += 1;
        LOG(INFO) << "method = " << (int)_method;
        if (_version >= 0x0940) {
            ft->f_read8(&_level);
            length += 1;
            LOG(INFO) << "level = " << (int)_level;
        }
        ft->f_read32(&_flags);
        length += 4;
        LOG(INFO) << "flags = " << std::hex << _flags;
        if (_flags & F_H_FILTER) {
            ft->f_read32(&_filter);
            length += 4;
            LOG(INFO) << "filter = " << std::hex << _filter;
        }
        ft->f_read32(&_mode);
        length += 4;
        LOG(INFO) << "mode = " << _mode;
        if (_flags & F_STDIN)
            _mode = 0;

        ft->f_read32(&_mtime_low);
        length += 4;
        LOG(INFO) << "mtime_low = " << _mtime_low;
        if (_version >= 0x0940) {
            ft->f_read32(&_mtime_high);
            length += 4;
            LOG(INFO) << "mtime_high = " << _mtime_high;
        }
        if (_version < 0x0120) {
            if (_mtime_low == 0xffffffffUL)
                _mtime_low = 0;
            _mtime_high = 0;
        }

        l = ft->f_read8(NULL);
        length += 1;
        LOG(INFO) << "l = " << l;
        if (l > 0) {
            char name[255 + 1];
            if (ft->f_read(name, l) != l) {
                return HLZO_ERROR;
            }
            length += l;
            name[l] = 0;
            // TODO
        }

        checksum = (_flags & F_H_CRC32) ? ft->_f_crc32 : ft->_f_adler32;
        ft->f_read32(&_header_checksum);
        length += 4;
        if (_header_checksum != checksum)
            return HLZO_ERROR;

        if (_method <= 0)
            return HLZO_ERROR;

        r = x_get_method();
        if (r != 0)
            return HLZO_ERROR;

        if (_flags & F_RESERVED)
            return HLZO_ERROR;

        if (_flags & F_H_EXTRA_FIELD) {
            u_int32_t k;

            ft->_f_adler32 = ADLER32_INIT_VALUE;
            ft->_f_crc32 = CRC32_INIT_VALUE;
            ft->f_read32(&_extra_field_len);
            length += 4;
            for (k = 0; k < _extra_field_len; ++k) {
                (void) ft->f_read8(NULL);
                length += 1;
            }
            checksum = (_flags & F_H_CRC32) ? ft->_f_crc32 : ft->_f_adler32;
            ft->f_read32(&_extra_field_checksum);
            length += 4;
            if (_extra_field_checksum != checksum)
                return HLZO_ERROR;
        }

        return length;
    }

    int HLZOLzoHeader::x_get_method() {
        int r = -1;

        if (_method == M_LZO1X_1) {
            _method_name = "LZO1X-1";
            if (_level == 0)
                _level = 3;
        } else if (_method == M_LZO1X_1_15) {
            _method_name = "LZO1X_1(15)";
            if (_level == 0)
                _level = 1;
        } else if (_method == M_LZO1X_999) {
            static char s[11+1] = "LZO1X-999  ";
            s[9] = 0;
            if (_level == 0)
                _level = 9;
            else if (_version >= 0x0950 && _version >= 0x1020) {
                s[9] = '/';
                s[10] = (char)(_level + '0');
            }
            _method_name = s;
        } else {
            return -1;
        }

        if (_level < 1 || _level > 9)
            return 15;

        return 0;
    }

}