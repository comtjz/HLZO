//
// Created by hongchao1 on 2018/4/9.
//

#include <glog/logging.h>

#include "hlzo.h"
#include "hlzo_def.h"
#include "hlzo_hdfs.h"

namespace HLZO {

    HLZO::HLZO() : _hdfsFile(nullptr) {
    }

    HLZO::~HLZO() {
        if (_hdfsFile) {
            delete _hdfsFile;
            _hdfsFile = nullptr;
        }
    }

    void HLZO::addHdfsConf(const string& conf_file) {
        _conf.addResource(conf_file);
    }

    bool HLZO::connectHdfs() {
        /* 先检查一下配置 */
        bool ret = _hlzo_hdfs_connector.isEffectConf(_conf);
        if (!ret) {
            LOG(ERROR) << "wrong hdfs config";
            return false;
        }

        ret = _hlzo_hdfs_connector.initHdfsBuilder(_conf);
        if (!ret) {
            LOG(ERROR) << "hdfs builder fail";
            return false;
        }

        ret  = _hlzo_hdfs_connector.connectHdfs();

        return ret;
    }

    void HLZO::disconnectHdfs() {
        _hlzo_hdfs_connector.disconnectHdfs();
    }

    int HLZO::setReadHdfs(const string& hdfs_file, size_t start, size_t len) {
        _hdfs_file = hdfs_file;
        _start = start;
        _length = len;

        _cur_start  = _start;
        _cur_length = 0;

        if (_hdfsFile) {
            delete _hdfsFile;
            _hdfsFile = nullptr;
        }

        _hdfsFile = new HLZOHdfsFile();
        _hdfsFile->setHdfsFS(_hlzo_hdfs_connector.getHdfsFS());
        int ret = _hdfsFile->openFile(hdfs_file, start, len);
        if (ret != HLZO_OK) {
            LOG(ERROR) << "open file fail";
            return ret;
        }
		
		return HLZO_OK;
    }

    ssize_t HLZO::getNext(string& out) {
        if (_cur_length >= _length) {
            LOG(INFO) << "context over, length = " << _length;
            return 0;
        }


        ssize_t len = 0;
        if (_cur_start == 0) {
            if ((len = _hdfsFile->checkMagic()) < 0) {
                LOG(ERROR) << "hdfs file magic fail";
                return HLZO_ERR_LZO_MAGIC;
            }
            _cur_length += len;

            if ((len = _hdfsFile->checkHead()) < 0) {
                LOG(ERROR) << "hdfs file head fail";
                return HLZO_ERR_LZO_HEADER;
            }
            _cur_length += len;

            _cur_start += _cur_length;
        }

        len = _hdfsFile->readContext(out);
        if (len < 0) {
            LOG(ERROR) << "read context fail";
            return HLZO_ERR_LZO_CONTEXT;
        } else if (len == 0) {
            LOG(INFO) << "read context over, cur_length = " << _cur_length << ", length = " << _length;
            return 0;
        }
        _cur_length += len;
        LOG(INFO) << "length = " << _length;
        LOG(INFO) << "cur_length = " << _cur_length;
        LOG(INFO) << "cur offset = " << _hdfsFile->tell() - _start;
        return len;
    }

    vector<vector<LzoSplit>> HLZO::getLzoSplit(const string& path, int split_num) {
        vector<vector<LzoSplit>> splits;

        HLZOHdfs hdfs(&_hlzo_hdfs_connector);
        HLZOHdfsFileInfo fileinfo = hdfs.getHdfsFileInfo(path);
        if (!fileinfo._flag) {
            LOG(ERROR) << "get file info fail, path:" << path;
            return splits;
        }

        HLZOLzoIndex lzoIndex(&_hlzo_hdfs_connector);

        if (fileinfo._kind == HLZOObjectKind::KindFile) {
            LOG(INFO) << "split from lzo file, filename:" << path;

            string datafile = path;
            u_int64_t length = fileinfo._size;
            string indexfile = datafile + ".index";
            vector<LzoSplit> split = lzoIndex.getSplits(indexfile, length, split_num);
            if (split.empty()) {
                LOG(ERROR) << "split file fail, filename:" << indexfile;
            } else {
                splits.push_back(split);
            }
            return splits;
        }

        LOG(INFO) << "split from lzo data directory";

        vector<string> ls = hdfs.listHdfsDirectory(path);
        if (ls.empty()) {
            LOG(ERROR) << "empty directory, directory = " << path;
        } else {
            string indexfile;
            string datafile;
            for (int i = 0; i < ls.size(); ++i) {
                indexfile = ls[i];

                string::size_type pos;
                if ((pos = indexfile.find(".lzo.index")) != string::npos) {
                    datafile = indexfile.substr(0, pos+4);
                } else {
                    continue;
                }

                u_int64_t length = hdfs.getFileLength(datafile);

                LOG(INFO) << "start split file, file:" << datafile;
                vector<LzoSplit> split = lzoIndex.getSplits(indexfile, length, split_num);
                if (split.empty()) {
                    LOG(ERROR) << "split file fail, filename:" << indexfile;
                }
                LOG(INFO) << "end split file, file:" << datafile;

                splits.push_back(split);
            }
        }

        return splits;
    }
}