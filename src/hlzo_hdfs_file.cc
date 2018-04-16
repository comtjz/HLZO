//
// Created by hongchao1 on 2018/4/8.
//

#include <glog/logging.h>

#include "hdfs.h"
#include "hlzo_def.h"
#include "hlzo_hdfs_file.h"

namespace HLZO {

    HLZOHdfsFile::HLZOHdfsFile() : HLZOFile(), _fs(nullptr), _readFile(nullptr) {}

    HLZOHdfsFile::~HLZOHdfsFile() {
        if (_readFile) {
            closeFile();
        }
    }

    int HLZOHdfsFile::openFile(const string& file, size_t pos, size_t length) {
        LOG(INFO) << "HLZO open hdfs file";
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
                return HLZO_ERR_HDFS_KIND;
            }

            if (pos > fileInfo->mSize || (pos + length) > fileInfo->mSize) {
                LOG(ERROR) << "wrong hdfs file length";
                return HLZO_ERR_HDFS_LENGTH;
            }

            hdfsFreeFileInfo(fileInfo, 1);

            /* 打开hdfs文件 */
            _readFile = hdfsOpenFile(_fs, file.c_str(), O_RDONLY, 0, 0, 0);
            if (!_readFile) {
                LOG(ERROR) << "open hdfs file fail";
                return HLZO_ERR_OPEN;
            }

            /* 检查文件magic */
            ssize_t len = checkMagic();
            if (len < 0) {
                LOG(ERROR) << "lzo file wrong magic";
                return HLZO_ERR_LZO_MAGIC;
            }

            /* 读取头部 */
            len = checkHead();
            if (len < 0) {
                LOG(ERROR) << "lzo file wrong header";
                return HLZO_ERR_LZO_HEADER;
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

    void HLZOHdfsFile::closeFile() {
        LOG(INFO) << "HLZO close hdfs file";

        if (_readFile) {
            hdfsCloseFile(_fs, _readFile);
            _readFile = nullptr;
        }
    }

    ssize_t HLZOHdfsFile::tell() {
        LOG(INFO) << "HLZO tell current offset in the file, in bytes";

        if (_readFile) {
            return hdfsTell(_fs, _readFile);
        }

        return -1;
    }

    ssize_t HLZOHdfsFile::checkMagic() {
        if (!_readFile) {
            LOG(ERROR) << "please open file";
        }

        LOG(INFO) << "check lzo magic";
        int r;
        ssize_t l;
        unsigned char magic[sizeof(lzop_magic)];

        l = read_buf(magic, sizeof(magic));
        if (l != (ssize_t)sizeof(magic))
            return HLZO_ERROR;

        if (memcmp(magic, lzop_magic, sizeof(lzop_magic)) == 0)
            return l;

        return HLZO_ERROR;
    }

    ssize_t HLZOHdfsFile::checkHead() {
        if (!_readFile) {
            LOG(INFO) << "Please open file";
            return false;
        }

        LOG(INFO) << "start read head";
        return _lzoHeader.parse_header(this);
    }

    ssize_t HLZOHdfsFile::readContext(string& out) {
        if (!_readFile) {
            LOG(ERROR) << "Please open file";
            return false;
        }

        LOG(INFO) << "start read context";
        return _lzoContext.parse_context(&_lzoHeader, this, out);
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
}