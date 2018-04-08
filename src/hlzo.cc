//
// Created by hongchao1 on 2018/4/8.
//

#include "hlzo.h"

namespace HLZO {

    HLZO::HLZO() {
        _hlzo_hdfs_connector.setParser(&_hlzo_lzo_parser);
    }

    HLZO::~HLZO() {
    }

    bool HLZO::connectHdfs(const string& hdfs_config_file) {
        if (hdfs_config_file.empty()) {
            LOG(ERROR) << "empty hdfs config";
            return false;
        }

        bool ret = _hlzo_hdfs_connector.initHdfsBuilderByFile(hdfs_config_file);
        if (!ret) {
            LOG(ERROR) << "wrong hdfs config";
            return false;
        }

        ret  = _hlzo_hdfs_connector.connectHdfs();

        return ret;
    }

    void HLZO::disconnectHdfs() {
        _hlzo_hdfs_connector.disconnectHdfs();
    }

    void HLZO::setReadHdfs(string hdfs_file, u_int32_t start, u_int32_t len) {
        std::lock_guard<std::mutex> locker(_lock);
        if (!_hdfs_file.empty()) {
            /* 当前实例可能已连接hdfs文件,需要关闭这些 */
            _hlzo_hdfs_connector.closeHdfsFile();
        }
        _hdfs_file = hdfs_file;
        _start = start;
        _length = len;

        _cur_start = start;
        _cur_length = 0;
    }

    int64_t HLZO::getNext(string& out) {
        int ret = 0;
        if (_cur_start == 0) {
            /* 文件头,lzo文件有个头部,需要读取头部 */
            ret = _hlzo_hdfs_connector.readHead();
            if (ret <= 0) {
                return ret;
            }
        }

        _cur_start  += ret;
        _cur_length += ret;

        if (_cur_length >= _length) {
            return 0;
        }

        int64_t ret = _hlzo_hdfs_connector.readData(out);
        return ret;
    }
}