//
// Created by hongchao1 on 2018/4/8.
//

#include <glog/logging.h>

#include "hlzo_def.h"
#include "hlzo_hdfs_connector.h"

namespace HLZO {
    HLZOHdfsConnector::HLZOHDfsConnector()  : _fs(NULL), _hdfsBuilder(NULL), _hdfsFile(NULL), _parser(NULL) {
    }

    HLZOHdfsConnector::~HLZOHdfsConnector() {
        _parser = nullptr;

        disconnectHdfs();
    }

    void HLZOHdfsConnector::setParser(HLZOLzoparser *parser) {
        _parser = parser;
    }

    bool HLZOHdfsConnector::initHdfsBuilderByFile(const string& conf_file) {
        LOG(ERROR) << "init hdfs builder by file";
        if (load_conf(conf_file) != HLZO_OK) {
            LOG(ERROR) << "load hdfs builder config fail";
            return false;
        }

        _hdfsBuilder = hdfsNewBuilder();
        if (_hdfsBuilder == NULL) {
            LOG(ERROR) << "new hdfs builder fail";
            return false;
        }

        map<string, string>::iterator iter = _hdfsBuilderConf.begin();
        while (iter != _hdfsBuilderConf.end()) {
            if (iter->first == "fs.default.name") {
                hdfsBuilderSetNameNode(hdfsBuilder_, iter->second.c_str());
            } else {
                int ret = hdfsBuilderConfSetStr(hdfsBuilder_, iter->first.c_str(), iter->second.c_str());
                if (ret != HLZO_OK) {
                    LOG(ERROR) << "hdfs builder set conf failed, key:" << iter->first << ", val:" << iter->second;
                    return false;
                }
            }
            ++iter;
        }

        return true;
    }

    bool HLZOHdfsConnector::connectHdfs() {
        LOG(INFO) << "start connect hdfs";
        if (hdfsBuilder_ == NULL) {
            LOG(ERROR) << "empty hdfs builder, can't connect";
            return false;
        }

        fs_ = hdfsBuilderConnect(hdfsBuilder_);
        /* The HDFS builder will be freeed, whether or not the connection was successful. */
        hdfsBuilder_ = NULL;
        if (!fs_) {
            LOG(ERROR) << "hdfs connect failed";
            return false;
        }

        return true;
    }

    void HLZOHdfsConnector::disconnectHdfs() {
        LOG(INFO) << "stop connect hdfs";
        if (hdfsBuilder_) {
            hdfsFreeBuilder(hdfsBuilder_);
            hdfsBuilder_ = NULL;
        }

        if (fs_) {
            hdfsDisconnect(fs_);
            fs_ = NULL;
        }

        return;
    }

    int64_t HLZOHdfsConnector::readHead() {
        HLZOLzoHeader  lzoheader;
        lzoheader.p_header(_hdfsFile);
    }

    int64_t HLZOHdfsConnector::readContext() {

    }

#if 0
    int64_t HLZOHdfsConnector::readData(const string& file, size_t pos, size_t length) {
        int64_t ret;
        if (_readFile == NULL) {
            if ((ret = openHdfsFile(file, pos, length)) != HLZO_OK) {
                LOG(ERROR) << "open hdfs file failed";
                return ret;
            }
        }

        /* 针对pos = 0的情况,需要特别处理,因为pos=0,为一个文件的开始,这些文件可能会在开始的位置放置头部信息。
         * 所以,当pos=0时,需要先读个头部信息() */
        if (pos == 0) {
            ret += _parser.parserHead();
            ret += _parser.parseContext();
        } else {
            ret += _parser.parseContext();
        }


    }
#endif

    int HLZOHdfsConnector::openHdfsFile(const string& file, size_t pos, size_t length) {
        _hdfsFile.setfs(_fs);
        _hdfsFile.openHdfsFile(file, pos, length);

        return HLZO_OK;
    }

    int HLZOHdfsConnector::load_conf(const string& conf_path) {
        if (conf_path.empty())
            return HLZO_ERR_PARAM;

        // open config file
        FILE *conf_file = fopen(conf_path.c_str(), "r");
        if (NULL == conf_file)
            return HLZO_ERR_OPEN;

        // read config file
        size_t line_num = 0;
        char line[HLZO_LINE_MAX_LEN] = {0};
        while (NULL != fgets(line, HLZO_LINE_MAX_LEN, conf_file)) {
            ++line_num;

            // empty line or comment line
            if ('\0' == line[0] || '#' == line[0] || '\n' == line[0])
                continue;

            // remove the LF at the end of line
            line[strlen(len) - 1] = '\0';

            // format error line
            char *delimiter = NULL;
            if (NULL == (delimiter = strchr(line, '='))) {
                LOG(ERROR) << "Error format of conf file! line:" << line_num;
                continue;
            }

            string key(line, delimiter - line);
            string value(delimiter + 1);

            //key = strtrim(key);
            //value = strtrim(value);
            if (key.empty() || value.empty()) {
                LOG(ERROR) << "Invalid conf of key! line_num:" << line_num <<", value:" << value;
                continue;
            }

            auto ret = hdfsBuilderConf_.insert(make_pair(key, value));
            if (!ret.second) {
                LOG(ERROR) << "Failed to put conf_map item:<" << key << ", " << value;
                continue;
            }
        }

        fclose(conf_file);
        conf_file = NULL;

        return HLZO_OK;
    }
}
