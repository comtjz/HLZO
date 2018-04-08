//
// Created by hongchao1 on 2018/4/8.
//

#include <errno.h>

#include "hlzo_def.h"
#include "hlzo_utils.h"
#include "hlzo_hdfs_file.h"

namespace HLZO {
    HLZOHdfsFile::HLZOHdfsFile() : _fs(nullptr), _readFile(nullptr) {
    }

    HLZOHdfsFile::~HLZOHdfsFile() {
        if (_readFile) {
            closeHdfsFile();
        }
    }

    int HLZOHdfsFile::openHdfsFile(const string& file, size_t pos, size_t length) {
        if (_fs == nullptr) {
            LOG(ERROR) << "first hdfs connect";
            return HLZO_ERROR;
        }

        if (_readFile == NULL) {
            /* 判断文件是否存在 */
            int exist = hdfsExists(_fs, file.c_str());
            if (exist != HLZO_OK) {
                LOG(ERROR) << "hdfs file no exist";
                return HLZO_ERR_PARAM;
            }

            /* 获取文件信息 */
            hdfsFileInfo *fileInfo = hdfsGetPathInfo(_fs, file.c_str());
            if (!fileInfo) {
                LOG(ERROR) << "get hdfs file info failed";
                return HLZO_ERR_OPEN;
            }

            /* 检查是否是文件 */
            if (fileInfo->mKind == kObjectKindDirectory) {
                LOG(ERROR) << "hdfs directory";
                return HLZO_ERR_PARAM;
            }

            if (pos > fileInfo->mSize || (pos + length) > fileInfo->mSize) {
                LOG(ERROR) << "wrong hdfs file length";
                return HLZO_ERR_PARAM;
            }

            hdfsFreeFileInfo(fileInfo, 1);

            /* 打开hdfs文件 */
            _readFile = hdfsOpenFile(_fs, file.c_str(), O_RDONLY, 0, 0, 0);
            if (!_readFile) {
                LOG(ERROR) << "open hdfs file fail";
                return HLZO_ERR_OPEN;
            }

            /* 调整偏移 */
            int ret = hdfsSeek(_fs, _readFile, pos);
            if (ret != HLZO_OK) {
                LOG(ERROR) << "seek to given offset fail";
                return HLZO_ERR_SYSTEM;
            }
        }

        return HLZO_OK;
    }

    void HLZOHdfsFile::closeHdfsFile() {

    }

    bool HLZOHdfsFile::read_check_head() {
        HLZOLzoHeader header;
    }

    ssize_t HLZOHdfsFile::read_buf(void *vptr, size_t cnt) {
        size_t nleft = cnt;
        char *ptr = (char *)vptr;
        ssize_t nread;

        while (nleft > 0) {
            if ((nread = hdfsRead(_fs, _readFile, ptr, nleft)) < 0) {
                if (errno == EINTR)
                    nread = 0;
                else
                    return -1;
            } else if (nread == 0) {
                break;
            }

            nleft -= nread;
            ptr += nread;
        }

        return (cnt - nleft);
    }

    void HLZOHdfsFile::read32(u_int32_t *v) {
        unsigned char b[4];
        if (read_buf(b, 4) != 4) {
            LOG(ERROR) << "read32 fail, error:" << strerror(errno);
            throw "Read Fail";
        }

        *v = get_be32(b);
    }

    int HLZOHdfsFile::f_read8(HLZOLzofile *ft, unsigned char *b) {
        unsigned char bb;
        if (read_buf(&bb, 1) != 1) {
            LOG(ERROR) << "f_read8 fail, error:" << strerror(errno);
            throw "Read Fail";
        }
        ft->f_adler32_ = lzo_adler32(ft->f_adler32_, &bb, 1);
        ft->f_crc32_ = lzo_crc32(ft->f_crc32_, &bb, 1);
        if (b)
            *b = bb;
        return bb;
    }

    void HLZOHdfsFile::f_read16(HLZOLzofile *ft, u_int16_t *v) {
        unsigned char b[2];
        if (read_buf(b, 2) != 2) {
            LOG(ERROR) << "f_read16 fail, error:" << strerror(errno);
            throw "Read fail";
        }
        ft->f_adler32_ = lzo_adler32(ft->f_adler32_, b, 2);
        ft->f_crc32_ = lzo_crc32(ft->f_crc32_, b, 2);
        *v = get_be16(b);
    }

    void HLZOHdfsFile::f_read32(HLZOLzofile *ft, u_int32_t *v) {
        unsigned char b[4];
        if (read_buf(b, 4) != 4) {
            LOG(ERROR) << "f_read32 fail, error:" << strerror(errno);
            throw "Read fail";
        }
        ft->f_adler32_ = lzo_adler32(ft->f_adler32_, b, 4);
        ft->f_crc32_ = lzo_crc32(ft->f_crc32_, b, 4);
        *v = get_be32(b);
    }

    ssize_t HLZOHdfsFile::f_read(HLZOLzofile *ft, void *buf, size_t cnt) {
        cnt = read_buf(ft->fd_, buf, cnt);
        if (cnt > 0) {
            ft->f_adler32_ = lzo_adler32(ft->f_adler32_, (const unsigned char *)buf, cnt);
            ft->f_crc32_ = lzo_crc32(ft->f_crc32_, (const unsigned char *)buf, cnt);
        }
        return cnt;
    }

}